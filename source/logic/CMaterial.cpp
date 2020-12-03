//
// Created by vinhi on 25/10/2020.
//

#include "CMaterial.h"

#include <cstring>
#include <string>

#include "../common/File.h"
#include "../common/Log.h"
#include "../loader/TextureLoader.h"
#include "Game.h"

CMaterial::CMaterial(Entity* owner, void* args) : IComponent(owner) {
  SetDiffuseTexture("../assets/textures/no-texture.png");
  SetNormalTexture("../assets/textures/no-texture.png");

  _material = {0.8, 1.0, {1.0, 1.0, 1.0, 1.0}};
  GetCPUBuffer()->data = malloc(sizeof(SubMaterial));
  GetCPUBuffer()->size = sizeof(SubMaterial);
  memcpy(GetCPUBuffer()->data, &_material, sizeof(SubMaterial));

  // TODO: copy CPUBuffer before calling device is quite dirty (but we're just
  // copying references to data)
  CPUBuffer<void> cpuBuffer = {};
  cpuBuffer.data = GetCPUBuffer()->data;
  cpuBuffer.size = GetCPUBuffer()->size;
  GPUBuffer gpuBuffer =
      GetEntity()->GetSystem()->GetDevice()->CreateUniformBuffer(cpuBuffer);
  SetGPUBuffer(gpuBuffer);
}

void CMaterial::SetDiffuseTexture(const std::string& path) {
  auto textureLoader = GetEntity()->GetSystem()->GetGame()->GetTextureLoader();
  if (textureLoader->IsLoaded(path)) {
    LOG_DEBUG("Optimisation success! (Diffuse texture)");
    _diffuse = textureLoader->Get(path);
  } else {
    auto normal = textureLoader->Load(path);
    _diffuse =
        GetEntity()->GetSystem()->GetDevice()->CreateTextureFromData(normal);
    textureLoader->Link(path, _diffuse);
  }

  // TODO: must free memory of last texture ^^
  _textures[0] = _diffuse;
}

void CMaterial::SetNormalTexture(const std::string& path) {
  auto textureLoader = GetEntity()->GetSystem()->GetGame()->GetTextureLoader();
  if (textureLoader->IsLoaded(path)) {
    LOG_DEBUG("Optimisation success! (Normal texture)");
    _normal = textureLoader->Get(path);
  } else {
    auto normal = textureLoader->Load(path);
    _normal =
        GetEntity()->GetSystem()->GetDevice()->CreateTextureFromData(normal);
    textureLoader->Link(path, _normal);
  }

  // TODO: must free memory of last texture ^^
  _textures[1] = _normal;
}

GPUTexture* CMaterial::GetTextures() { return &_textures[0]; }

int CMaterial::NbTextures() { return 2; }
