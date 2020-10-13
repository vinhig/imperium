//
// Created by vinhi on 12/10/2020.
//

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>

#include "Backend.h"
#include "Descriptions.h"
#include "GPUResources.h"

using namespace Microsoft::WRL;

/**
 * Backend built on DirectX 11.
 * Default backend on windows desktop.
 */
class BackendDx : public Backend {
 private:
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _context;
  ComPtr<IDXGISwapChain> _swapChain;

  // OpenGLified resources
  // Resources are represented by a uint32_t
  std::unordered_map<uint32_t, std::pair<ComPtr<ID3D11VertexShader>, ComPtr<ID3D11PixelShader>>>
      _programs;
  int _nbPrograms = 0;

 public:
  BackendDx(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
            ComPtr<IDXGISwapChain> swapChain);
  ~BackendDx() = default;

  void Clear(uint32_t framebuffer) override;

  GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) override;

  uint32_t CreateProgram(std::vector<uint32_t> vertexSource,
                         std::vector<uint32_t> fragmentSource) override;

  // Specific to this backend
  void Present();
};
