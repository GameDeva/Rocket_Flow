#include "Hero.h"


Hero::Hero()
{
	cameraSetting = Third;

	shotsAlive = vector<MovingObject*>();
	shotsInDiscard = vector<MovingObject*>();

	spotLights = vector<LightInfo*>();
	InitialiseSpotLights();

	dayLight = new LightInfo(glm::vec4(-100, 100, -100, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec3(0.f, -1.f, 0.f), 1.f, 15.f);
	nightLight = new LightInfo(glm::vec4(-100, 100, -100, 1), glm::vec3(0.5), glm::vec3(0.5), glm::vec3(0.5), glm::vec3(0.f, -1.f, 0.f), 1.f, 15.f);

	discard = Discard(2.f, true);
}


Hero::~Hero()
{
	if (shotsAlive.size() != 0)
	{
		for (int i = 0; i < shotsAlive.size(); i++)
		{
			delete shotsAlive[i];
		}
	}

	if (shotsInDiscard.size() != 0)
	{
		for (int i = 0; i < shotsInDiscard.size(); i++)
		{
			delete shotsInDiscard[i];
		}
	}

	if (spotLights.size() != 0)
	{
		for (int i = 0; i < spotLights.size(); i++)
		{
			delete spotLights[i];
		}
	}

	delete dayLight;
	delete nightLight;
}

// Initialise hero object
void Hero::Initialise()
{
	dead = false;
	rotationAngle = 0.f;
	discard.Start();

	// shotsFired = vector<Position>();
	if (shotsAlive.size() != 0)
	{
		for (int i = 0; i < shotsAlive.size(); i++)
		{
			delete shotsAlive[i];
		}
	}

	if (shotsInDiscard.size() != 0)
	{
		for (int i = 0; i < shotsInDiscard.size(); i++)
		{
			delete shotsInDiscard[i];
		}
	}

	shouldRender = true;

	// Gameplay variables
	score = 0;
	currentHealth = maxHealth;
	currentShotsRemaining = maxNumberOfShots;
	invulnerable = false;
	currentInvulnerabilityTimer = 0.f;
}

void Hero::Update(float deltaTime, float m_t, CCatmullRom &catmul)
{
	// Discard into appearance
	discard.update(deltaTime);

	// Update player movement
	CalculateMovement(deltaTime);

	// Placement
	catmul.SuperTNBMaker(position, rotationAngle, radialDistance, m_t);
	sideMovement = 0.f;

	// Invulnerbility
	if (invulnerable)
	{
		if (currentInvulnerabilityTimer >= invulnerabilityMaxTime)
		{
			// Reset values
			currentInvulnerabilityTimer = 0.f;
			invulnerable = false;
			shouldRender = true;
		}
		else
		{
			currentInvulnerabilityTimer += (float)deltaTime * 0.001;
			glm::sin((currentInvulnerabilityTimer / flashPeriod) * pi) > 0.f ? shouldRender = true : shouldRender = false;
		}
	}
	
	UpdateSpotLights(m_t, catmul);

	// Update Shots
	UpdateShots(deltaTime, m_t, catmul);

}

void Hero::UpdateCamera(float deltaTime, CCamera &camera)
{
	switch (cameraSetting)
	{
	case Freeview:
		// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
		camera.Update(deltaTime);
		break;
	case First:
		// First Person View
		camera.Set(position._point + position._B + position._T * 5.5f, position._point + position._B + position._T * 10.f, position._B);
		// UpdateCameraPosition(playpos, playpos + T * 10.f, v);
		break;
	case Third:
		// Third Person View
		camera.Set(position._point + 30.f  * position._B - 50.f * position._T, position._point + position._T * 10.f, position._B);
		// UpdateCameraPosition(playpos + 10.f  * B - 30.f * T, playpos + T * 10.f, playpos);
		break;
	case Top:
		// Top View
		camera.Set(position._point + 70.f  * position._B, position._point + position._T * 1.f, position._B);
		// UpdateCameraPosition(p + 20.f  * B, pNext, p + 0.1f * T);
		break;
	case SideLeft:
		// Side View
		camera.Set(position._point + 30.f  * position._B + 70.f *position._N, position._point + position._T * 1.f, position._B);
		// UpdateCameraPosition(p + 20.f  * B, pNext, p + 0.1f * T);
		break;
	case SideRight:
		// Side View
		camera.Set(position._point + 30.f  * position._B - 70.f * position._N, position._point + position._T * 1.f, position._B);
		// UpdateCameraPosition(p + 20.f  * B, pNext, p + 0.1f * T);
		break;
	}
}

void Hero::CalculateMovement(float deltaTime)
{
	if (GetKeyState(VK_LEFT) & 0x80) {
		sideMovement = 1.f;
	}

	if (GetKeyState(VK_RIGHT) & 0x80) {
		sideMovement = -1.f;
	}

	// 
	sideVelocity += sideMovement * sideAcceleration * (float)deltaTime * 0.001;
	sideVelocity *= sideFriction;

	// Make sure the player doesnt rotate too fast
	sideVelocity = glm::clamp(sideVelocity, -maxSideMoveSpeed, maxSideMoveSpeed);

	// Update the player angle 
	rotationAngle += sideVelocity * (float)deltaTime * 0.001;
	// playerRotationAngle = glm::round(playerRotationAngle);

}

void Hero::UpdateShots(float deltaTime, float m_t, CCatmullRom &catmul)
{
	currentShotTimer += (float)deltaTime * 0.001;
	if ((GetKeyState(VK_SPACE) & 0x80) && currentShotTimer > timeBetweenShots && currentShotsRemaining > 0) {
		// Add new shot position
		Position p = position;
		p._point += p._T * 2.f;
		shotsAlive.push_back(new MovingObject(m_t, rotationAngle, p, fireBallMoveSpeed, fireBallCurvetime));
		currentShotsRemaining--;
		// Reset shot timer
		currentShotTimer = 0.f;
	}

	// Alive shots
	for (int i = 0; i < shotsAlive.size();)
	{
		if (shotsAlive[i]->Alivetimer >= shotTimeBeforeDecay)
		{
			// Begin Discard 
			shotsAlive[i]->discard.Start();

			// Add to vectorlist of discarding shots
			shotsInDiscard.push_back(shotsAlive[i]);

			// Erase from the alive Shots
			shotsAlive.erase(shotsAlive.begin() + i);
		} 
		else
		{
			shotsAlive[i]->Update(deltaTime, catmul);
			++i;
		}
	}

	// Discarding shots
	for (int i = 0; i < shotsInDiscard.size();)
	{
		// If time is greater than discard time, then erase it from 
		if (!shotsInDiscard[i]->discard.shouldDiscard) // shotsInDiscard[i]->Alivetimer >= shotTimeForDiscardEffect
		{
			delete (shotsInDiscard[i]);
			// Erase from the alive Shots
			shotsInDiscard.erase(shotsInDiscard.begin() + i);

		}
		else 
		{
			shotsInDiscard[i]->Update(deltaTime, catmul);
			++i;
		}
	}

}

// Initialise Spotlights to arbitary position 
void Hero::InitialiseSpotLights()
{
	float gValue = 0.f;
	for (int i = 0; i < spotLightCount; i++)
	{
		spotLights.push_back(new LightInfo(glm::vec4(1), glm::vec3(1), glm::vec3(0.5f, 1.f, 0.f), glm::vec3(1.f, 0.5f, 0.f), glm::vec3(1), 150.f, 10.f));
		gValue += 1.0f / spotLightCount;
	}
}

// Update spotlights based on player position
void Hero::UpdateSpotLights(float m_t, CCatmullRom &catmul)
{
	Position position = Position();
	float j = 0;
	for (int i = 0; i < spotLights.size(); i++)
	{
		catmul.SuperTNBMaker(position, rotationAngle, radialDistance - 200, m_t + j);
		// glm::vec3 pos = position._point - position._B * 100.f + position._T * j;

		spotLights[i]->_position = glm::vec4(position._point, 1);
		spotLights[i]->_direction = - position._B; // (position._point + position._T * j) - pos;

		j += distanceBetweenSpotLights;
	}
}

void Hero::OnCrateDestroy()
{
	score++;
}

void Hero::OnGemCollect()
{


}

void Hero::OnTakeDamage(int damageValue)
{
	// Take damage if not invulnerable
	if (!invulnerable)
	{
		currentHealth -= 50;
		if (currentHealth <= 0)
			dead = true;
		invulnerable = true;
	}
}