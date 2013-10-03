@surfaceShader 

// The diffuse texture
uniform sampler2D unif_DiffuseMap;
varying vec2 var_TexCoord;

uniform float unif_Shininess;
uniform float unif_Gloss;

///////////////////////////////////////////////////////////////////////////////////////////////////
SurfaceData getSurfaceData(void)
{
	vec4 texColor = texture2D(unif_DiffuseMap, var_TexCoord);
	if(texColor.r > 0.5) discard;
	
	SurfaceData sd;
    sd.albedo = gl_Color * texColor;
	sd.emissive = vec4(0, 0, 0, 0);
	sd.shininess = unif_Shininess;
	sd.gloss = unif_Gloss;
	
	
	
	return sd;
}
