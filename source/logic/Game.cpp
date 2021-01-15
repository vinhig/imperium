#include "Game.h"

#include "render/Context.h"
#include "render/Device.h"

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
};

bool Game::ShouldClose() { return _device->ShouldClose(); }

}  // namespace Imperium::Logic