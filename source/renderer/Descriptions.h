//
// Created by vincent on 8/10/20.
//

#pragma once

#include <vector>

enum ApiDesc {
  OpenGL33,
  OpenGL46,
  OpenGLES32,
  Directx11,
};

enum DataType {
  Float,
  Int,
  Uint,
  Ushort,
  Mat4,
  Vec4,
  Vec3,
};

struct BackendDesc {
  int width;
  int height;
};

struct DeviceDesc {
  /**
   * Width of the window.
   */
  int width;
  /**
   * Height of the window.
   */
  int height;
  /**
   * Api requested.
   */
  ApiDesc api;
};

enum BufferTypeDesc {
  VertexBuffer,
  IndexBuffer,
  UniformBuffer,
};

enum BufferUsageDesc {
  StaticDraw,
  StaticRead,
  StaticCopy,
  DynamicDraw,
  DynamicRead,
  DynamicCopy,
  StreamDraw,
  StreamRead,
  StreamCopy,
};

struct BufferCreationDesc {
  BufferTypeDesc purpose;
  BufferUsageDesc usage;
  void* data;
  size_t size;
  size_t stride;
  // TODO: some mapping?
  // TODO: some readonly?
};

struct GPUBuffer;  // C++ bad
struct BufferUpdateDesc {
  GPUBuffer* buffer;
  void* data;
  size_t size;
  size_t offset;
};

struct InputLayoutEntryDesc {
  unsigned int index;
  int size;
  bool normalized;
  size_t stride;
  DataType subtype;
  void* offset;
};

struct GPUProgram;
struct InputLayoutDesc {
  GPUProgram* program;
  std::vector<InputLayoutEntryDesc> entries;
};