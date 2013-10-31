///////////////////////////////////////////////////////////////////////////////
float computeShadowMap(sampler2DShadow shadowTexture, vec4 sceneShadowProj)
{
	return shadow2DProj(shadowTexture, sceneShadowProj).x;
}
