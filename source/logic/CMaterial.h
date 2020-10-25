//
// Created by vinhi on 25/10/2020.
//

#pragma once

#include "../renderer/GPUResources.h"
#include "Ecs.h"

class CMaterial : public IComponent, public IComponentResource {
 private:
  GPUTexture _diffuse;
  GPUTexture _normal;

  GPUTexture _textures[2];

  struct SubMaterial {
    float ambient;
    float specular;
    float color[4];
  };

  SubMaterial _material;

 public:
  explicit CMaterial(Entity* owner, void* args);
  ~CMaterial() = default;

  int UUID() override { return 3; }
  static const int Uuid = 3;

  /**
   * Set diffuse texture and **load it**.
   * @param path Path to diffuse texture file.
   */
  void SetDiffuseTexture(const std::string& path);

  /**
   * Set normal texture and **load it**.
   * @param path Path to normal texture file.
   */
  void SetNormalTexture(const std::string& path);

  /**
   * Get all material textures.
   * @return Array of gpu textures.
   */
  GPUTexture* GetTextures();

  /**
   * Get number of loaded texture.
   * @return Length of array returned bu GetTextures().
   */
  int NbTextures();
};

template CMaterial* Entity::GetOrCreate<CMaterial>(void*);