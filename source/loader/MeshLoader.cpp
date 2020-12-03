//
// Created by vinhi on 16/10/2020.
//

#include "MeshLoader.h"

#include <ofbx.h>

#include <optional>
#include <stdexcept>

#include "../common/File.h"

bool MeshLoader::IsLoaded(const std::string& path) {
  return !(_meshes.find(path) == _meshes.end());
}

std::pair<std::vector<GPUDrawInput>, std::vector<int>> MeshLoader::Get(
    const std::string& path) {
  std::pair<std::vector<GPUDrawInput>, std::vector<int>> mesh;
  if (IsLoaded(path)) {
    mesh = _meshes[path];
  }
  return mesh;
}

std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> MeshLoader::Load(
    const std::string& path) {
  // Open file
  FILE* fp = fopen(path.c_str(), "rb");
  if (!fp) {
    throw std::runtime_error("Unable to open file " + path);
  }
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  auto* content = new ofbx::u8[file_size];
  fread(content, 1, file_size, fp);

  // Load scene
  auto scene = ofbx::load((ofbx::u8*)content, file_size,
                          (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

  std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> buffers(
      scene->getMeshCount());

  // TODO: we'll need tangents and other stuff
  int stride = (3 + 3 + 2 + 1);
  // positions + normals + uvs + material
  // 3+3+2
  for (int i = 0; i < scene->getMeshCount(); ++i) {
    auto mesh = scene->getMesh(i);

    // Get count of everything
    int vertexCount = mesh->getGeometry()->getVertexCount();
    int indexCount = mesh->getGeometry()->getIndexCount();

    // Extract vertex data
    auto position = mesh->getGeometry()->getVertices();
    auto normals = mesh->getGeometry()->getNormals();
    auto uvs = mesh->getGeometry()->getUVs();

    // Extract indices
    auto indices = mesh->getGeometry()->getFaceIndices();

    // Copy vertices
    CPUBuffer<float> verticesBuffer = {};
    verticesBuffer.stride = stride;
    verticesBuffer.data = (float*)malloc(sizeof(float) * stride * vertexCount);
    verticesBuffer.nbElements = vertexCount;

    for (int j = 0; j < vertexCount; j++) {
      verticesBuffer.data[j * stride + 0] = (float)position[j].x;
      verticesBuffer.data[j * stride + 1] = (float)position[j].y;
      verticesBuffer.data[j * stride + 2] = (float)position[j].z;

      verticesBuffer.data[j * stride + 3] = (float)normals[j].x;
      verticesBuffer.data[j * stride + 4] = (float)normals[j].y;
      verticesBuffer.data[j * stride + 5] = (float)normals[j].z;

      verticesBuffer.data[j * stride + 6] = (float)uvs[j].x;
      verticesBuffer.data[j * stride + 7] = (float)uvs[j].y;

      verticesBuffer.data[j * stride + 8] = (float)i;
    }

    // Copy indices
    CPUBuffer<int> indicesBuffer = {};
    indicesBuffer.data = (int*)malloc(sizeof(int) * indexCount);
    indicesBuffer.nbElements = indexCount;

    for (int w = 0; w < indexCount; w += 3) {
      if (indices[w + 0] < 0) {
        indicesBuffer.data[w + 0] = (int)(indices[w + 0] * (-1) - 1);
      } else {
        indicesBuffer.data[w + 0] = (int)indices[w + 0];
      }
      if (indices[w + 1] < 0) {
        indicesBuffer.data[w + 1] = (int)(indices[w + 1] * (-1) - 1);
      } else {
        indicesBuffer.data[w + 1] = (int)indices[w + 1];
      }
      if (indices[w + 2] < 0) {
        indicesBuffer.data[w + 2] = (int)(indices[w + 2] * (-1) - 1);
      } else {
        indicesBuffer.data[w + 2] = (int)indices[w + 2];
      }
    }

    buffers[i] = {verticesBuffer, indicesBuffer};
  }
  return buffers;
}

void MeshLoader::Link(const std::string& path, std::vector<GPUDrawInput> mesh,
                      std::vector<int> counts) {
  _meshes[path] = {mesh, counts};
}
