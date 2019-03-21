#include"CTetra.h"

CTetra::CTetra() { }

CTetra::~CTetra() { Release(); }

void CTetra::Create(string filename) {
	m_texture.Load(filename, true); 
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT); 
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT); 
	glGenVertexArrays(1, &m_vao); 
	glBindVertexArray(m_vao); 
	m_vbo.Create(); 
	m_vbo.Bind();
	
	// Write the code to add interleaved vertex attributes to the VBO
	
	// Side1
	p.push_back(glm::vec3(-1.f, -1.f, 1.f));
	p.push_back(glm::vec3(1.f, 1.f, 1.f));
	p.push_back(glm::vec3(-1.f, 1.f, -1.f));
	n.push_back(glm::cross(glm::vec3(1.f, 1.f, 1.f) - glm::vec3(-1.f, -1.f, 1.f), glm::vec3(-1.f, 1.f, -1.f) - glm::vec3(-1.f, -1.f, 1.f)));

	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.5f, 1.f));
	

	// Side 2
	p.push_back(glm::vec3(-1.f, -1.f, 1.f));
	p.push_back(glm::vec3(1.f, 1.f, 1.f));
	p.push_back(glm::vec3(1.f, -1.f, -1.f));
	n.push_back(glm::cross(glm::vec3(1.f, 1.f, 1.f) - glm::vec3(-1.f, -1.f, 1.f), glm::vec3(1.f, -1.f, -1.f) - glm::vec3(-1.f, -1.f, 1.f)));

	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.5f, 1.f));

	

	// side 3
	p.push_back(glm::vec3(-1.f, -1.f, 1.f));
	p.push_back(glm::vec3(-1.f, 1.f, -1.f));
	p.push_back(glm::vec3(1.f, -1.f, -1.f));
	n.push_back(glm::cross(glm::vec3(-1.f, 1.f, -1.f) - glm::vec3(-1.f, -1.f, 1.f), glm::vec3(1.f, -1.f, -1.f) - glm::vec3(-1.f, -1.f, 1.f)));

	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.5f, 1.f));

	

	// Side 4
	p.push_back(glm::vec3(1.f, 1.f, 1.f));
	p.push_back(glm::vec3(-1.f, 1.f, -1.f));
	p.push_back(glm::vec3(1.f, -1.f, -1.f));
	n.push_back(glm::cross(glm::vec3(-1.f, 1.f, -1.f) - glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, -1.f, -1.f) - glm::vec3(1.f, 1.f, 1.f)));

	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.5f, 1.f));


	int size = p.size();
	for (int i = 0, j = 0; i < size; i++)
	{
		m_vbo.AddData(&p[i], sizeof(glm::vec3));
		m_vbo.AddData(&s[i], sizeof(glm::vec2));
		m_vbo.AddData(&n[j], sizeof(glm::vec3));

		// Increase j after 3 passes of i
		if (i % 3 == 0 && i != 0)
			j++;
	}


	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CTetra::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, p.size());

	//// Call glDrawArrays to render each side
	//for (int i = 0; i < 12; i += 3) {
	//	glDrawArrays(GL_TRIANGLE_STRIP, i, 3);
	//}

	
}

void CTetra::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}