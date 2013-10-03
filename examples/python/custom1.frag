uniform float unif_Shininess;
uniform float unif_Gloss;

///////////////////////////////////////////////////////////////////////////////////////////////////
void main (void)
{
	gl_FragColor.rgb = gl_Color.rgb;	
	gl_FragColor.a = sin(gl_Position.y + unif_Shininess) * unif_Gloss;
}
