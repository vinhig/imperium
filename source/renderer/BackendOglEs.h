//
// Created by vincent on 2/10/20.
//

#pragma once

#include "Backend.h"

/**
 * Backend built on OpenGL Es 3.2.
 * Mainly used with the Android an Web build.
 */
class BackendOglEs : public Backend {
 public:
  void Clear(uint32_t framebuffer) override;

  GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) override;

  uint32_t CreateProgram(std::vector<uint32_t> vertexSource,
                         std::vector<uint32_t> fragmentSource) override;
};