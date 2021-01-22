//
// Created by vinhig on 21.01.2021.
//
#include "render/frontend/Mesh.h"

#include <cstdio>
#include <cstring>

#include "render/backend/Backend.h"

namespace Imperium::Render::Frontend {

void Mesh::Create() {
  if (!backend) {
    printf("Mesh doesn't reference a valid backend.\n");
    return;
  } else {
  }
  // Meshs are meant to be drawn
  // Temporary buffer
  auto stagingVertex = backend->CreateBuffer(
      Backend::BufferType::Staging, vertices.nbElements * sizeof(float));
  auto stagingIndex = backend->CreateBuffer(Backend::BufferType::Staging,
                                            indices.nbElements * sizeof(int));

  // Map and write buffer
  auto ptr = backend->MapBuffer(stagingVertex);
  memcpy(ptr, vertices.data, sizeof(float));
  backend->UnmapBuffer(stagingVertex);

  ptr = backend->MapBuffer(stagingIndex);
  memcpy(ptr, indices.data, sizeof(int));
  backend->UnmapBuffer(stagingIndex);

  vertexBuffer = backend->CreateBuffer(Backend::BufferType::Staging,
                                       vertices.nbElements * sizeof(float));
  indexBuffer = backend->CreateBuffer(Backend::BufferType::Staging,
                                      indices.nbElements * sizeof(int));

  backend->CopyBuffer(stagingVertex, vertexBuffer);
  backend->CopyBuffer(stagingIndex, indexBuffer);

  backend->DeleteBuffer(stagingVertex);
  backend->DeleteBuffer(stagingIndex);

  valid = true;
}

void Mesh::Destroy() {
  backend->DeleteBuffer(vertexBuffer);
  vertexBuffer = nullptr;
  backend->DeleteBuffer(indexBuffer);
  indexBuffer = nullptr;
  backend = nullptr;
}

}  // namespace Imperium::Render::Frontend
