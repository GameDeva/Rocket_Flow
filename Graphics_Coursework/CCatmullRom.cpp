#include "CCatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom(float trackWidth)
{
	this->trackWidth = trackWidth;
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b * t + c * t2 + d * t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	m_controlPoints.push_back(glm::vec3(1000, 300, 500));
	m_controlPoints.push_back(glm::vec3(900, 200, 271));
	m_controlPoints.push_back(glm::vec3(600, 100, 100));
	m_controlPoints.push_back(glm::vec3(300, 0, 0));
	m_controlPoints.push_back(glm::vec3(150, -50, -71));
	m_controlPoints.push_back(glm::vec3(50, -100, -71));
	m_controlPoints.push_back(glm::vec3(-150, -205, -200));
	m_controlPoints.push_back(glm::vec3(-200, -300, -400));
	m_controlPoints.push_back(glm::vec3(-400, -225, -290));
	m_controlPoints.push_back(glm::vec3(-600, -150, -170));
	m_controlPoints.push_back(glm::vec3(-450, -100, -100));
	m_controlPoints.push_back(glm::vec3(-200, -50, -50));
	m_controlPoints.push_back(glm::vec3(-100, -10, 0));
	m_controlPoints.push_back(glm::vec3(300, 100, 100));
	
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(trackSampleSize);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline); 
	glBindVertexArray(m_vaoCentreline);
	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < 500; i++) 
	{
		vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
}


void CCatmullRom::CreateOffsetCurves()
{
	int size = m_centrelinePoints.size();
	// Loop through each of the control points in the spline
	for (int i = 0; i < size; ++i)
	{
		glm::vec3 pNext, t, n, b;
		// pNext = (i + 1) == m_centrelinePoints.size() ? (m_centrelinePoints[i]) : m_centrelinePoints[i+1]; // TODO: simplify logic plz
		pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];


		t = glm::normalize(pNext - m_centrelinePoints[i]); // Tangent vector
		n = glm::normalize(glm::cross(t, glm::vec3(0, 1, 0))); // Normal vector
		b = glm::normalize(glm::cross(n, t)); // Binormal vector

		vector<glm::vec3> currentOffsetPoints;

		// For using the appropriate angle to find position of offset
		float angleIncrementer = (2.f*pi) / curveSampleSize;
		float theta = 0;
		for (int j = 0; j < curveSampleSize; j++)
		{
			currentOffsetPoints.push_back(m_centrelinePoints[i] + (b * (trackWidth / 2) * (float)sin(theta)) + (n * (trackWidth / 2) * (float)cos(theta)));
			theta += angleIncrementer;
		}

		m_offsetPoints.push_back(currentOffsetPoints);

	}

	//int currentOffsetIndex = 0;
	//// Define vertex attributes for each of the vbos of the offset points 
	//for (auto &vbo : outer_vbos)
	//{
	//	glBindVertexArray(m_vaoOffsetCurve[currentOffsetIndex++]);
	//	vbo.Bind();

	//	// Upload the VBO to the GPU
	//	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	//	// Vertex positions
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//	// Texture coordinates
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	//	// Normal vectors
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	//}


	// Flat Track
	/*
	// --------------------------------------------------------------------------  // --------------------------------------------------------------------------// --------------------------------------------------------------------------
	// Flat track

	//{

	//	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	//	// Note it is possible to only use one VAO / VBO with all the points instead.
	//	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	//	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	//	CVertexBufferObject r_vbo, l_vbo;
	//	l_vbo.Create();
	//	r_vbo.Create();
	//	// Set the vertex attribute locations
	//	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	//	glm::vec2 texCoord(0.0f, 0.0f);
	//	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	//	int size = m_centrelinePoints.size();
	//	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	//	for (int i = 0; i < size; i++)
	//	{
	//		glm::vec3 pNext, t, n, b;
	//		// pNext = (i + 1) == m_centrelinePoints.size() ? (m_centrelinePoints[i]) : m_centrelinePoints[i+1]; // TODO: simplify logic plz
	//		pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];


	//		t = glm::normalize(pNext - m_centrelinePoints[i]); // Tangent vector
	//		n = glm::normalize(glm::cross(t, glm::vec3(0, 1, 0))); // Normal vector
	//		b = glm::normalize(glm::cross(n, t)); // Binormal vector

	//		m_leftOffsetPoints.push_back(m_centrelinePoints[i] - (trackWidth / 2) * n); // Left point of track
	//		m_rightOffsetPoints.push_back(m_centrelinePoints[i] + (trackWidth / 2) * n); // Right point of track

	//		// Add data to both vbos
	//		l_vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
	//		l_vbo.AddData(&texCoord, sizeof(glm::vec2));
	//		l_vbo.AddData(&normal, sizeof(glm::vec3));

	//		r_vbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
	//		r_vbo.AddData(&texCoord, sizeof(glm::vec2));
	//		r_vbo.AddData(&normal, sizeof(glm::vec3));

	//	}


	//	glBindVertexArray(m_vaoLeftOffsetCurve);
	//	l_vbo.Bind();

	//	// Upload the VBO to the GPU
	//	l_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	//	// Vertex positions
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//	// Texture coordinates
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	//	// Normal vectors
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	//	glBindVertexArray(m_vaoRightOffsetCurve);
	//	r_vbo.Bind();

	//	// Upload the VBO to the GPU
	//	r_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	//	// Vertex positions
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//	// Texture coordinates
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	//	// Normal vectors
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	//}
	*/

}


void CCatmullRom::CreateTrack()
{
	m_texture.Load("resources\\textures\\water.jpg", true);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Index values for perlin noise
	vector<glm::vec2>  indexValues = vector<glm::vec2>();

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	// Create a VBO
	CVertexBufferObject vbo, vboFlip;
	vbo.Create();
	vboFlip.Create();
	
	int size = m_centrelinePoints.size();
	for (int i = 0; i < size; i++)
	{

		for (int j = 0; j < curveSampleSize; j++)
		{
			p.push_back(m_offsetPoints[i][(j + 1) % curveSampleSize]);
			p.push_back(m_offsetPoints[i][j]);
			p.push_back(m_offsetPoints[(i + 1) % size][(j + 1) % curveSampleSize]);
			p.push_back(m_offsetPoints[(i + 1) % size][j]);

			indexValues.push_back(glm::vec2(i, (j + 1) % curveSampleSize));
			indexValues.push_back(glm::vec2(i, j));
			indexValues.push_back(glm::vec2((i + 1) % size, (j + 1) % curveSampleSize));
			indexValues.push_back(glm::vec2((i + 1) % size, j));

			// Flipped
			//pFlip.push_back(m_offsetPoints[i][j]);
			//pFlip.push_back(m_offsetPoints[i][(j + 1) % curveSampleSize]);
			//pFlip.push_back(m_offsetPoints[i + 1][j]);
			//pFlip.push_back(m_offsetPoints[i + 1][(j + 1) % curveSampleSize]);
			
			// Tex
			s.push_back(glm::vec2(0.f, 0.f));
			s.push_back(glm::vec2(1.f, 0.f));
			s.push_back(glm::vec2(0.f, 1.f));
			s.push_back(glm::vec2(1.f, 1.f));

			// Calc normal
			n.push_back(glm::cross(m_offsetPoints[i][(j + 1) % curveSampleSize] - m_offsetPoints[i][j], m_offsetPoints[(i + 1) % size][j] - m_offsetPoints[i][j]));

		}

	}

	vbo.Bind();
	int pointsSize = p.size();
	for (int i = 0, j = 0; i < pointsSize; i++)
	{
		vbo.AddData(&p[i], sizeof(glm::vec3));
		vbo.AddData(&s[i], sizeof(glm::vec2));
		vbo.AddData(&n[j], sizeof(glm::vec3));
		vbo.AddData(&indexValues[i], sizeof(glm::vec2));

		// Increase j after 4 passes of i
		if (i % 4 == 0 && i != 0)
			j++;

		m_vertexCount++;
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + 2 * sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	// Index Value coordinates for perlin noise
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3)));
	// ---- 

	//vboFlip.Bind();
	//pointsSize = p.size();
	//for (int i = 0, j = 0; i < pointsSize; i++)
	//{
	//	vboFlip.AddData(&pFlip[i], sizeof(glm::vec3));
	//	vboFlip.AddData(&s[i], sizeof(glm::vec2));
	//	vboFlip.AddData(&n[j], sizeof(glm::vec3));

	//	// Increase j after 4 passes of i
	//	if (i % 4 == 0 && i != 0)
	//		j++;

	//	m_vertexCount++;
	//}

	//// Upload the VBO to the GPU
	//vboFlip.UploadDataToGPU(GL_STATIC_DRAW);
	//// Set the vertex attribute locations
	//stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	//// Vertex positions
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//// Texture coordinates
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	//// Normal vectors
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
	
	// Some drivers don't have this function
	glLineWidth(1.f);
}

void CCatmullRom::RenderOffsetCurves()
{
	for (int i = 0; i < curveSampleSize; i++)
	{
		glBindVertexArray(m_vaoOffsetCurve[i]);
		glDrawArrays(GL_LINE_LOOP, 0, m_offsetPoints.size());
		glLineWidth(1.f);
	}
	
}

void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	// glFrontFace(GL_CULL_FACE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < curveSampleSize * m_centrelinePoints.size() * 4; i += curveSampleSize)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, i, curveSampleSize);
	}
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}


void CCatmullRom::TNBAtSamplePoint(float samplePoint, Position &position)
{
	glm::vec3 p, pNext, v;
	Sample(samplePoint, p); // Point to move to
	Sample(samplePoint + 1, pNext); // Point ahead to work out the T vector (for TNB frame)
	// Sample(samplePoint + 30, v); // Point camera should look at

	// TNB at centre
	position._T = glm::normalize(pNext - p);
	position._N = glm::normalize(glm::cross(position._T, glm::vec3(0, 1, 0)));
	position._B = glm::normalize(glm::cross(position._N, position._T));

	// Position point
	position._point = p;
}

glm::vec3 CCatmullRom::positionAtAngle(Position &position, float angleInRad, float radius)
{
	// Returns new point on edge
	return position._point + (position._B * (radius) * (float)sin(angleInRad)) + (position._N * (radius) * (float)cos(angleInRad));
}

void CCatmullRom::UpdatePositionToEdge(glm::vec3 newPoint, Position &position)
{
	// Update TNB 
	if (!flipMode)
	{
		position._B = glm::normalize(newPoint - position._point);
		position._N = glm::normalize(glm::cross(position._T, position._B));
	}
	else {
		position._B = glm::normalize(position._point - newPoint);
		position._N = glm::normalize(glm::cross(position._T, position._B));
	}

	// Update position to one on edge
	position._point = newPoint;

}

void CCatmullRom::SuperTNBMaker(Position &position, float angle, float radius, float m_t)
{
	if (!flipMode)
	{
		TNBAtSamplePoint(m_t, position);
		glm::vec3 newPoint = positionAtAngle(position, angle, radius);
		UpdatePositionToEdge(newPoint, position);
	}
	else {
		TNBAtSamplePoint(m_t, position);
		glm::vec3 newPoint = positionAtAngle(position, angle, radius);
		UpdatePositionToEdge(newPoint, position);
	}


}


void CCatmullRom::PerlinateOffsetPoints()
{
	//int seed = 12345;
	//int octaves = 8;
	//float frequency = 8.f;

	//PerlinNoise perlinNoise = PerlinNoise(seed);

	//int size = m_centrelinePoints.size();
	//for (int i = 0; i < size - 1; i++)
	//{

	//	for (int j = 0; j < curveSampleSize; j++)
	//	{
	//		perlinNoise.octaveNoise0_1()
	//	}
	//}


}
