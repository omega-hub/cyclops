@surfaceShader 

//uniform float unif_Shininess;
//uniform float unif_Gloss;
//varying float var_Dist;
//varying vec3 var_MyEyeVector;

varying float power;

varying vec3 var_Normal;

///////////////////////////////////////////////////////////////////////////////////////////////////
SurfaceData getSurfaceData(void)
{
	//vec3 unif_P1 = vec3(-1, 0, -2);
	//vec3 unif_P2 = vec3(1, 0, -2);
	
	//float glowPower = pow(var_Dist * 0.9, 4); 
	
	//float rim = 1.0 - clamp(dot(normalize(var_EyeVector), var_Normal), 0.0, 1.0);
	
	SurfaceData sd;
    sd.albedo = vec4(0, 0, 0, 0); 
	sd.emissive = vec4(1, 0, 0, 0);
	sd.emissive.a = power;
	sd.shininess = 0;
	sd.gloss = 0;
	sd.normal = var_Normal;
	
	return sd;
}
