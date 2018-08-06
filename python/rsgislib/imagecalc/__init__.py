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
import numpy

haveRIOS = True
try:
    from rios import applier
    from rios import cuiprogress
except ImportError as riosErr:
    haveRIOS = False

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



class ImageBandRescale(object):
    """
Data structure for rescaling information for rescaleImgPxlVals function.
* band - specified image band (band numbering starts at 1).
* inMin - the input image band minimum value for rescaling. 
* inMax - the input image band maximum value for rescaling.
* inNoData - no data value for the input image band.
* outMin - the output image band minimum value for rescaling. 
* outMax - the output image band maximum value for rescaling.
* outNoData - no data value for the output image band.
"""
    def __init__(self, band=0, inMin=0.0, inMax=0.0, inNoData=0, outMin=0.0, outMax=0.0, outNoData=0.0):
        """
        * band - specified image band (band numbering starts at 1).
        * inMin - the input image band minimum value for rescaling. 
        * inMax - the input image band maximum value for rescaling.
        * inNoData - no data value for the input image band.
        * outMin - the output image band minimum value for rescaling. 
        * outMax - the output image band maximum value for rescaling.
        * outNoData - no data value for the output image band.
        """
        self.band = band
        self.inMin = inMin
        self.inMax = inMax
        self.inNoData = inNoData
        self.outMin = outMin
        self.outMax = outMax
        self.outNoData = outNoData
        
    def __str__(self):
        strVal = 'Band ' + str(self.band) + " ["+str(self.inMin)+", "+str(self.inMax)+", "+str(self.inNoData)+"] ["+str(self.outMin)+", "+str(self.outMax)+", "+str(self.outNoData)+"]"
        return strVal
        
    def __repr__(self):
        strVal = 'Band ' + str(self.band) + " ["+str(self.inMin)+", "+str(self.inMax)+", "+str(self.inNoData)+"] ["+str(self.outMin)+", "+str(self.outMax)+", "+str(self.outNoData)+"]"
        return strVal


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
    import rsgislib.imageutils
    classImgDS = gdal.Open(argVals[0], gdal.GA_ReadOnly)
    classImgBand = classImgDS.GetRasterBand(1)
    rsgislib.imageutils.createCopyImage(argVals[0], argVals[1], 1, argVals[3], argVals[4], rsgislib.TYPE_32FLOAT)
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
    import rsgislib.imageutils
    
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
    rsgislib.imageutils.createTiles(inputValsImg, imgTileBase, tileSize, tileSize, tileOverlap, 0, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputValsImg), 'kea')
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
    rsgislib.imageutils.createImageMosaic(distTiles, outputDistImg, 0, 0, 1, 1, gdalFormat, rsgislib.TYPE_32FLOAT)
    rsgislib.imageutils.popImageStats(outputDistImg, usenodataval=True, nodataval=0, calcpyramids=True)
    
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


def countPxlsOfVal(inputImg, vals=[0]):
    """
Function which counts the number of pixels of a set of values returning a list in the same order as the list of values provided.

Where:

* inputImg - the input image
* vals - is a list of pixel values to be counted

"""
    if len(vals) == 0:
        raise Exception('At least 1 value should be provided within the vals input varable.')
    numVals = len(vals)
    outVals = numpy.zeros(numVals,dtype=numpy.int64)
    
    from rios.imagereader import ImageReader

    reader = ImageReader(inputImg) 
    for (info, block) in reader:     
        for idx in range(numVals):
            outVals[idx] = outVals[idx] + (block == vals[idx]).sum()

    return outVals


def getPCAEigenVector(inputImg, pxlNSample, noData=None, outMatrixFile=None):
    """
A function which takes a sample from an input image and uses it to 
generate eigenvector for a PCA. Note. this can be used as input to rsgislib.imagecalc.pca

Where::

* inputImg - the image from which the random sample will be taken.
* pxlNSample - the sample to be taken (e.g., a value of 100 will sample every 100th pixel)
* noData - provide a no data value which is to be ignored during processing. If None then ignored (Default: None)
* outMatrixFile - path and name for the output rsgislib matrix file. If None file is not created (Default: None)

returns::

* array with the eigenvector
* array with the ratio of the explained variance

""" 
    from sklearn.decomposition import PCA
    import rsgislib.imageutils
    
    # Read input data from image file.
    X = rsgislib.imageutils.extractImgPxlSample(inputImg, pxlNSample, noData)
    
    print(str(X.shape[0]) + ' values were extracted from the input image.')
    
    pca = PCA()
    pca.fit(X)
    
    if outMatrixFile is not None:
        f = open(outMatrixFile, 'w')
        f.write('m='+str(pca.components_.shape[0])+'\n')
        f.write('n='+str(pca.components_.shape[1])+'\n')
        first = True
        for val in pca.components_.flatten():
            if first:
                f.write(str(val))
                first = False
            else:
                f.write(','+str(val))
        f.write('\n\n')
        f.flush()
        f.close()
    
    pcaComp = 1
    print("Prop. of variance explained:")
    for val in pca.explained_variance_ratio_:
        print('\t PCA Component ' + str(pcaComp) + ' = ' + str(round(val, 4)))
        pcaComp = pcaComp + 1

    return pca.components_, pca.explained_variance_ratio_


def performImagePCA(inputImg, outputImg, eigenVecFile, nComponents=None, pxlNSample=100, gdalformat='KEA', datatype=rsgislib.TYPE_32UINT, noData=None, calcStats=True):
    """
A function which performs a PCA on the input image.

Where::

* inputImg - the image from which the random sample will be taken.
* outputImg - the output image transformed using the calculated PCA
* eigenVecFile - path and name for the output rsgislib matrix file containing the eigenvector for the PCA.
* nComponents - the number of PCA compoents outputted. A value of None is all components (Default: None)
* pxlNSample - the sample to be taken (e.g., a value of 100 will sample every 100th pixel) (Default: 100)
* gdalformat - the output gdal supported file format (Default KEA)
* datatype - the output data type of the input image (Default: rsgislib.TYPE_32UINT)
* noData - provide a no data value which is to be ignored during processing. If None then ignored (Default: None)
* calcStats - Boolean specifying whether pyramids and statistics should be calculated for the output image. (Default: True)

"""
    import rsgislib.imageutils
    eigenVec, varExplain = rsgislib.imagecalc.getPCAEigenVector(inputImg, pxlNSample, noData, eigenVecFile)
    outNComp = varExplain.shape[0]
    if nComponents is not None:
        if nComponents > varExplain.shape[0]:
            raise Exception("You cannot output more components than the number of input image bands.")
        outNComp = nComponents
    
    rsgislib.imagecalc.pca(inputImg, eigenVecFile, outputImg, outNComp, gdalformat, datatype)
    if calcStats:
        usenodataval=False
        nodataval=0
        if noData is not None:
            usenodataval=True
            nodataval=noData
        rsgislib.imageutils.popImageStats(outputImg, usenodataval, nodataval, True)


def rescaleImgPxlVals(inputImg, outputImg, gdalformat, datatype, bandRescale, trim2Limits=True):
    """
Function which rescales an input image base on a list of rescaling parameters.

Where:

* inputImg - the input image
* outputImg - the output image file name and path (will be same dimensions as the input)
* gdalformat - the GDAL image file format of the output image file.
* bandRescale - list of ImageBandRescale objects
* trim2Limits - whether to trim the output to the output min/max values.

"""
    bandRescaleDict = dict()
    for rescaleObj in bandRescale:
        bandRescaleDict[rescaleObj.band-1] = rescaleObj
    
    numpyDT = numpy.float32
    if datatype == rsgislib.TYPE_8INT:
        numpyDT = numpy.int8
    elif datatype == rsgislib.TYPE_16INT:
        numpyDT = numpy.int16
    elif datatype == rsgislib.TYPE_32INT:
        numpyDT = numpy.int32
    elif datatype == rsgislib.TYPE_64INT:
        numpyDT = numpy.int64
    elif datatype == rsgislib.TYPE_8UINT:
        numpyDT = numpy.uint8
    elif datatype == rsgislib.TYPE_16UINT:
        numpyDT = numpy.uint16
    elif datatype == rsgislib.TYPE_32UINT:
        numpyDT = numpy.uint32
    elif datatype == rsgislib.TYPE_64UINT:
        numpyDT = numpy.uint64
    elif datatype == rsgislib.TYPE_32FLOAT:
        numpyDT = numpy.float32
    elif datatype == rsgislib.TYPE_64FLOAT:
        numpyDT = numpy.float64
    else:
        raise Exception('Datatype was not recognised.')
    
    infiles = applier.FilenameAssociations()
    infiles.image = inputImg
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    otherargs.rescaleDict = bandRescaleDict
    otherargs.trim = trim2Limits
    otherargs.numpyDT = numpyDT
    aControls = applier.ApplierControls()
    aControls.progress = cuiprogress.CUIProgressBar()
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    
    def _applyRescale(info, inputs, outputs, otherargs):
        """
        This is an internal rios function 
        """
        outputs.outimage = numpy.zeros_like(inputs.image, dtype=numpyDT)
        for idx in range(inputs.image.shape[0]):
            outputs.outimage[idx] = numpy.where(inputs.image[idx] == otherargs.rescaleDict[idx].inNoData, otherargs.rescaleDict[idx].outNoData, (((inputs.image[idx]-otherargs.rescaleDict[idx].inMin)/(inputs.image[idx]-otherargs.rescaleDict[idx].inMax - inputs.image[idx]-otherargs.rescaleDict[idx].inMin)) * (inputs.image[idx]-otherargs.rescaleDict[idx].outMax - inputs.image[idx]-otherargs.rescaleDict[idx].outMin)) + inputs.image[idx]-otherargs.rescaleDict[idx].outMin)
            if otherargs.trim:
                outputs.outimage[idx] = numpy.where((outputs.outimage[idx] != otherargs.rescaleDict[idx].outNoData) & (outputs.outimage[idx]<otherargs.rescaleDict[idx].outMin), otherargs.rescaleDict[idx].outMin, outputs.outimage[idx])
                outputs.outimage[idx] = numpy.where((outputs.outimage[idx] != otherargs.rescaleDict[idx].outNoData) & (outputs.outimage[idx]>otherargs.rescaleDict[idx].outMax), otherargs.rescaleDict[idx].outMax, outputs.outimage[idx])

    applier.apply(_applyRescale, infiles, outfiles, otherargs, controls=aControls)


