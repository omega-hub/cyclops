from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

scene = getSceneManager()

# define some colors
light1Color = Color("#ff5555")
light2Color = Color("#55ff55")
light3Color = Color("#5555ff")
blackColor = Color("black")
whiteColor = Color("white")

# Create first light, light sphere and interactor
light1 = Light.create()
light1.setColor(Color(0.8,0.8,0.1,1))
light1.setAmbient(Color(0.1,0.1,0.1,1))
light1.setEnabled(True)
light1.setAttenuation(1, 0.1, 0)

lightSphere1 = SphereShape.create(0.1, 4)
lightSphere1.setEffect("colored -d black -e yellow")
lightSphere1.setPosition(Vector3(-1, 3, -4))
interactor1 = ToolkitUtils.setupInteractor("config/interactor")
interactor1.setSceneNode(lightSphere1)
lightSphere1.addChild(light1)
lightSphere1.castShadow(False)

dirSphere1 = SphereShape.create(0.05, 4)
dirSphere1.setEffect("colored -d black -e blue")
dirSphere1.setPosition(Vector3(-1, 0, 0))
lightSphere1.addChild(dirSphere1)
interactor2 = ToolkitUtils.setupInteractor("config/interactor")
interactor2.setSceneNode(dirSphere1)
dirSphere1.castShadow(False)

ls = LineSet.create()
ls.setEffect("colored -d black -e blue")
ls.castShadow(False)
l1 = ls.addLine()
l1.setStart(Vector3(0,0,0))
lightSphere1.addChild(ls)

sm = ShadowMap()
light1.setShadow(sm)

# create ground
floor = PlaneShape.create(10, 10)
floor.setPosition(Vector3(0, 0, -4))
floor.pitch(radians(-90))
floor.setEffect("colored -d white")

back = PlaneShape.create(10, 10)
back.setPosition(Vector3(0, 0, -8))
back.setEffect("colored -d white")

left = PlaneShape.create(10, 10)
left.setPosition(Vector3(4, 0, -4))
left.yaw(radians(-90))
left.setEffect("colored -d white")

box = BoxShape.create(0.5, 2, 0.5)
box.setPosition(0, 0, -4)
box.setEffect("colored -d #808080")

c = getDefaultCamera()
c.setPosition(0, 3, 5)
c.pitch(radians(-20))

#------------------------------------------------------------------------------
# Light parameters
# attenuation
a1 = 1
a2 = 0.1
a3 = 0.1
shadowEnabled = True
shadowResolution = 512

def updateLight():
	light1.setAttenuation(a1, a2, a3)
	updateUi()
	
#------------------------------------------------------------------------------
# User Interface
ui = UiModule.createAndInitialize().getUi()
c = Container.create(ContainerLayout.LayoutVertical, ui)
c.setHorizontalAlign(HAlign.AlignLeft)
c.setPosition(Vector2(5, 25))
c.setLayer(WidgetLayer.Back)
c.setFillEnabled(True)
c.setFillColor(Color(0,0,0,0.5))
lightType = Label.create(c)
lightAtt = Label.create(c)
shadowInfo = Label.create(c)

def updateUi():
	lightType.setText("Light type (1): Point (2): Directional (3): Spot")
	lightAtt.setText("Attenuation (P|p)constant:" + str(a1) + " (O|o)linear:" + str(a2) + " (I|i)quadratic:" + str(a3))
	shadowInfo.setText("(u)Shadow Enabled:" + str(shadowEnabled) + " (Y|y)Resolution: " + str(shadowResolution) + "x" + str(shadowResolution))
	
updateUi()

#------------------------------------------------------------------------------
# Event function
def onEvent():
	global a1, a2, a3
	global shadowEnabled, shadowResolution
	# Use keys to turn on / of lights
	e = getEvent()
	# Shadow toggle
	if(e.isKeyDown(ord('u'))):
		shadowEnabled = not shadowEnabled
		if(shadowEnabled):
			light1.setShadow(sm)			
		else:
			light1.setShadow(None)
		updateUi()
	# Shadow resolution
	elif(e.isKeyDown(ord('Y'))):
		shadowResolution = shadowResolution * 2
		if(shadowResolution > 4096): shadowResolution = 4096
		sm.setTextureSize(shadowResolution, shadowResolution)
		updateUi()
	elif(e.isKeyDown(ord('y'))):
		shadowResolution = shadowResolution / 2
		if(shadowResolution < 32): shadowResolution = 32
		sm.setTextureSize(shadowResolution, shadowResolution)
		updateUi()
	# Light type
	elif(e.isKeyDown(ord('1'))):
		light1.setLightType(LightType.Point)
	elif(e.isKeyDown(ord('2'))):
		light1.setLightType(LightType.Directional)
	elif(e.isKeyDown(ord('3'))):
		light1.setLightType(LightType.Spot)
	# Light attenuation
	elif(e.isKeyDown(ord('P'))):
		a1 = a1 + 0.1
		updateLight()
	elif(e.isKeyDown(ord('p'))):
		a1 = a1 - 0.1
		if(a1 < 0): a1 = 0
		updateLight()
	elif(e.isKeyDown(ord('O'))):
		a2 = a2 + 0.1
		updateLight()
	elif(e.isKeyDown(ord('o'))):
		a2 = a2 - 0.1
		if(a2 < 0): a2 = 0
		updateLight()
	elif(e.isKeyDown(ord('I'))):
		a3 = a3 + 0.1
		updateLight()
	elif(e.isKeyDown(ord('i'))):
		a3 = a3 - 0.1
		if(a3 < 0): a3 = 0
		updateLight()

# register the event function
setEventFunction(onEvent)

def onUpdate(frame, time, dt):
	l1.setEnd(dirSphere1.getPosition())
	# compute a new light direction and spot cutoff
	dir = dirSphere1.getPosition()
	l = dir.magnitude()
	if(l > 1): l = 1
	dir.normalize()
	light1.setLightDirection(-dir)
	#print(l)
	light1.setSpotCutoff(l * 90)
setUpdateFunction(onUpdate)