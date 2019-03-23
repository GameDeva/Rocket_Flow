#pragma once
#include "Common.h"
#include "CCatmullRom.h"
#include "Camera.h"
#include "Helper.h"

class FireBall
{
private:
	const float moveSpeed = 2.f;

public:
	FireBall(float startTime, float rotationAngle, Position position) : sampleTime(startTime), _rotationAngle(rotationAngle), _position(position) { Alivetimer = 0.f; }
	~FireBall() {}

	Position _position;
	float _rotationAngle;
	float sampleTime;

	float Alivetimer;

	void Update(float deltaTime, CCatmullRom &catmul)
	{
		sampleTime += (float)deltaTime * 0.1 * moveSpeed;
		Alivetimer += (float)deltaTime * 0.001;

		_position._point += _position._T * (float)deltaTime * 0.1f * moveSpeed;

		// catmul.SuperTNBMaker(_position, _rotationAngle, sampleTime);
	}

};

//void FireBall::Update(float deltaTime, CCatmullRom &catmul)
//{
//	sampleTime += (float)deltaTime * 0.001 * moveSpeed;
//	Alivetimer += (float)deltaTime * 0.001;
//
//	catmul.SuperTNBMaker(position, _rotationAngle, sampleTime);
//
//}