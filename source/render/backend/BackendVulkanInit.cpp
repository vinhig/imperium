//
// Created by vinhig on 10.02.2021.
//

#include <SDL2/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include "render/Device.h"
#include "render/Resources.h"
#include "render/backend/BackendVulkan.h"

namespace Imperium::Render::Backend {

BackendVulkan::BackendVulkan(Device* device) {
  printf("Creating Vulkan Backend.\n");
  /**
   * Create instance
   */
  vkb::InstanceBuilder instanceBuilder;
  instanceBuilder.request_validation_layers(true);
  instanceBuilder.require_api_version(1, 0, 0);
  instanceBuilder.use_default_debug_messenger();

  auto instanceBuilderReturn = instanceBuilder.build();
  if (!instanceBuilderReturn) {
    printf("Failed to create vulkan instance.\n");
    return;
  }

  auto instance = instanceBuilderReturn.value();
  _instance = instance.instance;
  _debugMessenger = instance.debug_messenger;

  /**
   * Create surface
   */
  auto window = device->Window();
  _width = window.width;
  _height = window.height;
  if (window.desktop) {
    if (!SDL_Vulkan_CreateSurface(window.sdlWindow, _instance, &_surface)) {
      printf("Failed to create vulkan surface.");
      return;
    };
  }

  /**
   * Choose physical device
   */
  vkb::PhysicalDeviceSelector physicalDeviceSelector(instance);
  auto physicalDeviceSelectorReturn =
      physicalDeviceSelector.set_surface(_surface).select();

  if (!physicalDeviceSelectorReturn) {
    printf("Failed to find a suitable gpu.\n");
    return;
  }

  auto physicalDevice = physicalDeviceSelectorReturn.value();

  _physicalDevice = physicalDevice.physical_device;
  _memoryProperties = physicalDevice.memory_properties;

  /**
   * Create logical device
   */
  vkb::DeviceBuilder deviceBuilder(physicalDevice);
  auto deviceBuilderReturn = deviceBuilder.build();

  if (!deviceBuilderReturn) {
    printf("Failed to find a suitable logical device.\n");
    return;
  }

  auto logicalDevice = deviceBuilderReturn.value();
  _device = logicalDevice.device;

  /**
   * Create graphics queue
   */
  auto graphicsQueueReturn = logicalDevice.get_queue(vkb::QueueType::graphics);

  if (!graphicsQueueReturn) {
    printf("Failed to find a suitable graphics queue.\n");
    return;
  }

  _graphicsQueue = graphicsQueueReturn.value();
  _graphicsQueueFamily =
      logicalDevice.get_queue_index(vkb::QueueType::graphics).value();

  /**
   * Create transfer queue
   */
  auto transferQueueReturn = logicalDevice.get_queue(vkb::QueueType::transfer);

  if (!transferQueueReturn) {
    printf("Failed to find a suitable graphics queue.\n");
    return;
  }

  _transferQueue = transferQueueReturn.value();
  _transferQueueFamily =
      logicalDevice.get_queue_index(vkb::QueueType::transfer).value();

  /**
   * Create swapchain
   */

  vkb::SwapchainBuilder swapchainBuilder(logicalDevice);
  auto swapchainBuilderReturn = swapchainBuilder.build();

  if (!swapchainBuilderReturn) {
    printf("Faileds to create swapchain.\n");
    return;
  }

  vkb::Swapchain swapchain = swapchainBuilderReturn.value();
  _swapchainImages = swapchain.get_images().value();
  _swapchainImageViews = swapchain.get_image_views().value();
  _swapchainImageFormat = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
  _swapchain = swapchain.swapchain;

  _failed = false;

  /**
   * vma allocator
   */
  VmaAllocatorCreateInfo allocatorCreateInfo = {};
  allocatorCreateInfo.physicalDevice = _physicalDevice;
  allocatorCreateInfo.device = _device;
  allocatorCreateInfo.instance = _instance;
  vmaCreateAllocator(&allocatorCreateInfo, &_allocator);

  /**
   * Depth buffer
   */
  _depthFormat = VK_FORMAT_D32_SFLOAT;
  VkExtent3D depthImageExtent = {(uint32_t)_width, (uint32_t)_height, 1};
  auto depthBufferInfo =
      CreateImageInfo(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                      depthImageExtent);

  VmaAllocationCreateInfo depthBufferAllocInfo = {};
  depthBufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  depthBufferAllocInfo.requiredFlags =
      VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  vmaCreateImage(_allocator, &depthBufferInfo, &depthBufferAllocInfo,
                 &_depthImage._image, &_depthImage._allocation, nullptr);

  auto depthBufferViewInfo = CreateImageViewInfo(
      _depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

  vkCreateImageView(_device, &depthBufferViewInfo, nullptr, &_depthImageView);

  /**
   * Graphics command pool
   */
  VkCommandPoolCreateInfo graphicsCommandPoolCreateInfo = {};
  graphicsCommandPoolCreateInfo.sType =
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  graphicsCommandPoolCreateInfo.pNext = nullptr;

  graphicsCommandPoolCreateInfo.queueFamilyIndex = _graphicsQueueFamily;
  graphicsCommandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  vkCreateCommandPool(_device, &graphicsCommandPoolCreateInfo, nullptr,
                      &_graphicsCommandPool);

  /**
   * Graphics command pool
   */
  VkCommandPoolCreateInfo transferCommandPoolCreateInfo = {};
  transferCommandPoolCreateInfo.sType =
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  transferCommandPoolCreateInfo.pNext = nullptr;

  transferCommandPoolCreateInfo.queueFamilyIndex = _transferQueueFamily;
  transferCommandPoolCreateInfo.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  vkCreateCommandPool(_device, &transferCommandPoolCreateInfo, nullptr,
                      &_transferCommandPool);

  /**
   * Transfer command buffer
   */
  _transferCommandBuffer = this->CreateCommandBuffer(_transferCommandPool);

  /**
   * Default renderpass
   */
  VkAttachmentDescription depthAttachment = {};
  // Depth attachment
  depthAttachment.flags = 0;
  depthAttachment.format = _depthFormat;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = _swapchainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 2;
  renderPassInfo.pAttachments = &attachments[0];
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass);

  /**
   * Framebuffers
   * Triple-Buffering -> 3 framebuffers
   */
  VkFramebufferCreateInfo fbCreateInfo = {};
  fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fbCreateInfo.pNext = nullptr;

  fbCreateInfo.renderPass = _renderPass;
  fbCreateInfo.attachmentCount = 1;
  fbCreateInfo.width = window.width;
  fbCreateInfo.height = window.height;
  fbCreateInfo.layers = 1;

  auto fbCount = _swapchainImages.size();
  _framebuffers = std::vector<VkFramebuffer>(fbCount);

  for (size_t i = 0; i < fbCount; i++) {
    VkImageView attachments[2];
    attachments[0] = _swapchainImageViews[i];
    attachments[1] = _depthImageView;

    fbCreateInfo.attachmentCount = 2;
    fbCreateInfo.pAttachments = &attachments[0];
    vkCreateFramebuffer(_device, &fbCreateInfo, nullptr, &_framebuffers[i]);
  }

  /**
   * Synchronisation stuff
   */
  VkFenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.pNext = nullptr;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VkSemaphoreCreateInfo semaphoreCreateInfo = {};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreCreateInfo.pNext = nullptr;
  semaphoreCreateInfo.flags = 0;

  for (int i = 0; i < 2; i++) {
    vkCreateFence(_device, &fenceCreateInfo, nullptr,
                  &_framesData[i]._renderFence);

    _framesData[i]._commandBuffer = CreateCommandBuffer(_graphicsCommandPool);

    vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                      &_framesData[i]._renderSemaphore);

    vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                      &_framesData[i]._presentSemaphore);
    vkResetFences(_device, 1, &_framesData[i]._renderFence);
  }

  vkCreateFence(_device, &fenceCreateInfo, nullptr, &_transferFence);
  vkResetFences(_device, 1, &_transferFence);
}

BackendVulkan::~BackendVulkan() {
  printf("~BackendVulkan()\n");
  if (!_failed) {
    // Wait for everything before deleting any object
    vkWaitForFences(_device, 1, &GetCurrentFrameData()._renderFence, true,
                    1000000000);

    vkDeviceWaitIdle(_device);
    vkResetCommandBuffer(GetCurrentFrameData()._commandBuffer,
                         VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkResetCommandBuffer(_transferCommandBuffer,
                         VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    this->DeleteOldStuff();
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);

    for (auto const& imageView : _swapchainImageViews) {
      vkDestroyImageView(_device, imageView, nullptr);
    }

    vkDestroyImageView(_device, _depthImageView, nullptr);
    vmaDestroyImage(_allocator, _depthImage._image, _depthImage._allocation);

    vkDestroyCommandPool(_device, _transferCommandPool, nullptr);
    vkDestroyCommandPool(_device, _graphicsCommandPool, nullptr);

    vkDestroyRenderPass(_device, _renderPass, nullptr);

    vmaDestroyAllocator(_allocator);

    for (auto const& fb : _framebuffers) {
      vkDestroyFramebuffer(_device, fb, nullptr);
    }

    for (auto const& stuff : _pipelines) {
      vkDestroyPipelineLayout(_device, stuff.pipelineLayout, nullptr);
      vkDestroyPipeline(_device, stuff.pipeline, nullptr);
    }

    for (auto const& frameData : _framesData) {
      vkDestroyFence(_device, frameData._renderFence, nullptr);
      vkDestroySemaphore(_device, frameData._renderSemaphore, nullptr);
      vkDestroySemaphore(_device, frameData._presentSemaphore, nullptr);
    }

    vkDestroyFence(_device, _transferFence, nullptr);

    vkDestroyDevice(_device, nullptr);

    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    vkb::destroy_debug_utils_messenger(_instance, _debugMessenger, nullptr);
    vkDestroyInstance(_instance, nullptr);
  }
}

}  // namespace Imperium::Render::Backend