#include"CCube.h"

CCube::CCube() { }

CCube::~CCube() { Release(); }

void CCube::Create(string filename) {
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
	
	// Z - Positive
	p.push_back(glm::vec3(-1.f, -1.f, 1.f));
	p.push_back(glm::vec3(1.f, -1.f, 1.f));
	p.push_back(glm::vec3(-1.f, 1.f, 1.f));
	p.push_back(glm::vec3(1.f, 1.f, 1.f));
	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.f, 1.f));
	s.push_back(glm::vec2(1.f, 1.f));
	n.push_back(glm::vec3(0.f, 0.f, 1.f));
	

	// X - Positve
	p.push_back(glm::vec3(1.f, -1.f, 1.f));
	p.push_back(glm::vec3(1.f, -1.f, -1.f));
	p.push_back(glm::vec3(1.f, 1.f, 1.f));
	p.push_back(glm::vec3(1.f, 1.f, -1.f));
	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.f, 1.f));
	s.push_back(glm::vec2(1.f, 1.f));
	n.push_back(glm::vec3(1.f, 0.f, 0.f));


	// Y - Positive
	p.push_back(glm::vec3(1.f, 1.f, 1.f));
	p.push_back(glm::vec3(1.f, 1.f, -1.f));
	p.push_back(glm::vec3(-1.f, 1.f, 1.f));
	p.push_back(glm::vec3(-1.f, 1.f, -1.f));
	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.f, 1.f));
	s.push_back(glm::vec2(1.f, 1.f));
	n.push_back(glm::vec3(0.f, 1.f, 0.f));


	// Z - Negative
	p.push_back(glm::vec3(1.f, -1.f, -1.f));
	p.push_back(glm::vec3(-1.f, -1.f, -1.f));
	p.push_back(glm::vec3(1.f, 1.f, -1.f));
	p.push_back(glm::vec3(-1.f, 1.f, -1.f));
	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.f, 1.f));
	s.push_back(glm::vec2(1.f, 1.f));
	n.push_back(glm::vec3(0.f, 0.f, 1.f));

	// X - Negative
	p.push_back(glm::vec3(-1.f, -1.f, -1.f));
	p.push_back(glm::vec3(-1.f, -1.f, 1.f));
	p.push_back(glm::vec3(-1.f, 1.f, -1.f));
	p.push_back(glm::vec3(-1.f, 1.f, 1.f));
	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.f, 1.f));
	s.push_back(glm::vec2(1.f, 1.f));
	n.push_back(glm::vec3(0.f, 0.f, 1.f));


	// Y - Negative
	p.push_back(glm::vec3(-1.f, -1.f, -1.f));
	p.push_back(glm::vec3(1.f, -1.f, -1.f));
	p.push_back(glm::vec3(-1.f, -1.f, 1.f));
	p.push_back(glm::vec3(1.f, -1.f, 1.f));
	s.push_back(glm::vec2(0.f, 0.f));
	s.push_back(glm::vec2(1.f, 0.f));
	s.push_back(glm::vec2(0.f, 1.f));
	s.push_back(glm::vec2(1.f, 1.f));
	n.push_back(glm::vec3(0.f, 0.f, 1.f));

	int size = p.size();
	for (int i = 0, j = 0; i < size; i++)
	{
		m_vbo.AddData(&p[i], sizeof(glm::vec3));
		m_vbo.AddData(&s[i], sizeof(glm::vec2));
		m_vbo.AddData(&n[j], sizeof(glm::vec3));

		// Increase j after 4 passes of i
		if (i % 4 == 0 && i != 0)
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

void CCube::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();


	// Call glDrawArrays to render each side
	for (int i = 0; i < 24; i += 4) {
		glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
	}
	
}

void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}