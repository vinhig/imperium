//
// Created by vincent on 8/10/20.
//

#pragma once

#include "GPUResources.h"

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
  // TODO: some mapping?
  // TODO: some readonly?
};

struct InputLayoutEntryDesc {
  unsigned int index;
  int size;
  bool normalized;
  size_t stride;
  GPUBuffer buffer;
  DataType subtype;
  void* offset;
};

struct InputLayoutDesc {
  std::vector<InputLayoutEntryDesc> entries;
  GPUBuffer indexBuffer;
};