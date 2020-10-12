//
// Created by vincent on 2/10/20.
//

#include <fstream>
#include <iostream>
#include <string>

#include "renderer/BackendOgl.h"
#include "renderer/DeviceDesktop.h"

// Entry point for desktop platform
int main(int argc, char **argv) {
  // Create device
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGLES32});

  // Create a program
  // Some testing :)
  device->CreateProgram("basic");

  while (!device->ShouldClose()) {
    device->Clear(RenderTarget{});
    device->RequestAnimationFrame();
  }
}