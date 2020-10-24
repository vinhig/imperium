//
// Created by vinhi on 23/10/2020.
//

#include "Game.h"

#include <stdexcept>

void Game::SysLogicalUpdate() {
  LogicalUpdate(_tryHarder);
  _tryHarder->Do();
  _tryHarder->Wait();
}

void Game::SysResourceUpdate() { ResourceUpdate(_device); }

void Game::SysLoad() {
  // TODO: Load LOADING PLS WAIT texture

  Load(_device);
}

Game::Game(Device *device) {
  if (device) {
    _device = device;
  } else {
    throw std::runtime_error(
        "Couldn't not create a Game with a corrupted device.");
  }
  _tryHarder = new TryHarder();
}

void Game::Run() {
  SysLoad();
  while (!_device->ShouldClose()) {
    _device->Clear(RenderTarget{});
    SysLogicalUpdate();
    SysResourceUpdate();
    Draw(_device);
    _device->RequestAnimationFrame();
  }
}
