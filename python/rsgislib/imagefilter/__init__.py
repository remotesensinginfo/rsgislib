"""
Contains image filtering functionality for rsgislib
"""

# import the C++ extension into this level
from ._imagefilter import *
import os.path

class FilterParameters:
    """ Object, specifying the type of filter and filter parameters """
    def __init__(self, filterType, fileEnding, size = 3, option = None, nLooks = None, stddev = None, stddevX = None, stddevY = None, angle = None):
        self.filterType = filterType
        self.fileEnding = fileEnding
        self.size = size
        self.option = option
        self.nLooks = nLooks
        self.stddev = stddev
        self.stddevX = stddevX
        self.stddevY = stddevY
        self.angle = angle

def applyMedianFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a median filter to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* filterSize - int specfiying the size of the image filter (must be an odd number, i.e., 3, 5, 7, etc).
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::
    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    clumpsFile = 'jers1palsar_stack_median3.kea'
    imagefilter.applyMedianFilter(inputImage, clumpsFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Median', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
    

