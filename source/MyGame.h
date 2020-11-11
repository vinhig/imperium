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
  Entity* bench;
  Entity* ball;
  float _caca = 0.0f;

 public:
  explicit MyGame(Device* device) : Game(device) {}

  void Load(Device* device) override {
    // _transform = new CTransform(nullptr, device);
    // _mesh = new CMeshInstance(nullptr, device, "../assets/indoor-plant.fbx");

    viewport = new Entity(GetSystem(), 1);
    viewport->GetOrCreate<CViewport>(nullptr);

    camera = new Entity(GetSystem(), 2);
    auto cam = camera->GetOrCreate<CCamera>(nullptr);
    cam->SetRotation(glm::vec3(-25.0f, -90.0f, 0.0f));
    cam->SetPosition(glm::vec3(0.0f, 8.0f, 16.0f));

    light = new Entity(GetSystem(), 3);
    auto li = light->GetOrCreate<CDirectionalLight>(nullptr);
    // li->SetRotation(glm::vec3(0.0f, 0.0f, -80.0f));
    // li->SetPosition(glm::vec3(5.0f, 5.0f, 0.0f));

    ball = new Entity(GetSystem(), 5);
    std::string path3 = "../assets/meshes/camera.fbx";
    ball->GetOrCreate<CMeshInstance>((void*)&path3);
    ball->Get<CTransform>()->SetScale(glm::vec3(0.5, 0.5, 0.5));
    ball->Get<CMeshInstance>()->SetLayer(Layer::D);

    // Furnitures
    bench = new Entity(GetSystem(), 4);
    std::string path2 = "../assets/meshes/fucking-plane.fbx";
    bench->GetOrCreate<CMeshInstance>((void*)&path2);
    // bench->Get<CTransform>()->SetPosition(glm::vec3(0.0f, -2.2f, 0.0f));
    bench->Get<CTransform>()->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
    bench->Get<CTransform>()->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

    plant = new Entity(GetSystem(), 0);
#if __ANDROID__
    std::string path = "indoor-plant.fbx";
#else
    std::string path = "../assets/meshes/indoor-plant.fbx";
#endif
    auto mesh = plant->GetOrCreate<CMeshInstance>((void*)&path);
    plant->Get<CTransform>()->SetRotation(glm::vec3(-90.0f, 0.0f, 45.0f));
    plant->Get<CTransform>()->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
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
  };

  void LogicalUpdate(TryHarder* tryHarder) override {
    _caca += 0.02f;

    light->Get<CDirectionalLight>()->SetPosition(
        glm::vec3(-2.0f, 4.0f, sin(_caca) * 2.0f));

    light->Get<CDirectionalLight>()->SetRotation(
        glm::vec3(-30.0f, cos(_caca) * 180 / 3.14, -30.0f));

    ball->Get<CTransform>()->SetPosition(
        light->Get<CDirectionalLight>()->GetPosition());
    ball->Get<CTransform>()->SetRotation(
        light->Get<CDirectionalLight>()->GetRotation());
  };

  void ResourceUpdate(Device* device) override{};

  void Draw(Device* device) override{};
};
