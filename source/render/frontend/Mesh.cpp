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
  }
  // Meshs are meant to be drawn
  // Temporary buffer
  auto stagingVertex = backend->CreateBuffer(
      Backend::BufferType::Staging, vertices.nbElements * sizeof(float));
  auto stagingIndex = backend->CreateBuffer(Backend::BufferType::Staging,
                                            indices.nbElements * sizeof(int));

  // Map and write buffer
  auto ptr = backend->MapBuffer(stagingVertex);

  memcpy(ptr, vertices.data, sizeof(float) * vertices.nbElements);
  backend->UnmapBuffer(stagingVertex);

  ptr = backend->MapBuffer(stagingIndex);
  memcpy(ptr, indices.data, sizeof(int) * indices.nbElements);
  backend->UnmapBuffer(stagingIndex);

  vertexBuffer = backend->CreateBuffer(Backend::BufferType::Vertex,
                                       vertices.nbElements * sizeof(float));
  indexBuffer = backend->CreateBuffer(Backend::BufferType::Index,
                                      indices.nbElements * sizeof(int));

  backend->CopyBuffer(stagingIndex, indexBuffer);
  backend->CopyBuffer(stagingVertex, vertexBuffer);

  backend->DeferDeleteBuffer(stagingVertex);
  backend->DeferDeleteBuffer(stagingIndex);

  valid = true;
}

void Mesh::Destroy() {
  if (!backend) {
    printf("Mesh::Destroy(): Mesh doesn't reference a valid backend.\n");
  }
  printf("Mesh::Destroy()\n");
  backend->DeferDeleteBuffer(vertexBuffer);
  vertexBuffer = nullptr;
  backend->DeferDeleteBuffer(indexBuffer);
  indexBuffer = nullptr;
  backend = nullptr;
}

Mesh& Mesh::operator=(Mesh other) {
  if (other.backend == nullptr) {
    printf("Mesh::operator=(): Mesh doesn't reference a valid backend.\n");
  }
  backend = other.backend;
  vertexBuffer = other.vertexBuffer;
  indexBuffer = other.indexBuffer;
  valid = other.valid;
  vertices.data = other.vertices.data;
  vertices.nbElements = other.vertices.nbElements;
  indices.data = other.indices.data;
  indices.nbElements = other.indices.nbElements;

  return *this;
}

void Mesh::Draw() {
  backend->BindVertexBuffers(1, vertexBuffer);
  backend->BindIndexBuffer(indexBuffer);
  backend->DrawElements(3);
}

}  // namespace Imperium::Render::Frontend
