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
    std::vector<std::string> precisions;
    std::vector<std::string> inputs;
    std::string pointOfView;
    std::string shader;
    unsigned int layer;
    bool needTextures;
    bool needLights;
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
      pass.precisions = toml::find<std::vector<std::string>>(table, "outputs");
      pass.inputs = toml::find<std::vector<std::string>>(table, "deps");
      pass.pointOfView = toml::find<std::string>(table, "pointOfView");
      pass.shader = toml::find<std::string>(table, "shader");
      pass.needTextures = toml::find<bool>(table, "needTextures");
      pass.needLights = toml::find<bool>(table, "needLights");
      pass.multiple = toml::find<bool>(table, "multiple");

      // Get accepted layers
      std::vector<std::string> layers =
          toml::find<std::vector<std::string>>(table, "layers");

      for (const auto& l : layers) {
        if (l == "A") {
          pass.layer |= Layer::A;
        } else if (l == "B") {
          pass.layer |= Layer::B;
        } else if (l == "C") {
          pass.layer |= Layer::C;
        } else if (l == "D") {
          pass.layer |= Layer::D;
        } else if (l == "E") {
          pass.layer |= Layer::E;
        }
      }

      // toml11 internally use an unordered_map
      // that's why we need to sort pass
      auto order = toml::find<int>(table, "order");
      passes[order] = pass;
    }
  }

  // Store texture
  std::unordered_map<std::string, GPUTexture> coolTextures;

  // Create resources
  for (const auto& pass : passes) {
    // Shader for this rendering pass
    auto program = device->CreateProgram(pass.shader);

    // Depth buffer
    GPUTexture depth = device->CreateEmptyTexture(
        TextureFormat::DEPTH, TextureWrap::ClampToEdge, device->GetWidth(),
        device->GetHeight(), TexturePrecision::High);

    // All outputs color buffer
    std::vector<GPUTexture> outputs(pass.outputs.size());
    for (int i = 0; i < pass.outputs.size(); i++) {
      auto precision = TexturePrecision::High;
      if (pass.precisions[i] == "low") {
        precision = TexturePrecision::Low;
      }
      outputs[i] = device->CreateEmptyTexture(
          TextureFormat::RGBA, TextureWrap::ClampToEdge, device->GetWidth(),
          device->GetHeight(), precision);
      // Class them by name
      coolTextures[pass.outputs[i]] = outputs[i];
    }

    auto renderTarget = device->CreateRenderTarget(outputs, depth);

    std::vector<GPUTexture> inputs(pass.inputs.size());
    for (int i = 0; i < pass.inputs.size(); i++) {
      // DON'T CREATE AN INPUT TEXTURE
      // Just retrieve texture with this name created by a previous pass
      inputs[i] = coolTextures[pass.inputs[i]];
    }

    _renderingPasses.push_back(new RenderingPass(
        program, renderTarget, inputs, pass.name, pass.pointOfView,
        pass.needTextures, pass.needLights, pass.layer));

    std::cout << "Rendering pass brief '" << pass.name
              << "': \n\tNb outputs: " << pass.outputs.size()
              << "\n\tNb inputs: " << pass.inputs.size() << std::endl;
  }
}

void Frame::Commit(Device* device) {
  // Render all rendering passes
  for (const auto& pass : _renderingPasses) {
    pass->Commit(device);
  }
  // Blit last render target onto window
  device->BlitRenderTarget(
      _renderingPasses[_renderingPasses.size() - 1]->RenderTarget(),
      GPURenderTarget{0});
  device->BindRenderTarget(GPURenderTarget{0});
}

void Frame::RegisterDrawCall(DrawCall drawCall, Layer layer) {
  // Get render pass accepting this drawCall
  for (const auto& pass : _renderingPasses) {
    if (pass->CanAccept(layer)) {
      pass->AddDrawCall(drawCall);
    }
  }
}

void Frame::SetPointOfView(GPUBuffer uniformBuffer, std::string name) {
  for (const auto& pass : _renderingPasses) {
    if (pass->PointOfViewName() == name) {
      pass->SetPointOfView(uniformBuffer);
    }
  }
}
void Frame::SetLightView(GPUBuffer uniformBuffer) {
  for (const auto& pass : _renderingPasses) {
    if (pass->NeedLights()) {
      pass->SetLightView(uniformBuffer);
    }
  }
}
