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

DeviceAndroid *_device = nullptr;
AAssetManager *assetManager = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(
    JNIEnv *env, jclass clazz) {
  // OpenGL context created from activity
  // Create device
  _device = new DeviceAndroid(ApiDesc::OpenGLES32);

  // Set file reader

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
  _device->Clear(dummy);
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_MainActivity_on_1create(JNIEnv *env, jclass clazz,
                                                     jobject asset_manager) {
  // Open your file
  auto mgr = AAssetManager_fromJava(env, asset_manager);
  AAsset *file =
      AAssetManager_open(mgr, "mobile.frag.glsl", AASSET_MODE_BUFFER);

  if (!file) {
    throw std::runtime_error("Unable to open file.");
  }

  /*auto dir = AAssetManager_openDir(mgr, "/");

  auto fileName = AAssetDir_getNextFileName(dir);
  while (fileName) {
      // __android_log_print(ANDROID_LOG_INFO, "native-lib", "A FILE: %s",
  fileName); fileName = AAssetDir_getNextFileName(dir);
  }*/

  // Get the file length
  size_t fileLength = AAsset_getLength(file);

  // Allocate memory to read your file
  char *fileContent = new char[fileLength + 1];

  // Read your file
  AAsset_read(file, fileContent, fileLength);
  // For safety you can add a 0 terminating character at the end of your file
  // ...
  fileContent[fileLength] = '\0';

  __android_log_print(ANDROID_LOG_WARN, "native-lib", "%s", fileContent);

  delete[] fileContent;
}