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

#if defined(_WIN32) || defined(WIN32)
#include "BackendDx.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3DCompiler.lib")
#endif

static void GLAPIENTRY OglDebugOutput(GLenum source, GLenum type, GLuint id,
                                      GLenum severity, GLsizei length,
                                      const GLchar *message,
                                      const void *userParam) {
  if (type == GL_DEBUG_TYPE_ERROR) {
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
    throw std::runtime_error("An OpenGL occured. See logs above.");
  }
}

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
      // Setup debug output
      glEnable(GL_DEBUG_OUTPUT);
      glDebugMessageCallback(OglDebugOutput, nullptr);
      // Init backend
      _backend = new BackendOgl({deviceDesc.width, deviceDesc.height});
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // perfectly illegal
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
      _backend = new BackendOglEs({deviceDesc.width, deviceDesc.height});
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // perfectly illegal
      break;
    }
#if defined(_WIN32) || defined(WIN32)
    case Directx11: {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      // Create window
      _window = glfwCreateWindow(deviceDesc.width, deviceDesc.height,
                                 "Imperium", nullptr, nullptr);

      ComPtr<ID3D11Device> device;
      ComPtr<ID3D11DeviceContext> context;

      D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0,
                                    D3D_FEATURE_LEVEL_11_1};

      // Create device
      D3D_FEATURE_LEVEL featureLevel;
      auto result = D3D11CreateDevice(
          nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
          &levels[0], 2, D3D11_SDK_VERSION, &device, &featureLevel, &context);

      if (!SUCCEEDED(result)) {
        throw std::runtime_error(
            "Unable to create DX11 device. Go to https://getfedora.org to "
            "solve the problem");
      }

      // Create swapchain
      DXGI_SWAP_CHAIN_DESC scd = {};
      scd.BufferDesc.Width = deviceDesc.width;
      scd.BufferDesc.Height = deviceDesc.height;
      scd.BufferDesc.RefreshRate.Denominator = 60;
      scd.BufferDesc.RefreshRate.Numerator = 1;
      scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

      scd.SampleDesc.Count = 1;
      scd.SampleDesc.Quality = 0;

      scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      scd.BufferCount = 1;
      scd.OutputWindow = glfwGetWin32Window(_window);
      scd.Windowed = TRUE;
      scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
      scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

      ComPtr<IDXGIDevice3> dxgiDevice;
      device.As(&dxgiDevice);
      ComPtr<IDXGIAdapter> adapter;
      ComPtr<IDXGIFactory> factory;
      ComPtr<IDXGISwapChain> swapChain;

      result = dxgiDevice->GetAdapter(&adapter);

      if (!SUCCEEDED(result)) {
        throw std::runtime_error(
            "Cannot get adapter from dxgi device. Go to https://getfedora.org "
            "to solve the problem");
      }

      adapter->GetParent(IID_PPV_ARGS(&factory));
      result = factory->CreateSwapChain(device.Get(), &scd, &swapChain);
      if (!SUCCEEDED(result)) {
        throw std::runtime_error(
            "Failed to create swapChain. Go to https://getfedora.org "
            "to solve the problem");
      }

      _backend = new BackendDx({deviceDesc.width, deviceDesc.height}, device,
                               context, swapChain);
      _api = ApiDesc::Directx11;
      break;
    }
#endif
    default: {
      throw std::runtime_error(
          "Backend not implemented or not compatible with this platform.");
    }
  }
}

DeviceDesktop::~DeviceDesktop() = default;

bool DeviceDesktop::ShouldClose() { return glfwWindowShouldClose(_window); }

void DeviceDesktop::RequestAnimationFrame() {
  glfwPollEvents();
  if (_api != ApiDesc::Directx11) {
    glfwSwapBuffers(_window);
  } else {
#if defined(_WIN32) || defined(WIN32)
    ((BackendDx *)_backend)->Present();
#endif
  }
}

void DeviceDesktop::Clear(GPURenderTarget renderTarget) {
  _backend->Clear(renderTarget);
}

GPUBuffer DeviceDesktop::CreateVertexBuffer(CPUBuffer<float> cpuBuffer) {
  // Describe and create buffer
  BufferCreationDesc desc = {};
  desc.purpose = BufferTypeDesc::VertexBuffer;
  desc.usage = BufferUsageDesc::StaticDraw;
  desc.size = sizeof(float) * cpuBuffer.stride * cpuBuffer.nbElements;
  desc.data = (void *)cpuBuffer.data;
  desc.stride = sizeof(float) * cpuBuffer.stride;

  assert(desc.size != 0);
  assert(desc.data != nullptr);

  auto buffer = _backend->CreateBuffer(desc);

  return buffer;
}

GPUBuffer DeviceDesktop::CreateIndexBuffer(CPUBuffer<int> cpuBuffer) {
  // Describe and create buffer
  BufferCreationDesc desc = {};
  desc.purpose = BufferTypeDesc::IndexBuffer;
  desc.usage = BufferUsageDesc::StaticDraw;
  desc.size = sizeof(int) * cpuBuffer.nbElements;
  desc.data = (void *)cpuBuffer.data;
  desc.stride = sizeof(int) * cpuBuffer.stride;

  auto buffer = _backend->CreateBuffer(desc);
  return buffer;
}

GPUBuffer DeviceDesktop::CreateUniformBuffer(CPUBuffer<void> cpuBuffer) {
  // Describe and create buffer
  BufferCreationDesc desc = {};
  desc.purpose = BufferTypeDesc::UniformBuffer;
  desc.usage = BufferUsageDesc::DynamicDraw;
  desc.size = cpuBuffer.size + (cpuBuffer.size % 16);
  desc.data = (void *)cpuBuffer.data;

  // Uniform buffer size has to be multiple of 16
  // But we prefer allocating some memory instead of requiring specific size
  // assert(desc.size % 16 == 0);

  auto buffer = _backend->CreateBuffer(desc);
  return buffer;
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

GPUDrawInput DeviceDesktop::CreateDrawInput(
    GPUInputLayout inputLayout, const std::vector<GPUBuffer> &vertexBuffers,
    GPUBuffer indexBuffer) {
  return _backend->CreateDrawInput(inputLayout, vertexBuffers, indexBuffer);
}

GPUInputLayout DeviceDesktop::CreateInputLayout(
    InputLayoutDesc inputLayoutDesc) {
  return _backend->CreateInputLayout(inputLayoutDesc);
}

GPUTexture DeviceDesktop::CreateEmptyTexture(TextureFormat format,
                                             TextureWrap wrap, int width,
                                             int height,
                                             TexturePrecision precision) {
  // Call the same backend method as CreateTextureFromData
  // but pass a null pointer, forcing it to upload nothing
  TextureCreationDesc textureCreationDesc = {};
  textureCreationDesc.data = nullptr;
  textureCreationDesc.width = width;
  textureCreationDesc.height = height;
  textureCreationDesc.format = format;
  textureCreationDesc.wrap = wrap;
  textureCreationDesc.precision = precision;

  return _backend->CreateTexture(textureCreationDesc);
}

GPUTexture DeviceDesktop::CreateTextureFromData(CPUTexture cpuTexture) {
  // By creating a texture from data we assume that user load an image from
  // disk. Therefore, some value are set by default as the texture will be used
  // to color some meshes.
  TextureCreationDesc textureCreationDesc = {};
  textureCreationDesc.data = cpuTexture.data;
  textureCreationDesc.width = cpuTexture.width;
  textureCreationDesc.height = cpuTexture.height;
  textureCreationDesc.format = cpuTexture.format;
  textureCreationDesc.wrap = TextureWrap::Repeat;
  textureCreationDesc.precision = TexturePrecision::Low;

  return _backend->CreateTexture(textureCreationDesc);
}

GPURenderTarget DeviceDesktop::CreateRenderTarget(
    const std::vector<GPUTexture> &textures, const GPUTexture &depth) {
  return _backend->CreateRenderTarget(textures, depth);
}

void DeviceDesktop::UpdateUniformBuffer(GPUBuffer buffer,
                                        CPUBuffer<void> newData) {
  // Describe how to modify
  BufferUpdateDesc updateDesc = {};
  updateDesc.buffer = &buffer;
  updateDesc.data = newData.data;
  updateDesc.size = newData.size;
  updateDesc.offset = 0;

  _backend->UpdateBuffer(updateDesc);
}

void DeviceDesktop::BindProgram(GPUProgram program) {
  _currentProgram = program;
  _backend->BindProgram(program);
}

void DeviceDesktop::BindTextures(GPUTexture *textures, int nbTextures,
                                 int offset) {
  for (int i = 0; i < nbTextures; i++) {
    _backend->BindTexture(textures[i], i + offset);
  }
}

void DeviceDesktop::BindRenderTarget(GPURenderTarget renderTarget) {
  _backend->BindRenderTarget(renderTarget);
}

void DeviceDesktop::BindUniformBuffer(GPUBuffer uniformBuffer, int layout) {
  _backend->BindUniformBuffer(uniformBuffer, layout);
}

void DeviceDesktop::Draw(GPUDrawInput drawInput, int count, int times,
                         GPUBuffer *uniformBuffers, size_t nbUniformBuffers,
                         int offsetUniform) {
  // Bind one time all uniform buffers
  for (int i = 0; i < nbUniformBuffers; ++i) {
    _backend->BindUniformBuffer(uniformBuffers[i], i + offsetUniform);
  }
  // Issue draw calls
  _backend->Draw(drawInput, count, times);
}

void DeviceDesktop::BlitRenderTarget(GPURenderTarget from, GPURenderTarget to) {
  // Dimensions of source and destination render target have to be identical
  assert(from.height == to.height && from.width == to.width);
  _backend->BlitRenderTarget(from, to);
}

GPUProgram DeviceDesktop::GetCurrentProgram() { return _currentProgram; }
