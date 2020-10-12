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

float salut = 1.0f;

GPUBuffer BackendOglEs::CreateBuffer(BufferCreationDesc bufferCreationDesc) {
  return GPUBuffer{};
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

uint32_t BackendOglEs::CreateProgram(std::vector<uint32_t> vertexSource,
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

  return program;
}

void BackendOglEs::Clear(uint32_t framebuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glClearColor(salut, 1.0f - salut, salut, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  salut *= 0.99f;
}
