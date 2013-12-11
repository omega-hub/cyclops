# This example show how to render content using multiple cameras, and how to 
# customize object appearance using per-camera materials.
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

def createSecondaryCameraWindow(id, windowName, width, height, x, y):
    # create second camera
    cam = getOrCreateCamera(id)
    cam.setHeadOffset(Vector3(0, 2, 0))

    coutput = PixelData.create(width,height,PixelFormat.FormatRgba)
    cam.getOutput(0).setReadbackTarget(coutput)
    cam.getOutput(0).setEnabled(True)

    # create a movable window displaying the output of the second camera.
    uim = UiModule.createAndInitialize()
    container = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
    container.setStyleValue('fill', 'black')
    container.setPosition(Vector2(x, y))
    container.setAlpha(1)
    titleBar = Label.create(container)
    titleBar.setText(windowName)
    titleBar.setPinned(True)
    titleBar.setDraggable(True)
    titleBar.setVisible(True)
    titleBar.setAutosize(False)
    titleBar.setStyleValue('fill', '#000000ff')
    titleBar.setHeight(24)

    img = Image.create(container)
    img.setData(coutput)
    
    return cam
    
# create two secondary camera windows
c1 = createSecondaryCameraWindow('c1', 'Camera 1', 250, 250, 5, 25)
c2 = createSecondaryCameraWindow('c2', 'Camera 2 (custom render)', 250, 250, 270, 25)

# add a new wireframe material to the box object, that will only be drawn in camera 2
m2 = Material.create()
m2.parse('colored -e red -w -C')
m2.setCamera(c2)
box.addMaterial(m2)
# Camera 2 will draw the box using only the explicit material associated with it
# (that is, m2)
c2.setFlag(Material.CameraDrawExplicitMaterials)