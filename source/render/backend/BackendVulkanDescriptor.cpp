//
// Created by vinhig on 10.02.2021.
//

#include "render/Device.h"
#include "render/backend/BackendVulkan.h"

namespace Imperium::Render::Backend {
DescriptorLayout* BackendVulkan::CreateDescriptorLayout(
    int bindingCount, DescriptorBinding* bindings) {
  auto setLayoutBindings = new VkDescriptorSetLayoutBinding[bindingCount];

  for (int i = 0; i < bindingCount; ++i) {
    setLayoutBindings[i].binding = i;
    setLayoutBindings[i].descriptorCount = 1;

    switch (bindings[i].bindingType) {
      case BufferBinding:
        setLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        break;
        /*case TextureBinding:
          setLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
          break;*/
    }

    switch (bindings[i].stageType) {
      case VertexStage:
        setLayoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        break;
      case FragmentStage:
        setLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    }
  }

  VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo = {};
  setLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  setLayoutCreateInfo.pNext = nullptr;
  setLayoutCreateInfo.bindingCount = bindingCount;
  setLayoutCreateInfo.pBindings = setLayoutBindings;
  setLayoutCreateInfo.flags = 0;

  auto descriptorVulkan = new DescriptorLayoutVulkan;

  vkCreateDescriptorSetLayout(_device, &setLayoutCreateInfo, nullptr,
                              &descriptorVulkan->_setLayout);

  return (DescriptorLayout*)descriptorVulkan;
}

DescriptorSet* BackendVulkan::CreateDescriptorSet(
    DescriptorLayout* descriptorLayout, int bindingCount,
    DescriptorBinding* bindings) {
  // A descriptor set is frame dependant and so should be created multiple
  // times. All resources bound through it should be frame dependant too.
  auto descriptorSet = new DescriptorSetVulkan;
  // auto descriptorSet = new VkDescriptorSet[FRAME_OVERLAP];
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    VkDescriptorSetAllocateInfo info = {};
    info.pNext = nullptr;
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = _descriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = (VkDescriptorSetLayout*)descriptorLayout;

    vkAllocateDescriptorSets(_device, &info, &descriptorSet->_set[i]);

    // Bound resources to descriptor set
    // Atm, we just support uniform buffer, sad isn't it?
    for (int j = 0; j < bindingCount; j++) {
      switch (bindings[j].bindingType) {
        case BufferBinding: {
          VkDescriptorBufferInfo bufferInfo = {};
          // Here, we assume Buffer* references a BufferFrameDependant
          bufferInfo.buffer =
              ((BufferFrameDependant*)bindings[j].buffer)->buffers[i].buffer;
          bufferInfo.offset = j;
          bufferInfo.range = bindings[j].size;

          VkWriteDescriptorSet writeDescriptorSet = {};
          writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
          writeDescriptorSet.pNext = nullptr;
          writeDescriptorSet.dstBinding = 0;
          writeDescriptorSet.dstSet = descriptorSet->_set[i];
          writeDescriptorSet.descriptorCount = 1;
          writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
          writeDescriptorSet.pBufferInfo = &bufferInfo;

          vkUpdateDescriptorSets(_device, 1, &writeDescriptorSet, 0, nullptr);

          break;
        }
      }
    }
  }

  return (DescriptorSet*)descriptorSet;
}

void BackendVulkan::BindDescriptorSet(Pipeline* pipeline,
                                      DescriptorSet* descriptor) {
  vkCmdBindDescriptorSets(
      GetCurrentFrameData()._commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      ((PipelineVulkan*)pipeline)->_pipelineLayout, 0, 1,
      &((DescriptorSetVulkan*)descriptor)->_set[_currentFrame % FRAME_OVERLAP],
      0, nullptr);
}
}  // namespace Imperium::Render::Backend