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

GPUTexture TextureLoader::Get(const std::string& path) {
  GPUTexture texture = {};
  texture.texture = 0;
  if (IsLoaded(path)) {
    texture = _textures[path];
  }
  return texture;
}

CPUTexture TextureLoader::Load(
    const std::string& path
#ifdef __ANDROID__
    ,
    std::function<std::vector<unsigned char>(std::string)> fileReader
#endif
) {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);

#ifdef __ANDROID__
  auto vcontent = fileReader(path);
  auto content = vcontent.data();
  long file_size = vcontent.size();

  unsigned char* data = stbi_load_from_memory(content, file_size, &width,
                                              &height, &nrChannels, 4);
#else
  FILE* file = fopen(path.c_str(), "r");
  if (!file) {
    throw std::runtime_error("Unable to open texture: " + path);
  }
  fclose(file);

  // Invoke stb_image
  // https://github.com/nothings/stb
  stbi_info(path.c_str(), &width, &height, &nrChannels);
  unsigned char* data;
  if (nrChannels == 3) {
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
    nrChannels = 4;
  } else {
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
  }
#endif

  CPUTexture cpuTexture = {data, width, height, TextureFormat::RGBA};

  switch (nrChannels) {
    case 4:
      cpuTexture.format = TextureFormat::RGBA;
      std::cout << "TextureFormat::RGBA" << std::endl;
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

  return cpuTexture;
}

void TextureLoader::Link(const std::string& path, GPUTexture texture) {
  _textures[path] = texture;
}

int TextureLoader::Size() { return _textures.size(); }
