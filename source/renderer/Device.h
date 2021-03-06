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
  virtual void Clear(GPURenderTarget renderTarget) = 0;

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
   * @param height Height of the texture.
   * @param presion Precision of the texture.
   * @return GPUTexture created with those information and completely empty.
   */
  virtual GPUTexture CreateEmptyTexture(TextureFormat format, TextureWrap wrap,
                                        int width, int height,
                                        TexturePrecision precision) = 0;

  /**
   * Create a texture by uploading content loaded from a TextureLoader.
   * @param cpuTexture Texture loaded on the CPU.
   * @return GPUTexture containing corresponding to given CPUTexture.
   */
  virtual GPUTexture CreateTextureFromData(CPUTexture cpuTexture) = 0;

  /**
   * Create a render target from given textures.
   * @param textures Color buffers to link to render target.
   * @param depth Depth texture to link to render target.
   */
  virtual GPURenderTarget CreateRenderTarget(
      const std::vector<GPUTexture>& textures, const GPUTexture& depth) = 0;

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
   * @param offset Binding number to begin binding.
   */
  virtual void BindTextures(GPUTexture* textures, int nbTextures,
                            int offset) = 0;

  /**
   * Specify render target to target for next draw calls.
   * @param renderTarget Render target to draw to.
   */
  virtual void BindRenderTarget(GPURenderTarget renderTarget) = 0;

  /**
   * Specify an uniform buffer to bind at a specific layout to use for next draw
   * calls.
   * @param uniformBuffer GPUBuffer to bind.
   * @param layout Slot index to target.
   */
  virtual void BindUniformBuffer(GPUBuffer uniformBuffer, int layout) = 0;

  /**
   * Launch a draw call.
   * @param drawInput Structure of data to draw.
   * @param count Number of elements index to draw.
   * @param times Number of same draw calls to launch.
   * @param uniformBuffers Uniform buffers to bind during draw call.
   * @param nbUniformBuffers Number of uniform buffers.
   */
  virtual void Draw(GPUDrawInput drawInput, int count, int times,
                    GPUBuffer* uniformBuffers, size_t nbUniformBuffers,
                    int offsetUniform) = 0;

  /**
   * Blit a render target on another render target.
   * @param from Render target acting as source.
   * @param to Render target acting as destination.
   */
  virtual void BlitRenderTarget(GPURenderTarget from, GPURenderTarget to) = 0;

  /**
   * Ask underlying window backend to poll events and prepare subsystems for
   * registering calls.
   */
  virtual void BeginFrame() = 0;

  /**
   * Ask underlying backend to swap buffers. Swapping buffers means showing the
   * most recent back buffer on screen that issued all his draw calls. The back
   * buffer chosen may differ depend on the swap effect.
   */
  virtual void EndFrame() = 0;

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