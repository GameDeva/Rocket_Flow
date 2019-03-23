#pragma once
#include "Common.h"
#include "CCatmullRom.h"
#include "Camera.h"
#include "Helper.h"

class FireBall
{
private:
	const float moveSpeed = 2.f;
	const float curveTime = 0.2f;

public:
	FireBall(float startTime, float rotationAngle, Position position) : sampleTime(startTime), _rotationAngle(rotationAngle), _position(position) { Alivetimer = 0.f; discard = Discard(3.f, false); }
	~FireBall() {}

	Discard discard;
	Position _position;
	float _rotationAngle;
	float sampleTime;

	float Alivetimer;

	void Update(float deltaTime, CCatmullRom &catmul)
	{
		sampleTime += (float)deltaTime * 0.1 * moveSpeed;
		Alivetimer += (float)deltaTime * 0.001;

		// Curves along track for a given time then flys off in tangent
		if (Alivetimer < curveTime)
		{
			catmul.SuperTNBMaker(_position, _rotationAngle, 25.f, sampleTime);
		}
		else {
			_position._point += _position._T * (float)deltaTime * 0.1f * moveSpeed;
		}
	
		discard.update(deltaTime);
	}

};