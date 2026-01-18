#pragma once

#include <qnamespace.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "../native/nri.hpp"
#include "iskeypressed.hpp"

#include "../engine/transformation.hpp"

namespace beamcast {
class Camera {
   private:
	glm::mat4	  viewMatrix;
	glm::mat4	  projectionMatrix;
	float	  aspectRatio	   = 4.0f / 3.0f;
	float	  fov			   = glm::radians(60.0f);
	float	  nearPlane		   = 0.01f;
	float	  farPlane		   = 1000.0f;

	const bool	  flipY			 = false;
	bool		  controlsActive = true;
	QApplication &app;

	void updateViewMatrix();
	void updateProjectionMatrix();

   public:
	Transformation transform;
	Camera(QApplication &app, uint32_t width = 100, uint32_t height = 100, bool flipY = false);

	glm::vec3 getPosition() const { return transform.position; }
	glm::vec3 getRotation() const { return transform.rotation; }
	float getAspectRatio() const { return aspectRatio; }
	float getFOV() const { return fov; }
	float getNearPlane() const { return nearPlane; }
	float getFarPlane() const { return farPlane; }

	static nri::PushConstantRange getPushConstantRange();

	void	  setPushConstants(nri::Program &program, nri::CommandBuffer &commandBuffer) const;
	glm::mat4 getViewProjectionMatrix() const;
	void	  setAspectRatio(float ratio);
	void	  setResolution(uint32_t width, uint32_t height);
	void	  update(const IsKeyPressed &ikp, float deltaTime);
	void	  handleMouseEvent(QMouseEvent *event);
	void	  toggleControls();
};
}	  // namespace beamcast
