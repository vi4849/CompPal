// Copyright 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <openvkl/openvkl.h>
#include <openvkl/vdb.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

// ################################################################################
// Step 1 - review Welford's algorithm to incrementally compute sample variance.
// We use this to determine if a leaf node can be compressed into a tile.
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
struct OnlineVariance
{
  size_t N { 0 };
  float  mean { 0 };
  float  ssd  { 0 };

  inline void update(float value)
  {
    N++;
    const float diffOld = value - mean;
    mean += diffOld / static_cast<float>(N);
    const float diffNew = value - mean;
    ssd += diffOld * diffNew;
  }

  inline float sampleVariance() const
  {
    return (N > 1) ? (ssd / static_cast<float>(N-1)) : 0.f;
  }
};

// ################################################################################
// Step 2 - compute the procedural density wavelet.
// This scalar field is essentially the same as our WaveletVdbVolume.
inline float density(const vkl_vec3f &p)
{
  constexpr float f = 11.f;
  return 0.25f * (
      std::sin(f * p.x) + std::sin(f * p.y) + std::cos(f * p.z) + p.x
  );
}


// ################################################################################
// Step 3 - Create a VDB volume using the density() function above.
// VKL_VDB_RES_LEAF  =  resolution of a single leaf node.
// This is a compile time constant, and it can be configured. The default is 8.
VKLVolume createVolume(VKLDevice device, int32_t Nx, int32_t Ny, int32_t Nz)
{
  const vkl_vec3f scale = {1.f/static_cast<float>(Nx*VKL_VDB_RES_LEAF),
                           1.f/static_cast<float>(Ny*VKL_VDB_RES_LEAF),
                           1.f/static_cast<float>(Nz*VKL_VDB_RES_LEAF)};
  printf("VKL_VDB_RES_LEAF = %d\n", VKL_VDB_RES_LEAF);
  std::vector<uint32_t>  nodeFormat;
  std::vector<uint32_t>  nodeLevel;
  std::vector<vkl_vec3i> nodeOrigin;
  std::vector<VKLData>   nodeData;
  std::vector<float>     voxels;
  // In 3D computer graphics, a voxel represents a value on a regular grid in three-dimensional space
  // https://en.wikipedia.org/wiki/Voxel

  voxels.reserve(VKL_VDB_RES_LEAF*VKL_VDB_RES_LEAF*VKL_VDB_RES_LEAF);
  // reserve requests the vector capacity to be at least n elements long

  size_t numTiles = 0;
  size_t numConstant = 0;

  // Nx, Ny and Nz are the number of leaf nodes in each dimension. Nx = 10; Ny = 5; Nz = 8
  for (int32_t ix = 0; ix < Nx; ++ix) // Triple loop along the leaf nodes
  for (int32_t iy = 0; iy < Ny; ++iy)
  for (int32_t iz = 0; iz < Nz; ++iz)
  {
    const vkl_vec3i origin = {ix*VKL_VDB_RES_LEAF,
                              iy*VKL_VDB_RES_LEAF,
                              iz*VKL_VDB_RES_LEAF};

    voxels.clear(); // clear removes all elements from the vector

    // We compute sample variance to determine if a leaf node is 
    // "uniform enough" to be considered constant.
    OnlineVariance ov;

    for (int32_t x = 0; x < VKL_VDB_RES_LEAF; ++x) // triple loop alont the resolution
    for (int32_t y = 0; y < VKL_VDB_RES_LEAF; ++y)
    for (int32_t z = 0; z < VKL_VDB_RES_LEAF; ++z)
    {
      const vkl_vec3f p = {static_cast<float>(x+origin.x) * scale.x,
                           static_cast<float>(y+origin.y) * scale.y,
                           static_cast<float>(z+origin.z) * scale.z};
      const float d = density(p); // call to function of step 2
      ov.update(d);               // calculate the varince of the density
      voxels.push_back(d);        // push_back adds a new element at the vector end
    }

    nodeLevel.push_back(VKL_VDB_LEAF_LEVEL);
    nodeOrigin.push_back(origin);

    if (std::sqrt(ov.sampleVariance()) > 0.1f) {
      nodeFormat.push_back(VKL_FORMAT_DENSE_ZYX);
      ++numConstant;
    }
    else {
      nodeFormat.push_back(VKL_FORMAT_TILE);
      voxels.resize(1);
      ++numTiles;
    }
      voxels[0] = ov.mean;

    nodeData.push_back(vklNewData(device, voxels.size(), VKL_FLOAT, voxels.data()));
  }

  VKLVolume volume = vklNewVolume(device, "vdb");
  vklSetData(volume, "node.format", vklNewData(device, nodeFormat.size(), VKL_UINT , nodeFormat.data()));
  vklSetData(volume, "node.level" , vklNewData(device, nodeLevel.size() , VKL_UINT , nodeLevel.data()));
  vklSetData(volume, "node.origin", vklNewData(device, nodeOrigin.size(), VKL_VEC3I, nodeOrigin.data()));
  vklSetData(volume, "node.data"  , vklNewData(device, nodeData.size()  , VKL_DATA , nodeData.data()));
  vklCommit(volume);

  std::cout << "Creating volume with " << numTiles << " tiles and "
    << numConstant << " constant nodes." << std::endl;

  return volume;
}


// ################################################################################
// Step 4 - Helpers to draw the volume using ASCII symbols.
inline char mapDensity(float v) 
{
  static constexpr const char *map = " .,:lo8$0@";
  const int vm = std::max<int>(0,std::min<int>(static_cast<int>(v*10), 9));
  return map[vm];
}

void drawBuffer(int32_t rx, int32_t ry, const float *buffer)
{
  for (int32_t y = 0; y < ry; ++y) {
    for (int32_t x = 0; x < rx; ++x) {
      std::cout << mapDensity(*buffer++);
    }
    std::cout << std::endl;
  }
}


// ################################################################################
// Step 5 - main entry point to the program
int main()
{
  vklLoadModule("cpu_device");
  VKLDevice device = vklNewDevice("cpu");
  vklCommitDevice(device);
  
  // The number of leaf nodes in each dimension.
  constexpr int32_t Nx = 10;
  constexpr int32_t Ny = 5;
  constexpr int32_t Nz = 8;

  VKLVolume volume = createVolume(device, Nx, Ny, Nz);

  // We need a sampler object to be able to access the volume.
  VKLSampler sampler = vklNewSampler(volume);
  vklCommit(sampler);

  constexpr int32_t rx = 48; // Number of samples in x
  constexpr int32_t ry = 24; // Number of samples in y
  std::vector<float> imgBuffer;
  imgBuffer.reserve(rx*ry); // reserve requests the vector capacity to be at least n elements long

  // Sample a slice of the volume into a buffer.
  for (int32_t y = 0; y < ry; ++y)
  for (int32_t x = 0; x < rx; ++x) 
  {
    const vkl_vec3f p = { 
      (x / static_cast<float>(rx-1)) * (Nx * VKL_VDB_RES_LEAF),
      (y / static_cast<float>(ry-1)) * (Ny * VKL_VDB_RES_LEAF),
      0.5 * (Nz * VKL_VDB_RES_LEAF) // horizontal cross section in the middle high of the volume
    };
    imgBuffer.push_back(vklComputeSample(sampler, &p));
  }

  vklRelease(sampler);
  vklRelease(volume);
  vklReleaseDevice(device);

  drawBuffer(rx, ry, imgBuffer.data()); // calls function in step 4

  return 0;
}
