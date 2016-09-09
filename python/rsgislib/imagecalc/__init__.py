"""
The imagecalc module contains functions for performing a number of calculating on images.
"""

# import the C++ extension into this level
from ._imagecalc import *

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError:
    haveGDALPy = False
 
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



def calcDist2ImgVals(inputValsImg, outputDistImg, pxlVals, valsImgBand=1, outImgFormat='KEA', maxDist=None, noDataVal=None, unitGEO=True):
    """ 
    A function to calculate the distance to the nearest pixel value with one of the specified values.

    Where:

    * inputValsImg is a string specifying the input image file.
    * outputDistImg is a string specfiying the output image file.
    * pxlVals is a number of list of numbers specifying the features to which the distance from should be calculated.
    * valsImgBand is an integer specifying the image band of the input image to be used (Default = 1).
    * outImgFormat is a string specifying the output image format (Default = KEA)
    * maxDist is a number specifying the maximum distance to be calculated, if None not max value is used (Default = None).
    * noDataVal is the no data value in the input image for which distance should not be calculated for (Default = None; None = no specified no data value).
    * unitGEO is a boolean specifying the output distance units. True = Geographic units (e.g., metres), False is in Pixels (Default = True).
    
    Example::
    
    import rsgislib.imagecalc
    cloudsImg = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'
    dist2Clouds = 'LS5TM_20110701_lat52lon421_r24p204_distclouds.kea'
    # Pixel value 1 == Clouds
    # Pixel value 2 == Cloud Shadows
    rsgislib.imagecalc.calcDist2ImgVals(cloudsImg, dist2Clouds, pxlVals=[1,2])
    
    """
   
    # Check gdal is available
    if not haveGDALPy:
        raise ImportError("The GDAL python bindings are required for "
                          "calcDist2ImgVals function could not be imported")
    
    import rsgislib.imageutils
    
    haveListVals = False
    if type(pxlVals) is list:
        haveListVals = True
    
    proxOptions = []
    
    if maxDist is not None:
        proxOptions.append('MAXDIST='+str(maxDist))
    if noDataVal is not None:
        proxOptions.append('NODATA='+str(noDataVal))
    if unitGEO:
        proxOptions.append('DISTUNITS=GEO')
    else:
        proxOptions.append('DISTUNITS=PIXEL')
    
    if haveListVals:
        strVals = ''
        first = True
        for val in pxlVals:
            if first:
                strVals = str(val)
                first = False
            else:
                strVals = strVals + "," + str(val)
        proxOptions.append('VALUES='+strVals)    
    else:
        proxOptions.append('VALUES='+str(pxlVals))
        
    valsImgDS = gdal.Open(inputValsImg, gdal.GA_ReadOnly)
    valsImgBand = valsImgDS.GetRasterBand(valsImgBand)
    rsgislib.imageutils.createCopyImage(inputValsImg, outputDistImg, 1, 0.0, outImgFormat, rsgislib.TYPE_32FLOAT)
    distImgDS = gdal.Open(outputDistImg, gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    gdal.ComputeProximity(valsImgBand, distImgBand, proxOptions, callback=gdal.TermProgress)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None
    
    
    
    
    
