//
// Created by vinhi on 24/10/2020.
//

#include "CMeshInstance.h"

#include "../loader/MeshLoader.h"

CMeshInstance::CMeshInstance(Entity* owner, void* args) : IComponent(owner) {
  std::string path = *((std::string*)args);
  auto device = GetEntity()->GetSystem()->GetDevice();
  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  auto program = GetEntity()->GetSystem()->GetDevice()->GetCurrentProgram();
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back({0, 3, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void*)(sizeof(float) * 0)});
  inputLayoutDesc.entries.push_back({1, 3, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void*)(sizeof(float) * 3)});
  inputLayoutDesc.entries.push_back({2, 2, false, sizeof(float) * 9,
                                     DataType::Float,
                                     (void*)(sizeof(float) * 6)});

  auto inputLayout = device->CreateInputLayout(inputLayoutDesc);
  // TODO: MeshLoader should be a singleton
  auto meshLoader = new MeshLoader();
  auto meshes = meshLoader->Load(path);

  int i = 0;
  for (const auto& mesh : meshes) {
    // Create resources
    _vertexBuffers.push_back(device->CreateVertexBuffer(mesh.first));
    _indexBuffers.push_back(device->CreateIndexBuffer(mesh.second));
    _counts.push_back(mesh.second.nbElements);

    // Create draw input
    std::vector<GPUBuffer> buffers(3);
    buffers[0] = _vertexBuffers[i];
    buffers[1] = _vertexBuffers[i];
    buffers[2] = _vertexBuffers[i];
    auto drawInput =
        device->CreateDrawInput(inputLayout, buffers, _indexBuffers[i]);
    _drawInputs.push_back(drawInput);
    i++;
  }

  // It's mandatory to have a CTransform
  // And we need its corresponding GPUBuffer as a uniform buffer
  _transform = GetEntity()->GetOrCreate<CTransform>(nullptr);
  _uniforms.push_back(_transform->GetGPUBuffer());

  // How to draw this? Only the CMaterial component will say
  _material = GetEntity()->GetOrCreate<CMaterial>(nullptr);
  _uniforms.push_back(_material->GetGPUBuffer());
}

DrawCall CMeshInstance::Draw() {
  return DrawCall{_drawInputs.data(),
                  _uniforms.data(),
                  _material->GetTextures(),
                  (int)_uniforms.size(),
                  _material->NbTextures(),
                  _counts.data(),
                  1,
                  (int)_drawInputs.size()};
}
