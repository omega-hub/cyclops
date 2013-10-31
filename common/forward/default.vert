@vsinclude envMap
@vsinclude shadowFunctions

varying vec3 var_Normal;
varying vec3 var_EyeVector;

void setupSurfaceData(vec4 eyeSpacePosition);

///////////////////////////////////////////////////////////////////////////////
void main(void)
{
	gl_Position = ftransform();
	vec4 eyeSpacePosition = gl_ModelViewMatrix * gl_Vertex;
	
	setupEnvMap(eyeSpacePosition.xyz);
	setupSurfaceData(eyeSpacePosition);
	
	var_EyeVector = eyeSpacePosition.xyz;
	var_Normal = gl_NormalMatrix * gl_Normal;
	
	// Setup shadow maps for lights that have shadow maps enabled.
	@vertexShadowSection
	
	gl_FrontColor = gl_Color;
}
