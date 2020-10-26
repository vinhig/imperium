//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "CPUResources.h"
#include "GPUResources.h"

/**
 * Device creates API context and its corresponding window.
 * It interacts with a manually given backend.
 * It's stateful (registers draw-calls and buffer manipulations).
 */
class Device {
 public:
  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;

  /**
   * Clear given renderTarget. Clear means fill the whole texture with a
   * solid color. Each texture linked to the renderTarget will be cleared.
   * @param renderTarget RenderTarget to clear.
   */
  virtual void Clear(RenderTarget renderTarget) = 0;

  /**
   * Create a GPUBuffer containing vertices. Make sure vertices are correctly
   * structured as a vertex buffer can only be used next to an input layout with
   * the same structure.
   * @param buffer CPUBuffer containing structured vertices.
   * @return GPUBuffer containing given vertices.
   */
  virtual GPUBuffer CreateVertexBuffer(CPUBuffer<float> buffer) = 0;

  /**
   * Create a GPUBuffer containing indices. Each index targets a vertex from
   * another GPUBuffer. Make sure to not upload out-of-range indices. This
   * method does not perform any verification. At the time being, only signed
   * int are pushed. But TODO: we should use unsigned short.
   * @param buffer CPUBuffer containing indices.
   * @return GPUBuffer containing given indices.
   */
  virtual GPUBuffer CreateIndexBuffer(CPUBuffer<int> buffer) = 0;

  /**
   * Create a GPUBuffer containing arbitrary data. It's often a struct that will
   * be reflected in shaders.
   * @param buffer GPUBuffer containing data to copy.
   * @return GPUBuffer containing data.
   */
  virtual GPUBuffer CreateUniformBuffer(CPUBuffer<void> buffer) = 0;

  /**
   * This method has different behaviour depending on the chosen API backend:
   * - DX11: load spirv shaders, transpile them to HLSL and then compile them.
   *    (HLSL version 5)
   * - OGL33 and OGLES32: load spirv shaders, transpile them to GLSL and then
   * compile them. (GLSL version 330 core)
   * @param name Path to shaders without the file extension (Eg:
   * "assets/phong").
   * @return GPUProgram that can be used to achieve draw calls.
   */
  virtual GPUProgram CreateProgram(std::string name) = 0;

  /**
   * Create a "pack" of resources that can be used to achieve a draw call.
   * Number of vertexBuffers must equal number of input layout entries. You're
   * allowed to put the same vertexBuffer multiple times as the method will
   * simplify it.
   * @param inputLayout Structure of data that will be used to achieve this draw
   * call. View Descriptions.h for an extensive explanation.
   * @param vertexBuffers List of vertex buffers containing vertices.
   * @param indexBuffer Buffer containing indices.
   * @return GPUDrawInput that represent a pack containing resources to achieve
   * a draw call.
   */
  virtual GPUDrawInput CreateDrawInput(
      GPUInputLayout inputLayout, const std::vector<GPUBuffer>& vertexBuffers,
      GPUBuffer indexBuffer) = 0;

  /**
   * Create an abstract way to structure buffers during a draw call.
   * Creation of an input layout is separated from create of a draw input as
   * there can be multiple identical input layout linked to different buffers.
   * @return GPUInputLayout
   */
  virtual GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) = 0;

  /**
   * Create an empty texture with given characteristics.
   * @param format Describe color channels of the new texture.
   * @param wrap Behaviour of the sampler in case of out of range.
   * @param width Width of the texture.
   * @param height Height of the texture
   * @return GPUTexture created with those information and completely empty.
   */
  virtual GPUTexture CreateEmptyTexture(TextureFormat format, TextureWrap wrap,
                                        int width, int height) = 0;

  /**
   * Create a texture by uploading content loaded from a TextureLoader.
   * @param cpuTexture Texture loaded on the CPU.
   * @return GPUTexture containing corresponding to given CPUTexture.
   */
  virtual GPUTexture CreateTextureFromData(CPUTexture cpuTexture) = 0;

  /**
   * Completely replace data stored in GPU buffer.
   * @param buffer GPUBuffer to modify.
   * @param newData CPUBuffer containing the new data to store.
   */
  virtual void UpdateUniformBuffer(GPUBuffer buffer,
                                   CPUBuffer<void> newData) = 0;

  /**
   * Specify active program to use for next draw calls.
   * @param program Program to use.
   */
  virtual void BindProgram(GPUProgram program) = 0;

  /**
   * Specify which textures to use for next draw calls.
   * @param textures Array of textures to use.
   * @param nbTextures Number of textures in given array.
   */
   virtual void BindTextures(GPUTexture* textures, int nbTextures) = 0;

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

  /**
   * Ask underlying backend to swap buffers. Swapping buffers means showing the
   * most recent back buffer on screen that issued all his draw calls. The back
   * buffer chosen may differ depend on the swap effect.
   */
  virtual void RequestAnimationFrame() = 0;

  /**
   * Get current active program.
   * @return GPUProgram that will be used to achieve draw calls.
   */
  virtual GPUProgram GetCurrentProgram() = 0;

  /**
   * Check if the device should close.
   * @return True if the device should close.
   */
  virtual bool ShouldClose() = 0;
};