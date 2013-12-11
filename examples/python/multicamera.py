# This example show how to render content using multiple cameras.
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

# create second camera
c2 = getOrCreateCamera('c2')
c2.setHeadOffset(Vector3(0, 2, 0))

c2output = PixelData.create(250,250,PixelFormat.FormatRgba)
c2.getOutput(0).setReadbackTarget(c2output)
c2.getOutput(0).setEnabled(True)

# create a movable window displaying the output of the second camera.
uim = UiModule.createAndInitialize()
container = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
container.setStyleValue('fill', 'black')
container.setAlpha(1)
titleBar = Label.create(container)
titleBar.setText("Second Camera")
titleBar.setPinned(True)
titleBar.setDraggable(True)
titleBar.setVisible(True)
titleBar.setAutosize(False)
titleBar.setStyleValue('fill', '#000000ff')
titleBar.setHeight(24)

img = Image.create(container)
img.setData(c2output)