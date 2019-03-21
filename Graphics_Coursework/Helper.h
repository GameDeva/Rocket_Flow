#pragma once
#include "./include/glm/gtx/rotate_vector.hpp"

struct Position {
	glm::vec3 _point;
	glm::vec3 _T;
	glm::vec3 _N;
	glm::vec3 _B;

	Position() {}
	Position(glm::vec3 point, glm::vec3 T, glm::vec3 N, glm::vec3 B) : _point(point), _T(T), _N(N), _B(B) {}

	glm::mat4 getOrientation() {
		return glm::mat4(glm::mat3(_T, _B, _N));
	}

};

const float pi = 3.14159f;
