//
// Created by vinhig on 15.01.2021.
//

#include "render/backend/BackendVulkan.h"

#include <SDL2/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include <fstream>

#include "render/Device.h"
#include "render/Resources.h"

namespace Imperium::Render::Backend {
struct BufferVulkan {
  VkBuffer buffer{nullptr};
  VmaAllocation allocation{nullptr};
  size_t size;
};

struct VertexInputDescription {
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;

  VkPipelineVertexInputStateCreateFlags flags = 0;
};

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
   * Main command buffer
   */
  _mainCommandBuffer = this->CreateCommandBuffer(_graphicsCommandPool);

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
  vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence);
  vkCreateFence(_device, &fenceCreateInfo, nullptr, &_transferFence);
  vkResetFences(_device, 1, &_transferFence);

  VkSemaphoreCreateInfo semaphoreCreateInfo = {};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreCreateInfo.pNext = nullptr;
  semaphoreCreateInfo.flags = 0;

  vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore);
  vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore);
}

VkCommandBuffer BackendVulkan::CreateCommandBuffer(VkCommandPool commandPool) {
  VkCommandBufferAllocateInfo cmdAllocateInfo = {};
  cmdAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdAllocateInfo.pNext = nullptr;
  cmdAllocateInfo.commandPool = commandPool;
  cmdAllocateInfo.commandBufferCount = 1;
  cmdAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(_device, &cmdAllocateInfo, &commandBuffer);

  return commandBuffer;
}

Core::Option<VkShaderModule> BackendVulkan::CreateShaderModule(
    const char* filePath) {
  // Read file and convert it to u32 array
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    printf(
        "Hello there! Imperium wasn't able to find your shader "
        "'%s'\n",
        filePath);
    return Core::Option<VkShaderModule>();
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
  file.seekg(0);
  file.read((char*)buffer.data(), fileSize);
  file.close();

  // Reee Vulkan stuff
  VkShaderModuleCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.pNext = nullptr;
  info.codeSize = buffer.size() * sizeof(uint32_t);
  info.pCode = buffer.data();

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(_device, &info, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    printf(
        "Hello there! Imperium wasn't able to read and compile your shader "
        "'%s'",
        filePath);
    return Core::Option<VkShaderModule>();
  }

  return Core::Option<VkShaderModule>(shaderModule);
}

VkPipelineShaderStageCreateInfo BackendVulkan::CreateShaderStageInfo(
    VkShaderStageFlagBits stage, VkShaderModule shaderModule) {
  VkPipelineShaderStageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.stage = stage;
  info.module = shaderModule;
  info.pName = "main";
  return info;
}

VkPipelineVertexInputStateCreateInfo BackendVulkan::CreateVertexInputStateInfo(
    /*some things are needed here*/) {
  VkPipelineVertexInputStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  info.pNext = nullptr;
  info.vertexBindingDescriptionCount = 0;
  info.vertexAttributeDescriptionCount = 0;
  return info;
}

VkPipelineInputAssemblyStateCreateInfo
BackendVulkan::CreateInputAssemblyStateInfo(VkPrimitiveTopology topology) {
  VkPipelineInputAssemblyStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  info.pNext = nullptr;
  info.topology = topology;
  info.primitiveRestartEnable = VK_FALSE;
  return info;
}

VkPipelineRasterizationStateCreateInfo
BackendVulkan::CreateRasterizationStateInfo(VkPolygonMode polygonMode) {
  VkPipelineRasterizationStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  info.pNext = nullptr;
  info.depthClampEnable = VK_FALSE;
  info.rasterizerDiscardEnable = VK_FALSE;
  info.polygonMode = polygonMode;
  info.lineWidth = 1.0f;
  info.cullMode = VK_CULL_MODE_NONE;
  info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  info.depthBiasEnable = VK_FALSE;
  info.depthBiasConstantFactor = 0.0f;
  info.depthBiasClamp = 0.0f;
  info.depthBiasSlopeFactor = 0.0f;

  return info;
}

VkPipelineMultisampleStateCreateInfo
BackendVulkan::CreateMultisampleStateInfo() {
  VkPipelineMultisampleStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  info.pNext = nullptr;
  info.sampleShadingEnable = VK_FALSE;
  info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  info.minSampleShading = 1.0f;
  info.pSampleMask = nullptr;
  info.alphaToCoverageEnable = VK_FALSE;
  info.alphaToOneEnable = VK_FALSE;
  return info;
}

VkPipelineColorBlendAttachmentState
BackendVulkan::CreateColorBlendAttachmentStateInfo() {
  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  return colorBlendAttachment;
}

VkPipelineLayoutCreateInfo BackendVulkan::CreatePipelineLayoutInfo(
    size_t nbPushConstant, size_t* pushConstantSizes,
    VkShaderStageFlags* shaderStageFlags) {
  VkPushConstantRange* ranges = new VkPushConstantRange[nbPushConstant];
  for (int i = 0; i < nbPushConstant; i++) {
    ranges[i] = {};
    ranges[i].offset = i;
    ranges[i].size = pushConstantSizes[i];
    ranges[i].stageFlags = shaderStageFlags[i];
  }

  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.setLayoutCount = 0;
  info.pSetLayouts = nullptr;
  info.pushConstantRangeCount = nbPushConstant;
  info.pPushConstantRanges = ranges;
  return info;
}

VkImageCreateInfo BackendVulkan::CreateImageInfo(VkFormat format,
                                                 VkImageUsageFlags usageFlags,
                                                 VkExtent3D extent) {
  VkImageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.pNext = nullptr;

  info.imageType = VK_IMAGE_TYPE_2D;

  info.format = format;
  info.extent = extent;

  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.usage = usageFlags;

  return info;
}

VkImageViewCreateInfo BackendVulkan::CreateImageViewInfo(
    VkFormat format, VkImage image, VkImageAspectFlags aspectFlags) {
  VkImageViewCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;

  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.image = image;
  info.format = format;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;
  info.subresourceRange.aspectMask = aspectFlags;

  return info;
}

VkPipelineDepthStencilStateCreateInfo BackendVulkan::CreateDepthStencilInfo(
    bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp) {
  VkPipelineDepthStencilStateCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  info.pNext = nullptr;

  info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
  info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
  info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
  info.depthBoundsTestEnable = VK_FALSE;
  info.minDepthBounds = 0.0f;
  info.maxDepthBounds = 1.0f;
  info.stencilTestEnable = VK_FALSE;

  return info;
}

void BackendVulkan::BeginFrame() {
  // Wait for last frame dude
  vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);
  vkResetFences(_device, 1, &_renderFence);

  vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore,
                        nullptr, &_swapchainImageIndex);

  VkCommandBufferBeginInfo cmdBeginInfo = {};
  cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBeginInfo.pNext = nullptr;
  cmdBeginInfo.pInheritanceInfo = nullptr;
  cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(_mainCommandBuffer, &cmdBeginInfo);
}

void BackendVulkan::EndFrame() {
  VkCommandBuffer cmd = _mainCommandBuffer;

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

  // Delete old stuff
}

void BackendVulkan::DeleteOldStuff() {
  for (int i = 0; i < _nbOldBuffers; i++) {
    DeleteBuffer(_oldBuffers[i]);
  }
  delete _oldBuffers;
  _oldBuffers = nullptr;
  _nbOldBuffers = 0;
}

void BackendVulkan::BeginPipeline(int pipeline) {
  vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    _pipelines[pipeline].pipeline);
  // vkCmdDraw(_mainCommandBuffer, 3, 1, 0, 0);
}

void BackendVulkan::EndPipeline(int pipeline) {}

void BackendVulkan::BindRenderpass(int renderpass) {
  if (renderpass == 0) {
    VkCommandBuffer cmd = _mainCommandBuffer;

    VkClearValue clearValue;
    clearValue.color = {{0.3f, 0.2f, 0.1f, 1.0f}};

    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;

    VkClearValue clearValues[2] = {clearValue, depthClear};

    VkRenderPassBeginInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.pNext = nullptr;
    rpInfo.renderPass = _renderPass;
    rpInfo.renderArea.offset.x = 0;
    rpInfo.renderArea.offset.y = 0;
    rpInfo.renderArea.extent = {(uint32_t)_width, (uint32_t)_height};
    rpInfo.framebuffer = _framebuffers[_swapchainImageIndex];
    rpInfo.clearValueCount = 2;
    rpInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
  }
}

Core::Option<int> BackendVulkan::CreatePipeline(PipelineType pipelineType) {
  if (pipelineType == PipelineType::Graphics) {
    /**
     * Create shader stages
     */
    auto vertexShader = CreateShaderModule("assets/triangle.vert.glsl.spv");
    auto fragmentShader = CreateShaderModule("assets/triangle.frag.glsl.spv");

    // Creation of shader module may fail
    if (!vertexShader.HasValue()) {
      printf("Vertex shader failed to be created.\n");
      return Core::Option<int>();
    }
    if (!fragmentShader.HasValue()) {
      printf("Fragment shader failed to be created.\n");
      return Core::Option<int>();
    }

    auto vertexShaderStage =
        CreateShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader.Value());
    auto fragmentShaderStage = CreateShaderStageInfo(
        VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader.Value());

    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0] = vertexShaderStage;
    shaderStages[1] = fragmentShaderStage;

    /**
     * Create pipeline layout
     */
    size_t cameraPushConstant = 16 * 4;
    VkShaderStageFlags cameraStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    auto layoutInfo =
        CreatePipelineLayoutInfo(1, &cameraPushConstant, &cameraStageFlags);
    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &pipelineLayout);

    VkViewport viewport = {};
    viewport.height = _height;
    viewport.width = _width;
    viewport.x = 0;
    viewport.y = 0;
    viewport.minDepth = 0.0;
    viewport.maxDepth = 1.0;

    VkRect2D scissor = {};
    scissor.extent = {(uint32_t)_width, (uint32_t)_height};
    scissor.offset = {0, 0};

    VkPipelineViewportStateCreateInfo viewportStateInfo = {};
    viewportStateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateInfo.pNext = nullptr;
    viewportStateInfo.viewportCount = 1;
    viewportStateInfo.pViewports = &viewport;
    viewportStateInfo.scissorCount = 1;
    viewportStateInfo.pScissors = &scissor;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    auto attachment = CreateColorBlendAttachmentStateInfo();
    colorBlending.pAttachments = &attachment;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    auto depthStencilState =
        CreateDepthStencilInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    auto vertexInput = CreateVertexInputStateInfo();
    auto vertexDesc = Get3DVertexDescription();
    vertexInput.pVertexAttributeDescriptions = vertexDesc.attributes.data();
    vertexInput.vertexAttributeDescriptionCount = vertexDesc.attributes.size();
    vertexInput.pVertexBindingDescriptions = vertexDesc.bindings.data();
    vertexInput.vertexBindingDescriptionCount = vertexDesc.bindings.size();
    pipelineInfo.pVertexInputState = &vertexInput;
    auto inputAssembly =
        CreateInputAssemblyStateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportStateInfo;
    auto rasterizationState =
        CreateRasterizationStateInfo(VK_POLYGON_MODE_FILL);
    pipelineInfo.pRasterizationState = &rasterizationState;
    auto multisampleState = CreateMultisampleStateInfo();
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  nullptr, &pipeline) != VK_SUCCESS) {
      printf("Saaad.");
      return Core::Option<int>();
    }
    vkDestroyShaderModule(_device, vertexShader.Value(), nullptr);
    vkDestroyShaderModule(_device, fragmentShader.Value(), nullptr);

    _pipelines.push_back({pipeline, pipelineLayout});
    return Core::Option<int>(_pipelines.size() - 1);
  } else {
    return Core::Option<int>();
  }
}

Buffer* BackendVulkan::CreateBuffer(BufferType bufferType, int size) {
  VkBufferUsageFlags bufferUsage = 0;
  switch (bufferType) {
    case Vertex:
      bufferUsage =
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      break;
    case Index:
      bufferUsage =
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      break;
    case Uniform:
      bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      break;
    case Staging:
      bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      break;
  }
  VkBufferCreateInfo bufferCreateInfo = {};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = bufferUsage;

  auto buffer = new BufferVulkan;
  buffer->size = size;

  VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
  vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  vmaCreateBuffer(_allocator, &bufferCreateInfo, &vmaAllocationCreateInfo,
                  &buffer->buffer, &buffer->allocation, nullptr);

  return (Buffer*)buffer;
}

void BackendVulkan::DeleteBuffer(Buffer* b) {
  auto buffer = (BufferVulkan*)b;

  vmaDestroyBuffer(_allocator, buffer->buffer, buffer->allocation);
}

void BackendVulkan::DeferDeleteBuffer(Buffer* b) {
  _nbOldBuffers++;
  auto buffers = (Buffer**)malloc(sizeof(Buffer*) * _nbOldBuffers);
  for (int i = 0; i < _nbOldBuffers - 1; i++) {
    buffers[i] = _oldBuffers[i];
  }
  buffers[_nbOldBuffers - 1] = b;
  delete _oldBuffers;
  _oldBuffers = buffers;
}

void* BackendVulkan::MapBuffer(Buffer* b) {
  auto buffer = (BufferVulkan*)b;

  void* data;
  vmaMapMemory(_allocator, buffer->allocation, &data);
  return data;
}

void BackendVulkan::UnmapBuffer(Buffer* b) {
  auto buffer = (BufferVulkan*)b;

  vmaUnmapMemory(_allocator, buffer->allocation);
}

void BackendVulkan::CopyBuffer(Buffer* src, Buffer* dest) {
  auto bufferSrc = (BufferVulkan*)src;
  auto bufferDest = (BufferVulkan*)dest;
  // printf("BackendVulkan::CopyBuffer(): Copying buffer inoperant.\n");

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.pNext = nullptr;
  beginInfo.pInheritanceInfo = nullptr;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VkBufferCopy bufferCopy = {};
  bufferCopy.srcOffset = 0;
  bufferCopy.dstOffset = 0;
  bufferCopy.size = bufferSrc->size;

  vkBeginCommandBuffer(_transferCommandBuffer, &beginInfo);

  vkCmdCopyBuffer(_transferCommandBuffer, bufferSrc->buffer, bufferDest->buffer,
                  1, &bufferCopy);

  vkEndCommandBuffer(_transferCommandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.pNext = nullptr;
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &_transferCommandBuffer;

  vkQueueSubmit(_transferQueue, 1, &submitInfo, _transferFence);

  vkWaitForFences(_device, 1, &_transferFence, true, 1000000000);
  vkResetFences(_device, 1, &_transferFence);

  vkResetCommandBuffer(_transferCommandBuffer,
                       VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

VertexInputDescription BackendVulkan::Get3DVertexDescription() {
  VertexInputDescription description = {};

  VkVertexInputBindingDescription mainBinding = {};
  mainBinding.binding = 0;
  mainBinding.stride = sizeof(Render::Vertex);
  mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  description.bindings.push_back(mainBinding);

  // POSITION (location = 0)
  VkVertexInputAttributeDescription positionAttribute = {};
  positionAttribute.binding = 0;
  positionAttribute.location = 0;
  positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
  positionAttribute.offset = offsetof(Render::Vertex, position);

  // NORMAL (location = 1)
  VkVertexInputAttributeDescription normalAttribute = {};
  normalAttribute.binding = 0;
  normalAttribute.location = 1;
  normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
  normalAttribute.offset = offsetof(Render::Vertex, normal);

  // COLOR (location = 1)
  VkVertexInputAttributeDescription colorAttribute = {};
  colorAttribute.binding = 0;
  colorAttribute.location = 2;
  colorAttribute.format = VK_FORMAT_R32G32_SFLOAT;
  colorAttribute.offset = offsetof(Render::Vertex, texCoord);

  description.attributes.push_back(positionAttribute);
  description.attributes.push_back(normalAttribute);
  description.attributes.push_back(colorAttribute);

  return description;
}

void BackendVulkan::BindVertexBuffers(int count, Buffer* vertexBuffers) {
  auto buffers = (BufferVulkan*)vertexBuffers;
  auto vbuffers = new VkBuffer[count];
  for (int i = 0; i < count; i++) {
    vbuffers[i] = buffers[i].buffer;
  }
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(_mainCommandBuffer, 0, count, vbuffers, &offset);
}

void BackendVulkan::BindIndexBuffer(Buffer* indexBuffer) {
  auto buffer = (BufferVulkan*)indexBuffer;
  vkCmdBindIndexBuffer(_mainCommandBuffer, buffer->buffer, 0,
                       VkIndexType::VK_INDEX_TYPE_UINT32);
}

void BackendVulkan::BindUniform(int offset, int pipeline, size_t size,
                                void* data) {
  vkCmdPushConstants(_mainCommandBuffer, _pipelines[pipeline].pipelineLayout,
                     VK_SHADER_STAGE_VERTEX_BIT, 0, size, data);
}

void BackendVulkan::DrawElements(int count) {
  // vkCmdDraw(_mainCommandBuffer, count, 1, 0, 0);
  vkCmdDrawIndexed(_mainCommandBuffer, count, 1, 0, 0, 0);
}

BackendVulkan::~BackendVulkan() {
  printf("~BackendVulkan()\n");
  if (!_failed) {
    // Wait for everything before deleting any object
    vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);

    vkDeviceWaitIdle(_device);
    vkResetCommandBuffer(_mainCommandBuffer,
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

    vkDestroyFence(_device, _renderFence, nullptr);
    vkDestroyFence(_device, _transferFence, nullptr);
    vkDestroySemaphore(_device, _renderSemaphore, nullptr);
    vkDestroySemaphore(_device, _presentSemaphore, nullptr);

    vkDestroyDevice(_device, nullptr);

    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    vkb::destroy_debug_utils_messenger(_instance, _debugMessenger, nullptr);
    vkDestroyInstance(_instance, nullptr);
  }
}
}  // namespace Imperium::Render::Backend