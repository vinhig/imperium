#pragma once

//
// Created by vinhig on 28.01.2021.
//

namespace Imperium::Render::Frontend {
struct Mesh;
struct Material;
struct Model {
 private:
  struct Element {
    Mesh* _mesh{nullptr};
    Material* _material{nullptr};
  };
  Element* _elements;
  unsigned int _nbElements{0};
  unsigned int _currentElement{0};

 public:
  Model(unsigned int nbElements);
  ~Model();

  void AddMesh(Mesh* mesh, Material* material);
  void Draw();
  void Destroy();
};
}  // namespace Imperium::Render::Frontend