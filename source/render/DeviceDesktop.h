//
// Created by vinhig on 14.01.2021.
//

#pragma once

#include "render/Device.h"

struct SDL_Window;

namespace Imperium::Render {

class Context;

class DeviceDesktop : public Device {
 private:
  SDL_Window* _window{nullptr};
  bool _failed{true};
  bool _shouldClose{false};
  Context* _context{nullptr};

 public:
  DeviceDesktop(Context* context);
  ~DeviceDesktop();

  void PollEvents();

  bool Failed() const { return _failed; };

  bool ShouldClose() const override { return _shouldClose; };

  DeviceWindow Window() const override;

  Context* GetContext() const override;
};

}  // namespace Imperium::Render