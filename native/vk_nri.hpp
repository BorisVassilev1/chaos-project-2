#pragma once
#include <qelapsedtimer.h>
#include <qlocale.h>
#include <memory>
#include <optional>
#include <iostream>

#include <vulkan/vulkan_raii.hpp>

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

#include "nri.hpp"
#include "nriFactory.hpp"

class VulkanNRI;

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
	vk::raii::Buffer	 buffer;
	vk::Device			 device;
	VulkanNRIAllocation *allocation;

	std::size_t offset = 0;
	std::size_t size   = 0;

   public:
	VulkanNRIBuffer(vk::raii::Buffer &&buf, vk::Device dev, std::size_t size)
		: buffer(std::move(buf)), device(dev), allocation(nullptr), size(size) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;
	void				   *map(std::size_t offset, std::size_t size) override;
	void					unmap() override;

	void copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset, std::size_t dstOffset,
				  std::size_t size) override;
	void bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset) override;
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

	auto	   &getImageView() { return imageView; }
	const auto &getImageView() const { return imageView; }

	uint32_t getWidth() const override { return width; }
	uint32_t getHeight() const override { return height; }
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

	SubmitKey submit(NRICommandBuffer &commandBuffer) override;
	void	  wait(SubmitKey key) override;
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

	void beginRendering(NRIImage2D &renderTarget) override;
	void endRendering() override;

	VulkanNRICommandBuffer(vk::raii::CommandBuffer &&cmdBuf) : commandBuffer(std::move(cmdBuf)), isRecording(false) {}
};

class VulkanNRIProgramBuilder : public NRIProgramBuilder {
   protected:
	VulkanNRI &nri;

	std::pair<std::vector<vk::raii::ShaderModule>, std::vector<vk::PipelineShaderStageCreateInfo>> createShaderModules(
		std::vector<NRI::ShaderCreateInfo> &&stagesInfo, const vk::raii::Device &device);

	std::vector<vk::PushConstantRange> createPushConstantRanges(
		const std::vector<NRI::PushConstantRange> &nriPushConstantRanges);

   public:
	VulkanNRIProgramBuilder(VulkanNRI &nri) : nri(nri) {}

	std::unique_ptr<NRIGraphicsProgram> buildGraphicsProgram() override;
	std::unique_ptr<NRIComputeProgram>	buildComputeProgram() override;
};

class VulkanNRIProgram : virtual NRIProgram {
   private:
	vk::raii::Pipeline		 pipeline;
	vk::raii::PipelineLayout pipelineLayout;

   public:
	VulkanNRIProgram();
	VulkanNRIProgram(vk::raii::Pipeline &&ppln, vk::raii::PipelineLayout &&layout)
		: pipeline(std::move(ppln)), pipelineLayout(std::move(layout)) {}

	void bind(NRICommandBuffer &commandBuffer) override;
	void unbind(NRICommandBuffer &commandBuffer) override;
	
	void setPushConstants(NRICommandBuffer &commandBuffer, const void *data, std::size_t size,
						  std::size_t offset) override;

	friend class VulkanNRIProgramBuilder;
};

class VulkanNRIGraphicsProgram : public VulkanNRIProgram, public NRIGraphicsProgram {
   public:
	using VulkanNRIProgram::VulkanNRIProgram;

	void draw(NRICommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
			  uint32_t firstInstance) override;
};

class VulkanNRIComputeProgram : public VulkanNRIProgram, public NRIComputeProgram {
   public:
	using VulkanNRIProgram::VulkanNRIProgram;

	void dispatch(NRICommandBuffer &commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
				  uint32_t groupCountZ) override;
};

class VulkanNRIQWindow : public NRIQWindow {
	vk::raii::SurfaceKHR   surface;
	vk::raii::SwapchainKHR swapChain;
	VulkanNRICommandQueue  presentQueue;

	vk::raii::Semaphore imageAvailableSemaphore;
	vk::raii::Semaphore renderFinishedSemaphore;
	vk::raii::Fence		inFlightFence;

	std::vector<VulkanNRIImage2D> swapChainImages;

	std::unique_ptr<VulkanNRICommandBuffer> commandBuffer;

	uint32_t width = 0, height = 0;

   protected:
   public:
	VulkanNRIQWindow(VulkanNRI &nri, std::unique_ptr<Renderer> &&renderer);

	void createSwapChain(uint32_t &width, uint32_t &height);
	void drawFrame() override;

	vk::raii::SurfaceKHR		  &getSurface() { return surface; }
	vk::raii::SwapchainKHR		  &getSwapChain() { return swapChain; }
	vk::raii::Queue				  &getPresentQueue() { return presentQueue.queue; }
	std::vector<VulkanNRIImage2D> &getSwapChainImages() { return swapChainImages; }

	NRICommandQueue &getMainQueue() override { return presentQueue; }
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

	std::unique_ptr<NRIBuffer>		   createBuffer(std::size_t size, BufferUsage usage) override;
	std::unique_ptr<NRIImage2D>		   createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
													 NRI::ImageUsage usage) override;
	std::unique_ptr<NRIAllocation>	   allocateMemory(MemoryRequirements memoryRequirements) override;
	std::unique_ptr<NRICommandQueue>   createCommandQueue() override;
	std::unique_ptr<NRICommandBuffer>  createCommandBuffer(const NRICommandPool &commandPool) override;
	std::unique_ptr<NRICommandPool>	   createCommandPool() override;
	std::unique_ptr<NRIProgramBuilder> createProgramBuilder() override;
	NRIQWindow *createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) override;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
	};

	const vk::raii::Instance	   &getInstance() const { return instance; }
	const vk::raii::Device		   &getDevice() const { return device; }
	const vk::raii::PhysicalDevice &getPhysicalDevice() const { return physicalDevice; }
	NRICommandPool				   &getDefaultCommandPool() override { return defaultCommandPool; }

	bool shouldFlipY() const override { return true; }
	void synchronize() const override { device.waitIdle(); }

   private:
	QueueFamilyIndices queueFamilyIndices;
};
