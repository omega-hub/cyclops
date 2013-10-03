# startup script for orun
# NOTE: identifiers that start with `_` are considered system-level: they will not be deleted
# during application switches or when calling OmegaViewer.reset(). They will also be accessible from all
# scripts.
from omegaViewer import *
from omegaToolkit import *
from euclid import *

mm = MenuManager.createAndInitialize()
sysmnu = mm.getMainMenu().addSubMenu("Color Chooser")
	
btn1 = sysmnu.addButton("Blue", "colorWidget.setStyleValue('fill', '#0000ff')")
btn1.getButton().setCheckable(True)
btn1.getButton().setRadio(True)

btn2 = sysmnu.addButton("Green", "colorWidget.setStyleValue('fill', '#00ff00')")
btn2.getButton().setCheckable(True)
btn2.getButton().setRadio(True)

btn2 = sysmnu.addButton("Red", "colorWidget.setStyleValue('fill', '#ff0000')")
btn2.getButton().setCheckable(True)
btn2.getButton().setRadio(True)

coloritem = sysmnu.addContainer()
global colorWidget
colorWidget = coloritem.getContainer()
colorWidget.setAutosize(False)
colorWidget.setSize(Vector2(100, 100))
