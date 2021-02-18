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
  Staging
};

enum DescriptorBindingType {
  BufferBinding,
  // TextureBinding,
};

enum DescriptorStageType {
  VertexStage,
  FragmentStage,
};

struct Buffer;
struct DescriptorLayout;
struct DescriptorSet;
struct Pipeline;
struct Texture;

struct DescriptorBinding {
  DescriptorBindingType bindingType;
  DescriptorStageType stageType;
  unsigned size;
  union {
    Buffer* buffer;
    // Texture* texture;
  };
};

/**
 * Abstract class defining what methods a proper backend should implement. A
 * backend provides high level methods to interact with a specific graphics API.
 */
class Backend {
 public:
  virtual ~Backend() = default;

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void BeginPipeline(Pipeline* pipeline) = 0;
  virtual void EndPipeline(Pipeline* pipeline) = 0;

  virtual void BindRenderpass(int renderpass) = 0;

  virtual void BindVertexBuffers(int count, Buffer* vertexBuffers) = 0;
  virtual void BindIndexBuffer(Buffer* indexBuffer) = 0;
  // virtual void BindConstantBuffer(int offset, Buffer* constantBuffer) = 0;
  virtual void BindUniform(int offset, Pipeline* pipeline, size_t size,
                           void* data) = 0;

  virtual void DrawElements(int count) = 0;

  virtual Core::Option<Pipeline*> CreatePipeline(
      PipelineType pipeline, unsigned descriptorCount,
      DescriptorLayout* descriptors) = 0;
  virtual void DeletePipeline(Pipeline* pipeline) = 0;
  virtual void DeferDeletePipeline(Pipeline* buffer) = 0;

  virtual Buffer* CreateBuffer(BufferType bufferType, unsigned size) = 0;
  virtual void DeleteBuffer(Buffer* buffer) = 0;
  virtual void DeferDeleteBuffer(Buffer* buffer) = 0;
  virtual void* MapBuffer(Buffer* buffer) = 0;
  virtual void UnmapBuffer(Buffer* buffer) = 0;
  virtual void CopyBuffer(Buffer* src, Buffer* dest) = 0;

  virtual DescriptorLayout* CreateDescriptorLayout(
      int bindingCount, DescriptorBinding* bindings) = 0;
  virtual DescriptorSet* CreateDescriptorSet(DescriptorLayout* descriptorLayout,
                                             int bindingCount,
                                             DescriptorBinding* bindings) = 0;
  virtual void BindDescriptorSet(Pipeline* pipeline,
                                 DescriptorSet* descriptor) = 0;
};

}  // namespace Imperium::Render::Backend