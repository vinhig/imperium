//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include "logic/Game.h"

namespace Imperium::Render {
  class Device;
}

using namespace Imperium::Logic;
using namespace Imperium::Render;

class MyGame : public Game {
 private:

 public:
  explicit MyGame(Device* device) : Game(device) {}

  void Load(Device* device) override {};

  void ResourceUpdate(Device* device) override{};

  void Draw(Device* device) override{};

};
