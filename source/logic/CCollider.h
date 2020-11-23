//
// Created by vincent on 23/11/20.
//

#pragma once

#include <btBulletDynamicsCommon.h>

#include "Ecs.h"

enum ColliderShape {
  Sphere,
  Cube,
};

struct ColliderDesc {
  ColliderShape shape;
  float x;
  float y;
  float z;
};

class CCollider : public IComponent {
 private:
  btCollisionShape* _shape;

 public:
  explicit CCollider(Entity* owner, void* args);

  btCollisionShape* GetShape() { return _shape; };

  int UUID() override { return 8; };
  static const int Uuid;
};