//
// Created by vinhig on 24.10.2020.
//

#pragma once

#include "core/Option.h"

namespace Imperium::Loader {
class Loader;
}

namespace Imperium::Render {
class Device;
namespace Frontend {
class Mesh;
class Model;
}
}  // namespace Imperium::Render

namespace Imperium::Logic {

class Game {
 private:
  Render::Device* _device{nullptr};
  Loader::Loader* _loader{nullptr};

  void SysDraw();

 public:
  Game(Render::Device* device);
  ~Game();
  virtual void Load() = 0;
  virtual void ResourceUpdate(Render::Device* device) = 0;
  virtual void Draw(Render::Device* device) = 0;

  void Process();

  bool ShouldClose();

  Core::Option<Render::Frontend::Mesh*> LoadTriangle();
  Core::Option<Render::Frontend::Model*> LoadModel(const char* path);
};

}  // namespace Imperium::Logic