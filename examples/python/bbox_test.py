# Basic example showing how to load a static model, and display it in a basic scene.
from math import *
from euclid import *
from omega import *
from cyclops import *

scene = getSceneManager()

# Load a static model
torusModel = ModelInfo()
torusModel.name = "torus"
torusModel.path = "cyclops/test/torus.fbx"
torusModel.size = 1.0
scene.loadModel(torusModel)

# Create a scene object using the loaded model
torus = StaticObject.create("torus")
torus.setPosition(Vector3(0, 2, -4))
torus.pitch(radians(45))
torus.setEffect("colored")

# Draw a ground plane
plane = PlaneShape.create(10, 10)
plane.setPosition(Vector3(0, 1, -4))
plane.pitch(radians(-90))
plane.setEffect("colored -d gray")

# Create first light
light1 = Light.create()
light1.setColor(Color(1, 0.8, 0.8, 1))
light1.setPosition(Vector3(0, 0, -4))
light1.setEnabled(True)

# Create second light
light2 = Light.create()
light2.setColor(Color(0.8, 0.8, 0.1, 1))
light2.setPosition(Vector3(0, 4, -4))
light2.setLightType(LightType.Spot)
light2.setLightDirection(Vector3(0, -1, 0))
light2.setSpotCutoff(30)
light2.setSpotExponent(20)
light2.setEnabled(True)


# Spin the box!
def onUpdate(frame, t, dt):
	torus.pitch(dt)
	torus.yaw(dt / 3)
setUpdateFunction(onUpdate)

import bbox
bbox.BBox(torus)
