#include "Game.h"

#include <cstdio>

#include "loader/Loader.h"
#include "loader/Model.h"
#include "render/Context.h"
#include "render/Device.h"
#include "render/Resources.h"
#include "render/frontend/Mesh.h"
#include "render/frontend/Model.h"

namespace Imperium::Logic {

Game::Game(Render::Device* device) {
  _loader = new Loader::Loader();
  _device = device;
}

Game::~Game() {}

void Game::Process() {
  _device->PollEvents();

  this->SysDraw();
}

void Game::SysDraw() {
  _device->GetContext()->BeginFrame();
  this->Draw(_device);
  _device->GetContext()->EndFrame();
}

bool Game::ShouldClose() { return _device->ShouldClose(); }

Core::Option<Render::Frontend::Mesh*> Game::LoadTriangle() {
  // Load from disk
  // Loader loads :
  // - textures
  // - meshes
  // - skeleton
  // - animation
  auto model = _loader->CreateModelTriangle().Value();

  // Use context to create frontend objects
  // Texture(s), Mesh(es)
  auto meshes = model->GetMeshes();
  auto nbMeshes = model->NbMeshes();
  for (int i = 0; i < nbMeshes; i++) {
    Render::CPUBuffer<Render::Vertex> vertices = {};
    vertices.nbElements = meshes[i].nbVertices;
    vertices.data = meshes[i].vertices;

    Render::CPUBuffer<unsigned int> indices = {};
    indices.nbElements = meshes[i].nbIndices;
    indices.data = meshes[i].indices;

    auto mesh = _device->GetContext()->CreateMesh(vertices, indices);

    // TODO: atm it's just a triangle

    return Core::Option<Render::Frontend::Mesh*>(mesh);
  }
}

Core::Option<Render::Frontend::Model*> Game::LoadModel(const char* path) {
  auto model = _loader->CreateModelGltf(path);

  if (!model.HasValue()) {
    printf("Loader didn't manage to load model.");
    return Core::Option<Render::Frontend::Model*>();
  }

  auto meshes = model.Value()->GetMeshes();
  auto nbMeshes = model.Value()->NbMeshes();

  auto buildModel = new Render::Frontend::Model(nbMeshes);

  for (int i = 0; i < nbMeshes; i++) {
    Render::CPUBuffer<Render::Vertex> vertices = {};
    vertices.nbElements = meshes[i].nbVertices;
    vertices.data = meshes[i].vertices;

    Render::CPUBuffer<unsigned int> indices = {};
    indices.nbElements = meshes[i].nbIndices;
    indices.data = meshes[i].indices;

    auto mesh = _device->GetContext()->CreateMesh(vertices, indices);

    buildModel->AddMesh(mesh, nullptr);
  }

  return Core::Option<Render::Frontend::Model*>(buildModel);
}

}  // namespace Imperium::Logic