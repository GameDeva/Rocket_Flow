
#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"

class CCatmullRom
{
public:
	
	CCatmullRom(float trackWidth);
	CCatmullRom() { this->trackWidth = 30.f; 	m_vertexCount = 0; } // Defualt constructor with arbitary trackWidth
	~CCatmullRom();

	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();

	void CreateTrack();
	void RenderTrack();

	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3 &p, glm::vec3 &up = glm::vec3(0, 0, 0)); // Return a point on the centreline based on a certain distance along the control curve.

	void SuperTNBMaker(Position &position, float angle, float m_t);

	void TNBAtSamplePoint(float samplePoint, Position &position);
	glm::vec3 positionAtAngle(Position &position, float angleInRad, float radius);
	void UpdatePositionToEdge(glm::vec3 newPoint, Position &position);


private:

	bool flipMode = false;

	// const float pi = 3.14159f;
	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);

	const static int trackSampleSize = 300; // Sample size of points on the track to interpolate between the fixed control points
	const static int curveSampleSize = 16; // Number of vertices on polygon shape of the track

	vector<float> m_distances;
	CTexture m_texture;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoOffsetCurve[trackSampleSize];
	GLuint m_vaoTrack;

	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	vector<glm::vec3> m_centrelinePoints;	// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors

	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points
	vector<vector<glm::vec3>> m_offsetPoints;

	vector<glm::vec3> pFlip;
	vector<glm::vec3> p;
	vector<glm::vec2> s;
	vector<glm::vec3> n;


	float trackWidth;

	unsigned int m_vertexCount;				// Number of vertices in the track VBO
};
