//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include "logic/Actions.h"
#include "logic/CMeshInstance.h"
#include "logic/CTransform.h"
#include "logic/Ecs.h"
#include "logic/Game.h"
#include "renderer/DeviceDesktop.h"

class MyGame : public Game {
 private:
  System* system;
  Entity* plant;
  // CTransform* _transform;
  // CMeshInstance* _mesh;
  float _caca = 0.0f;

 public:
  explicit MyGame(Device* device) : Game(device) {}

  void Load(Device* device) override {
    // _transform = new CTransform(nullptr, device);
    // _mesh = new CMeshInstance(nullptr, device, "../assets/indoor-plant.fbx");
    system = new System(device);
    plant = new Entity(system, 0);
    std::string path = "../assets/indoor-plant.fbx";
    auto mesh = plant->GetOrCreate<CMeshInstance>((void*)&path);
  };

  void LogicalUpdate(TryHarder* tryHarder) override {
    // _transform->SetRotation(glm::vec3(_caca, 0.0f, 0.0f));
    // tryHarder->Register(_transform->Logic());
    _caca += 0.01f;
  };

  void ResourceUpdate(Device* device) override{
      // auto resourceChange = _transform->Resource();
      // if (resourceChange.enable) {
      // device->UpdateUniformBuffer(resourceChange.dest, resourceChange.from);
      //}
  };

  void Draw(Device* device) override { system->Draw(device); };
};
