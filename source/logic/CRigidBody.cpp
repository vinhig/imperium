//
// Created by vincent on 23/11/20.
//

#include "CRigidBody.h"

#include <glm/gtx/quaternion.hpp>

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

  auto quaternion =
      glm::quat({glm::radians(_transform->GetRotation().x - 90.0f),
                 glm::radians(_transform->GetRotation().y),
                 glm::radians(_transform->GetRotation().z)});
  _btTransform.setRotation(
      {quaternion.x, quaternion.y, quaternion.z, quaternion.w});

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
      mass, _motionState, _shape->GetShape(), localInertia);
  _body = new btRigidBody(rbInfo);

  GetEntity()->GetSystem()->AddBody(_body);
}

void CRigidBody::SyncBody() {
  auto vec = glm::eulerAngles(
      glm::quat(_body->getWorldTransform().getRotation().getW(),
                _body->getWorldTransform().getRotation().getX(),
                _body->getWorldTransform().getRotation().getY(),
                _body->getWorldTransform().getRotation().getZ()));

  glm::vec3 degVec;

  degVec.x = glm::degrees(vec.x) + 90.0f;
  degVec.y = glm::degrees(vec.y);
  degVec.z = glm::degrees(vec.z);

  _transform->SetPosition({_body->getWorldTransform().getOrigin().x(),
                           _body->getWorldTransform().getOrigin().y(),
                           _body->getWorldTransform().getOrigin().z()});
  _transform->SetRotation(degVec);
}

LogicCall CRigidBody::Logic() {
  LogicCall call = {};
  call.call = [this] { this->SyncBody(); };

  return call;
}

const int CRigidBody::Uuid = 7;