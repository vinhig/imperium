//
// Created by vinhi on 15/01/2020.
//

#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "render/backend/Backend.h"

namespace Imperium::Render {
class Device;
}

namespace Imperium::Render::Backend {

class BackendVulkan : public Backend {
 private:
  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debugMessenger;
  VkSurfaceKHR _surface;
  VkPhysicalDevice _physicalDevice;
  VkPhysicalDeviceMemoryProperties _memoryProperties;
  VkDevice _device;

  int _width{0};
  int _height{0};

  // Commands
  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;

  // Synchronisation
  VkFence _renderFence;
  VkSemaphore _renderSemaphore;
  VkSemaphore _presentSemaphore;

  // Swapchain
  VkSwapchainKHR _swapchain;
  std::vector<VkImage> _swapchainImages;
  std::vector<VkImageView> _swapchainImageViews;
  VkFormat _swapchainImageFormat;
  uint32_t _swapchainImageIndex;
  bool _failed{true};

  // Renderpass
  VkRenderPass _renderPass;
  std::vector<VkFramebuffer> _framebuffers;

 public:
  BackendVulkan(Device* device);
  ~BackendVulkan() override;

  VkCommandBuffer CreateCommandBuffer();

  void BeginFrame() override;
  void EndFrame() override;
};

}  // namespace Imperium::Render::Backend