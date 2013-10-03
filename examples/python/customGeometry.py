# Create a custom quad using a ModelGeometry object.
from math import *
from cyclops import *

geom = ModelGeometry.create('quad')
v1 = geom.addVertex(Vector3(0, 0, 0))
geom.addColor(Color('red'))
v2 = geom.addVertex(Vector3(0, -1, 0))
geom.addColor(Color('green'))
v3 = geom.addVertex(Vector3(1, 0, 0))
geom.addColor(Color('blue'))
v4 = geom.addVertex(Vector3(1, -1, 0))
geom.addColor(Color('yellow'))
geom.addPrimitive(PrimitiveType.TriangleStrip, 0, 4)

getSceneManager().addModel(geom)

quad = StaticObject.create('quad')
quad.setPosition(Vector3(0, 2, -3))

# Apply an emissive textured effect (no lighting)
quad.getMaterial().setProgram('colored byvertex-emissive')
quad.getMaterial().setTransparent(True)
quad.getMaterial().setAlpha(0.5)

# add a sphere, just to show transparency
s = SphereShape.create(0.6, 3)
s.setPosition(0, 2, -4)

# cycle vertex colors
def onUpdate(frame, time, dt):
	c = geom.getColor(v1)
	c.red = sin(time) * 0.5 + 0.5
	geom.setColor(v1, c)
	
	c = geom.getColor(v2)
	c.green = sin(time + 1) * 0.5 + 0.5
	geom.setColor(v2, c)
	
	c = geom.getColor(v3)
	c.blue = sin(time + 2) * 0.5 + 0.5
	geom.setColor(v3, c)
	
setUpdateFunction(onUpdate)
