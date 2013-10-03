from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

ui = UiModule.createAndInitialize()
wf = ui.getWidgetFactory()
overlay = ui.getUi()

circleImage = loadImage('examples/data/circle.png')

circles = []
	
#--------------------------------------------------------------------------------------------------
def onEvent():
	e = getEvent()
	if(e.getServiceType() == ServiceType.Pointer and e.getType() == EventType.Down):
		pos = e.getPosition()
		circle = Image.create(circleImage, overlay)
		circle.setCenter(Vector2(pos.x, pos.y))
		circle.setBlendMode(WidgetBlendMode.BlendAdditive)
		circle.setAlpha(1.0)
		circle.setScale(0.2)
		circles.append(circle)

#--------------------------------------------------------------------------------------------------
def onUpdate(frame, tile, dt):
	for circle in circles:
		value = circle.getAlpha()
		value = value - (value * dt * 0.1)
		if(value <= 0.1): 
			overlay.removeChild(circle)
			circles.remove(circle)
		else:
			circle.setAlpha(value)
			circle.setScale((1.0 - value) * 0.5 + 0.2)
	
setUpdateFunction(onUpdate)
setEventFunction(onEvent)