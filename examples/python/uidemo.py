from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

# create a background plane
background = PlaneShape.create(4.6, 2.8)
background.setPosition(Vector3(0, 2, -3))
background.setEffect('textured -v emissive -d examples/data/GradientBackground.jpg')

ui = UiModule.createAndInitialize()
wf = ui.getWidgetFactory()
uiroot = ui.getUi()

# Sample window
windowContainer = wf.createContainer('window', uiroot, ContainerLayout.LayoutFree)
windowContainer.setPosition(Vector2(400, 5))

# create the window content image
windowBackground = wf.createImage('windowBackground', windowContainer)
windowBackground.setData(loadImage('examples/data/RBC-details.png'))
windowBackground.setLayer(WidgetLayer.Back)

# load button images
HemoBtnUp = loadImage('examples/data/btns/Hemo-btn.png')
HemoBtnDown = loadImage('examples/data/btns/Hemo-btn-active.png')

# Create a button
button = wf.createImage('button', windowContainer)
button.setData(HemoBtnUp)
button.setPosition(Vector2(5, 400))

# Set the button event handler command
button.setUIEventCommand('onButtonClick()')
buttonSelected = False

# Create an additional label and button to switch between 2d / 3d ui mode
ui3dmode = False
ui3dlabel = wf.createLabel('ui3dlabel', uiroot, 'UI 3D Mode: Disabled')
ui3dlabel.setPosition(Vector2(5, 5))
ui3dbutton = wf.createCheckButton('ui3dbutton', uiroot)
ui3dbutton.setText('UI 3D Mode')
ui3dbutton.setPosition(Vector2(5, 30))
ui3dbutton.setUIEventCommand('onUiModeToggle()')

def onUiModeToggle():
	global ui3dmode
	ui3dmode = ui3dbutton.isChecked()
	if(ui3dmode == True):
		windowContainer.get3dSettings().enable3d = True
		windowContainer.get3dSettings().position = Vector3(0.5, 1.6, -2.5)
		windowContainer.get3dSettings().normal = Vector3(-0.2, 0, 0.9)
		windowContainer.get3dSettings().up = Vector3(0, 1, 0)
		ui3dlabel.setText('UI 3D Mode: Enabled')
	else:
		windowContainer.get3dSettings().enable3d = False
		ui3dlabel.setText('UI 3D Mode: Disabled')

def onButtonClick():
	global button
	global buttonSelected
	e = getEvent()
	if(e.getType() == EventType.Down):
		if(buttonSelected):
			button.setData(HemoBtnUp)
			buttonSelected = False
		else:
			button.setData(HemoBtnDown)
			buttonSelected = True
