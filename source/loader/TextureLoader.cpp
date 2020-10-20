//
// Created by vincent on 20.09.20.
//

#include "TextureLoader.h"

#include <stb/stb_image.h>

#include <cstdio>
#include <iostream>

#include "../renderer/Descriptions.h"

TextureLoader::TextureLoader() { _textures.reserve(10); }

TextureLoader::~TextureLoader() { Dispose(); }

void TextureLoader::Dispose() {
  for (int i = 0; i < _textures.size(); i++) {
    // Delete each loaded textures
    delete _data[i];
    _textures.clear();
  }
  _data.clear();
}

bool TextureLoader::IsLoaded(const std::string& path) {
  return !(_textures.find(path) == _textures.end());
}

uint32_t TextureLoader::Get(const std::string& path) {
  uint32_t texture = 0;
  if (IsLoaded(path)) {
    texture = _textures[path];
  }
  return texture;
}

CPUTexture TextureLoader::Load(const std::string& path, uint32_t texture) {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);

  // Check if file exists
  FILE* file = fopen(path.c_str(), "r");
  if (!file) {
    throw std::runtime_error("Unable to open texture: " + path);
  }
  fclose(file);

  // Invoke stb_image
  // https://github.com/nothings/stb
  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  CPUTexture cpuTexture = {data, width, height, TextureFormat::RGB};

  switch (nrChannels) {
    case 4:
      cpuTexture.format = TextureFormat::RGBA;
      break;
    case 3:
      cpuTexture.format = TextureFormat::RGB;
      break;
    case 2:
      cpuTexture.format = TextureFormat::RG;
      break;
    case 1:
      cpuTexture.format = TextureFormat::R;
      break;

    default:
      throw std::runtime_error("Unknown image format.");
  }

  // Register textures data
  _data.push_back(data);
  _textures[path] = texture;

  return cpuTexture;
}

int TextureLoader::Size() { return _textures.size(); }