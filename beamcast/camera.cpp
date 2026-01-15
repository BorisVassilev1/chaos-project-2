#include "camera.hpp"

void Camera::updateViewMatrix() {
	viewMatrix = glm::translate(glm::mat4x4(1), position);
	viewMatrix = glm::rotate(viewMatrix, rotation.z, glm::vec3(0, 0, 1));
	viewMatrix = glm::rotate(viewMatrix, rotation.y, glm::vec3(0, 1, 0));
	viewMatrix = glm::rotate(viewMatrix, rotation.x, glm::vec3(1, 0, 0));
	viewMatrix = glm::inverse(viewMatrix);
}

void Camera::updateProjectionMatrix() {
	projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	if (flipY) { projectionMatrix[1][1] *= -1; }
}


Camera::Camera(QApplication &app, uint32_t width, uint32_t height, bool flipY)
	: aspectRatio(static_cast<float>(width) / static_cast<float>(height)), flipY(flipY), app(app) {
	updateViewMatrix();
	updateProjectionMatrix();

	// app.setOverrideCursor(Qt::BlankCursor);
}

 NRI::PushConstantRange Camera::getPushConstantRange() { return {0, sizeof(glm::mat4)}; }

void Camera::setPushConstants(NRIProgram &program, NRICommandBuffer &commandBuffer) const {
	glm::mat4 vpMatrix = getViewProjectionMatrix();
	program.setPushConstants(commandBuffer, &vpMatrix, sizeof(glm::mat4), 0);
}

glm::mat4 Camera::getViewProjectionMatrix() const { return projectionMatrix * viewMatrix; }

void Camera::setAspectRatio(float ratio) {
	aspectRatio = ratio;
	updateProjectionMatrix();
}
void Camera::setResolution(uint32_t width, uint32_t height) {
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	updateProjectionMatrix();
}

void Camera::update(const IsKeyPressed &ikp, float deltaTime) {
	if (!controlsActive) return;

	const float cameraSpeed = 5.0f;		// units per second

	if (rotation.x > M_PI / 2) {
		rotation.x = M_PI / 2;
	} else if (rotation.x < -M_PI / 2) {
		rotation.x = -M_PI / 2;
	}

	glm::mat4 rotationMat(1);
	rotationMat		  = glm::rotate(rotationMat, rotation.x, glm::vec3(1, 0, 0));
	rotationMat		  = glm::rotate(rotationMat, rotation.y, glm::vec3(0, 1, 0));
	rotationMat		  = glm::rotate(rotationMat, rotation.z, glm::vec3(0, 0, 1));
	glm::vec3 forward = glm::vec3(rotationMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	forward.y		  = 0.0f;
	forward			  = glm::normalize(forward);

	glm::vec3 right = glm::vec3(rotationMat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	right.y			= 0.0f;
	right			= glm::normalize(right);

	if (ikp.isKeyPressed(Qt::Key_W)) { position += forward * cameraSpeed * deltaTime; }
	if (ikp.isKeyPressed(Qt::Key_S)) { position -= forward * cameraSpeed * deltaTime; }
	if (ikp.isKeyPressed(Qt::Key_A)) { position -= right * cameraSpeed * deltaTime; }
	if (ikp.isKeyPressed(Qt::Key_D)) { position += right * cameraSpeed * deltaTime; }
	if (ikp.isKeyPressed(Qt::Key_Space)) { position.y += cameraSpeed * deltaTime; }
	if (ikp.isKeyPressed(Qt::Key_Shift)) { position.y -= cameraSpeed * deltaTime; }

	updateViewMatrix();
}

void Camera::handleMouseEvent(QMouseEvent *event) {
	if (controlsActive && event->type() == QEvent::MouseMove) {
		auto *currWindow = app.focusWindow();
		if (!currWindow) return;

		QPoint center = app.focusWindow()->geometry().center();
		center		  = currWindow->mapToGlobal(center);
		QPointF delta = event->globalPosition() - center;

		const float sensitivity = 0.001f;
		rotation.y -= delta.x() * sensitivity;
		rotation.x -= delta.y() * sensitivity;

		QCursor::setPos(center);
		updateViewMatrix();
	}
}

void Camera::toggleControls() { controlsActive = !controlsActive; }
