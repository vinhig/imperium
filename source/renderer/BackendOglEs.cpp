//
// Created by vincent on 2/10/20.
//

#include "BackendOglEs.h"

#ifdef __ANDROID__

#include <GLES3/gl3.h>

#else
#include <glad/glad.h>
#endif

float salut = 1.0f;

uint32_t BackendOglEs::CreateBuffer(void *data, size_t size) { return 0; }

uint32_t BackendOglEs::CreateProgram(char *name) { return 0; }

void BackendOglEs::Clear(uint32_t framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(salut, 1.0f - salut, salut, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    salut *= 0.99f;
}
