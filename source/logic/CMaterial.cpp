//
// Created by vinhi on 25/10/2020.
//

#include "CMaterial.h"

#include <string>

#include "../common/File.h"
#include "../loader/TextureLoader.h"

CMaterial::CMaterial(Entity* owner, void* args) : IComponent(owner) {
#if __ANDROID__
  SetDiffuseTexture("no-texture.png");
  SetNormalTexture("no-texture.png");
#else
  SetDiffuseTexture("../assets/textures/no-texture.png");
  SetNormalTexture("../assets/textures/no-texture.png");
#endif

  _material = {0.8, 1.0, {1.0, 1.0, 1.0, 1.0}};
  GetCPUBuffer()->data = malloc(sizeof(SubMaterial));
  GetCPUBuffer()->size = sizeof(SubMaterial);
  memcpy(GetCPUBuffer()->data, &_material, sizeof(SubMaterial));

  // TODO: copy CPUBuffer before calling device is quite dirty (but we're just
  // copying references to data)
  CPUBuffer<void> cpuBuffer = {};
  cpuBuffer.data = GetCPUBuffer()->data;
  cpuBuffer.size = GetCPUBuffer()->size;
  GPUBuffer gpuBuffer = GetEntity()->GetSystem()->GetDevice()->CreateUniformBuffer(cpuBuffer);
  SetGPUBuffer(gpuBuffer);
}

void CMaterial::SetDiffuseTexture(const std::string& path) {
  if (File::textureLoader.IsLoaded(path)) {
    _diffuse = File::textureLoader.Get(path);
  } else {
    auto normal = File::textureLoader.Load(path);
    _diffuse =
        GetEntity()->GetSystem()->GetDevice()->CreateTextureFromData(normal);
    File::textureLoader.Link(path, _diffuse);
  }

  // TODO: must free memory of last texture ^^
  _textures[0] = _diffuse;
}

void CMaterial::SetNormalTexture(const std::string& path) {
  if (File::textureLoader.IsLoaded(path)) {
    _normal = File::textureLoader.Get(path);
  } else {
    auto normal = File::textureLoader.Load(path);
    _normal =
        GetEntity()->GetSystem()->GetDevice()->CreateTextureFromData(normal);
    File::textureLoader.Link(path, _normal);
  }

  // TODO: must free memory of last texture ^^
  _textures[1] = _normal;
}

GPUTexture* CMaterial::GetTextures() { return &_textures[0]; }

int CMaterial::NbTextures() { return 2; }
