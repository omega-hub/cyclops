# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *
from math import *

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


# Make a transparent box
sideThickness = 0.1
sideSize = 2.0
sideEffect = 'colored -d #05050670 -t -C'

box = SceneNode.create('box')
s1 = BoxShape.create(sideSize, sideSize, sideThickness)
s1.setPosition(0, - sideSize / 2, 0)
s1.pitch(radians(90))
s2 = BoxShape.create(sideSize, sideSize, sideThickness)
s2.setPosition(0, sideSize / 2, 0)
s2.pitch(radians(90))
s3 = BoxShape.create(sideSize, sideSize, sideThickness)
s3.setPosition(-sideSize / 2, 0, 0)
s3.yaw(radians(90))
s4 = BoxShape.create(sideSize, sideSize, sideThickness)
s4.setPosition(sideSize / 2, 0, 0)
s4.yaw(radians(90))
s5 = BoxShape.create(sideSize, sideSize, sideThickness)
s5.setPosition(0, 0, -sideSize / 2)
s6 = BoxShape.create(sideSize, sideSize, sideThickness)
s6.setPosition(0, 0, sideSize / 2)
sides = [s1, s2, s3, s4, s5, s6]

for s in sides:
    s.setEffect(sideEffect)
    s.getRigidBody().initialize(RigidBodyType.Box, 0)
    s.getRigidBody().setUserControlled(True)
    box.addChild(s)
    
box.setPosition(0, 2, -3)

# set the camera position
getDefaultCamera().focusOn(ground)
getDefaultCamera().setPosition(0, 3, 2)

def spawn():
    for x in range(-5, 5):
        for y in range(-5, 5):
            for z in range(0, 1):
                b = BoxShape.create(0.1, 0.1, 0.1)
                b.setPosition(Vector3(0 + float(x) / 10.0, 2 + float(y) / 10.0, -3 + float(z) / 10.0))
                b.getRigidBody().initialize(RigidBodyType.Box, 1)
                b.getRigidBody().sync()
                b.setEffect("textured -d cyclops/test/omega-transparent.png")
                b.castShadow(True)
#spawn()

# Spin the spinner!
def onUpdate(frame, t, dt):
    box.yaw(dt/2)
    box.pitch(dt/3)
    box.roll(dt/7)
    for s in sides:
        s.getRigidBody().sync()
setUpdateFunction(onUpdate)
