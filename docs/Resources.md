# Resources

Instead of characterizing everything as `data` or `big array of numbers`, it's good to clarify a bit what we call:

* Buffer
* Resource
* Data

in the Imperium ~~game engine~~.

## Resource

A resource is a usable part of memory. As they're two types of memory, they are two types of resources.
The first type is simply the resource located on your RAM. The nomenclature for that type follow the `CPUXxx` format.
A non-exhaustive list of CPUResources:

* CPUTexture (returned by TextureLoader)
* CPUBuffer (manually created or returned by MeshLoader)

The second type of resources is linked to the GPU used by the game. There are two way of creating them. The most obvious
manner is by uploading a CPUResource. And the other way is to create an empty GPUResource. A non-exhaustive list of
GPUResources:

* GPUBuffer (returned after creating a Vertex, Uniform or Index buffer)
* GPUTexture (returned after uploading a CPUTexture)
