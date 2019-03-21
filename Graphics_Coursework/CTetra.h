#pragma once

#include"Common.h"
#include"Texture.h"
#include"VertexBufferObject.h"

// Class for generating a unit cube
class CTetra
{
public:
	CTetra();
	~CTetra();
	void Create(string filename);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;

	vector<glm::vec3> p;
	vector<glm::vec2> s;
	vector<glm::vec3> n;
};