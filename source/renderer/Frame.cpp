//
// Created by vinhi on 26/10/2020.
//

#include "Frame.h"

#include <iostream>
#include <sstream>
#include <toml11/toml.hpp>

#include "../common/Log.h"

// String manipulation
bool endsWith(std::string const& string, std::string const& ending) {
  if (string.length() >= ending.length()) {
    return (0 == string.compare(string.length() - ending.length(),
                                ending.length(), ending));
  } else {
    return false;
  }
}

Frame::Frame(Device* device, const std::string& config) {
  struct Pass {
    std::string name;
    std::vector<std::string> outputs;
    std::vector<std::string> precisions;
    std::string dimensions;
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
      pass.precisions =
          toml::find<std::vector<std::string>>(table, "precisions");
      pass.dimensions = toml::find<std::string>(table, "dimensions");
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

    // Parse dimensions
    int width = device->GetWidth();
    int height = device->GetHeight();
    if (pass.dimensions != "device") {
      // Extract width and height from string
      auto toParse = pass.dimensions;
      int del = toParse.find("x");
      assert(del != -1);  // Must be this format : WIDTHxHEIGHT
      auto strWidth = std::stringstream(toParse.substr(0, del));
      auto strHeight =
          std::stringstream(toParse.substr(del + 1, toParse.length() - 1));
      // Convert to integer
      strWidth >> width;
      strHeight >> height;
    }

    // Create a depth buffer by default
    GPUTexture depth = device->CreateEmptyTexture(
        TextureFormat::DEPTH, TextureWrap::ClampToEdge, width,
        height, TexturePrecision::High);

    // All outputs color buffer
    std::vector<GPUTexture> outputs(pass.outputs.size());
    for (int i = 0; i < pass.outputs.size(); i++) {
      // Differentiation between depth and color texture
      if (endsWith(pass.outputs[i], "_DEPTH")) {
        coolTextures[pass.outputs[i]] = depth;
      } else {
        auto precision = TexturePrecision::High;
        if (pass.precisions[i] == "low") {
          precision = TexturePrecision::Low;
        }
        outputs[i] = device->CreateEmptyTexture(TextureFormat::RGBA,
                                                TextureWrap::ClampToEdge, width,
                                                height, precision);
        // Class them by name
        coolTextures[pass.outputs[i]] = outputs[i];
      }
    }

    auto renderTarget = device->CreateRenderTarget(outputs, depth);

    std::vector<GPUTexture> inputs(pass.inputs.size());
    for (int i = 0; i < pass.inputs.size(); i++) {
      // DON'T CREATE AN INPUT TEXTURE
      // Just retrieve texture with this name created by a previous pass
      if (coolTextures.find(pass.inputs[i]) != coolTextures.end()) {
        inputs[i] = coolTextures[pass.inputs[i]];
      } else {
        throw std::runtime_error(
            "Couldn't find this dependencies in render graph: " +
            pass.inputs[i]);
      }
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
      GPURenderTarget{0, device->GetWidth(), device->GetHeight()});

  device->BindRenderTarget(
      GPURenderTarget{0, device->GetWidth(), device->GetHeight()});
}

void Frame::RegisterDrawCall(DrawCall drawCall) {
  // Get render pass accepting this drawCall
  for (const auto& pass : _renderingPasses) {
    if (pass->CanAccept(drawCall.layer)) {
      pass->AddDrawCall(drawCall);
    }
  }
}

void Frame::SetPointOfView(GPUBuffer uniformBuffer, const std::string& name) {
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
