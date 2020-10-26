//
// Created by vinhi on 26/10/2020.
//

#include "Frame.h"

#include <iostream>
#include <toml11/toml.hpp>

#include "../common/Log.h"

Frame::Frame(Device* device, const std::string& config) {
  struct Pass {
    std::string name;
    std::vector<std::string> outputs;
    std::vector<std::string> inputs;
    std::string pointOfView;
    std::string shader;
    bool multiple;
  };

  // Parse given toml config
  auto data = toml::parse(config);

  auto configTable = toml::find(data, "config");
  auto configName = toml::find<std::string>(configTable, "name");
  LOG_DEBUG("Starting parsing toml: '" + configName + "'")

  std::vector<Pass> passes(toml::find<int>(configTable, "nbPasses"));

  // List all rendering passes
  for (const auto& [k, v] : data.as_table()) {
    auto table = v;
    if (k != "config") {
      Pass pass = {};
      pass.name = k;
      pass.outputs = toml::find<std::vector<std::string>>(table, "outputs");
      pass.inputs = toml::find<std::vector<std::string>>(table, "deps");
      pass.pointOfView = toml::find<std::string>(table, "pointOfView");
      pass.shader = toml::find<std::string>(table, "shader");
      pass.multiple = toml::find<bool>(table, "multiple");

      // toml11 internally use an unorderer_map
      // that's why we need to sort pass
      auto order = toml::find<int>(table, "order");
      passes[order] = pass;
    }
  }

  // Store texture
  std::unordered_map<std::string, GPUTexture> coolTextures;

  // Create resources
  for (const auto& pass : passes) {
    auto program = device->CreateProgram(pass.shader);
    std::vector<GPUTexture> outputs(pass.outputs.size());
    for (int i = 0; i < pass.outputs.size(); i++) {
      outputs[i] = device->CreateEmptyTexture(
          TextureFormat::RGBA, TextureWrap::ClampToEdge, device->GetWidth(),
          device->GetHeight());
      coolTextures[pass.outputs[i]] = outputs[i];
    }

    auto renderTarget = device->CreateRenderTarget(outputs.data(), outputs.size(), true);

    std::vector<GPUTexture> inputs(pass.inputs.size());
    for (int i = 0; i < pass.inputs.size(); i++) {
      // DON'T CREATE AN INPUT TEXTURE
      // Just retrieve texture with this name created by a previous pass
      inputs[i] = coolTextures[pass.inputs[i]];
    }

    std::cout << "Rendering pass brief '" << pass.name
              << "': \n\tNb outputs: " << pass.outputs.size()
              << "\n\tNb inputs: " << pass.inputs.size() << std::endl;
  }
}