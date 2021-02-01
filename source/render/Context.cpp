//
// Created by vinhig on 15.01.2021.
//

#include "render/Context.h"

#include <cstdio>

#include "render/Device.h"
#include "render/backend/BackendVulkan.h"
#include "render/frontend/Mesh.h"
#include "render/frontend/Texture.h"

namespace Imperium::Render {
Context::Context(API api, int width, int height) {
  _api = api;
  _width = width;
  _height = height;
}

void Context::Init(Device* device) {
  _device = device;
  switch (_api) {
    case API::Vulkan:
      _backend = new Backend::BackendVulkan(device);
      break;

    case API::OpenGL33:
      printf("Unsupported backend: OpenGL33.\n");
      return;

    case API::OpenGL46:
      printf("Unsupported backend: OpenGL46.\n");
      return;
  }

  auto mainPipeline = _backend->CreatePipeline(Backend::PipelineType::Graphics);

  if (!mainPipeline.HasValue()) {
    printf("Main pipeline failed to be created.\n");
    return;
  }

  _mainPipeline = mainPipeline.Value();

  // Everything is good
  _failed = false;
}

void Context::BeginFrame() {
  _backend->BeginFrame();
  _backend->BindRenderpass(0);
  _backend->BeginPipeline(_mainPipeline);
}

void Context::EndFrame() {
  _backend->EndPipeline(_mainPipeline);
  _backend->EndFrame();
}

Frontend::Texture* Context::CreateTexture(CPUTexture texture) {
  return nullptr;
}

Frontend::Mesh* Context::CreateMesh(CPUBuffer<Vertex> vertices,
                                    CPUBuffer<unsigned int> indices) {
  auto mesh = new Frontend::Mesh;
  mesh->backend = _backend;
  mesh->vertices = vertices;
  mesh->indices = indices;

  mesh->Create();

  return mesh;
}

Context::~Context() {
  printf("~Context()\n");
  delete _backend;
}
}  // namespace Imperium::Render