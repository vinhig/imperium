#include <jni.h>
#include <string>

#include <GLES3/gl3.h>


#include <renderer/Descriptions.h>
#include <renderer/DeviceAndroid.h>

DeviceAndroid *_device = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(JNIEnv *env, jclass clazz) {
    _device = new DeviceAndroid(ApiDesc::OpenGLES32);
    // OpenGL context created
    // Create device
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1changed(JNIEnv *env, jclass clazz) {
    // TODO: implement on_surface_changed()
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1draw_1frame(JNIEnv *env, jclass clazz) {
    _device->Clear();
}