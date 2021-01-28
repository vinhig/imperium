#include "render/frontend/Model.h"

#include <cstdio>

#include "render/frontend/Material.h"
#include "render/frontend/Mesh.h"

namespace Imperium::Render::Frontend {
Model::Model(unsigned int nbElements) {
  _elements = new Element[nbElements];
  _nbElements = nbElements;
}

Model::~Model() {}

void Model::AddMesh(Mesh* mesh, Material* material) {
  if (_currentElement >= _nbElements) {
    printf("Cannot add another mesh, out of range.\n");
    return;
  }
  if (!mesh) {
    printf("Cannot add a null mesh.\n");
    return;
  }
  _elements[_currentElement]._mesh = mesh;
  _elements[_currentElement]._material = material;
  _currentElement += 1;
}

void Model::Draw() {
  for (int i = 0; i < _nbElements; i++) {
    _elements[i]._mesh->Draw();
  }
}

void Model::Destroy() {
  for (int i = 0; i < _nbElements; i++) {
    _elements[i]._mesh->Destroy();
  }
}
}  // namespace Imperium::Render::Frontend