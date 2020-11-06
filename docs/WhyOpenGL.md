# Why OpenGL?

A list of weird OpenGL characteristics discovered while developing this engine :

* On OpenGL 33, it's okay to `glBlitFramebuffer` two framebuffers with textures that have different internal formats. But in OpenGL ES 3.2 it's not.
    * Make sure to always have a precision of `TexturePrecision::Low` for your final rendering pass in your render graph configuration file.
* Not very weird, but kinda annoying when your forget it: make sure to always call `glDrawBuffers` with your attachments after creating a framebuffer. Or else only the default attachment will be used (only one color buffer in that case).
    * Nothing to do on the client side; The engine take care of that. 