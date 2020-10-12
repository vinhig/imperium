//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdio>
#include <functional>
#include <string>

#include "Backend.h"
#include "Descriptions.h"
#include "Device.h"

class DeviceAndroid : public Device {
 private:
  Backend* _backend;
  std::function<std::string(std::string)> _fileReader;

 public:
  explicit DeviceAndroid(ApiDesc apiDesc);
  ~DeviceAndroid() = default;

  void Clear(RenderTarget renderTarget) override;
  uint32_t CreateVertexBuffer(CPUBuffer<float> buffer) override;
  uint32_t CreateIndexBuffer(CPUBuffer<int> buffer) override;
  uint32_t CreateUniformBuffer(CPUBuffer<void> buffer) override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;

  void SetFileReader(std::function<const char*(std::string)> fileReader) override;
};
