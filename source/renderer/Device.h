//
// Created by vincent on 8/10/20.
//

#pragma once

#include <cstdint>

/**
 * Device creates API context and its corresponding window.
 * It interacts with a manually given backend.
 * It's stateful (registers draw-calls and buffer manipulations).
 */
class Device {
 public:
  /**
   * Clear given renderTarget. Clear means fill the whole texture with a solid
   * color. Each texture linked to the renderTarget will be cleared.
   * @param renderTarget RenderTarget to clear.
   */
  virtual void Clear() = 0;
  /*virtual uint32_t CreateVAO() = 0;
  virtual uint32_t CreateVertexBuffer() = 0;
  virtual uint32_t CreateIndexBuffer() = 0;
  virtual uint32_t CreateUniformBuffer() = 0;

  virtual void Draw() = 0;*/

  virtual void RequestAnimationFrame() = 0;
  virtual bool ShouldClose() = 0;
};