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
  unsigned int count{0};
  bool valid{false};
  CPUBuffer<Vertex> vertices{};
  CPUBuffer<unsigned int> indices{};

  Mesh() = default;
  ~Mesh() = default;

  /**
   * Copy operator.
   */
  Mesh& operator=(Mesh other);

  void Create();
  void Destroy();
  void Draw();
};

}  // namespace Imperium::Render::Frontend