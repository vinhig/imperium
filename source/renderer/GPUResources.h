//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>

#include "Descriptions.h"

/**
 * GPURenderTarget is a group of texture that are used as drawing target.
 */
struct GPURenderTarget {
  uint32_t framebuffer;
};

/**
 * GPUBuffer represents a buffer containing data stored on the gpu.
 */
struct GPUBuffer {
  uint32_t buffer;
  size_t stride;
  BufferTypeDesc purpose;
};

struct GPUInputLayout {
  InputLayoutDesc cpuInputLayout;
  size_t stride;
  uint32_t inputLayout;
};

/**
 * GPUDrawInput stores how some specific buffers have to be structured for a
 * draw call. It links an index buffer and multiple vertex buffers.
 */
struct GPUDrawInput {
  uint32_t inputLayout;
  uint32_t vao;
};

/**
 * GPUProgram is the combination of a vertex and fragment shader.
 */
struct GPUProgram {
  uint32_t program;
};

/**
 * GPUTexture is an image stored on the gpu.
 */
struct GPUTexture {
  uint32_t texture;
  int width, height;
};