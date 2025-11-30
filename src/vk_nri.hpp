#pragma once
#include "nri.hpp"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_raii.hpp>

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

	uint32_t width;
	uint32_t height;

   public:
	VulkanNRIImage2D(vk::raii::Image &&img, vk::Device dev, uint32_t width, uint32_t height)
		: image(std::move(img)), device(dev), width(width), height(height) {}

	NRI::MemoryRequirements getMemoryRequirements() override;
	void					bindMemory(NRIAllocation &allocation, std::size_t offset) override;
};

class VulkanNRI : public NRI {
	vk::raii::Instance		 instance;
	vk::raii::PhysicalDevice physicalDevice;
	vk::raii::Device		 device;
	vk::raii::Queue			 graphicsQueue;

	void createInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();

   public:
	VulkanNRI();

	std::unique_ptr<NRIBuffer>	createBuffer(std::size_t size, BufferUsage usage) override;
	std::unique_ptr<NRIImage2D> createImage2D(std::size_t width, std::size_t height, NRI::Format fmt,
											  NRI::ImageUsage usage) override;

	std::unique_ptr<NRIAllocation> allocateMemory(MemoryRequirements memoryRequirements) override;
};
