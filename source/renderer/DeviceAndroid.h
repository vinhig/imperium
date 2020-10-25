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
  std::function<std::vector<unsigned char>(std::string)> _fileReader;

  GPUProgram _currentProgram;

 public:
  explicit DeviceAndroid(ApiDesc apiDesc, int width, int height);
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
  GPUTexture CreateEmptyTexture(TextureFormat format, TextureWrap wrap,
                                int width, int height) override;
  GPUTexture CreateTextureFromData(CPUTexture cpuTexture) override;

  void UpdateUniformBuffer(GPUBuffer buffer, CPUBuffer<void> newData) override;

  void BindProgram(GPUProgram program) override;
  void Draw(GPUDrawInput drawInput, int count, int times,
            GPUBuffer *uniformBuffers, size_t nbUniformBuffers) override;

  GPUProgram GetCurrentProgram() override;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
  void SetFileReader(
      std::function<std::vector<unsigned char>(std::string)> fileReader);

  Backend *_backend;
};
