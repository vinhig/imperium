//
// Created by vincent on 23/11/20.
//

#include "CCollider.h"

CCollider::CCollider(Entity* owner, void* args) : IComponent(owner) {
  // Cast arguments
  auto desc = (ColliderDesc*)args;

  // Bullet stuff
  if (desc->shape == ColliderShape::Cube) {
    _shape = new btBoxShape(
        {btScalar(desc->x), btScalar(desc->y), btScalar(desc->z)});
  } else {
    _shape = new btSphereShape(desc->x);
  }

  GetEntity()->GetSystem()->AddShape(_shape);
}

const int CCollider::Uuid = 8;