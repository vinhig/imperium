//
// Created by vinhig on 17.01.2021.
//

#include "render/Device.h"
#include "render/backend/BackendVulkan.h"

namespace Imperium::Render::Backend {
void BackendVulkan::BeginFrame() {
  // Wait for last frame dude
  vkWaitForFences(_device, 1, &GetCurrentFrameData()._renderFence, true,
                  1000000000);
  vkResetFences(_device, 1, &GetCurrentFrameData()._renderFence);

  vkAcquireNextImageKHR(_device, _swapchain, 1000000000,
                        GetCurrentFrameData()._presentSemaphore, nullptr,
                        &_swapchainImageIndex);

  VkCommandBufferBeginInfo cmdBeginInfo = {};
  cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBeginInfo.pNext = nullptr;
  cmdBeginInfo.pInheritanceInfo = nullptr;
  cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(GetCurrentFrameData()._commandBuffer, &cmdBeginInfo);
}

void BackendVulkan::EndFrame() {
  VkCommandBuffer cmd = GetCurrentFrameData()._commandBuffer;

  vkCmdEndRenderPass(cmd);
  vkEndCommandBuffer(cmd);

  VkPipelineStageFlags waitStage =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit = {};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.pNext = nullptr;
  submit.pWaitDstStageMask = &waitStage;
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &GetCurrentFrameData()._presentSemaphore;
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &GetCurrentFrameData()._renderSemaphore;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cmd;

  vkQueueSubmit(_graphicsQueue, 1, &submit, GetCurrentFrameData()._renderFence);

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;

  presentInfo.pSwapchains = &_swapchain;
  presentInfo.swapchainCount = 1;

  presentInfo.pWaitSemaphores = &GetCurrentFrameData()._renderSemaphore;
  presentInfo.waitSemaphoreCount = 1;

  presentInfo.pImageIndices = &_swapchainImageIndex;

  vkQueuePresentKHR(_graphicsQueue, &presentInfo);

  // Delete old stuff
  DeleteOldStuff();
}
}  // namespace Imperium::Render::Backend