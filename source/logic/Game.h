//
// Created by vinhi on 24.10.2020.
//

#pragma once

namespace Imperium::Render {
class Device;
}

namespace Imperium::Logic {

class Game {
 private:
  Render::Device* _device;

  void SysDraw();

 public:
  Game(Render::Device* device);
  ~Game();
  virtual void Load(Render::Device* device) = 0;
  virtual void ResourceUpdate(Render::Device* device) = 0;
  virtual void Draw(Render::Device* device) = 0;

  void Process();

  bool ShouldClose();
};

}  // namespace Imperium::Logic