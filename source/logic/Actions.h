//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <functional>

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"

enum Layer {
  A = 1, // CMeshInstance and stuff visible by default
  B = 2,
  C = 4, // CMeshInstance used for debug purpose
  D = 8, // CViewport used for deferred rendering and debug blit
  E = 10,
};

struct DrawCall {
  GPUDrawInput* drawInputs;
  GPUBuffer* uniforms;
  GPUTexture* textures;
  int nbUniforms;
  int nbTextures;
  int* counts;
  int times;
  int nbResources;
  Layer layer;
};

struct LogicCall {
  std::function<void(void)> call;
};

struct ResourceCall {
  CPUBuffer<void> from;
  GPUBuffer dest;
  bool enable;
};