///////////////////////////////////////////////////////////////////////////////
$@vertexShadowSection
{
    gl_TexCoord[@shadowUnit].s = dot( eyeSpacePosition, gl_EyePlaneS[@shadowUnit] );
    gl_TexCoord[@shadowUnit].t = dot( eyeSpacePosition, gl_EyePlaneT[@shadowUnit] );
    gl_TexCoord[@shadowUnit].p = dot( eyeSpacePosition, gl_EyePlaneR[@shadowUnit] );
    gl_TexCoord[@shadowUnit].q = dot( eyeSpacePosition, gl_EyePlaneQ[@shadowUnit] );
}
$