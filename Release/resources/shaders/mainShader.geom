#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

uniform bool bExplodeObject; 
uniform float explodeFactor;

//in vColourPass
in vec2 vTexCoord[];
in vec4 vEyePosition[];
in vec3 vEyeNorm[];
in vec3 worldPosition[];	// used for skybox

out vec2 gTexCoord;
out vec4 gEyePosition;
out vec3 gEyeNorm;

out vec3 gWorldPosition;	// used for skybox

layout(triangles) in;
layout(triangle_strip, max_vertices= 3) out;

void main() 
{ 
	float localExplode = bExplodeObject ? explodeFactor: 0.0;
	// Get triangle normal 
	vec3 e1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz; 
	vec3 e2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz; 
	vec3 n = normalize(cross(e1, e2)); 
	for (int i = 0; i < 3; i++) 
	{ 
		vec4 explodedPos = gl_in[i].gl_Position + localExplode * vec4(n, 0); 
		gl_Position = matrices.projMatrix * explodedPos; 
		gTexCoord = vTexCoord[i];
		gEyePosition = vEyePosition[i];
		gEyeNorm = vEyeNorm[i];
		gWorldPosition = worldPosition[i];

		//vColour = vColourPass[i]; 
		//vTexCoord = vTexCoordPass[i]; 
		EmitVertex(); 
	} 
	
	EndPrimitive();
}