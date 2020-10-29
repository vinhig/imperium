//
// Created by vinhi on 29/10/2020.
//

#pragma once

#include "CViewport.h"
#include "Ecs.h"

class CViewport : public IComponent, public IComponentDrawable {
 private:
  GPUBuffer _vertices{};
  GPUBuffer _indices{};
  GPUDrawInput _drawInput{};

  std::vector<int> _counts;

 public:
  CViewport(Entity* owner, void* args);

  DrawCall Draw() override;

  int UUID() override { return 4; }
  static const int Uuid = 4;
};

template CViewport* Entity::GetOrCreate<CViewport>(void*);