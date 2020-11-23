//
// Created by vincent on 23/11/20.
//

#include "CRigidBody.h"

#include "CCollider.h"

CRigidBody::CRigidBody(Entity *owner, void *args) : IComponent(owner) {
  // Cast argument
  auto description = (RigidBodyDesc *)args;

  // Get dependencies
  _transform = GetEntity()->GetOrCreate<CTransform>(nullptr);
  _shape = GetEntity()->Get<CCollider>();

  assert(_shape != nullptr);

  // Sync with user made transform
  _btTransform.setIdentity();
  _btTransform.setOrigin({_transform->GetPosition().x,
                          _transform->GetPosition().y,
                          _transform->GetPosition().z});

  _btTransform.setOrigin({2.0f, 2.0f, 2.0f});

  btScalar mass(description->mass);
  bool isDynamic = (description->mass != 0.0f);

  btVector3 localInertia(0.0f, 0.0f, 0.0f);
  if (isDynamic) {
    _shape->GetShape()->calculateLocalInertia(mass, localInertia);
  }

  std::cout << localInertia.x() << "; " << localInertia.y() << "; "
            << localInertia.z() << std::endl;

  _motionState = new btDefaultMotionState(_btTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(
      1.0f, _motionState, _shape->GetShape(), localInertia);
  _body = new btRigidBody(rbInfo);

  GetEntity()->GetSystem()->AddBody(_body);
}

void CRigidBody::SyncBody() {
  _transform->SetPosition({_btTransform.getOrigin().x(),
                           _btTransform.getOrigin().y(),
                           _btTransform.getOrigin().z()});

  //  std::cout << _btTransform.getOrigin().x() << "; "
  //            << _btTransform.getOrigin().y() << "; "
  //            << _btTransform.getOrigin().z() << std::endl;

  _body->applyForce({10.0f, 10.0f, 10.0f}, {0.0f, 0.0f, 0.0f});
}

LogicCall CRigidBody::Logic() {
  LogicCall call = {};
  call.call = [this] { this->SyncBody(); };

  return call;
}

const int CRigidBody::Uuid = 7;