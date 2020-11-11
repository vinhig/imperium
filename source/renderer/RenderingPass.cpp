//
// Created by vinhi on 26/10/2020.
//

#include "RenderingPass.h"

#include <iostream>
#include <utility>

RenderingPass::RenderingPass(GPUProgram program, GPURenderTarget renderTarget,
                             std::vector<GPUTexture> inputs, std::string name,
                             std::string pointOfViewName, bool needTextures,
                             bool needLights, unsigned int layer) {
  _program = program;
  _renderTarget = renderTarget;
  _inputs = std::move(inputs);
  _name = std::move(name);
  _pointOfViewName = std::move(pointOfViewName);
  _layer = layer;
  _needTextures = needTextures;
  _needLights = needLights;
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
  // Bind point of view
  if (_pointOfViewSet) {
    device->BindUniformBuffer(_pointOfView, 0);
  }
  if (_lightViewSet && _needLights) {
    device->BindUniformBuffer(_lightView, 1);
  }

  if (!_inputs.empty()) {
    device->BindTextures(_inputs.data(), _inputs.size(), 0);
  }
  int texOffset = _inputs.size();

  for (const auto& draw : _drawCalls) {
    if (_needTextures) {
      device->BindTextures(draw.textures, draw.nbTextures, texOffset);
    }
    for (int i = 0; i < draw.nbResources; i++) {
      device->Draw(draw.drawInputs[i], draw.counts[i], draw.times,
                   draw.uniforms, draw.nbUniforms, 2);
    }
  }
  _drawCalls.clear();
}

void RenderingPass::SetPointOfView(GPUBuffer pointOfView) {
  _pointOfView = pointOfView;
  _pointOfViewSet = true;
}

void RenderingPass::SetLightView(GPUBuffer lightView) {
  _lightView = lightView;
  _lightViewSet = true;
}

void RenderingPass::Sort() {
  // TODO: as we are just drawing a little plant, don't bother with sorting lol
}
