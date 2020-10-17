//
// Created by vinhi on 16/10/2020.
//

#pragma once

#include <string>
#include <vector>

#include "../renderer/CPUResources.h"

class MeshLoader {
 public:
  MeshLoader() = default;
  ~MeshLoader() = default;

  /**
   * Load one or more meshes from disk. A CPUBuffer is created for each mesh.
   * @return List of two CPUBuffer (vertices and indices).
   */
  std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> Load(
      const std::string& path
#ifdef __ANDROID__
      ,
      std::function<std::vector<unsigned char>(std::string)> fileReader
#endif
  );
};