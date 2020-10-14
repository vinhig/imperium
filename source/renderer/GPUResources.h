//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>

#include "Descriptions.h"

struct RenderTarget {
  uint32_t depthBuffer;
  uint32_t *colorBuffers;
  size_t nbColorBuffers;
  uint32_t framebuffer;
};

/**
 * GPUBuffer represents a buffer containing data stored on the gpu.
 */
struct GPUBuffer {
  uint32_t buffer;
  BufferTypeDesc purpose;
};

struct GPUInputLayout {
  InputLayoutDesc cpuInputLayout;
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