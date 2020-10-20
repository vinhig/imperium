//
// Created by vincent on 8/10/20.
//

#pragma once

#include <vector>

/**
 * Give a human readable name to each API.
 */
enum ApiDesc {
  OpenGL33,
  OpenGL46,
  OpenGLES32,
  Directx11,
};

/**
 * Give a human readable name to each data type.
 */
enum DataType {
  Float,
  Int,
  Uint,
  Ushort,
  Mat4,
  Vec4,
  Vec3,
};

/**
 * Describe channels of a texture.
 * Eg: RGBA -> the image has three channels red/green/blue/alpha
 *     RG   -> the image has two channels red/green
 *     (...)
 */
enum TextureFormat {
  R,
  RG,
  RGB,
  RGBA,
  DEPTH,
};

enum TextureWrap {
  Repeat,
  MirrorRepeat,
  ClampToEdge,
};

/**
 * Describe backend before his creation.
 */
struct BackendDesc {
  /**
   * Width of the previously created window.
   */
  int width;
  /**
   * Width of the previously created window.
   */
  int height;
};

/**
 * Describe device before his creation.
 */
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

/**
 * Characterize how the buffer will be used.
 */
enum BufferTypeDesc {
  /**
   * Contains vertices from a mesh.
   */
  VertexBuffer,
  /**
   * contains indices targeting vertices from another buffer. Indices aren't
   * pointer but unsigned int. They act like array indices.
   */
  IndexBuffer,
  UniformBuffer,
};

/**
 * Describe future data manipulation on this buffer.
 * (This enum is just used in the OpenGLs backend.)
 */
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

/**
 * Raw description of a CPUBuffer before his upload on the GPU.
 */
struct BufferCreationDesc {
  BufferTypeDesc purpose;
  BufferUsageDesc usage;
  void* data;
  size_t size;
  size_t stride;
};

/**
 * Raw description of a CPUTexture before his upload on the GPU.
 */
struct TextureCreationDesc {
  unsigned char* data;
  int width;
  int height;
  TextureFormat format;
  TextureWrap wrap;
};

/**
 * Raw description of a manipulation of data that will be applied on a
 * GPUBuffer.
 */
struct GPUBuffer;  // C++ bad
struct BufferUpdateDesc {
  GPUBuffer* buffer;
  void* data;
  size_t size;
  size_t offset;
};

/**
 * Entry of an input layout. We cannot make more explicit than "entry".
 * Vertices from buffer are split into multiple "entries".
 * Vertices: 1.0 1.0 1.0 0.2 0.3 0.5 0.2 0.3 ...
 * With three entries: position[size=3, index=0, stride=8]
 *                     normal  [size=3, index=1, stride=8]
 *                     uv      [size=2, index=2, stride=8]
 * Vertex Shader will be fed with
 *     [binding=0] position = vec3(1.0, 1.0, 1.0)
 *     [binding=1] normal   = vec3(0.2, 0.3, 0.5)
 *     [binding=2] uv       = vec2(0.2, 0.3)
 * for each vertex.
 */
struct InputLayoutEntryDesc {
  unsigned int index;
  int size;
  bool normalized;
  size_t stride;
  DataType subtype;
  void* offset;
};

/**
 * List of input layout entries linked to a program (to a vertex shader more
 * precisely).
 */
struct GPUProgram;
struct InputLayoutDesc {
  GPUProgram* program;
  std::vector<InputLayoutEntryDesc> entries;
};