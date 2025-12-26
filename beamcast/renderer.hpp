#pragma once
#include "../native/nri.hpp"

class BeamcastRenderer : public Renderer {
   public:
	int							   frameCount = 0;
	std::unique_ptr<NRIAllocation> vertexBufferMemory;
	std::unique_ptr<NRIBuffer>	   vertexBuffer;

	BeamcastRenderer(NRI &nri) : Renderer(nri) {}

	void initialize(NRIQWindow &window) override {
		// these will die eventually
		auto uploadBuffer = nri.createBuffer(
			10000, NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC | NRI::BufferUsage::BUFFER_USAGE_TRANSFER_DST);
		auto uploadBufferMemory = nri.allocateMemory(
			uploadBuffer->getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD));
		uploadBuffer->bindMemory(*uploadBufferMemory, 0);

		vertexBuffer = nri.createBuffer(18 * sizeof(float), NRI::BUFFER_USAGE_VERTEX | NRI::BUFFER_USAGE_TRANSFER_DST |
																NRI::BUFFER_USAGE_TRANSFER_SRC);
		vertexBufferMemory = nri.allocateMemory(
			vertexBuffer->getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_DEVICE));
		vertexBuffer->bindMemory(*vertexBufferMemory, 0);

		float vertices[] = {0.0f, -0.5f, 0.0f, 1.0f,  0.0f, 0.0f, 0.5f, 0.5f, 0.0f,
							0.0f, 1.0f,	 0.0f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
		{
			void *data = uploadBuffer->map(0, 10000);
			memcpy(data, vertices, sizeof(vertices));
			uploadBuffer->unmap();
		}

		auto cmdBuffer = nri.createCommandBuffer(nri.getDefaultCommandPool());
		vertexBuffer->copyFrom(*cmdBuffer, *uploadBuffer, 0, 0, sizeof(vertices));
		cmdBuffer->end();

		window.getMainQueue().submit(*cmdBuffer);
		window.getMainQueue().synchronize();
	}

	void render(NRIImage2D &currentImage, NRICommandBuffer &cmdBuf) override {
		++frameCount;

		cmdBuf.begin();

		cmdBuf.beginRendering(currentImage);
		// currentImage.clear(cmdBuf, glm::vec4(0.0f, glm::sin(frameCount / 50.f) * 0.5f + 0.5f, 0.0f, 1.0f));
		// currentImage.prepareForPresent(cmdBuf);

		cmdBuf.endRendering();

		currentImage.prepareForPresent(cmdBuf);

		cmdBuf.end();
	}
};
