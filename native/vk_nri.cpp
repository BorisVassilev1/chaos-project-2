#include "vk_nri.hpp"
#include <qapplication.h>
#include <qguiapplication.h>
#include <qguiapplication_platform.h>
#include <QtWidgets>
#include <iostream>
#include <ostream>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "dxc_include_handler.hpp"
#include <vulkan/vulkan_core.h>
#ifdef _WIN32
// #pragma comment(lib, "dxcompiler.lib")
#else
	#include <dxc/dxcapi.h>
#endif

#include <fstream>
#include <format>
#include "nri.hpp"
#include "vulkan/vulkan.hpp"

static const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

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

std::pair<const char *, bool> extensions[] = {
	{"VK_KHR_surface", true},
#ifdef __linux__
	{"VK_KHR_xlib_surface", true},
	{"VK_KHR_wayland_surface", false},
#elif defined(_WIN32)
	{"VK_KHR_win32_surface", true},
#endif
	{"VK_EXT_debug_utils", false},
};

std::vector<const char *> checkExtensionSupport() {
	auto					  availableExtensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char *> availableExtensionNames;

	for (const auto &[extensionName, required] : extensions) {
		bool extensionFound = false;

		for (const auto &extensionProperties : availableExtensions) {
			if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound) {
			if (required) throw std::runtime_error("Required extension not found!: " + std::string(extensionName));
			else dbLog(dbg::LOG_WARNING, "Optional extension not found: ", extensionName);
		} else {
			availableExtensionNames.push_back(extensionName);
		}
	}
	return availableExtensionNames;
}

static VulkanNRI::QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice &device) {
	VulkanNRI::QueueFamilyIndices indices;

	auto queueFamilies = device.getQueueFamilyProperties();
	dbLog(dbg::LOG_DEBUG, "Found ", queueFamilies.size(), " queue families.");

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		// supports graphics, compute, and transfer
		if (queueFamily.queueFlags &
			vk::QueueFlagBits::eGraphics	 // &&
											 // queueFamily.queueFlags & vk::QueueFlagBits::eCompute &&
											 // queueFamily.queueFlags & vk::QueueFlagBits::eTransfer
		) {
			indices.graphicsFamily = i;
			dbLog(dbg::LOG_INFO, "Queue family ", i, " supports graphics, compute, and transfer.");
		}
		i++;
	}

	if (indices.graphicsFamily.has_value()) {
		dbLog(dbg::LOG_INFO, "Found graphics queue family index: ", indices.graphicsFamily.value());
	} else {
		dbLog(dbg::LOG_WARNING, "No suitable graphics queue family found.");
	}

	return indices;
}

static bool isDeviceSuitable(const vk::raii::PhysicalDevice &device) {
	VulkanNRI::QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.graphicsFamily.has_value();
}

static void printDeviceQueueFamiliesInfo(const vk::raii::PhysicalDevice &device) {
	auto queueFamilies = device.getQueueFamilyProperties();
	dbLog(dbg::LOG_DEBUG, "Device has ", queueFamilies.size(), " queue families:");
	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		dbLog(dbg::LOG_DEBUG, " Queue Family ", i, ":");
		dbLog(dbg::LOG_DEBUG, "  Queue Count: ", queueFamily.queueCount);
		dbLog(dbg::LOG_DEBUG, "  Queue Flags: ");
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) { dbLog(dbg::LOG_DEBUG, "   - Graphics"); }
		if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) { dbLog(dbg::LOG_DEBUG, "   - Compute"); }
		if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) { dbLog(dbg::LOG_DEBUG, "   - Transfer"); }
		if (queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding) {
			dbLog(dbg::LOG_DEBUG, "   - Sparse Binding");
		}
		if (queueFamily.queueFlags & vk::QueueFlagBits::eProtected) { dbLog(dbg::LOG_DEBUG, "   - Protected"); }
		if (queueFamily.queueFlags & vk::QueueFlagBits::eVideoDecodeKHR) { dbLog(dbg::LOG_DEBUG, "   - Video Decode"); }
		if (queueFamily.queueFlags & vk::QueueFlagBits::eVideoEncodeKHR) { dbLog(dbg::LOG_DEBUG, "   - Video Encode"); }
		if (queueFamily.queueFlags & vk::QueueFlagBits::eOpticalFlowNV) { dbLog(dbg::LOG_DEBUG, "   - Optical Flow"); }
		i++;
	}
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

	auto availableExtensions = checkExtensionSupport();

	createInfo.enabledExtensionCount   = availableExtensions.size();
	createInfo.ppEnabledExtensionNames = availableExtensions.data();

	instance = vk::raii::Instance(vk::raii::Context(), createInfo);
}

void VulkanNRI::pickPhysicalDevice() {
	auto physicalDevices = instance.enumeratePhysicalDevices();
	if (physicalDevices.size() == 0) { throw std::runtime_error("Failed to find GPUs with Vulkan support!"); }

	for (const auto &device : physicalDevices) {
		dbLog(dbg::LOG_INFO, "Found device: ", device.getProperties().deviceName);
		printDeviceQueueFamiliesInfo(device);
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			// break;
		}
	}

	// list acceleration structure properties
	vk::PhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructureProperties;
	{
		vk::PhysicalDeviceProperties2 deviceProperties2;
		deviceProperties2.pNext = &accelerationStructureProperties;
		vkGetPhysicalDeviceProperties2(*physicalDevice, &*deviceProperties2);
		dbLog(dbg::LOG_INFO, "Acceleration Structure Properties:");
		dbLog(dbg::LOG_INFO, " maxGeometryCount: ", accelerationStructureProperties.maxGeometryCount);
		dbLog(dbg::LOG_INFO, " maxInstanceCount: ", accelerationStructureProperties.maxInstanceCount);
		dbLog(dbg::LOG_INFO, " maxPrimitiveCount: ", accelerationStructureProperties.maxPrimitiveCount);
		dbLog(dbg::LOG_INFO, " maxPerStageDescriptorAccelerationStructures: ",
			  accelerationStructureProperties.maxPerStageDescriptorAccelerationStructures);
		dbLog(dbg::LOG_INFO, " maxPerStageDescriptorUpdateAfterBindAccelerationStructures: ",
			  accelerationStructureProperties.maxPerStageDescriptorUpdateAfterBindAccelerationStructures);
		dbLog(dbg::LOG_INFO, " maxDescriptorSetAccelerationStructures: ",
			  accelerationStructureProperties.maxDescriptorSetAccelerationStructures);
		dbLog(dbg::LOG_INFO, " maxDescriptorSetUpdateAfterBindAccelerationStructures: ",
			  accelerationStructureProperties.maxDescriptorSetUpdateAfterBindAccelerationStructures);
		dbLog(dbg::LOG_INFO, " minAccelerationStructureScratchOffsetAlignment: ",
			  accelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment);
	}

	queueFamilyIndices = findQueueFamilies(physicalDevice);
	if (physicalDevice == nullptr) { throw std::runtime_error("Failed to find a suitable GPU!"); }
}

static std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
	VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
	VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME};

void VulkanNRI::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	float					  prio = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo({}, indices.graphicsFamily.value(), 1, &prio);

	vk::PhysicalDeviceFeatures					 deviceFeatures{};
	vk::PhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures(
		VK_TRUE,	 // shaderInputAttachmentArrayDynamicIndexing
		VK_TRUE,	 // shaderUniformTexelBufferArrayDynamicIndexing
		VK_TRUE,	 // shaderStorageTexelBufferArrayDynamicIndexing
		VK_TRUE,	 // shaderUniformBufferArrayNonUniformIndexing
		VK_TRUE,	 // shaderStorageBufferArrayNonUniformIndexing
		VK_TRUE,	 // shaderSampledImageArrayNonUniformIndexing
		VK_TRUE,	 // shaderStorageImageArrayNonUniformIndexing
		VK_TRUE,	 // shaderInputAttachmentArrayNonUniformIndexing
		VK_TRUE,	 // shaderUniformTexelBufferArrayNonUniformIndexing
		VK_TRUE,	 // shaderStorageTexelBufferArrayNonUniformIndexing
		VK_TRUE,	 // descriptorBindingUniformBufferUpdateAfterBind
		VK_TRUE,	 // descriptorBindingStorageBufferUpdateAfterBind
		VK_TRUE,	 // descriptorBindingSampledImageUpdateAfterBind
		VK_TRUE,	 // descriptorBindingStorageImageUpdateAfterBind
		VK_TRUE,	 // descriptorBindingStorageTexelBufferUpdateAfterBind
		VK_TRUE,	 // descriptorBindingUniformTexelBufferUpdateAfterBind
		VK_TRUE,	 // descriptorBindingUpdateUnusedWhilePending
		VK_TRUE,	 // descriptorBindingPartiallyBound
		VK_TRUE,	 // descriptorBindingVariableDescriptorCount
		VK_TRUE,	 // runtimeDescriptorArray
		nullptr);
	vk::PhysicalDeviceDynamicRenderingFeatures		   dynamicRenderingFeature(VK_TRUE, &descriptorIndexingFeatures);
	vk::PhysicalDeviceBufferDeviceAddressFeatures	   bufferDeviceAddressFeature(VK_TRUE, VK_FALSE, VK_FALSE,
																				  &dynamicRenderingFeature);
	vk::PhysicalDeviceRayTracingPipelineFeaturesKHR	   rayTracingPipelineFeatures(VK_TRUE, VK_FALSE, VK_FALSE, VK_FALSE,
																				  VK_TRUE, &bufferDeviceAddressFeature);
	vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures(
		VK_TRUE, VK_FALSE, VK_FALSE, VK_FALSE, VK_TRUE, &rayTracingPipelineFeatures);

	vk::DeviceCreateInfo createInfo(
		{}, 1, &queueCreateInfo, enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0,
		enableValidationLayers ? validationLayers.data() : nullptr, static_cast<uint32_t>(deviceExtensions.size()),
		deviceExtensions.data(), &deviceFeatures, &accelerationStructureFeatures);

	device = vk::raii::Device(physicalDevice, createInfo);
}

VulkanNRI::VulkanNRI()
	: instance(nullptr),
	  physicalDevice(nullptr),
	  device(nullptr),
	  defaultCommandPool(nullptr),
	  descriptorAllocator(std::nullopt) {
	createInstance();
	pickPhysicalDevice();
	createLogicalDevice();

	auto dcp		   = createCommandPool();
	defaultCommandPool = std::move(static_cast<VulkanNRICommandPool &>(*dcp));
	descriptorAllocator.emplace(*this);

	dbLog(dbg::LOG_INFO, "VulkanNRI initialized with device: ", physicalDevice.getProperties().deviceName);
}

VulkanNRI::~VulkanNRI() {
	device.waitIdle();
	// VulkanMemoryCache::destroy();
}
VulkanDescriptorAllocator::VulkanDescriptorAllocator(VulkanNRI &nri)
	: nri(nri), pool(nullptr), descriptorSetLayout(nullptr), bigDescriptorSet(nullptr) {
	std::array<vk::DescriptorPoolSize, 4> poolSizes = {
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 500),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 500),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 500),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 500),
	};

	vk::DescriptorPoolCreateInfo poolInfo(
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind, 1000,
		poolSizes.size(), poolSizes.data());

	pool = vk::raii::DescriptorPool(nri.getDevice(), poolInfo);

	std::array<vk::DescriptorSetLayoutBinding, 4> samplerBindings = {
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 500, vk::ShaderStageFlagBits::eAll,
									   nullptr),
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageImage, 500, vk::ShaderStageFlagBits::eAll,
									   nullptr),
		vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eUniformBuffer, 500, vk::ShaderStageFlagBits::eAll,
									   nullptr),
		vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 500, vk::ShaderStageFlagBits::eAll,
									   nullptr),
	};

	vk::DescriptorSetLayoutCreateInfo layoutInfo(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool |
													 vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPoolEXT,
												 samplerBindings.size(), samplerBindings.data());

	const vk::DescriptorBindingFlags bindingFlags = vk::DescriptorBindingFlagBits::eUpdateAfterBind;

	const vk::DescriptorBindingFlags bindingFlagsArr[] = {bindingFlags, bindingFlags, bindingFlags, bindingFlags};

	vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo(samplerBindings.size(), bindingFlagsArr);
	layoutInfo.pNext = &bindingFlagsInfo;

	descriptorSetLayout = vk::raii::DescriptorSetLayout(nri.getDevice(), layoutInfo);
	vk::DescriptorSetAllocateInfo allocInfo(pool, 1, &*descriptorSetLayout);
	auto						  descriptorSets = nri.getDevice().allocateDescriptorSets(allocInfo);
	bigDescriptorSet							 = std::move(descriptorSets[0]);
}

NRIResourceHandle VulkanDescriptorAllocator::addUniformBufferDescriptor(VulkanNRIBuffer &buffer) {
	uint32_t descriptorIndex = currentBufferDescriptorIndex++;

	vk::DescriptorBufferInfo bufferInfo(buffer.getBuffer(), 0, VK_WHOLE_SIZE);

	vk::WriteDescriptorSet descriptorWrite(bigDescriptorSet, descriptorIndex, 0, 1, vk::DescriptorType::eUniformBuffer,
										   nullptr, &bufferInfo, nullptr);

	(*nri.getDevice()).updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

	return NRIResourceHandle(NRIResourceHandle::ResourceType::RESOURCE_TYPE_UNIFORM_BUFFER, false, descriptorIndex);
}

NRIResourceHandle VulkanDescriptorAllocator::addSamplerImageDescriptor(VulkanNRIImage2D &image) {
	uint32_t descriptorIndex = currentImageDescriptorIndex++;

	vk::DescriptorImageInfo imageInfo(*(image.getSampler()), *(image.getImageView()),
									  vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::WriteDescriptorSet descriptorWrite(bigDescriptorSet, 0, descriptorIndex, 1,
										   vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr, nullptr);

	(*nri.getDevice()).updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

	return NRIResourceHandle(NRIResourceHandle::ResourceType::RESOURCE_TYPE_IMAGE_SAMPLER, false, descriptorIndex);
}

vk::MemoryPropertyFlags typeRequest2vkMemoryProperty[] = {
	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	vk::MemoryPropertyFlagBits::eDeviceLocal};

VulkanNRIAllocation::VulkanNRIAllocation(VulkanNRI &nri, NRI::MemoryRequirements memoryRequirements)
	: memory(nullptr), device(nri.getDevice()) {
	assert(memoryRequirements.typeRequest >= 0);
	assert(memoryRequirements.typeRequest < NRI::MemoryTypeRequest::_MEMORY_TYPE_NUM);
	vk::MemoryPropertyFlags properties = typeRequest2vkMemoryProperty[(uint32_t)memoryRequirements.typeRequest];

	vk::PhysicalDeviceMemoryProperties memProperties   = nri.getPhysicalDevice().getMemoryProperties();
	uint32_t						   memoryTypeIndex = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			memoryTypeIndex = i;
			break;
		}
	}

	if (memoryTypeIndex == -1u) { throw std::runtime_error("Failed to find suitable memory type!"); }

	vk::MemoryAllocateFlagsInfo allocFlagsInfo(vk::MemoryAllocateFlagBits::eDeviceAddress, {});
	vk::MemoryAllocateInfo		allocInfo(memoryRequirements.size, memoryTypeIndex, &allocFlagsInfo);

	memory = vk::raii::DeviceMemory(nri.getDevice(), allocInfo);
}

VulkanNRIBuffer::VulkanNRIBuffer(VulkanNRI &nri, std::size_t size, NRI::BufferUsage usage)
	: nri(&nri), buffer(nullptr), allocation(nullptr), offset(0), size(size) {
	vk::BufferUsageFlags bufferUsageFlags;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_VERTEX)
		bufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress |
							vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_INDEX)
		bufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress |
							vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_UNIFORM) bufferUsageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_STORAGE)
		bufferUsageFlags |= vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress |
							vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC) bufferUsageFlags |= vk::BufferUsageFlagBits::eTransferSrc;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST)
		bufferUsageFlags |= vk::BufferUsageFlagBits::eTransferDst |
							vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
	if (usage & NRI::BufferUsage::BUFFER_USAGE_ACCELERATION_STRUCTURE)
		bufferUsageFlags |=
			vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR | vk::BufferUsageFlagBits::eShaderDeviceAddress;

	vk::BufferCreateInfo bufferInfo({}, size, bufferUsageFlags, vk::SharingMode::eExclusive);

	this->buffer = vk::raii::Buffer(nri.getDevice(), bufferInfo);
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
	void *data = nullptr;
	vkMapMemory(allocation->getDevice(), allocation->getMemory(), offset, size, 0, &data);
	if (data == nullptr) { throw std::runtime_error("Failed to map buffer memory."); }
	return data;
}

void VulkanNRIBuffer::unmap() {
	assert(allocation != nullptr);

	// vk::MappedMemoryRange memoryRange(allocation->getMemory(), offset, VK_WHOLE_SIZE);
	// auto				  res = allocation->getDevice().flushMappedMemoryRanges(1, &memoryRange);
	// assert(res == vk::Result::eSuccess);

	vkUnmapMemory(allocation->getDevice(), allocation->getMemory());
}

std::size_t VulkanNRIBuffer::getSize() const { return this->size; }

std::size_t VulkanNRIBuffer::getOffset() const { return this->offset; }

void VulkanNRIBuffer::copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
							   std::size_t dstOffset, std::size_t size) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	auto &vkSrcBuf = static_cast<VulkanNRIBuffer &>(srcBuffer);

	if (!vkCmdBuf.isRecording) vkCmdBuf.begin();

	// TODO: put the right barriers here
	// vk::BufferMemoryBarrier bufferBarrier(vk::AccessFlagBits::eHostWrite,
	//									  vk::AccessFlagBits::eTransferRead, VK_QUEUE_FAMILY_IGNORED,
	//									  VK_QUEUE_FAMILY_IGNORED, vkSrcBuf.buffer, srcOffset, size);
	// vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost,
	//									   vk::PipelineStageFlagBits::eTransfer, {}, {}, {bufferBarrier}, {});

	vkCmdBuf.commandBuffer.copyBuffer(vkSrcBuf.buffer, this->buffer, vk::BufferCopy(srcOffset, dstOffset, size));

	vk::BufferMemoryBarrier bufferBarrier2(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eMemoryRead,
										   VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, this->buffer, dstOffset,
										   size);
	vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTopOfPipe,
										   {}, {}, {bufferBarrier2}, {});
}

void VulkanNRIBuffer::bindAsVertexBuffer(NRICommandBuffer &commandBuffer, uint32_t binding, std::size_t offset,
										 std::size_t stride) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	static_cast<void>(stride);

	vk::Buffer vkBuffer = this->buffer;

	vkCmdBuf.commandBuffer.bindVertexBuffers(binding, vkBuffer, vk::DeviceSize(offset));
}

vk::IndexType nriIndexType2vkIndexType[] = {
	vk::IndexType::eUint16,
	vk::IndexType::eUint32,
};

void VulkanNRIBuffer::bindAsIndexBuffer(NRICommandBuffer &commandBuffer, std::size_t offset, NRI::IndexType indexType) {
	auto		 &vkCmdBuf	  = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	vk::Buffer	  vkBuffer	  = this->buffer;
	vk::IndexType vkIndexType = nriIndexType2vkIndexType[static_cast<int>(indexType)];
	assert(int(vkIndexType) != -1);
	vkCmdBuf.commandBuffer.bindIndexBuffer(vkBuffer, vk::DeviceSize(offset), vkIndexType);
}

vk::DeviceAddress VulkanNRIBuffer::getAddress() {
	vk::BufferDeviceAddressInfo addressInfo;
	addressInfo.setBuffer(buffer);
	return nri->getDevice().getBufferAddress(addressInfo);
}

NRI::MemoryRequirements &NRI::MemoryRequirements::setTypeRequest(MemoryTypeRequest tr) {
	typeRequest = tr;
	return *this;
}

std::unique_ptr<NRIImage2D> VulkanNRI::createImage2D(uint32_t width, uint32_t height, NRI::Format format,
													 NRI::ImageUsage usage) {
	return std::make_unique<VulkanNRIImage2D>(*this, width, height, format, usage);
}

std::unique_ptr<NRIBuffer> VulkanNRI::createBuffer(std::size_t size, NRI::BufferUsage usage) {
	return std::make_unique<VulkanNRIBuffer>(*this, size, usage);
}

NRI::MemoryRequirements VulkanNRIImage2D::getMemoryRequirements() {
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(**device, image.get(), &memRequirements);

	return NRI::MemoryRequirements(memRequirements.size, NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE,
								   memRequirements.alignment);
}

void VulkanNRIImage2D::bindMemory(NRIAllocation &allocation, std::size_t offset) {
	VulkanNRIAllocation &vulkanAllocation = static_cast<VulkanNRIAllocation &>(allocation);

	vkBindImageMemory(**device, image.get(), vulkanAllocation.getMemory(), offset);

	vk::ImageViewCreateInfo imageViewInfo(
		{}, image.get(), vk::ImageViewType::e2D, vk::Format(this->format),
		vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
							 vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity),
		vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1));

	this->imageView = vk::raii::ImageView(*device, imageViewInfo);

	vk::SamplerCreateInfo samplerInfo({}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
									  vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
									  vk::SamplerAddressMode::eRepeat, 0.0f, VK_FALSE, 1.0f, VK_FALSE,
									  vk::CompareOp::eAlways, 0.0f, 0.0f, vk::BorderColor::eIntOpaqueBlack, VK_FALSE);
	this->sampler = vk::raii::Sampler(*device, samplerInfo);
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

void VulkanNRIImage2D::copyFrom(NRICommandBuffer &commandBuffer, NRIBuffer &srcBuffer, std::size_t srcOffset,
								uint32_t srcRowPitch) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	auto &vkSrcBuf = static_cast<VulkanNRIBuffer &>(srcBuffer);
	if (!vkCmdBuf.isRecording) vkCmdBuf.begin();
	transitionLayout(vkCmdBuf, vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits::eNone,
					 vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTopOfPipe,
					 vk::PipelineStageFlagBits::eTransfer);
	vk::BufferImageCopy region(srcOffset, srcRowPitch, 0, vk::ImageSubresourceLayers(aspectFlags, 0, 0, 1),
							   vk::Offset3D(0, 0, 0), vk::Extent3D(width, height, 1));
	vkCmdBuf.commandBuffer.copyBufferToImage(vkSrcBuf.getBuffer(), image.get(), vk::ImageLayout::eTransferDstOptimal,
											 region);
}

NRIResourceHandle VulkanNRIImage2D::getImageViewHandle() {
	if (imageViewHandle == NRIResourceHandle::INVALID_HANDLE)
		imageViewHandle =
			nri->getDescriptorAllocator().addSamplerImageDescriptor(*const_cast<VulkanNRIImage2D *>(this));
	assert(imageViewHandle != NRIResourceHandle::INVALID_HANDLE);
	return imageViewHandle;
}

vk::ImageAspectFlags VulkanNRIImage2D::getAspectFlags(vk::Format format) {
	switch (format) {
		case vk::Format::eUndefined: throw std::runtime_error("Undefined format has no aspect flags.");
		default: break;
		case vk::Format::eD32SfloatS8Uint:
		case vk::Format::eD24UnormS8Uint:
		case vk::Format::eD16UnormS8Uint: return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil; ;
		case vk::Format::eD32Sfloat:
		case vk::Format::eD16Unorm: return vk::ImageAspectFlagBits::eDepth;
	}
	return vk::ImageAspectFlagBits::eColor;
}

void VulkanNRIImage2D::transitionLayout(NRICommandBuffer &commandBuffer, vk::ImageLayout newLayout,
										vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
										vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage) {
	auto &vkBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	vk::ImageMemoryBarrier barrier(srcAccess, dstAccess, layout, newLayout, vk::QueueFamilyIgnored,
								   vk::QueueFamilyIgnored, image.get(), {aspectFlags, 0, 1, 0, 1});
	vkBuf.begin();
	vkBuf.commandBuffer.pipelineBarrier(srcStage, dstStage, vk::DependencyFlagBits::eByRegion, {}, {}, {barrier});
}

VulkanNRIImage2D::VulkanNRIImage2D(VulkanNRI &nri, uint32_t width, uint32_t height, NRI::Format format,
								   NRI::ImageUsage usage)
	: nri(&nri),
	  image(nullptr),
	  device(&nri.getDevice()),
	  layout(vk::ImageLayout::eUndefined),
	  format((vk::Format)nriFormat2vkFormat[static_cast<int>(format)]),
	  imageView(nullptr),
	  aspectFlags(getAspectFlags(this->format)),
	  sampler(nullptr),
	  width(width),
	  height(height) {
	assert(format != NRI::Format::FORMAT_UNDEFINED);
	assert(format < NRI::Format::_FORMAT_NUM);
	assert(this->format != vk::Format::eUndefined);
	assert(int(this->format) != -1);
	assert(width > 0 && height > 0);
	vk::Format vkFormat = (vk::Format)nriFormat2vkFormat[static_cast<int>(format)];

	vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, vk::Format(vkFormat),
								  vk::Extent3D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1), 1, 1,
								  vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
								  static_cast<vk::ImageUsageFlagBits>(usage), vk::SharingMode::eExclusive, 0, nullptr,
								  vk::ImageLayout::eUndefined);

	this->image = vk::raii::Image(nri.getDevice(), imageInfo);
}

std::unique_ptr<NRICommandPool> VulkanNRI::createCommandPool() {
	vk::CommandPoolCreateInfo poolCI(vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
									 queueFamilyIndices.graphicsFamily.value());

	vk::raii::CommandPool pool = vk::raii::CommandPool(device, poolCI);

	return std::make_unique<VulkanNRICommandPool>(std::move(pool));
}

std::unique_ptr<NRIProgramBuilder> VulkanNRI::createProgramBuilder() {
	return std::make_unique<VulkanNRIProgramBuilder>(*this);
}

std::unique_ptr<NRICommandQueue> VulkanNRI::createCommandQueue() {
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
	  inFlightFence(nri.getDevice(), vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)),
	  depthImage(std::nullopt) {
	if (imageAvailableSemaphore == nullptr || renderFinishedSemaphore == nullptr || inFlightFence == nullptr) {
		throw std::runtime_error("Failed to create synchronization objects for a frame!");
	}
	commandBuffer = std::unique_ptr<VulkanNRICommandBuffer>(
		(VulkanNRICommandBuffer *)nri.createCommandBuffer(nri.getDefaultCommandPool()).release());
}

void VulkanNRIQWindow::createSwapChain(uint32_t &width, uint32_t &height) {
	this->width								= width;
	this->height							= height;
	auto					  &nri			= static_cast<VulkanNRI &>(this->nri);
	vk::SurfaceCapabilitiesKHR capabilities = nri.getPhysicalDevice().getSurfaceCapabilitiesKHR(*surface);

	width  = capabilities.currentExtent.width;
	width  = std::min(capabilities.maxImageExtent.width, width);
	height = capabilities.currentExtent.height;
	height = std::min(capabilities.maxImageExtent.height, height);

	dbLog(dbg::LOG_INFO, "Creating swap chain with size: ", width, "x", height);

	vk::SwapchainCreateInfoKHR swapChainInfo(
		{}, *surface, capabilities.minImageCount + 1, vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear,
		{width, height}, 1, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
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

	this->swapChainImages.clear();
	vk::SwapchainKHR oldswapchain = swapChain.release();
	vkDestroySwapchainKHR(*nri.getDevice(), oldswapchain, nullptr);
	swapChain			 = vk::raii::SwapchainKHR(nri.getDevice(), _swapChain);
	auto swapChainImages = swapChain.getImages();
	for (const auto &image : swapChainImages) {
		vk::ImageViewCreateInfo imageViewInfo(
			{}, image, vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Unorm,
			vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
								 vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		vk::raii::ImageView imageView = vk::raii::ImageView(nri.getDevice(), imageViewInfo);

		VulkanNRIImage2D nriImage =
			VulkanNRIImage2D(nri, image, vk::ImageLayout::eUndefined, vk::Format::eB8G8R8A8Unorm, nri.getDevice(),
							 std::move(imageView), width, height);
		this->swapChainImages.push_back(std::move(nriImage));
		this->swapChainImages.back().transitionLayout(
			*commandBuffer, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eMemoryRead,
			vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe);
	}

	depthImage			 = VulkanNRIImage2D(nri, width, height, NRI::Format::FORMAT_D32_SFLOAT,
											NRI::ImageUsage::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT);
	depthImageAllocation = VulkanNRIAllocation(
		nri, depthImage->getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	depthImage->bindMemory(*depthImageAllocation, 0);

	depthImage->transitionLayout(
		*commandBuffer, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits::eNone,
		vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits::eTopOfPipe,
		vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests);
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

void VulkanNRIQWindow::beginRendering(NRICommandBuffer &cmdBuf, NRIImage2D &renderTarget) {
	auto &rt	   = static_cast<VulkanNRIImage2D &>(renderTarget);
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(cmdBuf);

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

	vk::RenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType		= vk::StructureType::eRenderingAttachmentInfo;
	depthAttachment.imageView	= depthImage->getImageView();
	depthAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	depthAttachment.loadOp		= vk::AttachmentLoadOp::eClear;
	depthAttachment.storeOp		= vk::AttachmentStoreOp::eDontCare;
	vk::ClearValue depthClearValue;
	depthClearValue.depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
	depthAttachment.clearValue	 = depthClearValue;

	renderingInfo.pDepthAttachment = &depthAttachment;

	vkCmdBuf.commandBuffer.beginRendering(renderingInfo);

	vkCmdBuf.commandBuffer.setViewport(
		0, vk::Viewport(0.0f, 0.0f, static_cast<float>(rt.getWidth()), static_cast<float>(rt.getHeight()), 0.0f, 1.0f));
	vkCmdBuf.commandBuffer.setScissor(0, vk::Rect2D({0, 0}, {rt.getWidth(), rt.getHeight()}));
}

void VulkanNRIQWindow::endRendering(NRICommandBuffer &cmdBuf) {
	static_cast<VulkanNRICommandBuffer &>(cmdBuf).commandBuffer.endRendering();
}

vk::PrimitiveTopology nriPrimitiveType2vkTopology[] = {
	vk::PrimitiveTopology::eTriangleList, vk::PrimitiveTopology::eTriangleStrip, vk::PrimitiveTopology::eLineList,
	vk::PrimitiveTopology::eLineStrip,	  vk::PrimitiveTopology::ePointList,
};

std::pair<std::vector<vk::raii::ShaderModule>, std::vector<vk::PipelineShaderStageCreateInfo>> VulkanNRIProgramBuilder::
	createShaderModules(std::vector<NRI::ShaderCreateInfo> &&stagesInfo, const vk::raii::Device &device) {
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	std::vector<vk::raii::ShaderModule>			   shaderModules;

	CComPtr<IDxcCompiler3> compiler;
	HRESULT				   hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	assert(SUCCEEDED(hr) && "Failed to create DX Compiler.");

	CComPtr<IDxcUtils> utils;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
	assert(SUCCEEDED(hr) && "Failed to create DX Utils.");

	auto includeHandler = std::make_unique<CustomIncludeHandler>(utils);

	for (const auto &stageInfo : stagesInfo) {
		vk::ShaderStageFlagBits stage;
		switch (stageInfo.shaderType) {
			case NRI::ShaderType::SHADER_TYPE_VERTEX: stage = vk::ShaderStageFlagBits::eVertex; break;
			case NRI::ShaderType::SHADER_TYPE_FRAGMENT: stage = vk::ShaderStageFlagBits::eFragment; break;
			case NRI::ShaderType::SHADER_TYPE_COMPUTE: stage = vk::ShaderStageFlagBits::eCompute; break;
			case NRI::ShaderType::SHADER_TYPE_RAYGEN: stage = vk::ShaderStageFlagBits::eRaygenKHR; break;
			case NRI::ShaderType::SHADER_TYPE_CLOSEST_HIT: stage = vk::ShaderStageFlagBits::eClosestHitKHR; break;
			case NRI::ShaderType::SHADER_TYPE_ANY_HIT: stage = vk::ShaderStageFlagBits::eAnyHitKHR; break;
			case NRI::ShaderType::SHADER_TYPE_MISS: stage = vk::ShaderStageFlagBits::eMissKHR; break;
			default:
				dbLog(dbg::LOG_ERROR, "Unsupported shader type: ", static_cast<int>(stageInfo.shaderType));
				throw std::runtime_error("Unsupported shader stage!");
		}
		const std::filesystem::path sourceFile{stageInfo.sourceFile};

		std::ifstream fin(sourceFile);
		std::string	  source{(std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>()};

		CComPtr<IDxcBlobEncoding> sourceBlob;
		std::wstring			  wSourceFile = std::wstring(stageInfo.sourceFile.begin(), stageInfo.sourceFile.end());
		HRESULT					  hr		  = utils->LoadFile(wSourceFile.c_str(), nullptr, &sourceBlob);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to load shader source file: " + std::string(stageInfo.sourceFile));
		}

		std::vector<LPCWSTR> arguments;
		arguments.push_back(L"-E");
		std::wstring entryPoint = std::wstring(stageInfo.entryPoint.begin(), stageInfo.entryPoint.end());
		arguments.push_back(entryPoint.c_str());
		arguments.push_back(L"-T");
		switch (stageInfo.shaderType) {
			case NRI::ShaderType::SHADER_TYPE_VERTEX: arguments.push_back(L"vs_6_0"); break;
			case NRI::ShaderType::SHADER_TYPE_FRAGMENT: arguments.push_back(L"ps_6_0"); break;
			case NRI::ShaderType::SHADER_TYPE_COMPUTE: arguments.push_back(L"cs_6_0"); break;
			case NRI::ShaderType::SHADER_TYPE_RAYGEN: arguments.push_back(L"lib_6_3"); break;
			case NRI::ShaderType::SHADER_TYPE_CLOSEST_HIT: arguments.push_back(L"lib_6_3"); break;
			case NRI::ShaderType::SHADER_TYPE_ANY_HIT: arguments.push_back(L"lib_6_3"); break;
			case NRI::ShaderType::SHADER_TYPE_MISS: arguments.push_back(L"lib_6_3"); break;
			default:
				dbLog(dbg::LOG_ERROR, "Unsupported shader type: ", static_cast<int>(stageInfo.shaderType));
				throw std::runtime_error("Unsupported shader stage!");
		}
		arguments.push_back(L"-spirv");
		arguments.push_back(L"-D");
		arguments.push_back(L"VULKAN");
		arguments.push_back(L"-I");
		arguments.push_back(PROJECT_ROOT_DIR L"/shaders/");
		arguments.push_back(L"-fvk-use-dx-layout");
		arguments.push_back(L"-fspv-target-env=vulkan1.2");
		arguments.push_back(L"-HV");
		arguments.push_back(L"2021");

		DxcBuffer buffer{};
		buffer.Ptr		= sourceBlob->GetBufferPointer();
		buffer.Size		= sourceBlob->GetBufferSize();
		buffer.Encoding = 0;

		dbLog(dbg::LOG_DEBUG, "\n\tCompiling shader: ", stageInfo.sourceFile,
			  "\n\tEntry point: ", stageInfo.entryPoint);

		includeHandler->reset();
		CComPtr<IDxcResult> result;
		hr = compiler->Compile(&buffer, arguments.data(), static_cast<UINT32>(arguments.size()), includeHandler.get(),
							   IID_PPV_ARGS(&result));
		if (FAILED(hr)) { throw std::runtime_error("Failed to compile shader: " + std::string(stageInfo.sourceFile)); }

		CComPtr<IDxcBlobEncoding> errorBuffer;
		result->GetErrorBuffer(&errorBuffer);
		if (errorBuffer != nullptr && errorBuffer->GetBufferSize() > 0) {
			std::string errorMessage(reinterpret_cast<const char *>(errorBuffer->GetBufferPointer()),
									 errorBuffer->GetBufferSize());
			std::cerr << "Shader compilation warnings/errors: " << errorMessage << std::endl;
		}

		CComPtr<IDxcBlob> spirvBlob;
		result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirvBlob), nullptr);

		vk::ShaderModuleCreateInfo shaderModuleInfo({}, spirvBlob->GetBufferSize(),
													reinterpret_cast<const uint32_t *>(spirvBlob->GetBufferPointer()));
		shaderModules.emplace_back(device, shaderModuleInfo);

		vk::PipelineShaderStageCreateInfo shaderStageInfo({}, stage, *shaderModules.back(),
														  stageInfo.entryPoint.c_str());
		shaderStages.push_back(shaderStageInfo);
	}
	return {std::move(shaderModules), std::move(shaderStages)};
}

std::vector<vk::PushConstantRange> VulkanNRIProgramBuilder::createPushConstantRanges(
	const std::vector<NRI::PushConstantRange> &nriPushConstantRanges) {
	std::vector<vk::PushConstantRange> vkPushConstantRanges;
	for (const auto &nriPushConstantRange : nriPushConstantRanges) {
		vkPushConstantRanges.emplace_back(vk::ShaderStageFlagBits::eAll, nriPushConstantRange.offset,
										  nriPushConstantRange.size);
	}
	return vkPushConstantRanges;
}

vk::VertexInputRate nriInputRate2vkInputRate[] = {
	vk::VertexInputRate::eVertex,
	vk::VertexInputRate::eInstance,
};

std::unique_ptr<NRIGraphicsProgram> VulkanNRIProgramBuilder::buildGraphicsProgram() {
	auto [shaderModules, shaderStages] = createShaderModules(std::move(shaderStagesInfo), nri.getDevice());

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	auto						 pushConstantRanges = createPushConstantRanges(this->pushConstantRanges);
	pipelineLayoutInfo.setPushConstantRangeCount(pushConstantRanges.size());
	pipelineLayoutInfo.setPPushConstantRanges(pushConstantRanges.data());
	pipelineLayoutInfo.setSetLayoutCount(1);
	pipelineLayoutInfo.setPSetLayouts(&*nri.getDescriptorAllocator().getDescriptorSetLayout());
	auto pipelineLayout = vk::raii::PipelineLayout(nri.getDevice(), pipelineLayoutInfo);

	std::vector<vk::VertexInputBindingDescription>	 bindingDescriptions;
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
	for (const auto &binding : this->vertexBindings) {
		vk::VertexInputRate inputRate = nriInputRate2vkInputRate[static_cast<int>(binding.inputRate)];
		assert(int(inputRate) != -1);
		vk::VertexInputBindingDescription bindingDescription(binding.binding, binding.stride, inputRate);
		bindingDescriptions.push_back(bindingDescription);

		for (const auto &attribute : binding.attributes) {
			vk::VertexInputAttributeDescription attributeDescription(attribute.location, binding.binding,
																	 vk::Format(attribute.format), attribute.offset);
			attributeDescriptions.push_back(attributeDescription);
		}
	}

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, bindingDescriptions.size(), bindingDescriptions.data(),
														   attributeDescriptions.size(), attributeDescriptions.data());

	vk::PrimitiveTopology vkTopology = nriPrimitiveType2vkTopology[static_cast<int>(primitiveType)];
	assert(int(vkTopology) != -1);
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, vkTopology, VK_FALSE);

	vk::Viewport						viewport(0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f);
	vk::Rect2D							scissor({0, 0}, {800, 600});
	vk::PipelineViewportStateCreateInfo viewportStateInfo({}, 1, &viewport, 1, &scissor);

	vk::PipelineRasterizationStateCreateInfo rasterizerInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
															vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise,
															VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);

	vk::PipelineMultisampleStateCreateInfo multisampleInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr,
														   VK_FALSE, VK_FALSE);

	vk::PipelineColorBlendAttachmentState colorBlendAttachment(
		VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eOne,
		vk::BlendFactor::eZero, vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo colorBlending({}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment,
														{0.0f, 0.0f, 0.0f, 0.0f});

	std::array<vk::DynamicState, 3>	   dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor,
														vk::DynamicState::eBlendConstants};
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates.size(), dynamicStates.data());

	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo({}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess, VK_FALSE,
															 VK_FALSE, vk::StencilOpState(), vk::StencilOpState(), 0.0f,
															 1.0f);

	vk::Format						colorFormat = vk::Format::eB8G8R8A8Unorm;
	vk::PipelineRenderingCreateInfo pipelineRenderingInfo(0, 1, &colorFormat, vk::Format::eD32Sfloat);

	vk::GraphicsPipelineCreateInfo pipelineInfo(
		{}, static_cast<uint32_t>(shaderStages.size()), shaderStages.data(), &vertexInputInfo, &inputAssemblyInfo,
		nullptr,	 // tessellation
		&viewportStateInfo, &rasterizerInfo, &multisampleInfo, &depthStencilInfo, &colorBlending, &dynamicStateInfo,
		*pipelineLayout, vk::RenderPass(nullptr), 0, nullptr, -1, &pipelineRenderingInfo);

	auto pipelines = nri.getDevice().createGraphicsPipelines(nullptr, {pipelineInfo});
	return std::make_unique<VulkanNRIGraphicsProgram>(nri, std::move(pipelines[0]), std::move(pipelineLayout));
}

std::unique_ptr<NRIComputeProgram> VulkanNRIProgramBuilder::buildComputeProgram() {
	auto [shaderModules, shaderStages] = createShaderModules(std::move(shaderStagesInfo), nri.getDevice());
	assert(shaderStages.size() == 1 && "Compute program must have exactly one shader stage.");

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	auto						 pipelineLayout = vk::raii::PipelineLayout(nri.getDevice(), pipelineLayoutInfo);

	vk::ComputePipelineCreateInfo pipelineInfo({}, shaderStages[0], *pipelineLayout);

	auto pipelines = nri.getDevice().createComputePipelines(nullptr, {pipelineInfo});
	return std::make_unique<VulkanNRIComputeProgram>(nri, std::move(pipelines[0]), std::move(pipelineLayout));
}

static auto findShaderStage(const std::vector<vk::PipelineShaderStageCreateInfo> &shaderStages,
							vk::ShaderStageFlagBits								  stage) {
	return std::find_if(shaderStages.begin(), shaderStages.end(),
						[stage](const vk::PipelineShaderStageCreateInfo &s) { return s.stage == stage; });
}

std::unique_ptr<NRIRayTracingProgram> VulkanNRIProgramBuilder::buildRayTracingProgram() {
	auto [shaderModules, shaderStages] = createShaderModules(std::move(shaderStagesInfo), nri.getDevice());

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	auto						 pushConstantRanges = createPushConstantRanges(this->pushConstantRanges);
	pipelineLayoutInfo.setPushConstantRangeCount(pushConstantRanges.size());
	pipelineLayoutInfo.setPPushConstantRanges(pushConstantRanges.data());
	pipelineLayoutInfo.setSetLayoutCount(1);
	pipelineLayoutInfo.setPSetLayouts(&*nri.getDescriptorAllocator().getDescriptorSetLayout());
	auto pipelineLayout = vk::raii::PipelineLayout(nri.getDevice(), pipelineLayoutInfo);

	// find ray generation shader stage
	auto raygenit	  = findShaderStage(shaderStages, vk::ShaderStageFlagBits::eRaygenKHR);
	auto closesthitit = findShaderStage(shaderStages, vk::ShaderStageFlagBits::eClosestHitKHR);
	auto anyhitit	  = findShaderStage(shaderStages, vk::ShaderStageFlagBits::eAnyHitKHR);
	auto missit		  = findShaderStage(shaderStages, vk::ShaderStageFlagBits::eMissKHR);

	auto raygen		= raygenit != shaderStages.end() ? raygenit - shaderStages.begin() : VK_SHADER_UNUSED_KHR;
	auto closesthit = closesthitit != shaderStages.end() ? closesthitit - shaderStages.begin() : VK_SHADER_UNUSED_KHR;
	auto anyhit		= anyhitit != shaderStages.end() ? anyhitit - shaderStages.begin() : VK_SHADER_UNUSED_KHR;
	auto miss		= missit != shaderStages.end() ? missit - shaderStages.begin() : VK_SHADER_UNUSED_KHR;

	if (raygen == VK_SHADER_UNUSED_KHR) dbLog(dbg::LOG_ERROR, "No ray generation shader found in ray tracing program!");
	if (miss != VK_SHADER_UNUSED_KHR) dbLog(dbg::LOG_ERROR, "Miss shaders not currently supported in VulkanNRI");

	std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups;
	shaderGroups.push_back(vk::RayTracingShaderGroupCreateInfoKHR(vk::RayTracingShaderGroupTypeKHR::eGeneral, raygen,
																  closesthit, anyhit, VK_SHADER_UNUSED_KHR));

	vk::RayTracingPipelineCreateInfoKHR pipelineInfo({}, static_cast<uint32_t>(shaderStages.size()),
													 shaderStages.data(), shaderGroups.size(), shaderGroups.data(),
													 1	   // no recursion
													 ,
													 {}, {}, {}, *pipelineLayout, {}, {});

	static PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR =
		reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(
			nri.getInstance().getProcAddr("vkCreateRayTracingPipelinesKHR"));
	assert(vkCreateRayTracingPipelinesKHR != nullptr);

	auto pipelines = nri.getDevice().createRayTracingPipelinesKHR(nullptr, nullptr, pipelineInfo, nullptr);
	if (!pipelines.size()) dbLog(dbg::LOG_ERROR, "Failed to create ray tracing pipeline!");

	return std::make_unique<VulkanNRIRayTracingProgram>(nri, std::move(pipelines[0]), std::move(pipelineLayout));
}

void VulkanNRIRayTracingProgram::traceRays(NRICommandBuffer &commandBuffer, uint32_t width, uint32_t height,
										   uint32_t depth) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	vkCmdBuf.begin();
	// vkCmdBuf.commandBuffer.traceRaysKHR(_, _, _, _, width, height, depth);
};

void VulkanNRIProgram::bind(NRICommandBuffer &commandBuffer) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	vkCmdBuf.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);	  // TODO: BAD

	auto &descriptorSet = static_cast<VulkanNRI &>(nri).getDescriptorAllocator().getDescriptorSet();
	vkCmdBuf.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelineLayout, 0, *descriptorSet, {});
}

void VulkanNRIProgram::unbind(NRICommandBuffer &commandBuffer) {
	static_cast<void>(commandBuffer);
	// No unbind in Vulkan
}

void VulkanNRIProgram::setPushConstants(NRICommandBuffer &commandBuffer, const void *data, std::size_t size,
										std::size_t offset) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	vkCmdPushConstants(*vkCmdBuf.commandBuffer, *pipelineLayout, (VkShaderStageFlags)vk::ShaderStageFlagBits::eAll,
					   offset, size, data);
}

void VulkanNRIGraphicsProgram::draw(NRICommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
									uint32_t firstVertex, uint32_t firstInstance) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	vkCmdBuf.commandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanNRIGraphicsProgram::drawIndexed(NRICommandBuffer &commandBuffer, uint32_t indexCount, uint32_t instanceCount,
										   uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	vkCmdBuf.commandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanNRIComputeProgram::dispatch(NRICommandBuffer &commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
									   uint32_t groupCountZ) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	vkCmdBuf.commandBuffer.dispatch(groupCountX, groupCountY, groupCountZ);
}

void VulkanMemoryCache::assureSize(VulkanNRI &nri, std::size_t size) {
	if (size <= buffer.getSize()) return;
	dbLog(dbg::LOG_DEBUG, "Resizing acceleration structure scratch buffer to ", size, " bytes.");
	buffer	   = VulkanNRIBuffer(nri, size,
								 NRI::BufferUsage::BUFFER_USAGE_STORAGE | NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC |
									 NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST);
	allocation = VulkanNRIAllocation(
		nri, buffer.getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	buffer.bindMemory(allocation, 0);
}

VulkanNRIBuffer &VulkanMemoryCache::getAccelerationStructureScratch(
	VulkanNRI &nri, vk::AccelerationStructureBuildSizesInfoKHR &sizeInfo) {
	assureSize(nri, sizeInfo.buildScratchSize);
	return buffer;
}

VulkanMemoryCache &VulkanMemoryCache::getInstance() {
	static VulkanMemoryCache instance;
	return instance;
}

void VulkanMemoryCache::destroy() {
	getInstance().buffer	 = VulkanNRIBuffer(nullptr);
	getInstance().allocation = VulkanNRIAllocation(nullptr);
}

#ifdef __linux__
	#include <X11/Xlib.h>
	#include <vulkan/vulkan_xlib.h>
	#include <wayland-client.h>
	#include <vulkan/vulkan_wayland.h>

VkSurfaceKHR createX11Surface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri) {
	auto *X11App = app.nativeInterface<QNativeInterface::QX11Application>();
	if (!X11App) return nullptr;

	static PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR =
		reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(nri->getInstance().getProcAddr("vkCreateXlibSurfaceKHR"));
	assert(vkCreateXlibSurfaceKHR != nullptr);

	VkXlibSurfaceCreateInfoKHR createInfo{};
	createInfo.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	createInfo.dpy	  = X11App->display();
	createInfo.window = static_cast<Window>(window.winId());

	VkSurfaceKHR surface;
	if (vkCreateXlibSurfaceKHR(*nri->getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) { return nullptr; }
	return surface;
}

	#undef None
	#include <qpa/qplatformnativeinterface.h>

VkSurfaceKHR createWaylandSurface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri) {
	auto *waylandApp = app.nativeInterface<QNativeInterface::QWaylandApplication>();
	if (!waylandApp) return nullptr;

	// auto *pni = app.platformNativeInterface();

	dbLog(dbg::LOG_INFO, "Platform: ", QGuiApplication::platformName().toStdString());

	static PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR =
		reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(nri->getInstance().getProcAddr("vkCreateWaylandSurfaceKHR"));
	assert(vkCreateWaylandSurfaceKHR != nullptr);

	VkWaylandSurfaceCreateInfoKHR createInfo{};
	createInfo.sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	createInfo.display = waylandApp->display();
	createInfo.surface = std::bit_cast<wl_surface *>(window.winId());

	VkSurfaceKHR surface;
	if (vkCreateWaylandSurfaceKHR(*nri->getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
		return nullptr;
	}
	return surface;
}
#endif
#ifdef _WIN32
	#include <windows.h>
	#include <vulkan/vulkan_win32.h>
VkSurfaceKHR createWindowsSurface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri) {
	static PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR =
		reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(nri->getInstance().getProcAddr("vkCreateWin32SurfaceKHR"));
	assert(vkCreateWin32SurfaceKHR != nullptr);

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd	 = reinterpret_cast<HWND>(window.winId());

	VkSurfaceKHR surface;
	if (vkCreateWin32SurfaceKHR(*nri->getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) { return nullptr; }
	return surface;
}
#endif

static VkSurfaceKHR createSurface(QApplication &app, VulkanNRIQWindow &window, const VulkanNRI *nri) {
	VkSurfaceKHR surface = nullptr;
#ifdef _WIN32
	surface = createWindowsSurface(app, window, nri);
	if (surface) return surface;
	throw std::runtime_error("Failed to create Vulkan surface for the current Windows windowing system!");
#elif __linux__
	surface = createX11Surface(app, window, nri);
	if (surface) return surface;
	surface = createWaylandSurface(app, window, nri);
	if (surface) return surface;
	throw std::runtime_error("Failed to create Vulkan surface for the current Linux windowing system!");
#else
	throw std::runtime_error("Unsupported platform for Vulkan surface creation!");
#endif
}

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
	window->startFrameTimer();
	return window;
}

VulkanNRIBLAS::VulkanNRIBLAS(VulkanNRI &nri, VulkanNRIBuffer &vertexBuffer, NRI::Format vertexFormat,
							 std::size_t vertexOffset, uint32_t vertexCount, std::size_t vertexStride,
							 VulkanNRIBuffer &indexBuffer, NRI::IndexType indexType, std::size_t indexOffset)
	: nri(&nri), accelerationStructure(nullptr), asBuffer(nullptr), asMemory(nullptr), indexOffset(indexOffset) {
	assert(vertexFormat != NRI::Format::FORMAT_UNDEFINED);
	assert(vertexFormat < NRI::Format::_FORMAT_NUM);
	vk::Format	  vkVertexFormat = (vk::Format)nriFormat2vkFormat[static_cast<int>(vertexFormat)];
	vk::IndexType vkIndexType	 = (vk::IndexType)nriIndexType2vkIndexType[static_cast<int>(indexType)];

	vk::AccelerationStructureGeometryTrianglesDataKHR trianglesData(
		vkVertexFormat, vertexBuffer.getAddress(), vertexStride, vertexCount, vkIndexType, indexBuffer.getAddress());
	this->tempBuildInfo				  = std::make_unique<TemporaryBuildInfo>();
	this->tempBuildInfo->vertexBuffer = &vertexBuffer;
	this->tempBuildInfo->indexBuffer  = &indexBuffer;

	this->tempBuildInfo->geometry = vk::AccelerationStructureGeometryKHR(vk::GeometryTypeKHR::eTriangles, trianglesData,
																		 vk::GeometryFlagBitsKHR::eOpaque);
	std::size_t primitiveCount	  = -1;
	if (indexType == NRI::IndexType::INDEX_TYPE_UINT16) primitiveCount = indexBuffer.getSize() / sizeof(uint16_t) / 3;
	else if (indexType == NRI::IndexType::INDEX_TYPE_UINT32)
		primitiveCount = indexBuffer.getSize() / sizeof(uint32_t) / 3;

	this->tempBuildInfo->buildRangeInfo =
		vk::AccelerationStructureBuildRangeInfoKHR(primitiveCount, vertexOffset, 0, 0);
	this->tempBuildInfo->buildGeometryInfo = vk::AccelerationStructureBuildGeometryInfoKHR(
		vk::AccelerationStructureTypeKHR::eBottomLevel, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace,
		vk::BuildAccelerationStructureModeKHR::eBuild, {}, this->accelerationStructure, 1,
		&this->tempBuildInfo->geometry);

	this->tempBuildInfo->sizeInfo = nri.getDevice().getAccelerationStructureBuildSizesKHR(
		vk::AccelerationStructureBuildTypeKHR::eDevice, this->tempBuildInfo->buildGeometryInfo,
		{this->tempBuildInfo->buildRangeInfo.primitiveCount});

	this->asBuffer = VulkanNRIBuffer(nri, this->tempBuildInfo->sizeInfo.accelerationStructureSize,
									 NRI::BufferUsage::BUFFER_USAGE_ACCELERATION_STRUCTURE);
	this->asMemory = VulkanNRIAllocation(
		nri, asBuffer.getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	this->asBuffer.bindMemory(asMemory, 0);

	vk::AccelerationStructureCreateInfoKHR asCreateInfo({}, asBuffer.getBuffer(), 0,
														this->tempBuildInfo->sizeInfo.accelerationStructureSize,
														vk::AccelerationStructureTypeKHR::eBottomLevel);

	this->tempBuildInfo->scratchBuffer =
		VulkanNRIBuffer(nri, this->tempBuildInfo->sizeInfo.buildScratchSize,
						NRI::BufferUsage::BUFFER_USAGE_STORAGE | NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC |
							NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST);

	this->tempBuildInfo->scratchMemory =
		VulkanNRIAllocation(nri, tempBuildInfo->scratchBuffer.getMemoryRequirements().setTypeRequest(
									 NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	this->tempBuildInfo->scratchBuffer.bindMemory(this->tempBuildInfo->scratchMemory, 0);

	this->tempBuildInfo->buildGeometryInfo.scratchData.setDeviceAddress(
		this->tempBuildInfo->scratchBuffer.getAddress());

	this->accelerationStructure = nri.getDevice().createAccelerationStructureKHR(asCreateInfo);
	this->tempBuildInfo->buildGeometryInfo.dstAccelerationStructure = this->accelerationStructure;
}

void VulkanNRIBLAS::build(NRICommandBuffer &commandBuffer) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);

	assert(this->tempBuildInfo != nullptr);
	const auto &buildGeometryInfo = this->tempBuildInfo->buildGeometryInfo;
	const auto &buildRangeInfos	  = this->tempBuildInfo->buildRangeInfo;

	vk::MemoryBarrier memoryBarrier(vk::AccessFlagBits::eAccelerationStructureWriteKHR,
									vk::AccessFlagBits::eAccelerationStructureReadKHR);
	// buffer barrier to wait for vertex and index buffer transfer writes to finish
	vk::BufferMemoryBarrier vertexBufferBarrier(
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eAccelerationStructureReadKHR | vk::AccessFlagBits::eShaderRead, VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED, this->tempBuildInfo->vertexBuffer->getBuffer(), 0,
		this->tempBuildInfo->vertexBuffer->getSize());
	vk::BufferMemoryBarrier indexBufferBarrier(
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eAccelerationStructureReadKHR | vk::AccessFlagBits::eShaderRead, VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED, this->tempBuildInfo->indexBuffer->getBuffer(), 0,
		this->tempBuildInfo->indexBuffer->getSize());

	vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
										   vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR, {},
										   {memoryBarrier}, {vertexBufferBarrier, indexBufferBarrier}, {});

	vkCmdBuf.commandBuffer.buildAccelerationStructuresKHR(buildGeometryInfo, &buildRangeInfos);

	vk::MemoryBarrier postBuildBarrier(vk::AccessFlagBits::eAccelerationStructureWriteKHR,
									   vk::AccessFlagBits::eAccelerationStructureReadKHR);
	vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
										   vk::PipelineStageFlagBits::eRayTracingShaderKHR, {}, {postBuildBarrier}, {},
										   {});
}

void VulkanNRIBLAS::buildFinished() { this->tempBuildInfo.reset(); }

vk::DeviceAddress VulkanNRIBLAS::getAddress() const {
	vk::AccelerationStructureDeviceAddressInfoKHR addressInfo(this->accelerationStructure);
	return nri->getDevice().getAccelerationStructureAddressKHR(addressInfo);
}

VulkanNRITLAS::VulkanNRITLAS(VulkanNRI &nri, const std::span<const NRIBLAS *> &blases,
							 std::optional<std::span<glm::mat4x3>> transforms)
	: nri(&nri), as(nullptr), asBuffer(nullptr), asMemory(nullptr) {
	tempBuildInfo = std::make_unique<TemporaryBuildInfo>();

	tempBuildInfo->instanceUploadBuffer =
		VulkanNRIBuffer(nri, sizeof(vk::AccelerationStructureInstanceKHR) * blases.size(),
						NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC | NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST);
	tempBuildInfo->instanceUploadMemory =
		VulkanNRIAllocation(nri, tempBuildInfo->instanceUploadBuffer.getMemoryRequirements().setTypeRequest(
									 NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD));
	tempBuildInfo->instanceUploadBuffer.bindMemory(tempBuildInfo->instanceUploadMemory, 0);
	{
		vk::AccelerationStructureInstanceKHR *data =
			(vk::AccelerationStructureInstanceKHR *)tempBuildInfo->instanceUploadBuffer.map(
				0, tempBuildInfo->instanceUploadBuffer.getSize());
		std::span<vk::AccelerationStructureInstanceKHR> instances{data,
																  data + tempBuildInfo->instanceUploadBuffer.getSize()};

		int i = 0;
		for (const auto &blas : blases) {
			auto								 &vulkanBLAS = static_cast<const VulkanNRIBLAS &>(*blas);
			vk::AccelerationStructureInstanceKHR &instance	 = instances[i];
			if (transforms.has_value()) {
				auto &t = transforms.value()[i];
				std::memcpy(&instance.transform.matrix, &t, sizeof(glm::mat4x3));
			} else {
				glm::mat4x3 identity = glm::mat4x3(1.0f);
				std::memcpy(&instance.transform.matrix, &identity, sizeof(glm::mat4x3));
			}
			instance.instanceCustomIndex					= 0;
			instance.mask									= 0xFF;
			instance.instanceShaderBindingTableRecordOffset = 0;
			instance.flags = (VkGeometryInstanceFlagsKHR)vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable;
			instance.accelerationStructureReference = vulkanBLAS.getAddress();
			++i;
		}
		tempBuildInfo->instanceUploadBuffer.unmap();
	}

	// allocate instance buffer
	tempBuildInfo->instanceBuffer = VulkanNRIBuffer(
		nri, tempBuildInfo->instanceUploadBuffer.getSize(),
		NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST | NRI::BufferUsage::BUFFER_USAGE_ACCELERATION_STRUCTURE);
	tempBuildInfo->instanceMemory =
		VulkanNRIAllocation(nri, tempBuildInfo->instanceBuffer.getMemoryRequirements().setTypeRequest(
									 NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	tempBuildInfo->instanceBuffer.bindMemory(tempBuildInfo->instanceMemory, 0);
	// will copy to it later

	// get size info
	vk::AccelerationStructureGeometryInstancesDataKHR instancesData(VK_FALSE,
																	tempBuildInfo->instanceBuffer.getAddress());
	tempBuildInfo->geometry = vk::AccelerationStructureGeometryKHR(vk::GeometryTypeKHR::eInstances, instancesData,
																   vk::GeometryFlagBitsKHR::eOpaque);

	tempBuildInfo->buildRangeInfo	 = vk::AccelerationStructureBuildRangeInfoKHR(blases.size(), 0, 0, 0);
	tempBuildInfo->buildGeometryInfo = vk::AccelerationStructureBuildGeometryInfoKHR(
		vk::AccelerationStructureTypeKHR::eTopLevel, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace,
		vk::BuildAccelerationStructureModeKHR::eBuild, {}, {}, 1, &tempBuildInfo->geometry, nullptr, nullptr);

	vk::AccelerationStructureBuildSizesInfoKHR sizeInfo = nri.getDevice().getAccelerationStructureBuildSizesKHR(
		vk::AccelerationStructureBuildTypeKHR::eDevice, tempBuildInfo->buildGeometryInfo,
		{tempBuildInfo->buildRangeInfo.primitiveCount});

	// create AS buffer and AS itself
	this->asBuffer =
		VulkanNRIBuffer(nri, sizeInfo.accelerationStructureSize, NRI::BufferUsage::BUFFER_USAGE_ACCELERATION_STRUCTURE);
	this->asMemory = VulkanNRIAllocation(
		nri, asBuffer.getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	this->asBuffer.bindMemory(asMemory, 0);

	vk::AccelerationStructureCreateInfoKHR asCreateInfo({}, asBuffer.getBuffer(), 0,
														sizeInfo.accelerationStructureSize);
	this->as = nri.getDevice().createAccelerationStructureKHR(asCreateInfo);
	tempBuildInfo->buildGeometryInfo.dstAccelerationStructure = this->as;

	// create scratch buffer
	tempBuildInfo->scratchBuffer =
		VulkanNRIBuffer(nri, sizeInfo.buildScratchSize,
						NRI::BufferUsage::BUFFER_USAGE_STORAGE | NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC |
							NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST);
	tempBuildInfo->scratchMemory =
		VulkanNRIAllocation(nri, tempBuildInfo->scratchBuffer.getMemoryRequirements().setTypeRequest(
									 NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	tempBuildInfo->scratchBuffer.bindMemory(tempBuildInfo->scratchMemory, 0);

	tempBuildInfo->buildGeometryInfo.scratchData.setDeviceAddress(tempBuildInfo->scratchBuffer.getAddress());
}

void VulkanNRITLAS::build(NRICommandBuffer &commandBuffer) {
	auto &vkCmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	assert(this->tempBuildInfo != nullptr);
	vkCmdBuf.begin();

	tempBuildInfo->instanceBuffer.copyFrom(commandBuffer, tempBuildInfo->instanceUploadBuffer, 0, 0,
										   tempBuildInfo->instanceUploadBuffer.getSize());

	vk::BufferMemoryBarrier instanceBufferBarrier(
		vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eAccelerationStructureReadKHR, VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED, this->tempBuildInfo->instanceBuffer.getBuffer(), 0,
		this->tempBuildInfo->instanceBuffer.getSize());
	vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
										   vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR, {}, {},
										   {instanceBufferBarrier}, {});

	vkCmdBuf.commandBuffer.buildAccelerationStructuresKHR({this->tempBuildInfo->buildGeometryInfo},
														  &this->tempBuildInfo->buildRangeInfo);

	vk::MemoryBarrier postBuildBarrier(vk::AccessFlagBits::eAccelerationStructureWriteKHR,
									   vk::AccessFlagBits::eAccelerationStructureReadKHR);
	vkCmdBuf.commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
										   vk::PipelineStageFlagBits::eRayTracingShaderKHR, {}, {postBuildBarrier}, {},
										   {});
}

void VulkanNRITLAS::buildFinished() { this->tempBuildInfo.reset(); }

std::unique_ptr<NRIBLAS> VulkanNRI::createBLAS(NRIBuffer &vertexBuffer, NRI::Format vertexFormat,
											   std::size_t vertexOffset, uint32_t vertexCount, std::size_t vertexStride,
											   NRIBuffer &indexBuffer, NRI::IndexType indexType,
											   std::size_t indexOffset) {
	return std::make_unique<VulkanNRIBLAS>(*this, static_cast<VulkanNRIBuffer &>(vertexBuffer), vertexFormat,
										   vertexOffset, vertexCount, vertexStride,
										   static_cast<VulkanNRIBuffer &>(indexBuffer), indexType, indexOffset);
}

std::unique_ptr<NRITLAS> VulkanNRI::createTLAS(const std::span<const NRIBLAS *>		&blases,
											   std::optional<std::span<glm::mat4x3>> transforms) {
	return std::make_unique<VulkanNRITLAS>(*this, blases, transforms);
}

std::unique_ptr<NRIAllocation> VulkanNRI::allocateMemory(NRI::MemoryRequirements memoryRequirements) {
	return std::make_unique<VulkanNRIAllocation>(*this, memoryRequirements);
}

std::unique_ptr<NRICommandBuffer> VulkanNRI::createCommandBuffer(const NRICommandPool &pool) {
	vk::CommandBufferAllocateInfo allocInfo(static_cast<const VulkanNRICommandPool &>(pool).commandPool,
											vk::CommandBufferLevel::ePrimary, 1);
	vk::raii::CommandBuffers	  buffers(device, allocInfo);
	return std::make_unique<VulkanNRICommandBuffer>(std::move(buffers[0]));
};

NRICommandQueue::SubmitKey VulkanNRICommandQueue::submit(NRICommandBuffer &commandBuffer) {
	auto &cmdBuf = static_cast<VulkanNRICommandBuffer &>(commandBuffer);
	cmdBuf.end();
	vk::CommandBuffer vk = cmdBuf.commandBuffer;
	queue.submit(vk::SubmitInfo(0, nullptr, nullptr, 1, &vk), nullptr);
	return 0;	  // TODO: fix this
}

void VulkanNRICommandQueue::wait(SubmitKey key) {
	static_cast<void>(key);		// TODO: fix this
	queue.waitIdle();
}

static int __asd = []() {
	NRIFactory::getInstance().registerNRI("Vulkan", []() -> NRI * { return new VulkanNRI(); });
	return 0;
}();
