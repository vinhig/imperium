//
// Created by vinhi on 16/10/2020.
//

#include "MeshLoader.h"

#include <ofbx.h>

#include <iostream>
#include <stdexcept>

std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> MeshLoader::Load(
    const std::string& path
#ifdef __ANDROID__
    ,
    std::function<std::vector<unsigned char>(std::string)> fileReader
#endif
) {
#ifndef __ANDROID__
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
#else
  auto vcontent = fileReader(path);
  auto content = vcontent.data();
  long file_size = vcontent.size();
#endif

  // Load scene
  auto scene = ofbx::load((ofbx::u8*)content, file_size,
                          (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

  std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> buffers(
      scene->getMeshCount());

  // TODO: we'll need tangents and other stuff
  int stride = (3 + 3 + 2);
  // positions + normals + uvs
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
