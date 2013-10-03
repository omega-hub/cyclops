from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

scene = getSceneManager()

#ss = ShadowSettings()
#ss.shadowsEnabled = True
#scene.resetShadowSettings(ss)

# define some colors
light1Color = Color("#ff5555")
light2Color = Color("#55ff55")
light3Color = Color("#5555ff")
blackColor = Color("black")

sphere = SphereShape.create(0.5, 4)
sphere.setPosition(Vector3(0, 2, -4))
sphere.setEffect("colored -d #00ffff50 -t -s 10 -g 1.0")

plane = PlaneShape.create(10, 10)
plane.setPosition(Vector3(0, 0, -4))
plane.pitch(radians(-90))
plane.setEffect("colored -d gray")

# Create first light, light sphere and interactor
light1 = Light.create()
light1.setColor(light1Color)
light1.setAmbient(Color("#200000"))
light1.setEnabled(True)
lightSphere1 = SphereShape.create(0.1, 4)
lightSphere1.setEffect("colored -d black -e #ff5555")
lightSphere1.setPosition(Vector3(-1, 3, -4))
interactor1 = ToolkitUtils.setupInteractor("config/interactor")
interactor1.setSceneNode(lightSphere1)
lightSphere1.addChild(light1)
lightSphere1.castShadow(False)

menu = lightSphere1.createContextMenu()
menu.addButton("Toggle", "toggleLight1()")


# Create second light, light sphere and interactor
light2 = Light.create()
light2.setColor(light2Color)
light2.setAmbient(Color("#002000"))
light2.setEnabled(True)
lightSphere2 = SphereShape.create(0.1, 4)
lightSphere2.setEffect("colored -d black -e #55ff55")
lightSphere2.setPosition(Vector3(0, 3, -4))
interactor2 = ToolkitUtils.setupInteractor("config/interactor")
interactor2.setSceneNode(lightSphere2)
lightSphere2.addChild(light2)
lightSphere2.castShadow(False)

menu = lightSphere2.createContextMenu()
menu.addButton("Toggle", "toggleLight2()")

# Create third light, light sphere and interactor
light3 = Light.create()
light3.setColor(light3Color)
light3.setAmbient(Color("#000020"))
light3.setEnabled(True)
lightSphere3 = SphereShape.create(0.1, 4)
lightSphere3.setEffect("colored -d black -e #5555ff")
lightSphere3.setPosition(Vector3(1, 3, -4))
interactor3 = ToolkitUtils.setupInteractor("config/interactor")
interactor3.setSceneNode(lightSphere3)
lightSphere3.addChild(light3)
lightSphere3.castShadow(False)

menu = lightSphere3.createContextMenu()
menu.addButton("Toggle", "toggleLight3()")

# Load a static model
torusModel = ModelInfo()
torusModel.name = "torus"
torusModel.path = "cyclops/test/torus.fbx"
torusModel.size = 1.0
scene.loadModel(torusModel)

# Create a scene object using the loaded model
torus = StaticObject.create("torus")
torus.setPosition(Vector3(-1, 2, -4))
torus.setEffect("colored -d #ffffff50 -t")

# Load an animated model
skelModel = ModelInfo()
skelModel.name = "skelModel"
skelModel.path = "data/surgseq/surgseq3.fbx"
scene.loadModel(skelModel)

# Create a scene object using the loaded animated model, and start the animation
skel = AnimatedObject.create("skelModel")
skel.setPosition(Vector3(2, 1.5, -4))
skel.setScale(Vector3(0.02, 0.02, 0.02))
skel.yaw(radians(-30))
skel.setEffect("textured")
skel.loopAnimation(0)

# create a main menu to turn on and off lights
mm = MenuManager.createAndInitialize()
lightmnu = mm.getMainMenu()
lightmnu.addButton("Toggle red light", "toggleLight1()")
lightmnu.addButton("Toggle green light", "toggleLight2()")
lightmnu.addButton("Toggle blue light", "toggleLight3()")

# Set shadow caster light
scene.setMainLight(light2)

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
		lightSphere1.getMaterial().setColor(light1Color, blackColor)

def toggleLight2():
	light2.setEnabled(not light2.isEnabled())
	if(light2.isEnabled()):
		lightSphere2.getMaterial().setColor(blackColor, light2Color)
	else:
		lightSphere2.getMaterial().setColor(light2Color, blackColor)
		
def toggleLight3():
	light3.setEnabled(not light3.isEnabled())
	if(light3.isEnabled()):
		lightSphere3.getMaterial().setColor(blackColor, light3Color)
	else:
		lightSphere3.getMaterial().setColor(light3Color, blackColor)

#------------------------------------------------------------------------------
# Update function
def onUpdate(frame, t, dt):
	torus.pitch(dt / 2)
	torus.yaw(dt / 4)
	

# register the update function
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
