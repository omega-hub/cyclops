from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

# create a background plane
background = PlaneShape.create(4.6, 2.8)
background.setPosition(Vector3(0, 2, -3))
background.setEffect('textured -v emissive -d data/GradientBackground.jpg')

ui = UiModule.createAndInitialize()
wf = ui.getWidgetFactory()
uiroot = ui.getUi()

# Create a label
container = wf.createContainer('container', uiroot, ContainerLayout.LayoutFree)
container.setAutosize(False)
container.setSize(Vector2(120, 120))
label = wf.createLabel('label', container, 'Hello World!!')
label.setPosition(Vector2(5, 5))

# Create a texture from the root ui container contents
container.setPixelOutputEnabled(True)
getSceneManager().createTexture('uiTexture', container.getPixels())

# create a box
box = BoxShape.create(0.5, 0.5, 0.5)
box.setPosition(Vector3(0, 2, -2))
box.setEffect('textured -v emissive -d uiTexture -t -a -D -C')
#box.setEffect('textured -v emissive -d cyclops/test/checker2.jpg')

def onUpdate(frame, time, dt):
	box.yaw(dt / 4)
	box.pitch(dt / 8)
	label.setText("Frame = " + str(frame))

setUpdateFunction(onUpdate)


