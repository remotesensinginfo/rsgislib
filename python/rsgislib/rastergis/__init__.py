"""
Contains segmentation functionality for rsgislib
"""

# import the C++ extension into this level
from ._rastergis import *

class BandAttStats:
    """ This is passed to the populateRATWithStats function """
    def __init__(self, band, threshold=0.0, countField=None, minField=None, maxField=None, sumField=None, medianField=None, stdDevField=None, meanField=None):
        self.band = band
        self.threshold = threshold
        self.minField = minField
        self.maxField = maxField
        self.sumField = sumField
        self.meanField = meanField
        self.stdDevField = stdDevField
        self.medianField = medianField
        self.countField = countField

class BandAttPercentiles:
    """ This is passed to the populateRATWithPercentiles function """
    def __init__(self, band, percentile, fieldName):
        self.band = band
        self.percentile = percentile
        self.fieldName = fieldName

class ShapeIndex:
    """ This is passed to the calcShapeIndices function """
    def __init__(self, colName, idx, colIdx=0):
        self.colName = colName
        self.colIdx = colIdx
        self.idx = idx