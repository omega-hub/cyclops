# Basic example showing how to load a static model, and display it in a basic scene.
from math import *
from euclid import *
from omega import *
from cyclops import *

scene = getSceneManager()

cam = getDefaultCamera()

# Create first light
light1 = Light.create()
light1.setColor(Color(1, 0.8, 0.8, 1))
light1.setPosition(Vector3(0, 0, 0))
light1.setEnabled(True)

cam.addChild(light1)

basePosition = Vector3(2000000, 2000000, 0)

# Load a static model
earth = SphereShape.create(6371000, 4)
earth.setPosition(basePosition)

cam.setPosition(earth.getPosition() + Vector3(0, 0, 20000000))

smallSphere1 = SphereShape.create(0.5, 4)
smallSphere1.setPosition(cam.getPosition() +  Vector3(-1, 1.5, -4))

smallSphere2 = SphereShape.create(0.5, 4)
smallSphere2.setPosition(cam.getPosition() + Vector3(-0.5, 1.5, -4.5))
smallSphere2.setEffect('colored -d green')


i = ToolkitUtils.setupInteractor('config/interactor')
i.setSceneNode(smallSphere2);

setNearFarZ(0.1, 100000000)
