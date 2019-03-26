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

	const float moveSpeed = 150.f;

	const int maxHealth = 100;
	int currentHealth;

	bool shouldRender;

	const int maxNumberOfShots = 30;
	int currentShotsRemaining;

	// Camera Related
	CameraSetting cameraSetting;
	
	Discard discard;
	Position position;
	int cratesDestroyed;

	const vector<FireBall*> & getShotsAliveList() { return shotsAlive; }
	const vector<FireBall*> & getShotsInDiscard() { return shotsInDiscard; }
	const vector<LightInfo*> & getspotLights() { return spotLights; }

	// IMPORTANT: when changing this, change size of array in mainShader.frag, since array cannot be dynamic
	const int spotLightCount = 8;

	LightInfo *dayLight;

private:

	float m_cameraMovepseed;
	float m_cameraRotation;
	//

	// Movement related
	//
	const float radialDistance = 25.f;
	float rotationAngle = 0.f;
	float rotationSpeed = 0.05f;

	float sideFriction = 0.9f;
	float sideMovement;
	float sideVelocity;
	float sideAcceleration = 500.f;

	float maxSideMoveSpeed = 2.f;

	vector<LightInfo*> spotLights;

	// Shots related
	vector<FireBall *> shotsAlive;
	vector<FireBall *> shotsInDiscard;

	float shotTimeBeforeDecay = 1.f;
	float shotTimeForDiscardEffect = 1.5f; // Includes timebeforedecay

	float currentShotTimer = 0.f;
	float timeBetweenShots = 0.2f; // Time between key presses to shoot
	//

	bool invulnerable;
	const float invulnerabilityMaxTime = 2.f;
	float currentInvulnerabilityTimer;
	const float flashPeriod = 0.2f;

	void CalculateMovement(float deltaTime);
	void UpdateShots(float deltaTime, float m_t, CCatmullRom &catmul);

	const float distanceBetweenSpotLights = 40.f;
	void InitialiseSpotLights();
	void UpdateSpotLights();
};

