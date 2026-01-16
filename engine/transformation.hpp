#pragma once
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <iostream>

namespace beamcast {
/**
 * @brief Position, Rotation and Scale in 3d space
 */
class Transformation {
	glm::mat4x4 worldMatrix;

   public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transformation();
	Transformation(const glm::vec3 &position);
	Transformation(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);
	Transformation(glm::mat4 mat);

	glm::mat4 &getWorldMatrix();
	void	   updateWorldMatrix();
	void	   updateVectors();

	static bool decomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation,
								   glm::vec3 &scale);

	bool operator==(const Transformation &other);

	friend std::ostream &operator<<(std::ostream &os, const Transformation &rhs);

	glm::vec3 forward();
};
}	  // namespace beamcast
