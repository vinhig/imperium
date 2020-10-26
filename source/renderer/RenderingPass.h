//
// Created by vinhi on 26/10/2020.
//

#pragma once

#include "GPUResources.h"
#include "../logic/Actions.h"

/**
 * RenderingPass registers issued draw calls and place them in a rendering context
 * characterized by a program and an render target.
 */
class RenderingPass {
 private:
  GPUProgram _program{};
  GPURenderTarget _renderTarget{};

  std::vector<DrawCall> _drawCalls;

 public:
  RenderingPass(GPUProgram program, GPURenderTarget renderTarget);
  ~RenderingPass();

  /**
   * Register a new draw call.
   * Remark: draw call isn't not achieved immediately.
   * @param drawCall
   */
  void AddDrawCall(DrawCall drawCall);

  /**
   * Sort draw calls by bindings to minimize api context changed.
   */
  void Sort();
};