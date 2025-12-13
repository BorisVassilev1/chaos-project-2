#pragma once
#include "../native/nri.hpp"

class BeamcastRenderer : public Renderer {
   public:
	int frameCount = 0;

	BeamcastRenderer() {}

	void render(NRIImage2D &currentImage, NRICommandBuffer &cmdBuf) override {
		++frameCount;

		cmdBuf.begin();

		currentImage.clear(cmdBuf, glm::vec4(0.0f, glm::sin(frameCount / 50.f) * 0.5f + 0.5f, 0.0f, 1.0f));
		currentImage.prepareForPresent(cmdBuf);

		cmdBuf.end();
	}
};

