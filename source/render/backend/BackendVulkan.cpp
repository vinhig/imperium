//
// Created by vinhig on 15.01.2021.
//

#include "render/backend/BackendVulkan.h"

#include <fstream>

#include "render/Device.h"

namespace Imperium::Render::Backend {

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

void BackendVulkan::BeginPipeline(Pipeline* pipeline) {
  vkCmdBindPipeline(GetCurrentFrameData()._commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    ((PipelineVulkan*)pipeline)->_pipeline);
  // vkCmdDraw(_mainCommandBuffer, 3, 1, 0, 0);
}

void BackendVulkan::EndPipeline(Pipeline* pipeline) {}

void BackendVulkan::BindRenderpass(int renderpass) {
  if (renderpass == 0) {
    VkCommandBuffer cmd = GetCurrentFrameData()._commandBuffer;

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

Core::Option<Pipeline*> BackendVulkan::CreatePipeline(
    PipelineType pipelineType, unsigned descriptorCount,
    DescriptorLayout* descriptors) {
  if (pipelineType == PipelineType::Graphics) {
    /**
     * Create shader stages
     */
    auto vertexShader = CreateShaderModule("assets/triangle.vert.glsl.spv");
    auto fragmentShader = CreateShaderModule("assets/triangle.frag.glsl.spv");

    // Creation of shader module may fail
    if (!vertexShader.HasValue()) {
      printf("Vertex shader failed to be created.\n");
      return Core::Option<Pipeline*>();
    }
    if (!fragmentShader.HasValue()) {
      printf("Fragment shader failed to be created.\n");
      return Core::Option<Pipeline*>();
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
    unsigned cameraPushConstant = 16 * 4;
    VkShaderStageFlags cameraStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    auto layoutInfo =
        CreatePipelineLayoutInfo(1, &cameraPushConstant,
                                 descriptorCount, descriptors, &cameraStageFlags);
    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &pipelineLayout);

    VkViewport viewport = {};
    viewport.height = (float)_height;
    viewport.width = (float)_width;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

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
      return Core::Option<Pipeline*>();
    }
    vkDestroyShaderModule(_device, vertexShader.Value(), nullptr);
    vkDestroyShaderModule(_device, fragmentShader.Value(), nullptr);

    auto p = new PipelineVulkan;
    p->_pipeline = pipeline;
    p->_pipelineLayout = pipelineLayout;

    // _pipelines.push_back({pipeline, pipelineLayout});
    return Core::Option<Pipeline*>((Pipeline*)p);
  } else {
    return Core::Option<Pipeline*>();
  }
}

void BackendVulkan::DeletePipeline(Pipeline* pipeline) {
  vkDestroyPipelineLayout(_device, ((PipelineVulkan*)pipeline)->_pipelineLayout,
                          nullptr);
  vkDestroyPipeline(_device, ((PipelineVulkan*)pipeline)->_pipeline, nullptr);
}

void BackendVulkan::DeferDeletePipeline(Pipeline* p) {
  _nbOldPipelines++;
  auto pipelines = (Pipeline**)malloc(sizeof(Buffer*) * _nbOldPipelines);
  for (int i = 0; i < _nbOldPipelines - 1; i++) {
    pipelines[i] = _oldPipelines[i];
  }
  pipelines[_nbOldPipelines - 1] = p;
  delete _oldPipelines;
  _oldPipelines = pipelines;
}

void BackendVulkan::BindVertexBuffers(int count, Buffer* vertexBuffers) {
  auto buffers = (BufferVulkan*)vertexBuffers;
  auto vbuffers = new VkBuffer[count];
  for (int i = 0; i < count; i++) {
    vbuffers[i] = buffers[i].buffer;
  }
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(GetCurrentFrameData()._commandBuffer, 0, count,
                         vbuffers, &offset);
}

void BackendVulkan::BindIndexBuffer(Buffer* indexBuffer) {
  auto buffer = (BufferVulkan*)indexBuffer;
  vkCmdBindIndexBuffer(GetCurrentFrameData()._commandBuffer, buffer->buffer, 0,
                       VkIndexType::VK_INDEX_TYPE_UINT32);
}

/*void BackendVulkan::BindConstantBuffer(int offset, Buffer* constantBuffer) {
  auto buffer = ((BufferFrameDependant*)constantBuffer)
                    ->buffers[_currentFrame % FRAME_OVERLAP];
}*/

void BackendVulkan::BindUniform(int offset, Pipeline* pipeline, size_t size,
                                void* data) {
  vkCmdPushConstants(GetCurrentFrameData()._commandBuffer,
                     ((PipelineVulkan*)pipeline)->_pipelineLayout,
                     VK_SHADER_STAGE_VERTEX_BIT, 0, size, data);
}

void BackendVulkan::DrawElements(int count) {
  // vkCmdDraw(_mainCommandBuffer, count, 1, 0, 0);
  vkCmdDrawIndexed(GetCurrentFrameData()._commandBuffer, count, 1, 0, 0, 0);
}

FrameData& BackendVulkan::GetCurrentFrameData() {
  return _framesData[(_currentFrame) % FRAME_OVERLAP];
}

}  // namespace Imperium::Render::Backend