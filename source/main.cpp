//
// Created by vincent on 2/10/20.
//

#include <fstream>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <string>

#include "loader/MeshLoader.h"
#include "renderer/BackendOgl.h"
#include "renderer/DeviceDesktop.h"

// Entry point for desktop platform
int main(int argc, char **argv) {
  // Create device
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGLES32});

  // Create a program
  // Some testing :)
  auto program = device->CreateProgram("basic");

  std::cout << "I've created a program OMG!!! " << program.program << std::endl;

  /*float vertices[] = {
      .5f,  .5f,  .5f,  1.0f, 0.2f, 0.3f,
      -.5f, .5f,  .5f,  0.3f, 0.6f, 1.0f,  // position and color
      -.5f, -.5f, .5f,  1.0f, 0.2f, 0.3f,
      .5f,  -.5f, .5f,  0.3f, 0.6f, 1.0f,  // v0,v1,v2,v3 (front)
      .5f,  .5f,  .5f,  1.0f, 0.2f, 0.3f,
      .5f,  -.5f, .5f,  0.3f, 0.6f, 1.0f,  // position and color
      .5f,  -.5f, -.5f, 1.0f, 0.2f, 0.3f,
      .5f,  .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // v0,v3,v4,v5 (right)
      .5f,  .5f,  .5f,  1.0f, 0.2f, 0.3f,
      .5f,  .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      -.5f, .5f,  -.5f, 1.0f, 0.2f, 0.3f,
      -.5f, .5f,  .5f,  0.3f, 0.6f, 1.0f,  // v0,v5,v6,v1 (top)
      -.5f, .5f,  .5f,  1.0f, 0.2f, 0.3f,
      -.5f, .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      -.5f, -.5f, -.5f, 1.0f, 0.2f, 0.3f,
      -.5f, -.5f, .5f,  0.3f, 0.6f, 1.0f,  // v1,v6,v7,v2 (left)
      -.5f, -.5f, -.5f, 0.7f, 0.2f, 0.3f,
      .5f,  -.5f, -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      .5f,  -.5f, .5f,  0.7f, 0.2f, 0.3f,
      -.5f, -.5f, .5f,  0.3f, 0.6f, 1.0f,  // v7,v4,v3,v2 (bottom)
      .5f,  -.5f, -.5f, 0.7f, 0.2f, 0.3f,
      -.5f, -.5f, -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      -.5f, .5f,  -.5f, 0.7f, 0.2f, 0.3f,
      .5f,  .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // v4,v7,v6,v5 (back)
  };

  int indices[] = {
      0,  1,  2,  2,  3,  0,   // v0-v1-v2, v2-v3-v0 (front)
      4,  5,  6,  6,  7,  4,   // v0-v3-v4, v4-v5-v0 (right)
      8,  9,  10, 10, 11, 8,   // v0-v5-v6, v6-v1-v0 (top)
      12, 13, 14, 14, 15, 12,  // v1-v6-v7, v7-v2-v1 (left)
      16, 17, 18, 18, 19, 16,  // v7-v4-v3, v3-v2-v7 (bottom)
      20, 21, 22, 22, 23, 20   // v4-v7-v6, v6-v5-v4 (back)
  };*/

  auto meshLoader = new MeshLoader();
  auto sphere = meshLoader->Load("../assets/sphere.fbx");

  // Not very clean; but we have to know how many indices to draw
  auto nbIndices = sphere[0].second.nbElements;

  // Is it a problem if i want to rotate my cute cubic cube ?
  auto position = glm::vec3(3.0f, 3.0f, 3.0f);
  auto lookAt = glm::vec3(0.0f, 0.0f, 0.0f);

  auto projection = glm::perspective(70.0f, 1024.0f / 768.0f, 0.1f, 100.0f);
  auto view = glm::lookAt(position, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
  auto model = glm::mat4(1.0f);

  auto mvp = projection * view * model;

  struct Material {
    float colors[4];
    // float ambient;
    // float specular;
    float mvp[16];
  };

  Material material = {{0.2, 0.3, 0.4, 1.0}};
  memcpy(&material.mvp[0], &mvp[0][0], 16 * sizeof(float));

  // Create vertex buffer
  // CPUBuffer<float> cpuBuffer1 = {};
  // cpuBuffer1.data = &vertices[0];
  // cpuBuffer1.nbElements = 72;
  // cpuBuffer1.stride = 6;
  // CPUBuffer<int> cpuBuffer2 = {};
  // cpuBuffer2.data = &indices[0];
  // cpuBuffer2.nbElements = 36;
  // cpuBuffer2.stride = 3;
  CPUBuffer<void> cpuBuffer3 = {};
  cpuBuffer3.data = (void *)&material;
  cpuBuffer3.size = sizeof(Material);

  // Create data to draw
  auto vertexBuffer = device->CreateVertexBuffer(sphere[0].first);
  auto indexBuffer = device->CreateIndexBuffer(sphere[0].second);
  auto uniformBuffer1 = device->CreateUniformBuffer(cpuBuffer3);

  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back({0, 3, false, sizeof(float) * 8,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 0)});
  inputLayoutDesc.entries.push_back({1, 3, false, sizeof(float) * 8,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 3)});
  inputLayoutDesc.entries.push_back({2, 2, false, sizeof(float) * 8,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 6)});

  auto inputLayout = device->CreateInputLayout(inputLayoutDesc);

  // Specify what to draw
  std::vector<GPUBuffer> buffers(3);
  buffers[0] = vertexBuffer;
  buffers[1] = vertexBuffer;
  buffers[2] = vertexBuffer;
  auto drawInput = device->CreateDrawInput(inputLayout, buffers, indexBuffer);

  // Let's light it up
  // - Lux, 2020
  // struct Lights {
  //   float camera[3];
  //   float positions[3][3];
  // };
  //
  // Lights lights = {{3.0f, 3.0f, 3.0f},
  //                  {
  //                      {4.0f, 0.0f, 0.0f},
  //                      {0.0f, 4.0f, 0.0f},
  //                      {0.0f, 0.0f, 4.0f},
  //                  }};
  //
  // CPUBuffer<void> cpuBuffer4 = {};
  // cpuBuffer4.data = (void *)&lights;
  // cpuBuffer4.size = sizeof(Lights);

  // auto uniformBuffer2 = device->CreateUniformBuffer(cpuBuffer4);

  GPUBuffer uniformBuffers[1] = {uniformBuffer1 /*, uniformBuffer2*/};

  while (!device->ShouldClose()) {
    device->Clear(RenderTarget{});
    device->_backend->BindProgram(program);
    device->_backend->Draw(drawInput, nbIndices, 1, &uniformBuffers[0], 1);
    device->RequestAnimationFrame();
  }
}
