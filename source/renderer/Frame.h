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
  std::vector<RenderingPass> _renderingPasses;

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
};