//
// Created by vincent on 2/10/20.
//

#include "BackendOgl.h"

#include <glad/glad.h>

uint32_t BackendOgl::CreateBuffer(void *data, size_t size) { return 0; }

uint32_t BackendOgl::CreateProgram(char *name) { return 0; }

void BackendOgl::Clear(uint32_t framebuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
