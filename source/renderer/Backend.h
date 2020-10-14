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
   * @param data Data to upload on GPU
   * @param size Size of data (in bytes)
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
   * Specify active program to use for next draw calls.
   * @param program Program to use.
   */
  virtual void BindProgram(GPUProgram program) = 0;
  /**
   * Launch a draw call.
   * @param drawInput Structure of data to draw.
   * @param count Number of elements index to draw.
   * @param times Number of same draw calls to launch.
   * @param uniformBuffers Uniform buffers to bind during draw call.
   * @param nbUniformBuffers Number of uniform buffers.
   */
  virtual void Draw(GPUDrawInput drawInput, int count, int times,
                    GPUBuffer* uniformBuffers, size_t nbUniformBuffers) = 0;
};
