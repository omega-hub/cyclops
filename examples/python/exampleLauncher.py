# startup script for orun
# NOTE: identifiers that start with `_` are considered system-level: they will not be deleted
# during application switches or when calling OmegaViewer.reset(). They will also be accessible from all
# scripts.
from omegaViewer import *
from omegaToolkit import *
from euclid import *

def _onLauncherStart():
	mm = MenuManager.createAndInitialize()
	sysmnu = mm.getMainMenu().addSubMenu("Applications")
	
	# default icon size
	ics = Vector2(64, 64)
	
	# billboard app
	mi = sysmnu.addButton("Billboard", "_startApp('./billboard.py')")
	i = loadImage("app.png")
	mi.setImage(i)
	mi.getButton().getImage().setSize(ics)

	# multilight app
	mi = sysmnu.addButton("Multilight", "_startApp('./multilight.py')")
	i = loadImage("app.png")
	mi.setImage(i)
	mi.getButton().getImage().setSize(ics)

	# embedded shader app
	mi = sysmnu.addButton("Embedded Shader", "_startApp('./embeddedShader.py')")
	i = loadImage("app.png")
	mi.setImage(i)
	mi.getButton().getImage().setSize(ics)
	
	# fxdemo app
	mi = sysmnu.addButton("Effects", "_startApp('./fxdemo.py')")
	i = loadImage("app.png")
	mi.setImage(i)
	mi.getButton().getImage().setSize(ics)
	
	# loadModel app
	mi = sysmnu.addButton("Model Loader", "_startApp('./loadModel.py')")
	i = loadImage("app.png")
	mi.setImage(i)
	mi.getButton().getImage().setSize(ics)
	
	# endurance app
	mi = sysmnu.addButton("ENDURANCE", "_startApp('OMEGA_APP_ROOT/endurance/endurancePoints.py')")
	i = loadImage("app.png")
	mi.setImage(i)
	mi.getButton().getImage().setSize(ics)
	
def _startApp(appScript):
	queueCommand(':r! ' + appScript)
	
v = getViewer()
v.setAppStartCommand(v.getAppStartCommand() + "; from euclid import *; _onLauncherStart()")

_onLauncherStart()
	