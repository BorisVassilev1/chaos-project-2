#pragma once
#include "../native/nri.hpp"
#include "buffer_utils.hpp"
#include "camera.hpp"
#include "iskeypressed.hpp"
#include "mesh.hpp"
#include "scene.hpp"

class BeamcastRenderer : public Renderer {
   public:
	int									frameCount = 0;
	std::unique_ptr<NRIGraphicsProgram> shader;
	Camera								camera;
	IsKeyPressed						isKeyPressed;
	std::unique_ptr<Mesh>				mesh;

	std::unique_ptr<Scene> scene;

	BeamcastRenderer(NRI &nri, QApplication &app)
		: Renderer(nri), camera(app, 800, 600, nri.shouldFlipY()), isKeyPressed(app) {}

	NRIQWindow *window;

	void initialize(NRIQWindow &window) override {
		this->window = &window;

		mesh = std::make_unique<TriangleMesh>(nri, window.getMainQueue());

		//scene = std::make_unique<Scene>(nri, window.getMainQueue(), PROJECT_ROOT_DIR "/export.json");
		//scene = std::make_unique<Scene>(nri, window.getMainQueue(), PROJECT_ROOT_DIR "/../chaos-project/scenes/14/scene1.crtscene");


		auto shaderBuilder = nri.createProgramBuilder();
		shader = shaderBuilder
					 ->setVertexBindings(mesh->getVertexBindings())
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

		mesh->bind(cmdBuf);

		camera.setPushConstants(*shader, cmdBuf);
		shader->drawIndexed(cmdBuf, 3, 1, 0, 0, 0);

		//scene->render(cmdBuf, *shader);

		cmdBuf.endRendering();

		currentImage.prepareForPresent(cmdBuf);

		cmdBuf.end();
	}
};
