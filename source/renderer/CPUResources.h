//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdio>

/**
 * CPUBuffer contains a bunch of data to be copied on the GPU memory and used in
 * draw calls.
 * @tparam T Type of data handled by this buffer.
 */
template <typename T>
struct CPUBuffer {
  /**
   * Array of elements.
   */
  T* data;
  /**
   * Number of elements in each entry.
   */
   int stride;
  /**
   * Number of elements in data.
   */
  union {
    int nbElements;
    size_t size;
  };
};