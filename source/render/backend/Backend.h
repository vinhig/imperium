//
// Created by vinhig on 15.01.2021.
//

#pragma once

#include "core/Option.h"

namespace Imperium::Render::Backend {

enum PipelineType { Graphics, Compute };

enum BufferType { Vertex, Index, Uniform };

class Backend {
 public:
  virtual ~Backend(){};

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void BeginPipeline(int pipeline) = 0;
  virtual void EndPipeline(int pipeline) = 0;

  virtual void BindRenderpass(int renderpass) = 0;

  virtual Core::Option<int> CreatePipeline(PipelineType pipeline) = 0;
  // virtual void CreateBuffer(BufferType bufferType) = 0;
};

}  // namespace Imperium::Render::Backend