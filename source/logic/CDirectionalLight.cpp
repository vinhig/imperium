//
// Created by vinhi on 06/11/2020.
//

#include "CDirectionalLight.h"

#include <cstring>

CDirectionalLight::CDirectionalLight(Entity *owner, void *args)
    : IComponent(owner) {
  auto device = ((IComponent *)this)->GetEntity()->GetSystem()->GetDevice();

  _position = glm::vec3(-2.0f, 8.0f, -1.0f);
  _rotation = glm::vec3(-30.0f, -30.0f, -30.0f);

  float near_plane = 0.01f;
  float far_plane = 30.0f;

  _projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

  glm::vec3 rads =
      glm::vec3(glm::radians(_rotation.x), glm::radians(_rotation.y),
                glm::radians(_rotation.z));

  _lookAt = glm::vec3(cos(rads.y) * cos(rads.x), sin(rads.x),
                      sin(rads.y) * cos(rads.x)) +
            _position;

  // View matrix
  // Currently at origin
  _view = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), _lookAt,
                      glm::vec3(0.0f, 1.0f, 0.0f));

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

LogicCall CDirectionalLight::Logic() {
  LogicCall call = {};
  call.call = [this] { this->UpdateVp(); };

  return call;
}

void CDirectionalLight::UpdateVp() {
  if (_update) {

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

const int CDirectionalLight::Uuid = 6;