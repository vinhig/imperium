//
// Created by vinhig on 19.01.2021.
//

#pragma once

#include "render/Resources.h"

namespace Imperium::Render::Backend {
class Backend;
}

namespace Imperium::Render::Frontend {

struct Texture {
  Backend::Backend* backend;
  Texture(CPUBuffer<int> indexBuffer, CPUBuffer<float> vertexBuffer);
  ~Texture();
};

}  // namespace Imperium::Render::Frontend