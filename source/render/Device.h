//
// Created by vinhig on 14.01.2021.
//

struct SDL_Window;

namespace Imperium::Render {

class Context;

struct DeviceWindow {
  union {
    SDL_Window* sdlWindow;
  };
  int width;
  int height;
  bool desktop;
};

class Device {
 public:
  virtual DeviceWindow Window() const = 0;
  virtual void PollEvents() = 0;
  virtual bool ShouldClose() const = 0;

  virtual Context* GetContext() const = 0;
};

}  // namespace Imperium::Render