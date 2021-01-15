//
// Created by vinhi on 15/01/2020.
//

#pragma once

namespace Imperium::Render::Backend {

class Backend {
 public:
  virtual ~Backend(){};

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;
};

}  // namespace Imperium::Render::Backend