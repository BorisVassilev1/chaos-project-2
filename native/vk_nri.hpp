#pragma once
#include <qelapsedtimer.h>
#include "nri.hpp"
#include "nriFactory.hpp"
#include <memory>
#include <optional>
#include <vulkan/vulkan_raii.hpp>

#include <iostream>

/// A helper class to manage ownership of an object or a reference to an object
/// template arguments can be for example: <vk::raii::Buffer, vk::Buffer>
template <class Owner, class NotOwner>
class OwnerOrNot {
	std::variant<Owner, NotOwner> data;
	bool						  isOwner;

   public:
	OwnerOrNot(Owner &&owner) : data(std::move(owner)), isOwner(true) {}
	OwnerOrNot(NotOwner &notOwner) : data(notOwner), isOwner(false) {}
	NotOwner get() {
		if (isOwner) {
			return (NotOwner)std::get<Owner>(data);
		} else {
			return std::get<NotOwner>(data);
		}
	}
};

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
	OwnerOrNot<vk::raii::Image, vk::Image> image;
	const vk::raii::Device				  &device;
	vk::ImageLayout						   layout;
	vk::Format							   format;
	vk::raii::ImageView					   imageView;

	uint32_t width;
	uint32_t height;

   public:
	void transitionLayout(NRICommandBuffer &commandBuffer, vk::ImageLayout newLayout, vk::AccessFlags srcAccess,
						  vk::AccessFlags dstAccess, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage);

	VulkanNRIImage2D(vk::raii::Image &&img, vk::ImageLayout layout, vk::Format fmt, const vk::raii::Device &dev,
					 vk::raii::ImageView &&imgView, uint32_t width, uint32_t height)
		: image(std::move(img)),
		  device(dev),
		  layout(layout),
		  format(fmt),
		  imageView(std::move(imgView)),
		  width(width),
		  height(height) {}
	VulkanNRIImage2D(vk::Image img, vk::ImageLayout layout, vk::Format fmt, const vk::raii::Device &dev,
					 vk::raii::ImageView &&imgView, uint32_t width, uint32_t height)
		: image(img),
		  device(dev),
		  layout(layout),
		  format(fmt),
		  imageView(std::move(imgView)),
		  width(width),
		  height(height) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) override;

	void prepareForPresent(NRICommandBuffer &commandBuffer) override;
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

	void begin() override {
		if (!isRecording) {
			vk::CommandBufferBeginInfo beginInfo;
			commandBuffer.begin(beginInfo);
			isRecording = true;
		}
	}

	void end() override {
		if (isRecording) {
			commandBuffer.end();
			isRecording = false;
		}
	}

	VulkanNRICommandBuffer(vk::raii::CommandBuffer &&cmdBuf) : commandBuffer(std::move(cmdBuf)), isRecording(false) {}
};

class VulkanNRI;

class VulkanNRIQWindow : public NRIQWindow {
	vk::raii::SurfaceKHR   surface;
	vk::raii::SwapchainKHR swapChain;
	vk::raii::Queue		   presentQueue;

	vk::raii::Semaphore imageAvailableSemaphore;
	vk::raii::Semaphore renderFinishedSemaphore;
	vk::raii::Fence		inFlightFence;

	std::vector<VulkanNRIImage2D> swapChainImages;

	std::unique_ptr<VulkanNRICommandBuffer> commandBuffer;

   protected:
   public:
	VulkanNRIQWindow(VulkanNRI &nri, std::unique_ptr<Renderer> &&renderer);

	void createSwapChain(uint32_t &width, uint32_t &height);
	void drawFrame() override;

	vk::raii::SurfaceKHR		  &getSurface() { return surface; }
	vk::raii::SwapchainKHR		  &getSwapChain() { return swapChain; }
	vk::raii::Queue				  &getPresentQueue() { return presentQueue; }
	std::vector<VulkanNRIImage2D> &getSwapChainImages() { return swapChainImages; }
};

class VulkanNRI : public NRI {
	vk::raii::Instance		 instance;
	vk::raii::PhysicalDevice physicalDevice;
	vk::raii::Device		 device;

	VulkanNRICommandPool defaultCommandPool;

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
	std::unique_ptr<NRICommandBuffer> createCommandBuffer(const NRICommandPool &commandPool) override;
	std::unique_ptr<NRICommandPool>	  createCommandPool() override;
	NRIQWindow *createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) override;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
	};

	const vk::raii::Instance	   &getInstance() const { return instance; }
	const vk::raii::Device		   &getDevice() const { return device; }
	const vk::raii::PhysicalDevice &getPhysicalDevice() const { return physicalDevice; }
	const VulkanNRICommandPool	   &getDefaultCommandPool() const { return defaultCommandPool; }

	void synchronize() const override { device.waitIdle(); }

   private:
	QueueFamilyIndices queueFamilyIndices;
};
