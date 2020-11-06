//
// Created by vinhi on 26/10/2020.
//

#pragma once

#include "../logic/Actions.h"
#include "Device.h"
#include "GPUResources.h"

/**
 * RenderingPass registers issued draw calls and place them in a rendering
 * context characterized by a program and an render target.
 */
class RenderingPass {
 private:
  GPUProgram _program{};
  GPURenderTarget _renderTarget{};
  GPUBuffer _pointOfView;

  std::vector<GPUTexture> _inputs;
  std::vector<DrawCall> _drawCalls;

  std::string _name;
  std::string _pointOfViewName;

  unsigned int _layer;

  bool _needTextures;
  bool _pointOfViewSet = false;

 public:
  RenderingPass(GPUProgram program, GPURenderTarget renderTarget,
                std::vector<GPUTexture> inputs, std::string name,
                std::string pointOfViewName, bool needTextures,
                unsigned int layer);
  ~RenderingPass();

  /**
   * Register a new draw call.
   * Remark: draw call isn't not achieved immediately.
   * @param drawCall
   */
  void AddDrawCall(DrawCall drawCall);

  /**
   * Check if given layer can be drawn with this RenderingPass.
   * @param layer Layer to check.
   * @return True if RenderingPass accepts this layer.
   */
  bool CanAccept(Layer layer) const;

  /**
   * Commit registered draw calls.
   * @param device Device to use.
   */
  void Commit(Device* device);

  /**
   * Sort draw calls by bindings to minimize api context changed.
   */
  void Sort();

  /**
   * Set which point of view to use.
   * @param pointOfView GPUBuffer to bind before any draw calls.
   */
  void SetPointOfView(GPUBuffer pointOfView);

  GPURenderTarget RenderTarget() { return _renderTarget; };

  std::string PointOfViewName() { return _pointOfViewName; };
};