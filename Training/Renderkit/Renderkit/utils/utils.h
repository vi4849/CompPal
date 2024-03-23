#pragma once

#include "ospray/ospray.h"
#include "ospray/ospray_util.h"
#include "ospray/ospray_cpp.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace utils {
inline void initializeOSPRay(int argc, const char **argv,
                             bool errorsFatal);

void writePPM(
    const char *fileName, const glm::ivec2 &size, const uint32_t *pixel);

static void loadObj(const std::string &fileName,
    ospray::cpp::World *_world,
    glm_box3 *_bounds)             
};