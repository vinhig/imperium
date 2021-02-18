//
// Created by vinhig on 15.01.2021.
//

#include "render/Context.h"

#include <mathfu/glsl_mappings.h>
#include <mathfu/matrix.h>

#include <cstdio>

#include "logic/Camera.h"
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

  // Create logical camera
  _cameraBuffer = _backend->CreateBuffer(Backend::BufferType::Uniform,
                                         Logic::Camera::Size());
  // Copy raw data to backend buffer to be used as a uniform buffer
  auto cameraPtr = _backend->MapBuffer(_cameraBuffer);
  _camera = new Logic::Camera({}, 70.0f, (float)Width() / (float)Height(),
                              0.01f, 100.0f);
  memcpy(cameraPtr, _camera->Data(), Logic::Camera::Size());
  _backend->UnmapBuffer(_cameraBuffer);

  // Create global descriptor layout
  Backend::DescriptorBinding bindings = {};
  bindings.bindingType = Backend::DescriptorBindingType::BufferBinding;
  bindings.buffer = _cameraBuffer;
  bindings.size = Logic::Camera::Size();
  auto descriptorLayout = _backend->CreateDescriptorLayout(1, &bindings);

  auto mainPipeline = _backend->CreatePipeline(Backend::PipelineType::Graphics,
                                               1, descriptorLayout);

  // Create global descriptor
  _globalDescriptor =
      _backend->CreateDescriptorSet(descriptorLayout, 1, &bindings);

  if (!mainPipeline.HasValue()) {
    printf("Main pipeline failed to be created.\n");
    return;
  }

  _mainPipeline = mainPipeline.Value();

  // Everything is good
  _failed = false;
}

void Context::BeginFrame() {
  /*auto perspective = mathfu::PerspectiveHelper(
      70.0f, (float)Width() / (float)Height(), 0.01f, 100.0f, -1.0f);
  auto lookAt = mathfu::LookAtHelper({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
                                     {0.0f, 1.0f, 0.0f}, -1.0f);*/

  auto rotation = mathfu::mat3::RotationX(M_PI) *
                  mathfu::mat3::RotationY(frame * 3.0f) *
                  mathfu::mat3::RotationZ(0.0f);

  auto model =
      mathfu::TransformHelper({0.0f, 0.0f, 0.0f}, rotation, {4.0f, 4.0f, 4.0f});

  /*auto mvp = perspective * lookAt * model;*/

  frame += 0.01;

  mathfu::vec4_packed packed[4];
  model.Pack(packed);

  _backend->BeginFrame();
  _backend->BindRenderpass(0);
  _backend->BeginPipeline(_mainPipeline);
  _backend->BindDescriptorSet(_mainPipeline, _globalDescriptor);
  _backend->BindUniform(0, _mainPipeline, 16 * 4, (void*)&packed->data_[0]);
  // _backend->BindConstantBuffer(0, _cameraBuffer);
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
  _backend->DeferDeletePipeline(_mainPipeline);
  _backend->DeferDeleteBuffer(_cameraBuffer);
  delete _camera;
  delete _backend;
}
}  // namespace Imperium::Render