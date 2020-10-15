//
// Created by vincent on 2/10/20.
//

#include <fstream>
#include <iostream>
#include <string>

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

  float vertices[] = {
      .5f,  .5f,  .5f,  -.5f, .5f,  .5f,
      -.5f, -.5f, .5f,  .5f,  -.5f, .5f,  // v0,v1,v2,v3 (front)
      .5f,  .5f,  .5f,  .5f,  -.5f, .5f,
      .5f,  -.5f, -.5f, .5f,  .5f,  -.5f,  // v0,v3,v4,v5 (right)
      .5f,  .5f,  .5f,  .5f,  .5f,  -.5f,
      -.5f, .5f,  -.5f, -.5f, .5f,  .5f,  // v0,v5,v6,v1 (top)
      -.5f, .5f,  .5f,  -.5f, .5f,  -.5f,
      -.5f, -.5f, -.5f, -.5f, -.5f, .5f,  // v1,v6,v7,v2 (left)
      -.5f, -.5f, -.5f, .5f,  -.5f, -.5f,
      .5f,  -.5f, .5f,  -.5f, -.5f, .5f,  // v7,v4,v3,v2 (bottom)
      .5f,  -.5f, -.5f, -.5f, -.5f, -.5f,
      -.5f, .5f,  -.5f, .5f,  .5f,  -.5f  // v4,v7,v6,v5 (back)
  };

  int indices[] = {
      0,  1,  2,  2,  3,  0,   // v0-v1-v2, v2-v3-v0 (front)
      4,  5,  6,  6,  7,  4,   // v0-v3-v4, v4-v5-v0 (right)
      8,  9,  10, 10, 11, 8,   // v0-v5-v6, v6-v1-v0 (top)
      12, 13, 14, 14, 15, 12,  // v1-v6-v7, v7-v2-v1 (left)
      16, 17, 18, 18, 19, 16,  // v7-v4-v3, v3-v2-v7 (bottom)
      20, 21, 22, 22, 23, 20   // v4-v7-v6, v6-v5-v4 (back)
  };
  //  float vertices[] = {
  //      0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
  //  };
  //
  //  int indices[] = {0, 1, 2};

  struct Material {
    float colors[4];
  };

  Material material = {{0.2, 0.3, 0.4, 1.0}};

  // Create vertex buffer
  CPUBuffer<float> cpuBuffer1 = {};
  cpuBuffer1.data = &vertices[0];
  cpuBuffer1.nbElements = 72;
  cpuBuffer1.stride = 3;
  CPUBuffer<int> cpuBuffer2 = {};
  cpuBuffer2.data = &indices[0];
  cpuBuffer2.nbElements = 36;
  cpuBuffer2.stride = 3;
  CPUBuffer<void> cpuBuffer3 = {};
  cpuBuffer3.data = (void *)&material;
  cpuBuffer3.size = sizeof(Material);

  // Create data to draw
  auto vertexBuffer = device->CreateVertexBuffer(cpuBuffer1);
  auto indexBuffer = device->CreateIndexBuffer(cpuBuffer2);
  auto uniformBuffer = device->CreateUniformBuffer(cpuBuffer3);

  std::cout << "Oh my gcc, " << vertexBuffer.buffer << " " << indexBuffer.buffer
            << " " << uniformBuffer.buffer << std::endl;

  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back(
      {0, 3, false, sizeof(float) * 3, DataType::Float, nullptr});
  auto inputLayout = device->CreateInputLayout(inputLayoutDesc);

  // Specify what to draw
  std::vector<GPUBuffer> buffers(1);
  buffers[0] = vertexBuffer;
  auto drawInput = device->CreateDrawInput(inputLayout, buffers, indexBuffer);

  while (!device->ShouldClose()) {
    device->Clear(RenderTarget{});
    device->_backend->BindProgram(program);
    device->_backend->Draw(drawInput, 36, 1, &uniformBuffer, 1);
    device->RequestAnimationFrame();
  }
}
