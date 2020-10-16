//
// Created by vincent on 2/10/20.
//

#include "BackendOglEs.h"

#ifdef __ANDROID__

#include <GLES3/gl3.h>

#else
#include <glad/glad.h>

#include <iostream>
#endif

#include <spirv_glsl.hpp>

#include "../common/Log.h"

BackendOglEs::BackendOglEs(BackendDesc backendDesc) {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  if (backendDesc.width != 0 && backendDesc.height != 0) {
    glViewport(0, 0, backendDesc.width, backendDesc.height);
  }
}

// TODO: okay we have two times the same compile shader function
uint32_t SecretCompileShader(const char* source, GLenum shaderType) {
  // Compile shader
  uint32_t shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  // Check result
  int result = GL_FALSE;
  int infoLength;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
  if (infoLength > 0) {
    char* errorMsg = new char[infoLength + 1];
    glGetShaderInfoLog(shader, infoLength, nullptr, &errorMsg[0]);
    // std::cout << source << std::endl;
    // std::cout << errorMsg << std::endl;
    throw std::runtime_error(errorMsg);
  }

  return shader;
}

GPUBuffer BackendOglEs::CreateBuffer(BufferCreationDesc bufferCreationDesc) {
  LOG_DEBUG("Creating buffer...")
  GLenum bufferType;
  GLenum bufferUsage;
  switch (bufferCreationDesc.purpose) {
    case VertexBuffer:
      bufferType = GL_ARRAY_BUFFER;
      break;
    case IndexBuffer:
      bufferType = GL_ELEMENT_ARRAY_BUFFER;
      break;
    case UniformBuffer:
      bufferType = GL_UNIFORM_BUFFER;
      break;
  }
  switch (bufferCreationDesc.usage) {
    case StaticDraw:
      bufferUsage = GL_STATIC_DRAW;
      break;
    case StaticRead:
      bufferUsage = GL_STATIC_READ;
      break;
    case StaticCopy:
      bufferUsage = GL_STATIC_COPY;
      break;
    case DynamicDraw:
      bufferUsage = GL_STATIC_DRAW;
      break;
    case DynamicRead:
      bufferUsage = GL_DYNAMIC_READ;
      break;
    case DynamicCopy:
      bufferUsage = GL_DYNAMIC_COPY;
      break;
    case StreamDraw:
      bufferUsage = GL_STREAM_DRAW;
      break;
    case StreamRead:
      bufferUsage = GL_STREAM_READ;
      break;
    case StreamCopy:
      bufferUsage = GL_STREAM_COPY;
      break;
  }
  uint32_t buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(bufferType, buffer);
  glBufferData(bufferType, bufferCreationDesc.size, bufferCreationDesc.data,
               bufferUsage);
  glBindBuffer(bufferType, 0);
  return GPUBuffer{buffer, bufferCreationDesc.stride};
}

GPUProgram BackendOglEs::CreateProgram(std::vector<uint32_t> vertexSource,
                                       std::vector<uint32_t> fragmentSource) {
  // Transpile spirv to glsl 330
  std::string vertexSourceCode;
  std::string fragmentSourceCode;
  {
    spirv_cross::CompilerGLSL glsl(std::move(vertexSource));

    spirv_cross::CompilerGLSL::Options options;
    options.version = 300;
    options.es = true;
    glsl.set_common_options(options);

    vertexSourceCode = glsl.compile();
  }
  {
    spirv_cross::CompilerGLSL glsl(std::move(fragmentSource));

    spirv_cross::CompilerGLSL::Options options;
    options.version = 300;
    options.es = true;
    glsl.set_common_options(options);

    fragmentSourceCode = glsl.compile();
  }
  // Compile shaders
  auto vertexShader =
      SecretCompileShader(vertexSourceCode.c_str(), GL_VERTEX_SHADER);
  auto fragmentShader =
      SecretCompileShader(fragmentSourceCode.c_str(), GL_FRAGMENT_SHADER);

  // Create program
  // and link compiled shaders
  auto program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  // Delete shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return GPUProgram{program};
}

void BackendOglEs::Clear(uint32_t framebuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GPUDrawInput BackendOglEs::CreateDrawInput(
    GPUInputLayout inputLayout, const std::vector<GPUBuffer>& vertexBuffers,
    GPUBuffer indexBuffer) {
  LOG_DEBUG("Creating draw input...")
  uint32_t vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.buffer);
  int index = 0;
  for (auto const& entry : inputLayout.cpuInputLayout.entries) {
    glEnableVertexAttribArray(entry.index);
    auto b = vertexBuffers[index].buffer;
    glBindBuffer(GL_ARRAY_BUFFER, b);
    GLenum subtype;
    switch (entry.subtype) {
      case Float:
        subtype = GL_FLOAT;
        break;
      default:
        throw std::runtime_error("Forbidden data type in a draw input.");
    }
    glVertexAttribPointer(entry.index, entry.size, subtype,
                          entry.normalized ? GL_TRUE : GL_FALSE, entry.stride,
                          entry.offset);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    index++;
  }
  glBindVertexArray(0);

  // Again, referencing an inputLayout is just useful for the directx 11 backend
  // We're passing a null value there
  return GPUDrawInput{0, vao};
}

GPUInputLayout BackendOglEs::CreateInputLayout(
    InputLayoutDesc inputLayoutDesc) {
  // View remark on namesake method from the OpenGL 3.3 backend
  return GPUInputLayout{inputLayoutDesc, 0};
}

void BackendOglEs::BindProgram(GPUProgram program) {
  glUseProgram(program.program);
}

void BackendOglEs::Draw(GPUDrawInput drawInput, int count, int times,
                        GPUBuffer* uniformBuffers, size_t nbUniformBuffers) {
  glBindVertexArray(drawInput.vao);
  for (int i = 0; i < nbUniformBuffers; ++i) {
    glBindBufferBase(GL_UNIFORM_BUFFER, i, uniformBuffers->buffer);
  }
  if (times == 1) {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
  } else {
    glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr,
                            times);
  }
}
