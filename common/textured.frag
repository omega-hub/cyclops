@surfaceShader 

// The diffuse texture
uniform sampler2D unif_DiffuseMap;
varying vec2 var_TexCoord;

uniform float unif_Gloss;
uniform float unif_Shininess;

varying vec3 var_Normal;

///////////////////////////////////////////////////////////////////////////////////////////////////
SurfaceData getSurfaceData(void)
{
	SurfaceData sd;
	sd.albedo = texture2D(unif_DiffuseMap, var_TexCoord) * gl_Color;
	sd.emissive = vec4(0, 0, 0, 1);
	sd.shininess = unif_Shininess;
	sd.gloss = unif_Gloss;
	sd.normal = var_Normal;
	
	return sd;
}
