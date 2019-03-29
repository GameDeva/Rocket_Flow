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


struct LightInfo
{
	glm::vec4 _position;
	glm::vec3 _lightAmb;
	glm::vec3 _lightDiff;
	glm::vec3 _lightSpec;
	glm::vec3 _direction;
	float _exponent;
	float _cutoff;

	LightInfo(glm::vec4 position, glm::vec3 lightAmb, glm::vec3 lightDiff, glm::vec3 lightSpec, glm::vec3 direction, float exponent, float cutoff) : _position(position), _lightAmb(lightAmb), _lightDiff(lightDiff), _lightSpec(lightSpec), _direction(direction), _exponent(exponent), _cutoff(cutoff) {}

	void UpdateLight(glm::vec4 position, glm::vec3 lightAmb, glm::vec3 lightDiff, glm::vec3 lightSpec, glm::vec3 direction, float exponent, float cutoff)
	{
		_position = position;
		_lightAmb = lightAmb;
		_lightDiff = lightDiff; 
		_lightSpec = lightSpec;
		_direction = direction; 
		_exponent = exponent;
		_cutoff = cutoff;
	}


};

enum GameState { START, GAMEPLAY, WIN, LOSE };


const float pi = 3.14159f;
