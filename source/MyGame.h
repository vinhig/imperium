//
// Created by vinhig on 15.01.2021.
//

#pragma once

#include "logic/Game.h"
#include "render/frontend/Mesh.h"

namespace Imperium::Render {
class Device;
}

class MyGame : public Imperium::Logic::Game {
 private:
  Imperium::Render::Frontend::Mesh triangle;

 public:
  explicit MyGame(Imperium::Render::Device* device)
      : Imperium::Logic::Game(device) {}

  void Load() override { triangle = LoadModel().Value(); };

  void ResourceUpdate(Imperium::Render::Device* device) override{};

  void Draw(Imperium::Render::Device* device) override{};

  ~MyGame() { triangle.Destroy(); }
};
