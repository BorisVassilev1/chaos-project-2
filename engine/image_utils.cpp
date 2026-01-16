#include "image_utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#if defined(__linux__) || defined(__APPLE__)
	#include <stb/stb_image.h>
#else
	#include <stb_image.h>
#endif

namespace beamcast {

std::pair<std::unique_ptr<nri::Image2D>, std::unique_ptr<nri::Allocation>> createImage2D(std::string_view	fileName,
																						 nri::NRI		   &nri,
																						 nri::CommandQueue &queue,
																						 nri::ImageUsage	usage) {
	int								 width, height, channels;
	std::unique_ptr<nri::Buffer>	 stagingBuffer;
	std::unique_ptr<nri::Allocation> stagingMemory;
	// scope for stbi-alocated pointer
	{
		stbi_set_flip_vertically_on_load(1);
		auto data = std::unique_ptr<stbi_uc, void (*)(void *)>(
			stbi_load(fileName.data(), &width, &height, &channels, STBI_rgb_alpha), stbi_image_free);

		if (!data) {
			dbLog(dbg::LOG_ERROR, "Failed to load image: ", fileName);
			throw std::runtime_error(std::format("Failed to load image: {}", fileName));
		}
		stbi_set_flip_vertically_on_load(0);
		std::size_t imageSize = width * height * 4;
		stagingBuffer		  = nri.createBuffer(imageSize, nri::BufferUsage::BUFFER_USAGE_TRANSFER_SRC);
		stagingMemory		  = nri.allocateMemory(
			stagingBuffer->getMemoryRequirements().setTypeRequest(nri::MemoryTypeRequest::MEMORY_TYPE_UPLOAD));
		stagingBuffer->bindMemory(*stagingMemory, 0);
		{
			void *mappedData = stagingBuffer->map(0, stagingBuffer->getSize());
			std::memcpy(mappedData, data.get(), imageSize);
			stagingBuffer->unmap();
		}
	}
	auto image = nri.createImage2D(width, height, nri::Format::FORMAT_R8G8B8A8_UNORM,
								   usage | nri::ImageUsage::IMAGE_USAGE_TRANSFER_DST);
	auto imageMemory =
		nri.allocateMemory(image->getMemoryRequirements().setTypeRequest(nri::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
	image->bindMemory(*imageMemory, 0);
	{
		auto commandBuffer = nri.createCommandBuffer(nri.getDefaultCommandPool());
		commandBuffer->begin();
		image->copyFrom(*commandBuffer, *stagingBuffer, 0, 0);
		commandBuffer->end();
		auto k = queue.submit(*commandBuffer);
		queue.wait(k);
	}
	return {std::move(image), std::move(imageMemory)};
}
}	  // namespace beamcast
