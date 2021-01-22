//
// Created by vinhig on 15.01.2021.
//

#pragma once

#include "core/Option.h"

namespace Imperium::Render::Backend {

enum PipelineType { Graphics, Compute };

enum BufferType {
  // Contains vertices, stored in device visible.
  Vertex,
  // Contains indices, stored in device visible.
  Index,
  // Contains arbitrary data, stored in host and device visible.
  Uniform,
  // Used to copy data, stored in host and device visible.
  Staging };

struct Buffer;

class Backend {
 public:
  virtual ~Backend()= default;;

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void BeginPipeline(int pipeline) = 0;
  virtual void EndPipeline(int pipeline) = 0;

  virtual void BindRenderpass(int renderpass) = 0;

  virtual Core::Option<int> CreatePipeline(PipelineType pipeline) = 0;

  virtual Buffer* CreateBuffer(BufferType bufferType, int size) = 0;
  virtual void DeleteBuffer(Buffer* buffer) = 0;
  virtual void* MapBuffer(Buffer* buffer) = 0;
  virtual void UnmapBuffer(Buffer* buffer) = 0;
  virtual void CopyBuffer(Buffer* src, Buffer* dest) = 0;
};

}  // namespace Imperium::Render::Backend