//
// Created by vinhi on 06/11/2020.
//

#include "CCamera.h"

#include <cstring>

CCamera::CCamera(Entity *owner, void *args) : IComponent(owner) {
  auto device = ((IComponent *)this)->GetEntity()->GetSystem()->GetDevice();

  _position = glm::vec3(0.01f, 0.01f, 0.01f);
  _rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  _projection = glm::perspective(
      70.0f,
      (float)GetEntity()->GetSystem()->GetDevice()->GetWidth() /
          (float)GetEntity()->GetSystem()->GetDevice()->GetHeight(),
      0.1f, 100.0f);

  _lookAt = glm::vec3(cos(_rotation.y) * cos(_rotation.x), sin(_rotation.x),
                      sin(_rotation.y) * cos(_rotation.x)) +
            _position;

  // View matrix
  // Currently at origin
  _view = glm::lookAt(_position, _lookAt, glm::vec3(0.0f, 1.0f, 0.0f));

  _view = _projection * _view;

  GetCPUBuffer()->data = malloc(sizeof(Matrices));
  GetCPUBuffer()->size = sizeof(Matrices);
  auto uniform = (Matrices *)GetCPUBuffer()->data;
  memcpy(&uniform->viewProj[0], &_view[0][0], 16 * sizeof(float));
  memcpy(&uniform->position[0], &_position[0], 3 * sizeof(float));

  // TODO: copy CPUBuffer before calling device is quite dirty
  CPUBuffer<void> cpuBuffer = {};
  cpuBuffer.data = GetCPUBuffer()->data;
  cpuBuffer.size = GetCPUBuffer()->size;
  GPUBuffer gpuBuffer = device->CreateUniformBuffer(cpuBuffer);
  SetGPUBuffer(gpuBuffer);

  _update = false;
}

LogicCall CCamera::Logic() {
  LogicCall call = {};
  call.call = [this] { this->UpdateVp(); };

  return call;
}

void CCamera::UpdateVp() {
  if (_update) {
    std::cout << "Updating VP" << std::endl;

    glm::vec3 rads =
        glm::vec3(glm::radians(_rotation.x), glm::radians(_rotation.y),
                  glm::radians(_rotation.z));
    _lookAt = glm::vec3(cos(rads.y) * cos(rads.x), sin(rads.x),
                        sin(rads.y) * cos(rads.x)) +
              _position;

    // View matrix
    // Currently at origin
    _view = glm::lookAt(_position, _lookAt, glm::vec3(0.0f, 1.0f, 0.0f));

    _view = _projection * _view;

    // Push modification to cpu buffer
    auto uniform = (Matrices *)GetCPUBuffer()->data;
    memcpy(&uniform->viewProj[0], &_view[0][0], 16 * sizeof(float));
    memcpy(&uniform->position[0], &_position[0], 3 * sizeof(float));

    ForceUpdate();
    _update = false;
  }
}

const int CCamera::Uuid = 5;
