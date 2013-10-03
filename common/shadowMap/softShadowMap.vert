///////////////////////////////////////////////////////////////////////////////////////////////////
void setupShadowMap(vec4 ecPosition)
{
	//ShadowMap Texcoords
    gl_TexCoord[4].s = dot( ecPosition, gl_EyePlaneS[4] );
    gl_TexCoord[4].t = dot( ecPosition, gl_EyePlaneT[4] );
    gl_TexCoord[4].p = dot( ecPosition, gl_EyePlaneR[4] );
    gl_TexCoord[4].q = dot( ecPosition, gl_EyePlaneQ[4] );
}
