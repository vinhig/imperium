//
// Created by vincent on 8/10/20.
//

#pragma once

#include "Backend.h"
#include "Descriptions.h"
#include "Device.h"

class DeviceAndroid : public Device {
 private:
  Backend* _backend;

 public:
  explicit DeviceAndroid(ApiDesc apiDesc);
  ~DeviceAndroid() = default;

  void RequestAnimationFrame() override;
  bool ShouldClose() override;
  void Clear() override;
};
