@fsinclude shadowMap

varying vec3 var_EyeVector;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct SurfaceData
{
	vec4 albedo;
	vec4 emissive;
	vec3 normal;
	float shininess;
	float gloss;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct LightData
{
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec3 dir;
	vec3 halfDir;
	
	vec3 spotDirection;
	float spotCutoff;
	float spotExponent;

	float shadow;
	float distance;
	vec3 attenuation;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct LitSurfaceData
{
	vec4 luminance;
};

@fsinclude lightFunctions
@customFragmentDefs

///////////////////////////////////////////////////////////////////////////////////////////////////
$@fragmentLightSection
{ 
	LightData ld;
	
	vec3 lightVector = vec3(gl_LightSource[@lightIndex].position.xyz - var_EyeVector);
	
	ld.diffuse = gl_LightSource[@lightIndex].diffuse;
	ld.specular = gl_LightSource[@lightIndex].specular;
	ld.ambient = gl_LightSource[@lightIndex].ambient;
	ld.dir = normalize(lightVector);
	ld.halfDir = reflect(-ld.dir, surf.normal);
	ld.distance = length(lightVector);
	ld.spotDirection = gl_LightSource[@lightIndex].spotDirection;
	ld.spotExponent = gl_LightSource[@lightIndex].spotExponent;
	ld.spotCutoff = gl_LightSource[@lightIndex].spotCosCutoff;
	
	ld.attenuation[0] = gl_LightSource[@lightIndex].constantAttenuation;
	ld.attenuation[1] = gl_LightSource[@lightIndex].linearAttenuation;
	ld.attenuation[2] = gl_LightSource[@lightIndex].quadraticAttenuation;
	
	if(@lightIndex == 0) ld.shadow = shadow;
	else ld.shadow = 1.0;
	
	vec4 lum = @lightFunction(surf, ld);
	litSurfData.luminance.rgb += lum.rgb;
	litSurfData.luminance.a *= lum.a;
} 	
$

///////////////////////////////////////////////////////////////////////////////////////////////////
LitSurfaceData computeLighting(SurfaceData surf)
{
	LitSurfaceData litSurfData;
	float shadow = computeShadowMap();
	
	surf.normal = normalize(surf.normal);
	
	// If we are rendering a back-facing fragment, invert the normal by default
	surf.normal = - surf.normal;
	// XXX - HACK !!!!
	// if(!gl_FrontFacing) surf.normal = -surf.normal;

	litSurfData.luminance = vec4(0, 0, 0, 1);

	@fragmentLightSection

	// Add emissive surface component to final luminance.
	litSurfData.luminance.rgb += surf.emissive.rgb;
	litSurfData.luminance.a *= surf.emissive.a;
	
	return litSurfData;
}

// Surface shader main function declaration
SurfaceData getSurfaceData(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
void main (void)
{
	SurfaceData sd = getSurfaceData();
	LitSurfaceData lsd = computeLighting(sd);
    gl_FragColor = lsd.luminance; 
}
