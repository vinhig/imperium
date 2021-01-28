#pragma once

//
// Created by vinhig on 22.01.2021.
//

#include <vector>

namespace Imperium::Loader {
struct RawMesh {
  float* vertices{nullptr};
  unsigned int* indices{nullptr};
  int nbVertices{0};
  int nbIndices{0};
};

using Meshes = std::vector<RawMesh>;

class Model {
 private:
  Meshes _meshes;
  int _nbMeshes{0};
 public:
  explicit Model(int nbBuffers);
  ~Model() = default;

  void AddRawMesh(RawMesh mesh);

  RawMesh* GetMeshes();
  int NbMeshes();
};
}  // namespace Imperium::Loader