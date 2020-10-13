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

 public:
  explicit DeviceDesktop(DeviceDesc deviceDesc);
  ~DeviceDesktop();

  void Clear(RenderTarget renderTarget) override;
  GPUBuffer CreateVertexBuffer(CPUBuffer<float> cpuBuffer) override;
  GPUBuffer CreateIndexBuffer(CPUBuffer<int> cpuBuffer) override;
  GPUBuffer CreateUniformBuffer(CPUBuffer<void> cpuBuffer) override;
  GPUProgram CreateProgram(std::string name) override;
  GPUDrawInput CreateDrawInput(
      InputLayoutDesc inputLayoutDesc) override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
  Backend* _backend;
};