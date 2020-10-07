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

### Android

Make sure you've installed NDK and Android SDK.

`./gradlew installDebug`

### Linux

GLFW3 is a mandatory dependence.

```shell script
mkdir build
cd build
cmake ..
./Attempt
```