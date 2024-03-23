

* Start with minimal01.cpp
  - Explain field: 3D coordinate, procedural noise.
  - Explain framebuffer: An image, and can draw itself to terminal.
  - fb.generate() calls the lambda for each pixel, fx, fy are normalized
    image coordinates in [0, 1].
  - For loop: We want to visualize multiple slices to get a better feel of 
    what the field looks like.

* Compile. Show CMakeLists.txt
  - Important to set minimal version.
  - find_package will find install openvkl as long as it is in the path.
  - Show export PATH and LD_LIBRARY_PATH
  - Show build: mkdir build, cmake .., make, ./minimal

* Explain Output
  - color map: red is positive, blue is negative.
  - We can see multiple slices through the volume. That is the for loop over fz!
  - Show how the field changes with a changing slice.


// Towards 02


* Remove for loop. Set fz = 0.f.
* Set fb to 64, 32.
* Include openvkl/openvkl.h
* Add LoadModule.
* Create, commit, release device.


// Towards 03

  * createVoxels
  * Call createVoxels
  * vklNewData
  * vklNewVolume
  * set dimensions
  * set gridSpacing
  * set data, release
  * commit volume
  * create sampler, commit.
  * create vec3f p
  * vklComputeSample
  * release sampler
  * release volume

// Towards 04
  * remove createVoxels
  * Add createVDBVolume:
    - Add vectors to hold parameters per node.
    - Add voxels buffer.
    - Add outer loop over all leaf nodes.
    - Create volume, set params.
    - Add parameters
    - Commit.
    - Add inner loop, rasterizing a single node.
  * Show maxSamplingDepth param on Sampler.
    vklSetInt(sampler, "maxSamplingDepth", 2);

// Towards 05 (optional)
  * add online variance header
  * add variance tracking
  * add compression.
  * Show compression by increasing threshold.


// Towards 07 
  * Start with 06!
 




