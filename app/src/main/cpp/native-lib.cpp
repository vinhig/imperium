#include <GLES3/gl3.h>
#include <MyGame.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <common/File.h>
#include <jni.h>
#include <loader/MeshLoader.h>
#include <loader/TextureLoader.h>
#include <logic/Game.h>
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

Game *g_game;

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(
    JNIEnv *env, jclass clazz) {
  // Get screen size
  int sizes[4];
  glGetIntegerv(GL_VIEWPORT, &sizes[0]);

  // OpenGL context created from activity
  // Create device
  g_device = new DeviceAndroid(ApiDesc::OpenGLES32, sizes[2], sizes[3]);

  File::FileReader = [](std::string path) -> std::vector<unsigned char> {
    // Open file
    AAsset *file =
        AAssetManager_open(g_assetManager, path.c_str(), AASSET_MODE_BUFFER);
    if (!file) {
      throw std::runtime_error("Unable to open file " + path +".");
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

  g_device->SetFileReader(File::FileReader);

  g_game = new MyGame(g_device);
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1changed(
    JNIEnv *env, jclass clazz) {
  // TODO: implement on_surface_changed()
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1draw_1frame(JNIEnv *env,
                                                             jclass clazz) {
  g_game->Process();
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_MainActivity_on_1create(JNIEnv *env, jclass clazz,
                                                     jobject asset_manager) {
  // Open your file
  g_assetManager = AAssetManager_fromJava(env, asset_manager);
}