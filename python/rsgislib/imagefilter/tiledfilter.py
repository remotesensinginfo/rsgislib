#! /usr/bin/env python

############################################################################
#  tiledfilter.py
#
#  Copyright 2016 RSGISLib.
#
#  RSGISLib: 'The Remote Sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose:  Provide functionality to perform image filtering using tiling
#           and multiple processing cores.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 14/05/2016
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import rsgislib
from rsgislib import imageutils
from rsgislib import imagefilter

import glob
import os.path
import os
import shutil
from multiprocessing import Pool
from abc import ABCMeta, abstractmethod

class RSGISAbstractFilter:
    """
    Abstract class for filter defining the interface to be used within the performTiledImgFilter function.
    Must define:
    
    * self.filterSize - size of the image filter (must be an odd number)
    * self.gdalformat - the output image file format
    * self.dataType - the output image data type (e.g., rsgislib.TYPE_16UINT)
    """
    __metaclass__ = ABCMeta
    
    def __init__(self):
        self.filterSize = 3
        self.gdalformat = 'KEA'
        self.dataType = rsgislib.TYPE_32FLOAT
    
    @abstractmethod
    def applyFilter(self, inputimage, outputImage): 
        """
        Abstract function through which the input image
        is filtered to produce the output image.
        
        * inputimage - is the name and path of the input image.
        * outputimage - is the name and path of the output image.
        """
        pass
    

    def getFilterSize(self):
        """
        return the size of the image filter
        """
        return self.filterSize
    
    def getFilterHSize(self):
        """
        return the hald size of the image filter (i.e., (filterSize-1)/2)
        """
        hSize = (self.filterSize-1)/2
        return hSize


class RSGISMedianFilter(RSGISAbstractFilter):
    """ A class to apply a median filter """
    
    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a median filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Median', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISMeanFilter(RSGISAbstractFilter):
    """ A class to apply a mean filter """
    
    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a mean filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Mean', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISMinFilter(RSGISAbstractFilter):
    """ A class to apply a min filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a min filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Min', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)
    

class RSGISMaxFilter(RSGISAbstractFilter):
    """ A class to apply a max filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a max filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Max', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISModeFilter(RSGISAbstractFilter):
    """ A class to apply a mode filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a mode filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Mode', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)

    
class RSGISStdDevFilter(RSGISAbstractFilter):
    """ A class to apply a standard deviation filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a std dev filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'StdDev', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)    


class RSGISRangeFilter(RSGISAbstractFilter):
    """ A class to apply a range filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a range filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Range', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)       
    

class RSGISMeanDiffFilter(RSGISAbstractFilter):
    """ A class to apply a mean difference filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a mean difference filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'MeanDiff', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISMeanDiffAbsFilter(RSGISAbstractFilter):
    """ A class to apply a mean absolute difference filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a mean absolute difference filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'MeanDiffAbs', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISTotalDiffFilter(RSGISAbstractFilter):
    """ A class to apply a total difference filter """
    
    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a total (i.e., sum) difference filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'TotalDiff', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISTotalDiffAbsFilter(RSGISAbstractFilter):
    """ A class to apply a total absolution difference filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a total absolute difference filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'TotalDiffAbs', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISCoeffOfVarFilter(RSGISAbstractFilter):
    """ A class to apply a coefficient of variance filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a coefficient of variance filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'CoeffOfVar', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISTotalFilter(RSGISAbstractFilter):
    """ A class to apply a total filter """
    
    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a total filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Total', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISNormVarFilter(RSGISAbstractFilter):
    """ A class to apply a normalised variance filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a normalised variance filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'NormVar', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISNormVarSqrtFilter(RSGISAbstractFilter):
    """ A class to apply a normalised variance square root filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a normalised variance square root filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'NormVarSqrt', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISNormVarLnFilter(RSGISAbstractFilter):
    """ A class to apply a normalised variance log filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a normalised variance natural log filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'NormVarLn', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISTextureVarFilter(RSGISAbstractFilter):
    """ A class to apply a texture variance filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a texture variance filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'TextureVar', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISKuwaharaFilter(RSGISAbstractFilter):
    """ A class to apply a kuwahara filter """

    def __init__(self, filterSize, gdalformat, dataType):
        self.filterSize = filterSize
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a kuwahara filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Kuwahara', fileEnding = '', size=self.filterSize) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISSobelFilter(RSGISAbstractFilter):
    """ A class to apply a sobel filter """

    def __init__(self, gdalformat, dataType):
        self.filterSize = 3
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a sobel filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = '', option = 'xy') )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)    


class RSGISSobelXFilter(RSGISAbstractFilter):
    """ A class to apply a sobel X filter """

    def __init__(self, gdalformat, dataType):
        self.filterSize = 3
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a sobel filter in X axis to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = '', option = 'x') )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISSobelYFilter(RSGISAbstractFilter):
    """ A class to apply a sobel Y filter """

    def __init__(self, gdalformat, dataType):
        self.filterSize = 3
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a sobel filter in Y axis to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Sobel', fileEnding = '', option = 'y') )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISPrewittFilter(RSGISAbstractFilter):
    """ A class to apply a Prewitt filter """

    def __init__(self, gdalformat, dataType):
        self.filterSize = 3
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a Prewitt filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'xy') )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)
    
  
class RSGISPrewittXFilter(RSGISAbstractFilter):
    """ A class to apply a Prewitt X filter """

    def __init__(self, gdalformat, dataType):
        self.filterSize = 3
        self.gdalformat = gdalformat
        self.dataType = dataType
   
    def applyFilter(self, inputimage, outputImage):
        """ Apply a Prewitt filter in X axis to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'x') )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISPrewittYFilter(RSGISAbstractFilter):
    """ A class to apply a Prewitt Y filter """

    def __init__(self, gdalformat, dataType):
        self.filterSize = 3
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a Prewitt filter in Y axis to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Prewitt', fileEnding = '', option = 'y') )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISGaussianSmoothFilter(RSGISAbstractFilter):
    """ A class to apply a Gaussian smoothing filter """

    def __init__(self, filterSize, stddevX, stddevY, filterAngle, gdalformat, dataType):
        self.filterSize = filterSize
        self.stddevX = stddevX
        self.stddevY = stddevY
        self.filterAngle = filterAngle
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a Gaussian smoothing filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'GaussianSmooth', fileEnding = '', size=self.filterSize, stddevX=self.stddevX, stddevY=self.stddevY, angle=self.filterAngle) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)
    

class RSGISGaussian1stDerivFilter(RSGISAbstractFilter):
    """ A class to apply a Gaussian first derivative filter """

    def __init__(self, filterSize, stddevX, stddevY, filterAngle, gdalformat, dataType):
        self.filterSize = filterSize
        self.stddevX = stddevX
        self.stddevY = stddevY
        self.filterAngle = filterAngle
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a Gaussian first derivative filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Gaussian1st', fileEnding = '', size=self.filterSize, stddevX=self.stddevX, stddevY=self.stddevY, angle=self.filterAngle) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


class RSGISGaussian2ndDerivFilter(RSGISAbstractFilter):
    """ A class to apply a Gaussian second derivative filter """

    def __init__(self, filterSize, stddevX, stddevY, filterAngle, gdalformat, dataType):
        self.filterSize = filterSize
        self.stddevX = stddevX
        self.stddevY = stddevY
        self.filterAngle = filterAngle
        self.gdalformat = gdalformat
        self.dataType = dataType
    
    def applyFilter(self, inputimage, outputImage):
        """ Apply a Gaussian second derivative filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Gaussian2nd', fileEnding = '', size=self.filterSize, stddevX=self.stddevX, stddevY=self.stddevY, angle=self.filterAngle) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)

class RSGISLaplacianFilter(RSGISAbstractFilter):
    """ A class to apply a Laplacian filter """

    def __init__(self, filterSize, stddev, gdalformat, dataType):
        self.filterSize = filterSize
        self.stddev = stddev
        self.gdalformat = gdalformat
        self.dataType = dataType

    def applyFilter(self, inputimage, outputImage):
        """ Apply a Laplacian filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Laplacian', fileEnding = '', size=self.filterSize, stddev=self.stddev) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)
    
    
class RSGISLeeFilter(RSGISAbstractFilter):
    """ A class to apply a SAR Lee filter """

    def __init__(self, filterSize, nLooks, gdalformat, dataType):
        self.filterSize = filterSize
        self.nLooks = nLooks
        self.gdalformat = gdalformat
        self.dataType = dataType
 
    def applyFilter(self, inputimage, outputImage):
        """ Apply a Lee SAR filter to the specified input image."""
        outputImageBase, outExt = os.path.splitext(outputImage)
        outExt = outExt.replace(".", "").strip()
        filters = []
        filters.append(imagefilter.FilterParameters(filterType = 'Lee', fileEnding = '', size=self.filterSize, nLooks=self.nLooks) )
        imagefilter.applyfilters(inputimage, outputImageBase, filters, self.gdalformat, outExt, self.dataType)


def _performFilteringFunc(filterParams):
    """
    Clump an image with values provides as an array for use within a multiprocessing Pool
    """
    filterParams[2].applyFilter(filterParams[0], filterParams[1])


def performTiledImgFilter(inputImg, outputImg, filterInst, dataType=None, imgFormat='KEA', tmpDIR='tmp', width=2000, height=2000, nCores=-1):
    """
This function will perform a filtering of an input image where the input image will be tiled and the tiles executed on multiple processing cores. This function is primarily of use for larger images or when using very large filter windows otherwise the over head of tiling and mosaicking are not worth it.

* inputImg - is the file name and path for the input image file.
* outputImg - is the file name and path for the output image file.
* filterInst - is an instance of a filter class available within rsgislib.imagefilter.tiledfilter. 
* datatype - is the output image data type (e.g., rsgislib.TYPE_32FLOAT; Default is None). If None then data type of input image is used.
* imgformat - string with the GDAL image format for the output image (Default = KEA). NOTE. KEA is used as intermediate format internally and therefore needs to be available.
* tmpDIR - the temporary directory where intermediate files will be written (default is 'tmp'). Directory will be created and deleted if does not exist.
* width - int for width of the image tiles used for processing (Default = 2000).
* height - int for height of the image tiles used for processing (Default = 2000).
* nCores - is an int specifying the number of cores to be used for clumping processing.

Example::

    import rsgislib
    from rsgislib.imagefilter import tiledfilter
    from rsgislib import imageutils
    
    inputImage = 'LandsatImg.kea'
    outputImage = 'LandsatImgMedianFilter.kea'
    
    medianFilter = tiledfilter.RSGISMedianFilter(7, "KEA", rsgislib.TYPE_16UINT)
    tiledfilter.performTiledImgFilter(inputImage, outputImage, medianFilter, width=2000, height=2000)
    imageutils.popImageStats(outputImage, usenodataval=False, nodataval=0, calcpyramids=True)
    
    """
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    if nCores <= 0:
        nCores = rsgisUtils.numProcessCores()
        
    uidStr = rsgisUtils.uidGenerator()
    if dataType == None:
        dataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
    
    baseName = os.path.splitext(os.path.basename(inputImg))[0]+"_"+uidStr
    
    imgTilesDIR = os.path.join(tmpDIR, "imgTiles_"+uidStr)
    tilesFilterDIR = os.path.join(tmpDIR, "imgFilterTiles_"+uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)

    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
        
    if not os.path.exists(tilesFilterDIR):
        os.makedirs(tilesFilterDIR)
    
    tileOverlap = filterInst.getFilterHSize()
    
    imageutils.createTiles(inputImg, tilesImgBase, int(width), int(height), int(tileOverlap), False, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), 'kea')
    imageTiles = glob.glob(tilesImgBase+"*.kea")    
    
    filterImgsVals = []
    for tile in imageTiles:
        tileBaseName = os.path.splitext(os.path.basename(tile))[0]
        filterTile = os.path.join(tilesFilterDIR, tileBaseName+'_filter.kea')
        filterImgsVals.append([tile, filterTile, filterInst])    
    
    with Pool(nCores) as p:
        p.map(_performFilteringFunc, filterImgsVals)
    
    imgFilterTiles = glob.glob(os.path.join(tilesFilterDIR,"*_filter.kea"))
    
    numOutBands = rsgisUtils.getImageBandCount(inputImg)
    
    imageutils.createCopyImage(inputImg, outputImg, numOutBands, 0, imgFormat, dataType)
    
    imageutils.includeImagesWithOverlap(outputImg, imgFilterTiles, int(tileOverlap))
    
    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesFilterDIR)
    if createdTmp:
        shutil.rmtree(tmpDIR)
        
def performTiledImgMultiFilter(inputImg, outputImgs, filterInsts, dataType=None, imgFormat='KEA', tmpDIR='tmp', width=2000, height=2000, nCores=-1):
    """
This function will perform the filtering using multiple filters of an input image where the input image will be tiled and the tiles executed on multiple processing cores. This function is primarily of use for larger images or when using very large filter windows otherwise the over head of tiling and mosaicking are not worth it.

* inputImg - is the file name and path for the input image file.
* outputImgs - is a list of file names and paths for the output image files - Note, must be the same length as filterInsts.
* filterInsts - is a list of filter instances of the classes available within rsgislib.imagefilter.tiledfilter  - Note, must be the same length as filterInsts.
* datatype - is the output image data type (e.g., rsgislib.TYPE_32FLOAT; Default is None). If None then data type of input image is used.
* imgformat - string with the GDAL image format for the output image (Default = KEA). NOTE. KEA is used as intermediate format internally and therefore needs to be available.
* tmpDIR - the temporary directory where intermediate files will be written (default is 'tmp'). Directory will be created and deleted if does not exist.
* width - int for width of the image tiles used for processing (Default = 2000).
* height - int for height of the image tiles used for processing (Default = 2000).
* nCores - is an int specifying the number of cores to be used for clumping processing.

Example::

    import rsgislib
    from rsgislib.imagefilter import tiledfilter
    from rsgislib import imageutils
    
    inputImage = 'LandsatImg.kea'
    outputImages = ['LandsatImgMedianFilter.kea', 'LandsatImgNormVarFilter.kea']
    
    filters = [tiledfilter.RSGISMedianFilter(7, "KEA", rsgislib.TYPE_16UINT), tiledfilter.RSGISNormVarFilter(7, "KEA", rsgisUtils.getRSGISLibDataTypeFromImg(inputImage))]
    tiledfilter.performTiledImgMultiFilter(inputImage, outputImages, filters, width=2000, height=2000)
    imageutils.popImageStats(outputImage, usenodataval=False, nodataval=0, calcpyramids=True)
    
    """
    
    if (len(outputImgs) != len(filterInsts)):
        raise rsgislib.RSGISPyException('The same number of filters and output images need to be provided.')
    
    numFilters = len(outputImgs)
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    if nCores <= 0:
        nCores = rsgisUtils.numProcessCores()
        
    uidStr = rsgisUtils.uidGenerator()
    if dataType == None:
        dataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
    
    baseName = os.path.splitext(os.path.basename(inputImg))[0]+"_"+uidStr
    
    imgTilesDIR = os.path.join(tmpDIR, "imgTiles_"+uidStr)
    tilesFilterDIR = os.path.join(tmpDIR, "imgFilterTiles_"+uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)

    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
    
    first = True
    tileOverlap = 0
    
    for filterInst in filterInsts:
        tmpOverlap = filterInst.getFilterHSize()
        if first:
            tileOverlap = tmpOverlap
            first = False
        elif tmpOverlap > tileOverlap:
            tileOverlap = tmpOverlap
        
    imageutils.createTiles(inputImg, tilesImgBase, width, height, tileOverlap, False, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), 'kea')
    imageTiles = glob.glob(tilesImgBase+"*.kea")    
    
    for i in range(numFilters):
        filterInst = filterInsts[i]
        outputImg = outputImgs[i]
        
        if not os.path.exists(tilesFilterDIR):
            os.makedirs(tilesFilterDIR)
        
        filterImgsVals = []
        for tile in imageTiles:
            tileBaseName = os.path.splitext(os.path.basename(tile))[0]
            filterTile = os.path.join(tilesFilterDIR, tileBaseName+'_filter.kea')
            filterImgsVals.append([tile, filterTile, filterInst])    
        
        with Pool(nCores) as p:
            p.map(_performFilteringFunc, filterImgsVals)
        
        imgFilterTiles = glob.glob(os.path.join(tilesFilterDIR,"*_filter.kea"))
        
        numOutBands = rsgisUtils.getImageBandCount(inputImg)
        
        imageutils.createCopyImage(inputImg, outputImg, numOutBands, 0, imgFormat, dataType)
        
        imageutils.includeImagesWithOverlap(outputImg, imgFilterTiles, int(filterInst.getFilterHSize()))
        
        shutil.rmtree(tilesFilterDIR)
        
    shutil.rmtree(imgTilesDIR)
    if createdTmp:
        shutil.rmtree(tmpDIR)

