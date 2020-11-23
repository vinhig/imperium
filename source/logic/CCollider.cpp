//
// Created by vincent on 23/11/20.
//

#include "CCollider.h"

CCollider::CCollider(Entity* owner, void* args) : IComponent(owner) {
  // Cast arguments
  ColliderDesc* desc = (ColliderDesc*)args;

  // Bullet stuff
  _shape = new btSphereShape(btScalar(2.));

  GetEntity()->GetSystem()->AddShape(_shape);
}

const int CCollider::Uuid = 8;