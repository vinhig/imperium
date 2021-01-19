//
// Created by vinhig on 19.01.2021.
//

#pragma once

#include "core/List.h"
#include "core/Pair.h"

using namespace Imperium;

namespace Imperium::Loader {
struct VertexBuffer {
  int nbElements{0};
  float* elements{nullptr};
};

struct IndexBuffer {
  int nbElements{0};
  int* elements{nullptr};
};

using Meshes = Core::List<Core::Pair<VertexBuffer, IndexBuffer>>;

class Model {
 private:
  Meshes _meshes;

 public:
  Model(int bufferCount);
  ~Model();

  Meshes& GetMeshes();
  void SetVertices(int bufferId, float* vertices, int nbElements);
  void SetIndices(int bufferId, int* indices, int nbElements);
};

}  // namespace Imperium::Loader