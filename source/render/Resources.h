//
// Created by vinhig on 19.01.2021.
//

#pragma once

namespace Imperium::Render {

struct Vertex {
  float position[3];
  float normal[3];
  float texCoord[2];
};

template <typename T>
struct CPUBuffer {
  T* data;
  int nbElements;

  /**
   * For debug purposes only, display in human-readable form the content of this
   * buffer.
   */
  void Display();
};

struct CPUTexture {
  char* data;
  int w;
  int h;
};

}  // namespace Imperium::Render