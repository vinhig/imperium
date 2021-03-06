//
// Created by vincent on 8/10/20.
//

#include "DeviceAndroid.h"

#include <stdexcept>
#include <utility>

#include "BackendOglEs.h"

DeviceAndroid::DeviceAndroid(ApiDesc apiDesc, int width, int height) {
  if (apiDesc == ApiDesc::OpenGLES32) {
    _backend = new BackendOglEs({width, height});
  } else {
    throw std::runtime_error("Incompatible API backend");
  }

  _width = width;
  _height = height;
}

void DeviceAndroid::Clear(RenderTarget renderTarget) { _backend->Clear(0); }

bool DeviceAndroid::ShouldClose() { return false; }

void DeviceAndroid::RequestAnimationFrame() {}

GPUBuffer DeviceAndroid::CreateVertexBuffer(CPUBuffer<float> cpuBuffer) {
  // Describe and create buffer
  BufferCreationDesc desc = {};
  desc.purpose = BufferTypeDesc::VertexBuffer;
  desc.usage = BufferUsageDesc::StaticDraw;
  desc.size = sizeof(float) * cpuBuffer.stride * cpuBuffer.nbElements;
  desc.data = (void *)cpuBuffer.data;
  desc.stride = sizeof(float) * cpuBuffer.stride;

  auto buffer = _backend->CreateBuffer(desc);

  return buffer;
}

GPUBuffer DeviceAndroid::CreateIndexBuffer(CPUBuffer<int> cpuBuffer) {
  // Describe and create buffer
  BufferCreationDesc desc = {};
  desc.purpose = BufferTypeDesc::IndexBuffer;
  desc.usage = BufferUsageDesc::StaticDraw;
  desc.size = sizeof(int) * cpuBuffer.nbElements;
  desc.data = (void *)cpuBuffer.data;
  desc.stride = sizeof(int) * cpuBuffer.stride;

  auto buffer = _backend->CreateBuffer(desc);
  return buffer;
}

GPUBuffer DeviceAndroid::CreateUniformBuffer(CPUBuffer<void> cpuBuffer) {
  // Describe and create buffer
  BufferCreationDesc desc = {};
  desc.purpose = BufferTypeDesc::UniformBuffer;
  desc.usage = BufferUsageDesc::DynamicDraw;
  desc.size = sizeof(int) * cpuBuffer.nbElements;
  desc.data = (void *)cpuBuffer.data;

  auto buffer = _backend->CreateBuffer(desc);
  return buffer;
}

GPUProgram DeviceAndroid::CreateProgram(std::string name) {
  // Read shaders spirv
  auto fragmentSource = _fileReader(name + ".frag.glsl.spv");
  auto vertexSource = _fileReader(name + ".vert.glsl.spv");

  // Convert to uint32_t array
  // Convert to uint32_t array
  std::vector<uint32_t> vertexSpirv;
  vertexSpirv.reserve(vertexSource.size() / 4);
  for (int i = 0; i < vertexSource.size(); i += 4) {
    uint32_t num = (uint32_t)vertexSource[i + 0] << 24 |
                   (uint32_t)vertexSource[i + 1] << 16 |
                   (uint32_t)vertexSource[i + 2] << 8 |
                   (uint32_t)vertexSource[i + 3];
    vertexSpirv.push_back(num);
  }
  std::vector<uint32_t> fragmentSpirv;
  fragmentSpirv.reserve(fragmentSource.size() / 4);
  for (int i = 0; i < fragmentSource.size(); i += 4) {
    uint32_t num = (uint32_t)fragmentSource[i + 0] << 24 |
                   (uint32_t)fragmentSource[i + 1] << 16 |
                   (uint32_t)fragmentSource[i + 2] << 8 |
                   (uint32_t)fragmentSource[i + 3];
    fragmentSpirv.push_back(num);
  }

  // Spirv is ready to be compiled by the backend
  auto program = _backend->CreateProgram(vertexSpirv, fragmentSpirv);

  return {program};

  return {0};
}

GPUDrawInput DeviceAndroid::CreateDrawInput(
    GPUInputLayout inputLayout, const std::vector<GPUBuffer> &vertexBuffers,
    GPUBuffer indexBuffer) {
  return _backend->CreateDrawInput(inputLayout, vertexBuffers, indexBuffer);
}

GPUInputLayout DeviceAndroid::CreateInputLayout(
    InputLayoutDesc inputLayoutDesc) {
  return _backend->CreateInputLayout(inputLayoutDesc);
}

GPUTexture DeviceAndroid::CreateEmptyTexture(TextureFormat format,
                                             TextureWrap wrap, int width,
                                             int height) {
  // Call the same backend method as CreateTextureFromData
  // but pass a null pointer, forcing it to upload nothing
  TextureCreationDesc textureCreationDesc = {};
  textureCreationDesc.data = nullptr;
  textureCreationDesc.width = width;
  textureCreationDesc.height = height;
  textureCreationDesc.format = format;
  textureCreationDesc.wrap = wrap;

  return _backend->CreateTexture(textureCreationDesc);
}

GPUTexture DeviceAndroid::CreateTextureFromData(CPUTexture cpuTexture) {
  // By creating a texture from data we assume that user load an image from
  // disk. Therefore, some value are set by default as the texture will be used
  // to color some meshes.
  TextureCreationDesc textureCreationDesc = {};
  textureCreationDesc.data = cpuTexture.data;
  textureCreationDesc.width = cpuTexture.width;
  textureCreationDesc.height = cpuTexture.height;
  textureCreationDesc.format = cpuTexture.format;
  textureCreationDesc.wrap = TextureWrap::Repeat;

  return _backend->CreateTexture(textureCreationDesc);
}

void DeviceAndroid::UpdateUniformBuffer(GPUBuffer buffer,
                                        CPUBuffer<void> newData) {
  // Describe how to modify
  BufferUpdateDesc updateDesc = {};
  updateDesc.buffer = &buffer;
  updateDesc.data = newData.data;
  updateDesc.size = newData.size;
  updateDesc.offset = 0;

  _backend->UpdateBuffer(updateDesc);
}

void DeviceAndroid::BindProgram(GPUProgram program) {
  _backend->BindProgram(program);
}

void DeviceAndroid::BindTextures(GPUTexture *textures, int nbTextures) {
  for (int i = 0; i < nbTextures; i++) {
    _backend->BindTexture(textures[i], i);
  }
}

void DeviceAndroid::Draw(GPUDrawInput drawInput, int count, int times,
                         GPUBuffer *uniformBuffers, size_t nbUniformBuffers) {
  _backend->Draw(drawInput, count, times, uniformBuffers, nbUniformBuffers);
}

GPUProgram DeviceAndroid::GetCurrentProgram() {
  return _currentProgram;
}


void DeviceAndroid::SetFileReader(
    std::function<std::vector<unsigned char>(std::string)> fileReader) {
  _fileReader = std::move(fileReader);
}
