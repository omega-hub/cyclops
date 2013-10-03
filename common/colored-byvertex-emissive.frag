@surfaceShader

uniform float unif_Shininess;
uniform float unif_Gloss;

varying vec3 var_Normal;

///////////////////////////////////////////////////////////////////////////////////////////////////
SurfaceData getSurfaceData(void)
{
	SurfaceData sd;
    sd.albedo = vec4(0,0,0,1); 
	sd.emissive = gl_Color;
	sd.shininess = unif_Shininess;
	sd.gloss = unif_Gloss;
	sd.normal = normalize(var_Normal);
	
	return sd;
}
