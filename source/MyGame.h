//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include "logic/Actions.h"
#include "logic/CMeshInstance.h"
#include "logic/CTransform.h"
#include "logic/Ecs.h"
#include "logic/Game.h"

class MyGame : public Game {
 private:
  // CTransform* _transform;
  // CMeshInstance* _mesh;
  Entity* plant;
  float _caca = 0.0f;

 public:
  explicit MyGame(Device* device) : Game(device) {}

  void Load(Device* device) override {
    // _transform = new CTransform(nullptr, device);
    // _mesh = new CMeshInstance(nullptr, device, "../assets/indoor-plant.fbx");
    plant = new Entity(GetSystem(), 0);
#if __ANDROID__
    std::string path = "indoor-plant.fbx";
#else
    std::string path = "../assets/indoor-plant.fbx";
#endif
    auto mesh = plant->GetOrCreate<CMeshInstance>((void*)&path);
    plant->Get<CTransform>()->SetRotation(
        glm::vec3(-90.0f, 0.0f, 0.0f));
    plant->Get<CTransform>()->SetPosition(glm::vec3(0.0f, -1.5f, 0.0f));
    plant->Get<CTransform>()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
  };

  void LogicalUpdate(TryHarder* tryHarder) override {
    // _transform->SetRotation(glm::vec3(_caca, 0.0f, 0.0f));
    // tryHarder->Register(_transform->Logic());
    _caca += 0.01f;
  };

  void ResourceUpdate(Device* device) override{};

  void Draw(Device* device) override{};
};
