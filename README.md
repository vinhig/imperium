# Imperium

## Introduction

At the time being, this is just a skeleton of a multiplatform rendering engine.

|               | Linux | Windows | Android |
|---------------|-------|---------|---------|
| OpenGL ES 3.2 |   Y   |    Y    |    Y    |
| OpenGL 3.3    |   Y   |    Y    |   N/A   |
| OpenGL 4.6    |   Y   |    Y    |   N/A   |
| DirectX 11    |  N/A  |    Y    |   N/A   |

(This repo is a Work-In-Progress and the board doesn't correspond to current implementation, but rather a goal to achieve.)

## Building

Before building on any platforms, make sure to fetch git submodules

```
git clone https://github.com/vinhig/Imperium
cd Imperium
git submodule init
git submodule update
```

### Android

Make sure you've installed NDK and Android SDK.

`./gradlew installDebug`

### Linux

GLFW3 is a mandatory dependence.

```shell script
mkdir build
cd build
cmake ..
./Imperium
```

### Windows

vcpkg is used to retrieve dependencies.

(DirectX SDK doesn't have a vpckg package. You'll have to install it manually.)

```
vcpkg install glfw3
# clone repo and fetch submodules
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\path\to\your\vcpkg\scripts\buildsystems\vcpkg.cmake"
MSBuild.exe Imperium.sln /property:Configuration=Release /property:Platform=x64
./Debug/Imperium.exe
```

## Architecture

Everything starts with a `device`. This `device` creates an API context and its corresponding window. Then, the user
can connect the desired backend that will be used through high-level device commands.

Example: We are on a Windows/Ogl build and we decide to create a RenderTarget object. So, to do this:

* Call to `Device::CreateRenderTarget(something)`
* That calls to `BackendOgl::CreateTexture(something)` multiple times
* Then calls `BackendOgl::CreateFramebuffer` and links previously created textures
* Then returns an `RenderTarget` object

This kind of abstraction is called `double level of abstraction` (MAYBE someone has already written a document or given
it a more selling name).
 