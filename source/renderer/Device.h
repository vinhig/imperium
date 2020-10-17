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
  /**
   * Clear given renderTarget. Clear means fill the whole texture with a
   * solid color. Each texture linked to the renderTarget will be cleared.
   * @param renderTarget RenderTarget to clear.
   */
  virtual void Clear(RenderTarget renderTarget) = 0;

  /**
   * Create a GPUBuffer containing vertices. Make sure vertices are correclty
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
   * Completely replace data stored in GPU buffer.
   * @param buffer GPUBuffer to modify.
   * @param newData CPUBuffer containing the new data to store.
   */
  virtual void UpdateUniformBuffer(GPUBuffer buffer,
                                   CPUBuffer<void> newData) = 0;

  // virtual void Draw() = 0;

  /**
   * Ask underlying backend to swap buffers. Swapping buffers means showing the
   * most recent back buffer on screen that issued all his draw calls. The back
   * buffer chosen may differ depend on the swap effect.
   */
  virtual void RequestAnimationFrame() = 0;

  /**
   * Check if the device should close.
   * @return True if the device should close.
   */
  virtual bool ShouldClose() = 0;
};