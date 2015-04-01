"""
The imagecalc module contains functions for performing a number of calculating on images.
"""

# import the C++ extension into this level
from ._imagecalc import *

# define our own classes
class BandDefn(object):
    """
    Create a list of these objects to pass to the bandMath function
    as the 'bands' parameter.
    """
    def __init__(self, bandName=None, fileName=None, bandIndex=None):
        self.bandName = bandName
        self.fileName = fileName
        self.bandIndex = bandIndex


class StatsSummary:
    """ This is passed to the imagePixelColumnSummary function """
    def __init__(self, min=0.0, max=0.0, sum=0.0, median=0.0, stdDev=0.0, mean=0.0,
                 calcMin=False, calcMax=False, calcSum=False, calcMean=False, calcStdDev=False, calcMedian=False):
        self.min = min
        self.max = max
        self.sum = sum
        self.mean = mean
        self.stdDev = stdDev
        self.median = median
        self.calcMin = calcMin
        self.calcMax = calcMax
        self.calcSum = calcSum
        self.calcMean = calcMean
        self.calcStdDev = calcStdDev
        self.calcMedian = calcMedian


def calcNDVI(image, rBand, nBand, outImage, stats=True, gdalFormat='KEA'):
    """ Helper function to calculate NDVI 
    
Where:

* image is a string specifying the input image file.
* rBand is an int specifying the red band in the input image (band indexing starts at 1)
* nBand is an int specifying the nir band in the input image (band indexing starts at 1)
* outImage is a string specifying the output image file.
* stats is a boolean specifying whether pyramids and stats should be calculated (Default: True)
* gdalFormat is a string specifing the output image file format (Default: KEA)
    
"""
    from rsgislib import imageutils
    import rsgislib
    expression = '(nir-red)/(nir+red)'
    bandDefns = []
    bandDefns.append(BandDefn('red', image, rBand))
    bandDefns.append(BandDefn('nir', image, nBand))
    bandMath(outImage, expression, gdalFormat, rsgislib.TYPE_32FLOAT, bandDefns)
    if stats:
        imageutils.popImageStats(outImage,False,0.,True)

def calcWBI(image, bBand, nBand, outImage, stats=True, gdalFormat='KEA'):
    """ Helper function to calculate WBI 
    
Where:

* image is a string specifying the input image file.
* bBand is an int specifying the blue band in the input image (band indexing starts at 1)
* nBand is an int specifying the nir band in the input image (band indexing starts at 1)
* outImage is a string specifying the output image file.
* stats is a boolean specifying whether pyramids and stats should be calculated (Default: True)
* gdalFormat is a string specifing the output image file format (Default: KEA)
    
"""
    from rsgislib import imageutils
    import rsgislib
    expression = 'blue/nir'
    bandDefns = []
    bandDefns.append(BandDefn('blue', image, bBand))
    bandDefns.append(BandDefn('nir', image, nBand))
    bandMath(outImage, expression, gdalFormat, rsgislib.TYPE_32FLOAT, bandDefns)
    if stats:
        imageutils.popImageStats(outImage,False,0.,True)


