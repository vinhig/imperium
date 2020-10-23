//
// Created by vinhi on 23/10/2020.
//

#include "MeshMerger.h"

#include <cassert>
#include <iostream>

std::vector<IndexRange> MeshMerger::Merge(
    const std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> &buffers,
    CPUBuffer<float> *vertices, CPUBuffer<int> *indices) {
  // Compute size of final buffer
  // And check stride of each buffer
  int verticesCount = 0;
  int indicesCount = 0;
  int stride = buffers[0].first.stride;
  std::cout << "Stride: " << stride << std::endl;
  for (const auto &buffer : buffers) {
    verticesCount += buffer.first.nbElements;
    indicesCount += buffer.second.nbElements;
    assert(buffer.first.stride == stride);
  }
  std::cout << "Size: " << verticesCount << std::endl;

  vertices->data = (float *)malloc(sizeof(float) * verticesCount * stride);
  vertices->nbElements = verticesCount;
  vertices->stride = stride;

  indices->data = (int *)malloc(sizeof(int) * indicesCount);
  indices->nbElements = indicesCount;

  // Vertices can be innocently copied
  int lastSize = 0;
  for (const auto &buffer : buffers) {
    int bufferSize = buffer.first.nbElements * buffer.first.stride;

    // Dummy copy
    // TODO: should use memcpy of crap like that
    for (int j = 0; j < buffer.first.nbElements; j++) {
      auto data = vertices->data;  // Just to make one line copy lol
      data[j * stride + 0 + lastSize] = buffer.first.data[j * stride + 0];
      data[j * stride + 1 + lastSize] = buffer.first.data[j * stride + 1];
      data[j * stride + 2 + lastSize] = buffer.first.data[j * stride + 2];

      data[j * stride + 3 + lastSize] = buffer.first.data[j * stride + 3];
      data[j * stride + 4 + lastSize] = buffer.first.data[j * stride + 4];
      data[j * stride + 5 + lastSize] = buffer.first.data[j * stride + 5];

      data[j * stride + 6 + lastSize] = buffer.first.data[j * stride + 6];
      data[j * stride + 7 + lastSize] = buffer.first.data[j * stride + 7];

      data[j * stride + 8 + lastSize] = buffer.first.data[j * stride + 8];
      /*memcpy(&vertices->data[lastSize], buffer.first.data,
             bufferSize * sizeof(float));*/
    }

    lastSize += bufferSize;
  }
  // Indices should be modified by adding an offset
  std::vector<IndexRange> ranges;
  int offset = 0;
  for (const auto &buffer : buffers) {
    int nbIndices = buffer.second.nbElements;
    for (int i = 0; i < nbIndices; i++) {
      indices->data[i + offset] = buffer.second.data[i] + offset;
    }
    ranges.push_back({offset, offset + nbIndices});
    offset += nbIndices;
  }

  return ranges;
}
