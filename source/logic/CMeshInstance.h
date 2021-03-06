//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"
#include "CMaterial.h"
#include "CTransform.h"
#include "Ecs.h"

class CMeshInstance : public IComponent, public IComponentDrawable {
 private:
  // Store references to resources even if...
  std::vector<GPUBuffer> _vertexBuffers;
  std::vector<GPUBuffer> _indexBuffers;
  std::vector<int> _counts;

  // ... we just need this little guy
  std::vector<GPUDrawInput> _drawInputs;
  std::vector<GPUBuffer> _uniforms;

  CTransform* _transform;
  CMaterial* _material;

 public:
  CMeshInstance(Entity* owner, void* args);

  DrawCall Draw() override;

  int UUID() override { return 2; }
  static const int Uuid = 2;
};

template CMeshInstance* Entity::GetOrCreate<CMeshInstance>(void*);