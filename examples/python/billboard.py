from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

scene = getSceneManager()

# create a floor
plane = PlaneShape.create(10, 10)
plane.setPosition(Vector3(0, 0, -4))
plane.pitch(radians(-90))
plane.setEffect("colored -e yellow")

# create a floating image panel
img1 = PlaneShape.create(1, 1)
img1.setPosition(Vector3(-1, 2, -4))
img1.setEffect("textured -v emissive -d data/surgseq/chestxray.jpg")
img1.setFacingCamera(getDefaultCamera())
# Let the user move the panel around.
interactor1 = ToolkitUtils.setupInteractor("config/interactor")
interactor1.setSceneNode(img1)


# create another image panel, just because.
img2 = PlaneShape.create(1, 0.5)
img2.setPosition(Vector3(1, 2, -4))
img2.setEffect("textured -v emissive -d data/surgseq/vhabs.jpg")
img2.setFacingCamera(getDefaultCamera())
# Let the user move the panel around.
interactor2 = ToolkitUtils.setupInteractor("config/interactor")
interactor2.setSceneNode(img2)
