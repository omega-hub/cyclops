from math import *
from euclid import *
from omega import *
from cyclops import *

scene = getSceneManager()
scene.setShaderMacroToString('customFragmentDefs', '''
	///////////////////////////////////////////////////////////////////////////////////////////////////
	vec4 dropShadowLightFunction(SurfaceData sd, LightData ld)
	{
		vec4 l = vec4(0, 0, 0, 0);
		float lambertTerm = dot(sd.normal, ld.dir); 
		l += sd.albedo * ld.diffuse; 
		
		vec3 ka = ld.attenuation;
		float att = clamp(0, 1, 1.0 / (ka[0] + ld.distance * ka[1] + ld.distance * ld.distance * ka[2]));
		
		// Scale luminance by attenuation and return
		vec4 luminance = l * (1 - att) * lambertTerm;
		luminance += sd.albedo * ld.ambient;
		return luminance;
	}
''')

plane = PlaneShape.create(10, 10)
plane.setPosition(Vector3(0, 0, -4))
plane.pitch(radians(-90))
plane.setEffect("colored -d white")

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
dslight.setLightType(LightType.Custom)
dslight.setLightFunction('dropShadowLightFunction')
dslight.setAttenuation(0, 1, 1)

sphere = SphereShape.create(0.5, 4)
sphere.setEffect("colored -d blue")
sphere.setPosition(Vector3(-1, 3, -4))
interactor = ToolkitUtils.setupInteractor("config/interactor")
interactor.setSceneNode(sphere)
sphere.addChild(dslight)



