//
// Created by vincent on 02.10.20.
//

#include <fstream>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <string>

#include "loader/MeshLoader.h"
#include "loader/MeshMerger.h"
#include "loader/TextureLoader.h"
#include "renderer/BackendOgl.h"
#include "renderer/DeviceDesktop.h"

// Entry point for desktop platform
int main(int argc, char **argv) {
  // Create device
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGL33});

  // Create a program
  // Some testing :)
  auto program = device->CreateProgram("basic");

  // Load robot mesh
  // Merge all its sub-meshes into one
  auto meshLoader = new MeshLoader();
  // auto meshMerger = new MeshMerger();
  auto plant = meshLoader->Load("../assets/indoor-plant.fbx");
  // auto bench = meshLoader->Load("../assets/bench.fbx");

  // Create data to draw
  std::vector<GPUBuffer> vertexBuffers;
  std::vector<GPUBuffer> indexBuffers;
  std::vector<int> nbElements;
  for (const auto &submesh : plant) {
    vertexBuffers.push_back(device->CreateVertexBuffer(submesh.first));
    indexBuffers.push_back(device->CreateIndexBuffer(submesh.second));
    nbElements.push_back(submesh.second.nbElements);
  }

  // Is it a problem if i want to rotate my cute cubic cube ?
  auto position = glm::vec3(3.0f, 3.0f, 3.0f);
  auto lookAt = glm::vec3(0.0f, 0.0f, 0.0f);

  auto projection = glm::perspective(70.0f, 1024.0f / 768.0f, 0.1f, 100.0f);
  auto view = glm::lookAt(position, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
  auto model = glm::mat4(1.0f);

  auto mvp = projection * view * model;

  struct Material {
    float ambient;
    float specular;
    float something;
    float dumb;
  };

  Material materials = {0.4, 1.0};

  CPUBuffer<void> cpuBuffer5 = {};
  cpuBuffer5.data = (void *)&materials;
  cpuBuffer5.size = sizeof(Material);
  auto uniformBuffer3 = device->CreateUniformBuffer(cpuBuffer5);

  struct Object {
    float mvp[16];
    float model[16];
  };

  Object obj = {};
  memcpy(&obj.mvp[0], &mvp[0][0], 16 * sizeof(float));
  memcpy(&obj.model[0], &model[0][0], 16 * sizeof(float));

  auto b = sizeof(double);

  CPUBuffer<void> cpuBuffer3 = {};
  cpuBuffer3.data = (void *)&obj;
  cpuBuffer3.size = sizeof(Object);
  std::cout << "sizeof(Object) == " << sizeof(Object) << std::endl;
  auto uniformBuffer1 = device->CreateUniformBuffer(cpuBuffer3);

  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back({0, 3, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 0)});
  inputLayoutDesc.entries.push_back({1, 3, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 3)});
  inputLayoutDesc.entries.push_back({2, 2, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 6)});

  auto inputLayout = device->CreateInputLayout(inputLayoutDesc);

  // Specify what to draw
  std::vector<GPUDrawInput> drawInputs;
  for (int i = 0; i < plant.size(); i++) {
    std::vector<GPUBuffer> buffers(3);
    buffers[0] = vertexBuffers[i];
    buffers[1] = vertexBuffers[i];
    buffers[2] = vertexBuffers[i];
    auto drawInput =
        device->CreateDrawInput(inputLayout, buffers, indexBuffers[i]);
    drawInputs.push_back(drawInput);
  }

  // Let's light it up
  // - Lux, 2020
  struct Lights {
    float camera[4];  // vec3 have the same width as vec4
    float positions[4];
  };

  Lights lights = {};

  lights.camera[0] = 3.0f;
  lights.camera[1] = 3.0f;
  lights.camera[2] = 3.0f;
  lights.camera[3] = 1.0f;

  lights.positions[0] = 4.0f;
  lights.positions[1] = 0.0f;
  lights.positions[2] = 0.0f;
  lights.positions[3] = 1.0f;

  CPUBuffer<void> cpuBuffer4 = {};
  cpuBuffer4.data = (void *)&lights;
  cpuBuffer4.size = sizeof(Lights);

  auto uniformBuffer2 = device->CreateUniformBuffer(cpuBuffer4);

  GPUBuffer uniformBuffers[3] = {uniformBuffer1, uniformBuffer2,
                                 uniformBuffer3};

  // Some fucking good texturing
  auto textureLoader = new TextureLoader();
  // Diffuse stuff
  auto diffuseCpuTexture =
      textureLoader->Load("../assets/indoor-plant-diffuse.jpg");
  auto diffuseTexture = device->CreateTextureFromData(diffuseCpuTexture);
  textureLoader->Link("../assets/indoor-plant-diffuse.png", diffuseTexture.texture);
  // Normal stuff
  auto normalCpuTexture = textureLoader->Load("../assets/indoor-plant-normal.png");
  auto normalTexture = device->CreateTextureFromData(normalCpuTexture);
  textureLoader->Link("../assets/wheels.png", normalTexture.texture);

  float caca = 0.0f;
  while (!device->ShouldClose()) {
    device->Clear(RenderTarget{});
    // Update light position
    lights.positions[0] = cos(caca) * 4;
    lights.positions[1] = sin(caca) * 4;
    lights.positions[2] = sin(caca) * 4;
    device->UpdateUniformBuffer(uniformBuffer2, cpuBuffer4);

    model = glm::translate(glm::vec3(0.0f, -1.5f, 0.0f)) *
            glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
    mvp = projection * view * model;
    memcpy(&obj.mvp[0], &mvp[0][0], 16 * sizeof(float));
    memcpy(&obj.model[0], &model[0][0], 16 * sizeof(float));
    device->UpdateUniformBuffer(uniformBuffer1, cpuBuffer3);

    device->_backend->BindProgram(program);
    device->_backend->BindTexture(diffuseTexture, 0);
    device->_backend->BindTexture(normalTexture, 1);
    device->_backend->BindTexture(diffuseTexture, 0);
    int i = 0;
    for (const auto &draw : drawInputs) {
      device->_backend->Draw(draw, nbElements[i], 1, uniformBuffers, 3);
      i++;
    }

    caca += 0.02f;
    device->RequestAnimationFrame();
  }
}
