#include "vk_nri.hpp"
#include <qapplication.h>
#include <qguiapplication_platform.h>
#include <QtWidgets>
#include <iostream>
#include <ostream>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_core.h>

#include <dxc/dxcapi.h>

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

const char *extensions[] = {
	"VK_KHR_surface",
#ifdef __linux__
	"VK_KHR_xlib_surface",
#elif defined(_WIN32)
	"VK_KHR_win32_surface",
#endif
	"VK_EXT_debug_utils",
	//"VK_KHR_dynamic_rendering"
};

bool checkExtensionSupport() {
	auto availableExtensions = vk::enumerateInstanceExtensionProperties();

	for (const char *extensionName : extensions) {
		bool extensionFound = false;

		for (const auto &extensionProperties : availableExtensions) {
			if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound) {
			throw std::runtime_error("Required extension not found!: " + std::string(extensionName));
		}
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
	vk::ApplicationInfo appInfo("VulkanNRIApp", 1, "NoEngine", 1, vk::ApiVersion13);

	vk::InstanceCreateInfo createInfo({}, &appInfo);

	if (enableValidationLayers) {
		if (!checkValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested, but not available!");
		}

		createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	checkExtensionSupport();

	createInfo.enabledExtensionCount   = sizeof(extensions) / sizeof(extensions[0]);
	createInfo.ppEnabledExtensionNames = extensions;

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

static std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
													 VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};

void VulkanNRI::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	float					  prio = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo({}, indices.graphicsFamily.value(), 1, &prio);

	vk::PhysicalDeviceFeatures				   deviceFeatures{};
	vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature(VK_TRUE);

	vk::DeviceCreateInfo createInfo(
		{}, 1, &queueCreateInfo, enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0,
		enableValidationLayers ? validationLayers.data() : nullptr, static_cast<uint32_t>(deviceExtensions.size()),
		deviceExtensions.data(), &deviceFeatures, &dynamicRenderingFeature);

	device = vk::raii::Device(physicalDevice, createInfo);
}

VulkanNRI::VulkanNRI() : instance(nullptr), physicalDevice(nullptr), device(nullptr), defaultCommandPool(nullptr) {
	createInstance();
	pickPhysicalDevice();
	createLogicalDevice();

	auto dcp		   = createCommandPool();
	defaultCommandPool = std::move(static_cast<VulkanNRICommandPool &>(*dcp));

	std::cout << "VulkanNRI initialized with device: " << physicalDevice.getProperties().deviceName << std::endl;
}

NRI::MemoryRequirements VulkanNRIBuffer::getMemoryRequirements() {
	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();

	return NRI::MemoryRequirements(memRequirements.size, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE,
								   memRequirements.alignment);
}

void VulkanNRIBuffer::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	VulkanNRIAllocation &vulkanAllocation = static_cast<VulkanNRIAllocation &>(allocation);
	this->offset						  = offset;

	buffer.bindMemory(vulkanAllocation.getMemory(), offset);
	this->allocation = &vulkanAllocation;
}
void *VulkanNRIBuffer::map(std::size_t offset, std::size_t size) {
	assert(allocation != nullptr);
	void *data;
	vkMapMemory(allocation->getDevice(), allocation->getMemory(), offset, size, 0, &data);
	return data;
}

void VulkanNRIBuffer::unmap() {
	assert(allocation != nullptr);

	//vk::MappedMemoryRange memoryRange(allocation->getMemory(), offset, VK_WHOLE_SIZE);
	//auto				  res = allocation->getDevice().flushMappedMemoryRanges(1, &memoryRange);
	//assert(res == vk::Result::eSuccess);

	vkUnmapMemory(allocation->getDevice(), allocation->getMemory());
}

void VulkanNRIBuffer::copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
							   std::size_t dstOffset, std::size_t size) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	auto &vkSrcBuf = static_cast<VulkanNRIBuffer &>(srcBuffer);

	if (!vkCmdBuf.isRecording) vkCmdBuf.begin();

	//vk::BufferMemoryBarrier bufferBarrier(vk::AccessFlagBits::eHostWrite,
	//									  vk::AccessFlagBits::eTransferRead, VK_QUEUE_FAMILY_IGNORED,
	//									  VK_QUEUE_FAMILY_IGNORED, vkSrcBuf.buffer, srcOffset, size);
	//vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost,
	//									   vk::PipelineStageFlagBits::eTransfer, {}, {}, {bufferBarrier}, {});

	vkCmdBuf.commandBuffer.copyBuffer(vkSrcBuf.buffer, this->buffer, vk::BufferCopy(srcOffset, dstOffset, size));

	vk::BufferMemoryBarrier bufferBarrier2(
		vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eMemoryRead,
		VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, this->buffer, dstOffset, size);
	vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
										   vk::PipelineStageFlagBits::eTopOfPipe, {}, {}, {bufferBarrier2}, {});
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
	vk::Format vkFormat = (vk::Format)nriFormat2vkFormat[static_cast<int>(format)];

	vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, vk::Format(vkFormat),
								  vk::Extent3D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1), 1, 1,
								  vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
								  static_cast<vk::ImageUsageFlagBits>(usage), vk::SharingMode::eExclusive, 0, nullptr,
								  vk::ImageLayout::eUndefined);

	vk::raii::Image image = vk::raii::Image(device, imageInfo);

	return std::make_unique<VulkanNRIImage2D>(std::move(image), vk::ImageLayout::eUndefined, vkFormat, device,
											  vk::raii::ImageView(nullptr), width, height);
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

	return std::make_unique<VulkanNRIBuffer>(std::move(buffer), device, size);
}

NRI::MemoryRequirements VulkanNRIImage2D::getMemoryRequirements() {
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(*device, image.get(), &memRequirements);

	return NRI::MemoryRequirements(memRequirements.size, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE,
								   memRequirements.alignment);
}

void VulkanNRIImage2D::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	VulkanNRIAllocation &vulkanAllocation = static_cast<VulkanNRIAllocation &>(allocation);

	vkBindImageMemory(*device, image.get(), vulkanAllocation.getMemory(), offset);

	vk::ImageViewCreateInfo imageViewInfo(
		{}, image.get(), vk::ImageViewType::e2D, vk::Format(this->format),
		vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
							 vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

	vk::raii::ImageView imageView = vk::raii::ImageView(device, imageViewInfo);
}

void VulkanNRIImage2D::clear(NRICommandBuffer &commandBuffer, glm::vec4 color) {
	auto &vkBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	vk::ClearColorValue v{color.r, color.g, color.b, color.a};
	vkBuf.begin();

	transitionLayout(vkBuf, vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits::eNone,
					 vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTopOfPipe,
					 vk::PipelineStageFlagBits::eTransfer);
	vkBuf.commandBuffer.clearColorImage(image.get(), vk::ImageLayout::eTransferDstOptimal, v,
										{vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)});
}

void VulkanNRIImage2D::prepareForPresent(NRICommandBuffer &commandBuffer) {
	auto &vkBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	transitionLayout(vkBuf, vk::ImageLayout::ePresentSrcKHR,
					 vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eColorAttachmentWrite,
					 vk::AccessFlagBits::eMemoryRead,
					 vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eColorAttachmentOutput,
					 vk::PipelineStageFlagBits::eBottomOfPipe);
}

void VulkanNRIImage2D::transitionLayout(NRICommandBuffer &commandBuffer, vk::ImageLayout newLayout,
										vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
										vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage) {
	auto &vkBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	vk::ImageMemoryBarrier barrier(srcAccess, dstAccess, layout, newLayout, vk::QueueFamilyIgnored,
								   vk::QueueFamilyIgnored, image.get(), {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
	vkBuf.begin();
	vkBuf.commandBuffer.pipelineBarrier(srcStage, dstStage, vk::DependencyFlagBits::eByRegion, {}, {}, {barrier});
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

VulkanNRIQWindow::VulkanNRIQWindow(VulkanNRI &nri, std::unique_ptr<Renderer> &&renderer)
	: NRIQWindow(nri, std::move(renderer)),
	  surface(nullptr),
	  swapChain(nullptr),
	  presentQueue(nullptr),
	  imageAvailableSemaphore(nri.getDevice(), vk::SemaphoreCreateInfo()),
	  renderFinishedSemaphore(nri.getDevice(), vk::SemaphoreCreateInfo()),
	  inFlightFence(nri.getDevice(), vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)) {
	if (imageAvailableSemaphore == nullptr || renderFinishedSemaphore == nullptr || inFlightFence == nullptr) {
		throw std::runtime_error("Failed to create synchronization objects for a frame!");
	}
	commandBuffer = std::unique_ptr<VulkanNRICommandBuffer>(
		(VulkanNRICommandBuffer *)nri.createCommandBuffer(nri.getDefaultCommandPool()).release());
}

void VulkanNRIQWindow::createSwapChain(uint32_t &width, uint32_t &height) {
	this->width								= width;
	this->height							= height;
	auto					  &nri			= static_cast<const VulkanNRI &>(this->nri);
	vk::SurfaceCapabilitiesKHR capabilities = nri.getPhysicalDevice().getSurfaceCapabilitiesKHR(*surface);

	width  = capabilities.currentExtent.width;
	height = capabilities.currentExtent.height;

	vk::SwapchainCreateInfoKHR swapChainInfo(
		{}, *surface, capabilities.minImageCount + 1, vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear,
		capabilities.currentExtent, 1, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive, 0, nullptr, vk::SurfaceTransformFlagBitsKHR::eIdentity,
		vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eFifo, VK_TRUE, *swapChain);

	static PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR =
		reinterpret_cast<PFN_vkCreateSwapchainKHR>(nri.getInstance().getProcAddr("vkCreateSwapchainKHR"));
	assert(vkCreateSwapchainKHR != nullptr);

	VkSwapchainKHR _swapChain;
	if (vkCreateSwapchainKHR(*nri.getDevice(), (VkSwapchainCreateInfoKHR *)&swapChainInfo, nullptr, &_swapChain) !=
		VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan swap chain!");
	}

	swapChain			 = vk::raii::SwapchainKHR(nri.getDevice(), _swapChain);
	auto swapChainImages = swapChain.getImages();
	this->swapChainImages.clear();
	for (const auto &image : swapChainImages) {
		vk::ImageViewCreateInfo imageViewInfo(
			{}, image, vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Unorm,
			vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		vk::raii::ImageView imageView = vk::raii::ImageView(nri.getDevice(), imageViewInfo);

		VulkanNRIImage2D nriImage = VulkanNRIImage2D(image, vk::ImageLayout::eUndefined, vk::Format::eB8G8R8A8Unorm,
													 nri.getDevice(), std::move(imageView), width, height);
		this->swapChainImages.push_back(std::move(nriImage));
		this->swapChainImages.back().transitionLayout(
			*commandBuffer, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eMemoryRead,
			vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe);
	}
}

void VulkanNRIQWindow::drawFrame() {
	auto &nri = static_cast<const VulkanNRI &>(this->nri);

	auto result = nri.getDevice().waitForFences({inFlightFence}, VK_TRUE, UINT64_MAX);
	assert(result == vk::Result::eSuccess);
	nri.getDevice().resetFences({inFlightFence});
	assert(result == vk::Result::eSuccess);
	auto imageIndex = swapChain.acquireNextImage(UINT64_MAX, *imageAvailableSemaphore, nullptr);

	swapChainImages[imageIndex.value].transitionLayout(
		*commandBuffer, vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits::eMemoryRead,
		vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eBottomOfPipe,
		vk::PipelineStageFlagBits::eColorAttachmentOutput);

	renderer->render(swapChainImages[imageIndex.value], *commandBuffer);

	vk::PipelineStageFlags stages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	presentQueue.queue.submit(vk::SubmitInfo(1, &(*imageAvailableSemaphore), &stages, 1, &*commandBuffer->commandBuffer,
											 1, &(*renderFinishedSemaphore)),
							  *inFlightFence);

	vk::PresentInfoKHR presentInfo =
		vk::PresentInfoKHR(1, &*renderFinishedSemaphore, 1, &*swapChain, &imageIndex.value);

	// result = presentQueue.presentKHR(presentInfo);
	vk::Result res = (vk::Result)vkQueuePresentKHR(*presentQueue.queue, &*presentInfo);
	switch (res) {
		case vk::Result::eSuccess: break;
		case vk::Result::eErrorOutOfDateKHR:
		case vk::Result::eSuboptimalKHR:
			vkDeviceWaitIdle(*nri.getDevice());
			uint32_t width, height;
			createSwapChain(width, height);
			break;
		case vk::Result::eErrorSurfaceLostKHR: break;
		default: assert(res == vk::Result::eSuccess);
	}

	presentQueue.queue.waitIdle();
}

void VulkanNRICommandBuffer::beginRendering(NRIImage2D &renderTarget) {
	auto &rt = static_cast<VulkanNRIImage2D &>(renderTarget);

	vk::RenderingInfo renderingInfo{};
	renderingInfo.sType		 = vk::StructureType::eRenderingInfo;
	renderingInfo.renderArea = vk::Rect2D({0, 0}, {rt.getWidth(), rt.getHeight()});
	vk::RenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType		= vk::StructureType::eRenderingAttachmentInfo;
	colorAttachment.imageView	= rt.getImageView();
	colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
	colorAttachment.loadOp		= vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp		= vk::AttachmentStoreOp::eStore;
	vk::ClearValue clearValue;
	clearValue.color				   = vk::ClearColorValue(std::array<float, 4>({0.0f, 0.0f, 0.0f, 1.0f}));
	colorAttachment.clearValue		   = clearValue;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments	   = &colorAttachment;
	renderingInfo.layerCount		   = 1;

	commandBuffer.beginRendering(renderingInfo);
}

void VulkanNRICommandBuffer::endRendering() { commandBuffer.endRendering(); }


VulkanNRIProgram::VulkanNRIProgram(std::vector<NRIProgram::ShaderInfo> &&stagesInfo, const vk::raii::Device &device) : 
	pipeline(nullptr), pipelineLayout(nullptr)
{
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	std::vector<vk::raii::ShaderModule>		 shaderModules;

	IDxcCompiler3 *compiler;
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

    IDxcUtils* utils;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));

	for (const auto &stageInfo : stagesInfo) {
		vk::ShaderStageFlagBits stage;
		switch (stageInfo.shaderType) {
			case NRI::ShaderType::SHADER_TYPE_VERTEX: stage = vk::ShaderStageFlagBits::eVertex; break;
			case NRI::ShaderType::SHADER_TYPE_FRAGMENT: stage = vk::ShaderStageFlagBits::eFragment; break;
			default: throw std::runtime_error("Unsupported shader stage!");
		}

		std::vector<uint8_t>		sourceCode;
		IDxcBlobEncoding		 *sourceBlob;
		std::wstring wSourceFile = std::wstring(stageInfo.sourceFile.begin(), stageInfo.sourceFile.end());
		HRESULT					 hr = utils->LoadFile(wSourceFile.c_str(), nullptr, &sourceBlob);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to load shader source file: " + std::string(stageInfo.sourceFile));
		}

		std::vector<LPCWSTR> arguments;
		arguments.push_back(L"-E");
		std::wstring entryPoint = std::wstring(stageInfo.entryPoint.begin(), stageInfo.entryPoint.end());
		arguments.push_back(entryPoint.c_str());
		arguments.push_back(L"-T");
		arguments.push_back(L"vs_6_0");
		
		DxcBuffer buffer{};
		buffer.Ptr = sourceBlob->GetBufferPointer();
    	buffer.Size = sourceBlob->GetBufferSize();
    	buffer.Encoding = 0;

		IDxcResult *result;
		hr = compiler->Compile(&buffer, arguments.data(), static_cast<UINT32>(arguments.size()), nullptr, IID_PPV_ARGS(&result));
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to compile shader: " + std::string(stageInfo.sourceFile));
		}
		IDxcBlob* spirvBlob;
		result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirvBlob), nullptr);

		vk::ShaderModuleCreateInfo shaderModuleInfo({}, spirvBlob->GetBufferSize(),
												   reinterpret_cast<const uint32_t *>(spirvBlob->GetBufferPointer()));
		shaderModules.emplace_back(device, shaderModuleInfo);

		vk::PipelineShaderStageCreateInfo shaderStageInfo(
			{}, stage, *shaderModules.back(), stageInfo.entryPoint.c_str());
		shaderStages.push_back(shaderStageInfo);
	}

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);


}

static VkSurfaceKHR createSurface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri);

#ifdef __linux__
	#include <X11/Xlib.h>
	#include <vulkan/vulkan_xlib.h>

VkSurfaceKHR createSurface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri) {
	auto *X11App = app.nativeInterface<QNativeInterface::QX11Application>();
	assert(X11App);

	static PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR =
		reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(nri->getInstance().getProcAddr("vkCreateXlibSurfaceKHR"));
	assert(vkCreateXlibSurfaceKHR != nullptr);

	VkXlibSurfaceCreateInfoKHR createInfo{};
	createInfo.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	createInfo.dpy	  = X11App->display();
	createInfo.window = static_cast<Window>(window.winId());

	VkSurfaceKHR surface;
	if (vkCreateXlibSurfaceKHR(*nri->getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan XCB surface!");
	}
	return surface;
}
#endif

#ifdef _WIN32
	#include <windows.h>
	#include <vulkan/vulkan_win32.h>
VkSurfaceKHR createSurface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri) {
	static PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR =
		reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(nri->getInstance().getProcAddr("vkCreateWin32SurfaceKHR"));
	assert(vkCreateWin32SurfaceKHR != nullptr);

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd	 = reinterpret_cast<HWND>(window.winId());

	VkSurfaceKHR surface;
	if (vkCreateWin32SurfaceKHR(*nri->getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan Win32 surface!");
	}
	return surface;
}
#endif

NRIQWindow *VulkanNRI::createQWidgetSurface(QApplication &app, std::unique_ptr<Renderer> &&renderer) {
	auto *window = new VulkanNRIQWindow(*this, std::move(renderer));

	vk::SurfaceKHR surface = createSurface(app, *window, this);
	window->getSurface()   = vk::raii::SurfaceKHR(instance, surface);

	vk::Bool32 presentSupport =
		physicalDevice.getSurfaceSupportKHR(queueFamilyIndices.graphicsFamily.value(), *window->getSurface());
	if (!presentSupport) { throw std::runtime_error("Selected GPU does not support presentation to the surface!"); }

	uint32_t width;
	uint32_t height;
	window->createSwapChain(width, height);

	vk::raii::Queue presentQueue = vk::raii::Queue(device, queueFamilyIndices.graphicsFamily.value(), 0);
	window->getPresentQueue()	 = std::move(presentQueue);

	window->getRenderer()->initialize(*window);
	return window;
}

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

std::unique_ptr<NRICommandBuffer> VulkanNRI::createCommandBuffer(const NRICommandPool &pool) {
	vk::CommandBufferAllocateInfo allocInfo(static_cast<const VulkanNRICommandPool &>(pool).commandPool,
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

void VulkanNRICommandQueue::synchronize() { queue.waitIdle(); }

static int __asd = []() {
	NRIFactory::getInstance().registerNRI("Vulkan", []() -> NRI * { return new VulkanNRI(); });
	return 0;
}();
