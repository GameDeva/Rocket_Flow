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
uniform LightInfo dayLight;
uniform LightInfo nightLight;
uniform LightInfo light[8]; 
uniform int lightCount;
uniform MaterialInfo material1; 


// in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec4 vEyePosition;
in vec3 vEyeNorm;

out vec4 vOutputColour;		// The output colour

uniform bool nightMode;
uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool applyDiscard;
uniform float discardValue;
in vec3 worldPosition;


// Blinn Phong approximation
vec3 BlinnPhongModel(LightInfo l, vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(l.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	
	vec3 h = normalize(v + s);

	vec3 ambient = l.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = l.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = l.Ls * material1.Ms * pow(max(dot(h, n), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}


vec3 BlinnPhongSpotlightModel(LightInfo l, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(l.position -p));
	float angle = acos(dot(-s, l.direction));
	float cutoff = radians(clamp(l.cutoff, 0.0, 90.0));
	vec3 ambient = l.La * material1.Ma;
	if(angle < cutoff) 
	{
		float spotFactor = pow(dot(-s, l.direction), l.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = l.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		if(sDotN > 0.0)
			specular = l.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		return ambient + spotFactor * (diffuse + specular);
	}
	else
	{
		return ambient;
	}
}



void main()
{
	vec3 vColour = vec3(0);

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		// Check should discard fragment
		if(applyDiscard && vTexColour.r < discardValue) {
			discard;
		}
		else 
		{
			if(nightMode)
			{
				vColour += BlinnPhongModel(nightLight, vEyePosition, normalize(vEyeNorm));
				for(int i = 0; i < lightCount; i++)
				{
					vColour += BlinnPhongSpotlightModel(light[i], vEyePosition, normalize(vEyeNorm));
				}
			}
			else 
			{
				vColour = BlinnPhongModel(dayLight, vEyePosition, normalize(vEyeNorm));
			}
			if (bUseTexture)
			{
				vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
			}
			else
			{
				vOutputColour = vec4(vColour, 1.0f);	// Just use the colour	instead
			}
		}
	}
	
	
}
