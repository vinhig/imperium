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

class DeviceDesktop : public Device {
 private:
  Backend* _backend;
  GLFWwindow* _window;

 public:
  explicit DeviceDesktop(DeviceDesc deviceDesc);
  ~DeviceDesktop();

  void Clear() override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
};