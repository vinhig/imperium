//
// Created by vinhi on 15.01.2020.
//

#pragma once

namespace Imperium::Render::Backend {

enum PipelineType { Graphics, Compute };

class Backend {
 public:
  virtual ~Backend(){};

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void BeginPipeline(int pipeline) = 0;
  virtual void EndPipeline(int pipeline) = 0;

  virtual void BindRenderpass(int renderpass) = 0;

  virtual int CreatePipeline(PipelineType pipeline) = 0;
};

}  // namespace Imperium::Render::Backend