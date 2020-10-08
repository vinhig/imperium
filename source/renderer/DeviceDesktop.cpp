//
// Created by vincent on 8/10/20.
//

#include "DeviceDesktop.h"

#include <stdexcept>

#include "BackendOgl.h"
#include "BackendOglEs.h"
#include "Descriptions.h"

DeviceDesktop::DeviceDesktop(DeviceDesc deviceDesc) {
  if (!glfwInit()) {
    throw std::runtime_error("Unable to init glfw");
  }

  switch (deviceDesc.api) {
    case OpenGL33: {
      // Request corresponding context
      // from GLFW
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      // Create window
      _window = glfwCreateWindow(deviceDesc.width, deviceDesc.height,
                                 "Imperium", nullptr, nullptr);
      glfwMakeContextCurrent(_window);
      // Load function pointer
      // from GLAD
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Unable to load OpenGL 3.3 functions.");
      }
      // Init backend
      _backend = new BackendOgl();
      glClearColor(0.5f, 0.2f, 0.1f, 1.0f); // perfectly illegal
      break;
    }
    case OpenGL46: {
      throw std::runtime_error("Backend not implemented.");
    }
    case OpenGLES32: {
      // Request corresponding context
      // from GLFW
      glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      // Create window
      _window = glfwCreateWindow(deviceDesc.width, deviceDesc.height,
                                 "Imperium", nullptr, nullptr);
      glfwMakeContextCurrent(_window);
      // Load function pointer
      // from GLAD
      if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Unable to load OpenGLES 3.2 functions.");
      }
      // Init backend
      _backend = new BackendOglEs();
      glClearColor(0.4f, 0.6f, 0.9f, 1.0f); // perfectly illegal
      break;
    }
#ifdef _WIN32
    case Directx11: {
      throw std::runtime_error("Backend not implemented.");
    }
#endif
    default: {
      throw std::runtime_error(
          "Backend not implemented or not compatible with this platform.");
    }
  }
}
bool DeviceDesktop::ShouldClose() { return glfwWindowShouldClose(_window); }

void DeviceDesktop::RequestAnimationFrame() {
  glfwPollEvents();
  glfwSwapBuffers(_window);
}

void DeviceDesktop::Clear() { _backend->Clear(0); }

DeviceDesktop::~DeviceDesktop() = default;
