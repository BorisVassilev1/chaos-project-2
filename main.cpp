#include <iostream>
#include <thread>
#include "src/vk_nri.hpp"

int main() {
	VulkanNRI nri;
	std::cout << "Vulkan NRI initialized successfully." << std::endl;

	auto buf = nri.createBuffer(1024 * 1024 * 1024, NRI::BUFFER_USAGE_VERTEX | NRI::BUFFER_USAGE_STORAGE);
	std::cout << "Buffer created successfully." << std::endl;

	auto mem = nri.allocateMemory(buf->getMemoryRequirements().setTypeRequest(NRI::MEMORY_TYPE_DEVICE));
	std::cout << "Memory allocated successfully." << std::endl;

	buf->bindMemory(*mem, 0);

	std::this_thread::sleep_for(std::chrono::seconds(3));

	auto img =
		nri.createImage2D(1024, 1024, NRI::Format::FORMAT_R16G16B16A16_SFLOAT,
						  NRI::IMAGE_USAGE_SAMPLED | NRI::IMAGE_USAGE_TRANSFER_DST | NRI::IMAGE_USAGE_TRANSFER_SRC);
	auto imgMem = nri.allocateMemory(img->getMemoryRequirements().setTypeRequest(NRI::MEMORY_TYPE_DEVICE));

	img->bindMemory(*mem, 0);
	std::cout << "Image created and memory bound successfully." << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(3));

	return 0;
}
