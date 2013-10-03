# basic omegalib script: just display a textured spinning cube.
from omega import *
from cyclops import *
from math import *

planes = []

logo = loadImage('cyclops/test/omega-transparent.png')

n = logo.getWidth() / 10

logo.beginPixelAccess()
for i in range(0, n):
	for j in range(0, n):
		x = float(i) / n * 2 - 1
		y = float(j) / n * 2 - 1
		s = 1.0 / n
		plane = PlaneShape.create(s, s)
		plane.setPosition(Vector3(x, y + 2, -3))
		
		r = float(logo.getPixelR(i * 10, j * 10)) / 255
		g = float(logo.getPixelG(i * 10, j * 10)) / 255
		b = float(logo.getPixelB(i * 10, j * 10)) / 255
		a = float(logo.getPixelA(i * 10, j * 10)) / 255
		
		plane.getMaterial().setProgram('colored')
		plane.getMaterial().setColor(Color(0,0,0,1), Color(r,g,b,a))
		planes.append(plane)
logo.endPixelAccess()

# Spin the planes
def onUpdate(frame, t, dt):
	for p in planes:
		k = (p.getPosition().x + p.getPosition().y) / 32
		p.pitch(dt + k)
		p.yaw(dt / 3 + k)
		
setUpdateFunction(onUpdate)