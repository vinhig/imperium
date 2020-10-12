//
// Created by vincent on 8/10/20.
//

#include "DeviceDesktop.h"

#include <spirv_glsl.hpp>
#include <stdexcept>

#include "../common/File.h"
#include "BackendOgl.h"
#include "BackendOglEs.h"
#include "Descriptions.h"

DeviceDesktop::DeviceDesktop(DeviceDesc deviceDesc) {
  if (!glfwInit()) {
    throw std::runtime_error("Unable to init glfw");
  }

  switch (deviceDesc.api) {
    case OpenGL33: {
      // Request corresponding context
      // from GLFW
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      // Create window
      _window = glfwCreateWindow(deviceDesc.width, deviceDesc.height,
                                 "Imperium", nullptr, nullptr);
      glfwMakeContextCurrent(_window);
      // Load function pointer
      // from GLAD
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Unable to load OpenGL 3.3 functions.");
      }
      // Init backend
      _backend = new BackendOgl();
      glClearColor(0.5f, 0.2f, 0.1f, 1.0f);  // perfectly illegal
      break;
    }
    case OpenGL46: {
      throw std::runtime_error("Backend not implemented.");
    }
    case OpenGLES32: {
      // Request corresponding context
      // from GLFW
      glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      // Create window
      _window = glfwCreateWindow(deviceDesc.width, deviceDesc.height,
                                 "Imperium", nullptr, nullptr);
      glfwMakeContextCurrent(_window);
      // Load function pointer
      // from GLAD
      if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Unable to load OpenGLES 3.2 functions.");
      }
      // Init backend
      _backend = new BackendOglEs();
      glClearColor(0.4f, 0.6f, 0.9f, 1.0f);  // perfectly illegal
      break;
    }
#ifdef _WIN32
    case Directx11: {
      throw std::runtime_error("Backend not implemented.");
    }
#endif
    default: {
      throw std::runtime_error(
          "Backend not implemented or not compatible with this platform.");
    }
  }
}
bool DeviceDesktop::ShouldClose() { return glfwWindowShouldClose(_window); }

void DeviceDesktop::RequestAnimationFrame() {
  glfwPollEvents();
  glfwSwapBuffers(_window);
}

void DeviceDesktop::Clear(RenderTarget renderTarget) { _backend->Clear(0); }

GPUBuffer DeviceDesktop::CreateVertexBuffer(CPUBuffer<float> buffer) {
  return GPUBuffer{};
}
GPUBuffer DeviceDesktop::CreateIndexBuffer(CPUBuffer<int> buffer) {
  return GPUBuffer{};
}

GPUBuffer DeviceDesktop::CreateUniformBuffer(CPUBuffer<void> buffer) {
  return GPUBuffer{};
}

GPUProgram DeviceDesktop::CreateProgram(std::string name) {
  // Read fragment and vertex shader
  auto vertexSource = File::ReadAllBinary("assets/" + name + ".vert.glsl.spv");
  auto fragmentSource =
      File::ReadAllBinary("assets/" + name + ".frag.glsl.spv");

  // Convert to uint32_t array
  std::vector<uint32_t> vertexSpirv;
  vertexSpirv.reserve(vertexSource.size() / 4);
  for (int i = 0; i < vertexSource.size(); i += 4) {
    uint32_t num = (uint32_t)vertexSource[i + 0] << 24 |
                   (uint32_t)vertexSource[i + 1] << 16 |
                   (uint32_t)vertexSource[i + 2] << 8 |
                   (uint32_t)vertexSource[i + 3];
    vertexSpirv.push_back(num);
  }
  std::vector<uint32_t> fragmentSpirv;
  fragmentSpirv.reserve(fragmentSource.size() / 4);
  for (int i = 0; i < fragmentSource.size(); i += 4) {
    uint32_t num = (uint32_t)fragmentSource[i + 0] << 24 |
                   (uint32_t)fragmentSource[i + 1] << 16 |
                   (uint32_t)fragmentSource[i + 2] << 8 |
                   (uint32_t)fragmentSource[i + 3];
    fragmentSpirv.push_back(num);
  }

  // Spirv is ready to be compiled by the backend
  auto program = _backend->CreateProgram(vertexSpirv, fragmentSpirv);

  return {program};
}

DeviceDesktop::~DeviceDesktop() = default;
