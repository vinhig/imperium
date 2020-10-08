//
// Created by vincent on 8/10/20.
//

#pragma once

enum ApiDesc {
  OpenGL33,
  OpenGL46,
  OpenGLES32,
  Directx11,
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