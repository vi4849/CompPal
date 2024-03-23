#define _GLIBCXX_USE_CXX11_ABI 0
// rkcommon
#define OSPRAY_CPP_RKCOMMON_TYPES 

// rkcommon
#include "rkcommon/math/rkmath.h"
#include "rkcommon/math/vec.h"
#include "rkcommon/utility/ParameterizedObject.h"
#include "rkcommon/containers/TransactionalBuffer.h"
#include "rkcommon/math/AffineSpace.h"

// ospray
#include "ospray/ospray_cpp.h"
#include "ArcballCamera.h"

// ospray_testing
#include "ospray/ospray_testing/ospray_testing.h"

// GL Math library headers
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm_box3.h"

// utils
#include "utils.h"

// std
#include <iostream>
#include <stdexcept>
#include <exception>
#include <vector>
#include <functional>
#include <string>

// argh - the command line parser 
#include "argh.h"

using namespace ospray;
using namespace rkcommon::math;
using namespace std;

// Vector of strings corresponding to the different scenes listed
static const std::vector<std::string> g_scenes = {
    "boxes",
    "cornell_box",
    "curves",
    "gravity_spheres_volume",
    "gravity_spheres_isosurface",
    "perlin_noise_volumes",
    "random_spheres",
    "streamlines",
    "subdivision_cube",
    "unstructured_volume",
    "planes",
    "clip_with_spheres",
    "clip_with_planes",
    "clip_gravity_spheres_volume",
    "clip_perlin_noise_volumes"
};
                
// ######################################################################
// Entry point to the program

int main(int argc, const char* argv[]) try {

    // setting up the command line parser: Inputs from the command line when the program ospExamples.cpp is called
    auto parser = argh::parser(argc, argv);
    parser.parse(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    
    // setting the szRenderer variable parsed from the command-line
    std::string szRenderer;
    parser({"-szRenderer","--szRenderer"}, "scivis") >> szRenderer;
    
    // setting the nScene variable parsed from the command-line
    int nScene;
    parser({ "-nScene", "--nScene"}, 0) >> nScene;
    
    // Full initialization of the OSPRay renderer/device
    utils::initializeOSPRay(argc, argv);
    
    // image size
    vec2i imgSize;
    imgSize.x = 1024; // width
    imgSize.y =  768; // height

    // OSPRay objects 
    cpp::Renderer renderer;
    cpp::Camera   camera{"perspective"};
    cpp::World    world;

    // Arcball camera instance
    std::unique_ptr<ArcballCamera> arcballCamera;

    vec4f backgroundColor { 0.25f, 0.25f, 0.25f, 1.f };

    // a running list of OSPRay objects to commit
    rkcommon::containers::TransactionalBuffer<OSPObject> objectsToCommit;

    auto builder = testing::newBuilder(g_scenes[nScene]);
    testing::setParam(builder, "rendererType", szRenderer);
    testing::commit(builder);

    world = testing::buildWorld(builder);
    testing::release(builder);

    world.commit();

    // creating the renderer object - either scivis or pathtracer
    renderer = cpp::Renderer(szRenderer.c_str());
    
    // retains a set background color on renderer change
    renderer.setParam("backgroundColor", backgroundColor);
    if(szRenderer == "sciviz") {
        renderer.setParam("aoSamples", 16);
        renderer.setParam("aoRadius", 100.f);
        renderer.setParam("aoIntensity", 1.f);
        renderer.setParam("volumeSamplingRate", 1.f);
    } else if(szRenderer == "pathtracer") {
        renderer.setParam("lightSamples",1);
        renderer.setParam("geometryLights",true);
        renderer.setParam("roulettePathLength",5);
        renderer.setParam("maxContribution", 10.f);
    }
    renderer.commit();

    // create the arcball camera model
    arcballCamera.reset(new ArcballCamera(world.getBounds<box3f>(), imgSize));
    arcballCamera->updateWindowSize(imgSize);
    arcballCamera->rotate(vec2f(0.f), vec2f(0.25f));
    
    camera.setParam("aspect",    imgSize.x / float(imgSize.y));
    camera.setParam("position",  arcballCamera->eyePos());
    camera.setParam("direction", arcballCamera->lookDir());
    camera.setParam("up",        arcballCamera->upDir());
    camera.commit();

    // create and setup framebuffer. The framebuffer holds the rendered 2D image.
    // with arguments as x and y size, format, color and channel of the picture
    // These are the picture formats allowed:
        // OSP_FB_NONE:    framebuffer will not be mapped by the application
        // OSP_FB_RGBA8:   bit [0–255] linear component red, green, blue, alpha
        // OSP_FB_SRGBA:   bit sRGB gamma encoded color components, and linear alpha
        // OSP_FB_RGBA32F: 32 bit float components red, green, blue, alpha
    ospray::cpp::FrameBuffer framebuffer(
        imgSize.x, imgSize.y, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
    framebuffer.clear();

    // render one frame
    framebuffer.renderFrame(renderer, camera, world);

    // access framebuffer and write its content as PNG file
    uint32_t *fb = (uint32_t *)framebuffer.map(false ? OSP_FB_ALBEDO : OSP_FB_COLOR);

    utils::writePNG("boxes.png", glm::ivec2(1024, 768), fb);
    
    framebuffer.unmap(fb);

    return 0;
} catch( std::exception &ex ) {
  std::cout << ex.what() << std::endl;
}
