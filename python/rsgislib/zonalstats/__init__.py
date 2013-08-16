"""
Contains zonal stats functionality for rsgislib
"""

# import the C++ extension into this level
from ._zonalstats import *

METHOD_POLYCONTAINSPIXEL = 0           # Polygon completely contains pixel
METHOD_POLYCONTAINSPIXELCENTER = 1     # Pixel center is within the polygon
METHOD_POLYOVERLAPSPIXEL = 2           # Polygon overlaps the pixel
METHOD_POLYOVERLAPSORCONTAINSPIXEL = 3 # Polygon overlaps or contains the pixel
METHOD_PIXELCONTAINSPOLY = 4           # Pixel contains the polygon
METHOD_PIXELCONTAINSPOLYCENTER = 5     # Polygon center is within pixel
METHOD_ADAPTIVE = 6                    # The method is chosen based on relative areas of pixel and polygon.
METHOD_ENVELOPE = 7                    # All pixels in polygon envelope chosen
METHOD_PIXELAREAINPOLY = 8             # Percent of pixel area that is within the polygon
METHOD_POLYAREAINPIXEL = 9             # Percent of polygon area that is within pixel

class ZonalAttributes:
    """ This is passed to the pixelStats2SHP function """
    def __init__(self, minThreshold, maxThreshold , calcCount=False, calcMin=False, calcMax=False, calcMean=False, calcStdDev=False, calcMode=False, calcSum=False):
        self.minThreshold = minThreshold
        self.maxThreshold = maxThreshold
        self.calcCount = calcCount
        self.calcMin = calcMin
        self.calcMax = calcMax
        self.calcMean = calcMean
        self.calcStdDev = calcStdDev 
        self.calcMode = calcMode
        self.calcSum = calcSum
