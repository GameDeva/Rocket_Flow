#pragma once
#include "Common.h"
#include "CCatmullRom.h"
#include "Camera.h"
#include "Helper.h"
#include "FireBall.h"

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

	void OnTakeDamage(int damageValue);
	void OnCrateDestroy();
	void OnGemCollect();

	const int maxHealth = 100;
	int currentHealth;

	const int maxNumberOfShots = 30;
	int currentShotsRemaining;

	// Camera Related
	CameraSetting cameraSetting;
	
	Position position;
	int cratesDestroyed;

	const vector<FireBall*> & getShotsAliveList() { return shotsAlive; }
	const vector<FireBall*> & getShotsInDiscard() { return shotsInDiscard; }

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


	// Shots related
	vector<FireBall *> shotsAlive;
	vector<FireBall *> shotsInDiscard;

	float shotTimeBeforeDecay = 3.f;
	float shotTimeForDiscardEffect = 3.5f; // Includes timebeforedecay

	float currentShotTimer = 0.f;
	float timeBetweenShots = 0.2f; // Time between key presses to shoot
	//

	void CalculateMovement(float deltaTime);
	void UpdateShots(float deltaTime, float m_t, CCatmullRom &catmul);
};

