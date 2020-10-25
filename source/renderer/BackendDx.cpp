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

#include "../common/File.h"

BackendDx::BackendDx(BackendDesc backendDesc, ComPtr<ID3D11Device> device,
                     ComPtr<ID3D11DeviceContext> context,
                     ComPtr<IDXGISwapChain> swapChain) {
  // Copy resources
  _device = std::move(device);
  _context = std::move(context);
  _swapChain = std::move(swapChain);

  // Get backbuffer
  ComPtr<ID3D11Texture2D> _backBuffer;
  HRESULT result = _swapChain->GetBuffer(
      0, __uuidof(ID3D11Texture2D),
      reinterpret_cast<void**>(_backBuffer.GetAddressOf()));
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to get backbuffer.");
  }

  // Create renderTargetView and depthStencilView
  result = _device->CreateRenderTargetView(_backBuffer.Get(), nullptr,
                                           _renderTargetView.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create renderTargetView.");
  }

  D3D11_TEXTURE2D_DESC depthStencilDesc = {};
  depthStencilDesc.Width = backendDesc.width;
  depthStencilDesc.Height = backendDesc.height;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  result = _device->CreateTexture2D(&depthStencilDesc, nullptr,
                                    _depthStencilBuffer.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create depth stencil texture.");
  }

  result = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr,
                                           _depthStencilView.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create depth stencil view.");
  }

  //  _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(),
  //                               _depthStencilView.Get());

  D3D11_DEPTH_STENCIL_DESC depthStencilStatedesc = {};
  depthStencilStatedesc.DepthEnable = true;
  depthStencilStatedesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilStatedesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

  result = _device->CreateDepthStencilState(&depthStencilStatedesc,
                                            _depthStencilState.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create depth stencil state.");
  }

  // TODO: make opengl backend setup same parameters
  D3D11_VIEWPORT viewport = {};
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = backendDesc.width;
  viewport.Height = backendDesc.height;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;

  _context->RSSetViewports(1, &viewport);

  D3D11_RASTERIZER_DESC rasterizerDesc = {};
  rasterizerDesc.FillMode = D3D11_FILL_SOLID;
  rasterizerDesc.CullMode = D3D11_CULL_NONE;

  result = _device->CreateRasterizerState(&rasterizerDesc,
                                          _rasterizerState.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create rasterizer state.");
  }
  _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(),
                               _depthStencilView.Get());
  _context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _context->RSSetState(_rasterizerState.Get());
  // _context->OMSetDepthStencilState(_depthStencilState.Get(), 0);
}

void BackendDx::Clear(uint32_t framebuffer) {
  // Clear the backbuffer ?
  if (framebuffer == 0) {
    float bgColor[] = {1.0f, 0.61f, 1.0f, 1.0f};
    _context->ClearRenderTargetView(_renderTargetView.Get(), bgColor);
    _context->ClearDepthStencilView(_depthStencilView.Get(),
                                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                    1.0f, 0);
  }
}

GPUBuffer BackendDx::CreateBuffer(BufferCreationDesc bufferCreationDesc) {
  auto bufferUsage = D3D11_USAGE_DEFAULT;
  auto cpuAccess = 0;
  // TODO: make a correct switch that set correct cpuAccess value
  /*switch (bufferCreationDesc.usage) {
    case StaticDraw:
      bufferUsage = D3D11_USAGE_IMMUTABLE;
      break;
    case StaticRead:
      bufferUsage = D3D11_USAGE_IMMUTABLE;
      break;
    case StaticCopy:
      bufferUsage = D3D11_USAGE_IMMUTABLE;
      break;
    case DynamicDraw: {
      cpuAccess = D3D11_CPU_ACCESS_WRITE;
      bufferUsage = D3D11_USAGE_DYNAMIC;
      break;
    }
    case DynamicRead:
      bufferUsage = D3D11_USAGE_DYNAMIC;
      break;
    case DynamicCopy:
      bufferUsage = D3D11_USAGE_DYNAMIC;
      break;
    case StreamDraw:
      bufferUsage = D3D11_USAGE_DEFAULT;
      break;
    case StreamRead:
      bufferUsage = D3D11_USAGE_DEFAULT;
      break;
    case StreamCopy:
      bufferUsage = D3D11_USAGE_DEFAULT;
      break;
  }*/
  unsigned int bindFlags;
  switch (bufferCreationDesc.purpose) {
    case VertexBuffer:
      bindFlags = D3D11_BIND_VERTEX_BUFFER;
      break;
    case IndexBuffer:
      bindFlags = D3D11_BIND_INDEX_BUFFER;
      break;
    case UniformBuffer:
      bindFlags = D3D11_BIND_CONSTANT_BUFFER;
      break;
  }
  D3D11_BUFFER_DESC vertexBufferDesc = {};
  vertexBufferDesc.Usage = bufferUsage;
  vertexBufferDesc.ByteWidth = bufferCreationDesc.size;

  vertexBufferDesc.BindFlags = bindFlags;
  vertexBufferDesc.CPUAccessFlags = cpuAccess;
  vertexBufferDesc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA vertexBufferData = {};
  vertexBufferData.pSysMem = bufferCreationDesc.data;

  ComPtr<ID3D11Buffer> buffer;
  HRESULT result =
      _device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &buffer);
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create buffer.");
  }

  _buffers[_nbBuffers] = buffer;
  _nbBuffers++;

  return GPUBuffer{_nbBuffers - 1, bufferCreationDesc.stride};
}

ComPtr<ID3DBlob> DxCompileShader(const char* sourceCode, size_t length,
                                 const char* target) {
  ComPtr<ID3DBlob> blob;  // Actual binary shader
  ComPtr<ID3DBlob> errorMsg;
  HRESULT hr =
      D3DCompile(sourceCode, length, nullptr, nullptr, nullptr, "main", target,
#if _DEBUG
                 D3DCOMPILE_DEBUG,
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
  ComPtr<ID3DBlob> vertexBlob = DxCompileShader(
      vertexSourceCode.c_str(), vertexSourceCode.length(), "vs_5_0");
  ComPtr<ID3DBlob> fragmentBlob = DxCompileShader(
      fragmentSourceCode.c_str(), fragmentSourceCode.length(), "ps_5_0");

  // From that point pixel shader == fragment shader
  ComPtr<ID3D11PixelShader> pixelShader;
  HRESULT result = _device->CreatePixelShader(
      fragmentBlob->GetBufferPointer(), fragmentBlob->GetBufferSize(), nullptr,
      pixelShader.GetAddressOf());

  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create pixel shader.\n" + message);
  }

  ComPtr<ID3D11VertexShader> vertexShader;
  result = _device->CreateVertexShader(vertexBlob->GetBufferPointer(),
                                       vertexBlob->GetBufferSize(), nullptr,
                                       vertexShader.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create vertex shader.\n" + message);
  }

  _programs[_nbPrograms] = {vertexShader, pixelShader};
  _shadersCode[_nbPrograms] = {vertexBlob, fragmentBlob};
  _nbPrograms++;

  return GPUProgram{_nbPrograms - 1};
}

GPUDrawInput BackendDx::CreateDrawInput(
    GPUInputLayout inputLayout, const std::vector<GPUBuffer>& vertexBuffers,
    GPUBuffer indexBuffer) {
  // Remove double
  std::vector<GPUBuffer> _vertexBuffers;

  // For each given vertex buffers
  for (int i = 0; i < vertexBuffers.size(); i++) {
    bool contains = false;
    // For each already registered vertex buffers
    for (int j = 0; j < _vertexBuffers.size(); j++) {
      // Check if we do not add an already added vertex buffers
      // So check if they're the same
      if (_vertexBuffers[j].buffer == vertexBuffers[i].buffer) {
        contains = true;
        break;
      }
    }
    if (!contains) {
      _vertexBuffers.push_back(vertexBuffers[i]);
    }
  }

  _vaos[_nbVaos] = {_vertexBuffers, indexBuffer};
  _nbVaos++;

  return GPUDrawInput{inputLayout.inputLayout, _nbVaos - 1};
}

GPUInputLayout BackendDx::CreateInputLayout(InputLayoutDesc inputLayoutDesc) {
  auto layoutDesc = (D3D11_INPUT_ELEMENT_DESC*)malloc(
      sizeof(D3D11_INPUT_ELEMENT_DESC) * inputLayoutDesc.entries.size());
  int index = 0;
  for (auto const& entry : inputLayoutDesc.entries) {
    DXGI_FORMAT format;
    if (entry.subtype == DataType::Float) {
      switch (entry.size) {
        case 1:
          format = DXGI_FORMAT_R32_FLOAT;
          break;
        case 2:
          format = DXGI_FORMAT_R32G32_FLOAT;
          break;
        case 3:
          format = DXGI_FORMAT_R32G32B32_FLOAT;
          break;
        case 4:
          format = DXGI_FORMAT_R32G32B32A32_FLOAT;
          break;
        default:
          throw std::runtime_error("Forbidden data size in an input layout.");
      }
    } else {
      throw std::runtime_error("Forbidden data type in an input layout.");
    }
    layoutDesc[index] = {"TEXCOORD",
                         entry.index,
                         format,
                         0,
                         (unsigned int)entry.offset,
                         D3D11_INPUT_PER_VERTEX_DATA,
                         0};
    index++;
  }
  // Create input layout from vertex shader
  ComPtr<ID3D11InputLayout> inputLayout;
  auto vertexShader = _shadersCode[inputLayoutDesc.program->program].first;
  HRESULT result = _device->CreateInputLayout(
      layoutDesc, inputLayoutDesc.entries.size(),
      vertexShader.Get()->GetBufferPointer(),
      vertexShader.Get()->GetBufferSize(), inputLayout.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    throw std::runtime_error("Unable to create input layout.\n" + message);
  }

  delete[] layoutDesc;

  _inputLayouts[_nbInputLayouts] = inputLayout;
  _nbInputLayouts++;

  return GPUInputLayout{inputLayoutDesc, _nbInputLayouts - 1};
}

GPUTexture BackendDx::CreateTexture(TextureCreationDesc textureCreationDesc) {
  ComPtr<ID3D11Texture2D> texture;
  ComPtr<ID3D11SamplerState> samplerState;
  ComPtr<ID3D11ShaderResourceView> shaderResourceView;

  auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
  auto pitch = 4;
  switch (textureCreationDesc.format) {
    case R:
      format = DXGI_FORMAT_R8_UNORM;
      pitch = 1;
      break;
    case RG:
      format = DXGI_FORMAT_R8G8_UNORM;
      pitch = 2;
      break;
    case RGB:
      // oh fuck
      // where is my DXGI_FORMAT_R8G8B8_UNORM ?
      format = DXGI_FORMAT_R8G8B8A8_UNORM;
      pitch = 3;
      break;
    case RGBA:
      format = DXGI_FORMAT_R8G8B8A8_UNORM;
      pitch = 4;
      break;
    case DEPTH:
      break;
  }

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = textureCreationDesc.width;
  desc.Height = textureCreationDesc.height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = format;
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  desc.MiscFlags = 0;
  desc.SampleDesc = {};
  desc.SampleDesc.Count = 1;

  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

  D3D11_SUBRESOURCE_DATA textureData = {};
  textureData.pSysMem = textureCreationDesc.data;
  textureData.SysMemPitch = textureCreationDesc.width * 4;
  textureData.SysMemSlicePitch =
      textureCreationDesc.width * textureCreationDesc.height * 4;
  assert(textureCreationDesc.data != nullptr);

  // D3D11_SHADER_RESOURCE_VIEW_DESC* shaderResourceViewDesc = {};
  // shaderResourceViewDesc->Format = format;

  HRESULT result = _device->CreateTexture2D(&desc, &textureData, &texture);
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    std::cout << message << std::endl;
    throw std::runtime_error("Unable to create texture.");
  }
  result = _device->CreateSamplerState(&sampDesc, &samplerState);
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    std::cout << message << std::endl;
    throw std::runtime_error("Unable to create sampler.");
  }

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;

  result = _device->CreateShaderResourceView(texture.Get(), &srvDesc,
                                             shaderResourceView.GetAddressOf());
  if (FAILED(result)) {
    std::string message = std::system_category().message(result);
    std::cout << message << std::endl;
    throw std::runtime_error("Unable to create shader resource view.");
  }

  _textures[_nbTextures] = {texture, samplerState, shaderResourceView};
  _nbTextures++;

  return GPUTexture{_nbTextures - 1, textureCreationDesc.width,
                    textureCreationDesc.height};
}

void BackendDx::BindProgram(GPUProgram program) {
  auto shaders = _programs[program.program];
  _context->VSSetShader(shaders.first.Get(), nullptr, 0);
  _context->PSSetShader(shaders.second.Get(), nullptr, 0);
}

void BackendDx::BindTexture(GPUTexture texture, int index) {
  _context->PSSetShaderResources(
      index, 1, _textures[texture.texture].shaderResource.GetAddressOf());
  _context->PSSetSamplers(
      index, 1, _textures[texture.texture].samplerState.GetAddressOf());
}

void BackendDx::BindTextures(const std::vector<GPUTexture>& texture,
                             int index) {}

void BackendDx::Draw(GPUDrawInput drawInput, int count, int times,
                     GPUBuffer* uniformBuffers, size_t nbUniformBuffers) {
  _context->IASetInputLayout(_inputLayouts[drawInput.inputLayout].Get());

  unsigned int stride = 0;
  unsigned int offset = 0;

  // Set an array of vertex buffers
  std::vector<ID3D11Buffer*> vertexBuffers(_vaos[drawInput.vao].first.size());
  for (int i = 0; i < _vaos[drawInput.vao].first.size(); i++) {
    // Get the corresponding buffer
    // targeted by the index `_vaos[drawInput.vao].first[i].buffer`
    // (a kinda awful line i know)
    vertexBuffers[i] = _buffers[_vaos[drawInput.vao].first[i].buffer].Get();
    stride += _vaos[drawInput.vao].first[i].stride;
  }
  _context->IASetVertexBuffers(0, _vaos[drawInput.vao].first.size(),
                               vertexBuffers.data(), &stride, &offset);

  // Set an array of uniform buffers
  // (they're called constant buffers in directx)
  std::vector<ID3D11Buffer*> constantBuffers(nbUniformBuffers);
  for (int j = 0; j < nbUniformBuffers; ++j) {
    constantBuffers[j] = _buffers[uniformBuffers[j].buffer].Get();
  }
  _context->VSSetConstantBuffers(0, nbUniformBuffers, constantBuffers.data());

  // Set the only index buffer
  _context->IASetIndexBuffer(_buffers[_vaos[drawInput.vao].second.buffer].Get(),
                             DXGI_FORMAT_R32_UINT, 0);
  // _context->DrawInstanced(count, times, 0, 0);

  if (times == 1) {
    _context->DrawIndexed(count, 0, 0);
  } else {
    _context->DrawIndexedInstanced(count, times, 0, 0, 0);
  }
}

void BackendDx::UpdateBuffer(BufferUpdateDesc updateDesc) {
  // We don't care about nature of buffer
  auto buffer = _buffers[updateDesc.buffer->buffer].Get();
  _context->UpdateSubresource(buffer, 0, nullptr, updateDesc.data, 0, 0);
}

void BackendDx::Present() {
  _swapChain->Present(1, 0);
  _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(),
                               _depthStencilView.Get());
}
