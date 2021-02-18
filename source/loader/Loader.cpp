//
// Created by vinhig on 19.01.2021.
//

#include "Loader.h"

#include <cgltf.h>

#include <cstdio>
#include <cstring>

#include "loader/Model.h"
#include "render/Resources.h"

namespace Imperium::Loader {
Loader::Loader() = default;
Loader::~Loader() = default;

Core::Option<Model *> Loader::CreateModelTriangle() {
  auto vertices = new Render::Vertex[3];
  vertices[0] = {
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
  };
  vertices[1] = {
      -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };
  vertices[2] = {
      0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
  };

  auto indices = new unsigned int[3];
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  auto model = new Model(1);
  model->AddRawMesh({vertices, indices, 27, 3});
  return Core::Option<Model *>(model);
}

void CollectVertices(cgltf_primitive *primitive, Render::Vertex *vertices,
                     int vertexCount) {
  for (int i = 0; i < primitive->attributes_count; i++) {
    auto attribute = primitive->attributes[i];

    switch (attribute.type) {
      case cgltf_attribute_type_position: {
        auto positions = new float[vertexCount * 3];
        cgltf_accessor_unpack_floats(attribute.data, positions,
                                     vertexCount * 3);
        for (int l = 0; l < vertexCount; l++) {
          vertices[l].position[0] = positions[l * 3 + 0];
          vertices[l].position[1] = positions[l * 3 + 1];
          vertices[l].position[2] = positions[l * 3 + 2];
        }
        break;
      }
      case cgltf_attribute_type_normal: {
        auto normals = new float[vertexCount * 3];
        cgltf_accessor_unpack_floats(attribute.data, normals, vertexCount * 3);
        for (int l = 0; l < vertexCount; l++) {
          vertices[l].normal[0] = normals[l * 3 + 0];
          vertices[l].normal[1] = normals[l * 3 + 1];
          vertices[l].normal[2] = normals[l * 3 + 2];
        }
        break;
      }
      case cgltf_attribute_type_texcoord: {
        auto texcoords = new float[vertexCount * 2];
        cgltf_accessor_unpack_floats(attribute.data, texcoords,
                                     vertexCount * 2);
        for (int l = 0; l < vertexCount; l++) {
          vertices[l].texCoord[0] = texcoords[l * 2 + 0];
          vertices[l].texCoord[1] = texcoords[l * 2 + 1];
        }
        break;
      }
      default:
        break;
    }
  }
}

Core::Option<Model *> Loader::CreateModelGltf(const char *path) {
  cgltf_options options = {};
  cgltf_data *data;
  cgltf_result result = cgltf_parse_file(&options, path, &data);

  if (result != cgltf_result_success) {
    printf("Something went wrong during file parsing.\n");
    return Core::Option<Model *>();
  }
  result = cgltf_load_buffers(&options, data, path);
  if (result != cgltf_result_success) {
    printf("Something went wrong during file loading.\n");
    return Core::Option<Model *>();
  }
  result = cgltf_validate(data);
  if (result != cgltf_result_success) {
    printf("Something went wrong after validation.\n");
    return Core::Option<Model *>();
  }

  // cgltf model
  // \--- mesh
  //   \--- primitive -> material
  // imperium bundle
  // \--- mesh -> material
  // The concept of parent mesh disappears and each primitive is uploaded as a
  // mesh.
  auto model = new Model(1);
  for (int i = 0; i < data->meshes_count; i++) {
    auto mesh = data->meshes[i];
    for (int j = 0; j < mesh.primitives_count; j++) {
      auto primitive = mesh.primitives[j];

      auto vertexCount = (unsigned int)primitive.attributes[0].data->count;
      auto vertices = new Render::Vertex[vertexCount];
      CollectVertices(&primitive, vertices, vertexCount);

      auto indexCount = (unsigned int)primitive.indices->count;
      auto indices = new unsigned int[indexCount];
      for (int k = 0; k < indexCount; ++k) {
        cgltf_accessor_read_uint(primitive.indices, k, &indices[k],
                                 sizeof(unsigned int));
      }

      model->AddRawMesh({vertices, indices, vertexCount, indexCount});
    }
  }

  return Core::Option<Model *>(model);
}
}  // namespace Imperium::Loader