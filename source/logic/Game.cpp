//
// Created by vinhi on 23/10/2020.
//

#include "Game.h"

#include <cassert>
#include <stdexcept>

#include "CCamera.h"
#include "CDirectionalLight.h"
#include "CMeshInstance.h"
#include "CViewport.h"
#include "CRigidBody.h"

void Game::SysLogicalUpdate() {
  // TODO: looks like a fucking hack
  _system->UpdatePhysics();
  // TODO: we should feed a tryharder instead of calling updatable components
  // 1 is a logically updatable uuid
  for (int i = 0; i < _system->Components(1).size(); ++i) {
    auto comp = (CTransform*)_system->Components(1)[i];
    comp->UpdateMvp();
  }
  // 5 is a logically updatable uuid
  for (int i = 0; i < _system->Components(5).size(); ++i) {
    auto comp = (CCamera*)_system->Components(5)[i];
    comp->UpdateVp();
  }
  // 6 is a logically updatable uuid
  for (int i = 0; i < _system->Components(6).size(); ++i) {
    auto comp = (CCamera*)_system->Components(6)[i];
    comp->UpdateVp();
  }
  // 7 is a logically updatable uuid
  for (int i = 0; i < _system->Components(7).size(); ++i) {
    auto comp = (CRigidBody*)_system->Components(7)[i];
    comp->SyncBody();
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
  // 5 is a resource updatable uuid
  for (int i = 0; i < _system->Components(5).size(); ++i) {
    auto comp = (CCamera*)_system->Components(5)[i];
    auto res = comp->Resource();
    if (res.enable) {
      _device->UpdateUniformBuffer(res.dest, res.from);
    }
  }
  // 6 is a resource updatable uuid
  for (int i = 0; i < _system->Components(6).size(); ++i) {
    auto comp = (CCamera*)_system->Components(6)[i];
    auto res = comp->Resource();
    if (res.enable) {
      _device->UpdateUniformBuffer(res.dest, res.from);
    }
  }
  ResourceUpdate(_device);
}

void Game::SysLoad() {
  // Load rendering graph
  _frame = new Frame(_device, "../assets/forward.toml");
  // TODO: Load LOADING PLS WAIT texture
  // Dummy shader
  // Has to be used to create input layout with the directx backend
  _basic = _device->CreateProgram("basic");
  _device->BindProgram(_basic);
  Load(_device);
}

void Game::SysDraw() {
  // 2 is a drawable uuid
  /*
  for (int i = 0; i < _system->Components(2).size(); ++i) {
    auto comp = _system->Components(2)[i];
    auto draw = ((CMeshInstance*)comp)->Draw();
    assert(draw.nbResources != 0);
    _device->BindTextures(draw.textures, draw.nbTextures);
    for (int j = 0; j < draw.nbResources; j++) {
      _device->Draw(draw.drawInputs[j], draw.counts[j], draw.times,
                    draw.uniforms, draw.nbUniforms);
    }
  }
   */
  // Get point of view
  auto mainCamera = _system->GetFirstActive<CCamera>();
  if (mainCamera == nullptr) {
    throw std::runtime_error("Game must provide a CCamera.");
  }
  _frame->SetPointOfView(mainCamera->GetGPUBuffer(), "main_camera");
  // Get light view
  auto light = _system->GetFirstActive<CDirectionalLight>();
  if (light == nullptr) {
    throw std::runtime_error("Game must provide a CDirectional light.");
  }
  _frame->SetLightView(light->GetGPUBuffer());

  // Iterate through all drawable components
  // 2 is CMeshInstance so drawable
  for (int i = 0; i < _system->Components(2).size(); i++) {
    auto comp = _system->Components(2)[i];
    auto draw = ((CMeshInstance*)comp)->Draw();
    // Send this draw call to frame
    // It's up to Frame to decide where and when to draw
    _frame->RegisterDrawCall(draw);
  }

  for (int i = 0; i < _system->Components(4).size(); i++) {
    auto comp = _system->Components(4)[i];
    auto draw = ((CViewport*)comp)->Draw();
    // Send this draw call to frame
    // It's up to Frame to decide where and when to draw
    _frame->RegisterDrawCall(draw);
  }
  Draw(_device);
  _frame->Commit(_device);
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

bool Game::ShouldClose() { return _device->ShouldClose(); }

void Game::Process() {
  if (frame == 0) {
    SysLoad();
  }
  _device->Clear(GPURenderTarget{});
  SysLogicalUpdate();
  SysResourceUpdate();
  SysDraw();
  _device->RequestAnimationFrame();
  frame++;
}

System* Game::GetSystem() { return _system; }