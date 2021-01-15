#include "render/Context.h"

#include <cstdio>

#include "render/Device.h"
#include "render/backend/BackendVulkan.h"

namespace Imperium::Render {
Context::Context(API api, int width, int height) {
  _api = api;
  _width = width;
  _height = height;
}

void Context::Init(Device* device) {
  switch (_api) {
    case API::Vulkan:
      _failed = false;

      _backend = new Backend::BackendVulkan(device);
      break;

    case API::OpenGL33:
      printf("Unsupported backend: OpenGL33.\n");
      return;

    case API::OpenGL46:
      printf("Unsupported backend: OpenGL46.\n");
      return;
  }
}

void Context::BeginFrame() {
  _backend->BeginFrame();
}

void Context::EndFrame() {
  _backend->EndFrame();
}

Context::~Context() {
  printf("~Context()\n");
  delete _backend;
}
}  // namespace Imperium::Render