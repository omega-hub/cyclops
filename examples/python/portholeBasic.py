# Basic example showing use of the porthole streaming HTML5 interface
from omega import *
from cyclops import *

import porthole

porthole.initialize()
porthole.getService().setConnectedCommand("print('client connected: %id%')")
porthole.getService().setDisconnectedCommand("print('client disconnected: %id%')")
porthole.getService().setCameraCreatedCommand("onPortholeCameraCreated(%id%)")
porthole.getService().setCameraDestroyedCommand("onPortholeCameraDestroyed(%id%)")

def onPortholeCameraCreated(id):
	print('Porthole camera created: ' + str(id))
	cam = getCameraById(id)
	# Diable overlay (2D) stuff for the porthole camera. Just draw the 3D scene.
	cam.setOverlayEnabled(False)
	# Attach the porthole camera to the default one, to follow it.
	getDefaultCamera().addChild(cam)

def onPortholeCameraDestroyed(id):
	cam = getCameraById(id)
	getDefaultCamera().removeChild(cam)

box = BoxShape.create(0.8, 0.8, 0.8)
box.setPosition(Vector3(0, 2, -3))

# Apply an emissive textured effect (no lighting)
box.setEffect("textured -v emissive -d cyclops/test/omega-transparent.png")

# Spin the box!
def onUpdate(frame, t, dt):
	global btn
	box.pitch(dt)
	box.yaw(dt / 3)
	
setUpdateFunction(onUpdate)

def onEvent():
	print(getEvent().getPosition())
setEventFunction(onEvent)

