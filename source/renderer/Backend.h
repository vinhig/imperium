//
// Created by vincent on 2/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "CPUResources.h"
#include "Descriptions.h"
#include "GPUResources.h"

/**
 * Backend handles specific API calls.
 * It doesn't create its context nor its window.
 * It's stateless (no bindSomething).
 */
class Backend {
 public:
  /**
   * Clear given framebuffer. Clear means fill the whole texture with a solid
   * color. Each texture linked to the framebuffer will be cleared.
   * @param framebuffer Framebuffer to clear.
   */
  virtual void Clear(uint32_t framebuffer) = 0;
  /**
   * Create some fucking good buffer of data.
   * @param data Data to upload on GPU
   * @param size Size of data (in bytes)
   * @return GPU address of buffer
   */
  virtual GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) = 0;
  /**
   * Create a program from fragment and vertex shader binaries.
   * @param name
   * @return GPU address of program
   */
  virtual uint32_t CreateProgram(std::vector<uint32_t> vertexSource,
                                 std::vector<uint32_t> fragmentSource) = 0;
};
