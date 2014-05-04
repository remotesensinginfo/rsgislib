"""
The Raster GIS module contains functions for attributing and manipulating raster attribute tables.
"""

# import the C++ extension into this level
from ._rastergis import *

class BandAttStats:
    """ This is passed to the populateRATWithStats function """
    def __init__(self, band, minField=None, maxField=None, sumField=None, stdDevField=None, meanField=None):
        self.band = band
        self.minField = minField
        self.maxField = maxField
        self.sumField = sumField
        self.meanField = meanField
        self.stdDevField = stdDevField

class BandAttPercentiles:
    """ This is passed to the populateRATWithPercentiles function """
    def __init__(self, percentile, fieldName):
        self.percentile = percentile
        self.fieldName = fieldName

class ShapeIndex:
    """ This is passed to the calcShapeIndices function """
    def __init__(self, colName, idx, colIdx=0):
        self.colName = colName
        self.colIdx = colIdx
        self.idx = idx
