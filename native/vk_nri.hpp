#pragma once
#include <qelapsedtimer.h>
#include <qlocale.h>
#include <memory>
#include <optional>
#include <iostream>

#include <vulkan/vulkan_raii.hpp>

// #include <slang/slang.h>
// #include <slang/slang-com-ptr.h>

#include "nri.hpp"
#include "nriFactory.hpp"
#include "vulkan/vulkan.hpp"

class VulkanNRI;
class VulkanNRIBuffer;
class VulkanNRIImage2D;
class VulkanNRITLAS;

/// A helper class to manage ownership of an object or a reference to an object
/// template arguments can be for example: <vk::raii::Buffer, vk::Buffer>
template <class Owner, class NotOwner>
class OwnerOrNot {
	std::variant<Owner, NotOwner> data;
	bool						  isOwner;

   public:
	OwnerOrNot(Owner &&owner) : data(std::move(owner)), isOwner(true) {}
	OwnerOrNot(NotOwner &notOwner) : data(notOwner), isOwner(false) {}

	OwnerOrNot(OwnerOrNot &&other)			  = default;
	OwnerOrNot &operator=(OwnerOrNot &&other) = default;

	NotOwner get() {
		if (isOwner) {
			return (NotOwner)std::get<Owner>(data);
		} else {
			return std::get<NotOwner>(data);
		}
	}
};

class VulkanDescriptorAllocator {
	VulkanNRI					 &nri;
	vk::raii::DescriptorPool	  pool;
	vk::raii::DescriptorSetLayout descriptorSetLayout;
	vk::raii::DescriptorSet		  bigDescriptorSet;

	int currentBufferDescriptorIndex = 0;
	int currentImageDescriptorIndex	 = 0;
	int currentASDescriptorIndex	 = 0;

   public:
	VulkanDescriptorAllocator(VulkanNRI &nri);

	NRIResourceHandle addUniformBufferDescriptor(VulkanNRIBuffer &buffer);
	NRIResourceHandle addSamplerImageDescriptor(VulkanNRIImage2D &image);
	NRIResourceHandle addAccelerationStructureDescriptor(VulkanNRITLAS &tlas);

	auto	   &getDescriptorSet() { return bigDescriptorSet; }
	const auto &getDescriptorSet() const { return bigDescriptorSet; }

	auto	   &getDescriptorSetLayout() { return descriptorSetLayout; }
	const auto &getDescriptorSetLayout() const { return descriptorSetLayout; }
};

class VulkanNRIAllocation : public NRIAllocation {
	vk::raii::DeviceMemory memory;
	vk::Device			   device;

   public:
	VulkanNRIAllocation(std::nullptr_t) : memory(nullptr), device(nullptr) {}
	VulkanNRIAllocation(VulkanNRI &nri, NRI::MemoryRequirements memoryRequirements);
	VulkanNRIAllocation(vk::raii::DeviceMemory &&mem, vk::Device dev) : memory(std::move(mem)), device(dev) {}

	vk::DeviceMemory getMemory() { return memory; }
	vk::Device		 getDevice() { return device; }
};

class VulkanNRIBuffer : public NRIBuffer {
	VulkanNRI			*nri;
	vk::raii::Buffer	 buffer;
	VulkanNRIAllocation *allocation;

	std::size_t offset = 0;
	std::size_t size   = 0;

   public:
	VulkanNRIBuffer(std::nullptr_t) : nri(nullptr), buffer(nullptr), allocation(nullptr), offset(0), size(0) {}
	VulkanNRIBuffer(VulkanNRI &nri, std::size_t size, NRI::BufferUsage usage);

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;
	void				   *map(std::size_t offset, std::size_t size) override;
	void					unmap() override;

	std::size_t getSize() const override;
	std::size_t getOffset() const override;
	auto	   &getBuffer() { return buffer; }

	void copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset, std::size_t dstOffset,
				  std::size_t size) override;
	void bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset,
							std::size_t stride) override;
	void bindAsIndexBuffer(NRICommandBuffer &commandBuffer, std::size_t offset, NRI::IndexType indexType) override;

	vk::DeviceAddress getAddress();
};

class VulkanNRIImage2D : public NRIImage2D {
	VulkanNRI							  *nri;
	OwnerOrNot<vk::raii::Image, vk::Image> image;
	vk::raii::Device					  *device;
	vk::ImageLayout						   layout;
	vk::Format							   format;
	vk::raii::ImageView					   imageView;
	vk::ImageAspectFlags				   aspectFlags;
	vk::raii::Sampler					   sampler;
	NRIResourceHandle					   imageViewHandle = NRIResourceHandle::INVALID_HANDLE;

	uint32_t width;
	uint32_t height;

	static vk::ImageAspectFlags getAspectFlags(vk::Format format);

   public:
	void transitionLayout(NRICommandBuffer &commandBuffer, vk::ImageLayout newLayout, vk::AccessFlags srcAccess,
						  vk::AccessFlags dstAccess, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage);

	VulkanNRIImage2D(VulkanNRI &nri, vk::raii::Image &&img, vk::ImageLayout layout, vk::Format fmt,
					 vk::raii::Device &dev, vk::raii::ImageView &&imgView, uint32_t width, uint32_t height)
		: nri(&nri),
		  image(std::move(img)),
		  device(&dev),
		  layout(layout),
		  format(fmt),
		  imageView(std::move(imgView)),
		  aspectFlags(getAspectFlags(fmt)),
		  sampler(nullptr),
		  width(width),
		  height(height) {}
	VulkanNRIImage2D(VulkanNRI &nri, vk::Image img, vk::ImageLayout layout, vk::Format fmt, vk::raii::Device &dev,
					 vk::raii::ImageView &&imgView, uint32_t width, uint32_t height)
		: nri(&nri),
		  image(img),
		  device(&dev),
		  layout(layout),
		  format(fmt),
		  imageView(std::move(imgView)),
		  aspectFlags(getAspectFlags(fmt)),
		  sampler(nullptr),
		  width(width),
		  height(height) {}

	VulkanNRIImage2D(VulkanNRI &nri, uint32_t width, uint32_t height, NRI::Format format, NRI::ImageUsage usage);

	VulkanNRIImage2D(VulkanNRIImage2D &&other)			  = default;
	VulkanNRIImage2D &operator=(VulkanNRIImage2D &&other) = default;

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;

	void clear(NRICommandBuffer &commandBuffer, glm::vec4 color) override;

	void prepareForPresent(NRICommandBuffer &commandBuffer) override;
	void copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
				  uint32_t srcRowPitch) override;

	auto	   &getImageView() { return imageView; }
	const auto &getImageView() const { return imageView; }

	auto	   &getSampler() { return sampler; }
	const auto &getSampler() const { return sampler; }

	uint32_t getWidth() const override { return width; }
	uint32_t getHeight() const override { return height; }

	NRIResourceHandle getImageViewHandle() override;
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

	std::unique_ptr<NRIGraphicsProgram>	  buildGraphicsProgram() override;
	std::unique_ptr<NRIComputeProgram>	  buildComputeProgram() override;
	std::unique_ptr<NRIRayTracingProgram> buildRayTracingProgram() override;
};

class VulkanNRIProgram : virtual NRIProgram {
   protected:
	VulkanNRI				&nri;
	vk::raii::Pipeline		 pipeline;
	vk::raii::PipelineLayout pipelineLayout;

   public:
	VulkanNRIProgram(VulkanNRI &nri, vk::raii::Pipeline &&ppln, vk::raii::PipelineLayout &&layout)
		: nri(nri), pipeline(std::move(ppln)), pipelineLayout(std::move(layout)) {}

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
	void drawIndexed(NRICommandBuffer &commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
					 int32_t vertexOffset, uint32_t firstInstance) override;
};

class VulkanNRIComputeProgram : public VulkanNRIProgram, public NRIComputeProgram {
   public:
	using VulkanNRIProgram::VulkanNRIProgram;

	void dispatch(NRICommandBuffer &commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
				  uint32_t groupCountZ) override;
};

class VulkanNRIRayTracingProgram : public VulkanNRIProgram, public NRIRayTracingProgram {
   public:
	using VulkanNRIProgram::VulkanNRIProgram;

	void traceRays(NRICommandBuffer &commandBuffer, uint32_t width, uint32_t height, uint32_t depth) override;
};

class VulkanMemoryCache {
	VulkanNRIBuffer		buffer;
	VulkanNRIAllocation allocation;

	VulkanMemoryCache() : buffer(nullptr), allocation(nullptr) {}

	void assureSize(VulkanNRI &nri, std::size_t size);

   public:
	VulkanNRIBuffer &getAccelerationStructureScratch(VulkanNRI									&nri,
													 vk::AccelerationStructureBuildSizesInfoKHR &sizeInfo);

   private:		// this does not work currently
	static VulkanMemoryCache &getInstance();
	static void				  destroy();
};

class VulkanNRIBLAS : public NRIBLAS {
	VulkanNRI						  *nri;
	vk::raii::AccelerationStructureKHR accelerationStructure;
	VulkanNRIBuffer					   asBuffer;
	VulkanNRIAllocation				   asMemory;
	std::size_t						   indexOffset = 0;

	struct TemporaryBuildInfo {
		vk::AccelerationStructureBuildSizesInfoKHR	  sizeInfo;
		vk::AccelerationStructureGeometryKHR		  geometry;
		vk::AccelerationStructureBuildRangeInfoKHR	  buildRangeInfo;
		vk::AccelerationStructureBuildGeometryInfoKHR buildGeometryInfo;
		VulkanNRIBuffer								 *vertexBuffer;
		VulkanNRIBuffer								 *indexBuffer;
		VulkanNRIBuffer								  scratchBuffer = nullptr;
		VulkanNRIAllocation							  scratchMemory = nullptr;
	};
	std::unique_ptr<TemporaryBuildInfo> tempBuildInfo;

   public:
	VulkanNRIBLAS(VulkanNRI &nri, VulkanNRIBuffer &vertexBuffer, NRI::Format vertexFormat, std::size_t vertexOffset,
				  uint32_t vertexCount, std::size_t vertexStride, VulkanNRIBuffer &indexBuffer,
				  NRI::IndexType indexType, std::size_t indexOffset);

	void build(NRICommandBuffer &commandBuffer) override;
	void buildFinished() override;

	auto			 &getAccelerationStructure() { return accelerationStructure; }
	const auto		 &getAccelerationStructure() const { return accelerationStructure; }
	vk::DeviceAddress getAddress() const;
};

class VulkanNRITLAS : public NRITLAS {
	VulkanNRI						  *nri;
	vk::raii::AccelerationStructureKHR as;
	VulkanNRIBuffer					   asBuffer;
	VulkanNRIAllocation				   asMemory;
	NRIResourceHandle				   handle = NRIResourceHandle::INVALID_HANDLE;

	struct TemporaryBuildInfo {
		vk::AccelerationStructureGeometryKHR		  geometry;
		vk::AccelerationStructureBuildRangeInfoKHR	  buildRangeInfo;
		vk::AccelerationStructureBuildGeometryInfoKHR buildGeometryInfo;
		VulkanNRIBuffer								  instanceUploadBuffer = nullptr;
		VulkanNRIAllocation							  instanceUploadMemory = nullptr;

		VulkanNRIBuffer		instanceBuffer = nullptr;
		VulkanNRIAllocation instanceMemory = nullptr;

		VulkanNRIBuffer		scratchBuffer = nullptr;
		VulkanNRIAllocation scratchMemory = nullptr;
	};

	std::unique_ptr<TemporaryBuildInfo> tempBuildInfo;

   public:
	VulkanNRITLAS(VulkanNRI &nri, const std::span<const NRIBLAS *> &blases,
				  std::optional<std::span<glm::mat4x3>> transforms = std::nullopt);

	void build(NRICommandBuffer &commandBuffer) override;
	void buildFinished() override;
	NRIResourceHandle getHandle() override;

	auto	   &getTLAS() { return as; }
	const auto &getTLAS() const { return as; }
};

class VulkanNRIQWindow : public NRIQWindow {
	vk::raii::SurfaceKHR   surface;
	vk::raii::SwapchainKHR swapChain;
	VulkanNRICommandQueue  presentQueue;

	vk::raii::Semaphore imageAvailableSemaphore;
	vk::raii::Semaphore renderFinishedSemaphore;
	vk::raii::Fence		inFlightFence;

	std::vector<VulkanNRIImage2D>	   swapChainImages;
	std::optional<VulkanNRIImage2D>	   depthImage;
	std::optional<VulkanNRIAllocation> depthImageAllocation;

	std::unique_ptr<VulkanNRICommandBuffer> commandBuffer;

	uint32_t width = 0, height = 0;

   protected:
   public:
	VulkanNRIQWindow(VulkanNRI &nri, std::unique_ptr<Renderer> &&renderer);

	void createSwapChain(uint32_t &width, uint32_t &height);
	void drawFrame() override;

	void beginRendering(NRICommandBuffer &cmdBuf, NRIImage2D &renderTarget) override;
	void endRendering(NRICommandBuffer &cmdBuf) override;

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

	VulkanNRICommandPool					 defaultCommandPool;
	std::optional<VulkanDescriptorAllocator> descriptorAllocator;

	void createInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();

   public:
	VulkanNRI();
	~VulkanNRI();

	std::unique_ptr<NRIBuffer>		   createBuffer(std::size_t size, BufferUsage usage) override;
	std::unique_ptr<NRIImage2D>		   createImage2D(uint32_t width, uint32_t height, NRI::Format fmt,
													 NRI::ImageUsage usage) override;
	std::unique_ptr<NRIAllocation>	   allocateMemory(MemoryRequirements memoryRequirements) override;
	std::unique_ptr<NRICommandQueue>   createCommandQueue() override;
	std::unique_ptr<NRICommandBuffer>  createCommandBuffer(const NRICommandPool &commandPool) override;
	std::unique_ptr<NRICommandPool>	   createCommandPool() override;
	std::unique_ptr<NRIProgramBuilder> createProgramBuilder() override;
	NRIQWindow				*createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) override;
	std::unique_ptr<NRIBLAS> createBLAS(NRIBuffer &vertexBuffer, NRI::Format vertexFormat, std::size_t vertexOffset,
										uint32_t vertexCount, std::size_t vertexStride, NRIBuffer &indexBuffer,
										NRI::IndexType indexType, std::size_t indexOffset) override;

	std::unique_ptr<NRITLAS> createTLAS(const std::span<const NRIBLAS *>	 &blases,
										std::optional<std::span<glm::mat4x3>> transforms = std::nullopt) override;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
	};

	const vk::raii::Instance	   &getInstance() const { return instance; }
	const vk::raii::Device		   &getDevice() const { return device; }
	vk::raii::Device			   &getDevice() { return device; }
	const vk::raii::PhysicalDevice &getPhysicalDevice() const { return physicalDevice; }
	NRICommandPool				   &getDefaultCommandPool() override { return defaultCommandPool; }
	VulkanDescriptorAllocator	   &getDescriptorAllocator() {
		 assert(descriptorAllocator.has_value());
		 return descriptorAllocator.value();
	}

	bool shouldFlipY() const override { return true; }
	bool supportsRayTracing() const override { return true; }
	bool supportsTextures() const override { return true; }
	void synchronize() const override { device.waitIdle(); }

   private:
	QueueFamilyIndices queueFamilyIndices;
};
