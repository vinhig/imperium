//
// Created by vinhi on 16/10/2020.
//

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../renderer/CPUResources.h"
#include "../renderer/GPUResources.h"

class MeshLoader {
 private:
  std::unordered_map<std::string,
      std::pair<std::vector<GPUDrawInput>, std::vector<int>>>
      _meshes;

 public:
  MeshLoader() = default;
  ~MeshLoader() = default;

  /**
   * Check if a mesh has been loaded.
   * @param path Path to texture.
   */
  bool IsLoaded(const std::string& path);

  /**
   * Get previously loaded mesh as draw input. Return 0 if the mesh doesnt'
   * exist.
   *
   */
  std::pair<std::vector<GPUDrawInput>, std::vector<int>> Get(const std::string& path);

  /**
   * Link a mesh name and his already loaded correspondent. You should link
   * every mesh you load to avoid creation of duplicates.
   * @param path Path to mesh.
   * @param mesh GPU address of the mesh.
   */
  void Link(const std::string& path, std::vector<GPUDrawInput> mesh,
            std::vector<int> counts);

  /**
   * Load one or more meshes from disk. A CPUBuffer is created for each mesh.
   * @return List of two CPUBuffer (vertices and indices).
   */
  std::vector<std::pair<CPUBuffer<float>, CPUBuffer<int>>> Load(
      const std::string& path);
};