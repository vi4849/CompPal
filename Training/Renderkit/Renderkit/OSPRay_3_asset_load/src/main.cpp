
// ospray
#include "ospray/ospray.h"
#include "ospray/ospray_cpp.h"
#include "ospray/ospray_util.h"

// std
#include <experimental/filesystem>
#include <iostream>
#include <regex>
#include <vector>

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

// argh - the command line parser
#include "argh.h"

// tinyobjloader header
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

// GL Math library headers
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm_box3.h"

// utils
#include "utils.h"

// ################################################################################
// setting up to use the GL Math Library (GLM)
namespace ospray {
OSPTYPEFOR_SPECIALIZATION(glm::vec2, OSP_VEC2F);
OSPTYPEFOR_SPECIALIZATION(glm::ivec2, OSP_VEC2I);
OSPTYPEFOR_SPECIALIZATION(glm::vec3, OSP_VEC3F);
OSPTYPEFOR_SPECIALIZATION(glm::uvec3, OSP_VEC3UI);
OSPTYPEFOR_SPECIALIZATION(glm::vec4, OSP_VEC4F);
}  // namespace ospray

using namespace std;
using namespace ospray;
namespace fs = std::experimental::filesystem;

// ################################################################################
// This is this program's definition of the OBJData that the loader will parse
// the tinyobjloader library is used to parse the input OBJ/MTL files
struct OBJData {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
};

// ################################################################################
// wrapper around OBJ loader that will input the values into OSPRay
static void loadObj(const std::string &fileName, OSPWorld *_world,
                    glm_box3 *_bounds) {
  auto &world  = *_world;
  auto &bounds = *_bounds;

  // create OSPWorld-type variable named world
  world = ospNewWorld();

  // setup a bounding box that will be sized to the OBJ object size
  auto inf = std::numeric_limits<float>::infinity();
  bounds = glm_box3(glm::vec3(inf, inf, inf), glm::vec3(-inf, -inf, -inf));

  OBJData objdata;
  std::string warn;
  std::string err;

  fs::path objPath(fileName);

  // use the tinyObj loader to parse OBJ data
  auto retval = tinyobj::LoadObj(
      &objdata.attrib, &objdata.shapes, &objdata.materials, &warn, &err,
      objPath.c_str(), objPath.parent_path().c_str(), true, true);

  std::cerr << warn << std::endl;
  std::cerr << err  << std::endl;

  if (!retval) throw std::runtime_error("failed to open/parse obj file!");

  // create vertex buffers for the data in OBJ
  auto vertices = ospNewData(OSP_VEC3F, objdata.attrib.vertices.size() / 3);
  auto tmp = ospNewSharedData(objdata.attrib.vertices.data(), OSP_VEC3F,
                              objdata.attrib.vertices.size() / 3);
  ospCommit(tmp);
  ospCopyData(tmp, vertices);
//  ospRelease(tmp);
//  ospCommit(vertices);

  for (size_t i = 0; i < objdata.attrib.vertices.size() / 3; i += 3) {
    auto *vert = (glm::vec3 *)&objdata.attrib.vertices[i];
    bounds.extend(*vert);
  }

  // set up for the materials found in the OBJ
  std::vector<OSPMaterial> materials;
  auto defaultMaterial = ospNewMaterial("pathtracer", "obj");
  ospCommit(defaultMaterial);

  for (auto &mat : objdata.materials) {
    auto m = ospNewMaterial("pathtracer", "obj");

    ospSetParam(m, "kd", OSP_VEC3F, &mat.diffuse[0]);
    ospSetParam(m, "ks", OSP_VEC3F, &mat.specular[0]);
    ospSetParam(m, "ns", OSP_FLOAT, &mat.shininess);
    ospSetParam(m, "d" , OSP_FLOAT, &mat.dissolve);
    ospCommit(m);

    materials.push_back(m);
  }

  // set up to handle the geomtries found in the OBJ
  std::vector<OSPGeometricModel> meshes;

  for (auto &shape : objdata.shapes) {
    auto numSrcIndices = shape.mesh.indices.size();

    std::vector<glm::uvec3> vi;

    for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
      const auto i0 = shape.mesh.indices[i + 0].vertex_index;
      const auto i1 = shape.mesh.indices[i + 1].vertex_index;
      const auto i2 = shape.mesh.indices[i + 2].vertex_index;

      vi.emplace_back(i0, i1, i2);
    }

    auto geom = ospNewGeometry("mesh");
    ospSetParam(geom, "vertex.position", OSP_DATA, &vertices);
    utils::ospSetParamAsData1D(geom, "index", OSP_VEC3UI, vi.data(), vi.size());

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

  utils::ospSetParamAsData1D(group, "geometry", OSP_GEOMETRIC_MODEL,
                             meshes.data(), meshes.size());
  ospCommit(group);

  auto instance = ospNewInstance(group);
  ospCommit(instance);

  utils::ospSetParamAsData1D(world, "instance", OSP_INSTANCE, &instance, 1);

  auto light = ospNewLight("ambient");
  ospCommit(light);
  utils::ospSetParamAsData1D(world, "light", OSP_LIGHT, &light, 1);
  ospRelease(light);

  ospCommit(world);

  ospRelease(group);
  ospRelease(instance);
  for (auto &m : meshes) ospRelease(m);
  for (auto &m : materials) ospRelease(m);
  ospRelease(defaultMaterial);
}

// ################################################################################
// entry point to the program

int main(int argc, const char **argv) try {
  // initializing OSPRay
  utils::initializeOSPRay(argc, argv);

  // setting up the command line parser
  auto parser = argh::parser(argc, argv);
  parser.parse(argc, argv);

  /////////////////////  This is the command we are going to use  ///////////////////////
  //                                                                                   //
  //  ! ./run.sh --szObjFile=assets/chalet.obj --szEyePos=[1,1,1] --nNumIterations=50  //
  //                                                                                   //
  ///////////////////////////////////////////////////////////////////////////////////////

  // from command line setup of szNumIterations: number of frames to accumulate
  size_t nNumIterations;
  parser({"-nNumIterations","--nNumIterations"},10) >> nNumIterations;

  // from command line setup of szObjFile: object to load, default value assets/teapot.obj
  std::string szObjFile;
  parser({"-szObjFile", "--szObjFile"}, "assets/teapot.obj") >> szObjFile;

  // parsing szEyePos from command-line
  std::string szEyePos;
  parser({"-szEyePos", "--szEyePos"}, "") >> szEyePos;

  auto parsePosition = [&]() -> glm::vec3 {
    glm::vec3 oPos;

    std::string float_pattern("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?f?");
    std::regex float_regex(float_pattern, std::regex_constants::ECMAScript |
                                              std::regex_constants::icase);

    auto arg_begin =
        std::sregex_iterator(szEyePos.begin(), szEyePos.end(), float_regex);

    auto arg_end = std::sregex_iterator();

    size_t idx{0};
    for (std::sregex_iterator i = arg_begin; i != arg_end; ++i) {
      std::smatch match = *i;
      std::string match_str = match.str();

      oPos[idx++] = atof(match_str.c_str());
    }

    return oPos;
  };
  auto p = parsePosition();

  // Set up world with null (blank)
  OSPWorld world{nullptr};

  // setup a bounding box that will be sized to the OBJ object size
  glm_box3 bounds;

  ////////////////////////////////////////////////////////
  // call the function loadObj to load the OBJ file
  loadObj(szObjFile, &world, &bounds);
  ////////////////////////////////////////////////////////

  std::cout << "Parsed OBJ file successfully!" << std::endl;

  // choose pathtracer as renderer
  auto renderer = ospNewRenderer("pathtracer");
  float color[4] = {0.f, 1.f, 0.f, 1.f};

  // add the parameters to the OSPObject named renderer
  ospSetParam(renderer, "backgroundColor", OSP_VEC4F, color);
  ospCommit(renderer);

  // create the new frame buffer named framebuffer (s_x, s_y, format, color and channel)
  auto framebuffer =
      ospNewFrameBuffer(1024, 768, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);

  // set up camera
  // ********************* NOTE
  // NOTE: if you want to change the camera to position and lookat you can
  // change these
    
  glm::vec3 eyepos; // eyepos comes from the parser
  if (szEyePos.empty()) {
    eyepos = utils::centerCamera(bounds);
  } else {
    eyepos = p;
  }

  // defining where the point the camera will look-at
  glm::vec3 lookAtPoint;
  lookAtPoint = bounds.center();

  // defining the direction of the camera
  auto direction = glm::normalize(lookAtPoint - eyepos);

  float cam_up[] = {0.f, 1.f, 0.f};  // this means Y-up
  float cam_view[3];
  cam_view[0] = direction.x;
  cam_view[1] = direction.y;
  cam_view[2] = direction.z;

  float cam_pos[3];
  cam_pos[0] = eyepos.x;
  cam_pos[1] = eyepos.y;
  cam_pos[2] = eyepos.z;

  // create and setup camera
  auto camera = ospNewCamera("perspective");
  auto aspect = 1024 / (float)768;
  ospSetFloat(camera, "aspect", aspect);
  ospSetParam(camera, "position", OSP_VEC3F, cam_pos);
  ospSetParam(camera, "direction", OSP_VEC3F, cam_view);
  ospSetParam(camera, "up", OSP_VEC3F, cam_up);
  ospCommit(camera);  // commit each object to indicate modifications are done

  // with ospRenderFrameBlocking we make the render frame syncrhronous
  // for (int frames = 0; frames < nNumIterations; frames++)  // MODIFY THIS TO SHOW ACCUMULATION
      ospRenderFrameBlocking(framebuffer, renderer, camera, world);
    
  std::cout << "Rendered Successfully!" << std::endl;

  // access framebuffer and write its content as PNG file
  uint32_t *fb = (uint32_t *)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);
  utils::writePNG("asset.png", glm::ivec2(1024, 768), fb);
  ospUnmapFrameBuffer(fb, framebuffer);

  ospRelease(camera);
  ospRelease(framebuffer);
  ospRelease(world);

  ospShutdown();

  std::cout << "Done!" << std::endl;

  return 0;
} catch (std::exception &ex) {
  std::cerr << "Exception: " << ex.what() << std::endl;
}
