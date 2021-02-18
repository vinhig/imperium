#pragma once

//
// Created by vinhig on 17.01.2021.
//

#include <mathfu/glsl_mappings.h>
#include <mathfu/vector.h>

namespace Imperium::Logic {
struct Camera {
 private:
  struct CameraPacked {
    float _view[16];
    float _projection[16];
    float _viewProj[16];
  };

  bool _projectionChanged{true};
  bool _viewChanged{true};
  mathfu::mat4 _projection;
  mathfu::mat4 _view;
  mathfu::mat4 _viewProj;

  mathfu::vec3 _pos;
  float _fov;
  float _aspect;
  float _near;
  float _far;

  CameraPacked _cameraPacked{};

  void UpdateMatrices();

 public:
  /**
   * Create camera from given parameters. Atm, pos isn't used. Default position
   * is set to {0, 0, 0}. Perspective camera.
   * @param pos Position of the center of the camera.
   * @param fov Field of view.
   * @param aspect Aspect ratio (often width of screen/height of screen).
   * @param near Minimum depth.
   * @param far Maximum depth.
   */
  Camera(const mathfu::vec3& pos, float fov, float aspect, float near,
         float far);
  ~Camera();

  /**
   * Packed data to use with buffer. Chunk of floats of size = Size(). See
   * CameraPacked in `class Camera;`.
   * @return Pointer to packed data.
   */
  void* Data();

  /**
   * Size of underlying structure. See CameraPacked in `class Camera;`.
   * @return Size of underlying camera structure.
   */
  static unsigned Size();
};
}  // namespace Imperium::Logic