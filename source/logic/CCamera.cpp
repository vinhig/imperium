//
// Created by vinhi on 06/11/2020.
//

#include "CCamera.h"
CCamera::CCamera(Entity *owner, void *args) : IComponent(owner) {
  auto device = ((IComponent *)this)->GetEntity()->GetSystem()->GetDevice();

  _position = glm::vec3(0.01f, 0.01f, 0.01f);
  _rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  _lookAt = glm::vec3(0.0f, 0.0f, 1.0f);

  _projection = glm::perspective(
      70.0f,
      (float)GetEntity()->GetSystem()->GetDevice()->GetWidth() /
          (float)GetEntity()->GetSystem()->GetDevice()->GetHeight(),
      0.1f, 100.0f);
  // View matrix
  // Currently at origin
  _view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));

  // Move it from origin to desired location
  auto translation = glm::translate(_position);
  auto rotationX =
      glm::rotate(glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  auto rotationY =
      glm::rotate(glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  auto rotationZ =
      glm::rotate(glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  auto rotation = rotationX * rotationY * rotationZ;

  _view = _projection * (_view * translation * rotation);

  GetCPUBuffer()->data = malloc(sizeof(Matrices));
  GetCPUBuffer()->size = sizeof(Matrices);
  auto uniform = (Matrices *)GetCPUBuffer()->data;
  memcpy(&uniform->viewProj[0], &_view[0][0], 16 * sizeof(float));

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

    // View matrice
    // Currently at origin
    _view =
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));

    // Move it from origin to desired location
    auto translation = glm::translate(_position);
    auto rotationX =
        glm::rotate(glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    auto rotationY =
        glm::rotate(glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    auto rotationZ =
        glm::rotate(glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    auto rotation = rotationX * rotationY * rotationZ;

    _view = _projection * (_view * rotation * translation);

    // Push modification to cpu buffer
    auto uniform = (Matrices *)GetCPUBuffer()->data;
    memcpy(&uniform->viewProj[0], &_view[0][0], 16 * sizeof(float));

    ForceUpdate();
    _update = false;
  }
}
