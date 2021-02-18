//
// Created by vinhig on 17.01.2021.
//

#include "Camera.h"
namespace Imperium::Logic {
Camera::Camera(const mathfu::vec3& pos, float fov, float aspect, float near,
               float far)
    : _pos(pos), _fov(fov), _aspect(aspect), _near(near), _far(far) {
  UpdateMatrices();
}

void Camera::UpdateMatrices() {
  bool _viewProjChanged{false};
  if (_viewChanged) {
    _viewProjChanged = true;
    _view = mathfu::LookAtHelper({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
                                 {0.0f, 1.0f, 0.0f}, -1.0f);

    mathfu::vec4_packed projectionPacked[4];
    _projection.Pack(projectionPacked);

    memcpy(&_cameraPacked._projection[0], &projectionPacked->data_[0],
           sizeof(float) * 16);
  }
  if (_projectionChanged) {
    _viewProjChanged = true;
    _projection = mathfu::PerspectiveHelper(_fov, _aspect, _near, _far, -1.0f);

    mathfu::vec4_packed viewPacked[4];
    _view.Pack(viewPacked);

    memcpy(&_cameraPacked._view[0], &viewPacked->data_[0], sizeof(float) * 16);
  }
  if (_viewProjChanged) {
    _viewProj = _projection * _view;

    mathfu::vec4_packed viewProjectionPacked[4];
    _viewProj.Pack(viewProjectionPacked);

    memcpy(&_cameraPacked._viewProj[0], &viewProjectionPacked->data_[0],
           sizeof(float) * 16);
  }
}

void* Camera::Data() { return &_cameraPacked; }

unsigned Camera::Size() { return sizeof(CameraPacked); }

Camera::~Camera() {

}
}  // namespace Imperium::Logic
