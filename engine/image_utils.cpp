#include "image_utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#if defined(__linux__) || defined(__APPLE__)
	#include <stb/stb_image.h>
#else
	#include <stb_image.h>
#endif

std::pair<std::unique_ptr<NRIImage2D>, std::unique_ptr<NRIAllocation>> createImage2D(std::string_view fileName,
																					 NRI &nri, NRICommandQueue &queue,
																					 NRI::ImageUsage usage) {
	int				width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc *data = stbi_load(fileName.data(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data) { 
		dbLog(dbg::LOG_ERROR, "Failed to load image: ", fileName);
		throw std::runtime_error("Failed to load image: " + std::string(fileName));
	}
	stbi_set_flip_vertically_on_load(0);
	std::size_t imageSize = width * height * 4;
	auto stagingBuffer	 = nri.createBuffer(imageSize, NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC);
	auto stagingMemory	 = nri.allocateMemory(stagingBuffer->getMemoryRequirements().setTypeRequest(
		NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD));
	stagingBuffer->bindMemory(*stagingMemory, 0);
	{
		void *mappedData = stagingBuffer->map(0, stagingBuffer->getSize());
		std::memcpy(mappedData, data, imageSize);
		stagingBuffer->unmap();
	}
	stbi_image_free(data);
	auto image = nri.createImage2D(width, height, NRI::Format::FORMAT_R8G8B8A8_UNORM,
									usage | NRI::ImageUsage::IMAGE_USAGE_TRANSFER_DST);
	auto imageMemory = nri.allocateMemory(image->getMemoryRequirements().setTypeRequest(
		NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	image->bindMemory(*imageMemory, 0);
	{
		auto commandBuffer = nri.createCommandBuffer(nri.getDefaultCommandPool());
		commandBuffer->begin();
		image->copyFrom(*commandBuffer, *stagingBuffer, 0, 0);
		commandBuffer->end();
		auto k =  queue.submit(*commandBuffer);
		queue.wait(k);
	}
	return {std::move(image), std::move(imageMemory)};
}



