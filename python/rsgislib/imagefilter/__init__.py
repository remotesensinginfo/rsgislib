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
    clumpsFile = 'jers1palsar_stack_sobel.kea'
    imagefilter.applySobelFilter(inputImage, clumpsFile, "KEA", rsgislib.TYPE_32FLOAT)

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
    clumpsFile = 'jers1palsar_stack_sobelx.kea'
    imagefilter.applySobelXFilter(inputImage, clumpsFile, "KEA", rsgislib.TYPE_32FLOAT)

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
    clumpsFile = 'jers1palsar_stack_sobely.kea'
    imagefilter.applySobelYFilter(inputImage, clumpsFile, "KEA", rsgislib.TYPE_32FLOAT)

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
    clumpsFile = 'jers1palsar_stack_prewitt.kea'
    imagefilter.applyPrewittFilter(inputImage, clumpsFile, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'xy') )
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
    clumpsFile = 'jers1palsar_stack_mean3.kea'
    imagefilter.applyMeanFilter(inputImage, clumpsFile, 3, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Mean', fileEnding = '', size=filterSize) )
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
    clumpsFile = 'jers1palsar_stack_gausmooth.kea'
    imagefilter.applyGaussianSmoothFilter(inputImage, clumpsFile, 3, 1.0, 1.0. 0.0, "KEA", rsgislib.TYPE_32FLOAT)

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
    clumpsFile = 'jers1palsar_stack_gau1st.kea'
    imagefilter.applyGaussian1stDerivFilter(inputImage, clumpsFile, 3, 1.0, 1.0. 0.0, "KEA", rsgislib.TYPE_32FLOAT)

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
    clumpsFile = 'jers1palsar_stack_gau1st.kea'
    imagefilter.applyGaussian2ndDerivFilter(inputImage, clumpsFile, 3, 1.0, 1.0. 0.0, "KEA", rsgislib.TYPE_32FLOAT)

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
    clumpsFile = 'jers1palsar_stack_laplacian.kea'
    imagefilter.applyLaplacianFilter(inputImage, clumpsFile, 3, 1.0, "KEA", rsgislib.TYPE_32FLOAT)

    """
    outputImageBase, outExt = os.path.splitext(outputImage)
    outExt = outExt.replace(".", "").strip()
    filters = []
    filters.append(FilterParameters(filterType = 'Laplacian', fileEnding = '', size=filterSize, stddev=stddev) )
    applyfilters(inputimage, outputImageBase, filters, gdalformat, outExt, datatype)
    
    
    

