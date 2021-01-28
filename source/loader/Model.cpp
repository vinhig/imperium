//
// Created by vinhig on 22.01.2021.
//

#include "loader/Model.h"
#include <cstdio>

namespace Imperium::Loader {
Model::Model(int nbBuffers) { _meshes.reserve(nbBuffers); }

void Model::AddRawMesh(RawMesh mesh) {
  _meshes.push_back(mesh);
  
  _nbMeshes += 1;
}

RawMesh* Model::GetMeshes() { return _meshes.data(); }

int Model::NbMeshes() { return _nbMeshes; }
}  // namespace Imperium::Loader