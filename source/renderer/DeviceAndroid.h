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
  Backend *_backend;
  std::function<std::vector<unsigned char>(std::string)> _fileReader;

 public:
  explicit DeviceAndroid(ApiDesc apiDesc);
  ~DeviceAndroid() = default;

  void Clear(RenderTarget renderTarget) override;
  GPUBuffer CreateVertexBuffer(CPUBuffer<float> cpuBuffer) override;
  GPUBuffer CreateIndexBuffer(CPUBuffer<int> cpuBuffer) override;
  GPUBuffer CreateUniformBuffer(CPUBuffer<void> cpuBuffer) override;
  GPUProgram CreateProgram(std::string name) override;
  GPUDrawInput CreateDrawInput(GPUInputLayout inputLayout,
                               const std::vector<GPUBuffer> &vertexBuffers,
                               GPUBuffer indexBuffer) override;
  GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
  void SetFileReader(
      std::function<std::vector<unsigned char>(std::string)> fileReader);
};
