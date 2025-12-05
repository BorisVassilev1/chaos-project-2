#include "vk_nri.hpp"
#include <iostream>
#include <ostream>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

static const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	//"VK_LAYER_NV_optimus"
};

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif

static VkFormat nriFormat2vkFormat[] = {
	VK_FORMAT_UNDEFINED,
	VK_FORMAT_R4G4_UNORM_PACK8,
	VK_FORMAT_R4G4B4A4_UNORM_PACK16,
	VK_FORMAT_B4G4R4A4_UNORM_PACK16,
	VK_FORMAT_R5G6B5_UNORM_PACK16,
	VK_FORMAT_B5G6R5_UNORM_PACK16,
	VK_FORMAT_R5G5B5A1_UNORM_PACK16,
	VK_FORMAT_B5G5R5A1_UNORM_PACK16,
	VK_FORMAT_A1R5G5B5_UNORM_PACK16,
	VK_FORMAT_R8_UNORM,
	VK_FORMAT_R8_SNORM,
	VK_FORMAT_R8_USCALED,
	VK_FORMAT_R8_SSCALED,
	VK_FORMAT_R8_UINT,
	VK_FORMAT_R8_SINT,
	VK_FORMAT_R8_SRGB,
	VK_FORMAT_R8G8_UNORM,
	VK_FORMAT_R8G8_SNORM,
	VK_FORMAT_R8G8_USCALED,
	VK_FORMAT_R8G8_SSCALED,
	VK_FORMAT_R8G8_UINT,
	VK_FORMAT_R8G8_SINT,
	VK_FORMAT_R8G8_SRGB,
	VK_FORMAT_R8G8B8_UNORM,
	VK_FORMAT_R8G8B8_SNORM,
	VK_FORMAT_R8G8B8_USCALED,
	VK_FORMAT_R8G8B8_SSCALED,
	VK_FORMAT_R8G8B8_UINT,
	VK_FORMAT_R8G8B8_SINT,
	VK_FORMAT_R8G8B8_SRGB,
	VK_FORMAT_B8G8R8_UNORM,
	VK_FORMAT_B8G8R8_SNORM,
	VK_FORMAT_B8G8R8_USCALED,
	VK_FORMAT_B8G8R8_SSCALED,
	VK_FORMAT_B8G8R8_UINT,
	VK_FORMAT_B8G8R8_SINT,
	VK_FORMAT_B8G8R8_SRGB,
	VK_FORMAT_R8G8B8A8_UNORM,
	VK_FORMAT_R8G8B8A8_SNORM,
	VK_FORMAT_R8G8B8A8_USCALED,
	VK_FORMAT_R8G8B8A8_SSCALED,
	VK_FORMAT_R8G8B8A8_UINT,
	VK_FORMAT_R8G8B8A8_SINT,
	VK_FORMAT_R8G8B8A8_SRGB,
	VK_FORMAT_B8G8R8A8_UNORM,
	VK_FORMAT_B8G8R8A8_SNORM,
	VK_FORMAT_B8G8R8A8_USCALED,
	VK_FORMAT_B8G8R8A8_SSCALED,
	VK_FORMAT_B8G8R8A8_UINT,
	VK_FORMAT_B8G8R8A8_SINT,
	VK_FORMAT_B8G8R8A8_SRGB,
	VK_FORMAT_A8B8G8R8_UNORM_PACK32,
	VK_FORMAT_A8B8G8R8_SNORM_PACK32,
	VK_FORMAT_A8B8G8R8_USCALED_PACK32,
	VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
	VK_FORMAT_A8B8G8R8_UINT_PACK32,
	VK_FORMAT_A8B8G8R8_SINT_PACK32,
	VK_FORMAT_A8B8G8R8_SRGB_PACK32,
	VK_FORMAT_A2R10G10B10_UNORM_PACK32,
	VK_FORMAT_A2R10G10B10_SNORM_PACK32,
	VK_FORMAT_A2R10G10B10_USCALED_PACK32,
	VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
	VK_FORMAT_A2R10G10B10_UINT_PACK32,
	VK_FORMAT_A2R10G10B10_SINT_PACK32,
	VK_FORMAT_A2B10G10R10_UNORM_PACK32,
	VK_FORMAT_A2B10G10R10_SNORM_PACK32,
	VK_FORMAT_A2B10G10R10_USCALED_PACK32,
	VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
	VK_FORMAT_A2B10G10R10_UINT_PACK32,
	VK_FORMAT_A2B10G10R10_SINT_PACK32,
	VK_FORMAT_R16_UNORM,
	VK_FORMAT_R16_SNORM,
	VK_FORMAT_R16_USCALED,
	VK_FORMAT_R16_SSCALED,
	VK_FORMAT_R16_UINT,
	VK_FORMAT_R16_SINT,
	VK_FORMAT_R16_SFLOAT,
	VK_FORMAT_R16G16_UNORM,
	VK_FORMAT_R16G16_SNORM,
	VK_FORMAT_R16G16_USCALED,
	VK_FORMAT_R16G16_SSCALED,
	VK_FORMAT_R16G16_UINT,
	VK_FORMAT_R16G16_SINT,
	VK_FORMAT_R16G16_SFLOAT,
	VK_FORMAT_R16G16B16_UNORM,
	VK_FORMAT_R16G16B16_SNORM,
	VK_FORMAT_R16G16B16_USCALED,
	VK_FORMAT_R16G16B16_SSCALED,
	VK_FORMAT_R16G16B16_UINT,
	VK_FORMAT_R16G16B16_SINT,
	VK_FORMAT_R16G16B16_SFLOAT,
	VK_FORMAT_R16G16B16A16_UNORM,
	VK_FORMAT_R16G16B16A16_SNORM,
	VK_FORMAT_R16G16B16A16_USCALED,
	VK_FORMAT_R16G16B16A16_SSCALED,
	VK_FORMAT_R16G16B16A16_UINT,
	VK_FORMAT_R16G16B16A16_SINT,
	VK_FORMAT_R16G16B16A16_SFLOAT,
	VK_FORMAT_R32_UINT,
	VK_FORMAT_R32_SINT,
	VK_FORMAT_R32_SFLOAT,
	VK_FORMAT_R32G32_UINT,
	VK_FORMAT_R32G32_SINT,
	VK_FORMAT_R32G32_SFLOAT,
	VK_FORMAT_R32G32B32_UINT,
	VK_FORMAT_R32G32B32_SINT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32A32_UINT,
	VK_FORMAT_R32G32B32A32_SINT,
	VK_FORMAT_R32G32B32A32_SFLOAT,
	VK_FORMAT_R64_UINT,
	VK_FORMAT_R64_SINT,
	VK_FORMAT_R64_SFLOAT,
	VK_FORMAT_R64G64_UINT,
	VK_FORMAT_R64G64_SINT,
	VK_FORMAT_R64G64_SFLOAT,
	VK_FORMAT_R64G64B64_UINT,
	VK_FORMAT_R64G64B64_SINT,
	VK_FORMAT_R64G64B64_SFLOAT,
	VK_FORMAT_R64G64B64A64_UINT,
	VK_FORMAT_R64G64B64A64_SINT,
	VK_FORMAT_R64G64B64A64_SFLOAT,
	VK_FORMAT_B10G11R11_UFLOAT_PACK32,
	VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
	VK_FORMAT_D16_UNORM,
	VK_FORMAT_X8_D24_UNORM_PACK32,
	VK_FORMAT_D32_SFLOAT,
	VK_FORMAT_S8_UINT,
	VK_FORMAT_D16_UNORM_S8_UINT,
	VK_FORMAT_D24_UNORM_S8_UINT,
	VK_FORMAT_D32_SFLOAT_S8_UINT,
};

bool checkValidationLayerSupport() {
	auto availableLayers = vk::enumerateInstanceLayerProperties();

	for (const char *layerName : validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) { return false; }
	}

	return true;
}

static VulkanNRI::QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice &device) {
	VulkanNRI::QueueFamilyIndices indices;

	auto queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) { indices.graphicsFamily = i; }
		i++;
	}

	return indices;
}

static bool isDeviceSuitable(const vk::raii::PhysicalDevice &device) {
	VulkanNRI::QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.graphicsFamily.has_value();
}

void VulkanNRI::createInstance() {
	vk::ApplicationInfo appInfo("VulkanNRIApp", 1, "NoEngine", 1, vk::ApiVersion14);

	vk::InstanceCreateInfo createInfo({}, &appInfo);

	if (enableValidationLayers) {
		if (!checkValidationLayerSupport()) {
			// throw std::runtime_error("Validation layers requested, but not available!");
			std::cout << "Validation layers requested, but not available!" << std::endl;
		}

		createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	instance = vk::raii::Instance(vk::raii::Context(), createInfo);
}

void VulkanNRI::pickPhysicalDevice() {
	auto physicalDevices = instance.enumeratePhysicalDevices();
	if (physicalDevices.size() == 0) { throw std::runtime_error("Failed to find GPUs with Vulkan support!"); }

	for (const auto &device : physicalDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	queueFamilyIndices = findQueueFamilies(physicalDevice);
	if (physicalDevice == nullptr) { throw std::runtime_error("Failed to find a suitable GPU!"); }
}

void VulkanNRI::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	float					  prio = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo({}, indices.graphicsFamily.value(), 1, &prio);

	vk::PhysicalDeviceFeatures deviceFeatures{};

	vk::DeviceCreateInfo createInfo(
		{}, 1, &queueCreateInfo, enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0,
		enableValidationLayers ? validationLayers.data() : nullptr, 0, nullptr, &deviceFeatures);

	device = vk::raii::Device(physicalDevice, createInfo);
}

VulkanNRI::VulkanNRI() : instance(nullptr), physicalDevice(nullptr), device(nullptr) {
	createInstance();
	pickPhysicalDevice();
	createLogicalDevice();

	std::cout << "VulkanNRI initialized with device: " << physicalDevice.getProperties().deviceName << std::endl;
}

NRI::MemoryRequirements VulkanNRIBuffer::getMemoryRequirements() {
	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

	return NRI::MemoryRequirements(memRequirements.size, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE,
								   memRequirements.alignment);
}

void VulkanNRIBuffer::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	VulkanNRIAllocation &vulkanAllocation = static_cast<VulkanNRIAllocation &>(allocation);

	buffer.bindMemory(vulkanAllocation.getMemory(), offset);
}

NRI::MemoryRequirements &NRI::MemoryRequirements::setTypeRequest(MemoryTypeRequest tr) {
	typeRequest = tr;
	return *this;
}

std::unique_ptr<NRIImage2D> VulkanNRI::createImage2D(uint32_t width, uint32_t height, NRI::Format format,
													 NRI::ImageUsage usage) {
	assert(format != NRI::Format::FORMAT_UNDEFINED);
	assert(format < NRI::Format::_FORMAT_NUM);
	assert(width > 0 && height > 0);
	VkFormat vkFormat = nriFormat2vkFormat[static_cast<int>(format)];

	vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, vk::Format(vkFormat),
								  vk::Extent3D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1), 1, 1,
								  vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
								  static_cast<vk::ImageUsageFlagBits>(usage), vk::SharingMode::eExclusive, 0, nullptr,
								  vk::ImageLayout::eUndefined);

	vk::raii::Image image = vk::raii::Image(device, imageInfo);

	return std::make_unique<VulkanNRIImage2D>(std::move(image), vk::ImageLayout::eUndefined, device, width, height);
}

std::unique_ptr<NRIBuffer> VulkanNRI::createBuffer(std::size_t size, NRI::BufferUsage usage) {
	vk::BufferUsageFlags bufferUsageFlags;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_VERTEX) bufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_INDEX) bufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_UNIFORM) bufferUsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_STORAGE) bufferUsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC) bufferUsageFlags |= vk::BufferUsageFlagBits::eTransferSrc;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST) bufferUsageFlags |= vk::BufferUsageFlagBits::eTransferDst;

	vk::BufferCreateInfo bufferInfo({}, size, bufferUsageFlags, vk::SharingMode::eExclusive);

	vk::raii::Buffer buffer = vk::raii::Buffer(device, bufferInfo);

	return std::make_unique<VulkanNRIBuffer>(std::move(buffer), device);
}

NRI::MemoryRequirements VulkanNRIImage2D::getMemoryRequirements() {
	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();

	return NRI::MemoryRequirements(memRequirements.size, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE,
								   memRequirements.alignment);
}

void VulkanNRIImage2D::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	VulkanNRIAllocation &vulkanAllocation = static_cast<VulkanNRIAllocation &>(allocation);

	image.bindMemory(vulkanAllocation.getMemory(), offset);
}

void VulkanNRIImage2D::clear(NRICommandBuffer &commandBuffer, glm::vec4 color) {
	auto &vkBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	vk::ClearColorValue v{color.r, color.g, color.b, color.a};
	vkBuf.begin();

	transitionLayout(vkBuf, vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits::eNone,
					 vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTopOfPipe,
					 vk::PipelineStageFlagBits::eTransfer);
	vkBuf.commandBuffer.clearColorImage(image, vk::ImageLayout::eTransferDstOptimal, v,
										{vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)});
}

void VulkanNRIImage2D::transitionLayout(NRICommandBuffer &commandBuffer, vk::ImageLayout newLayout,
										vk::AccessFlagBits srcAccess, vk::AccessFlagBits dstAccess,
										vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage) {
	auto &vkBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	vk::ImageMemoryBarrier barrier(srcAccess, dstAccess, layout, newLayout, vk::QueueFamilyIgnored,
								   vk::QueueFamilyIgnored, image, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
	vkBuf.begin();
	vkBuf.commandBuffer.pipelineBarrier(srcStage, dstStage,
										vk::DependencyFlagBits::eByRegion, {}, {}, {barrier});
}

std::unique_ptr<NRICommandPool> VulkanNRI::createCommandPool() {
	vk::CommandPoolCreateInfo poolCI(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
									 queueFamilyIndices.graphicsFamily.value());

	vk::raii::CommandPool pool = vk::raii::CommandPool(device, poolCI);

	return std::make_unique<VulkanNRICommandPool>(std::move(pool));
}

std::unique_ptr<NRICommandQueue> VulkanNRI::createCommandQueue() {
	vk::CommandPoolCreateInfo poolCI(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
									 queueFamilyIndices.graphicsFamily.value());

	vk::raii::CommandPool pool = vk::raii::CommandPool(device, poolCI);

	vk::raii::Queue queue = vk::raii::Queue(device, queueFamilyIndices.graphicsFamily.value(), 0);

	return std::make_unique<VulkanNRICommandQueue>(std::move(queue));
};

std::unique_ptr<NRIAllocation> VulkanNRI::allocateMemory(NRI::MemoryRequirements memoryRequirements) {
	vk::MemoryPropertyFlags properties;
	if (memoryRequirements.typeRequest & NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD)
		properties |= vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	if (memoryRequirements.typeRequest & NRI::MemoryTypeRequest::MEMORY_TYPE_READBACK)
		properties |= vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	if (memoryRequirements.typeRequest & NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE)
		properties |= vk::MemoryPropertyFlagBits::eDeviceLocal;

	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

	uint32_t memoryTypeIndex = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			memoryTypeIndex = i;
			break;
		}
	}

	if (memoryTypeIndex == -1u) { throw std::runtime_error("Failed to find suitable memory type!"); }

	vk::MemoryAllocateInfo allocInfo(memoryRequirements.size, memoryTypeIndex);

	vk::raii::DeviceMemory memory = vk::raii::DeviceMemory(device, allocInfo);

	return std::make_unique<VulkanNRIAllocation>(std::move(memory), device);
}

std::unique_ptr<NRICommandBuffer> VulkanNRI::createCommandBuffer(NRICommandPool &pool) {
	vk::CommandBufferAllocateInfo allocInfo(static_cast<VulkanNRICommandPool &>(pool).commandPool,
											vk::CommandBufferLevel::ePrimary, 1);
	vk::raii::CommandBuffers	  buffers(device, allocInfo);
	return std::make_unique<VulkanNRICommandBuffer>(std::move(buffers[0]));
};

void VulkanNRICommandQueue::submit(NRICommandBuffer &commandBuffer) {
	auto &cmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	cmdBuf.end();
	vk::CommandBuffer vk = cmdBuf.commandBuffer;
	queue.submit(vk::SubmitInfo(0, nullptr, nullptr, 1, &vk), nullptr);
}

void VulkanNRICommandQueue::synchronize() {
	queue.waitIdle();
}
