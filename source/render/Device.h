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

/**
 * Abstract class defining what methods a proper device should implement. A
 * device interacts with a specific platform by creating a window and managing
 * inputs.
 */
class Device {
 public:
  /**
   * Active window of this device.
   * @return Active window.
   */
  virtual DeviceWindow Window() const = 0;
  virtual void PollEvents() = 0;
  virtual bool ShouldClose() const = 0;

  /**
   * Get underlying context. Context is often passed to devices implementing
   * abstract `Device` class with specific constructor.
   * @return Underlying context.
   */
  virtual Context* GetContext() const = 0;
};

}  // namespace Imperium::Render