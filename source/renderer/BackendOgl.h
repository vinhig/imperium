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

  void Clear(uint32_t frambuffer) override;

  uint32_t CreateBuffer(void *data, size_t size) override;

  uint32_t CreateProgram(char *name) override;
};