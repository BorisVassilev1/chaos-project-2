#include "vk_nri.hpp"
#include <iostream>
#include <ostream>
#include <vulkan/vulkan.hpp>

static const std::vector<const char*> validationLayers = {
	//"VK_LAYER_KHRONOS_validation"
	//"VK_LAYER_NV_optimus"
};

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif

bool checkValidationLayerSupport() {
	auto availableLayers = vk::enumerateInstanceLayerProperties();

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) { return false; }
	}

	return true;
}

VulkanNRI::VulkanNRI() : instance(nullptr) {
	vk::ApplicationInfo appInfo("VulkanNRIApp", 1, "NoEngine", 1, vk::ApiVersion14);

	vk::InstanceCreateInfo createInfo({}, &appInfo);

	if (enableValidationLayers) {
		if (!checkValidationLayerSupport()) {
			//throw std::runtime_error("Validation layers requested, but not available!");
			std::cout << "Validation layers requested, but not available!" << std::endl;
		}

		createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	instance = vk::raii::Instance(vk::raii::Context(), createInfo);

	auto availableLayers = vk::enumerateInstanceLayerProperties();
}
