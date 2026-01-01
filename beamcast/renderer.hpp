#pragma once
#include "../native/nri.hpp"
#include "camera.hpp"
#include "iskeypressed.hpp"

class BeamcastRenderer : public Renderer {
   public:
	int									frameCount = 0;
	std::unique_ptr<NRIAllocation>		vertexBufferMemory;
	std::unique_ptr<NRIBuffer>			vertexBuffer;
	std::unique_ptr<NRIGraphicsProgram> shader;
	Camera								camera;
	IsKeyPressed						isKeyPressed;

	BeamcastRenderer(NRI &nri, QApplication &app)
		: Renderer(nri), camera(app, 800, 600, nri.shouldFlipY()), isKeyPressed(app) {}

	NRIQWindow *window;

	void initialize(NRIQWindow &window) override {
		this->window = &window;

		// these will die eventually
		auto uploadBuffer		= nri.createBuffer(10000, NRI::BufferUsage::BUFFER_USAGE_TRANSFER_SRC);
		auto uploadBufferMemory = nri.allocateMemory(
			uploadBuffer->getMemoryRequirements().setTypeRequest(NRI::MemoryTypeRequest::MEMORY_TYPE_UPLOAD));
		uploadBuffer->bindMemory(*uploadBufferMemory, 0);

		vertexBuffer = nri.createBuffer(18 * sizeof(float), NRI::BUFFER_USAGE_VERTEX | NRI::BUFFER_USAGE_TRANSFER_DST);
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

		auto key = window.getMainQueue().submit(*cmdBuffer);
		window.getMainQueue().wait(key);

		auto shaderBuilder = nri.createProgramBuilder();

		shader = shaderBuilder
					 ->setVertexBindings({
						 {0,
						  24,
						  NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX,
						  {
							  {0, NRI::Format::FORMAT_R32G32B32_SFLOAT, 0,
							   NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX, "inPosition"},
							  {1, NRI::Format::FORMAT_R32G32B32_SFLOAT, 12,
							   NRI::VertexInputRate::VERTEX_INPUT_RATE_VERTEX, "inColor"},
						  }},
					 })
					 .addShaderModule(
						 {PROJECT_ROOT_DIR "shaders/simple.hlsl", "VSMain", NRI::ShaderType::SHADER_TYPE_VERTEX})
					 .addShaderModule(
						 {PROJECT_ROOT_DIR "shaders/simple.hlsl", "PSMain", NRI::ShaderType::SHADER_TYPE_FRAGMENT})
					 .setPrimitiveType(NRI::PrimitiveType::PRIMITIVE_TYPE_TRIANGLES)
					 .setPushConstantRanges({camera.getPushConstantRange()})
					 .buildGraphicsProgram();

		std::cout << "Renderer initialized." << std::endl;

		window.addResizeCallback([this](QResizeEvent *event) { this->resizeEvent(event); });
		window.addKeyCallback([this](QKeyEvent *event) { this->keyEvent(event); });
		window.addMouseCallback([this](QMouseEvent *event) { this->mouseEvent(event); });
	}

	void resizeEvent(QResizeEvent *event) override {
		camera.setAspectRatio(static_cast<float>(event->size().width()) / static_cast<float>(event->size().height()));
	}

	void keyEvent(QKeyEvent *event) { isKeyPressed.keyEvent(event); }

	void mouseEvent(QMouseEvent *event) { camera.handleMouseEvent(event); }

	void render(NRIImage2D &currentImage, NRICommandBuffer &cmdBuf) override {
		camera.update(isKeyPressed, window->deltaTime());
		++frameCount;

		cmdBuf.begin();

		shader->bind(cmdBuf);
		cmdBuf.beginRendering(currentImage);
		// currentImage.clear(cmdBuf, glm::vec4(0.0f, glm::sin(frameCount / 50.f) * 0.5f + 0.5f, 0.0f, 1.0f));
		// currentImage.prepareForPresent(cmdBuf);

		vertexBuffer->bindAsVertexBuffer(cmdBuf, 0, 0);
		glm::vec4 pushConstants =
			glm::vec4(glm::sin(frameCount / 50.f) * 0.5f + 0.5f, 0.0f, glm::cos(frameCount / 50.f) * 0.5f + 0.5f, 1.0f);
		camera.setPushConstants(*shader, cmdBuf);
		shader->draw(cmdBuf, 3, 1, 0, 0);
		shader->unbind(cmdBuf);

		cmdBuf.endRendering();

		currentImage.prepareForPresent(cmdBuf);

		cmdBuf.end();
	}
};
