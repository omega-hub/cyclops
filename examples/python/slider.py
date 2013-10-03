# Example implementation of a slider, using custom images instead of the widget implementation
from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

# disable camera navigation
getDefaultCamera().setControllerEnabled(False)

# create a background plane
background = PlaneShape.create(4.6, 2.8)
background.setPosition(Vector3(0, 2, -3))
background.setEffect('textured -v emissive -d data/GradientBackground.jpg')

ui = UiModule.createAndInitialize()
wf = ui.getWidgetFactory()
uiroot = ui.getUi()

# slider container
sliderContainer = wf.createContainer('sliderContainer', uiroot, ContainerLayout.LayoutFree)
sliderContainer.setPosition(Vector2(100, 400))

# create the slider background
sliderBackground = wf.createImage('sliderBackground', sliderContainer)
sliderBackground.setData(loadImage('data/timesliderbar.png'))
sliderBackground.setLayer(WidgetLayer.Back)

# Create the slider element
slider = wf.createImage('button', sliderContainer)
slider.setData(loadImage('data/timesliderbtn.png'))
slider.setCenter(Vector2(16, 24))
#slider.setScale(0.1)
sliderPressed = False
sliderValue = 0

# Create a text item to print the value of the slider
label = wf.createLabel('label', uiroot, 'Slider Value: ' + str(sliderValue))
label.setPosition(Vector2(5, 5))

#--------------------------------------------------------------------------------------------------
def updateSliderPosition():
        sliderPos = slider.getCenter()
        sliderPos.x = sliderValue * sliderBackground.getSize().x / 100
        slider.setCenter(sliderPos)
        label.setText('Slider Value: ' + str(sliderValue))
        
#--------------------------------------------------------------------------------------------------
def updateSliderValue(pos):
	global sliderValue
        point = sliderBackground.transformPoint(pos)
        sliderValue = point.x / sliderBackground.getSize().x * 100
        if(sliderValue < 0): sliderValue = 0
        if(sliderValue > 100): sliderValue = 100
        updateSliderPosition()
        
#--------------------------------------------------------------------------------------------------
def onEvent():
	global sliderPressed
	e = getEvent()
	mousePos = Vector2(e.getPosition().x, e.getPosition().y)
	if(e.getType() == EventType.Move):
		if(sliderPressed == True):
			updateSliderValue(mousePos)
	elif(e.getType() == EventType.Down):
		if(sliderBackground.hitTest(mousePos)):
			sliderPressed = True
			updateSliderValue(mousePos)
	elif(e.getType() == EventType.Up):
		sliderPressed = False
	
setEventFunction(onEvent)
