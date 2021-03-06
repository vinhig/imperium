//
// Created by vincent on 20.09.20.
//

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"

/**
 * Load textures from disk. Register their data and GPU pointer. Free data on
 * destruction. Prevent duplicates.
 */
class TextureLoader {
 private:
  std::vector<unsigned char*> _data;
  std::unordered_map<std::string, GPUTexture> _textures;

 public:
  TextureLoader();

  /**
   * Destroy this TextureLoader. Invoke this->Dispose.
   */
  ~TextureLoader();

  /**
   * Delete all registered textures. Doesn't delete GPU textures.
   */
  void Dispose();

  /**
   * Check if a texture has been loaded.
   * @param path Path to texture.
   */
  bool IsLoaded(const std::string& path);

  /**
   * Get previously loaded texture. Return 0 if the texture doesnt' exist.
   *
   */
  GPUTexture Get(const std::string& path);

  /**
   * Load a texture from disk. Register texture. Doesn't check if the texture
   * has been previously loaded.
   * @param path Path to texture.
   * @param texture GPU texture.
   */
  CPUTexture Load(
      const std::string& path
#ifdef __ANDROID__
      // ,
      // std::function<std::vector<unsigned char>(std::string)> fileReader
#endif
  );

  /**
   * Link a texture name and his already loaded correspondent. You should link
   * every texture you load to avoid creation of duplicates.
   * @param path Path to texture texture.
   * @param texture GPU address of the texture.
   */
  void Link(const std::string& path, GPUTexture texture);

  /**
   * Number of textures loaded.
   */
  int Size();
};