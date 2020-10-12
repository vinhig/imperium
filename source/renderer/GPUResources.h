//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>

struct RenderTarget {
  uint32_t depthBuffer;
  uint32_t *colorBuffers;
  size_t nbColorBuffers;
  uint32_t framebuffer;
};

struct GPUTexture {
  uint32_t texture;
  int width, height;
};

struct GPUBuffer {
  uint32_t buffer;
  BufferTypeDesc purpose;
};

struct GPUProgram {
  uint32_t program;
};