#pragma once
#include <qnamespace.h>
#include "../native/nri.hpp"
#include "camera.hpp"
#include "iskeypressed.hpp"
#include "mesh.hpp"
#include "scene.hpp"

#include "../engine/image_utils.hpp"
#include "../engine/transformation.hpp"

namespace beamcast {

class BeamcastRenderer : public nri::Renderer {
   public:
	int									  frameCount = 0;
	std::unique_ptr<nri::GraphicsProgram> shader;
	Camera								  camera;
	IsKeyPressed						  isKeyPressed;
	std::unique_ptr<Mesh>				  mesh;

	std::unique_ptr<nri::Image2D>	 texture;
	std::unique_ptr<nri::Allocation> textureMemory;
	nri::ResourceHandle				 textureHandle;
	std::unique_ptr<nri::ImageView>	 textureView;

	std::unique_ptr<Scene> scene;

	std::unique_ptr<nri::RayTracingProgram> rayTracingShader;
	std::unique_ptr<nri::Image2D>			rayTracingOutputImage;
	std::unique_ptr<nri::Allocation>		rayTracingOutputImageMemory;
	std::unique_ptr<nri::ImageView>			rayTracingOutputImageRWView;
	std::unique_ptr<nri::ImageView>			rayTracingOutputImageSampledView;
	bool raytracing = false;

	BeamcastRenderer(nri::NRI &nri, QApplication &app);

	nri::QWindow *window;

	void initialize(nri::QWindow &window) override;

	void resizeEvent(QResizeEvent *event) override;

	void keyEvent(QKeyEvent *event);

	void mouseEvent(QMouseEvent *event);

	glm::mat4 calcMatrix(const Camera &camera);

	void render(const nri::ImageAndViewRef &currentImage, nri::CommandBuffer &cmdBuf) override;
};
}	  // namespace beamcast
