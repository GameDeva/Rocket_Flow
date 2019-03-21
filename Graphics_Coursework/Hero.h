#pragma once
#include "Common.h"
#include "CCatmullRom.h"
#include "Camera.h"
#include "Helper.h"

class Hero
{
public:
	enum CameraSetting
	{
		First, Third, Top, SideRight, SideLeft, Freeview
	};

	enum Movement
	{
		None, Left, Right
	};


	Hero();
	~Hero();

	void Initialise();
	void Update(float deltaTime, float m_t, CCatmullRom &catmul);
	void UpdateCamera(float deltaTime, CCamera &camera);

	void OnCrateDestroy();
	void OnGemCollect();


	// Camera Related
	CameraSetting cameraSetting;
	
	Position position;
	int cratesDestroyed;


private:

	float m_cameraMovepseed;
	float m_cameraRotation;
	//

	// Movement related
	//
	float rotationAngle = 0.f;
	float rotationSpeed = 0.05f;

	float sideFriction = 0.9f;
	float sideMovement;
	float sideVelocity;
	float sideAcceleration = 500.f;

	float maxSideMoveSpeed = 2.f;



	void CalculateMovement(float deltaTime);
	
};

