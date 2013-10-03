# This example shows how to use the shader post lighting section.
# It uses the setShaderMacro functions to add depth fog to all rendered entities.
# The fog scale and color is controlled through two uniforms added to the definition section
# of the shader using the customFragmentDefs macro.
from math import *
from euclid import *
from omega import *
from cyclops import *

scene = getSceneManager()
scene.setShaderMacroToString('customFragmentDefs', '''
	// Add a fog scale and color uniforms
	uniform float unif_FogScale;
	uniform vec4 unif_FogColor;
''')
scene.setShaderMacroToString('postLightingSection', '''
	litSurfData.luminance.a *= unif_Alpha;
	litSurfData.luminance.rgb = mix(litSurfData.luminance.rgb, unif_FogColor.rgb, clamp(var_EyeVector.z * unif_FogScale, 0, 1));
''')

# set uniforms
fogScale = scene.getGlobalUniforms().addUniform("unif_FogScale", UniformType.Float)
fogColor = scene.getGlobalUniforms().addUniform("unif_FogColor", UniformType.Color)

grayColor = Color(0.6, 0.6, 0.6, 1.0)

fogScale.setFloat(-0.1)
fogColor.setColor(grayColor)

scene.setBackgroundColor(grayColor)

plane = PlaneShape.create(10, 10)
plane.setPosition(Vector3(0, 0, -4))
plane.pitch(radians(-90))
plane.setEffect("colored -d yellow")

scenelight = Light.create()
scenelight.setColor(Color('#555555'))
scenelight.setAmbient(Color("black"))
scenelight.setPosition(Vector3(0, 10, -4))
scenelight.setEnabled(True)

# Create first light, light sphere and interactor
dslight = Light.create()
dslight.setColor(Color('#555555'))
dslight.setAmbient(Color("black"))
dslight.setEnabled(True)
dslight.setPosition(Vector3(0, 3, 3))

sphere = SphereShape.create(0.5, 4)
sphere.setEffect("colored -d blue -g 2.0 -s 20")
sphere.setPosition(Vector3(-1, 3, -4))
interactor = ToolkitUtils.setupInteractor("config/interactor")
interactor.setSceneNode(sphere)



