//
// Created by vinhi on 24/10/2020.
//

#pragma once

#include <unordered_map>

#include "../common/Assert.h"
#include "../common/Log.h"
#include "../renderer/Device.h"
#include "Actions.h"
#include "TryHarder.h"
#include "btBulletDynamicsCommon.h"

class Game;  // c++ bad;
class IComponent;
class Entity;

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
 private:
  Layer _layer;

 public:
  explicit IComponentDrawable(){};
  ~IComponentDrawable() = default;

  Layer GetLayer() { return _layer; };
  void SetLayer(Layer layer) { _layer = layer; };

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

  GPUBuffer GetGPUBuffer() { return _gpuBuffer; };
  void SetGPUBuffer(GPUBuffer gpuBuffer) { _gpuBuffer = gpuBuffer; };

  /**
   * Force push of data to the GPU on next update.
   */
  void ForceUpdate() { _update = true; }
};

class System {
 private:
  // Components for entity
  // int: id of the entity
  // std::vector<IComponent*>: components owned by this entity
  std::unordered_map<int, std::vector<IComponent*>> _componentsForEntity;

  // Components for type
  // int: uuid of the component
  // std::vector<IComponent*>: components of this type
  std::unordered_map<int, std::vector<IComponent*>> _componentsForType;

  // Bullet components
  btDefaultCollisionConfiguration* _collisionConfiguration{};
  btCollisionDispatcher* _dispatcher{};
  btBroadphaseInterface* _overlappingPairCache{};
  btSequentialImpulseConstraintSolver* _solver{};
  btDiscreteDynamicsWorld* _dynamicsWorld{};
  btAlignedObjectArray<btCollisionShape*> _collisionShapes;

  Device* _device{};
  Game* _game{};

 public:
  explicit System(Game* game, Device* device) {
    _game = game;
    _device = device;

    // Initialize physics components with default configuration
    _collisionConfiguration = new btDefaultCollisionConfiguration();
    _dispatcher = new btCollisionDispatcher(_collisionConfiguration);
    _overlappingPairCache = new btDbvtBroadphase();
    _solver = new btSequentialImpulseConstraintSolver;
    _dynamicsWorld = new btDiscreteDynamicsWorld(
        _dispatcher, _overlappingPairCache, _solver, _collisionConfiguration);

    _dynamicsWorld->setGravity(btVector3(0, -10, 0));
  };

  ~System() = default;

  Device* GetDevice() { return _device; };

  Game* GetGame() { return _game; };

  std::vector<IComponent*> Components(int uuid) {
    return _componentsForType[uuid];
  };

  /**
   * Search for a component with a T type owned by a specific entity.
   * @tparam T Type of component to retrieve.
   * @param entity Owner of the component to retrieve.
   * @return T* reference to found component. nullptr if no components
   * correspond.
   */
  template <typename T>
  T* Get(Entity* entity);

  template <typename T>
  T* GetFirstActive();

  float test = 0.0f;

  /**
   * Register a created component owned by a specific entity.
   * @tparam T Type of component to register.
   * @param entity
   * @param component
   */
  template <typename T>
  void Create(Entity* entity, void* args);

  void AddShape(btCollisionShape* shape) { _collisionShapes.push_back(shape); };
  void AddBody(btRigidBody* body) { _dynamicsWorld->addRigidBody(body); };

  void UpdatePhysics() {
    // TODO: should have a more precise timestep
    test += 1.0f / 60.0f;
    _dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
    _dynamicsWorld->stepSimulation((1.0f / 60.0f), 10);
  }
};

class Entity {
 private:
  System* _system = nullptr;
  Entity* _parent = nullptr;
  int _id;

  bool _enabled = true;

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

  bool IsEnabled() { return _enabled; }

  template <typename T>
  T* GetOrCreate(void* args) {
    auto comp = _system->Get<T>(this);
    if (comp == nullptr) {
      _system->Create<T>(this, args);
    }
    comp = _system->Get<T>(this);

    std::string err = "Component '" + std::string(typeid(T).name()) + "' must no fail its creation.";
    ASSERT_NOT_EQUAL(comp, nullptr, err.c_str());
    return comp;
  };

  template <typename T>
  T* Get() {
    return _system->Get<T>(this);
  };
};

#ifndef ECS_IMPLEMENTATION
#define ECS_IMPLEMENTATION

template <typename T>
T* System::Get(Entity* entity) {
  // Get all components owned by this entity
  std::unordered_map<int, std::vector<IComponent*>>::iterator comp =
      _componentsForEntity.find(entity->GetId());
  if (comp != _componentsForEntity.end()) {
    if (comp->second.size() != 0) {
      for (int i = 0; i < comp->second.size(); i++) {
        if (comp->second[i]->UUID() == T::Uuid) {
          return (T*)comp->second[i];
        }
      }
    }
  }
  // Sad, we don't found any component
  return nullptr;
}

template <typename T>
T* System::GetFirstActive() {
  std::unordered_map<int, std::vector<IComponent*>>::iterator components =
      _componentsForType.find(T::Uuid);
  if (components != _componentsForType.end()) {
    if (components->second.size() != 0) {
      for (int i = 0; i < components->second.size(); i++) {
        if (components->second[i]->GetEntity()->IsEnabled()) {
          return (T*)components->second[i];
        }
      }
    }
  }
  return nullptr;
}

template <typename T>
void System::Create(Entity* entity, void* args) {
  std::unordered_map<int, std::vector<IComponent*>>::iterator comp =
      _componentsForEntity.find(entity->GetId());
  if (comp == _componentsForEntity.end()) {
    // If this entity isn't registered
    _componentsForEntity[entity->GetId()];
  }
  auto newComp = new T(entity, args);
  _componentsForEntity[entity->GetId()].push_back(newComp);
  _componentsForType[newComp->UUID()].push_back(newComp);
}

#endif  // ECS_IMPLEMENTATION