//
// Created by vinhi on 26/10/2020.
//

#include "RenderingPass.h"

#include <iostream>
#include <utility>

RenderingPass::RenderingPass(GPUProgram program, GPURenderTarget renderTarget,
                             std::vector<GPUTexture> inputs, std::string name,
                             bool needTextures, unsigned int layer) {
  _program = program;
  _renderTarget = renderTarget;
  _inputs = std::move(inputs);
  _name = std::move(name);
  _layer = layer;
  _needTextures = needTextures;
}

// TODO: should delete _renderTarget
RenderingPass::~RenderingPass() = default;

void RenderingPass::AddDrawCall(DrawCall drawCall) {
  if (drawCall.times > 0 && drawCall.nbResources > 0) {
    _drawCalls.push_back(drawCall);
  }
}

bool RenderingPass::CanAccept(Layer layer) const {
  unsigned int test = _layer | layer;
  return test == _layer;
}

void RenderingPass::Commit(Device* device) {
  // TODO: this step should be parameterizable
  device->Clear(_renderTarget);
  // Specific to this rendering pass
  device->BindProgram(_program);            // program
  device->BindRenderTarget(_renderTarget);  // render target

  for (const auto& draw : _drawCalls) {
    if (_needTextures) {
      device->BindTextures(draw.textures, draw.nbTextures);
    }
    if (!_inputs.empty()) {
      device->BindTextures(_inputs.data(), _inputs.size());
    }
    for (int i = 0; i < draw.nbResources; i++) {
      device->Draw(draw.drawInputs[i], draw.counts[i], draw.times,
                   draw.uniforms, draw.nbUniforms);
    }
  }
  _drawCalls.clear();
}

void RenderingPass::Sort() {
  // TODO: as we are just drawing a little plant, don't bother with sorting lol
}