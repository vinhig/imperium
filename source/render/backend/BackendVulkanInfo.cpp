//
// Created by vinhig on 10.02.2021.
//

#include "render/Resources.h"
#include "render/backend/BackendVulkan.h"

namespace Imperium::Render::Backend {

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
}  // namespace Imperium::Render::Backend