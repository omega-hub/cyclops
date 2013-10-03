@vsinclude shadowMap
@vsinclude envMap

attribute vec3 attrib_Tangent;

varying vec3 var_EyeVector;
varying vec3 var_LightVector[@numLights]; 
varying vec3 var_LightHalfVector[@numLights]; 

void setupSurfaceData(vec4 eyeSpacePosition);

///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	gl_Position = ftransform();
	vec4 eyeSpacePosition = gl_ModelViewMatrix * gl_Vertex;
	
	// Building the matrix Eye Space -> Tangent Space
	vec3 n = normalize (gl_NormalMatrix * gl_Normal);
	vec3 t = normalize (gl_NormalMatrix * attrib_Tangent);
	vec3 b = cross (n, t);
	
	vec3 eyeDirection = normalize(eyeSpacePosition.xyz);
	
	int i; 
	for (i=0; i< @numLights; ++i) 
	{ 
		// transform light and half angle vectors by tangent basis
		vec3 lightDir = normalize(vec3(gl_LightSource[i].position.xyz - eyeSpacePosition.xyz));
		vec3 v;
		v.x = dot (lightDir, t);
		v.y = dot (lightDir, b);
		v.z = dot (lightDir, n);
		var_LightVector[i] = normalize (v);
		
		vec3 halfVector = normalize(eyeDirection + lightDir);
		v.x = dot (halfVector, t);
		v.y = dot (halfVector, b);
		v.z = dot (halfVector, n);
		var_LightHalfVector[i] = v;
	}

	// Compute the tangent space eye position
	vec3 v;
	v.x = dot (eyeSpacePosition, t);
	v.y = dot (eyeSpacePosition, b);
	v.z = dot (eyeSpacePosition, n);
	var_EyeVector = normalize (v);

	setupShadowMap(eyeSpacePosition);
	setupEnvMap(eyeSpacePosition.xyz);
	setupSurfaceData(eyeSpacePosition);
	
	gl_FrontColor = gl_Color;
}
