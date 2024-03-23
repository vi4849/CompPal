#include "utils.h"
#include "glm_box3.h"

#include "ospray/ospray_cpp.h"
#include "ospray/ospray_util.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace utils
{

  // ############################################################
  // initializing the OSPRay device
  inline void initializeOSPRay(int argc, const char **argv,
                               bool errorsFatal)
  {
    // initialize OSPRay; OSPRay parses (and removes) its commandline parameters,
    // e.g. "--osp:debug"
    OSPError initError = ospInit(&argc, argv);

    if (initError != OSP_NO_ERROR)
      throw std::runtime_error("OSPRay not initialized correctly!");

    OSPDevice device = ospGetCurrentDevice();
    if (!device)
      throw std::runtime_error("OSPRay device could not be fetched!");

    // set an error callback to catch any OSPRay errors and exit the application
    if (errorsFatal)
    {
      ospDeviceSetErrorCallback(
          device,
          [](void *, OSPError error, const char *errorDetails) {
            std::cerr << "OSPRay error: " << errorDetails << std::endl;
            exit(error);
          },
          nullptr);
    }
    else
    {
      ospDeviceSetErrorCallback(
          device,
          [](void *, OSPError, const char *errorDetails) {
            std::cerr << "OSPRay error: " << errorDetails << std::endl;
          },
          nullptr);
    }

    ospDeviceSetStatusCallback(
        device, [](void *, const char *msg) { std::cout << msg; }, nullptr);

    bool warnAsErrors = true;
    auto logLevel = OSP_LOG_WARNING;

    ospDeviceSetParam(device, "warnAsError", OSP_BOOL, &warnAsErrors);
    ospDeviceSetParam(device, "logLevel", OSP_INT, &logLevel);

    ospDeviceCommit(device);
    ospDeviceRelease(device);
  }

  // ############################################################
  // writing a PPM file
  void writePPM(
      const char *fileName, const glm::ivec2 &size, const uint32_t *pixel)
  {
    FILE *file = fopen(fileName, "wb");
    if (file == nullptr)
    {
      fprintf(stderr, "fopen('%s', 'wb') failed: %d", fileName, errno);
      return;
    }
    fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
    unsigned char *out = (unsigned char *)alloca(3 * size.x);
    for (int y = 0; y < size.y; y++)
    {
      const unsigned char *in =
          (const unsigned char *)&pixel[(size.y - 1 - y) * size.x];
      for (int x = 0; x < size.x; x++)
      {
        out[3 * x + 0] = in[4 * x + 0];
        out[3 * x + 1] = in[4 * x + 1];
        out[3 * x + 2] = in[4 * x + 2];
      }
      fwrite(out, 3 * size.x, sizeof(char), file);
    }
    fprintf(file, "\n");
    fclose(file);
  }

  // ############################################################
  // set param data as a 1D structure
  void ospSetParamAsData1D(OSPObject target,
                           const char *n,
                           OSPDataType type,
                           const void *mem,
                           uint64_t numItems)
  {
    auto tmp = ospNewSharedData(mem, type, numItems);
    auto data = ospNewData(type, numItems);
    ospCopyData(tmp, data);
    ospSetParam(target, n, OSP_DATA, &data);
    ospRelease(tmp);
    ospRelease(data);
  }

  // ############################################################
  // load and OBJ/MTL file set
  static std::string pathOf(const std::string &filename)
{
#ifdef _WIN32
  const char path_sep = '\\';
#else
  const char path_sep = '/';
#endif

  size_t pos = filename.find_last_of(path_sep);
  if (pos == std::string::npos)
    return "";
  return filename.substr(0, pos + 1);
}

struct OBJData
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
};

  static void loadObj(const std::string &fileName,
    ospray::cpp::World *_world,
    glm_box3 *_bounds)
{
  auto &world = *_world;
  auto &bounds = *_bounds;

  world = ospray::cpp::World();

  auto inf = std::numeric_limits<float>::infinity();
  bounds = glm_box3(glm::vec3(inf, inf, inf), glm::vec3(-inf, -inf, -inf));

  OBJData objdata;
  std::string warn;
  std::string err;
  auto retval = tinyobj::LoadObj(
      &objdata.attrib, &objdata.shapes, &objdata.materials, &warn, &err,
      fileName.c_str(), pathOf(fileName).c_str(), true, true);

  std::cerr << warn << std::endl;

  std::cerr << err << std::endl;

  if (!retval)
    throw std::runtime_error("failed to open/parse obj file!");


  // create vertex buffers
  auto vertices = ospNewData(OSP_VEC3F, objdata.attrib.vertices.size() / 3);
  auto tmp = ospNewSharedData(objdata.attrib.vertices.data(),
                              OSP_VEC3F,
                              objdata.attrib.vertices.size() / 3);
  ospCommit(tmp);
  ospCopyData(tmp, vertices);
  ospRelease(tmp);
  ospCommit(vertices);

  for (size_t i = 0; i < objdata.attrib.vertices.size() / 3; i += 3)
  {
    auto *vert = (glm::vec3 *)&objdata.attrib.vertices[i];
    bounds.extend(*vert);
  }

  // materials
  std::vector<OSPMaterial> materials;
  auto defaultMaterial = ospNewMaterial("pathtracer", "obj");
  ospCommit(defaultMaterial);

  for (auto &mat : objdata.materials)
  {
    auto m = ospNewMaterial("pathtracer", "obj");

    ospSetParam(m, "kd", OSP_VEC3F, &mat.diffuse[0]);
    ospSetParam(m, "ks", OSP_VEC3F, &mat.specular[0]);
    ospSetParam(m, "ns", OSP_FLOAT, &mat.shininess);
    ospSetParam(m, "d", OSP_FLOAT, &mat.dissolve);
    ospCommit(m);

    materials.push_back(m);
  }

  std::vector<OSPGeometricModel> meshes;

  for (auto &shape : objdata.shapes)
  {
    auto numSrcIndices = shape.mesh.indices.size();

    std::vector<glm::uvec3> vi;

    for (size_t i = 0; i < shape.mesh.indices.size(); i += 3)
    {
      const auto i0 = shape.mesh.indices[i + 0].vertex_index;
      const auto i1 = shape.mesh.indices[i + 1].vertex_index;
      const auto i2 = shape.mesh.indices[i + 2].vertex_index;

      vi.emplace_back(i0, i1, i2);
    }

    auto geom = ospNewGeometry("mesh");
    ospSetParam(geom, "vertex.position", OSP_DATA, &vertices);
    ospSetParamAsData1D(geom, "index", OSP_VEC3UI, vi.data(), vi.size());

    ospCommit(geom);

    auto model = ospNewGeometricModel(geom);

    int matID = shape.mesh.material_ids[0];
    auto mat = matID < 0 ? defaultMaterial : materials[matID];
    ospSetParam(model, "material", OSP_MATERIAL, &mat);
    ospCommit(model);
    ospRelease(geom);
    meshes.push_back(model);
  }
    /////////////////////////////////////////////////////////////////////////////

    auto group = ospNewGroup();

    ospSetParamAsData1D(group,
      "geometry",
      OSP_GEOMETRIC_MODEL,
      meshes.data(),
      meshes.size());
    ospCommit(group);

    auto instance = ospNewInstance(group);
    ospCommit(instance);

    ospSetParamAsData1D(world, "instance", OSP_INSTANCE, &instance, 1);

    auto light = ospNewLight("ambient");
    ospCommit(light);
    ospSetParamAsData1D(world, "light", OSP_LIGHT, &light, 1);
    ospRelease(light);

    ospCommit(world);

    ospRelease(group);
    ospRelease(instance);
    for (auto &m : meshes)
      ospRelease(m);
    for (auto &m : materials)
      ospRelease(m);
    ospRelease(defaultMaterial);
}
}; // namespace utils