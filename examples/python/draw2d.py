from math import *
from euclid import *
from omega import *
#from cyclops import *
#from omegaToolkit import *

white = Color("white")
black = Color("black")

y = 0

def onDraw(displaySize, tileSize, camera, painter):
    global white
    global black
    global y

    # Get the default font
    font = painter.getDefaultFont()

    # Set some text and compute its width and height in pixels (given the font we cant to use)
    text = "Hello World From Python!"
    textSize = font.computeSize(text)

    # Draw a white box and put the text inside it
    painter.drawRect(Vector2(10, 10 + y), textSize + Vector2(10, 10), white)
    painter.drawText(text, font, Vector2(15, 15 + y), TextAlign.HALeft | TextAlign.VATop, black)


def onUpdate(frame, t, dt):
    global y
    y = abs(sin(radians(t) * 100)) * 100

setUpdateFunction(onUpdate)
setDrawFunction(onDraw)

