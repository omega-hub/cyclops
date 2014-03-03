# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *

l1 = Light.create()
l1.setColor(Color(0.5,0.5,0.4,1))
l1.setAmbient(Color(0.1,0.1,0.15,1))
l1.setPosition(-2, 2, 0)
l1.setLightType(LightType.Directional)
l1.setLightDirection(Vector3(-0.5, -2, -2).normalize())

l2 = Light.create()
l2.setColor(Color(0.4,0.4,0.5,1))
l2.setPosition(2, 4, 0)
l2.setLightType(LightType.Directional)
l2.setLightDirection(Vector3(-0.5, -2, -1).normalize())

s1 = ShadowMap()
s1.setTextureSize(2048,2048)
l1.setShadow(s1)

s4 = ShadowMap()
s4.setTextureSize(2048,2048)
l2.setShadow(s4)

ground = BoxShape.create(10, 0.1, 10)
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
spinner.castShadow(True)
# set the camera position
getDefaultCamera().setPosition(0, 1, 2)
getDefaultCamera().lookAt(ground.getPosition(), Vector3(0, 1, 0))

def spawn():
	for i in range(0, 10):
		box = BoxShape.create(0.1, 0.1, 0.1)
		box.setPosition(Vector3(0, 3 + i / 5.0, -3))
		box.roll(2 + i / 0.5)
		box.pitch(2 + i / 0.5)
		box.getRigidBody().initialize(RigidBodyType.Box, 1)
		box.getRigidBody().sync()
		box.setEffect("textured -d cyclops/test/omega-transparent.png")
		box.castShadow(True)
spawn()

# Spin the spinner!
def onUpdate(frame, t, dt):
	spinner.yaw(dt/2)
	spinner.getRigidBody().sync()
setUpdateFunction(onUpdate)
