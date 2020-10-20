//
// Created by vincent on 2/10/20.
//

#include <fstream>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <string>

#include "loader/MeshLoader.h"
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
    float camera[4];  // vec3 have the same width as vec4
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

  // Some fucking good texturing
  auto textureLoader = new TextureLoader();
  // Diffuse stuff
  auto diffuseCpuTexture = textureLoader->Load("../assets/Diffuse.png");
  auto diffuseTexture = device->CreateTextureFromData(diffuseCpuTexture);
  textureLoader->Link("../assets/Diffuse.png", diffuseTexture.texture);
  // Normal stuff
  auto normalCpuTexture = textureLoader->Load("../assets/Normal.png");
  auto normalTexture = device->CreateTextureFromData(normalCpuTexture);
  textureLoader->Link("../assets/Normal.png", diffuseTexture.texture);

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
    device->_backend->BindTexture(diffuseTexture, 0);
    device->_backend->BindTexture(normalTexture, 1);
    device->_backend->Draw(drawInput, nbIndices, 1, uniformBuffers, 2);
    caca += 0.05f;
    device->RequestAnimationFrame();
  }
}
