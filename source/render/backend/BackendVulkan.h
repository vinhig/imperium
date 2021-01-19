//
// Created by vinhig on 15.01.2021.
//

#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "core/Option.h"
#include "render/backend/Backend.h"

using namespace Imperium;

namespace Imperium::Render {
class Device;
}

namespace Imperium::Render::Backend {

struct PipelineStuff {
  VkPipeline pipeline;
  VkPipelineLayout pipelineLayout;
};

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

  std::vector<PipelineStuff> _pipelines;

  VkCommandBuffer CreateCommandBuffer();
  Core::Option<VkShaderModule> CreateShaderModule(const char* filePath);
  static VkPipelineShaderStageCreateInfo CreateShaderStageInfo(
      VkShaderStageFlagBits stage, VkShaderModule shader);
  static VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo(
      /*some things are needed here*/);
  static VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo(
      VkPrimitiveTopology topology);
  static VkPipelineRasterizationStateCreateInfo CreateRasterizationStateInfo(
      VkPolygonMode polygonMode);
  static VkPipelineMultisampleStateCreateInfo CreateMultisampleStateInfo();
  static VkPipelineColorBlendAttachmentState
  CreateColorBlendAttachmentStateInfo();
  static VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo();

 public:
  BackendVulkan(Device* device);
  ~BackendVulkan() override;

  void BeginFrame() override;
  void EndFrame() override;

  void BeginPipeline(int pipeline) override;
  void EndPipeline(int pipeline) override;

  void BindRenderpass(int renderpass) override;

  Core::Option<int> CreatePipeline(PipelineType pipelineType) override;
  // void CreateBuffer(BufferType bufferType) override;
};

}  // namespace Imperium::Render::Backend