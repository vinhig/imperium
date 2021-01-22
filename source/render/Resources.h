//
// Created by vinhig on 19.01.2021.
//

#pragma once

namespace Imperium::Render {

template <typename T>
struct CPUBuffer {
  T* data;
  int nbElements;
};

struct CPUTexture {
  char* data;
  int w;
  int h;
};

}  // namespace Imperium::Render