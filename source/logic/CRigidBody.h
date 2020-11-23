//
// Created by vincent on 23/11/20.
//

#pragma once

#include <btBulletDynamicsCommon.h>

#include "CCollider.h"
#include "CTransform.h"
#include "Ecs.h"

struct RigidBodyDesc {
  float mass;
};

class CRigidBody : public IComponent, public IComponentLogic {
 private:
  // Dependencies
  CTransform* _transform;
  CCollider* _shape;

  // Bullet stuff
  btRigidBody* _body;
  btTransform _btTransform;  // has to be sync with _transform
  btDefaultMotionState* _motionState;

 public:
  explicit CRigidBody(Entity* owner, void* args);

  LogicCall Logic() override;

  void SyncBody();

  int UUID() override { return 7; };
  static const int Uuid;
};
