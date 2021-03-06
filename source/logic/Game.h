//
// Created by vinhi on 23/10/2020.
//

#pragma once

#include "../loader/MeshLoader.h"
#include "../loader/TextureLoader.h"
#include "../renderer/Device.h"
#include "../renderer/Frame.h"
#include "Ecs.h"
#include "TryHarder.h"

/**
 * Game spawned at the opening of our application. Has to be manually linked to
 * a device and a backend. Has to be inherited for each user application.
 */
class Game {
 private:
  GPUProgram _basic;
  Frame* _frame;
  int frame = 0;
  /**
   * Call public LogicalUpdate and dispatch registered jobs to workers.
   */
  void SysLogicalUpdate();

  /**
   * Call public ResourceUpdate.
   */
  void SysResourceUpdate();

  /**
   * Draw whole system then call public Draw.
   */
  void SysDraw();
  /**
   * Call public _Load. Initialize resources helper.
   */
  void SysLoad();

  Device* _device;
  TryHarder* _tryHarder;

  MeshLoader* _meshLoader;
  TextureLoader* _textureLoader;

  System* _system;

 public:
  explicit Game(Device* device);
  ~Game() = default;

  // Logic methods
  /**
   * Load assets from disk. Asynchronous loading with a cute "LOADING PLS WAIT"
   * image.
   */
  virtual void Load(Device* device) = 0;

  /**
   * Called each game tick. Register game update operation.
   */
  virtual void LogicalUpdate(TryHarder* tryHarder) = 0;

  /**
   * Called after each game tick. Modify GPU resources.
   * @param device Underlying device linked to this game.
   */
  virtual void ResourceUpdate(Device* device) = 0;

  /**
   * Called each frame by underlying device. Register game draw calls.
   * TODO: draw calls should be registered, not direct.
   * @param device Underlying device linked to this game.
   */
  virtual void Draw(Device* device) = 0;

  /**
   * Should the game close.
   */
  bool ShouldClose();

  /**
   * Process a game loop.
   */
  void Process();

  MeshLoader* GetMeshLoader();
  TextureLoader* GetTextureLoader();

  System* GetSystem();
};