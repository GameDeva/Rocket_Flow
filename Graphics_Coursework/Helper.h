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

struct Discard {
	float currentTime;
	float _speed;
	bool _isReverse;

	bool shouldDiscard;

	Discard() {}
	Discard(float speed, bool isReverse) : _speed(speed), _isReverse(isReverse)
	{
		shouldDiscard = false;
		currentTime = 0.f;
	}

	void update(float deltaTime)
	{
		if (shouldDiscard)
		{
			if (currentTime >= 1.f) {
				shouldDiscard = false;
				currentTime = 0.f;
			}
			else {
				currentTime += (float)deltaTime * 0.001f * _speed;
			}
		}

	}

	// Reset to beginning for testing
	void Start()
	{
		shouldDiscard = true;
		currentTime = 0.f;
	}

};


const float pi = 3.14159f;
