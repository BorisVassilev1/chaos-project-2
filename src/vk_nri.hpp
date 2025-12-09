#pragma once
#include "nri.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>
#include <optional>
#include <vulkan/vulkan_raii.hpp>

#include <iostream>

class VulkanNRIAllocation : public NRIAllocation {
	vk::raii::DeviceMemory memory;
	vk::Device			   device;

   public:
	VulkanNRIAllocation(vk::raii::DeviceMemory &&mem, vk::Device dev) : memory(std::move(mem)), device(dev) {}

	vk::DeviceMemory getMemory() { return memory; }
	vk::Device		 getDevice() { return device; }
};

class VulkanNRIBuffer : public NRIBuffer {
	vk::raii::Buffer buffer;
	vk::Device		 device;

   public:
	VulkanNRIBuffer(vk::raii::Buffer &&buf, vk::Device dev) : buffer(std::move(buf)), device(dev) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;
};

class VulkanNRIImage2D : public NRIImage2D {
	vk::raii::Image image;
	vk::Device		device;
	vk::ImageLayout layout;

	uint32_t width;
	uint32_t height;

	void transitionLayout(NRICommandBuffer &commandBuffer, vk::ImageLayout newLayout, vk::AccessFlagBits srcAccess,
						  vk::AccessFlagBits dstAccess, vk::PipelineStageFlags srcStage,
						  vk::PipelineStageFlags dstStage);

   public:
	VulkanNRIImage2D(vk::raii::Image &&img, vk::ImageLayout layout, vk::Device dev, uint32_t width, uint32_t height)
		: image(std::move(img)), device(dev), layout(layout), width(width), height(height) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) override;
};

class VulkanNRICommandPool : public NRICommandPool {
   public:
	vk::raii::CommandPool commandPool;

	VulkanNRICommandPool(vk::raii::CommandPool &&pool) : commandPool(std::move(pool)) {}
};

class VulkanNRICommandQueue : public NRICommandQueue {
   public:
	vk::raii::Queue queue;

	VulkanNRICommandQueue(vk::raii::Queue &&q) : queue(std::move(q)) {}

	void submit(NRICommandBuffer &commandBuffer) override;
	void synchronize() override;
};

class VulkanNRICommandBuffer : public NRICommandBuffer {
   public:
	vk::raii::CommandBuffer commandBuffer;
	bool					isRecording;

	void begin() {
		if (!isRecording) {
			vk::CommandBufferBeginInfo beginInfo;
			commandBuffer.begin(beginInfo);
			isRecording = true;
		}
	}

	void end() {
		if (isRecording) {
			commandBuffer.end();
			isRecording = false;
		}
	}

	VulkanNRICommandBuffer(vk::raii::CommandBuffer &&cmdBuf) : commandBuffer(std::move(cmdBuf)), isRecording(false) {}
};

class VulkanNRIQWindow : public NRIQWindow {
	vk::raii::SurfaceKHR surface;
	vk::raii::SwapchainKHR swapChain;

   public:
	VulkanNRIQWindow() : surface(nullptr), swapChain(nullptr) {}

	void closeEvent(QCloseEvent *event) override {
		QWindow::closeEvent(event);
		surface = nullptr;
	}

	vk::raii::SurfaceKHR &getSurface() { return surface; }
	vk::raii::SwapchainKHR &getSwapChain() { return swapChain; }

};

class VulkanNRI : public NRI {
	vk::raii::Instance		 instance;
	vk::raii::PhysicalDevice physicalDevice;
	vk::raii::Device		 device;

	void createInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();

   public:
	VulkanNRI();

	std::unique_ptr<NRIBuffer>		  createBuffer(std::size_t size, BufferUsage usage) override;
	std::unique_ptr<NRIImage2D>		  createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
													NRI::ImageUsage usage) override;
	std::unique_ptr<NRIAllocation>	  allocateMemory(MemoryRequirements memoryRequirements) override;
	std::unique_ptr<NRICommandQueue>  createCommandQueue() override;
	std::unique_ptr<NRICommandBuffer> createCommandBuffer(NRICommandPool &commandPool) override;
	std::unique_ptr<NRICommandPool>	  createCommandPool() override;
	NRIQWindow* createQWidgetSurface(QApplication &app) override;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
	};

	vk::raii::Instance &getInstance() { return instance; }

   private:
	QueueFamilyIndices queueFamilyIndices;
};
