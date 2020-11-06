//
// Created by vinhi on 06/11/2020.
//

#pragma once

#include "CCamera.h"
#include "Ecs.h"

class CDirectionalLight : public CCamera {
 public:
  explicit CDirectionalLight(Entity* owner, void* args)
      : CCamera(owner, nullptr) {}

  int UUID() override { return 6; };
  static const int Uuid = 6;
};

template CDirectionalLight* Entity::GetOrCreate<CDirectionalLight>(void*);
template CDirectionalLight* System::GetFirstActive<CDirectionalLight>();