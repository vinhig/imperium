//
// Created by vinhi on 23/10/2020.
//

#pragma once

#include "../renderer/Device.h"
#include "TryHarder.h"

/**
 * Game spawned at the opening of our application. Has to be manually linked to
 * a device and a backend. Has to be inherited for each user application.
 */
class Game {
 private:
  /**
   * Call public LogicalUpdate and dispatch registered jobs to workers.
   */
  void SysLogicalUpdate();

  /**
   * Call public ResourceUpdate.
   */
  void SysResourceUpdate();

  /**
   * Call public _Load. Initialize resources helper.
   */
  void SysLoad();

  Device* _device;
  TryHarder* _tryHarder;



 public:
  explicit Game(Device* device);
  Game() = default;
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
   * Launch the game.
   */
  void Run();
};