//
// Created by vinhig on 10.02.2021.
//

#include <vk_mem_alloc.h>

#include "render/backend/BackendVulkan.h"

namespace Imperium::Render::Backend {

void BackendVulkan::DeleteOldStuff() {
  for (int i = 0; i < _nbOldBuffers; i++) {
    DeleteBuffer(_oldBuffers[i]);
  }
  delete _oldBuffers;
  _oldBuffers = nullptr;
  _nbOldBuffers = 0;

  for (int i = 0; i < _nbOldPipelines; i++) {
    DeletePipeline(_oldPipelines[i]);
  }
  delete _oldPipelines;
  _oldPipelines = nullptr;
  _nbOldPipelines = 0;
}

Buffer* BackendVulkan::CreateBuffer(BufferType bufferType, unsigned size) {
  VkBufferUsageFlags bufferUsage = 0;
  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  switch (bufferType) {
    case Vertex:
      bufferUsage =
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
      break;
    case Index:
      bufferUsage =
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
      break;
    case Uniform:
      // Uniform buffer are a bit special
      // They should be created in 'group', a group is composed by multiple
      // uniform buffers and each of them will be used in a specific frame. For
      // example, if FRAME_OVERLAP is set to 2, creating one Buffer will create
      // 2 Buffers. First one will be used in even frames and the other in odd
      // frames.
      bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
      break;
    case Staging:
      bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
      break;
  }
  VkBufferCreateInfo bufferCreateInfo = {};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = bufferUsage;

  if (bufferType != BufferType::Uniform) {
    auto buffer = new BufferVulkan;
    buffer->size = size;

    VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
    vmaAllocationCreateInfo.usage = memoryUsage;
    vmaCreateBuffer(_allocator, &bufferCreateInfo, &vmaAllocationCreateInfo,
                    &buffer->buffer, &buffer->allocation, nullptr);

    return (Buffer*)buffer;
  } else {
    auto* bufferFrameDependant = new BufferFrameDependant;
    int i = 0;
    for (auto& buffer : bufferFrameDependant->buffers) {
      buffer.size = size;
      if (i < FRAME_OVERLAP - 1) {
        buffer.next = &bufferFrameDependant->buffers[i + 1];
      }

      VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
      vmaAllocationCreateInfo.usage = memoryUsage;
      vmaCreateBuffer(_allocator, &bufferCreateInfo, &vmaAllocationCreateInfo,
                      &buffer.buffer, &buffer.allocation, nullptr);
      i++;
    }
    return (Buffer*)bufferFrameDependant;
  }
}

void BackendVulkan::DeleteBuffer(Buffer* b) {
  auto buffer = (BufferVulkan*)b;

  vmaDestroyBuffer(_allocator, buffer->buffer, buffer->allocation);

  if (buffer->next) {
    DeleteBuffer((Buffer*)buffer->next);
  }
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

}  // namespace Imperium::Render::Backend