#include "Game.h"

#include <wglm/gtx/euler_angles.hpp>

glm::quat Look::getQuat() {
	return glm::vec3(this->roll, this->pitch, this->yaw);
}

void Look::changePitch(float d) {
	this->pitch += d;
	this->pitch = glm::clamp(this->pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);
}

glm::vec3 Look::getDir() {
	return this->getTransform3() * glm::vec3{ 1.0f, 0.0f, 0.0f };
}

void Look::changeYaw(float d) {
	this->yaw += d;
	if (this->yaw > glm::two_pi<float>()) {
		this->yaw -= glm::two_pi<float>();
	}
	else if (this->yaw < -glm::two_pi<float>()) {
		this->yaw += glm::two_pi<float>();
	}
}

void Look::changeRoll(float d) {
	getQuat();
	this->roll += d;
	if (this->roll > glm::two_pi<float>()) {
		this->roll -= glm::two_pi<float>();
	}
	else if (this->roll < -glm::two_pi<float>()) {
		this->roll += glm::two_pi<float>();
	}
}

glm::mat4 Look::getTransform() {
	return glm::eulerAngleZ(this->yaw) * glm::eulerAngleY(this->pitch) * glm::eulerAngleX(this->roll);
}

glm::mat3 Look::getTransform3() {
	return this->getTransform();
}
