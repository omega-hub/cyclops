///////////////////////////////////////////////////////////////////////////////////////////////////
varying vec3 var_ReflectDir;

///////////////////////////////////////////////////////////////////////////////////////////////////
void main (void)
{
	//Enable texture coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = gl_MultiTexCoord2;
	gl_TexCoord[3] = gl_MultiTexCoord3;

	//Cubemap
	gl_Position    = ftransform();
	vec3 normal    = normalize(gl_NormalMatrix*gl_Normal);
	//vec4 pos       = gl_ModelViewMatrix*gl_Vertex;
	//vec4 pos       = gl_ModelViewMatrix*gl_Vertex;
	//vec3 eyeDir    = pos.xyz;
	var_ReflectDir = -gl_Normal; //reflect(gl_Vertex,normal);
	//var_ReflectDir = normal;
}