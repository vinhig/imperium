//
// Created by vinhig on 19.01.2021.
//

#include "Loader.h"

#include <string.h>

#include "loader/Model.h"

namespace Imperium::Loader {
Loader::Loader() = default;
Loader::~Loader() = default;

Core::Option<Model *> Loader::CreateModelTriangle() {
  // Data for basic triangle
  // clang-format off
  float v[27] = {
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };

  auto vertices = new float[27];
  memcpy(vertices, v, sizeof(float) * 27);

  auto indices = new unsigned int[3];
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  // clang-format on
  auto model = new Model(1);
  model->AddRawMesh({vertices, indices, 27, 3});
  return Core::Option<Model *>(model);
}
}  // namespace Imperium::Loader