from math import *
from euclid import *
from omega import *
from cyclops import *

bboxes = []

root = SceneNode.create('bboxRoot')

# create a bounding box actor that will draw an axis-aligned bounding box 
# around a target scene node
class BBox(Actor):
    box = None
    target = None
    def __init__(self, target):
        bboxes.append(self)
        super(BBox, self).__init__("BBox")
        self.box = BoxShape.create(1, 1, 1)
        # Customize the look of the bounding box here.
        self.box.setEffect('colored -e #ff8080 -w')
        self.target = target
        root.addChild(self.box)
        self.setUpdateEnabled(True)
		
    def dispose(self):
        bboxes.remove(self)
        root.removeChild(self.box)
	
    def onUpdate(self, frame, time, dt):
        self.box.setPosition(self.target.getBoundCenter())
        mn = self.target.getBoundMinimum()
        mx = self.target.getBoundMaximum()
        self.box.setScale(mx - mn)

