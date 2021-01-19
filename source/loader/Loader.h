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
   * Load fbx model from disk.
   * @param path
   */
  Core::Option<Model*> CreateModelTriangle();
};
}  // namespace Imperium::Loader