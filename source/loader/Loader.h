//
// Created by vinhig on 19.01.2021.
//

#pragma once

#include "core/Option.h"

namespace Imperium::Loader {
class Model;
class Loader {
 public:
  Loader();
  ~Loader();

  /**
   * Create dummy triangle model.
   */
  Core::Option<Model*> CreateModelTriangle();

  /**
   * Load complex GLTF model from disk. GLTF version 2.0 with vertices
   * (position, normal, texture coordinate) and indices are supported.
   * Hierarchy, nodes and textures aren't.
   * @param path Path to gltf on disk.
   * @return Optional model (no value if model failed to be loaded).
   */
  Core::Option<Model*> CreateModelGltf(const char* path);
};
}  // namespace Imperium::Loader