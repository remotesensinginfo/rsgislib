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
    outImgFile = 'jers1palsar_stack_median3.kea'
    imagefilter.applyMedianFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Median', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)

def applyMeanFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a mean filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_mean3.kea'
    imagefilter.applyMeanFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Mean', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)

def applyMinFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a min filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_min3.kea'
    imagefilter.applyMinFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Min', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applyMaxFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a max filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_max3.kea'
    imagefilter.applyMaxFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Max', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applyModeFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a mode filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_mode3.kea'
    imagefilter.applyModeFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Mode', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
    
def applyStdDevFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a std dev filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_stdev3.kea'
    imagefilter.applyStdDevFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'StdDev', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)    



def applyRangeFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a range filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_range3.kea'
    imagefilter.applyRangeFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Range', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)       


def applyMeanDiffFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a mean difference filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_meandiff3.kea'
    imagefilter.applyMeanDiffFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'MeanDiff', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applyMeanDiffAbsFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a mean absolute difference filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_meandiffabs3.kea'
    imagefilter.applyMeanDiffAbsFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'MeanDiffAbs', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)




def applyTotalDiffFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a total (i.e., sum) difference filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_totaldiff3.kea'
    imagefilter.applyTotalDiffFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'TotalDiff', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applyTotalDiffAbsFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a total absolute difference filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_totaldiffabs3.kea'
    imagefilter.applyTotalDiffAbsFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'TotalDiffAbs', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)




def applyCoeffOfVarFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a coefficient of variance filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_cofvar3.kea'
    imagefilter.applyCoeffOfVarFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'CoeffOfVar', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applyTotalFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a total filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_total3.kea'
    imagefilter.applyTotalFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Total', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)

def applyNormVarFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a normalised variance filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_NormVar3.kea'
    imagefilter.applyNormVarFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'NormVar', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applyNormVarSqrtFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a normalised variance square root filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_NormVarSqrt3.kea'
    imagefilter.applyNormVarSqrtFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'NormVarSqrt', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)




def applyNormVarLnFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a normalised variance natural log filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_NormVarLn3.kea'
    imagefilter.applyNormVarLnFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'NormVarLn', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)



def applyTextureVarFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a texture variance filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_NormVarLn3.kea'
    imagefilter.applyTextureVarFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'TextureVar', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)



def applyKuwaharaFilter(inputimage, outputImage, filterSize, gdalformat, datatype):
    """ Apply a kuwahara filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_kuwa3.kea'
    imagefilter.applyKuwaharaFilter(inputImage, outImgFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Kuwahara', fileEnding = '', size=filterSize) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


def applySobelFilter(inputimage, outputImage, gdalformat, datatype):
    """ Apply a sobel filter to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_sobel.kea'
    imagefilter.applySobelFilter(inputImage, outImgFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Sobel', fileEnding = '', option = 'xy') )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)    
    
def applySobelXFilter(inputimage, outputImage, gdalformat, datatype):
    """ Apply a sobel filter in X axis to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_sobelx.kea'
    imagefilter.applySobelXFilter(inputImage, outImgFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Sobel', fileEnding = '', option = 'x') )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)

def applySobelYFilter(inputimage, outputImage, gdalformat, datatype):
    """ Apply a sobel filter in Y axis to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_sobely.kea'
    imagefilter.applySobelYFilter(inputImage, outImgFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Sobel', fileEnding = '', option = 'y') )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
    
def applyPrewittFilter(inputimage, outputImage, gdalformat, datatype):
    """ Apply a Prewitt filter to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_prewitt.kea'
    imagefilter.applyPrewittFilter(inputImage, outImgFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'xy') )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
  
  
   
def applyPrewittXFilter(inputimage, outputImage, gdalformat, datatype):
    """ Apply a Prewitt filter in X axis to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_prewitt.kea'
    imagefilter.applyPrewittXFilter(inputImage, outImgFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'x') )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)



    
def applyPrewittYFilter(inputimage, outputImage, gdalformat, datatype):
    """ Apply a Prewitt filter in Y axis to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_prewitt.kea'
    imagefilter.applyPrewittYFilter(inputImage, outImgFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'y') )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)    



    




def applyGaussianSmoothFilter(inputimage, outputImage, filterSize, stddevX, stddevY, filterAngle, gdalformat, datatype):
    """ Apply a Gaussian smoothing filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_gausmooth.kea'
    imagefilter.applyGaussianSmoothFilter(inputImage, outImgFile, 3, 1.0, 1.0. 0.0, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'GaussianSmooth', fileEnding = '', size=filterSize, stddevX = stddevX, stddevY = stddevY, angle = filterAngle) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
    
    
def applyGaussian1stDerivFilter(inputimage, outputImage, filterSize, stddevX, stddevY, filterAngle, gdalformat, datatype):
    """ Apply a Gaussian first derivative filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_gau1st.kea'
    imagefilter.applyGaussian1stDerivFilter(inputImage, outImgFile, 3, 1.0, 1.0. 0.0, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Gaussian1st', fileEnding = '', size=filterSize, stddevX = stddevX, stddevY = stddevY, angle = filterAngle) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
def applyGaussian2ndDerivFilter(inputimage, outputImage, filterSize, stddevX, stddevY, filterAngle, gdalformat, datatype):
    """ Apply a Gaussian second derivative filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_gau1st.kea'
    imagefilter.applyGaussian2ndDerivFilter(inputImage, outImgFile, 3, 1.0, 1.0. 0.0, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Gaussian2nd', fileEnding = '', size=filterSize, stddevX = stddevX, stddevY = stddevY, angle = filterAngle) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    

def applyLaplacianFilter(inputimage, outputImage, filterSize, stddev, gdalformat, datatype):
    """ Apply a Laplacian filter to the specified input image.

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
    outImgFile = 'jers1palsar_stack_laplacian.kea'
    imagefilter.applyLaplacianFilter(inputImage, outImgFile, 3, 1.0, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Laplacian', fileEnding = '', size=filterSize, stddev=stddev) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
    
    
 
def applyLeeFilter(inputimage, outputImage, filterSize, nLooks, gdalformat, datatype):
    """ Apply a Lee SAR filter to the specified input image.

Where:

* inputImage - string specifying the input image to be filtered.
* outputImage - string specifying the output image file..
* filterSize - int specfiying the size of the image filter (must be an odd number, i.e., 3, 5, 7, etc).
* nLooks - int specfiying the number of looks applied to the SAR image.
* gdalformat - string specifying the output image format (e.g., KEA).
* datatype - Specifying the output image pixel data type (e.g., rsgislib.TYPE_32FLOAT).

Example::

    import rsgislib
    from rsgislib import imagefilter
    inputImage = 'jers1palsar_stack.kea'
    outImgFile = 'jers1palsar_stack_lee.kea'
    imagefilter.applyLeeFilter(inputImage, outImgFile, 3, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Lee', fileEnding = '', size=filterSize, nLooks=nLooks) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)


