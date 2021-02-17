//
// Created by vinhig on 10.02.2021.
//

#include "render/backend/BackendVulkan.h"

namespace Imperium::Render::Backend {

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

}  // namespace Imperium::Render::Backend