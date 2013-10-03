from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

ui = UiModule.createAndInitialize()
wf = ui.getWidgetFactory()
overlay = ui.getUi()

starImage = loadImage('data/star.png')

stars = []

label = wf.createLabel('label', overlay, '')
label.setPosition(Vector2(5, 5))

#--------------------------------------------------------------------------------------------------
def onEvent():
	e = getEvent()
	if(e.getServiceType() == ServiceType.Pointer and e.getType() == EventType.Move):
		pos = e.getPosition()
		s = Image.create(overlay)
		s.setData(starImage)
		s.setCenter(Vector2(pos.x, pos.y))
		s.setBlendMode(WidgetBlendMode.BlendAdditive)
		s.setAlpha(1.0)
		s.setScale(1.0)
		stars.append(s)

#--------------------------------------------------------------------------------------------------
def onUpdate(frame, tile, dt):
	label.setText("Stars: " + str(len(stars)))
	
	for s in stars:
		value = s.getAlpha()
		value = value - (value * dt * 0.02)
		if(value <= 0.1): 
			overlay.removeChild(s)
			stars.remove(s)
		else:
			s.setAlpha(value)
			s.setScale((1.0 - value) * 2 + 1)
	
setUpdateFunction(onUpdate)
setEventFunction(onEvent)