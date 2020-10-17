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
  // virtual uint32_t CreateVAO() = 0;
  virtual GPUBuffer CreateVertexBuffer(CPUBuffer<float> buffer) = 0;
  virtual GPUBuffer CreateIndexBuffer(CPUBuffer<int> buffer) = 0;
  virtual GPUBuffer CreateUniformBuffer(CPUBuffer<void> buffer) = 0;
  virtual GPUProgram CreateProgram(std::string name) = 0;
  virtual GPUDrawInput CreateDrawInput(
      GPUInputLayout inputLayout, const std::vector<GPUBuffer>& vertexBuffers,
      GPUBuffer indexBuffer) = 0;
  virtual GPUInputLayout CreateInputLayout(InputLayoutDesc inputLayoutDesc) = 0;

  /**
   * Completely replace data stored in GPU buffer.
   * @param buffer GPUBuffer to modify.
   * @param newData CPUBuffer containing the new data to store.
   */
  virtual void UpdateUniformBuffer(GPUBuffer buffer, CPUBuffer<void> newData) = 0;

  // virtual void Draw() = 0;

  virtual void RequestAnimationFrame() = 0;
  virtual bool ShouldClose() = 0;
};