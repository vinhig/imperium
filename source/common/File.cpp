//
// Created by vincent on 09.10.20.
//

#include "File.h"

#include <fstream>
#include <iostream>

std::string File::ReadAllFile(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    printf("Couldn't read file at %s.", path.c_str());
    return "";
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
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    printf("Couldn't read file at %s.", path.c_str());
    return std::vector<unsigned char>(0);
  }
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

  return buffer;
}