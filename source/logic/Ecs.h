//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <cassert>
#include <iostream>
#include <unordered_map>

#include "../renderer/Device.h"
#include "Actions.h"
#include "TryHarder.h"

class System;
class Game;  // c++ bad;

class Entity {
 private:
  System* _system = nullptr;
  Entity* _parent = nullptr;
  int _id;

 public:
  Entity(System* system, int id) {
    _system = system;
    _id = id;
  };
  ~Entity() = default;

  System* GetSystem() { return _system; };
  int GetId() { return _id; };

  /**
   * Get parent entity.
   * @return Reference to parent entity.
   */
  Entity* GetParent() { return _parent; };

  /**
   * Set parent entity. Can be a null parent if this entity is a root.
   * @param parent  Reference to parent entity.
   */
  void SetParent(Entity* parent) { _parent = parent; };

  template <typename T>
  T* GetOrCreate(void* args) {
    auto comp = _system->Get<T>(this);
    if (comp == nullptr) {
      _system->Create<T>(args);
    }
    comp = _system->Get<T>(this);
    assert(comp != nullptr);
    return comp;
  };

  template <typename T>
  T* Get() {
    return _system->Get<T>(this);
  };
};

class IComponent {
 private:
  Entity* _owner;

 public:
  explicit IComponent(Entity* owner) { _owner = owner; }
  ~IComponent() = default;

  Entity* GetEntity() { return _owner; };

  virtual int UUID() = 0;
};

class IComponentDrawable {
 public:
  explicit IComponentDrawable(){};
  ~IComponentDrawable() = default;
  virtual DrawCall Draw() = 0;
};

class IComponentLogic {
 public:
  explicit IComponentLogic(){};
  ~IComponentLogic() = default;
  virtual LogicCall Logic() = 0;
};

class IComponentResource {
 private:
  CPUBuffer<void> _cpuBuffer;
  GPUBuffer _gpuBuffer;

  bool _update = false;

 public:
  IComponentResource(){};
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

class System {
 private:
  std::unordered_map<int, std::vector<IComponent*>> _components;

  Device* _device;

 public:
  System(Device* device) { _device = device; };
  ~System() = default;

  Device* GetDevice() { return _device; }

  /**
   * Ask components to load themselves.
   */
  void Load(Device* device){
      // Fake lol
      // Each time you create a component, it's loaded by its constructor
  };

  /**
   * Ask components to update logically themselves.
   */
  void LogicalUpdate(TryHarder* tryHarder){

  };

  /**
   * Ask components to update their underlying resources.
   */
  void ResourceUpdate(Device* device){

  };

  /**
   * Ask components to draw themselves.
   */
  void Draw(Device* device){

  };

  /**
   * Search for a component with a T type owned by a specific entity.
   * @tparam T Type of component to retrieve.
   * @param entity Owner of the component to retrieve.
   * @return T* reference to found component. nullptr if no components
   * correspond.
   */
  template <typename T>
  T* Get(Entity* entity) {
    // Get all components owned by this entity
    auto comp = _components[entity->GetId()];
    if (comp != _components.end() || comp.size() != 0) {
      for (int i = 0; i < comp.size(); i++) {
        if (comp[i]->UUID() == T::Uuid) {
          return comp[i];
        }
      }
    }
    // Sad, we don't found any component
    return nullptr
  };

  /**
   * Register a created component owned by a specific entity.
   * @tparam T Type of component to register.
   * @param entity
   * @param component
   */
  template <typename T>
  void Create(Entity* entity, void* args) {
    assert(entity != nullptr && component != nullptr);
    auto comp = _components[entity->GetId()];
    if (comp == _components.end()) {
      // If this entity isn't registered
      _components[entity->GetId()];
    }
    _components[entity->GetId()].push_back(new T(args));
  }
};
