//
// Created by vincent on 2/10/20.
//

#include <iostream>

#include "renderer/BackendOgl.h"
#include "renderer/DeviceDesktop.h"

// Entry point for desktop platform
int main(int argc, char **argv) {
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGLES32});

  while (!device->ShouldClose()) {
    device->Clear();
    device->RequestAnimationFrame();
  }
}