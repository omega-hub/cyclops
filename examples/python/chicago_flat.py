from omega import *
from cyclops import *

scene = getSceneManager()

# Load a static model
torusModel = ModelInfo()
torusModel.name = "torus"
torusModel.path = "chicago_yahoo.earth"
scene.loadModel(torusModel)

# Create a scene object using the loaded model
torus = StaticObject.create("torus")
torus.setEffect("colored")
setNearFarZ(1, 2 * torus.getBoundRadius())

cam = getDefaultCamera()

# Setting the camera by hand. Should find a better way
cam.setPosition(torus.getBoundCenter() + Vector3(7768.82, 2281.18, 7034.08))
cam.getController().setSpeed(300)
