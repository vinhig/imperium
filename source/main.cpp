//
// Created by vinhi on 24/10/2020.
//

#include <iostream>

#include "logic/Actions.h"
#include "logic/CMeshInstance.h"
#include "logic/CTransform.h"
#include "logic/Ecs.h"
#include "logic/Game.h"
#include "renderer/DeviceDesktop.h"

// https://music.youtube.com/watch?v=3GicPBWG5EM&feature=share

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

  void ResourceUpdate(Device* device) override {
    // auto resourceChange = _transform->Resource();
    // if (resourceChange.enable) {
      // device->UpdateUniformBuffer(resourceChange.dest, resourceChange.from);
    //}
  };

  void Draw(Device* device) override {
    /*auto d = _mesh->Draw();
    for (int i = 0; i < d.drawInputs.size(); i++) {
      ((DeviceDesktop*)device)
          ->_backend->Draw(d.drawInputs[i], d.counts[i], d.times, nullptr, 0);
    }*/
  };
};

int main(int argc, char** argv) {
  auto device = new DeviceDesktop({1024, 768, ApiDesc::OpenGL33});

  auto myGame = new MyGame(device);

  myGame->Run();
}