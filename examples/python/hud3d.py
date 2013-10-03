# A custom container and attach it to the camera as a 3d head-up display
from omegaToolkit import *
uim = UiModule.createAndInitialize()

hud = Container.create(ContainerLayout.LayoutVertical, uim.getUi())
hud.setStyle('fill: #00000080')
l1 = Label.create(hud)
l2 = Label.create(hud)
l3 = Label.create(hud)

l1.setFont('fonts/arial.ttf 20')
l1.setText("Heads up display test")

l2.setFont('fonts/arial.ttf 14')
l2.setText("Camera position:")

l3.setFont('fonts/arial.ttf 14')

# enable 3d mode for the hud container and attach it to the camera.
c3d = hud.get3dSettings()
c3d.enable3d = True
c3d.position = Vector3(0, 2.5, -2.5)
# Rotate the hud a little. Note that rotation needs to be specified 
# as a vector.
c3d.normal = quaternionFromEulerDeg(0,-30,0) * Vector3(0,0,1)
# Scale is the conversion factor between pixels and meters
c3d.scale = 0.004
c3d.node = getDefaultCamera()


def onUpdate(frame, time, dt):
	l3.setText(str(getDefaultCamera().getPosition()))
setUpdateFunction(onUpdate)