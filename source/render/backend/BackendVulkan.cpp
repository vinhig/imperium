#include "render/backend/BackendVulkan.h"

#include <SDL2/SDL_vulkan.h>
#include <VkBootstrap.h>

#include "render/Device.h"

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
  auto queueReturn = logicalDevice.get_queue(vkb::QueueType::graphics);

  if (!queueReturn) {
    printf("Failed to find a suitable graphics queue.\n");
    return;
  }

  _graphicsQueue = queueReturn.value();
  _graphicsQueueFamily =
      logicalDevice.get_queue_index(vkb::QueueType::graphics).value();

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
  _swapchainImageFormat = swapchain.image_format;
  _swapchain = swapchain.swapchain;

  _failed = false;

  /**
   * Command pool
   */
  VkCommandPoolCreateInfo commandPoolCreateInfo = {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.pNext = nullptr;

  commandPoolCreateInfo.queueFamilyIndex = _graphicsQueueFamily;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  vkCreateCommandPool(_device, &commandPoolCreateInfo, nullptr, &_commandPool);

  /**
   * Main command buffer
   */
  _mainCommandBuffer = this->CreateCommandBuffer();

  /**
   * Default renderpass
   */
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

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
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
    fbCreateInfo.pAttachments = &_swapchainImageViews[i];
    vkCreateFramebuffer(_device, &fbCreateInfo, nullptr, &_framebuffers[i]);
  }

  /**
   * Synchronisation stuff
   */
  VkFenceCreateInfo fenceCreateInfo = {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.pNext = nullptr;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence);

  VkSemaphoreCreateInfo semaphoreCreateInfo = {};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreCreateInfo.pNext = nullptr;
  semaphoreCreateInfo.flags = 0;

  vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore);
  vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore);
}

VkCommandBuffer BackendVulkan::CreateCommandBuffer() {
  VkCommandBufferAllocateInfo cmdAllocateInfo = {};
  cmdAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdAllocateInfo.pNext = nullptr;
  cmdAllocateInfo.commandPool = _commandPool;
  cmdAllocateInfo.commandBufferCount = 1;
  cmdAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(_device, &cmdAllocateInfo, &commandBuffer);

  return commandBuffer;
}

void BackendVulkan::BeginFrame() {
  // Wait for last frame dude
  vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);
  vkResetFences(_device, 1, &_renderFence);

  vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore,
                        nullptr, &_swapchainImageIndex);
}

void BackendVulkan::EndFrame() {
  VkCommandBuffer cmd = _mainCommandBuffer;
  VkCommandBufferBeginInfo cmdBeginInfo = {};
  cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBeginInfo.pNext = nullptr;
  cmdBeginInfo.pInheritanceInfo = nullptr;
  cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(cmd, &cmdBeginInfo);

  VkClearValue clearValue;
  clearValue.color = {{0.3f, 0.2f, 0.1f, 1.0f}};

  VkRenderPassBeginInfo rpInfo = {};
  rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rpInfo.pNext = nullptr;
  rpInfo.renderPass = _renderPass;
  rpInfo.renderArea.offset.x = 0;
  rpInfo.renderArea.offset.y = 0;
  rpInfo.renderArea.extent = {_width, _height};
  rpInfo.framebuffer = _framebuffers[_swapchainImageIndex];
  rpInfo.clearValueCount = 1;
  rpInfo.pClearValues = &clearValue;

  vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdEndRenderPass(cmd);
  vkEndCommandBuffer(cmd);

  VkPipelineStageFlags waitStage =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit = {};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.pNext = nullptr;
  submit.pWaitDstStageMask = &waitStage;
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &_presentSemaphore;
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &_renderSemaphore;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cmd;

  vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence);

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;

  presentInfo.pSwapchains = &_swapchain;
  presentInfo.swapchainCount = 1;

  presentInfo.pWaitSemaphores = &_renderSemaphore;
  presentInfo.waitSemaphoreCount = 1;

  presentInfo.pImageIndices = &_swapchainImageIndex;

  vkQueuePresentKHR(_graphicsQueue, &presentInfo);
}

BackendVulkan::~BackendVulkan() {
  printf("~BackendVulkan()\n");
  if (!_failed) {
    // Wait for everything before deleting any object
    vkDeviceWaitIdle(_device);
    vkResetCommandBuffer(_mainCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);

    for (auto const& imageView : _swapchainImageViews) {
      vkDestroyImageView(_device, imageView, nullptr);
    }

    vkDestroyCommandPool(_device, _commandPool, nullptr);

    vkDestroyRenderPass(_device, _renderPass, nullptr);

    for (auto const& fb : _framebuffers) {
      vkDestroyFramebuffer(_device, fb, nullptr);
    }

    vkDestroyFence(_device, _renderFence, nullptr);
    vkDestroySemaphore(_device, _renderSemaphore, nullptr);
    vkDestroySemaphore(_device, _presentSemaphore, nullptr);

    vkDestroyDevice(_device, nullptr);

    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    vkb::destroy_debug_utils_messenger(_instance, _debugMessenger, nullptr);
    vkDestroyInstance(_instance, nullptr);
  }
}
}  // namespace Imperium::Render::Backend