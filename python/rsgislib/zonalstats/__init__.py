"""
Contains zonal stats functionality for rsgislib
"""

# import the C++ extension into this level
from ._zonalstats import *


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
