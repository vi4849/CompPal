// Copyright 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "create_voxels.h"
#include "framebuffer.h"

#include <openvkl/openvkl.h>
#include <openvkl/device/openvkl.h>

// TASK: Include _MSC_VER 
// TASK: Include malloc.h 
// TASK: Include else 
// TASK: Include alloca.h 
// TASK: Include endif 

int main(int argc, char **argv)
{
  vklInit();
  VKLDevice device = vklNewDevice("cpu");
  vklCommitDevice(device);

  constexpr size_t res      = 128;
  std::vector<float> voxels = createVoxels(res);

  VKLVolume volume = vklNewVolume(device, "structuredRegular");
  vklSetVec3i(volume, "dimensions", res, res, res);
  const float spacing = 1.f / static_cast<float>(res);
  vklSetVec3f(volume, "gridSpacing", spacing, spacing, spacing);
  VKLData voxelData = vklNewData(
      device, voxels.size(), VKL_FLOAT, voxels.data(), VKL_DATA_SHARED_BUFFER);
  vklSetData(volume, "data", voxelData);
  vklRelease(voxelData);
  vklCommit(volume);

  VKLSampler sampler = vklNewSampler(volume);
  vklCommit(sampler);

// TASK: Create isovalues 
// TASK: Create context 
// TASK: Create isovaluesData 
// TASK: Create vklSetData 
// TASK: Release isovaluesData 
// TASK: Commit context 

  Framebuffer fb(64, 32);

  // We will create iterators below, and we will need to know how much memory
  // to allocate.

// TASK: create iteratorSize 

  fb.generate([&](float fx, float fy) {                                
    // Set up the ray, as iterators work on rays.

// TASK: create rayOrigin
// TASK: create rayDirection
// TASK: create rayTRange

    // Create a buffer for the iterator.

// TASK: create buffer with _malloca

// TASK: create buffer with alloca
#
    // Initialize iterator into the buffer we just created.

// TASK: create hitIterator

    // Loop over all ray-isosurface intersections along our ray.
    // vklIterateHit will return false when there
    // is no more hit left.

// TASK: Create hit

    Color color = {0.f};

// TASK: create while over hitIterator

      {
        const Color c = transferFunction(??);  // TASK: modify transferFunction ??
        color = over(color, c);
    }
    return color;
  });

  fb.drawToTerminal();

// TASK: Release context

  vklRelease(sampler);
  vklRelease(volume);
  vklReleaseDevice(device);

  return 0;
}