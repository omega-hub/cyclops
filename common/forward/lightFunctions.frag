///////////////////////////////////////////////////////////////////////////////
vec4 pointLightFunction(SurfaceData sd, LightData ld)
{
	vec4 l = vec4(0, 0, 0, sd.albedo.a);
	float lambertTerm = dot(sd.normal, ld.dir) * ld.shadow; 
	if (lambertTerm > 0.0) 
	{ 
		l.rgb += sd.albedo.rgb * ld.diffuse.rgb * lambertTerm; 
		
		// Compute specular factor
		float specular = pow( max(dot(ld.halfDir, sd.normal), 0.0), sd.shininess ); 
		l.rgb += ld.specular.rgb * specular * sd.gloss; 
	} 
	l.rgb += sd.albedo.rgb * ld.ambient.rgb;
	
	vec3 ka = ld.attenuation;
	float att = 1.0 / (ka[0] + ld.distance * ka[1] + ld.distance * ld.distance * ka[2]);
	// Scale luminance by attenuation and return
	vec4 luminance = l * att;
	luminance.a = l.a;
	return luminance;
}

///////////////////////////////////////////////////////////////////////////////
vec4 spotLightFunction(SurfaceData sd, LightData ld)
{
	vec4 l = vec4(0, 0, 0, sd.albedo.a);
	float lambertTerm = dot(sd.normal, ld.dir) * ld.shadow; 
	if (lambertTerm > 0.0) 
	{ 
		float spot = dot(ld.spotDirection, -ld.dir);
		
		if(spot > ld.spotCutoff)
		{
			l.rgb += sd.albedo.rgb * ld.diffuse.rgb * lambertTerm; 
			
			// Compute specular factor
			float specular = pow( max(dot(ld.halfDir, sd.normal), 0.0), sd.shininess ); 
			l.rgb += ld.specular.rgb * specular * sd.gloss; 
			
			// Ambient
			l.rgb += sd.albedo.rgb * ld.ambient.rgb;
			
			vec3 ka = ld.attenuation;
			float att = 1.0 / (ka[0] + ld.distance * ka[1] + ld.distance * ld.distance * ka[2]);
				
			// Scale luminance by attenuation and return
			l.rgb *= att * pow(spot, ld.spotExponent);
		}
	} 
	return l;
}

///////////////////////////////////////////////////////////////////////////////
vec4 directionalLightFunction(SurfaceData sd, LightData ld)
{
	ld.dir = normalize(ld.spotDirection);
	ld.distance = 0.0;
	ld.halfDir = reflect(-ld.dir, sd.normal);
	return pointLightFunction(sd, ld);
}
