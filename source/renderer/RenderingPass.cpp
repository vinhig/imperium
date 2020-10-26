//
// Created by vinhi on 26/10/2020.
//

#include "RenderingPass.h"
RenderingPass::RenderingPass(GPUProgram program, GPURenderTarget renderTarget) {
  _program = program;
  _renderTarget = renderTarget;
}

// TODO: should delete _renderTarget
RenderingPass::~RenderingPass() = default;

void RenderingPass::AddDrawCall(DrawCall drawCall) {
  if (drawCall.times > 0 && drawCall.nbResources > 0) {
    _drawCalls.push_back(drawCall);
  }
}

void RenderingPass::Sort() {
  // TODO: as we are just drawing a little plant, don't bother with sorting lol
}
