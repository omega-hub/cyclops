@surfaceShader 
@fsinclude envMap

uniform float unif_Shininess;
uniform float unif_Gloss;

varying vec3 var_Normal;
///////////////////////////////////////////////////////////////////////////////////////////////////
SurfaceData getSurfaceData(void)
{
	SurfaceData sd;
    sd.albedo = gl_Color; 
	sd.shininess = unif_Shininess;
	sd.gloss = unif_Gloss;
	
	// Modulate environment map contribution by material glossyness and color
	sd.emissive = getEnvMapColor()	* gl_Color * unif_Gloss;
	sd.normal = var_Normal;
	
	return sd;
}
