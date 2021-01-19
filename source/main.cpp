//
// Created by vinhig on 24.10.2020.
//

#include <iostream>

#include "MyGame.h"
#include "render/Context.h"
#include "render/DeviceDesktop.h"

using namespace Imperium::Render;

int main(int argc, char** argv) {
  auto context = new Context(API::Vulkan, 1024, 768);
  auto device = new DeviceDesktop(context);

  if (device->Failed()) {
    printf("Failed to create DesktopDevice.\n");
    return -1;
  } else {
    printf("DesktopDevice created successfully.\n");
  }

  auto myGame = new MyGame(device);

  while (!myGame->ShouldClose()) {
    myGame->Process();
  }

  delete device;
}
