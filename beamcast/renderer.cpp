#include "renderer.hpp"

namespace beamcast {

BeamcastRenderer::BeamcastRenderer(nri::NRI &nri, QApplication &app)
	: Renderer(nri), camera(app, 800, 600, nri.shouldFlipY()), isKeyPressed(app) {}

struct RayTracingPushConstants {
	glm::mat4			cameraWorldMatrix;
	nri::ResourceHandle outputImageHandle;
	nri::ResourceHandle sceneHandle;
};

void BeamcastRenderer::initialize(nri::QWindow &window) {
	this->window = &window;

	mesh = std::make_unique<QuadMesh>(nri, window.getMainQueue(), 2.0f);

	scene = std::make_unique<Scene>(nri, window.getMainQueue(), PROJECT_ROOT_DIR "/export.json");
	if (nri.supportsRayTracing()) nri::ResourceHandle sceneTextureHandle = scene->getTLAS().getHandle();

	if (nri.supportsTextures()) {
		std::tie(texture, textureMemory) =
			beamcast::createImage2D(PROJECT_ROOT_DIR "textures/bricks/albedo.jpg", nri, window.getMainQueue());
		textureView	  = texture->createTextureView();
		textureHandle = textureView->getHandle();
		dbLog(dbg::LOG_INFO, "Texture loaded and bound. index: ", textureHandle.getIndex());
	}

	auto shaderBuilder = nri.createProgramBuilder();
	shader =
		shaderBuilder->setVertexBindings(mesh->getVertexBindings())
			.addShaderModule({PROJECT_ROOT_DIR "shaders/simple.hlsl", "VSMain", nri::ShaderType::SHADER_TYPE_VERTEX})
			.addShaderModule({PROJECT_ROOT_DIR "shaders/simple.hlsl", "PSMain", nri::ShaderType::SHADER_TYPE_FRAGMENT})
			.setPrimitiveType(nri::PrimitiveType::PRIMITIVE_TYPE_TRIANGLES)
			.setPushConstantRanges({Scene::getPushConstantRange()})
			.buildGraphicsProgram();

	dbLog(dbg::LOG_INFO, "Renderer initialized.");

	rayTracingOutputImage =
		nri.createImage2D(window.size().width(), window.size().height(), nri::Format::FORMAT_R8G8B8A8_UNORM,
						  nri::ImageUsage::IMAGE_USAGE_STORAGE | nri::ImageUsage::IMAGE_USAGE_TRANSFER_SRC);
	rayTracingOutputImageMemory = nri.allocateMemory(rayTracingOutputImage->getMemoryRequirements());
	rayTracingOutputImage->bindMemory(*rayTracingOutputImageMemory, 0);
	rayTracingOutputImageView	= rayTracingOutputImage->createStorageView();
	rayTracingOutputImageHandle = rayTracingOutputImageView->getHandle();

	{
		auto cmdBuf = nri.createCommandBuffer(nri.getDefaultCommandPool());
		cmdBuf->begin();

		shaderBuilder	 = nri.createProgramBuilder();
		rayTracingShader = shaderBuilder
							   ->addShaderModule({PROJECT_ROOT_DIR "shaders/raygen.hlsl", "RayGenMain",
												  nri::ShaderType::SHADER_TYPE_RAYGEN})
							   .setPushConstantRanges({{0, sizeof(RayTracingPushConstants)}})
							   .buildRayTracingProgram(*cmdBuf);

		dbLog(dbg::LOG_INFO, "Ray tracing shader initialized.");
		cmdBuf->end();
		auto &presentQueue = window.getMainQueue();
		auto  submitKey	   = presentQueue.submit(*cmdBuf);
		presentQueue.wait(submitKey);
	}

	window.addResizeCallback([this](QResizeEvent *event) { this->resizeEvent(event); });
	window.addKeyCallback([this](QKeyEvent *event) { this->keyEvent(event); });
	window.addMouseCallback([this](QMouseEvent *event) { this->mouseEvent(event); });
}

void BeamcastRenderer::resizeEvent(QResizeEvent *event) {
	camera.setAspectRatio(static_cast<float>(event->size().width()) / static_cast<float>(event->size().height()));
}

void BeamcastRenderer::keyEvent(QKeyEvent *event) {
	isKeyPressed.keyEvent(event);
	if (event->key() == Qt::Key_Escape && event->type() == QEvent::KeyPress) window->close();
	if (event->key() == Qt::Key_1 && event->type() == QEvent::KeyPress) camera.toggleControls();
}

void BeamcastRenderer::mouseEvent(QMouseEvent *event) { camera.handleMouseEvent(event); }

glm::mat4 getCameraWorldMatrix(const Camera &camera) {
	Transformation cameraTransform = camera.transform;
	cameraTransform.updateWorldMatrix();

	glm::mat4 result = glm::mat4(1.0f);
	result			 = glm::translate(result, cameraTransform.position);
	result			 = glm::rotate(result, cameraTransform.rotation.z, glm::vec3(0, 0, 1));
	result			 = glm::rotate(result, cameraTransform.rotation.y, glm::vec3(0, 1, 0));
	result			 = glm::rotate(result, cameraTransform.rotation.x, glm::vec3(1, 0, 0));

	return result;
}

glm::mat4 BeamcastRenderer::calcMatrix(const Camera &camera) {
	Transformation cameraTransform = camera.transform;
	cameraTransform.updateWorldMatrix();

	glm::mat4 result = getCameraWorldMatrix(camera);
	result			 = glm::translate(result, glm::vec3(-2.0f, -1.0f, -3.0f));

	return result;
}

void BeamcastRenderer::render(const nri::ImageAndViewRef &currentImage, nri::CommandBuffer &cmdBuf) {
	camera.update(isKeyPressed, window->deltaTime());
	++frameCount;

	cmdBuf.begin();

	if (nri.supportsRayTracing()) {
		rayTracingShader->bind(cmdBuf);

		RayTracingPushConstants pushConstants = {};
		pushConstants.cameraWorldMatrix		  = getCameraWorldMatrix(camera);
		pushConstants.outputImageHandle		  = rayTracingOutputImageHandle;
		pushConstants.sceneHandle			  = scene->getTLAS().getHandle();

		rayTracingShader->setPushConstants(cmdBuf, &pushConstants, sizeof(pushConstants), 0);
		rayTracingShader->traceRays(cmdBuf, rayTracingOutputImage->getWidth(), rayTracingOutputImage->getHeight(), 1);
	}

	shader->bind(cmdBuf);
	// draw scene
	{
		window->beginRendering(cmdBuf, currentImage);
		// calculate transform matrix for display
		auto matrix = calcMatrix(camera);
		scene->drawMesh(cmdBuf, *shader, camera, *mesh, matrix, rayTracingOutputImageHandle);

		scene->render(cmdBuf, *shader, camera);

		window->endRendering(cmdBuf);
	}

	currentImage.image.prepareForPresent(cmdBuf);

	cmdBuf.end();
}
}	  // namespace beamcast
