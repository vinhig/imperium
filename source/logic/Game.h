#pragma once

//
// Created by vinhig on 24.10.2020.
//

#include "core/Option.h"

namespace Imperium::Loader {
class Loader;
}

namespace Imperium::Render {
class Context;
namespace Frontend {
class Mesh;
class Model;
}  // namespace Frontend
}  // namespace Imperium::Render

namespace Imperium::Logic {

class Game {
 private:
  // Render::Device* _device{nullptr};
  Loader::Loader* _loader{nullptr};
  Render::Context* _context{nullptr};

  void SysDraw();

 public:
  Game(Render::Context* device);
  ~Game();
  /**
   * Called at the early beginning of the startup, just after the creation of
   * `Context`, `Device` and `Backend`.
   */
  virtual void Load() = 0;

  /**
   * Never called, don't use it atm.
   * @param device
   */
  virtual void ResourceUpdate(Render::Context* device) = 0;

  /**
   * Called one time per frame. Place your draw calls (whateverItIs->Draw())
   * there.
   * @param context Active context (high level object for resources
   * manipulation).
   */
  virtual void Draw(Render::Context* context) = 0;

  /**
   * To call one time per frame, tick all subsystems and swap buffers (this one
   * should be moved to another function).
   */
  void Process();

  /**
   * User or event requires the game to shut down.
   * @return Application should close.
   */
  bool ShouldClose();

  Core::Option<Render::Frontend::Mesh*> LoadTriangle();
  Core::Option<Render::Frontend::Model*> LoadModel(const char* path);
};

}  // namespace Imperium::Logic