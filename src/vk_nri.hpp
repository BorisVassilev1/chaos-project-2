#pragma once
#include "nri.hpp"
#include <vulkan/vulkan_raii.hpp>

class VulkanNRI : public NRI {
	vk::raii::Instance instance;

   public:
	VulkanNRI();
};
