#include "renderer.hpp"

namespace beamcast {

BeamcastRenderer::BeamcastRenderer(nri::NRI &nri, QApplication &app)
	: Renderer(nri), camera(app, 800, 600, nri.shouldFlipY()), isKeyPressed(app) {}

struct RayTracingPushConstants {
	glm::mat4			cameraWorldMatrix;
	float				fovy;
	nri::ResourceHandle outputImageHandle;
	nri::ResourceHandle sceneHandle;
	nri::ResourceHandle materialsHandle;
	nri::ResourceHandle meshObjectsHandle;
	nri::ResourceHandle meshesHandle;
};

void BeamcastRenderer::initialize(nri::QWindow &window) {
	this->window = &window;

	mesh = std::make_unique<QuadMesh>(nri, window.getMainQueue(), 2.0f);

	scene = std::make_unique<Scene>(nri, window.getMainQueue(), PROJECT_ROOT_DIR "/export.json");

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
		nri.createImage2D(1024, 600, nri::Format::FORMAT_R32G32B32A32_SFLOAT,
						  nri::ImageUsage::IMAGE_USAGE_STORAGE | nri::ImageUsage::IMAGE_USAGE_TRANSFER_SRC |
							  nri::ImageUsage::IMAGE_USAGE_SAMPLED);
	rayTracingOutputImageMemory = nri.allocateMemory(rayTracingOutputImage->getMemoryRequirements());
	rayTracingOutputImage->bindMemory(*rayTracingOutputImageMemory, 0);
	rayTracingOutputImageRWView		 = rayTracingOutputImage->createStorageView();
	rayTracingOutputImageSampledView = rayTracingOutputImage->createTextureView();
	dbLog(dbg::LOG_INFO,
		  "Ray tracing output image created. handle index: ", rayTracingOutputImageRWView->getHandle().getIndex());

	{
		auto cmdBuf = nri.createCommandBuffer(nri.getDefaultCommandPool());
		cmdBuf->begin();

		shaderBuilder = nri.createProgramBuilder();
		rayTracingShader =
			shaderBuilder
				->addShaderModule(
					{PROJECT_ROOT_DIR "shaders/rt.hlsl", "RayGenMain", nri::ShaderType::SHADER_TYPE_RAYGEN})
				.addShaderModule({PROJECT_ROOT_DIR "shaders/rt.hlsl", "MissMain", nri::ShaderType::SHADER_TYPE_MISS})
				.addShaderModule(
					{PROJECT_ROOT_DIR "shaders/rt.hlsl", "ClosestHitMain", nri::ShaderType::SHADER_TYPE_CLOSEST_HIT})
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

	rayTracingOutputImage =
		nri.createImage2D(window->width(),window->height(), nri::Format::FORMAT_R32G32B32A32_SFLOAT,
						  nri::ImageUsage::IMAGE_USAGE_STORAGE | nri::ImageUsage::IMAGE_USAGE_TRANSFER_SRC |
							  nri::ImageUsage::IMAGE_USAGE_SAMPLED);
	rayTracingOutputImageMemory = nri.allocateMemory(rayTracingOutputImage->getMemoryRequirements());
	rayTracingOutputImage->bindMemory(*rayTracingOutputImageMemory, 0);
	rayTracingOutputImageRWView		 = rayTracingOutputImage->createStorageView();
	rayTracingOutputImageSampledView = rayTracingOutputImage->createTextureView();
}

void BeamcastRenderer::keyEvent(QKeyEvent *event) {
	isKeyPressed.keyEvent(event);
	if (event->key() == Qt::Key_Escape && event->type() == QEvent::KeyPress) window->close();
	if (event->key() == Qt::Key_1 && event->type() == QEvent::KeyPress) camera.toggleControls();
	if (event->key() == Qt::Key_2 && event->type() == QEvent::KeyPress) { this->raytracing = !this->raytracing; }
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

glm::mat4 getMatrixInFrontOfCamera(const Camera &camera) {
	Transformation cameraTransform = camera.transform;
	cameraTransform.updateWorldMatrix();

	glm::mat4 result  = getCameraWorldMatrix(camera);
	result			  = glm::translate(result, glm::vec3(0.0f, 0.0f, -1.f / glm::tan(camera.getFOV() / 2.0f)));
	float aspectRatio = camera.getAspectRatio();
	result			  = glm::scale(result, glm::vec3(aspectRatio, 1.0f, 1.0f));

	return result;
}

void BeamcastRenderer::render(const nri::ImageAndViewRef &currentImage, nri::CommandBuffer &cmdBuf) {
	camera.update(isKeyPressed, window->deltaTime());
	++frameCount;

	cmdBuf.begin();

	if (raytracing) {
		if (nri.supportsRayTracing()) {
			rayTracingShader->bind(cmdBuf);

			RayTracingPushConstants pushConstants = {};
			pushConstants.cameraWorldMatrix		  = getCameraWorldMatrix(camera);
			pushConstants.fovy					  = camera.getFOV();
			pushConstants.outputImageHandle		  = rayTracingOutputImageRWView->getHandle();
			rayTracingOutputImage->prepareForStorage(cmdBuf);
			pushConstants.sceneHandle		= scene->getTLAS().getHandle();
			pushConstants.materialsHandle	= scene->getMaterialsBuffer().getHandle();
			pushConstants.meshObjectsHandle = scene->getMeshObjectsBuffer().getHandle();
			pushConstants.meshesHandle		= scene->getMeshesBuffer().getHandle();

			rayTracingShader->setPushConstants(cmdBuf, &pushConstants, sizeof(pushConstants), 0);
			rayTracingShader->traceRays(cmdBuf, rayTracingOutputImage->getWidth(), rayTracingOutputImage->getHeight(),
										1);

			rayTracingOutputImage->prepareForTexture(cmdBuf);

			shader->bind(cmdBuf);
			window->beginRendering(cmdBuf, currentImage);
			scene->drawMesh(cmdBuf, *shader, camera, *mesh, getMatrixInFrontOfCamera(camera),
							rayTracingOutputImageSampledView->getHandle());
			window->endRendering(cmdBuf);
		}
	} else {
		shader->bind(cmdBuf);
		// draw scene
		{
			window->beginRendering(cmdBuf, currentImage);
			// calculate transform matrix for display
			// auto matrix = calcMatrix(camera);
			// scene->drawMesh(cmdBuf, *shader, camera, *mesh, matrix, rayTracingOutputImageSampledView->getHandle());

			scene->render(cmdBuf, *shader, camera);

			window->endRendering(cmdBuf);
		}
	}

	currentImage.image.prepareForPresent(cmdBuf);

	cmdBuf.end();
}
}	  // namespace beamcast
