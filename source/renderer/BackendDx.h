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

  // Swapchain backbuffers
  ComPtr<ID3D11RenderTargetView> _renderTargetView;
  ComPtr<ID3D11Texture2D> _depthStencilBuffer;
  ComPtr<ID3D11DepthStencilState> _depthStencilState;
  ComPtr<ID3D11DepthStencilView> _depthStencilView;
  ComPtr<ID3D11RasterizerState> _rasterizerState;

  // OpenGLified resources
  // Resources are represented by a uint32_t
  std::unordered_map<uint32_t, std::pair<ComPtr<ID3D11VertexShader>,
                                         ComPtr<ID3D11PixelShader>>>
      _programs;

  std::unordered_map<uint32_t,
                     std::pair<ComPtr<ID3D10Blob>, ComPtr<ID3D10Blob>>>
      _shadersCode;
  uint32_t _nbPrograms = 0;

  std::unordered_map<uint32_t, std::pair<std::vector<GPUBuffer>, GPUBuffer>>
      _vaos;
  uint32_t _nbVaos = 0;

  std::unordered_map<uint32_t, ComPtr<ID3D11InputLayout>> _inputLayouts;
  uint32_t _nbInputLayouts = 0;

  std::unordered_map<uint32_t, ComPtr<ID3D11Buffer>> _buffers;
  uint32_t _nbBuffers = 0;

  struct DxTexture {
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11SamplerState> samplerState;
    ComPtr<ID3D11ShaderResourceView> shaderResource;
  };

  std::unordered_map<uint32_t, DxTexture> _textures;
  uint32_t _nbTextures = 0;

 public:
  BackendDx(BackendDesc backendDesc, ComPtr<ID3D11Device> device,
            ComPtr<ID3D11DeviceContext> context,
            ComPtr<IDXGISwapChain> swapChain);
  ~BackendDx() = default;

  void Clear(GPURenderTarget renderTarget) override;
  GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) override;
  GPUProgram CreateProgram(std::vector<uint32_t> vertexSource,
                           std::vector<uint32_t> fragmentSource) override;
  GPUDrawInput CreateDrawInput(GPUInputLayout inputLayout,
                               const std::vector<GPUBuffer>& vertexBuffers,
                               GPUBuffer indexBuffer) override;
  GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) override;
  GPUTexture CreateTexture(TextureCreationDesc textureCreationDesc) override;
  GPURenderTarget CreateRenderTarget(const std::vector<GPUTexture>& colors,
                                     GPUTexture depth) override;

  void BindProgram(GPUProgram program) override;
  void BindTexture(GPUTexture texture, int index) override;
  void BindTextures(const std::vector<GPUTexture>& texture, int index) override;
  void BindRenderTarget(GPURenderTarget renderTarget) override;
  void BindUniformBuffer(GPUBuffer uniformBuffer, int layout) override;

  void Draw(GPUDrawInput drawInput, int count, int times) override;
  void BlitRenderTarget(GPURenderTarget from, GPURenderTarget to) override;

  void UpdateBuffer(BufferUpdateDesc updateDesc) override;

  // Specific to this backend
  void Present();
};
