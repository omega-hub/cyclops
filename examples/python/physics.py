# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *

l1 = Light.create()
l1.setEnabled(True)
l1.setColor(Color(0.5,0.5,0.4,1))
l1.setPosition(-2, 2, -4)
l2 = Light.create()
l2.setEnabled(True)
l2.setColor(Color(0.4,0.4,0.5,1))
l2.setPosition(2, 2, 4)

ground = BoxShape.create(1.5, 0.1, 2)
ground.setPosition(0, 0, -3)
ground.getRigidBody().initialize(RigidBodyType.Box, 0)
ground.getRigidBody().sync()
ground.setEffect('colored -d white')

getSceneManager().setGravity(Vector3(0, -0.8, 0))
getSceneManager().setPhysicsEnabled(True)

spinner = BoxShape.create(1.5, 0.3, 0.1)
spinner.setPosition(0, 0, -3)
spinner.getRigidBody().initialize(RigidBodyType.Box, 0)
spinner.getRigidBody().setUserControlled(True)
spinner.setEffect('colored -d yellow')
# set the camera position
getDefaultCamera().focusOn(ground)
getDefaultCamera().setPosition(0, 1, 2)

def spawn():
	for i in range(0, 10):
		box = BoxShape.create(0.1, 0.1, 0.1)
		box.setPosition(Vector3(0, 3 + i / 5.0, -3))
		box.roll(2 + i / 0.5)
		box.pitch(2 + i / 0.5)
		box.getRigidBody().initialize(RigidBodyType.Box, 1)
		box.getRigidBody().sync()
		box.setEffect("textured -d cyclops/test/omega-transparent.png")

spawn()

# Spin the spinner!
def onUpdate(frame, t, dt):
	spinner.yaw(dt/2)
	spinner.getRigidBody().sync()
setUpdateFunction(onUpdate)
