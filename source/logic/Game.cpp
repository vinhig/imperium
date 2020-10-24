//
// Created by vinhi on 23/10/2020.
//

#include "Game.h"

#include <cassert>
#include <stdexcept>

#include "CMeshInstance.h"

void Game::SysLogicalUpdate() {
  // 1 is a logically updatable uuid
  for (int i = 0; i < _system->Components(1).size(); ++i) {
    auto comp = (CTransform*)_system->Components(1)[i];
    comp->UpdateMvp();
  }
  LogicalUpdate(_tryHarder);
  _tryHarder->Do();
  _tryHarder->Wait();
}

void Game::SysResourceUpdate() {
  // 1 is a resource updatable uuid
  for (int i = 0; i < _system->Components(1).size(); ++i) {
    auto comp = (CTransform*)_system->Components(1)[i];
    auto res = comp->Resource();
    if (res.enable) {
      _device->UpdateUniformBuffer(res.dest, res.from);
    }
  }
  ResourceUpdate(_device);
}

void Game::SysLoad() {
  // TODO: Load LOADING PLS WAIT texture
  // Dummy shader
  basic = _device->CreateProgram("basic");
  _device->BindProgram(basic);
  Load(_device);
}

void Game::SysDraw() {
  // 2 is a drawable uuid
  for (int i = 0; i < _system->Components(2).size(); ++i) {
    auto comp = _system->Components(2)[i];
    auto draw = ((CMeshInstance*)comp)->Draw();
    assert(draw.nbResources != 0);
    for (int j = 0; j < draw.nbResources; j++) {
      _device->Draw(draw.drawInputs[j], draw.counts[j], draw.times,
                    draw.uniforms, draw.nbUniforms);
    }
  }
  Draw(_device);
}

Game::Game(Device* device) {
  if (device) {
    _device = device;
  } else {
    throw std::runtime_error(
        "Couldn't not create a Game with a corrupted device.");
  }
  _tryHarder = new TryHarder();
  _system = new System(_device);
}

void Game::Run() {
  SysLoad();
  int frame = 0;
  while (!_device->ShouldClose()) {
    _device->Clear(RenderTarget{});
    SysLogicalUpdate();
    SysResourceUpdate();
    SysDraw();
    _device->RequestAnimationFrame();
    frame += 1;
  }
}

System* Game::GetSystem() { return _system; }