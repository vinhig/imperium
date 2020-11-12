//
// Created by vinhi on 06/11/2020.
//

#pragma once

#include <glm/gtx/transform.hpp>

#include "Ecs.h"

class CDirectionalLight : public IComponent,
                          public IComponentLogic,
                          public IComponentResource {
 private:
  struct Matrices {
    float viewProj[16];
    float position[4];
  };

  glm::mat4 _projection;
  glm::mat4 _view;
  glm::vec3 _lookAt;

  glm::vec3 _position;
  glm::vec3 _rotation;

  bool _update;

 public:
  explicit CDirectionalLight(Entity* owner, void* args);

  void UpdateVp();

  LogicCall Logic() override;

  int UUID() override { return 6; };
  static const int Uuid;

  const glm::vec3& GetPosition() const { return _position; }
  void SetPosition(const glm::vec3& position) {
    _position = position;
    _update = true;
  }

  const glm::vec3& GetRotation() const { return _rotation; }
  void SetRotation(const glm::vec3& rotation) {
    _rotation = rotation;
    _update = true;
  }
};

template CDirectionalLight* Entity::GetOrCreate<CDirectionalLight>(void*);
template CDirectionalLight* System::GetFirstActive<CDirectionalLight>();
