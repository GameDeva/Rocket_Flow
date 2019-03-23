#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CCube;
class CTetra;
class Hero;
struct Position;
struct Discard;

class Game {
private:

	// const float pi = 3.14159f;
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void CheckForCollisions();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCube *m_pCube;
	CTetra * m_pGemTetra;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBugShipMesh;
	COpenAssetImportMesh *m_pSkullMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_catmull;
	Hero *m_phero;
	CCamera * m_pcamera;

	// Some other member variables
	double deltaTime;
	int m_framesPerSecond;
	bool m_appActive;

	// Camera spline points
	glm::vec3 p0 = glm::vec3(-500, 10, -200);
	glm::vec3 p1 = glm::vec3(0, 10, -200);
	glm::vec3 p2 = glm::vec3(0, 10, 200);
	glm::vec3 p3 = glm::vec3(-500, 10, 200);

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();
	

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void DisplayHUD();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	float m_t;

	float xCurrentPos;

	const float trackWidth = 40.f;
	const int trackSampleSize = 300;

	const int crateDamageValue = 10;
	const int skullDamageValue = 20;
	
	vector<Position> cratePositions;
	vector<Position> gemPositions;

	void RenderWater();

};
