#pragma once

#include <cstdlib>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "rkcommon/math/rkmath.h"
#include "rkcommon/math/vec.h"
#include "rkcommon/utility/ParameterizedObject.h"
#include "rkcommon/containers/TransactionalBuffer.h"
#include "rkcommon/math/AffineSpace.h"

#include "ospray/ospray.h"

using namespace ospray;
using namespace rkcommon::math;
using namespace std;

namespace utils {
    
    inline void writePNG(const char *fileName, const glm::ivec2 &size, const uint32_t *pixel)
    {
      constexpr int nChannels{4};
      const int stride{nChannels * size.x};
      stbi_flip_vertically_on_write(true);
      stbi_write_png(fileName, size.x, size.y, nChannels, pixel, stride);
    }

    glm::vec3 centerCamera(glm_box3 &bounds) {
      auto diag = bounds.size();
      diag = max(diag, glm::vec3(0.1f * glm::length(diag)));
      auto centerTranslation = glm::translate(glm::mat4(1.0f), -bounds.center());
      auto translation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, glm::length(diag)));
      auto rot = glm::mat4(1.f);
      auto cam = translation * rot * centerTranslation; // translation * rot * centerTranslation;
      auto invCamera = glm::inverse(cam);
      return (invCamera * glm::vec4(0.f, 0.f, 0.f, 1.f));
    }
    
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
    
    inline void initializeOSPRay(
        int argc, const char **argv, bool errorsFatal = true)
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
      if (errorsFatal) {
        ospDeviceSetErrorCallback(
            device,
            [](void *, OSPError error, const char *errorDetails) {
              std::cerr << "OSPRay error: " << errorDetails << std::endl;
              exit(error);
            },
            nullptr);
      } else {
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
};