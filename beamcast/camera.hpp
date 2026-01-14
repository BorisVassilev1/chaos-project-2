#pragma once

#include <qnamespace.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "../native/nri.hpp"
#include "iskeypressed.hpp"

class Camera {
   private:
	glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 viewMatrix	   = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	float	  aspectRatio	   = 4.0f / 3.0f;
	float	  fov			   = glm::radians(60.0f);
	float	  nearPlane		   = 0.01f;
	float	  farPlane		   = 1000.0f;

	const bool flipY		  = false;
	bool	   controlsActive = true;
	QApplication &app;

	void updateViewMatrix();
	void updateProjectionMatrix();


   public:
	Camera(QApplication &app, uint32_t width = 100, uint32_t height = 100, bool flipY = false);

	static NRI::PushConstantRange getPushConstantRange();

	void setPushConstants(NRIProgram &program, NRICommandBuffer &commandBuffer) const;
	glm::mat4 getViewProjectionMatrix() const;
	void setAspectRatio(float ratio);
	void setResolution(uint32_t width, uint32_t height);
	void update(IsKeyPressed ikp, float deltaTime);
	void handleMouseEvent(QMouseEvent *event);
	void toggleControls();
};
