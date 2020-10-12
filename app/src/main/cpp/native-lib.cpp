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

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_RendererWrapper_on_1surface_1created(
        JNIEnv *env, jclass clazz) {
    // OpenGL context created from activity
    // Create device
    g_device = new DeviceAndroid(ApiDesc::OpenGLES32);

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

    auto test = g_device->CreateProgram("basic");
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
}

extern "C" JNIEXPORT void JNICALL
Java_com_warnengine_imperium_MainActivity_on_1create(JNIEnv *env, jclass clazz,
                                                     jobject asset_manager) {
    // Open your file
    g_assetManager = AAssetManager_fromJava(env, asset_manager);
}