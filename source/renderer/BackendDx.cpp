//
// Created by vinhi on 12/10/2020.
//

#include "BackendDx.h"

#include <d3dcompiler.h>

#include <iostream>
#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>
#include <system_error>
#include <utility>

BackendDx::BackendDx(ComPtr<ID3D11Device> device,
                     ComPtr<ID3D11DeviceContext> context,
                     ComPtr<IDXGISwapChain> swapChain) {
  // Copy resources
  _device = std::move(device);
  _context = std::move(context);
  _swapChain = std::move(swapChain);
}

void BackendDx::Clear(uint32_t framebuffer) {}

GPUBuffer BackendDx::CreateBuffer(BufferCreationDesc bufferCreationDesc) {
  return GPUBuffer{};
}

ComPtr<ID3DBlob> DxCompileShader(const char* sourceCode, size_t length,
                                 const char* target) {
  ComPtr<ID3DBlob> blob;  // Actual binary shader
  ComPtr<ID3DBlob> errorMsg;
  HRESULT hr =
      D3DCompile(sourceCode, length, nullptr, nullptr, nullptr, "main", target,
#if _DEBUG
                 D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
#else
                 D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
                 0, &blob, &errorMsg);
  if (FAILED(hr)) {
    std::string message = std::system_category().message(hr);
    std::string error((char*)errorMsg->GetBufferPointer());
    std::cout << message << std::endl;
    printf("%s", sourceCode);
    std::cout << error << std::endl;
    throw std::runtime_error("Unable to compile hlsl.");
  }

  return blob;
}

GPUProgram BackendDx::CreateProgram(std::vector<uint32_t> vertexSource,
                                    std::vector<uint32_t> fragmentSource) {
  // Compiling a shader for DirectX is a bit more verbose
  // than compiling one for OpenGL.
  // To solve that problem: https://getfedora.org

  // Transpile to plain hlsl source code
  // Same step for every BackendSomething::CreateProgram
  std::string vertexSourceCode;
  std::string fragmentSourceCode;
  {
    spirv_cross::CompilerHLSL hlsl(std::move(vertexSource));

    spirv_cross::CompilerHLSL::Options options;
    options.shader_model = 50;
    hlsl.set_hlsl_options(options);

    vertexSourceCode = hlsl.compile();
  }
  {
    spirv_cross::CompilerHLSL hlsl(std::move(fragmentSource));

    spirv_cross::CompilerHLSL::Options options;
    options.shader_model = 50;
    hlsl.set_hlsl_options(options);

    fragmentSourceCode = hlsl.compile();
  }

  // Compile shader using D3DCompiler
  ComPtr<ID3DBlob> fragmentBlob = DxCompileShader(
      fragmentSourceCode.c_str(), fragmentSourceCode.length(), "ps_5_0");
  ComPtr<ID3DBlob> vertexBlob = DxCompileShader(
      vertexSourceCode.c_str(), vertexSourceCode.length(), "vs_5_0");

  // From that point pixel shader == fragment shader
  ComPtr<ID3D11PixelShader> pixelShader;
  HRESULT result = _device->CreatePixelShader(fragmentBlob->GetBufferPointer(),
                                              fragmentBlob->GetBufferSize(),
                                              nullptr, &pixelShader);

  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create pixel shader.\n" + message);
  }

  ComPtr<ID3D11VertexShader> vertexShader;
  result = _device->CreateVertexShader(vertexBlob->GetBufferPointer(),
                                       vertexBlob->GetBufferSize(), nullptr,
                                       &vertexShader);
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create vertex shader.\n" + message);
  }

  _programs[_nbPrograms] = {vertexShader, pixelShader};
  _nbPrograms++;

  return GPUProgram{_nbPrograms - 1};
}

GPUDrawInput BackendDx::CreateDrawInput(InputLayoutDesc inputLayoutDesc) {
  return GPUDrawInput();
}

void BackendDx::BindProgram(GPUProgram program) {}

void BackendDx::Draw(GPUDrawInput drawInput, int count, int times,
                     GPUBuffer* uniformBuffers, size_t nbUniformBuffers) {}

void BackendDx::Present() { _swapChain->Present(1, DXGI_PRESENT_DO_NOT_WAIT); }
