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

bool DeviceAndroid::ShouldClose() { return false; }

void DeviceAndroid::RequestAnimationFrame() {}

void DeviceAndroid::Clear() { _backend->Clear(0); }
