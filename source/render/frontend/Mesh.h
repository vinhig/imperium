#pragma once

//
// Created by vinhig on 19.01.2021.
//

#include "render/Resources.h"

namespace Imperium::Render::Backend {
class Backend;
struct Buffer;
}  // namespace Imperium::Render::Backend

namespace Imperium::Render::Frontend {

struct Mesh {
  Backend::Backend* backend{nullptr};
  Backend::Buffer* vertexBuffer{nullptr};
  Backend::Buffer* indexBuffer{nullptr};
  bool valid{false};
  CPUBuffer<float> vertices{};
  CPUBuffer<int> indices{};

  Mesh() = default;
  ~Mesh() = default;

  void Create();
  void Destroy();
};

}  // namespace Imperium::Render::Frontend