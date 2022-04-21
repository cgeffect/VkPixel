//
//  VkPixelCmdBuffer.cpp
//  VkPixel
//
//  Created by Jason on 2022/4/21.
//

#include "VkPixelCommandBuffer.h"

namespace vkpixel {

VkPixelCommandBuffer::VkPixelCommandBuffer(const VkPixelDevice::Ptr& device, const VkPixelCommandPool::Ptr& commandPool, bool asSecondary) {
    mDevice = device;
    mCommandPool = commandPool;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = mCommandPool->getCommandPool();
    allocInfo.level = asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(mDevice->getDevice(), &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error: falied to create commandBuffer");
    }
}

VkPixelCommandBuffer::~VkPixelCommandBuffer() {
    if (mCommandBuffer != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(mDevice->getDevice(), mCommandPool->getCommandPool(), 1, &mCommandBuffer);
    }
}//会随着CommandPool析构，而析构释放

void VkPixelCommandBuffer::begin(VkCommandBufferUsageFlags flag, const VkCommandBufferInheritanceInfo& inheritance) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flag;
    beginInfo.pInheritanceInfo = &inheritance;

    if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to begin commandBuffer");
    }
}

void VkPixelCommandBuffer::beginRenderPass(
    const VkRenderPassBeginInfo& renderPassBeginInfo,
    const VkSubpassContents& subPassContents
) {
    vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subPassContents);
}

void VkPixelCommandBuffer::bindGraphicPipeline(const VkPipeline& pipeline) {
    vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void VkPixelCommandBuffer::bindVertexBuffer(const std::vector<VkBuffer>& buffers) {
    std::vector<VkDeviceSize> offsets(buffers.size(), 0);

    vkCmdBindVertexBuffers(mCommandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
}

void VkPixelCommandBuffer::bindIndexBuffer(const VkBuffer& buffer) {
    vkCmdBindIndexBuffer(mCommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
}

void VkPixelCommandBuffer::bindDescriptorSet(const VkPipelineLayout layout, const VkDescriptorSet &descriptorSet) {
    vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet, 0, nullptr);
}

void VkPixelCommandBuffer::draw(size_t vertexCount) {
    vkCmdDraw(mCommandBuffer, (uint32_t)vertexCount, 1, 0, 0);
}

void VkPixelCommandBuffer::drawIndex(size_t indexCount) {
    vkCmdDrawIndexed(mCommandBuffer, (uint32_t)indexCount, 1, 0, 0, 0);
}

void VkPixelCommandBuffer::endRenderPass() {
    vkCmdEndRenderPass(mCommandBuffer);
}

void VkPixelCommandBuffer::end() {
    if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error:failed to end Command Buffer");
    }
}

void VkPixelCommandBuffer::copyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t copyInfoCount, const std::vector<VkBufferCopy>& copyInfos) {
    vkCmdCopyBuffer(mCommandBuffer, srcBuffer, dstBuffer, copyInfoCount, copyInfos.data());
}

void VkPixelCommandBuffer::copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t width, uint32_t height) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;

    //为0代表不需要进行padding(内存对齐)
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0; //只有一个mipLevel层
    region.imageSubresource.baseArrayLayer = 0;//0号layer
    region.imageSubresource.layerCount = 1; //只有一个layer
    region.imageOffset = {0, 0, 0}; //读取偏移量
    region.imageExtent = {width, height, 1}; //宽高深

    vkCmdCopyBufferToImage(mCommandBuffer, srcBuffer, dstImage, dstImageLayout, 1, &region);
}

void VkPixelCommandBuffer::submitSync(VkQueue queue, VkFence fence) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, fence);

    vkQueueWaitIdle(queue);
}

void VkPixelCommandBuffer::transferImageLayout(const VkImageMemoryBarrier &imageMemoryBarrier, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
    vkCmdPipelineBarrier(mCommandBuffer,
                         srcStageMask,
                         dstStageMask,
                         0,
                         0, nullptr,//MemoryBarrier
                         0, nullptr, //BufferMemoryBarrier
                         1, &imageMemoryBarrier);
}
}
