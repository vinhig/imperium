//
// Created by vincent on 2/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>

#include "Backend.h"

/**
 * Backend built on OpenGL 3.3.
 * Default backend on desktop (Linux, Mac and Windows).
 */
class BackendOgl : public Backend {
 public:
  BackendOgl() = default;
  ~BackendOgl() = default;

  void Clear(uint32_t framebuffer) override;

  GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) override;

  uint32_t CreateProgram(std::vector<uint32_t> vertexSource,
                         std::vector<uint32_t> fragmentSource) override;
};