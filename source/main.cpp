//
// Created by vincent on 2/10/20.
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
  auto meshMerger = new MeshMerger();
  auto robot = meshLoader->Load("../assets/robot.fbx");

  CPUBuffer<float> vertices = {};
  CPUBuffer<int> indices = {};
  meshMerger->Merge(robot, &vertices, &indices);
  std::cout << "SSize: " << vertices.nbElements << std::endl;

  // Create data to draw
  auto vertexBuffer = device->CreateVertexBuffer(vertices);
  auto indexBuffer = device->CreateIndexBuffer(indices);

  // Not very clean; but we have to know how many indices to draw
  auto nbIndices = indices.nbElements;

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
    float texture;
    float dumb;  // TODO: opengl glsl compiler seems to convert this to vec3,
                 // therefore we're in a stride of 4, so if we do not have this
                 // dumb variable, nothing work
  };

  Material materials[3] = {
      {1.0, 1.0, 1.0},
      {1.0, 1.0, 0.0},
      {1.0, 1.0, 0.0}
  };

  CPUBuffer<void> cpuBuffer5 = {};
  cpuBuffer5.data = (void *)&materials;
  cpuBuffer5.size = sizeof(Material[3]);
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
  inputLayoutDesc.entries.push_back({3, 1, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 8)});

  auto inputLayout = device->CreateInputLayout(inputLayoutDesc);

  // Specify what to draw
  std::vector<GPUBuffer> buffers(4);
  buffers[0] = vertexBuffer;
  buffers[1] = vertexBuffer;
  buffers[2] = vertexBuffer;
  buffers[3] = vertexBuffer;
  auto drawInput = device->CreateDrawInput(inputLayout, buffers, indexBuffer);

  // Let's light it up
  // - Lux, 2020
  struct Lights {
    float camera[4];  // vec3 have the same width as vec4
    float positions[3];
  };

  Lights lights = {};

  lights.camera[0] = 3.0f;
  lights.camera[1] = 3.0f;
  lights.camera[2] = 3.0f;

  lights.positions[0] = 0.0f;
  lights.positions[1] = 0.0f;
  lights.positions[2] = 1.0f;

  CPUBuffer<void> cpuBuffer4 = {};
  cpuBuffer4.data = (void *)&lights;
  cpuBuffer4.size = sizeof(Lights);

  auto uniformBuffer2 = device->CreateUniformBuffer(cpuBuffer4);

  GPUBuffer uniformBuffers[3] = {uniformBuffer1, uniformBuffer2,
                                 uniformBuffer3};

  // Some fucking good texturing
  auto textureLoader = new TextureLoader();
  // Diffuse stuff
  auto diffuseCpuTexture = textureLoader->Load("../assets/Robots.png");
  auto diffuseTexture = device->CreateTextureFromData(diffuseCpuTexture);
  textureLoader->Link("../assets/Robots.png", diffuseTexture.texture);
  // Normal stuff
  auto normalCpuTexture = textureLoader->Load("../assets/Wheels.png");
  auto normalTexture = device->CreateTextureFromData(normalCpuTexture);
  textureLoader->Link("../assets/Wheels.png", normalTexture.texture);

  float caca = 0.0f;
  while (!device->ShouldClose()) {
    device->Clear(RenderTarget{});
    // Update light position
    /*lights.positions[0] = cos(caca) * 4;
    lights.positions[1] = sin(caca) * 4;
    lights.positions[2] = sin(caca) * 4;
    device->UpdateUniformBuffer(uniformBuffer2, cpuBuffer4);*/

    model = glm::rotate(caca, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::scale(glm::vec3(0.01f, 0.01f, 0.01f));
    mvp = projection * view * model;
    memcpy(&obj.mvp[0], &mvp[0][0], 16 * sizeof(float));
    memcpy(&obj.model[0], &model[0][0], 16 * sizeof(float));
    device->UpdateUniformBuffer(uniformBuffer1, cpuBuffer3);

    device->_backend->BindProgram(program);
    device->_backend->BindTexture(normalTexture, 0);
    device->_backend->BindTexture(diffuseTexture, 1);
    // device->_backend->BindTexture(normalTexture, 2);
    device->_backend->Draw(drawInput, nbIndices, 1, uniformBuffers, 3);

    caca += 0.02f;
    device->RequestAnimationFrame();
  }
}
