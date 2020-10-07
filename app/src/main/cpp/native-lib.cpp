#include <jni.h>
#include <string>

#include <GLES3/gl3.h>

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(JNIEnv *env, jclass clazz) {
    glClearColor(1.0f, 0.2f, 1.0f, 1.0f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1changed(JNIEnv *env, jclass clazz) {
    // TODO: implement on_surface_changed()
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1draw_1frame(JNIEnv *env, jclass clazz) {
    glClear(GL_COLOR_BUFFER_BIT);
}