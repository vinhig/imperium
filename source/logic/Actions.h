//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <functional>

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"

struct DrawCall {
  std::vector<GPUDrawInput> drawInputs;
  std::vector<int> counts;
  int times;
};

struct LogicCall {
  std::function<void(void)> call;
};

struct ResourceCall {
  CPUBuffer<void> from;
  GPUBuffer dest;
  bool enable;
};