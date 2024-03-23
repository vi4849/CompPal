// Copyright 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "create_voxels.h"
#include "framebuffer.h"

#include <openvkl/openvkl.h>
#include <openvkl/device/openvkl.h>

int main(int argc, char **argv)
{
  vklInit();
  VKLDevice device = vklNewDevice("cpu");
  vklCommitDevice(device);

  constexpr size_t res      = 128;
  std::vector<float> voxels = createVoxels(res);

  VKLVolume volume = vklNewVolume(device, "structuredRegular");     // NOTE: modified back to structuredRegular
  vklSetVec3i(volume, "dimensions", res, res, res);

  const float spacing = 1.f / static_cast<float>(res);
  vklSetVec3f(volume, "gridSpacing", spacing, spacing, spacing);    // NOTE: modified back to structuredRegular
  VKLData voxelData = vklNewData(
      device, voxels.size(), VKL_FLOAT, voxels.data(), VKL_DATA_SHARED_BUFFER);
  vklSetData(volume, "data", voxelData);
  vklRelease(voxelData);

  vklCommit(volume);

  VKLSampler sampler = vklNewSampler(volume);
  vklCommit(sampler);

  Framebuffer fb(64, 32);

  // We trace the volume with simple ray marching.
  // Conceptually, this is a series of camera-aligned,
  // semi transparent planes.
  // We walk along the ray in regular steps.

// TASK: create numSteps
// TASK: create tMax
// TASK: create tStep
    
  fb.generate([&](float fx, float fy) {

// TASK: initialize color
// TASK: create loop over numSteps
      
    {
        const vkl_vec3f p = {fx, fy, ??};                             // TASK: modify z
        ?? ?? ?? = transferFunction(vklComputeSample(&sampler, &p));  // TASK: transfer transferFunction values to color variable

        // We use the over operator to blend semi-transparent
        // "surfaces" together.

// TASK: create the accumulated color

        // Now we've created a very simple volume renderer using
        // Open VKL!
    }

// TASK: add return

  });

  fb.drawToTerminal();

  vklRelease(sampler);
  vklRelease(volume);
  vklReleaseDevice(device);

  return 0;
}