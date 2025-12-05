#include <iostream>
#include <thread>
#include "src/vk_nri.hpp"

#include <QtWidgets>
#include <QVulkanWindow>

int main(int argc, char* argv[]) {
	VulkanNRI nri;
	std::cout << "Vulkan NRI initialized successfully." << std::endl;

	auto buf = nri.createBuffer(1024 * 1024 * 1024, NRI::BUFFER_USAGE_VERTEX | NRI::BUFFER_USAGE_STORAGE);
	std::cout << "Buffer created successfully." << std::endl;

	auto mem = nri.allocateMemory(buf->getMemoryRequirements().setTypeRequest(NRI::MEMORY_TYPE_DEVICE));
	std::cout << "Memory allocated successfully." << std::endl;

	buf->bindMemory(*mem, 0);

	auto img =
		nri.createImage2D(1024, 1024, NRI::Format::FORMAT_R16G16B16A16_SFLOAT,
						  NRI::IMAGE_USAGE_SAMPLED | NRI::IMAGE_USAGE_TRANSFER_DST | NRI::IMAGE_USAGE_TRANSFER_SRC);
	auto imgMem = nri.allocateMemory(img->getMemoryRequirements().setTypeRequest(NRI::MEMORY_TYPE_DEVICE));

	img->bindMemory(*mem, 0);
	std::cout << "Image created and memory bound successfully." << std::endl;

	auto cmdQueue = nri.createCommandQueue();
	auto cmdPool = nri.createCommandPool();
	std::cout << "Command queue and pool created successfully." << std::endl;

	auto cmdBuffer = nri.createCommandBuffer(*cmdPool);
	std::cout << "Command buffer created successfully." << std::endl;

	img->clear(*cmdBuffer, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	cmdQueue->submit(*cmdBuffer);
	cmdQueue->synchronize();

	QApplication app(argc, argv);

	QVulkanInstance inst;
	// enable the standard validation layers, when available
	inst.setLayers({"VK_LAYER_KHRONOS_validation"});
	if (!inst.create()) qFatal("Failed to create Vulkan instance: %d", inst.errorCode());

	QVulkanWindow window;
	window.setVulkanInstance(&inst);
	window.show();

	window.setTitle("BeamCast GUI");

	app.exec();

	return 0;
}
