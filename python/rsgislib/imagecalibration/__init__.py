"""
The image calibration module contains functions for calibrating optical data from DN to radience and top of atmosphere reflectance and, using coefficients from 6S, surface reflectance.

For obtaining the correct parameters for each function it is recomented that rather than running directly they are called through the Atmospheric and Radiometric Correction of Satellite Imagery (ARCSI) software. 

More details on ARCSI are avaialble from http://rsgislib.org/arcsi

"""

# import the C++ extension into this level
from ._imagecalibration import *


def performDOSCalc(inputFile, outputFile, gdalFormat='KEA', nonNegative=True, noDataVal=0, darkObjReflVal=0, darkObjPercentile=0.01, copyBandNames=True, calcStatsPyd=True):
    """
A command to perform a dark object subtraction (DOS) on an input image.

* inputFile - input image to which the DOS method is to be applied. Typically, this image with be in top of atmosphere reflectance (TOA)
* outputFile - the output image file
* gdalFormat - the output image file format (default = KEA)
* nonNegative - is a boolean specifying where negative output pixel values will be accepted (Dafualt is True; i.e., no negative values)
* noDataVal - is the no data value within the input image file. 
* darkObjReflVal - is an offset which is applied to all pixel values to make a minimum reflectance value (Default = 0)
* darkObjPercentile - is the percentile of the input image used to define the dark object threshold, range is 0 - 1 (Default is 0.01; i.e., 1%).
* copyBandNames - is a boolean specifying that the band names of the input image should be copied to the output image file (Default: True)
* calcStatsPyd - is a boolean specifying that the image stats and pyramids should be calculated on the output image (Default: True)

Example::

import rsgislib.imagecalibration
rsgislib.imagecalibration.performDOSCalc("LS5TM_20110701_lat52lon421_r24p204_rad_toa.kea", 'LS5TM_20110701_lat52lon421_r24p204_rad_toa_dos.kea")

"""
    import rsgislib
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import collections
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    outDataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputFile)
    
    percentList = rsgislib.imagecalc.bandPercentile(inputFile, darkObjPercentile, noDataVal)
        
    offsetsList = list()
    OffVal = collections.namedtuple('DOSOffset', ['offset'])
    for val in percentList:
        offsetsList.append(OffVal(offset=val))
    
    applySubtractSingleOffsets(inputFile, outputFile, gdalFormat, outDataType, nonNegative, True, noDataVal, darkObjReflVal, offsetsList)
    
    if copyBandNames:
        bandNames = rsgislib.imageutils.getBandNames(inputFile)
        rsgislib.imageutils.setBandNames(outputFile, bandNames)
    
    if calcStatsPyd:
        rsgislib.imageutils.popImageStats(outputFile, usenodataval=True, nodataval=noDataVal, calcpyramids=True)

 