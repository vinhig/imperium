//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <iostream>

#include "../renderer/Device.h"
#include "Actions.h"

class System;

class Entity {
 private:
  System* _system = nullptr;
  Entity* _parent = nullptr;

 public:
  Entity(System* system) { _system = system; };
  ~Entity() = default;

  System* System() { return _system; };

  Entity* GetParent() { return _parent; };
  void SetParent(Entity* parent) { _parent = parent; };

  template <typename T>
  T* GetOrCreate();

  template <typename T>
  T* Get();
};

class IComponent {
 private:
  Entity* _owner;

 public:
  explicit IComponent(Entity* owner) { _owner = owner; }
  ~IComponent() = default;

  virtual int UUID() = 0;
};

class IComponentDrawable : public IComponent {
 public:
  IComponentDrawable(Entity* owner, Device* device) : IComponent(owner){};
  virtual DrawCall Draw() = 0;
};

class IComponentLogic : public IComponent {
 public:
  explicit IComponentLogic(Entity* owner) : IComponent(owner){};
  ~IComponentLogic() = default;
  virtual LogicCall Logic() = 0;
};

class IComponentResource : public IComponent {
 private:
  CPUBuffer<void> _cpuBuffer;
  GPUBuffer _gpuBuffer;

  bool _update = false;

 public:
  IComponentResource(Entity* owner, Device* device) : IComponent(owner){};
  ~IComponentResource() = default;

  /**
   * Compute a resource action detailed by a source and a destination buffer.
   * @return ResourceCall to execute.
   */
  ResourceCall Resource() {
    if (_update) {
      std::cout << "Pushing some data" << std::endl;
      _update = false;
      return ResourceCall{_cpuBuffer, _gpuBuffer, true};
    } else {
      // Nothing has changed so nothing to push
      ResourceCall call = {};
      call.enable = false;

      return call;
    }
  };

  /**
   * Return underlying cpu buffer. Doesn't not check if changes were applied.
   * Use ForceUpdate to force a resource update.
   * @return Reference to CPU buffer.
   */
  CPUBuffer<void>* GetCPUBuffer() { return &_cpuBuffer; };

  void SetGPUBuffer(GPUBuffer gpuBuffer) { _gpuBuffer = gpuBuffer; };

  /**
   * Force push of data to the GPU on next update.
   */
  void ForceUpdate() { _update = true; }
};