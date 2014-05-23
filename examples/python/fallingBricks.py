# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *
from math import *

getSceneManager().setBackgroundColor(Color('black'))

l2 = Light.create()
l2.setLightType(LightType.Spot)
l2.setColor(Color(0.4,0.4,0.5,1))
l2.setAmbient(Color(0.05,0.05,0.01,1))
l2.setPosition(4, 5, -1)
l2.setEnabled(True)
l2.setAttenuation(1, 0, 0)
l2.setSpotCutoff(70)
l2.setSpotExponent(5)
l2.setLightDirection(Vector3(-0.5, -2, -1).normalize())

l1 = Light.create()
l1.setColor(Color(0.6,0.6,0.4,1))
l1.setAmbient(Color(0.08,0.08,0.1,1))
l1.setPosition(0, 0, 10)
l1.setLightType(LightType.Directional)
l1.setLightDirection(Vector3(-0.2, -0.2, -2).normalize())

s1 = ShadowMap()
s1.setTextureSize(2048,2048)
s1.setSoft(True)
s1.setSoftShadowParameters(0.001, 1)
l1.setShadow(s1)

s4 = ShadowMap()
s4.setTextureSize(1024,1024)
s4.setSoft(True)
s4.setSoftShadowParameters(0.004, 1)
l2.setShadow(s4)

linearWallFriction = 5
rollWallFriction = 0.1

ground = BoxShape.create(10, 0.1, 10)
ground.setPosition(0, 0.05, -3)
ground.getRigidBody().initialize(RigidBodyType.Box, 0)
ground.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
ground.getRigidBody().sync()
ground.setEffect('textured -d cyclops/test/wall002.jpg')

ground2 = BoxShape.create(10, 0.1, 10)
ground2.setPosition(0, 0.05, 7)
ground2.getRigidBody().initialize(RigidBodyType.Box, 0)
ground2.getRigidBody().setFriction(linearWallFriction)
ground2.getRigidBody().setRollingFriction(rollWallFriction)
ground2.getRigidBody().sync()
ground2.setEffect('textured -d cyclops/test/wall002.jpg')

back = BoxShape.create(12, 10, 1)
back.setPosition(0, 4, -8)
back.getRigidBody().initialize(RigidBodyType.Box, 0)
back.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
back.getRigidBody().sync()
back.setEffect('textured -d cyclops/test/wall002.jpg')

lside = BoxShape.create(1, 10, 10)
lside.setPosition(-5, 4, -3)
lside.getRigidBody().initialize(RigidBodyType.Box, 0)
lside.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
lside.getRigidBody().sync()
lside.setEffect('textured -d cyclops/test/wall002.jpg')

rside = BoxShape.create(1, 10, 10)
rside.setPosition(5, 4, -3)
rside.getRigidBody().initialize(RigidBodyType.Box, 0)
rside.getRigidBody().setFriction(linearWallFriction)
ground.getRigidBody().setRollingFriction(rollWallFriction)
rside.getRigidBody().sync()
rside.setEffect('textured -d cyclops/test/wall002.jpg')

# set the camera position
getDefaultCamera().focusOn(ground)
getDefaultCamera().setPosition(0, 2, 10)

getSceneManager().setGravity(Vector3(0, -4.5, 0))
getSceneManager().setPhysicsEnabled(True)

boxw = 0.6
boxh = 0.6
boxd = 0.6

startx = -2
starty = boxh / 2 + 0.1
startz = -3

boxesx = 9
boxesy = 7
boxesz = 1

boxRoot = SceneNode.create('boxRoot')
bulletRoot = SceneNode.create('bulletRoot')

aimSet = LineSet.create()
aimSet.setEffect('colored -d gray -g 2.0 -s 20')
aim = aimSet.addLine()
aim.setThickness(0.3)

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
                if(x == 4):
                    b.setEffect("colored -d #B1FF9Bf0 -g 2.0 -s 20 -t")
                else:
                    b.setEffect("colored -d #a0a0f8f0 -g 2.0 -s 20 -t")
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
shootDone = False
def shoot():
    global shootDone
    if(not shootDone):
        throw(flingStart, flingDir, flingSpeed)
        shootDone = True

#-------------------------------------------------------------------------------
bulletSize = 0.1
bulletWeight = 1
def throw(pos, dir, speed):
                print("pos:{0} dir:{1} speed:{2}".format(pos, dir, speed))
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
        getSceneManager().setGravity(Vector3(0, -9.8, 0))

setUpdateFunction(onUpdate)

#-------------------------------------------------------------------------------
flingMarkers = (2014,2016)
flingStartMarkerPos = Vector3(0,0,0)
flingEndMarkerPos = Vector3(0,0,0)
flingStart = Vector3(0,0,0)
flingDir = Vector3(0,0,-1)
flingSpeed = 20
def updateFlingFromMarkers():
    global flingSpeed
    global flingStart
    global flingDir
    global flingStartMarkerPos
    global flingEndMarkerPos
    #print("fs:{0}     fe:{1}".format(flingStartMarkerPos, flingEndMarkerPos))
    flingStart = flingStartMarkerPos
    flingDir = flingEndMarkerPos - flingStartMarkerPos
    flingSpeed = flingDir.magnitude() * 100
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
    c = getDefaultCamera()

    # Set fling start/dir based on mouse position (no speed control)
    if(e.getServiceType() == ServiceType.Pointer):
        r = getRayFromEvent(e)
        flingStart = r[1]
        flingDir = r[2]
    # If we are using 3D fling markers, use them to compute fling start, dir, speed
    if(e.getSourceId() == flingMarkers[0]):
        #print(e.getPosition())
        flingStartMarkerPos = c.localToWorldPosition(e.getPosition())
        updateFlingFromMarkers()
    elif(e.getSourceId() == flingMarkers[1]):
        flingEndMarkerPos = c.localToWorldPosition(e.getPosition() - Vector3(0, 0.15, 0))
        updateFlingFromMarkers()
    
    # Buttons (reset / throw)
    if(e.isButtonDown(EventFlags.Right) or e.isButtonDown(EventFlags.Button7)):
        throw(flingStart, flingDir, flingSpeed)
    if(e.isButtonDown(EventFlags.ButtonLeft)):
        reset()
setEventFunction(onEvent)


def onUpdate(frame, time, dt):
    global flingStart
    global flingDir
    global flingSpeed
    global shootDone
    shootDone = False
    if(frame == 2):
        l1.setShadow(None)
    aim.setStart(flingStart)
    aim.setEnd(flingStart + flingDir * 1)
    #aim.setThickness(flingSpeed / 400)
        
setUpdateFunction(onUpdate)
