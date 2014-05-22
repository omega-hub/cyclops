# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *
from math import *

l1 = Light.create()
l1.setColor(Color(0.5,0.5,0.4,1))
l1.setAmbient(Color(0.1,0.1,0.15,1))
l1.setPosition(0, 2, 2)
l1.setLightType(LightType.Directional)
l1.setLightDirection(Vector3(0, 0, -2).normalize())

l2 = Light.create()
l2.setColor(Color(0.4,0.4,0.5,1))
l2.setPosition(4, 5, -1)
#l2.setLightType(LightType.Directional)
#l2.setLightDirection(Vector3(-0.5, -2, -1).normalize())

#s1 = ShadowMap()
#s1.setTextureSize(2048,2048)
#l1.setShadow(s1)

s4 = ShadowMap()
s4.setTextureSize(2048,2048)
s4.setSoft(True)
s4.setSoftShadowParameters(0.004, 1)
l2.setShadow(s4)

linearWallFriction = 5
rollWallFriction = 0.1

ground = BoxShape.create(10, 0.1, 50)
ground.setPosition(0, 0.05, -3)
ground.getRigidBody().initialize(RigidBodyType.Box, 0)
ground.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
ground.getRigidBody().sync()
ground.setEffect('colored -d white')

back = BoxShape.create(12, 10, 1)
back.setPosition(0, 4, -10)
back.getRigidBody().initialize(RigidBodyType.Box, 0)
back.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
back.getRigidBody().sync()
back.setEffect('colored -d white')

lside = BoxShape.create(1, 10, 10)
lside.setPosition(-5, 4, -3)
lside.getRigidBody().initialize(RigidBodyType.Box, 0)
lside.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
lside.getRigidBody().sync()
lside.setEffect('colored -d white')

rside = BoxShape.create(1, 10, 10)
rside.setPosition(5, 4, -3)
rside.getRigidBody().initialize(RigidBodyType.Box, 0)
rside.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
rside.getRigidBody().sync()
rside.setEffect('colored -d white')


getSceneManager().setGravity(Vector3(0, -4.5, 0))
getSceneManager().setPhysicsEnabled(True)

# set the camera position
getDefaultCamera().focusOn(ground)
getDefaultCamera().setPosition(0, 2, 10)

boxw = 0.1
boxh = 0.1
boxd = 0.1

startx = 0
starty = boxh / 2 + 0.1
startz = -3

boxesx = 7
boxesy = 7
boxesz = 1

boxRoot = SceneNode.create('boxRoot')
bulletRoot = SceneNode.create('bulletRoot')

#-------------------------------------------------------------------------------
def spawn():
    for x in range(0, boxesx):
        for y in range(0, boxesy):
            for z in range(0, boxesz):
                b = BoxShape.create(boxw, boxh, boxd)
                
                px = startx + x * boxw
                py = starty + y * boxh
                pz = startz + z * boxd
                
                b.setPosition(px, py, pz)
                b.getRigidBody().initialize(RigidBodyType.Box, 1)
                b.getRigidBody().sync()
                b.getRigidBody().setFriction(5)
                #b.setEffect("textured -d cyclops/test/omega-transparent.png")
                b.setEffect("colored -d #f0f0f0")
                b.castShadow(True)
                boxRoot.addChild(b)
spawn()


#-------------------------------------------------------------------------------
def reset():
    global boxRoot
    global bulletRoot
    getScene().removeChildByRef(boxRoot)
    getScene().removeChildByRef(bulletRoot)
    boxRoot = SceneNode.create('boxRoot')
    bulletRoot = SceneNode.create('bulletRoot')
    spawn()
    

#-------------------------------------------------------------------------------
bulletSize = 0.1
bulletWeight = 1
def throw(pos, dir, speed):
                b = SphereShape.create(bulletSize, 2)
                b.setPosition(pos)
                b.getRigidBody().initialize(RigidBodyType.Sphere, bulletWeight)
                b.getRigidBody().sync()
                #b.getRigidBody().setFriction(1000)
                b.getRigidBody().setRollingFriction(0.5)
                b.setEffect("colored -d red")
                b.castShadow(True)
                b.getRigidBody().applyCentralImpulse(dir * speed)
                bulletRoot.addChild(b)


#-------------------------------------------------------------------------------
def onUpdate(frame, time, dt):
    if(frame < 60):
        getSceneManager().setGravity(Vector3(0, -4.5 * frame / 60, 0))
    if(frame == 60):
        getSceneManager().setGravity(Vector3(0, -4.5, 0))

setUpdateFunction(onUpdate)

#-------------------------------------------------------------------------------
flingMarkers = (-1,-1)
flingStartMarkerPos = Vector3(0,0,0)
flingEndMarkerPos = Vector3(0,0,0)
flingStart = Vector3(0,0,0)
flingDir = Vector3(0,0,-1)
flingSpeed = 20
def updateFlingFromMarkers():
    global flingSpeed
    global flingStart
    global flingDir
    flingStart = flingStartMarkerPos
    flingDir = flingEndMarkerPos - flingStartMarkerPos
    flingSpeed = flingDir.length()
    flingDir.normalize()

#-------------------------------------------------------------------------------
def onEvent():
    global flingMarkers
    global flingStart
    global flingDir
    global flingSpeed
    global flingStartMarkerPos
    global flingEndMarkerPos
    e = getEvent()

    # Set fling start/dir based on mouse position (no speed control)
    if(e.getServiceType() == ServiceType.Pointer):
        r = getRayFromEvent(e)
        flingStart = r[1]
        flingDir = r[2]
    # If we are using 3D fling markers, use them to compute fling start, dir, speed
    if(e.getSourceId() == flingMarkers[0]):
        flingStartMarkerPos = e.getPosition()
        updateFlingFromMarkers()
    elif(e.getSourceId() == flingMarkers[1]):
        flingEndMarkerPos = e.getPosition()
        updateFlingFromMarkers()
    
    # Buttons (reset / throw)
    if(e.isButtonDown(EventFlags.Right)):
        throw(flingStart, flingDir, flingSpeed)
    if(e.isButtonDown(EventFlags.ButtonLeft)):
        reset()
setEventFunction(onEvent)