//
// Created by vinhi on 26/10/2020.
//

#pragma once

#include <vector>

#include "Device.h"
#include "RenderingPass.h"

/**
 * A Frame is what user sees each 16ms. It's the combination of multiple
 * rendering pass. It can be compared as a render graph, cause it's programmable
 * and involves multiple graphics technics.
 */
class Frame {
 private:
  std::vector<RenderingPass*> _renderingPasses;

 public:
  /**
   * Create a frame, all its underlying rendering passes and all resources
   * needed for future draw calls. RenderTargets are created and dispatched
   * through rendering passes that need them. Shaders are compiled and linked.
   * This constructor does a lot of think actually ^^. The architecture of the
   * Frame is constructed from a toml configuration file.
   * Remark: all shaders referenced by the toml should share the same input
   * layout (some workaround with dx, it's a shame i know).
   * @param config
   */
  Frame(Device* device, const std::string& config);

  /**
   * Commit all draw calls from all rendering passes.
   * @param device Device to use.
   */
  void Commit(Device* device);

  /**
   * Register a new draw call. Distribute draw call across rendering passes if
   * they accept it.
   * @param drawCall Draw call to register.
   * @param layer Layer to accept.
   */
  void RegisterDrawCall(DrawCall drawCall);

  /**
   * Link an uniform buffer as a named point of view. Frame will find the
   * underlying render pass that needs it.
   * @param uniformBuffer GPUBuffer representing a point of view.
   * @param name Name of the given point of view. Must match at least one name
   * from rendering pipeline config file.
   */
  void SetPointOfView(GPUBuffer uniformBuffer, const std::string& name);

  /**
   * Link a uniform buffer as a light view. Frame will find the underlying
   * render pass that needs it.
   * @param uniformBuffer GPUBuffer representing a light view.
   */
  void SetLightView(GPUBuffer uniformBuffer);
};