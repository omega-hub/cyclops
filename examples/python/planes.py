# basic omegalib script: just display a textured spinning cube.
from omega import *
from euclid import *
from cyclops import *

n = 20

planes = []

for i in range(0, n):
	for j in range(0, n):
		x = float(i) / n * 2 - 1
		y = float(j) / n * 2 - 1
		s = 1.0 / n
		plane = PlaneShape.create(s, s)
		plane.setPosition(Vector3(x, y + 2, -3))
		plane.setEffect('colored -d #ff60ff -g 1.0 -s 10 -C')
		planes.append(plane)

light = Light.create()
light.setColor(Color('white'))
light.setAmbient(Color("#3030a0"))
light.setEnabled(True)
light.setLightType(LightType.Directional)
light.setLightDirection(Vector3(0 -0.5, -0.5))
		
# Spin the planes
def onUpdate(frame, t, dt):
	for p in planes:
		k = (p.getPosition().x + p.getPosition().y) / 32
		p.pitch(dt + k)
		p.yaw(dt / 3 + k)
		
setUpdateFunction(onUpdate)