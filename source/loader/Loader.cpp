//
// Created by vinhig on 19.01.2021.
//

#include "Loader.h"

#include "loader/Model.h"

namespace Imperium::Loader {
Loader::Loader() = default;
Loader::~Loader() = default;

Core::Option<Model *> Loader::CreateModelTriangle() {
  // Data for basic triangle
  // clang-format off
    float vertices[] = {
      1.0f,  1.0f, 0.0f, 
      -1.0f, 1.0f, 0.0f,
      0.0f, -1.0f, 0.0f,
    };
    int indices[] = {
      0, 1, 2
    };
  // clang-format on
  auto model = new Model(1);
  model->SetVertices(0, &vertices[0], 9);
  model->SetIndices(0, &indices[0], 3);
  return Core::Option<Model *>(model);
}
}  // namespace Imperium::Loader