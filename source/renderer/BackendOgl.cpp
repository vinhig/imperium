//
// Created by vincent on 2/10/20.
//

#include "BackendOgl.h"

#include <glad/glad.h>

#include <iostream>
#include <spirv_glsl.hpp>

#include "../common/Log.h"

BackendOgl::BackendOgl(BackendDesc backendDesc) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glViewport(0, 0, backendDesc.width, backendDesc.height);
}

GPUBuffer BackendOgl::CreateBuffer(BufferCreationDesc bufferCreationDesc) {
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

uint32_t CompileShader(const char* source, GLenum shaderType) {
  LOG_DEBUG("Compiling shader...")
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
    std::cout << source << std::endl;
    std::cout << errorMsg << std::endl;
    throw std::runtime_error(errorMsg);
  }

  return shader;
}

GPUProgram BackendOgl::CreateProgram(std::vector<uint32_t> vertexSource,
                                     std::vector<uint32_t> fragmentSource) {
  LOG_DEBUG("Creating program...")
  // Transpile spirv to glsl 330
  std::string vertexSourceCode;
  std::string fragmentSourceCode;
  {
    spirv_cross::CompilerGLSL glsl(std::move(vertexSource));

    spirv_cross::CompilerGLSL::Options options;
    options.version = 330;
    options.es = false;
    glsl.set_common_options(options);

    vertexSourceCode = glsl.compile();
  }
  {
    spirv_cross::CompilerGLSL glsl(std::move(fragmentSource));

    spirv_cross::CompilerGLSL::Options options;
    options.version = 330;
    options.es = false;
    glsl.set_common_options(options);

    fragmentSourceCode = glsl.compile();
  }
  // Compile shaders
  auto vertexShader = CompileShader(vertexSourceCode.c_str(), GL_VERTEX_SHADER);
  auto fragmentShader =
      CompileShader(fragmentSourceCode.c_str(), GL_FRAGMENT_SHADER);

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

void BackendOgl::Clear(GPURenderTarget renderTarget) {
  glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.framebuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GPUDrawInput BackendOgl::CreateDrawInput(
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

GPUInputLayout BackendOgl::CreateInputLayout(InputLayoutDesc inputLayoutDesc) {
  // Creating an input layout doesn't do anything
  // as it's represented by a vertex array object that links directly
  // to vertex buffers and index buffer.
  // We're just going to pass the InputLayoutDesc with a null value.
  return GPUInputLayout{inputLayoutDesc, 0};
}

GPUTexture BackendOgl::CreateTexture(TextureCreationDesc textureCreationDesc) {
  // A texture cannot be 0x0
  assert(textureCreationDesc.width && textureCreationDesc.height);

  uint32_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  auto wrap = GL_MIRRORED_REPEAT;
  switch (textureCreationDesc.wrap) {
    case Repeat:
      wrap = GL_REPEAT;
      break;
    case MirrorRepeat:
      wrap = GL_MIRRORED_REPEAT;
      break;
    case ClampToEdge:
      wrap = GL_CLAMP_TO_EDGE;
      break;
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  auto format = GL_RGB;
  auto internalFormat = GL_RGB;
  auto subType = GL_UNSIGNED_BYTE;
  switch (textureCreationDesc.format) {
    case R: {
      if (textureCreationDesc.precision == TexturePrecision::Low) {
        internalFormat = GL_R;
      } else {
        internalFormat = GL_R16F;
        subType = GL_FLOAT;
      }
      format = GL_R;
      break;
    }
    case RG: {
      if (textureCreationDesc.precision == TexturePrecision::Low) {
        internalFormat = GL_RG;
      } else {
        internalFormat = GL_RG16F;
        subType = GL_FLOAT;
      }
      format = GL_RG;
      break;
    }
    case RGB: {
      if (textureCreationDesc.precision == TexturePrecision::Low) {
        internalFormat = GL_RGB;
      } else {
        internalFormat = GL_RGB16F;
        subType = GL_FLOAT;
      }
      format = GL_RGB;
      break;
    }
    case RGBA: {
      if (textureCreationDesc.precision == TexturePrecision::Low) {
        internalFormat = GL_RGBA;
      } else {
        internalFormat = GL_RGBA16F;
        subType = GL_FLOAT;
      }
      format = GL_RGBA;
      break;
    }
    case DEPTH: {
      format = GL_DEPTH_COMPONENT;
      internalFormat = GL_DEPTH_COMPONENT;
      subType = GL_FLOAT;
      break;
    }
  }
  // Upload
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureCreationDesc.width,
               textureCreationDesc.height, 0, format, subType,
               textureCreationDesc.data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return GPUTexture{texture, textureCreationDesc.width,
                    textureCreationDesc.height};
}

GPURenderTarget BackendOgl::CreateRenderTarget(
    const std::vector<GPUTexture>& colors, GPUTexture depth) {
  // All attached textures have to share the same dimensions
  int checkWidth = 0;
  int checkHeight = 0;
  for (const auto& color : colors) {
    if (checkWidth && checkHeight) {
      assert(checkWidth == color.width && checkHeight == color.height);
    } else {
      checkWidth = color.width;
      checkHeight = color.height;
    }
  }

  // Create framebuffer
  uint32_t framebuffer = 0;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  // Bind each color buffer
  auto i = 0;
  std::vector<GLenum> colorBuffers;
  for (const auto& color : colors) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, color.texture, 0);
    colorBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    i++;
  }
  // DON'T FORGET THIS
  glDrawBuffers(colors.size(), colorBuffers.data());

  // Bind depth buffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth.texture, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Width and Height of render target
  // In case we do not have any attached color texture
  if (!(checkWidth && checkHeight)) {
    checkWidth = depth.height;
    checkHeight = depth.height;
  }

  return GPURenderTarget{framebuffer, checkWidth, checkHeight};
}

void BackendOgl::BindProgram(GPUProgram program) {
  glUseProgram(program.program);
}

void BackendOgl::BindTexture(GPUTexture texture, int index) {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, texture.texture);
}

void BackendOgl::BindTextures(const std::vector<GPUTexture>& texture,
                              int index) {}

void BackendOgl::BindRenderTarget(GPURenderTarget renderTarget) {
  glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.framebuffer);
  glViewport(0, 0, renderTarget.width, renderTarget.height);
}

void BackendOgl::BindUniformBuffer(GPUBuffer uniformBuffer, int layout) {
  glBindBufferBase(GL_UNIFORM_BUFFER, layout, uniformBuffer.buffer);
}

void BackendOgl::Draw(GPUDrawInput drawInput, int count, int times) {
  glBindVertexArray(drawInput.vao);

  if (times == 1) {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
  } else {
    glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr,
                            times);
  }
}

void BackendOgl::BlitRenderTarget(GPURenderTarget from, GPURenderTarget to) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, from.framebuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.framebuffer);
  glBlitFramebuffer(0, 0, from.width, from.height, 0, 0, to.width, to.height,
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void BackendOgl::UpdateBuffer(BufferUpdateDesc updateDesc) {
  // Vertex and index buffer can't be modified (at the time being)
  // We can bind it as an Uniform Buffer
  assert(updateDesc.data != nullptr);
  glBindBuffer(GL_UNIFORM_BUFFER, updateDesc.buffer->buffer);
  glBufferSubData(GL_UNIFORM_BUFFER, updateDesc.offset, updateDesc.size,
                  updateDesc.data);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
