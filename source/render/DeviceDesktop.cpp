//
// Created by vinhig on 15.01.2021.
//

#include "render/DeviceDesktop.h"

#include <SDL2/SDL.h>

#include "render/Context.h"

namespace Imperium::Render {
DeviceDesktop::DeviceDesktop(Context* context) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    // Failed to init SDL
    // Let's stop there
    printf("SDL Init failed.\n");
    return;
  }
  _context = context;
  _window =
      SDL_CreateWindow("Imperium Game Engine", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, context->Width(),
                       context->Height(), SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
  if (!_window) {
    printf("SDL Window failed to be created. %s\n", SDL_GetError());
    return;
  }
  _context->Init(this);
  if (_context->Failed()) {
    printf("Context failed to be intialized.\n");
    return;
  }
  _failed = false;
}

void DeviceDesktop::PollEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      _shouldClose = true;
    }
  }
}

DeviceWindow DeviceDesktop::Window() const {
  if (_context == nullptr) {
    printf("wtf\n");
  }
  DeviceWindow window;
  window.desktop = true;
  window.sdlWindow = _window;
  window.height = _context->Height();
  window.width = _context->Width();

  return window;
}

Context* DeviceDesktop::GetContext() const {
  return _context;
}

DeviceDesktop::~DeviceDesktop() {
  printf("~DeviceDesktop()\n");
  delete _context;

  SDL_DestroyWindow(_window);
}
}  // namespace Imperium::Render