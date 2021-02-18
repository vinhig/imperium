//
// Created by vinhig on 15.01.2021.
//

#pragma once

#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocator)
VK_DEFINE_HANDLE(VmaAllocation)

#include <vector>

#include "core/Option.h"
#include "render/backend/Backend.h"

constexpr unsigned int FRAME_OVERLAP = 3;

using namespace Imperium;

namespace Imperium::Render {
class Device;
}

namespace Imperium::Render::Backend {

struct PipelineStuff {
  VkPipeline pipeline{nullptr};
  VkPipelineLayout pipelineLayout{nullptr};
};

struct ImageStuff {
  VkImage _image{nullptr};
  VmaAllocation _allocation{nullptr};
};

struct PipelineVulkan {
  VkPipeline _pipeline{nullptr};
  VkPipelineLayout _pipelineLayout{nullptr};
};

struct BufferVulkan {
  VkBuffer buffer{nullptr};
  VmaAllocation allocation{nullptr};
  size_t size{0};
  // Used with frame dependant buffers
  BufferVulkan* next{nullptr};
};

struct BufferFrameDependant {
  BufferVulkan buffers[FRAME_OVERLAP];
};

struct FrameData {
  VkCommandBuffer _commandBuffer{nullptr};

  VkFence _renderFence{nullptr};
  VkSemaphore _renderSemaphore{nullptr};
  VkSemaphore _presentSemaphore{nullptr};
};

struct DescriptorLayoutVulkan {
  VkDescriptorSetLayout _setLayout{nullptr};
};

struct DescriptorSetVulkan {
  VkDescriptorSet _set[FRAME_OVERLAP];
};

struct VertexInputDescription {
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;

  VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct VertexInputDescription;

class BackendVulkan : public Backend {
 private:
  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debugMessenger;
  VkSurfaceKHR _surface;
  VkPhysicalDevice _physicalDevice;
  VkPhysicalDeviceMemoryProperties _memoryProperties;
  VkDevice _device;
  VmaAllocator _allocator;
  VkDescriptorPool _descriptorPool;

  int _width{0};
  int _height{0};
  unsigned int _currentFrame{0};

  // Commands
  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;
  VkQueue _transferQueue;
  uint32_t _transferQueueFamily;
  VkCommandPool _graphicsCommandPool;
  VkCommandPool _transferCommandPool;
  VkCommandBuffer _transferCommandBuffer;

  // Synchronisation
  VkFence _transferFence;
  FrameData _framesData[FRAME_OVERLAP];

  // Swapchain
  VkSwapchainKHR _swapchain;
  std::vector<VkImage> _swapchainImages;
  std::vector<VkImageView> _swapchainImageViews;
  VkFormat _swapchainImageFormat;
  uint32_t _swapchainImageIndex;
  bool _failed{true};

  // Depth buffer
  VkImageView _depthImageView;
  ImageStuff _depthImage;
  VkFormat _depthFormat;

  // Renderpass
  VkRenderPass _renderPass;
  std::vector<VkFramebuffer> _framebuffers;

  // std::vector<PipelineStuff> _pipelines;

  // Stuff to delete at end of frame
  Buffer** _oldBuffers{nullptr};
  int _nbOldBuffers{0};
  Pipeline** _oldPipelines{nullptr};
  int _nbOldPipelines{0};

  VkCommandBuffer CreateCommandBuffer(VkCommandPool commandPool);
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
  static VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo(
      unsigned pushConstantCount, const unsigned* pushConstantSizes,
      unsigned descriptorCount, DescriptorLayout* descriptors,
      const VkShaderStageFlags* shaderStageFlags);
  static VkImageCreateInfo CreateImageInfo(VkFormat format,
                                           VkImageUsageFlags usageFlags,
                                           VkExtent3D extent);
  static VkImageViewCreateInfo CreateImageViewInfo(
      VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
  static VkPipelineDepthStencilStateCreateInfo CreateDepthStencilInfo(
      bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

  VertexInputDescription Get3DVertexDescription();
  void DeleteOldStuff();

  FrameData& GetCurrentFrameData();

 public:
  explicit BackendVulkan(Device* device);
  ~BackendVulkan() override;

  void BeginFrame() override;
  void EndFrame() override;

  void BeginPipeline(Pipeline* pipeline) override;
  void EndPipeline(Pipeline* pipeline) override;

  void BindRenderpass(int renderpass) override;

  void BindVertexBuffers(int count, Buffer* vertexBuffers) override;
  void BindIndexBuffer(Buffer* indexBuffer) override;
  // void BindConstantBuffer(int offset, Buffer* constantBuffer) override;
  void BindUniform(int offset, Pipeline* pipeline, size_t size,
                   void* data) override;

  void DrawElements(int count) override;

  Core::Option<Pipeline*> CreatePipeline(
      PipelineType pipeline, unsigned descriptorCount,
      DescriptorLayout* descriptors) override;
  void DeletePipeline(Pipeline* pipeline) override;
  void DeferDeletePipeline(Pipeline* buffer) override;
  // void CreateBuffer(BufferType bufferType) override;

  Buffer* CreateBuffer(BufferType bufferType, unsigned size) override;
  void DeleteBuffer(Buffer* buffer) override;
  void DeferDeleteBuffer(Buffer* buffer) override;
  void* MapBuffer(Buffer* buffer) override;
  void UnmapBuffer(Buffer* buffer) override;

  DescriptorLayout* CreateDescriptorLayout(
      int bindingCount, DescriptorBinding* bindings) override;
  DescriptorSet* CreateDescriptorSet(DescriptorLayout* descriptorLayout,
                                     int bindingCount,
                                     DescriptorBinding* bindings) override;
  void BindDescriptorSet(Pipeline* pipeline,
                         DescriptorSet* descriptor) override;

  /**
   * Reset transfer command buffer. Register a new copy buffer command,
   * instantly run it, but don't wait for it.
   * @param src Buffer to copy from.
   * @param dest Buffer to copy to.
   */
  void CopyBuffer(Buffer* src, Buffer* dest) override;
};

}  // namespace Imperium::Render::Backend