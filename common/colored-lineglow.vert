@vertexShader

uniform float unif_Gloss;
uniform float unif_Shininess;
uniform float unif_Thickness;

//varying float var_Dist;
//varying vec3 var_MyEyeVector;

varying float power;

///////////////////////////////////////////////////////////////////////////////////////////////////
void setupSurfaceData(vec4 eyeSpacePosition)
{
	vec4 P1 = vec4(0, 0, -1, 1);
	vec4 P2 = vec4(0, 0, 1, 1);
	
	gl_Position.xyz += gl_Normal * unif_Gloss * unif_Thickness;
	
	vec4 P0 = gl_Vertex;
	
	P1 = gl_ProjectionMatrix * gl_ModelViewMatrix * P1;
	P2 = gl_ProjectionMatrix * gl_ModelViewMatrix * P2;
	
	vec2 P1_2d = P1.xy / P1.w;
	vec2 P2_2d = P2.xy / P2.w;
	
	//vec4 P0 = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	P0 = gl_ProjectionMatrix * gl_ModelViewMatrix * P0;
	vec2 P0_2d = P0.xy / P0.w;
	
	float num = abs((P2_2d.x - P1_2d.x) * (P1_2d.y - P0_2d.y) -
			(P1_2d.x - P0_2d.x) * (P2_2d.y - P1_2d.y));
			
	float det = sqrt((P2_2d.x - P1_2d.x) * (P2_2d.x - P1_2d.x)
	+ (P2_2d.y - P1_2d.y) * (P2_2d.y - P1_2d.y));
	
	float le = unif_Thickness * (1 + unif_Gloss);
	
	power = pow(1 - (num / det / le), unif_Shininess);
}