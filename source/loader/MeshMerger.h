//
// Created by vinhi on 23.10.2020.
//

#pragma once

#include <vector>

#include "../renderer/CPUResources.h"
#include "../renderer/Descriptions.h"

/**
 * Optimize multiple mesh buffers for drawing. Merge them into on buffer. Return
 * indices range for each merged buffer.
 */
class MeshMerger {
 public:
  MeshMerger() = default;
  ~MeshMerger() = default;

  /**
   * Merge mesh buffers into one buffer and return indices range for each of
   * them. Before being merged, buffers are optimized. All buffers have to share
   * the same stride.
   * TODO: indices ranges are completely broken
   * @param buffers Mesh buffers to merge.
   * @param destination CPUBuffer containing merged vertices.
   * @return Indices range for each merged buffer. Same length as number of
   * input buffers.
   */
  std::vector<IndexRange> Merge(
      const std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> &buffers,
      CPUBuffer<float> *vertices, CPUBuffer<int> *indices);
};
