#pragma once
#include "Common.h"
#include "CCatmullRom.h"
#include "Camera.h"
#include "Helper.h"

class MovingObject
{
private:
	float _moveSpeed;
	float _curveTime;

public:
	MovingObject(float startTime, float rotationAngle, Position position, float moveSpeed, float curveTime) : sampleTime(startTime), _rotationAngle(rotationAngle), _position(position), _moveSpeed(moveSpeed), _curveTime(curveTime) { Alivetimer = 0.f; discard = Discard(3.f, false); }
	~MovingObject() {}

	Discard discard;
	Position _position;
	float _rotationAngle;
	float sampleTime;

	float Alivetimer;

	void Update(float deltaTime, CCatmullRom &catmul)
	{
		sampleTime += (float)deltaTime * 0.1 * _moveSpeed;
		Alivetimer += (float)deltaTime * 0.001;

		// Curves along track for a given time then flys off in tangent
		if (Alivetimer < _curveTime)
		{
			catmul.SuperTNBMaker(_position, _rotationAngle, 25.f, sampleTime);
		}
		else {
			_position._point += _position._T * (float)deltaTime * 0.1f * _moveSpeed;
		}
	
		discard.update(deltaTime);
	}

};