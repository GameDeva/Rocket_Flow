#include "Hero.h"


Hero::Hero()
{
	// shotsFired = vector<Position>();
	shotsAlive = vector<FireBall*>();
	shotsInDiscard = vector<FireBall*>();

	// Gameplay variables
	cratesDestroyed = 0;
	currentHealth = maxHealth;
	currentShotsRemaining = maxNumberOfShots;
}


Hero::~Hero()
{
}

// Initialise hero object
void Hero::Initialise()
{


}

void Hero::Update(float deltaTime, float m_t, CCatmullRom &catmul)
{
	// Update player movement
	CalculateMovement(deltaTime);

	// Update Shots
	UpdateShots(deltaTime, m_t, catmul);

	// Hero
	catmul.SuperTNBMaker(position, rotationAngle, m_t);
	sideMovement = 0.f;

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
		shotsAlive.push_back(new FireBall(m_t, rotationAngle, p));
		currentShotsRemaining--;
		// Reset shot timer
		currentShotTimer = 0.f;
	}

	// Alive shots
	for (int i = 0; i < shotsAlive.size();)
	{
		if (shotsAlive[i]->Alivetimer >= shotTimeBeforeDecay)
		{
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
		if (shotsInDiscard[i]->Alivetimer >= shotTimeForDiscardEffect)
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

void Hero::OnCrateDestroy()
{
	cratesDestroyed++;
}

void Hero::OnGemCollect()
{


}

void Hero::OnTakeDamage(int damageValue)
{
	currentHealth -= damageValue;
}
