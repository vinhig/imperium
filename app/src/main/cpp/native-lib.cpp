#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <jni.h>
#include <loader/MeshLoader.h>
#include <ofbx.h>
#include <renderer/Descriptions.h>
#include <renderer/DeviceAndroid.h>

#include <cmath>
#include <cstdio>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <string>
#include <thread>

#define APPNAME "Imperium"

DeviceAndroid *g_device = nullptr;
AAssetManager *g_assetManager = nullptr;

GPUProgram program;

GPUBuffer vertexBuffer;
GPUBuffer indexBuffer;
GPUBuffer uniformBuffer;
GPUDrawInput drawInput;

GPUBuffer uniformBuffers[2];

CPUBuffer<void> cpuBuffer3;
CPUBuffer<void> cpuBuffer4;

glm::mat4 projection, view, model, mvp;

struct Material {
  float colors[4];
  float mvp[16];
  float model[16];
  float ambient;
  float specular;
};

Material material;

struct Lights {
  float camera[4];  // vec3 have the same width as vec4
  float positions[3][3];
};

Lights lights;

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(
    JNIEnv *env, jclass clazz) {
  // Get screen size
  int sizes[4];
  glGetIntegerv(GL_VIEWPORT, &sizes[0]);

  // OpenGL context created from activity
  // Create device
  g_device = new DeviceAndroid(ApiDesc::OpenGLES32, sizes[2], sizes[3]);

  // Set file reader
  auto fileReader = [](std::string path) -> std::vector<unsigned char> {
    // Open file
    AAsset *file =
        AAssetManager_open(g_assetManager, path.c_str(), AASSET_MODE_BUFFER);
    if (!file) {
      throw std::runtime_error("Unable to open file.");
    }
    // Read file
    size_t fileLength = AAsset_getLength(file);
    auto fileContent = new unsigned char[fileLength + 1];
    AAsset_read(file, fileContent, fileLength);
    std::vector<unsigned char> content;
    content.reserve(fileLength);
    for (int i = 0; i < fileLength; i++) {
      content.push_back(fileContent[i]);
    }
    // fileContent[fileLength] = '\0';

    return content;
  };

  g_device->SetFileReader(fileReader);

  program = g_device->CreateProgram("basic");

  auto meshLoader = new MeshLoader();
  auto sphere = meshLoader->Load("sphere.fbx", fileReader);

  /*float vertices[] = {
      .5f,  .5f,  .5f,  1.0f, 0.2f, 0.3f,
      -.5f, .5f,  .5f,  0.3f, 0.6f, 1.0f,  // position and color
      -.5f, -.5f, .5f,  1.0f, 0.2f, 0.3f,
      .5f,  -.5f, .5f,  0.3f, 0.6f, 1.0f,  // v0,v1,v2,v3 (front)
      .5f,  .5f,  .5f,  1.0f, 0.2f, 0.3f,
      .5f,  -.5f, .5f,  0.3f, 0.6f, 1.0f,  // position and color
      .5f,  -.5f, -.5f, 1.0f, 0.2f, 0.3f,
      .5f,  .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // v0,v3,v4,v5 (right)
      .5f,  .5f,  .5f,  1.0f, 0.2f, 0.3f,
      .5f,  .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      -.5f, .5f,  -.5f, 1.0f, 0.2f, 0.3f,
      -.5f, .5f,  .5f,  0.3f, 0.6f, 1.0f,  // v0,v5,v6,v1 (top)
      -.5f, .5f,  .5f,  1.0f, 0.2f, 0.3f,
      -.5f, .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      -.5f, -.5f, -.5f, 1.0f, 0.2f, 0.3f,
      -.5f, -.5f, .5f,  0.3f, 0.6f, 1.0f,  // v1,v6,v7,v2 (left)
      -.5f, -.5f, -.5f, 0.7f, 0.2f, 0.3f,
      .5f,  -.5f, -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      .5f,  -.5f, .5f,  0.7f, 0.2f, 0.3f,
      -.5f, -.5f, .5f,  0.3f, 0.6f, 1.0f,  // v7,v4,v3,v2 (bottom)
      .5f,  -.5f, -.5f, 0.7f, 0.2f, 0.3f,
      -.5f, -.5f, -.5f, 0.3f, 0.6f, 1.0f,  // position and color
      -.5f, .5f,  -.5f, 0.7f, 0.2f, 0.3f,
      .5f,  .5f,  -.5f, 0.3f, 0.6f, 1.0f,  // v4,v7,v6,v5 (back)
  };

  int indices[] = {
      0,  1,  2,  2,  3,  0,   // v0-v1-v2, v2-v3-v0 (front)
      4,  5,  6,  6,  7,  4,   // v0-v3-v4, v4-v5-v0 (right)
      8,  9,  10, 10, 11, 8,   // v0-v5-v6, v6-v1-v0 (top)
      12, 13, 14, 14, 15, 12,  // v1-v6-v7, v7-v2-v1 (left)
      16, 17, 18, 18, 19, 16,  // v7-v4-v3, v3-v2-v7 (bottom)
      20, 21, 22, 22, 23, 20   // v4-v7-v6, v6-v5-v4 (back)
  };*/

  // Is it a problem if i want to rotate my cute cubic cube ?
  auto position = glm::vec3(3.0f, 3.0f, 3.0f);
  auto lookAt = glm::vec3(0.0f, 0.0f, 0.0f);

  projection =
      glm::perspective(70.0f, (float)sizes[2] / (float)sizes[3], 0.1f, 100.0f);
  view = glm::lookAt(position, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::mat4(1.0f);

  mvp = projection * view * model;

  material = {};
  material.colors[0] = 0.8;
  material.colors[1] = 0.8;
  material.colors[2] = 0.0;
  material.colors[3] = 1.0;
  material.ambient = 0.1;
  material.specular = 0.1;
  memcpy(&material.mvp[0], &mvp[0][0], 16 * sizeof(float));
  memcpy(&material.model[0], &model[0][0], 16 * sizeof(float));

  cpuBuffer3 = {};
  cpuBuffer3.data = (void *)&material;
  cpuBuffer3.size = sizeof(Material);

  // Create data to draw
  auto vertexBuffer = g_device->CreateVertexBuffer(sphere[0].first);
  auto indexBuffer = g_device->CreateIndexBuffer(sphere[0].second);
  auto uniformBuffer1 = g_device->CreateUniformBuffer(cpuBuffer3);

  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back({0, 3, false, sizeof(float) * 8,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 0)});
  inputLayoutDesc.entries.push_back({1, 3, false, sizeof(float) * 8,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 3)});
  inputLayoutDesc.entries.push_back({2, 2, false, sizeof(float) * 8,
                                     DataType::Float,
                                     (void *)(sizeof(float) * 6)});

  auto inputLayout = g_device->CreateInputLayout(inputLayoutDesc);

  // Specify what to draw
  // Specify what to draw
  std::vector<GPUBuffer> buffers(3);
  buffers[0] = vertexBuffer;
  buffers[1] = vertexBuffer;
  buffers[2] = vertexBuffer;
  drawInput = g_device->CreateDrawInput(inputLayout, buffers, indexBuffer);

  // Let's light it up
  // - Lux, 2020

  lights = {};

  lights.camera[0] = 3.0f;
  lights.camera[1] = 3.0f;
  lights.camera[2] = 3.0f;

  lights.positions[0][0] = 1.0f;
  lights.positions[0][1] = 0.0f;
  lights.positions[0][2] = 0.0f;

  cpuBuffer4 = {};
  cpuBuffer4.data = (void *)&lights;
  cpuBuffer4.size = sizeof(Lights);

  auto uniformBuffer2 = g_device->CreateUniformBuffer(cpuBuffer4);

  uniformBuffers[0] = uniformBuffer1;
  uniformBuffers[1] = uniformBuffer2;
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1changed(
    JNIEnv *env, jclass clazz) {
  // TODO: implement on_surface_changed()
}

float caca = 0.0f;

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1draw_1frame(JNIEnv *env,
                                                             jclass clazz) {
  g_device->Clear(RenderTarget{});
  // Update light position
  lights.positions[0][0] = cos(caca) * 3;
  lights.positions[0][1] = sin(caca) * 3;
  lights.positions[0][2] = sin(caca) * 3;
  g_device->UpdateUniformBuffer(uniformBuffers[1], cpuBuffer4);

  model = glm::translate(glm::vec3(cos(caca), 0.0f, 0.0f));
  mvp = projection * view * model;
  memcpy(&material.mvp[0], &mvp[0][0], 16 * sizeof(float));
  memcpy(&material.model[0], &model[0][0], 16 * sizeof(float));
  g_device->UpdateUniformBuffer(uniformBuffers[0], cpuBuffer3);

  g_device->_backend->BindProgram(program);
  g_device->_backend->Draw(drawInput, 960, 1, uniformBuffers, 2);
  caca += 0.05f;
  g_device->RequestAnimationFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_MainActivity_on_1create(JNIEnv *env, jclass clazz,
                                                     jobject asset_manager) {
  // Open your file
  g_assetManager = AAssetManager_fromJava(env, asset_manager);
}