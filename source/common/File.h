//
// Created by vincent on 25.08.20.
//

#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "../loader/MeshLoader.h"
#include "../loader/TextureLoader.h"

class File {
 public:
  static std::function<std::vector<unsigned char>(std::string)> FileReader;

  static TextureLoader textureLoader;

  static MeshLoader meshLoader;

  /**
   * Read text from file specified by path.
   * @param path Path to file to read.
   * @return Content of file.
   */
  static std::string ReadAllFile(const std::string& path);
  /**
   * Read binary content from file specified by path.
   * @param path Path to file to read.
   * @return Content of file.
   */
  static std::vector<unsigned char> ReadAllBinary(const std::string& path);
};