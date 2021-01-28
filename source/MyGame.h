//
// Created by vinhig on 15.01.2021.
//

#pragma once

#include "logic/Game.h"
#include "render/frontend/Mesh.h"
#include "render/frontend/Model.h"

namespace Imperium::Render {
class Device;
}

class MyGame : public Imperium::Logic::Game {
 private:
  Imperium::Render::Frontend::Model* triangle;

 public:
  explicit MyGame(Imperium::Render::Device* device)
      : Imperium::Logic::Game(device) {}

  void Load() override {
    auto path =
        "/home/vincent/Projects/glTF-Sample-Models/2.0/Avocado/glTF/"
        "Avocado.gltf";
    triangle = LoadModel(path).Value();
    // triangle = LoadTriangle().Value();
  };

  void ResourceUpdate(Imperium::Render::Device* device) override{};

  void Draw(Imperium::Render::Device* device) override { triangle->Draw(); };

  ~MyGame() { triangle->Destroy(); }
};
