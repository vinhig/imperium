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
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGL33});

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
    float mvp[16];
    float model[16];
    float ambient;
    float specular;
  };

  Material material = {};
  material.colors[0] = 0.8;
  material.colors[1] = 0.8;
  material.colors[2] = 0.0;
  material.colors[3] = 1.0;
  material.ambient = 0.1;
  material.specular = 0.1;
  memcpy(&material.mvp[0], &mvp[0][0], 16 * sizeof(float));
  memcpy(&material.model[0], &model[0][0], 16 * sizeof(float));

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
  struct Lights {
    float camera[4]; // vec3 have the same width as vec4
    float positions[3][3];
  };

  Lights lights = {};

  lights.camera[0] = 3.0f;
  lights.camera[1] = 3.0f;
  lights.camera[2] = 3.0f;

  lights.positions[0][0] = 1.0f;
  lights.positions[0][1] = 0.0f;
  lights.positions[0][2] = 0.0f;

  CPUBuffer<void> cpuBuffer4 = {};
  cpuBuffer4.data = (void *)&lights;
  cpuBuffer4.size = sizeof(Lights);

  auto uniformBuffer2 = device->CreateUniformBuffer(cpuBuffer4);

  GPUBuffer uniformBuffers[2] = {uniformBuffer1, uniformBuffer2};
  std::cout << "salut" << std::endl;
  float caca = 0.0f;
  while (!device->ShouldClose()) {
    device->Clear(RenderTarget{});
    // Update light position
    lights.positions[0][0] = cos(caca) * 3;
    lights.positions[0][1] = sin(caca) * 3;
    lights.positions[0][2] = sin(caca) * 3;
    device->UpdateUniformBuffer(uniformBuffer2, cpuBuffer4);

    model = glm::translate(glm::vec3(cos(caca), 0.0f, 0.0f));
    mvp = projection * view * model;
    memcpy(&material.mvp[0], &mvp[0][0], 16 * sizeof(float));
    memcpy(&material.model[0], &model[0][0], 16 * sizeof(float));
    device->UpdateUniformBuffer(uniformBuffer1, cpuBuffer3);

    device->_backend->BindProgram(program);
    device->_backend->Draw(drawInput, nbIndices, 1, uniformBuffers, 2);
    caca += 0.05f;
    device->RequestAnimationFrame();
  }
}
