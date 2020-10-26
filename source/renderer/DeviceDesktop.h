//
// Created by vincent on 8/10/20.
//

#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "Backend.h"
#include "Descriptions.h"
#include "Device.h"

#include <cstdio>
#include <functional>
#include <string>

class DeviceDesktop : public Device {
 private:
  GLFWwindow* _window;
  std::function<std::string(std::string)> _fileReader;
  ApiDesc _api;

  GPUProgram _currentProgram;

  int _width = 1024;
  int _height = 768;

 public:
  explicit DeviceDesktop(DeviceDesc deviceDesc);
  ~DeviceDesktop();

  int GetWidth() override { return _width; };
  int GetHeight() override { return _height; };

  void Clear(GPURenderTarget renderTarget) override;
  GPUBuffer CreateVertexBuffer(CPUBuffer<float> cpuBuffer) override;
  GPUBuffer CreateIndexBuffer(CPUBuffer<int> cpuBuffer) override;
  GPUBuffer CreateUniformBuffer(CPUBuffer<void> cpuBuffer) override;
  GPUProgram CreateProgram(std::string name) override;
  GPUDrawInput CreateDrawInput(GPUInputLayout inputLayout,
                               const std::vector<GPUBuffer>& vertexBuffers,
                               GPUBuffer indexBuffer) override;
  GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) override;
  GPUTexture CreateEmptyTexture(TextureFormat format, TextureWrap wrap,
                                int width, int height) override;
  GPUTexture CreateTextureFromData(CPUTexture cpuTexture) override;
  GPURenderTarget CreateRenderTarget(int nbColors, bool depth) override;

  void UpdateUniformBuffer(GPUBuffer buffer, CPUBuffer<void> newData) override;

  void BindProgram(GPUProgram program) override;
  void BindTextures(GPUTexture* textures, int nbTextures) override;

  void Draw(GPUDrawInput drawInput, int count, int times,
            GPUBuffer* uniformBuffers, size_t nbUniformBuffers) override;

  GPUProgram GetCurrentProgram() override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
  Backend* _backend;
};