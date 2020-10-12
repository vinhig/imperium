//
// Created by vincent on 8/10/20.
//

#include "DeviceAndroid.h"

#include <stdexcept>

#include "BackendOglEs.h"

DeviceAndroid::DeviceAndroid(ApiDesc apiDesc) {
  if (apiDesc == ApiDesc::OpenGLES32) {
    _backend = new BackendOglEs();
  } else {
    throw std::runtime_error("Incompatible API backend");
  }
}

void DeviceAndroid::Clear(RenderTarget renderTarget) { _backend->Clear(0); }

bool DeviceAndroid::ShouldClose() { return false; }

void DeviceAndroid::RequestAnimationFrame() {}

uint32_t DeviceAndroid::CreateVertexBuffer(CPUBuffer<float> buffer) {
  return 0;
}

uint32_t DeviceAndroid::CreateIndexBuffer(CPUBuffer<int> buffer) { return 0; }

uint32_t DeviceAndroid::CreateUniformBuffer(CPUBuffer<void> buffer) {
  return 0;
}

void DeviceAndroid::SetFileReader(
    std::function<const char*(std::string)> fileReader) {
  _fileReader = fileReader;
}
