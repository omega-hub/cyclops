# This example show how to render content using multiple cameras, and how to 
# customize object appearance using per-camera materials.
from omega import *
from cyclops import *

l = Light.create()
l.setPosition(1, 2, 0)
l.setColor(Color('white'))
l.setAmbient(Color(0.1,0.1,0.1, 1))

layer1 = SceneLayer()
layer1.setId(SceneLayer.Layer1)

layer2 = SceneLayer()
layer2.setId(SceneLayer.Layer2)

getSceneManager().getLightingLayer().addLayer(layer2)
getSceneManager().getLightingLayer().addLayer(layer1)

# Create a big box and fill it up with spheres. Put the box and spheres in different layers.
box = BoxShape.create(0.8, 0.8, 0.8)
box.setPosition(Vector3(0, 2, -3))
box.setEffect("colored -d green")

# The default camera will display layer 1 (that is, the box)
box.setLayer(layer1)
getDefaultCamera().setFlag(SceneLayer.Layer1)

n = 4
for i in range(0, n):
    for j in range(0, n):
        for k in range(0, n):
            x = float(i) / n * 0.8 - 0.4
            y = float(j) / n * 0.8 - 0.4
            z = float(k) / n * 0.8 - 0.4
            s = 0.16 / n
            sphere = SphereShape.create(s, 2)
            sphere.setPosition(Vector3(x, y, z))
            sphere.setEffect('colored -e red -s 10 -g 1.0')
            box.addChild(sphere)
            sphere.setLayer(layer2)


# Spin the box!
def onUpdate(frame, t, dt):
    box.pitch(dt)
    box.yaw(dt / 3)
setUpdateFunction(onUpdate)

def createSecondaryCameraWindow(id, windowName, width, height, x, y):
    # create second camera
    cam = getOrCreateCamera(id)
    cam.setFlag(SceneLayer.Layer2)
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
    titleBar.setEnabled(True)
    titleBar.setStyleValue('fill', '#000000ff')
    titleBar.setHeight(24)
    #titleBar.setWidth(width)

    img = Image.create(container)
    img.setData(coutput)
    
    return cam
    
# The secondary camera will display layer 2 (that is, the spheres)

c2 = createSecondaryCameraWindow('c2', 'Camera 2', 250, 250, 5, 125)

c1 = createSecondaryCameraWindow('c1', 'Camera 1', 250, 250, 5, 25)

c1.setBackgroundColor(Color('blue'))
c2.setBackgroundColor(Color('lime'))


# attach the secondary camera to the default camera, so they move together.
getDefaultCamera().addChild(c1)
getDefaultCamera().addChild(c2)