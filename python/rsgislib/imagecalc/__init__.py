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
import rsgislib
from rsgislib import imageutils

# define our own classes
class BandDefn(object):
    """
Create a list of these objects to pass to the bandMath function as the 'bands' parameter.
"""
    def __init__(self, bandName=None, fileName=None, bandIndex=None):
        self.bandName = bandName
        self.fileName = fileName
        self.bandIndex = bandIndex


class StatsSummary:
    """ 
This is passed to the imagePixelColumnSummary function 
"""
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
    """ 
Helper function to calculate NDVI 
    
Where:

* image is a string specifying the input image file.
* rBand is an int specifying the red band in the input image (band indexing starts at 1)
* nBand is an int specifying the nir band in the input image (band indexing starts at 1)
* outImage is a string specifying the output image file.
* stats is a boolean specifying whether pyramids and stats should be calculated (Default: True)
* gdalFormat is a string specifing the output image file format (Default: KEA)
    
"""
    expression = '(nir-red)/(nir+red)'
    bandDefns = []
    bandDefns.append(BandDefn('red', image, rBand))
    bandDefns.append(BandDefn('nir', image, nBand))
    bandMath(outImage, expression, gdalFormat, rsgislib.TYPE_32FLOAT, bandDefns)
    if stats:
        imageutils.popImageStats(outImage,False,0.,True)

def calcWBI(image, bBand, nBand, outImage, stats=True, gdalFormat='KEA'):
    """ 
Helper function to calculate WBI 
    
Where:

* image is a string specifying the input image file.
* bBand is an int specifying the blue band in the input image (band indexing starts at 1)
* nBand is an int specifying the nir band in the input image (band indexing starts at 1)
* outImage is a string specifying the output image file.
* stats is a boolean specifying whether pyramids and stats should be calculated (Default: True)
* gdalFormat is a string specifing the output image file format (Default: KEA)
    
"""
    expression = 'blue/nir'
    bandDefns = []
    bandDefns.append(BandDefn('blue', image, bBand))
    bandDefns.append(BandDefn('nir', image, nBand))
    bandMath(outImage, expression, gdalFormat, rsgislib.TYPE_32FLOAT, bandDefns)
    if stats:
        imageutils.popImageStats(outImage,False,0.,True)



def calcDist2ImgVals(inputValsImg, outputDistImg, pxlVals, valsImgBand=1, gdalFormat='KEA', maxDist=None, noDataVal=None, unitGEO=True):
    """ 
A function to calculate the distance to the nearest pixel value with one of the specified values.

Where:

* inputValsImg is a string specifying the input image file.
* outputDistImg is a string specfiying the output image file.
* pxlVals is a number of list of numbers specifying the features to which the distance from should be calculated.
* valsImgBand is an integer specifying the image band of the input image to be used (Default = 1).
* gdalFormat is a string specifying the output image format (Default = KEA)
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
    rsgislib.imageutils.createCopyImage(inputValsImg, outputDistImg, 1, 0.0, gdalFormat, rsgislib.TYPE_32FLOAT)
    distImgDS = gdal.Open(outputDistImg, gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    gdal.ComputeProximity(valsImgBand, distImgBand, proxOptions, callback=gdal.TermProgress)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None
    






def _computeProximityArrArgsFunc(argVals):
    """
This function is used internally within calcDist2Classes for the multiprocessing Pool
"""
    classImgDS = gdal.Open(argVals[0], gdal.GA_ReadOnly)
    classImgBand = classImgDS.GetRasterBand(1)
    imageutils.createCopyImage(argVals[0], argVals[1], 1, argVals[3], argVals[4], rsgislib.TYPE_32FLOAT)
    distImgDS = gdal.Open(argVals[1], gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    gdal.ComputeProximity(classImgBand, distImgBand, argVals[2], callback=gdal.TermProgress)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None



def calcDist2ImgValsTiled(inputValsImg, outputDistImg, pxlVals, valsImgBand=1, maxDist=1000, noDataVal=1000, gdalFormat='KEA', unitGEO=True, tmpDIR='./tmp', tileSize=2000,  nCores=-1):
    """ 
A function to calculate the distance to the nearest pixel value with one of the specified values.

Where:

* inputValsImg is a string specifying the input image file.
* outputDistImg is a string specfiying the output image file.
* pxlVals is a number of list of numbers specifying the features to which the distance from should be calculated.
* valsImgBand is an integer specifying the image band of the input image to be used (Default = 1).
* gdalFormat is a string specifying the output image format (Default = KEA)
* maxDist is a number specifying the maximum distance to be calculated, if None not max value is used (Default = None).
* noDataVal is the no data value in the input image for which distance should not be calculated for (Default = None; None = no specified no data value).
* unitGEO is a boolean specifying the output distance units. True = Geographic units (e.g., metres), False is in Pixels (Default = True).
* tmpDIR is a directory to be used for storing the image tiles and other temporary files - if not directory does not exist it will be created and deleted on completion (Default: ./tmp).
* tileSize is an int specifying in pixels the size of the image tiles used for processing (Default: 2000)
* nCores is the number of processing cores which are available to be used for this processing. If -1 all available cores will be used. (Default: -1)

Example::

    import rsgislib.imagecalc
    cloudsImg = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'
    dist2Clouds = 'LS5TM_20110701_lat52lon421_r24p204_distclouds.kea'
    # Pixel value 1 == Clouds
    # Pixel value 2 == Cloud Shadows
    rsgislib.imagecalc.calcDist2ImgValsTiled(cloudsImg, dist2Clouds, pxlVals=[1,2])

"""
    
    # Check gdal is available
    if not haveGDALPy:
        raise ImportError("The GDAL python bindings are required for "
                          "calcDist2ImgValsTiled function could not be imported")
    import os.path
    import math
    import glob
    import shutil    
    from multiprocessing import Pool
    
    haveListVals = False
    if type(pxlVals) is list:
        haveListVals = True
    
    tmpPresent = True
    if not os.path.exists(tmpDIR):
        print("WARNING: \'" + tmpDIR + "\' directory does not exist so creating it...")
        os.makedirs(tmpDIR)
        tmpPresent = False
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    if nCores <= 0:
        nCores = rsgisUtils.numProcessCores()
    
    uid = rsgisUtils.uidGenerator()
    
    xRes, yRes = rsgisUtils.getImageRes(inputValsImg)    
    if unitGEO:
        xMaxDistPxl = math.ceil(maxDist/xRes)
        yMaxDistPxl = math.ceil(maxDist/yRes)
    else:
        xMaxDistPxl = maxDist
        yMaxDistPxl = maxDist
                
    print("Max Dist Pxls X = {}, Y = {}".format(xMaxDistPxl, yMaxDistPxl))
    
    tileOverlap = xMaxDistPxl
    if yMaxDistPxl > xMaxDistPxl:
        tileOverlap = yMaxDistPxl
    
    imgTilesDIR = os.path.join(tmpDIR, 'ImgTiles_'+uid)
    imgTilesDIRPresent = True
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
        imgTilesDIRPresent = False
        
    imgTileBase = os.path.join(imgTilesDIR, 'ImgTile')
    imageutils.createTiles(inputValsImg, imgTileBase, tileSize, tileSize, tileOverlap, 0, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputValsImg), 'kea')
    imgTileFiles = glob.glob(imgTileBase+'*.kea')
    
    distTilesDIR = os.path.join(tmpDIR, 'DistTiles_'+uid)
    distTilesDIRPresent = True
    if not os.path.exists(distTilesDIR):
        os.makedirs(distTilesDIR)
        distTilesDIRPresent = False    
    
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
    

    distTiles = []
    distTileArgs = []
    for tileFile in imgTileFiles:
        baseTileName = os.path.basename(tileFile)
        distTileFile = os.path.join(distTilesDIR, baseTileName)
        tileArgs = [tileFile, distTileFile, proxOptions, noDataVal, 'KEA']
        distTiles.append(distTileFile)
        distTileArgs.append(tileArgs)
    
    with Pool(nCores) as p:
        p.map(_computeProximityArrArgsFunc, distTileArgs)
            
    # Mosaic Tiles
    imageutils.createImageMosaic(distTiles, outputDistImg, 0, 0, 1, 1, gdalFormat, rsgislib.TYPE_32FLOAT)
    imageutils.popImageStats(outputDistImg, usenodataval=True, nodataval=0, calcpyramids=True)
    
    for imgFile in distTiles:
        rsgisUtils.deleteFileWithBasename(imgFile)
    
    if not imgTilesDIRPresent:
        shutil.rmtree(imgTilesDIR, ignore_errors=True)
    else:
        for tileFile in imgTileFiles:
            rsgisUtils.deleteFileWithBasename(tileFile)
    
    if not distTilesDIRPresent:
        shutil.rmtree(distTilesDIR, ignore_errors=True)
    
    if not tmpPresent:
        shutil.rmtree(tmpDIR, ignore_errors=True)

