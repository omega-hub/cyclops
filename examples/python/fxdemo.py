from math import *
from euclid import *
from omega import *
from cyclops import *
from omegaToolkit import *

#--------------------------------------------------------------------------------------------------
# create a sphere and a box
sphere = SphereShape.create(0.5, 3)
sphere.setPosition(Vector3(-1, 1.8, -4))
sphere.setEffect("colored -d blue")
sphere.setSelectable(True)
sphere.setName("Sphere")

box = BoxShape.create(0.5, 0.5, 0.5)
box.setPosition(Vector3(1, 1.8, -4))
box.setEffect("colored -d green")
box.setSelectable(True)
box.setName("Box")

# Place a light
light = Light.create()
light.setColor(Color("white"))
light.setAmbient(Color("#202020"))
light.setEnabled(True)
lightDistance = 0.5

scene = getSceneManager()

# selected will store the currently active object
selected = None

#--------------------------------------------------------------------------------------------------
mm = MenuManager.createAndInitialize()
# disable main menu
mm.setMainMenu(None)
fxmnu = mm.createMenu("fxmenu")

cmnu = fxmnu.addSubMenu("colored")
tmnu = fxmnu.addSubMenu("textured")
xmnu = fxmnu.addSubMenu("custom")
bmnu = fxmnu.addSubMenu("bump")
kmnu = fxmnu.addSubMenu("multipass")

#--------------------------------------------------------------------------------------------------
# colored effect definitions
fx = "colored -d red"; cmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "colored -d #ff404050 -t"; cmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "colored -d blue -g 1.0 -s 30"; cmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "colored -d #20202050 -g 1.0 -s 30 -t"; cmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "colored -d #202020 -g 1.0 -s 30 -t -a -C"; cmnu.addButton(fx, "selected.setEffect('" + fx + "')")

#--------------------------------------------------------------------------------------------------
# textured effects definitions
fx = "textured -d cyclops/test/checker.jpg"; tmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "textured -d cyclops/test/checker.jpg -g 1.0 -s 50"; tmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "textured -d cyclops/test/graduated.jpg -t -a"; tmnu.addButton(fx, "selected.setEffect('" + fx + "')")

#--------------------------------------------------------------------------------------------------
# bump effects definitions
fx = "bump -d cyclops/test/wall002.jpg -n cyclops/test/wall002DOT3.jpg -g 1.0 -s 20"; bmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "bump -d cyclops/test/wall002.jpg -n cyclops/test/wall002DOT3.jpg"; bmnu.addButton(fx, "selected.setEffect('" + fx + "')")

#--------------------------------------------------------------------------------------------------
# custom shader effect definitions
fx = "./wire -d yellow -g 1.0 -s 10 -t"; xmnu.addButton(fx, "selected.setEffect('" + fx + "')")
fx = "textured -v examples/custom2 -d cyclops/test/checker.jpg -t"; xmnu.addButton(fx, "selected.setEffect('" + fx + "')")

#--------------------------------------------------------------------------------------------------
# multipass effect definitions
fx = "textured -d cyclops/test/checker.jpg | ./wire -d yellow -g 1.0 -s 10 -t -D"; kmnu.addButton(fx, "selected.setEffect('" + fx + "')")

#--------------------------------------------------------------------------------------------------
def onUpdate(frame, t, dt):
	box.pitch(dt / 2)
	box.yaw(dt / 4)

#--------------------------------------------------------------------------------------------------
def onObjectSelected(node, distance):
	global fxmnu
	global selected
	global light
	# If no object has ben selected and the context menu is open, close it.
	if(node == None):
		if(fxmnu.isVisible()): fxmnu.hide()
	else:
		e = getEvent()
		selected = node
		fxmnu.placeOnWand(e)
		fxmnu.show()
		mm.setMainMenu(fxmnu)
		#fxmnu.setLabel(selected.getName() + " effects")

#--------------------------------------------------------------------------------------------------
def onEvent():
	e = getEvent()
	#if(e.getType() != EventType.Update): print(e.getType() )
	if(e.getServiceType() == ServiceType.Pointer or e.getServiceType() == ServiceType.Wand):
		# Button mappings are different when using wand or mouse
		confirmButton = EventFlags.Button3
		if(e.getServiceType() == ServiceType.Wand): confirmButton = EventFlags.Button5
		
		r = getRayFromEvent(e)
		if(r[0]):
			newPos = r[1] + r[2] * lightDistance
			light.setPosition(newPos)
		
		# When the confirm button is pressed:
		if(e.isButtonDown(confirmButton)):
			# If the effect menu is is open, close it
			if(fxmnu.isVisible()): 
				fxmnu.hide()
				mm.setMainMenu(None)
			# ...otherwise, shoot a ray in the scene to perform selection
			else:
				if(r[0]): querySceneRay(r[1], r[2], onObjectSelected)

#--------------------------------------------------------------------------------------------------
setUpdateFunction(onUpdate)
setEventFunction(onEvent)







