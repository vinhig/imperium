//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <functional>

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"

struct DrawCall {
  GPUDrawInput* drawInputs;
  GPUBuffer* uniforms;
  int nbUniforms;
  int* counts;
  int times;
  int nbResources;
};

struct LogicCall {
  std::function<void(void)> call;
};

struct ResourceCall {
  CPUBuffer<void> from;
  GPUBuffer dest;
  bool enable;
};