#pragma once

//
// Created by vinhig on 22.01.2021.
//

#include <vector>

namespace Imperium::Render {
struct Vertex;
}

namespace Imperium::Loader {

struct RawMesh {
  Render::Vertex* vertices{nullptr};
  unsigned int* indices{nullptr};
  unsigned int nbVertices{0};
  unsigned int nbIndices{0};
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