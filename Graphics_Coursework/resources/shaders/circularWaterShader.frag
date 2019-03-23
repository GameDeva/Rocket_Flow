#version 400 core

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;

	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform MaterialInfo material1; 


// in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec4 vEyePosition;
in vec3 vEyeNorm;

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;


// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;


	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}

// Blinn Phong approximation
vec3 BlinnPhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	
	vec3 h = normalize(v + s);

	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(h, n), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}


vec3 BlinnPhongSpotlightModel(vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light1.position -p));
	float angle = acos(dot(-s, light1.direction));
	float cutoff = radians(clamp(light1.cutoff, 0.0, 90.0));
	vec3 ambient = light1.La * material1.Ma;
	if(angle < cutoff) 
	{
		float spotFactor = pow(dot(-s, light1.direction), light1.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light1.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		if(sDotN > 0.0)
			specular = light1.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		return ambient + spotFactor * (diffuse + specular);
	}
	else
		return ambient;
}



void main()
{
	vec3 vColour;

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
		{
			vColour = BlinnPhongSpotlightModel(vEyePosition, normalize(vEyeNorm));
			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
		}
		else
		{
			vColour = BlinnPhongSpotlightModel(vEyePosition, normalize(vEyeNorm));
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
		}
	}
	
	
}
