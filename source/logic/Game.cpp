#include "Game.h"

#include "loader/Loader.h"
#include "loader/Model.h"
#include "render/Context.h"
#include "render/Device.h"
#include "render/Resources.h"
#include "render/frontend/Mesh.h"

namespace Imperium::Logic {

Game::Game(Render::Device* device) { _device = device; }

Game::~Game() {}

void Game::Process() {
  _device->PollEvents();

  this->SysDraw();
}

void Game::SysDraw() {
  _device->GetContext()->BeginFrame();

  _device->GetContext()->EndFrame();
}

bool Game::ShouldClose() { return _device->ShouldClose(); }

Core::Option<Render::Frontend::Mesh> Game::LoadModel() {
  // Load from disk
  auto model = _loader->CreateModelTriangle().Value();

  // Use context to create frontend objects
  // Texture(s), Mesh(es)
  auto meshes = model->Meshes()[0].Value();

  // Okay we know that's a simple triangle
  Render::CPUBuffer<float> vertices = {};
  Render::CPUBuffer<int> indices = {};

  auto mesh = _device->GetContext()->CreateMesh(vertices, indices);
  return Core::Option<Render::Frontend::Mesh>(mesh);
}

}  // namespace Imperium::Logic