//
// Created by vinhig on 19.01.21.
//

#include "Model.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Imperium::Loader {
Model::Model(int bufferCount) {
  _meshes = Meshes(bufferCount);
}

Meshes& Model::GetMeshes() { return _meshes; }

void Model::SetVertices(int bufferId, float* vertices, int nbElements) {
  auto buffer = _meshes[bufferId];
  if (buffer.HasValue()) {
    // Delete previous
    delete[] buffer.Value()->Left().elements;
    // Perform a memcpy
    VertexBuffer vertexBuffer = {nbElements,
                                 (float*)malloc(sizeof(float) * nbElements)};
    memcpy(vertexBuffer.elements, vertices, sizeof(float) * nbElements);
    buffer.Value()->SetLeft(vertexBuffer);
  } else {
    printf("No vertex buffer with this ID (from Model::SetVertices).\n");
    return;
  }
}

void Model::SetIndices(int bufferId, int* indices, int nbElements) {
  auto buffer = _meshes[bufferId];
  if (buffer.HasValue()) {
    // Delete previous
    delete[] buffer.Value()->Right().elements;
    // Perform a memcpy
    IndexBuffer indexBuffer = {nbElements,
                               (int*)malloc(sizeof(int) * nbElements)};
    memcpy(indexBuffer.elements, indices, sizeof(int) * nbElements);
    buffer.Value()->SetRight(indexBuffer);
  } else {
    printf("No index buffer with this ID (from Model::SetIndices).\n");
    return;
  }
}

}  // namespace Imperium::Loader