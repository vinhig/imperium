//
// Created by vincent on 2/10/20.
//

#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "CPUResources.h"
#include "Descriptions.h"
#include "GPUResources.h"

/**
 * Backend handles specific API calls.
 * It doesn't create its context nor its window.
 */
class Backend {
 public:
  /**
   * Clear given framebuffer. Clear means fill the whole texture with a solid
   * color. Each texture linked to the framebuffer will be cleared.
   * @param framebuffer Framebuffer to clear.
   */
  virtual void Clear(uint32_t framebuffer) = 0;

  /**
   * Create some fucking good buffer of data.
   * @param BufferCreationDesc Description of the new buffer.
   * @return GPU address of buffer
   */
  virtual GPUBuffer CreateBuffer(BufferCreationDesc bufferCreationDesc) = 0;

  /**
   * Create a program from fragment and vertex shader binaries.
   * @param vertexSource Binary vertex shader (spirv assembly).
   * @param fragmentSource Binary fragment shader (spirv assembly).
   * @return GPU address of program
   */
  virtual GPUProgram CreateProgram(std::vector<uint32_t> vertexSource,
                                   std::vector<uint32_t> fragmentSource) = 0;

  /**
   * Create a draw input from vertex and index buffers structured by an input
   * layout.
   * @param inputLayoutDesc Input layout description.
   * @return GPU address of Draw input.
   */
  virtual GPUDrawInput CreateDrawInput(
      GPUInputLayout inputLayout, const std::vector<GPUBuffer>& vertexBuffers,
      GPUBuffer indexBuffer) = 0;

  /**
   * Create an abstract way to structure buffers during a draw call.
   * Creation of an input layout is separated from create of a draw input as
   * there can be multiple identical input layout linked to different buffers.
   * @param inputLayoutDesc
   * @return Drawable structure of buffers.
   */
  virtual GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) = 0;

  /**
   * Create a texture from a description (optionally from given data).
   * @param textureCreationDesc Description of the texture.
   * @return GPU address of texture and some useful information.
   */
  virtual GPUTexture CreateTexture(TextureCreationDesc textureCreationDesc) = 0;

  /**
   * Specify active program to use for next draw calls.
   * @param program Program to use.
   */
  virtual void BindProgram(GPUProgram program) = 0;

  /**
   * Bind a texture to a specific index. This method is only implemented for non
   * bindless backend. Can and has to be called even if it's not implemented to
   * ensure the cross-platform character of this engine.
   * @param texture GPUTexture to use for future draw calls.
   * @param index Index of the texture in program. Defined in
   * `layout(binding=index)` from shader source code.
   */
  virtual void BindTexture(GPUTexture texture, int index) = 0;

  /**
   * Bind multiple textures. This method is only implemented for bindless
   * backend as it require texture to be texture handle. Can and has to be
   * called even if it's not implemented to ensure the cross-platform character
   * of this engine.
   * @param textures List of textures handle to bind.
   * @param binding Index of the textures structure in the shader.
   */
  virtual void BindTextures(const std::vector<GPUTexture>& textures,
                            int index) = 0;

  /**
   * Launch a draw call. Can draw from a whole index buffer by setting `start`
   * and `end` to 0.
   * @param drawInput Structure of data to draw.
   * @param count Number of elements index to draw.
   * @param times Number of same draw calls to launch.
   * @param start First index of bound index buffer.
   * @param end Last index of bound index buffer.
   * @param uniformBuffers Uniform buffers to bind during draw call.
   * @param nbUniformBuffers Number of uniform buffers.
   */
  virtual void Draw(GPUDrawInput drawInput, int count, int end,
                    GPUBuffer* uniformBuffers, size_t nbUniformBuffers) = 0;

  /**
   * Replace a part of data stored in a buffer. Part of this buffer is specified
   * by an offset and a size.
   */
  virtual void UpdateBuffer(BufferUpdateDesc updateDesc) = 0;
};
