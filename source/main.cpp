//
// Created by vinhi on 24/10/2020.
//

#include <iostream>

#include "MyGame.h"
#include "renderer/DeviceDesktop.h"

int main(int argc, char** argv) {
  auto device = new DeviceDesktop({1920, 1080, ApiDesc::OpenGL33});

  auto myGame = new MyGame(device);

  while (!myGame->ShouldClose()) {
    myGame->Process();
  }
}
