//
// Created by vinhi on 24/10/2020.
//

#include "CTransform.h"

#include <cstring>
#include <iostream>

CTransform::CTransform(Entity* owner, void* args) : IComponent(owner) {
  auto device = ((IComponent*)this)->GetEntity()->GetSystem()->GetDevice();
  _position = glm::vec3(0.0f, 0.0f, 0.0f);
  _rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  _scale = glm::vec3(1.0f, 1.0f, 1.0f);

  _model = glm::mat4(1.0f);

  GetCPUBuffer()->data = malloc(sizeof(Matrices));
  GetCPUBuffer()->size = sizeof(Matrices);
  auto uniform = (Matrices*)GetCPUBuffer()->data;
  memcpy(&uniform->model[0], &_model[0][0], 16 * sizeof(float));

  // TODO: copy CPUBuffer before calling device is quite dirty
  CPUBuffer<void> cpuBuffer = {};
  cpuBuffer.data = GetCPUBuffer()->data;
  cpuBuffer.size = GetCPUBuffer()->size;
  GPUBuffer gpuBuffer = device->CreateUniformBuffer(cpuBuffer);
  SetGPUBuffer(gpuBuffer);

  _update = true;
}

void CTransform::UpdateMvp() {
  if (_update) {
    // Compute matrices changes
    auto translation = glm::translate(_position);
    auto scale = glm::scale(_scale);
    auto rotationX =
        glm::rotate(glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    auto rotationY =
        glm::rotate(glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    auto rotationZ =
        glm::rotate(glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    auto rotation = rotationX * rotationY * rotationZ;
    _model = translation * rotation * scale;

    // Push modification to cpu buffer
    auto uniform = (Matrices*)GetCPUBuffer()->data;
    memcpy(&uniform->model[0], &_model[0][0], 16 * sizeof(float));

    ForceUpdate();

    _update = false;
  }
}

LogicCall CTransform::Logic() {
  LogicCall call = {};
  call.call = [this] { this->UpdateMvp(); };

  return call;
}
