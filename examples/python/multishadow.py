from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

# define some colors
# light1Color = Color("#ff5555")
# light2Color = Color("#55ff55")
# light3Color = Color("#5555ff")
light1Color = Color("#ff8888")
light2Color = Color("#ffff88")
light3Color = Color("#8888ff")
blackColor = Color("black")
whiteColor = Color("white")

scene = getSceneManager()
scene.setBackgroundColor(blackColor)

getDefaultCamera().setPosition(0,0,2)
getDefaultCamera().pitch(radians(-10))

n = 3
for i in range(0, n):
	for j in range(0, n):
		for k in range(0, n):
			x = float(i) / n * 2 - 1
			y = float(j) / n * 2 - 1
			z = float(k) / n * 2 - 1
			s = 0.8 / n
			sphere = SphereShape.create(s, 2)
			sphere.setPosition(Vector3(x + 0.5, y + 1, -4.5 + z))
			sphere.setEffect('colored -d white -s 10 -g 1.0')

# sphere1 = SphereShape.create(0.5, 4)
# sphere1.setPosition(-1, 1, -4)
# sphere1.setEffect("colored -d white -s 10 -g 1.0")

# sphere2 = SphereShape.create(0.5, 4)
# sphere2.setPosition(0, 1, -4)
# sphere2.setEffect("colored -d white -s 10 -g 1.0")

# sphere3 = SphereShape.create(0.5, 4)
# sphere3.setPosition(1, 1, -4)
# sphere3.setEffect("colored -d white -s 10 -g 1.0")

# Create first light, light sphere and interactor
light1 = Light.create()
light1.setColor(light1Color)
light1.setAmbient(Color("#202020"))
light1.setEnabled(True)
light1.setAttenuation(1, 0.1, 0.05)
lightSphere1 = BoxShape.create(0.05, 0.1, 0.05)
lightSphere1.setEffect("colored -d black -e white")
lightSphere1.setPosition(Vector3(-1, 2, -4))
interactor1 = ToolkitUtils.setupInteractor("config/interactor")
interactor1.setSceneNode(lightSphere1)
lightSphere1.addChild(light1)
lightSphere1.castShadow(False)

# Create second light, light sphere and interactor
light2 = Light.create()
light2.setColor(light2Color)
light2.setAmbient(Color("#202020"))
light2.setEnabled(True)
light2.setAttenuation(1, 0.1, 0.05)
lightSphere2 = BoxShape.create(0.05, 0.1, 0.05)
lightSphere2.setEffect("colored -d black -e white")
lightSphere2.setPosition(Vector3(0, 2, -3))
interactor2 = ToolkitUtils.setupInteractor("config/interactor")
interactor2.setSceneNode(lightSphere2)
lightSphere2.addChild(light2)
lightSphere2.castShadow(False)

# Create third light, light sphere and interactor
light3 = Light.create()
light3.setColor(light3Color)
light3.setAmbient(Color("#202020"))
light3.setEnabled(True)
light3.setAttenuation(1, 0.1, 0.05)
lightSphere3 = BoxShape.create(0.05, 0.1, 0.05)
lightSphere3.setEffect("colored -d black -e white")
lightSphere3.setPosition(Vector3(1, 2, -4))
interactor3 = ToolkitUtils.setupInteractor("config/interactor")
interactor3.setSceneNode(lightSphere3)
lightSphere3.addChild(light3)
lightSphere3.castShadow(False)

light1.setLightType(LightType.Spot)
light2.setLightType(LightType.Spot)
light3.setLightType(LightType.Spot)
light1.setSpotCutoff(50)
light2.setSpotCutoff(50)
light3.setSpotCutoff(50)
light1.setSpotExponent(10)
light2.setSpotExponent(10)
light3.setSpotExponent(10)
light1.setLightDirection(Vector3(0, 0, -1))
light2.setLightDirection(Vector3(0, 0, -1))
light3.setLightDirection(Vector3(0, 0, -1))

sm1 = ShadowMap()
sm2 = ShadowMap()
sm3 = ShadowMap()
sm1.setTextureSize(1024,1024)
sm2.setTextureSize(1024,1024)
sm3.setTextureSize(1024,1024)
light1.setShadow(sm1)
light2.setShadow(sm2)
light3.setShadow(sm3)

# create ground
plane = PlaneShape.create(10, 10)
plane.setPosition(Vector3(0, 0, -4))
plane.pitch(radians(-90))
plane.setEffect("colored -d white")


# create a main menu to turn on and off lights
mm = MenuManager.createAndInitialize()
lightmnu = mm.getMainMenu()
lightmnu.addButton("Toggle red light", "toggleLight1()")
lightmnu.addButton("Toggle green light", "toggleLight2()")
lightmnu.addButton("Toggle blue light", "toggleLight3()")

#------------------------------------------------------------------------------
# Functions to toggle lights
# we define one function for each light, each one switched the light state 
# on / off. To add some eye candy, we also change the light sphere color, when 
# the light is off.
def toggleLight1():
	light1.setEnabled(not light1.isEnabled())
	if(light1.isEnabled()):
		lightSphere1.getMaterial().setColor(blackColor, light1Color)
	else:
		lightSphere1.getMaterial().setColor(whiteColor, blackColor)

def toggleLight2():
	light2.setEnabled(not light2.isEnabled())
	if(light2.isEnabled()):
		lightSphere2.getMaterial().setColor(blackColor, light2Color)
	else:
		lightSphere2.getMaterial().setColor(whiteColor, blackColor)
		
def toggleLight3():
	light3.setEnabled(not light3.isEnabled())
	if(light3.isEnabled()):
		lightSphere3.getMaterial().setColor(blackColor, light3Color)
	else:
		lightSphere3.getMaterial().setColor(whiteColor, blackColor)

#------------------------------------------------------------------------------
# Update function
def onUpdate(frame, time, dt):
	lightSphere1.lookAt(Vector3(0, 1, -4), Vector3(1, 0, 0))
	lightSphere2.lookAt(Vector3(0, 1, -4), Vector3(1, 0, 0))
	lightSphere3.lookAt(Vector3(0, 1, -4), Vector3(1, 0, 0))
	# little trick: wait 5 frames then set light to lazy
	# shadow update to increase performance (works because all 
	# shadow casting objects in the scene are static).
	# we wait a few frames before doing this to make sure the
	# scene is fully populated. 
	if(frame == 5):
		light1.setShadowRefreshMode(ShadowRefreshMode.OnLightMove)
		light2.setShadowRefreshMode(ShadowRefreshMode.OnLightMove)
		light3.setShadowRefreshMode(ShadowRefreshMode.OnLightMove)
	
	
setUpdateFunction(onUpdate)

#------------------------------------------------------------------------------
# Event function
def onEvent():
	# Use keys to turn on / of lights
	e = getEvent()
	if(e.isKeyDown(ord('1'))): toggleLight1()
	if(e.isKeyDown(ord('2'))): toggleLight2()
	if(e.isKeyDown(ord('3'))): toggleLight3()

# register the event function
setEventFunction(onEvent)
