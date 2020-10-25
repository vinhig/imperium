//
// Created by vinhi on 24/10/2020.
//

#include <iostream>

#include "MyGame.h"
#include "renderer/DeviceDesktop.h"

// https://music.youtube.com/watch?v=3GicPBWG5EM&feature=share

int main(int argc, char** argv) {
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGL33});

  auto myGame = new MyGame(device);

  while(!myGame->ShouldClose()) {
    myGame->Process();
  }
}