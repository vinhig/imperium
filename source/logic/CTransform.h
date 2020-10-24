//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <glm/gtx/transform.hpp>

#include "Ecs.h"

class CTransform : public IComponent,
                   public IComponentLogic,
                   public IComponentResource {
 private:
  struct Matrices {
    float mvp[16];
    float model[16];
  };
  glm::vec3 _position;
  glm::vec3 _rotation;
  glm::vec3 _scale;

  // TODO: we should use a camera for that lol
  glm::mat4 _projection;
  glm::mat4 _view;
  glm::mat4 _mvp;

  glm::mat4 _model;

  bool _update;

 public:
  explicit CTransform(Entity* owner, void* args);

  void UpdateMvp();

  LogicCall Logic() override;

  int UUID() override { return 1; };
  static const int Uuid = 1;

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
  const glm::vec3& GetScale() const { return _scale; }
  void SetScale(const glm::vec3& scale) {
    _scale = scale;
    _update = true;
  }
};

template <>
CTransform* Entity::GetOrCreate<CTransform>(void*);