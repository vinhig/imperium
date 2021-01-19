# Imperium

## Introduction

Minimal game engine that builds itself and all its dependencies in less than 15sec. Featuring blazing fast assets loading with custom exporter/importer.

## Building

### Linux

### Windows

```
./vcpkg.exe install sdl2[core,vulkan] --triplet x64-windows
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\path\to\your\vcpkg\scripts\buildsystems\vcpkg.cmake"
MSBuild.exe Imperium.sln /property:Configuration=Release /property:Platform=x64
./Release/Imperium.exe
```