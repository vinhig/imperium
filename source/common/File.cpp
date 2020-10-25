//
// Created by vincent on 9/10/20.
//

#include "File.h"

std::string File::ReadAllFile(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: '" + path + "'.");
  }
  std::string content;
  std::string line;
  while (std::getline(file, line)) {
    content += line + "\n";
  }
  file.close();
  return content;
}

std::vector<unsigned char> File::ReadAllBinary(const std::string& path) {
#if __ANDROID__
  auto buffer = FileReader(path);

  return buffer;
#else
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: '" + path + "'.");
  }
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

  return buffer;
#endif
}

std::function<std::vector<unsigned char>(std::string)> File::FileReader =
    [](std::string path) -> std::vector<unsigned char> {
  throw std::runtime_error("Setup your android file reader or i kill you.");
};

TextureLoader File::textureLoader;
MeshLoader File::meshLoader;