//
// Created by vinhig on 14.01.2021.
//

#pragma once

#include "render/Device.h"

struct SDL_Window;

namespace Imperium::Render {

class Context;

/**
 * Device to use on desktop platform (GNU/Linux and Windows). Basically a
 * wrapper around SDL2.
 */
class DeviceDesktop : public Device {
 private:
  SDL_Window* _window{nullptr};
  bool _failed{true};
  bool _shouldClose{false};
  Context* _context{nullptr};

 public:
  explicit DeviceDesktop(Context* context);
  ~DeviceDesktop();

  void PollEvents() override;

  /**
   * Something went wrong during initialisation of this device. Nothing should
   * be expected from a Device that failed its creation.
   * @return Device failed to be created (false = success, true = problem
   * encountered :-/).
   */
  bool Failed() const { return _failed; };

  bool ShouldClose() const override { return _shouldClose; };

  DeviceWindow Window() const override;

  Context* GetContext() const override;
};

}  // namespace Imperium::Render