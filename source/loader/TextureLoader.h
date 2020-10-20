//
// Created by vincent on 20.09.20.
//

#include <string>
#include <unordered_map>
#include <vector>

#include "../renderer/CPUResources.h"

/**
 * Load textures from disk. Register their data and GPU pointer. Free data on
 * destruction. Prevent duplicates.
 */
class TextureLoader {
 private:
  std::vector<unsigned char*> _data;
  std::unordered_map<std::string, uint32_t> _textures;

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
  uint32_t Get(const std::string& path);

  /**
   * Load a texture from disk. Register texture. Doesn't check if the texture
   * has been previously loaded.
   * @param path Path to texture.
   * @param texture GPU texture.
   */
  CPUTexture Load(const std::string& path, uint32_t texture);

  /**
   * Number of textures loaded.
   */
  int Size();
};