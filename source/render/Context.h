//
// Created by vinhi on 14.01.2020.
//

namespace Imperium::Render::Backend {
class Backend;
}

namespace Imperium::Render {

class Device;

enum API { Vulkan, OpenGL33, OpenGL46 };

class Context {
 private:
  API _api{API::Vulkan};
  int _width{0};
  int _height{0};
  bool _failed{true};

  int _mainPipeline;

  Backend::Backend* _backend;

 public:
  Context(API api, int width, int height);

  ~Context();


  /**
   * API in use for this context.
   */
  API Api() const { return _api; };

  /**
   * Width of this context
   */
  int Width() const { return _width; };

  /**
   * Height of this context
   */
  int Height() const { return _height; };

  bool Failed() const { return _failed; };

  /**
   * Init underlying backend.
   */
  void Init(Device* device);

  void BeginFrame();

  void EndFrame();
};

}  // namespace Imperium::Render