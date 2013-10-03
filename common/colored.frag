@surfaceShader

uniform float unif_Shininess;
uniform float unif_Gloss;

varying vec3 var_Normal;

///////////////////////////////////////////////////////////////////////////////////////////////////
SurfaceData getSurfaceData(void)
{
	SurfaceData sd;
    sd.albedo = gl_FrontMaterial.diffuse; 
	sd.emissive = gl_FrontMaterial.emission;
	sd.shininess = unif_Shininess;
	sd.gloss = unif_Gloss;
	sd.normal = normalize(var_Normal);
	
	return sd;
}
