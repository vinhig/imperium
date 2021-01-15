//
// Created by vincent on 25.08.20.
//

#pragma once

#include <string>
#include <vector>

class File {
 public:
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