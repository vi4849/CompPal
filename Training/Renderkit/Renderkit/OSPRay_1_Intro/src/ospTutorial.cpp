// system includes
#include <alloca.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

// ospray and math includes
#include "ospray/ospray_util.h"
#include "rkcommon/math/rkmath.h"
#include "rkcommon/math/vec.h"

// includes to STB for writing out a PNG image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

using namespace rkcommon::math;

// ################################################################################
// A simple function that uses the STB library to write out the framebuffer in
// PNG format.

void writeHDPNG(const char *fileName, const vec2i &size, const float *pixel) {
  constexpr int nChannels{4};
  const int stride{nChannels * size.x};
    /* Added for OIDN */
  uint32_t* pxout = (uint32_t*)malloc(sizeof(uint32_t)*size.x*size.y);
  for(size_t j = 0; j < size.y; j++) {
   for(size_t i = 0; i < size.x; i++) {
          //pxout[j * nChannels * size.x + i * nChannels + c] = (unsigned char)clamp(pixel[j * nChannels * size.x + i * nChannels + c] * 255.f, 0.f, 255.0f);
       uint32_t r = (unsigned char)clamp(pixel[j * nChannels * size.x + i * nChannels] * 255.f, 0.f, 255.0f);
       uint32_t g = (unsigned char)clamp(pixel[j * nChannels * size.x + i * nChannels + 1] * 255.f, 0.f, 255.0f);;
       uint32_t b = (unsigned char)clamp(pixel[j * nChannels * size.x + i * nChannels + 2] * 255.f, 0.f, 255.0f);;
       uint32_t a = (unsigned char)(clamp(pixel[j * nChannels * size.x + i * nChannels + 3] * 255.f, 0.f, 255.0f));
       pxout[j * size.x + i] = (a << 24) | (b << 16) | (g << 8) | r;
       //Repack the color channels
       //Example for r... here are is where r should be for the pxout array of uint32_t objects: 0x000000000000FFFFh
       //Example for r in a binary representation of the above 0x00000000000000000000000011111111b
       //Example for g... here are is where g should be for the pxout array of uint32_t objects: 0x00000000FFFF0000h
       //Example for g in a binary representation of the above 0x00000000000000001111111100000000b
   }
  }
        
  stbi_write_png(fileName, size.x, size.y, nChannels, pxout, stride);
  if(pxout) { 
      free(pxout);
      pxout = 0;
  }
}
      
void writePNG(const char *fileName, const vec2i &size, const uint32_t *pixel) {
  constexpr int nChannels{4};
  const int stride{nChannels * size.x};  
  stbi_write_png(fileName, size.x, size.y, nChannels, pixel, stride);

}

// ################################################################################
// The entry point into the program that exercises the API

int main(int argc, const char **argv) {


  // ########## Step 1 - Set up common objects to be used in the program

  // Define the width and height of the framebuffer
  // image size
  vec2i imgSize;
  imgSize.x = 1024;  // width
  imgSize.y = 768;   // height

  // camera
  // Placing the camera at the origin <0,0,0>
  float cam_pos[] = {0.f, 0.f, 0.f};

  // Orient the camera noting Y-up
  float cam_up[] = {0.f, 1.f, 0.f};

  // set the camera view direction
  float cam_view[] = {0.1f, 0.f, 1.f};

  // triangle mesh data
  // 4 vertices each with a XYZ position
  float vertex[] = {
    -1.0f, -1.0f, 3.0f, 
    -1.0f, 1.0f, 3.0f,
    1.0f, -1.0f, 3.0f, 
    0.1f,  0.1f, 0.3f};

  // 4 colors denoted by RGBA
  float color[] = {
    0.9f, 0.5f, 0.5f, 1.0f, 
    0.8f, 0.8f, 0.8f, 1.0f,
    0.8f, 0.8f, 0.8f, 1.0f, 
    0.5f, 0.9f, 0.5f, 1.0f};

  // index for the triangles
  int32_t index[] = {0, 1, 2, 1, 2, 3};


  // ########## Step 2 - Initialize the renderer using ospInit()
    printf("initialize OSPRay...");

  // initialize OSPRay; OSPRay parses (and removes) its commandline parameters,
  // It is the first thing to do in OSPRay
  OSPError init_error = ospInit(&argc, argv);
  if (init_error != OSP_NO_ERROR)
    return init_error;

  printf("done!\n");
    
    //load denoiser module
   if( !(ospLoadModule("denoiser") == OSP_NO_ERROR )) {
       printf("error loading ospray denoiser\n");
    exit(-1);
   }
  

  // ########## Step 3 - Setup the camera and commit it
  printf("setting up camera...");

  // Feeding the camera data to OSPRay
  // create and setup camera
  OSPCamera camera = ospNewCamera("perspective");
  ospSetFloat(camera, "aspect", imgSize.x / (float)imgSize.y);
  // ospSetParam passes data to objects
  // We pass "position" to the camera object, position type = OSP_VEC3F with value = cam_pos
  ospSetParam(camera, "position",  OSP_VEC3F, cam_pos); 
  ospSetParam(camera, "direction", OSP_VEC3F, cam_view);
  ospSetParam(camera, "up",        OSP_VEC3F, cam_up);
  ospCommit(camera);  // commit each object to indicate modifications are done

  printf("done!\n");


  // ########## Step 4 - Setup the scene
  printf("setting up scene...");

  // #################### Step 4.1 Feeding the model/vertex data to OSPRay
  // create OSPData-type variable named data with value = vertex
  // commit it
  // create OSPGeometry-type geometry named mesh,
  // put data into mesh in the attribute named "vertex.position"
  // release data
  // commit mesh
  OSPData data = ospNewSharedData1D(vertex, OSP_VEC3F, 4);
  ospCommit(data);
  OSPGeometry mesh = ospNewGeometry("mesh");
  ospSetObject(mesh, "vertex.position", data);
  ospRelease(data);  // we are done using this handle

  // repeat for color instead of vertex
  data = ospNewSharedData1D(color, OSP_VEC4F, 4);
  ospCommit(data);
  ospSetObject(mesh, "vertex.color", data);
  ospRelease(data);

  // repeat for index instead of color
  data = ospNewSharedData1D(index, OSP_VEC3UI, 2);
  ospCommit(data);
  ospSetObject(mesh, "index", data);
  ospRelease(data);

  ospCommit(mesh);

  // #################### Step 4.2 - Create a material
  // create OSPMaterial-type variable named mat and commit it
  OSPMaterial mat = ospNewMaterial("pathtracer", "obj");
  ospCommit(mat);

  // #################### Step 4.3 - Put mesh into a model and apply material and release mesh
  // create OSPGeometricModel-type variable named model and commit it
  // OSPGeometricModel = OSPGeometry + OSPMaterial
  OSPGeometricModel model = ospNewGeometricModel(mesh);
  ospSetObject(model, "material", mat);
  ospCommit(model);
  ospRelease(mesh);
  ospRelease(mat);

  // #################### Step 4.4 - Put model into a group (collection of models), commit group and release model
  // create OSPGroup-type variable named group and add model into group
  // OSPGroup = OSPGeometricModel(several if required) + lights (In this case there is no light) but the creation of group is still necessary
  // In out case our group has only one model and zero lights
  OSPGroup group = ospNewGroup();
  ospSetObjectAsData(group, "geometry", OSP_GEOMETRIC_MODEL, model);
  ospCommit(group);
  ospRelease(model);

  /// #################### Step 4.5 - Put group into an instance (give the group a world transform), commit instance and release group
  // create OSPInstance-type variable names instance
  // Instances in OSPRay represent a single group’s placement into the world via a transform
  OSPInstance instance = ospNewInstance(group);
  ospCommit(instance);
  ospRelease(group);

  // #################### Step 4.6 - Put instance in the world, commit world and release instance
  // World is a container of scene data represented by instances.
  // create OSPWorld-type variable named world, put instance in world and release instance
  OSPWorld world = ospNewWorld();
  ospSetObjectAsData(world, "instance", OSP_INSTANCE, instance);
  ospRelease(instance);

  // #################### Step 4.7 - Create a light for Ambient Occlusion, and put light in world. Commit world and release light
  // create OSPLight-type variable named light, put light in the world, commit world and release light
  OSPLight light = ospNewLight("ambient");
  ospCommit(light);
  ospSetObjectAsData(world, "light", OSP_LIGHT, light);
  ospRelease(light);

  ospCommit(world);

  printf("done!\n");


  // ########## Step 5 - Create the world bounds
  // create OSPBounds-type bounds named liworldBoundsght
  // once the world has been setup we can carry out the bounds
  OSPBounds worldBounds = ospGetBounds(world);
  printf("\nworld bounds: ({%f, %f, %f}, {%f, %f, %f}\n\n",
         worldBounds.lower[0], worldBounds.lower[1], worldBounds.lower[2],
         worldBounds.upper[0], worldBounds.upper[1], worldBounds.upper[2]);


  // ########## Step 6 - Create renderer
  // create OSPRenderer-type renderer named renderer
  OSPRenderer renderer = ospNewRenderer("pathtracer");  // choose between pathtracer or scivis
  ospSetFloat(renderer, "backgroundColor", 1.0f);  // white, transparent
  ospCommit(renderer);
  printf("setting up renderer...");


  // ########## Step 7 - Create and setup framebuffer
  // The framebuffer holds the rendered 2D image
  // Its arguments are object size in pixels, format, color, and channel use
  // These are the allowed formats:
      // OSP_FB_NONE:    framebuffer will not be mapped by the application
      // OSP_FB_RGBA8:   bit [0–255] linear component red, green, blue, alpha
      // OSP_FB_SRGBA:   bit sRGB gamma encoded color components, and linear alpha
      // OSP_FB_RGBA32F: 32 bit float components red, green, blue, alpha
//  OSPFrameBuffer framebuffer =
//    ospNewFrameBuffer(imgSize.x, imgSize.y, OSP_FB_SRGBA,
//                      OSP_FB_COLOR | /*OSP_FB_DEPTH |*/ OSP_FB_ACCUM);
    
    OSPFrameBuffer framebuffer =
      ospNewFrameBuffer(imgSize.x, imgSize.y, OSP_FB_RGBA32F,
                        OSP_FB_COLOR | /*OSP_FB_DEPTH |*/ OSP_FB_ACCUM);
  
   
// OIDN implementation start 
   OSPImageOperation imops[1];
   imops[0] = ospNewImageOperation("denoiser");
   ospCommit(imops[0]);
   data = ospNewSharedData1D(imops, OSP_IMAGE_OPERATION, 1);
   ospCommit(data);
   ospSetObject(framebuffer, "imageOperation", data);  
   ospCommit(framebuffer);
// OIDN implementation end 

  ospResetAccumulation(framebuffer);

  printf("rendering initial frame to firstFrame.png...");


  // ########## Step 8 - Render one frame
  ospRenderFrameBlocking(framebuffer, renderer, camera, world);


  // ########## Step 9 - Access framebuffer and write its content as PNG file
//  const uint32_t *fb = (uint32_t *)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);
//  writePNG("firstFrame.png", imgSize, fb);
  const float *fb = (float *)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);
  writeHDPNG("firstFrame_Denoised.png", imgSize, fb);
  ospUnmapFrameBuffer(fb, framebuffer);
  printf("done!\n");


  // ########## Step 10 - Shutdown system and clean up
  printf("\ncleaning up objects...");
  ospRelease(renderer);
  ospRelease(camera);
  ospRelease(framebuffer);
  ospRelease(world);
    
    /* added for denoiser */
  ospRelease(data);
  ospRelease(imops[0]);

  printf("done!\n");

  ospShutdown();

  return 0;
}
