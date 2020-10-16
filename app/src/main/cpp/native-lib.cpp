#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <jni.h>
#include <renderer/Descriptions.h>
#include <renderer/DeviceAndroid.h>

#include <cstdio>
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

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(
    JNIEnv *env, jclass clazz) {
  // OpenGL context created from activity
  // Create device
  g_device = new DeviceAndroid(ApiDesc::OpenGLES32, 0, 0);

  // Set file reader
  g_device->SetFileReader([](std::string path) -> std::vector<unsigned char> {
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
  });

  program = g_device->CreateProgram("basic");

  float vertices[] = {
      .5f,  .5f,  .5f,  -.5f, .5f,  .5f,
      -.5f, -.5f, .5f,  .5f,  -.5f, .5f,  // v0,v1,v2,v3 (front)
      .5f,  .5f,  .5f,  .5f,  -.5f, .5f,
      .5f,  -.5f, -.5f, .5f,  .5f,  -.5f,  // v0,v3,v4,v5 (right)
      .5f,  .5f,  .5f,  .5f,  .5f,  -.5f,
      -.5f, .5f,  -.5f, -.5f, .5f,  .5f,  // v0,v5,v6,v1 (top)
      -.5f, .5f,  .5f,  -.5f, .5f,  -.5f,
      -.5f, -.5f, -.5f, -.5f, -.5f, .5f,  // v1,v6,v7,v2 (left)
      -.5f, -.5f, -.5f, .5f,  -.5f, -.5f,
      .5f,  -.5f, .5f,  -.5f, -.5f, .5f,  // v7,v4,v3,v2 (bottom)
      .5f,  -.5f, -.5f, -.5f, -.5f, -.5f,
      -.5f, .5f,  -.5f, .5f,  .5f,  -.5f  // v4,v7,v6,v5 (back)
  };

  int indices[] = {
      0,  1,  2,  2,  3,  0,   // v0-v1-v2, v2-v3-v0 (front)
      4,  5,  6,  6,  7,  4,   // v0-v3-v4, v4-v5-v0 (right)
      8,  9,  10, 10, 11, 8,   // v0-v5-v6, v6-v1-v0 (top)
      12, 13, 14, 14, 15, 12,  // v1-v6-v7, v7-v2-v1 (left)
      16, 17, 18, 18, 19, 16,  // v7-v4-v3, v3-v2-v7 (bottom)
      20, 21, 22, 22, 23, 20   // v4-v7-v6, v6-v5-v4 (back)
  };

  struct Material {
    float colors[4];
  };

  Material material = {{0.2, 0.3, 0.4, 1.0}};

  // Create vertex buffer
  CPUBuffer<float> cpuBuffer1 = {};
  cpuBuffer1.data = &vertices[0];
  cpuBuffer1.nbElements = 72;
  cpuBuffer1.stride = 3;
  CPUBuffer<int> cpuBuffer2 = {};
  cpuBuffer2.data = &indices[0];
  cpuBuffer2.nbElements = 36;
  cpuBuffer2.stride = 3;
  CPUBuffer<void> cpuBuffer3 = {};
  cpuBuffer3.data = (void *)&material;
  cpuBuffer3.size = sizeof(Material);

  // Create data to draw
  vertexBuffer = g_device->CreateVertexBuffer(cpuBuffer1);
  indexBuffer = g_device->CreateIndexBuffer(cpuBuffer2);
  uniformBuffer = g_device->CreateUniformBuffer(cpuBuffer3);

  // Specify how to draw data
  InputLayoutDesc inputLayoutDesc = {};
  inputLayoutDesc.program = &program;
  inputLayoutDesc.entries.push_back(
      {0, 3, false, sizeof(float) * 3, DataType::Float, nullptr});
  auto inputLayout = g_device->CreateInputLayout(inputLayoutDesc);

  // Specify what to draw
  std::vector<GPUBuffer> buffers(1);
  buffers[0] = vertexBuffer;
  drawInput = g_device->CreateDrawInput(inputLayout, buffers, indexBuffer);
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1changed(
    JNIEnv *env, jclass clazz) {
  // TODO: implement on_surface_changed()
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1draw_1frame(JNIEnv *env,
                                                             jclass clazz) {
  RenderTarget dummy = {};
  g_device->Clear(dummy);

  g_device->Clear(RenderTarget{});
  g_device->_backend->BindProgram(program);
  g_device->_backend->Draw(drawInput, 36, 1, &uniformBuffer, 1);
  g_device->RequestAnimationFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_MainActivity_on_1create(JNIEnv *env, jclass clazz,
                                                     jobject asset_manager) {
  // Open your file
  g_assetManager = AAssetManager_fromJava(env, asset_manager);
}