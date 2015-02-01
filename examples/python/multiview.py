# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *

box = BoxShape.create(0.8, 0.8, 0.8)
box.setPosition(Vector3(0, 2, -3))

# Apply an emissive textured effect (no lighting)
box.setEffect("textured -v emissive -d cyclops/test/omega-transparent.png")

# Spin the box!
def onUpdate(frame, t, dt):
	box.pitch(dt)
	box.yaw(dt / 3)
setUpdateFunction(onUpdate)

c1 = getDefaultCamera()
c1.setViewSize(0.5,1)

# create second camera
c2 = getOrCreateCamera('c2')
c2.setOverlayEnabled(True)
c2.setHeadOffset(Vector3(0, 2, 0))
c2.setViewSize(0.5,1)
c2.setViewPosition(0.5,0)

# add a new wireframe material to the box object, that will only be drawn in camera 2
m2 = Material.create()
m2.parse('colored -e red -w -C')
m2.setCamera(c2)
box.addMaterial(m2)
# Camera 2 will draw the box using only the explicit material associated with it
# (that is, m2)
c2.setFlag(Material.CameraDrawExplicitMaterials)

# make the two cameras move together
c1.addChild(c2)