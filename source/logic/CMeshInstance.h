//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"
#include "Ecs.h"

class CMeshInstance : public IComponentDrawable {
 private:
  // Store reference to resource even if...
  std::vector<GPUBuffer> _vertexBuffers;
  std::vector<GPUBuffer> _indexBuffers;
  std::vector<int> _counts;

  // ... we just need this little guy
  std::vector<GPUDrawInput> _drawInputs;

 public:
  CMeshInstance(Entity* owner, Device* device, const std::string& path);

  DrawCall Draw() override;

  int UUID() override { return 2; }
};