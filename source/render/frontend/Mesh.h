//
// Created by vinhig on 19.01.2021.
//

#pragma once

#include "render/Resources.h"

namespace Imperium::Render::Backend {
class Backend;
}

namespace Imperium::Render::Frontend {

struct Mesh {
  Backend::Backend* backend;
  Mesh() = default;
  Mesh(CPUBuffer<int> indexBuffer, CPUBuffer<float> vertexBuffer);
  ~Mesh() = default;
};

}  // namespace Imperium::Render::Frontend