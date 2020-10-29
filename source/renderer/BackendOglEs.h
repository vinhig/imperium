//
// Created by vincent on 2/10/20.
//

#pragma once

#include "Backend.h"

/**
 * Backend built on OpenGL Es 3.2.
 * Mainly used with the Android an Web build.
 */
class BackendOglEs : public Backend {
 public:
  explicit BackendOglEs(BackendDesc backendDesc);
  ~BackendOglEs() = default;
  void Clear(GPURenderTarget renderTarget) override;

  GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) override;
  GPUProgram CreateProgram(std::vector<uint32_t> vertexSource,
                           std::vector<uint32_t> fragmentSource) override;
  GPUDrawInput CreateDrawInput(GPUInputLayout inputLayout,
                               const std::vector<GPUBuffer>& vertexBuffers,
                               GPUBuffer indexBuffer) override;
  GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) override;
  GPUTexture CreateTexture(TextureCreationDesc textureCreationDesc) override;
  GPURenderTarget CreateRenderTarget(const std::vector<GPUTexture>& colors, GPUTexture depth) override;

  void BindProgram(GPUProgram program) override;
  void BindTexture(GPUTexture texture, int index) override;
  void BindTextures(const std::vector<GPUTexture>& texture, int index) override;
  void BindRenderTarget(GPURenderTarget renderTarget) override;

  void Draw(GPUDrawInput drawInput, int count, int times,
            GPUBuffer* uniformBuffers, size_t nbUniformBuffers) override;
  void BlitRenderTarget(GPURenderTarget from, GPURenderTarget to) override;

  void UpdateBuffer(BufferUpdateDesc updateDesc) override;
};