//
// Created by vinhig on 19.01.2021.
//

#pragma once

#include <cstdio>

#include "core/List.h"
#include "core/Pair.h"

using namespace Imperium;

namespace Imperium::Loader {
struct VertexBuffer {
  int nbElements{0};
  float* elements{nullptr};

  VertexBuffer() {}
};

struct IndexBuffer {
  int nbElements{0};
  int* elements{nullptr};
};

using Meshes = Core::List<Core::Pair<VertexBuffer, IndexBuffer>>;

class Model {
 private:
  // List of meshes
  Meshes _meshes;

  // Meshes may be not initialized
  Core::List<bool> _setVertices;
  Core::List<bool> _setIndices;

 public:
  explicit Model(int bufferCount);
  ~Model() = default;

  Meshes& GetMeshes();
  void SetVertices(int bufferId, float* vertices, int nbElements);
  void SetIndices(int bufferId, int* indices, int nbElements);
};

}  // namespace Imperium::Loader