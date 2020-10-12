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
  Backend* _backend;
  GLFWwindow* _window;
  std::function<std::string(std::string)> _fileReader;

 public:
  explicit DeviceDesktop(DeviceDesc deviceDesc);
  ~DeviceDesktop();

  void Clear(RenderTarget renderTarget) override;
  GPUBuffer CreateVertexBuffer(CPUBuffer<float> buffer) override;
  GPUBuffer CreateIndexBuffer(CPUBuffer<int> buffer) override;
  GPUBuffer CreateUniformBuffer(CPUBuffer<void> buffer) override;
  GPUProgram CreateProgram(std::string name) override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
};