//
// Created by vinhi on 29/10/2020.
//

#include "CViewport.h"

CViewport::CViewport(Entity* owner, void* args) : IComponent(owner) {
  auto device = GetEntity()->GetSystem()->GetDevice();
  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  auto program = GetEntity()->GetSystem()->GetDevice()->GetCurrentProgram();
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back({0, 2, false, sizeof(float) * 4,
                                     DataType::Float,
                                     (void*)(sizeof(float) * 0)});
  inputLayoutDesc.entries.push_back({1, 2, false, sizeof(float) * 4,
                                     DataType::Float,
                                     (void*)(sizeof(float) * 2)});

  auto inputLayout = device->CreateInputLayout(inputLayoutDesc);

  CPUBuffer<float> vertices = {};
  vertices.data = (float*)malloc(sizeof(float) * 16);
  vertices.nbElements = 4;
  vertices.stride = 4;
  vertices.data[0] = -1.0f;
  vertices.data[1] = 1.0f;
  vertices.data[2] = 0.0f;
  vertices.data[3] = 1.0f;

  vertices.data[4] = 1.0f;
  vertices.data[5] = 1.0f;
  vertices.data[6] = 1.0f;
  vertices.data[7] = 1.0f;

  vertices.data[8] = -1.0f;
  vertices.data[9] = -1.0f;
  vertices.data[10] = 0.0f;
  vertices.data[11] = 0.0f;

  vertices.data[12] = 1.0f;
  vertices.data[13] = -1.0f;
  vertices.data[14] = 1.0f;
  vertices.data[15] = 0.0f;
  _vertices = device->CreateVertexBuffer(vertices);

  CPUBuffer<int> indices = {};
  indices.data = (int*)malloc(sizeof(int) * 6);
  indices.nbElements = 6;
  indices.data[0] = 0;
  indices.data[1] = 1;
  indices.data[2] = 2;
  indices.data[3] = 1;
  indices.data[4] = 3;
  indices.data[5] = 2;
  _indices = device->CreateIndexBuffer(indices);

  _counts.push_back(6);

  std::vector<GPUBuffer> verticesBuffer(2);
  verticesBuffer[0] = _vertices;
  verticesBuffer[1] = _vertices;
  _drawInput = device->CreateDrawInput(inputLayout, verticesBuffer, _indices);

  SetLayer(Layer::C);
}

DrawCall CViewport::Draw() {
  // int count = 6;
  auto draw = DrawCall{&_drawInput,    nullptr, nullptr, 0,         0,
                       _counts.data(), 1,       1,       GetLayer()};

  return draw;
}
