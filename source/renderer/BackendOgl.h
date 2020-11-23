//
// Created by vincent on 2/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>

#include "Backend.h"

/**
 * Backend built on OpenGL 3.3.
 * Default backend on desktop (Linux, Mac and Windows).
 */
class BackendOgl : public Backend {
 private:
  int _shaderVersion;
  bool _shaderEs;
 public:
  explicit BackendOgl(BackendDesc backendDesc, bool esApi);
  ~BackendOgl() = default;

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
};