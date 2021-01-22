//
// Created by vinhig on 19.01.21.
//

#include "Model.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Imperium::Loader {
Model::Model(int bufferCount) {
  // List doesn't init objects
  _meshes = Meshes(bufferCount);
  _setVertices = Core::List<bool>(bufferCount);
  _setIndices = Core::List<bool>(bufferCount);
  // Indices and vertices aren't set
  for (int i = 0; i < bufferCount; ++i) {
    _setVertices.Set(i, false);
    _setIndices.Set(i, false);
  }
}

Meshes& Model::GetMeshes() { return _meshes; }

void Model::SetVertices(int bufferId, float* vertices, int nbElements) {
  auto buffer = _meshes[bufferId];
  if (buffer.HasValue()) {
    // Delete previous (if there is a previous)
    if (*(_setVertices[bufferId].Value())) {
      delete buffer.Value()->Left().elements;
    }
    // Perform a memcpy
    VertexBuffer vertexBuffer = {};
    vertexBuffer.nbElements = nbElements;
    vertexBuffer.elements = (float*)malloc(9 * 4);
    memcpy(vertexBuffer.elements, vertices, sizeof(float) * nbElements);
    buffer.Value()->SetLeft(vertexBuffer);
    *(_setVertices[bufferId].Value()) = true;
  } else {
    printf("No vertex buffer with this ID (from Model::SetVertices).\n");
    return;
  }
}

void Model::SetIndices(int bufferId, int* indices, int nbElements) {
  auto buffer = _meshes[bufferId];
  if (buffer.HasValue()) {
    // Delete previous (if there is a previous)
    if (*(_setIndices[bufferId].Value())) {
      delete buffer.Value()->Left().elements;
    }
    // Perform a memcpy
    IndexBuffer indexBuffer = {};
    indexBuffer.nbElements = nbElements;
    indexBuffer.elements = (int*)malloc(sizeof(int) * nbElements);
    memcpy(indexBuffer.elements, indices, sizeof(int) * nbElements);
    buffer.Value()->SetRight(indexBuffer);
    *(_setIndices[bufferId].Value()) = true;
  } else {
    printf("No index buffer with this ID (from Model::SetIndices).\n");
    return;
  }
}

}  // namespace Imperium::Loader