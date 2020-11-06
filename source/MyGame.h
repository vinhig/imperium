//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include "logic/Actions.h"
#include "logic/CCamera.h"
#include "logic/CDirectionalLight.h"
#include "logic/CMeshInstance.h"
#include "logic/CTransform.h"
#include "logic/CViewport.h"
#include "logic/Ecs.h"
#include "logic/Game.h"

class MyGame : public Game {
 private:
  // CTransform* _transform;
  // CMeshInstance* _mesh;
  Entity* plant;
  Entity* viewport;
  Entity* camera;
  Entity* light;
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
    std::string path = "../assets/meshes/indoor-plant.fbx";
#endif
    auto mesh = plant->GetOrCreate<CMeshInstance>((void*)&path);
    plant->Get<CTransform>()->SetRotation(glm::vec3(-90.0f, 0.0f, 45.0f));
    plant->Get<CTransform>()->SetPosition(glm::vec3(0.0f, -1.5f, 0.0f));
    plant->Get<CTransform>()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

#if __ANDROID__
    plant->Get<CMaterial>()->SetDiffuseTexture("indoor-plant-diffuse.jpg");
    plant->Get<CMaterial>()->SetNormalTexture("indoor-plant-normal.jpg");
#else
    plant->Get<CMaterial>()->SetDiffuseTexture(
        "../assets/textures/indoor-plant-diffuse.jpg");
    plant->Get<CMaterial>()->SetNormalTexture(
        "../assets/textures/indoor-plant-normal.jpg");
#endif

    viewport = new Entity(GetSystem(), 1);
    viewport->GetOrCreate<CViewport>(nullptr);

    camera = new Entity(GetSystem(), 2);
    auto cam = camera->GetOrCreate<CCamera>(nullptr);

    cam->SetRotation(glm::vec3(-25.0f, 0.0f, 0.0f));
    cam->SetPosition(glm::vec3(0.0f, -2.0f, 4.0f));

    light = new Entity(GetSystem(), 3);
    auto li = light->GetOrCreate<CDirectionalLight>(nullptr);

    li->SetPosition(glm::vec3(3.0f,3.0f,3.0f));
  };

  void LogicalUpdate(TryHarder* tryHarder) override { _caca += 2.0f; };

  void ResourceUpdate(Device* device) override{};

  void Draw(Device* device) override{};
};
