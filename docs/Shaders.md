# Shaders

At the time being, Imperium is featuring four different backends.

* OpenGLES 3.2 (targeting mobile)
* OpenGL 3.3 (targeting low-end desktop)
* OpenGL 4.6 (targeting high-end desktop)
* DirectX 11 (targeting high-end windows)

We try to minimize code fragmentation as much as possible. So the dummy solution consisting in reimplementing shaders in
four different languages is just impossible and completely forbidden. Eventually, the concept of a single codebase based
on a modern GLSL that transpiles to other flavor or version has been chosen thanks to spirv-cross.

* At compile time, GLSL 450 core shaders are compiled to spirv.
* At runtime time, spirv shaders are transpiled depending on the enabled backend.

NB: It's possible that the backend can directly ingest spirv binaries. Therefore, the second step is simply omitted. 