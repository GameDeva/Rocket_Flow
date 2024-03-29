/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "Game.h"

// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CCatmullRom.h"
#include "CCube.h"
#include "CTetra.h"
#include "Hero.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBugShipMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_catmull = NULL;
	m_pCube = NULL;
	m_phero = NULL;
	m_pcamera = NULL;
	m_enemyShip - NULL;
	m_pTree = NULL;

	deltaTime = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBugShipMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_catmull;
	delete m_pCube;
	delete m_pMill;
	delete m_pGemTetra;
	delete m_phero;
	delete m_pcamera;
	delete m_enemyShip;
	delete m_pTree;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;
	
	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBugShipMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_catmull = new CCatmullRom(trackWidth);
	m_pCube = new CCube;
	m_pMill = new COpenAssetImportMesh;
	m_pGemTetra = new CTetra;
	m_phero = new Hero;
	m_pcamera = new CCamera;
	m_enemyShip = new COpenAssetImportMesh;
	m_pTree = new COpenAssetImportMesh;
	
	RECT dimensions = m_gameWindow.GetDimensions();
	// m_gameWindow.SetFullscreen();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pcamera->SetOrthographicProjectionMatrix(width, height); 
	m_pcamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.geom");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("circularWaterShader.vert");
	sShaderFileNames.push_back("circularWaterShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->AddShaderToProgram(&shShaders[2]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->AddShaderToProgram(&shShaders[4]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create a shader program for Water
	CShaderProgram *pCircularWaterProgram = new CShaderProgram;
	pCircularWaterProgram->CreateProgram();
	pCircularWaterProgram->AddShaderToProgram(&shShaders[5]);
	pCircularWaterProgram->AddShaderToProgram(&shShaders[6]);
	pCircularWaterProgram->LinkProgram();
	m_pShaderPrograms->push_back(pCircularWaterProgram);

	// Spline work
	m_catmull->CreateCentreline();
	m_catmull->CreateOffsetCurves();
	m_catmull->CreateTrack();

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Cube creation
	m_pCube->Create("resources\\textures\\scifibox.jpg");

	// Gem creation
	m_pGemTetra->Create("resources\\textures\\greenCrystal.jpg");

	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "Tile41a.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("consola.ttf", 128);
	// 	m_pFtFont->LoadFont("resources\\fonts\\airstrike.ttf", 256);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBugShipMesh->Load("resources\\models\\orbiter bugship\\orbiter bugship.obj"); // Downloaded from turbosquid
	m_pMill->Load("resources\\models\\Mill\\low-poly-mill.obj");// ^^
	m_enemyShip->Load("resources\\models\\Trident\\Trident-A10.obj");// ^^
	m_pTree->Load("resources\\models\\Tree\\Lowpoly_tree_sample.obj"); // ^^

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "fireball.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("Resources\\Audio\\Dreams_Instru.mp3");	// J Cole - Dreams Instrumental
	m_pAudio->PlayMusicStream();
	
	// Create list of enemies
	enemiesAlive = vector<MovingObject*>();

	// Tree positions
	for (int i = 0; i < 10; i++)
	{
		treePoints.push_back(glm::vec3(i * 80 + 50, i * 30 + (randfloat() * 20 - 50), i * 30 - 50));
	}
	

	// After all the things are done, which don't need to be done on game restart
	// Set up the level to start the game
	SetupLevel();
	SetTitleScreen();

	state = START;
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bExplodeObject", false);
	pMainProgram->SetUniform("explodeFactor", 0.f);
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	
	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pcamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pcamera->GetPosition(), m_pcamera->GetView(), m_pcamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pcamera->ComputeNormalMatrix(viewMatrix);

	pMainProgram->SetUniform("nightMode", nightMode);
	if (nightMode)
	{
		pMainProgram->SetUniform("lightCount", m_phero->spotLightCount);

		// Initalise all lights
		vector<LightInfo*> spotLights = m_phero->getspotLights();
		if (spotLights.size() != 0)
		{
			for (int i = 0; i < spotLights.size(); i++)
			{
				AddLight(pMainProgram, to_string(i), viewMatrix, viewNormalMatrix, *spotLights[i]);
			}
		}

		pMainProgram->SetUniform("nightLight.position", viewMatrix * m_phero->nightLight->_position); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("nightLight.La", m_phero->nightLight->_lightAmb);		// Ambient colour of light
		pMainProgram->SetUniform("nightLight.Ld", m_phero->nightLight->_lightDiff);		// Diffuse colour of light
		pMainProgram->SetUniform("nightLight.Ls", m_phero->nightLight->_lightSpec);		// Specular colour of light
		pMainProgram->SetUniform("nightLight.direction", glm::normalize(viewNormalMatrix*m_phero->nightLight->_direction));		// Direction of spot light
		pMainProgram->SetUniform("nightLight.exponent", m_phero->nightLight->_exponent);		// Exponent of spot light
		pMainProgram->SetUniform("nightLight.cutoff", m_phero->nightLight->_cutoff);		// Cutoff of spot light

		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance
		pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	}
	else {
		// AddLight(pMainProgram, to_string(0), viewMatrix, viewNormalMatrix, *m_phero->dayLight);

		pMainProgram->SetUniform("dayLight.position", viewMatrix * m_phero->dayLight->_position); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("dayLight.La", m_phero->dayLight->_lightAmb);		// Ambient colour of light
		pMainProgram->SetUniform("dayLight.Ld", m_phero->dayLight->_lightDiff);		// Diffuse colour of light
		pMainProgram->SetUniform("dayLight.Ls", m_phero->dayLight->_lightSpec);		// Specular colour of light
		pMainProgram->SetUniform("dayLight.direction", glm::normalize(viewNormalMatrix*m_phero->dayLight->_direction));		// Direction of spot light
		pMainProgram->SetUniform("dayLight.exponent", m_phero->dayLight->_exponent);		// Exponent of spot light
		pMainProgram->SetUniform("dayLight.cutoff", m_phero->dayLight->_cutoff);		// Cutoff of spot light

		pMainProgram->SetUniform("material1.Ma", glm::vec3(1.f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance	
		pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	}
	
	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pcamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit, nightMode);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	/*modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();
*/

	// Render the crate
	int size = cratePositions.size();
	for(int i = 0; i < size; i++) 
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(cratePositions[i]._point);
		modelViewMatrixStack *= cratePositions[i].getOrientation();
		modelViewMatrixStack.Rotate(cratePositions[i]._N, m_t);
		modelViewMatrixStack.Rotate(cratePositions[i]._T, m_t);
		modelViewMatrixStack.Scale(crateRenderSize);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pCube->Render();
		modelViewMatrixStack.Pop();
	}
	
	// Gem positions
	size = gemPositions.size();
	for (int i = 0; i < size; i++)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(gemPositions[i]._point);
		modelViewMatrixStack *= gemPositions[i].getOrientation();
		modelViewMatrixStack.Rotate(gemPositions[i]._N, m_t);
		modelViewMatrixStack.Rotate(gemPositions[i]._T, m_t);
		modelViewMatrixStack.Scale(gemRenderSize);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pGemTetra->Render();
		modelViewMatrixStack.Pop();
	}

	// Enemy positions
	if (enemiesAlive.size() != 0)
	{
		for (vector<MovingObject*>::iterator it = enemiesAlive.begin(); it != enemiesAlive.end();)
		{
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate((*it)->_position._point);
			modelViewMatrixStack *= (*it)->_position.getOrientation();
			modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), 180.f);
			modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), 90.f);
			modelViewMatrixStack.Scale(enemyRenderSize);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_enemyShip->Render();
			modelViewMatrixStack.Pop();

			++it;
		}
	}
	
	// Trees
	for (int i = 0; i < treePoints.size(); i++)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(treePoints[i]);
		// modelViewMatrixStack *= gemPositions[i].getOrientation();
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), i * 80);
		// modelViewMatrixStack.Rotate(gemPositions[i]._T, m_t);
		modelViewMatrixStack.Scale(treeRenderSize);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTree->Render();
		modelViewMatrixStack.Pop();
	}

	// The mill
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-250, -200, -250));
	// modelViewMatrixStack *= m_phero->position.getOrientation();
	// modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), 180.f);
	modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), 180.f);
	modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), 180.f);
	modelViewMatrixStack.Scale(100);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pMill->Render();
	modelViewMatrixStack.Pop();

	//  The space ship
	if (m_phero->shouldRender)
	{
		if (m_phero->discard.shouldDiscard)
		{
			pMainProgram->SetUniform("applyDiscard", true);
			pMainProgram->SetUniform("discardValue", m_phero->discard._isReverse ? 1.f - m_phero->discard.currentTime : m_phero->discard.currentTime);
		}

		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_phero->position._point);
		modelViewMatrixStack *= m_phero->position.getOrientation();
		// modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), 180.f);
		modelViewMatrixStack.Rotate(glm::vec3(0, 0, 1), 180.f);
		modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), 180.f);
		modelViewMatrixStack.Scale(heroShipRenderSize);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBugShipMesh->Render();
		modelViewMatrixStack.Pop();

		pMainProgram->SetUniform("applyDiscard", false);
	}

	vector<MovingObject*> shotsAliveList = m_phero->getShotsAliveList();
	if (shotsAliveList.size() != 0)
	{
		for (vector<MovingObject*>::iterator it = shotsAliveList.begin(); it != shotsAliveList.end();)
		{
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate((*it)->_position._point);
			modelViewMatrixStack *= (*it)->_position.getOrientation();
			modelViewMatrixStack.Scale(fireBallRenderSize);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pSphere->Render();
			modelViewMatrixStack.Pop();

			++it;
		}
	}

	vector<MovingObject*> shotsDiscardList = m_phero->getShotsInDiscard();
	if (shotsDiscardList.size() != 0)
	{
		pMainProgram->SetUniform("applyDiscard", true);

		for (vector<MovingObject*>::iterator it = shotsDiscardList.begin(); it != shotsDiscardList.end();)
		{
			// Do not render discard shots that have completed discarding
			if (!(*it)->discard.shouldDiscard) {
				++it;
				continue;
			}

			pMainProgram->SetUniform("discardValue", (*it)->discard._isReverse ? 1.f - (*it)->discard.currentTime : (*it)->discard.currentTime);

			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate((*it)->_position._point);
			modelViewMatrixStack *= (*it)->_position.getOrientation();
			modelViewMatrixStack.Scale(fireBallRenderSize);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pSphere->Render();
			modelViewMatrixStack.Pop();

			++it;
		}
		pMainProgram->SetUniform("applyDiscard", false);
	}

	// Render exploding objects
	pMainProgram->SetUniform("bExplodeObject", true);
	if (explodingCratePositionTimes.size() != 0)
	{
		int size = explodingCratePositionTimes.size();
		for (int i = 0; i < size; i++)
		{
			pMainProgram->SetUniform("explodeFactor", explodingCratePositionTimes[i].second);
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(explodingCratePositionTimes[i].first._point);
			modelViewMatrixStack *= explodingCratePositionTimes[i].first.getOrientation();
			modelViewMatrixStack.Scale(crateRenderSize);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pSphere->Render();
			modelViewMatrixStack.Pop();
		}
	}
	if (explodingEnemyPositionTimes.size() != 0)
	{
		int size = explodingEnemyPositionTimes.size();
		for (int i = 0; i < size; i++)
		{
			pMainProgram->SetUniform("explodeFactor", explodingEnemyPositionTimes[i].second);
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(explodingEnemyPositionTimes[i].first._point);
			modelViewMatrixStack *= explodingEnemyPositionTimes[i].first.getOrientation();
			modelViewMatrixStack.Scale(enemyRenderSize);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pSphere->Render();
			modelViewMatrixStack.Pop();
		}
	}
	pMainProgram->SetUniform("bExplodeObject", false);

	// Spline Render
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// m_catmull->RenderCentreline();
	// m_catmull->RenderOffsetCurves();
	m_catmull->RenderTrack();
	modelViewMatrixStack.Pop();

	RenderWater();

	// Render on start screen
	if (state == START)
	{
		Game::Display3DText("Rocket Flow", 120, glm::vec3(450, 100, -100), nightMode ? nightTitleColour : titleColour, modelViewMatrixStack);
		Game::Display3DText("Space to Begin!", 30, glm::vec3(350, 70, -100), nightMode ? nightTitleColour : titleColour, modelViewMatrixStack);
	}
	else {
		// Draw the 2D graphics after the 3D graphics
		DisplayHUD();
	}

	if (state == WIN)
	{
		DisplayEndGameText();

	}

	if (state == LOSE)
	{
		DisplayEndGameText();

	}

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	// Increase sample rate 
	m_t += m_phero->moveSpeed * 0.001f* (float)deltaTime;
	// titleMoveTimer = sin(deltaTime);
	titleMoveTimer += titleMoveSpeed * 0.001f * (float)deltaTime;
	MoveTitleCamera();
	UpdateEnemyPositions();

	switch (state)
	{
	case START:
		SetTitleScreen();
		// Set camera to single position
		if (GetKeyState(VK_SPACE) & 0x80)
		{
			// Begin game
			StartGame();
			m_phero->UpdateCamera(deltaTime, *m_pcamera);
		}
		break;
	case GAMEPLAY:
		// If hero is alive
		if (!m_phero->dead)
		{
			if (enemiesAlive.size() <= 0 && cratePositions.size() < 0)
			{
				OnWinGame();
			}

			// Update the hero
			m_phero->Update(deltaTime, m_t, *m_catmull);
			m_phero->UpdateCamera(deltaTime, *m_pcamera);

		}
		else {
			OnLoseGame();
		}
		break;
	case WIN:
		if (GetKeyState(VK_SPACE) & 0x80)
		{
			// Begin game
			SetupLevel();
			StartGame();
		}
		else if (GetKeyState(VK_SHIFT) & 0x80)
		{
			SetupLevel();
			SetTitleScreen();
			state = START;
		}
		break;
	case LOSE:
		if (GetKeyState(VK_SPACE) & 0x80)
		{
			// Begin game
			SetupLevel();
			StartGame();
		}
		else if (GetKeyState(VK_SHIFT) & 0x80)
		{
			SetupLevel();
			SetTitleScreen();
			state = START;
		}
		break;
	default:
		break;
	}

	// Update exploding object timers
	for (int i = 0; i < explodingCratePositionTimes.size(); i++)
	{
		explodingCratePositionTimes[i].second += explodeSpeed * 0.001f * (float)deltaTime;
		if (explodingCratePositionTimes[i].second < maxExplodeTimer)
		{
			explodingCratePositionTimes.erase(explodingCratePositionTimes.begin() + i);
		}
	}
	// Update exploding object timers
	for (int i = 0; i < explodingEnemyPositionTimes.size(); i++)
	{
		explodingEnemyPositionTimes[i].second += explodeSpeed * 0.001f *  (float)deltaTime;
		if (explodingEnemyPositionTimes[i].second < maxExplodeTimer)
		{
			explodingEnemyPositionTimes.erase(explodingEnemyPositionTimes.begin() + i);
		}
	}

	currentNightToggleTimer += (float)deltaTime * 0.001;
	// Night mode toggle
	if (GetKeyState(VK_NUMPAD7) & 0x80 && currentNightToggleTimer > timeBetweenNightToggle)
	{
		nightMode = !nightMode;
		currentNightToggleTimer = 0.f;
	}

	m_pAudio->Update();
}

void Game::MoveTitleCamera()
{
	currentCamTitlePosition = glm::mix(titleCamPosition1, titleCamPosition2, sin(titleMoveTimer));

}

void Game::UpdateEnemyPositions()
{
	// Enemies
	for (vector<MovingObject*>::iterator it = enemiesAlive.begin(); it != enemiesAlive.end(); it++)
	{
		(*it)->Update(deltaTime, *m_catmull);
	}
	// for (int i = 0; i < enemyPositions.size(); i++)
	//{
	//	 enemyPositions[i]._point -= enemyPositions[i]._T * enemyMoveSpeed  * 0.001f * (float)deltaTime;
	//}

}

void Game::CheckForCollisions()
{
	glm::vec3 heroPoint = m_phero->position._point;

	// Crate Collisions
	for (vector<Position>::iterator it = cratePositions.begin(); it != cratePositions.end();)
	{
		bool crateDestroyed = false;

		// Check for collision with hero
		if (glm::distance((*it)._point, heroPoint) < 5.f)
		{
			// Hero takes damage 
			m_phero->OnTakeDamage(crateDamageValue);
			// Destroyed crate is added to list of exploding crate
			explodingCratePositionTimes.push_back(pair<Position, float>(*it, 0.f));
			// Crate removed from this list
			it = cratePositions.erase(it);

			crateDestroyed = true;
		}
		else {
			// Check for collision with each fireball alive
			vector<MovingObject*> shotsAliveList = m_phero->getShotsAliveList();
			if (shotsAliveList.size() != 0)
			{
				for (vector<MovingObject*>::iterator ft = shotsAliveList.begin(); ft != shotsAliveList.end();)
				{
					if (glm::distance((*ft)->_position._point, (*it)._point) < 8.f)
					{
						m_phero->OnPointScore(cratePoint);
						// Destroyed crate is added to list of exploding crate
						explodingCratePositionTimes.push_back(pair<Position, float>(*it, 0.f));
						// Crate removed from this list
						it = cratePositions.erase(it);
						crateDestroyed = true;
						break;
					}
					++ft;
				}
			}
			if(!crateDestroyed)
				++it;
		}
	}

	// Enemy Collisions
	for (vector<MovingObject*>::iterator it = enemiesAlive.begin(); it != enemiesAlive.end();)
	{
		bool enemyDestroyed = false;

		// Check for collision with hero
		if (glm::distance((*it)->_position._point, heroPoint) < 5.f)
		{
			// Hero takes damage 
			m_phero->OnTakeDamage(crateDamageValue);
			// Destroyed crate is added to list of exploding crate
			explodingCratePositionTimes.push_back(pair<Position, float>((*it)->_position, 0.f));
			// Crate removed from this list
			it = enemiesAlive.erase(it);

			enemyDestroyed = true;
		}
		else {
			// Check for collision with each fireball alive
			vector<MovingObject*> shotsAliveList = m_phero->getShotsAliveList();
			if (shotsAliveList.size() != 0)
			{
				for (vector<MovingObject*>::iterator ft = shotsAliveList.begin(); ft != shotsAliveList.end();)
				{
					if (glm::distance((*ft)->_position._point, (*it)->_position._point) < 8.f)
					{
						m_phero->OnPointScore(enemyPoint);
						// Destroyed crate is added to list of exploding crate
						explodingCratePositionTimes.push_back(pair<Position, float>((*it)->_position, 0.f));
						// Crate removed from this list
						it = enemiesAlive.erase(it);
						enemyDestroyed = true;
						break;
					}
					++ft;
				}
			}
			if (!enemyDestroyed)
				++it;
		}
	}


	// Gem collisions
	for (vector<Position>::iterator it = gemPositions.begin(); it != gemPositions.end();)
	{
		if (glm::distance((*it)._point, heroPoint) < 5.f)
		{
			m_phero->OnPointScore(gemPoint);
			it = gemPositions.erase(it);
		}
		else {
			++it;
		}
	}

}

void Game::DisplayHUD()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;


	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pcamera->GetOrthographicProjectionMatrix());
	
	if(nightMode)
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	else 
		fontProgram->SetUniform("vColour", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pFtFont->Render(m_gameWindow.SCREEN_WIDTH - 240, height - 40, 20, "Score: %d", m_phero->score);
	
	m_pFtFont->Render(21, 40, 20, "Fireball Stack: %d", m_phero->currentShotsRemaining);
	m_pFtFont->Render(20, 70, 40, "HEALTH: %d", m_phero->currentHealth);

	// Frame rate
	DisplayFrameRate();
}

void Game::DisplayEndGameText()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;


	// Use the font shader program and render the text
	fontProgram->UseProgram();
	glDisable(GL_DEPTH_TEST);
	fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
	fontProgram->SetUniform("matrices.projMatrix", m_pcamera->GetOrthographicProjectionMatrix());

	if (nightMode)
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	else
		fontProgram->SetUniform("vColour", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	m_pFtFont->Render(m_gameWindow.SCREEN_WIDTH - 900, height - (height / 2), 150, state == WIN ? "SUCESS" : "FAILURE");
	m_pFtFont->Render(m_gameWindow.SCREEN_WIDTH - 850, height - (height / 2) - 50, 50, "Final Score: %d", m_phero->score);

	m_pFtFont->Render(21, 40, 20, "Fireball Stack: %d", m_phero->currentShotsRemaining);
	m_pFtFont->Render(20, 70, 40, "HEALTH: %d", m_phero->currentHealth);

	// Frame rate
	DisplayFrameRate();
}

void Game::Display3DText(string text, int size, glm::vec3 q, glm::vec4 colour, glutil::MatrixStack modelViewMatrixStack)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); 
	glm::vec4 vp(viewport[0], viewport[1], viewport[2], viewport[3]);
	glm::vec3 p = glm::project(q, modelViewMatrixStack.Top(), *(m_pcamera->GetPerspectiveProjectionMatrix()), vp);
	GLfloat depth; glReadPixels((int)p.x, (int)p.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth); 
	float offset = 0.001f; 
	if (p.z - offset < depth) 
	{ 
		CShaderProgram*fontProgram = (*m_pShaderPrograms)[1]; 
		fontProgram->UseProgram(); fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1)); 
		fontProgram->SetUniform("matrices.projMatrix", m_pcamera->GetOrthographicProjectionMatrix()); 
		fontProgram->SetUniform("vColour", colour); 
		m_pFtFont->Render(p.x, p.y, size, text.c_str());
		//fontProgram->SetUniform("vColour", glm::vec4(1, 0, 0, 1));
		//m_pFtFont->Render(p.x+3, p.y+3, size, text.c_str());
	}

}

void Game::DisplayFrameRate()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += deltaTime;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
	{
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
	}

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pcamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(0.1f, 0.9f, 0.0f, 1.0f));
		m_pFtFont->Render(20, height - 15, 17, "FPS: %d", m_framesPerSecond);
	}
}

void Game::RenderWater()
{
	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	CShaderProgram *circularWaterProgram = (*m_pShaderPrograms)[2];

	circularWaterProgram->UseProgram();

	circularWaterProgram->SetUniform("bUseTexture", true);
	circularWaterProgram->SetUniform("sampler0", 0);

	// Set the projection matrix
	circularWaterProgram->SetUniform("matrices.projMatrix", m_pcamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pcamera->GetPosition(), m_pcamera->GetView(), m_pcamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pcamera->ComputeNormalMatrix(viewMatrix);


	circularWaterProgram->SetUniform("nightMode", nightMode);
	if (nightMode)
	{
		circularWaterProgram->SetUniform("lightCount", m_phero->spotLightCount);

		// Initalise all lights
		vector<LightInfo*> spotLights = m_phero->getspotLights();
		if (spotLights.size() != 0)
		{
			for (int i = 0; i < spotLights.size(); i++)
			{
				AddLight(circularWaterProgram, to_string(i), viewMatrix, viewNormalMatrix, *spotLights[i]);
			}
		}

		circularWaterProgram->SetUniform("nightLight.position", viewMatrix * m_phero->nightLight->_position); // Position of light source *in eye coordinates*
		circularWaterProgram->SetUniform("nightLight.La", m_phero->nightLight->_lightAmb);		// Ambient colour of light
		circularWaterProgram->SetUniform("nightLight.Ld", m_phero->nightLight->_lightDiff);		// Diffuse colour of light
		circularWaterProgram->SetUniform("nightLight.Ls", m_phero->nightLight->_lightSpec);		// Specular colour of light
		circularWaterProgram->SetUniform("nightLight.direction", glm::normalize(viewNormalMatrix*m_phero->nightLight->_direction));		// Direction of spot light
		circularWaterProgram->SetUniform("nightLight.exponent", m_phero->nightLight->_exponent);		// Exponent of spot light
		circularWaterProgram->SetUniform("nightLight.cutoff", m_phero->nightLight->_cutoff);		// Cutoff of spot light

		circularWaterProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
		circularWaterProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
		circularWaterProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance
		circularWaterProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	}
	else {
		// AddLight(pMainProgram, to_string(0), viewMatrix, viewNormalMatrix, *m_phero->dayLight);

		circularWaterProgram->SetUniform("dayLight.position", viewMatrix * m_phero->dayLight->_position); // Position of light source *in eye coordinates*
		circularWaterProgram->SetUniform("dayLight.La", m_phero->dayLight->_lightAmb);		// Ambient colour of light
		circularWaterProgram->SetUniform("dayLight.Ld", m_phero->dayLight->_lightDiff);		// Diffuse colour of light
		circularWaterProgram->SetUniform("dayLight.Ls", m_phero->dayLight->_lightSpec);		// Specular colour of light
		circularWaterProgram->SetUniform("dayLight.direction", glm::normalize(viewNormalMatrix*m_phero->dayLight->_direction));		// Direction of spot light
		circularWaterProgram->SetUniform("dayLight.exponent", m_phero->dayLight->_exponent);		// Exponent of spot light
		circularWaterProgram->SetUniform("dayLight.cutoff", m_phero->dayLight->_cutoff);		// Cutoff of spot light

		circularWaterProgram->SetUniform("material1.Ma", glm::vec3(1.f));	// Ambient material reflectance
		circularWaterProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
		circularWaterProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance	
		circularWaterProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	}

	perlin_t += 0.001f* (float)deltaTime;
	// float t = sin(perlin_t);
	circularWaterProgram->SetUniform("timePassed", perlin_t);
	// circularWaterProgram->SetUniform("nightMode", nightMode);

	// Spline Render
	modelViewMatrixStack.Push();
		circularWaterProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		circularWaterProgram->SetUniform("matrices.normalMatrix", m_pcamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// m_catmull->RenderCentreline();
		// m_catmull->RenderOffsetCurves();
		m_catmull->RenderTrack();
	modelViewMatrixStack.Pop();


}

void Game::AddLight(CShaderProgram *&shaderProgram, string index, glm::mat4 &viewMatrix, glm::mat3 &viewNormalMatrix, const LightInfo &lightInfo)
{

	// glm::vec4 lightPosition1 = glm::vec4(0, 50, 0, 1); // Position of light source *in world coordinates*
	shaderProgram->SetUniform("light[" + index + "].position", viewMatrix * lightInfo._position); // Position of light source *in eye coordinates*
	shaderProgram->SetUniform("ligh[" + index + "].La", lightInfo._lightAmb);		// Ambient colour of light
	shaderProgram->SetUniform("light[" + index + "].Ld", lightInfo._lightDiff);		// Diffuse colour of light
	shaderProgram->SetUniform("light[" + index + "].Ls", lightInfo._lightSpec);		// Specular colour of light
	shaderProgram->SetUniform("light[" + index + "].direction", glm::normalize(viewNormalMatrix* lightInfo._direction));		// Direction of spot light
	shaderProgram->SetUniform("light[" + index + "].exponent", lightInfo._exponent);		// Exponent of spot light
	shaderProgram->SetUniform("light[" + index + "].cutoff", lightInfo._cutoff);		// Cutoff of spot light
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	
	// Fixed timer
	deltaTime = m_pHighResolutionTimer->Elapsed();
	if (deltaTime > 1000.0 / (double) Game::FPS) {
		m_pHighResolutionTimer->Start();
		Update();
		CheckForCollisions();
		Render();
	}
	
	//// Variable timer
	//m_pHighResolutionTimer->Start();
	//Update();
	//Render();
	//deltaTime = m_pHighResolutionTimer->Elapsed();
	//

}


void Game::SetTitleScreen()
{
	// Place Camera
	// glm::vec3 camPos = glm::vec3(0, 0, -50);
	glm::vec3 camLook = titlePosition;
	m_pcamera->Set(currentCamTitlePosition, camLook, glm::vec3(0, 1, 0));

}

void Game::SetupLevel()
{
	perlin_t = 0.f;
	// Spline work
	m_t = 0.0f;
	deltaTime = 0.0f;
	// Enemy list
	// enemyPositions;

	// Clear all lists if there are any in them
	// 
	if (gemPositions.size() != 0)
		gemPositions.clear();
	if (cratePositions.size() != 0)
		cratePositions.clear();
	if (enemyPositions.size() != 0)
		enemyPositions.clear();
	if (explodingCratePositionTimes.size() != 0)
		explodingCratePositionTimes.clear();
	if (explodingEnemyPositionTimes.size() != 0)
		explodingEnemyPositionTimes.clear();

	if (enemiesAlive.size() != 0)
	{
		for (int i = 0; i < enemiesAlive.size(); i++)
		{
			delete enemiesAlive[i];
		}
	}

	float angleIncrementer = pi / 4;
	int size = m_catmull->getControlPoints().size();
	for (int i = 0; i < size; i+=10)
	{
		for (float k = 0; k < 2 * pi; k += angleIncrementer)
		{
			float r = randfloat();

			if (r < probOfObstacle)
			{
				float q = randfloat();
				if (q > probOfCratevsEnemy)
				{
					// Spawn crate
					cratePositions.push_back(Position());
					m_catmull->SuperTNBMaker(cratePositions[cratePositions.size() - 1], k, (trackWidth / 2) + 4, i * 200);
				}
				else
				{
					// Spawn enemy
					Position p;
					m_catmull->SuperTNBMaker(p, k, (trackWidth / 2) + 4, i * 200);
					enemiesAlive.push_back(new MovingObject(i * 200, k, p, -enemyMoveSpeed, 1000.f)); // Large arbitary value for curvetime, so enemies are always on curve

					// enemyPositions.push_back(Position());
					// m_catmull->SuperTNBMaker(enemyPositions[enemyPositions.size() - 1], k, (trackWidth / 2) + 4, i * 200);
				}

			}
			else if(r < probOfGem)
			{

				// Spawn gem
				gemPositions.push_back(Position());
				m_catmull->SuperTNBMaker(gemPositions[gemPositions.size() - 1], k, (trackWidth / 2) + 4, i * 200);
			}
			// Otherwise leave empty

		}
	}

	int d = 0;
	d++;

}

void Game::StartGame()
{
	m_phero->Initialise();

	state = GAMEPLAY;
}

void Game::OnLoseGame()
{
	// Blow up player 
	finalCamPos =  m_pcamera->GetPosition();

	state = LOSE;
}

void Game::OnWinGame()
{
	finalCamPos = m_pcamera->GetPosition();

	state = WIN;
}

WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		case VK_NUMPAD1:
			m_phero->cameraSetting = m_phero->Freeview;
			m_pcamera->Set(glm::vec3(0.f, 10.f, 0.f), glm::vec3(1.f, 10.f, 0.f), glm::vec3(0.f, 1.f, 0.f)); // Reset camera position
			break;
		case VK_NUMPAD2:
			m_phero->cameraSetting = m_phero->First;
			break;
		case VK_NUMPAD3:
			m_phero->cameraSetting = m_phero->Third;
			break;
		case VK_NUMPAD4:
			m_phero->cameraSetting = m_phero->Top;
			break;
		case VK_NUMPAD5:
			m_phero->cameraSetting = m_phero->SideLeft;
			break;
		case VK_NUMPAD6:
			m_phero->cameraSetting = m_phero->SideRight;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
