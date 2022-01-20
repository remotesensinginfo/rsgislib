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
import math
from multiprocessing import Pool
from abc import ABCMeta, abstractmethod


class RSGISAbstractFilter:
    """
    Abstract class for filter defining the interface to be used within the
    perform_tiled_img_filter function.

    :param self.filter_size: size of the image filter (must be an odd number)
    :param self.gdalformat: the output image file format
    :param self.datatype: the output image data type (e.g., rsgislib.TYPE_16UINT)
    """

    __metaclass__ = ABCMeta

    def __init__(self):
        self.filter_size = 3
        self.gdalformat = "KEA"
        self.datatype = rsgislib.TYPE_32FLOAT

    @abstractmethod
    def applyFilter(self, input_img, output_img):
        """
        Abstract function through which the input image
        is filtered to produce the output image.

        :param input_img: is the name and path of the input image.
        :param output_img: is the name and path of the output image.
        """
        pass

    def getFilterSize(self):
        """
        return the size of the image filter
        """
        return self.filter_size

    def getFilterHSize(self):
        """
        return the hald size of the image filter (i.e., (filter_size-1)/2)
        """
        hSize = (self.filter_size - 1) / 2
        return hSize


class RSGISMedianFilter(RSGISAbstractFilter):
    """A class to apply a median filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a median filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Median", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISMeanFilter(RSGISAbstractFilter):
    """A class to apply a mean filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a mean filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Mean", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISMinFilter(RSGISAbstractFilter):
    """A class to apply a min filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a min filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Min", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISMaxFilter(RSGISAbstractFilter):
    """A class to apply a max filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a max filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Max", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISModeFilter(RSGISAbstractFilter):
    """A class to apply a mode filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a mode filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Mode", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISStdDevFilter(RSGISAbstractFilter):
    """A class to apply a standard deviation filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a std dev filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="StdDev", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISRangeFilter(RSGISAbstractFilter):
    """A class to apply a range filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a range filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Range", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISMeanDiffFilter(RSGISAbstractFilter):
    """A class to apply a mean difference filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a mean difference filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="MeanDiff", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISMeanDiffAbsFilter(RSGISAbstractFilter):
    """A class to apply a mean absolute difference filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a mean absolute difference filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="MeanDiffAbs", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISTotalDiffFilter(RSGISAbstractFilter):
    """A class to apply a total difference filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a total (i.e., sum) difference filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="TotalDiff", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISTotalDiffAbsFilter(RSGISAbstractFilter):
    """A class to apply a total absolution difference filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a total absolute difference filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="TotalDiffAbs", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISCoeffOfVarFilter(RSGISAbstractFilter):
    """A class to apply a coefficient of variance filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a coefficient of variance filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="CoeffOfVar", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISTotalFilter(RSGISAbstractFilter):
    """A class to apply a total filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a total filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Total", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISNormVarFilter(RSGISAbstractFilter):
    """A class to apply a normalised variance filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a normalised variance filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="NormVar", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISNormVarSqrtFilter(RSGISAbstractFilter):
    """A class to apply a normalised variance square root filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """
        Apply a normalised variance square root filter to the specified input image.
        """
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="NormVarSqrt", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISNormVarLnFilter(RSGISAbstractFilter):
    """A class to apply a normalised variance log filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """
        Apply a normalised variance natural log filter to the specified input image.
        """
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="NormVarLn", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISTextureVarFilter(RSGISAbstractFilter):
    """A class to apply a texture variance filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a texture variance filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="TextureVar", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISKuwaharaFilter(RSGISAbstractFilter):
    """A class to apply a kuwahara filter"""

    def __init__(self, filter_size, gdalformat, datatype):
        self.filter_size = filter_size
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a kuwahara filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Kuwahara", file_ending="", size=self.filter_size
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISSobelFilter(RSGISAbstractFilter):
    """A class to apply a sobel filter"""

    def __init__(self, gdalformat, datatype):
        self.filter_size = 3
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a sobel filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Sobel", file_ending="", option="xy"
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISSobelXFilter(RSGISAbstractFilter):
    """A class to apply a sobel X filter"""

    def __init__(self, gdalformat, datatype):
        self.filter_size = 3
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a sobel filter in X axis to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Sobel", file_ending="", option="x"
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISSobelYFilter(RSGISAbstractFilter):
    """A class to apply a sobel Y filter"""

    def __init__(self, gdalformat, datatype):
        self.filter_size = 3
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a sobel filter in Y axis to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Sobel", file_ending="", option="y"
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISPrewittFilter(RSGISAbstractFilter):
    """A class to apply a Prewitt filter"""

    def __init__(self, gdalformat, datatype):
        self.filter_size = 3
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Prewitt filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Prewitt", file_ending="", option="xy"
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISPrewittXFilter(RSGISAbstractFilter):
    """A class to apply a Prewitt X filter"""

    def __init__(self, gdalformat, datatype):
        self.filter_size = 3
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Prewitt filter in X axis to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Prewitt", file_ending="", option="x"
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISPrewittYFilter(RSGISAbstractFilter):
    """A class to apply a Prewitt Y filter"""

    def __init__(self, gdalformat, datatype):
        self.filter_size = 3
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Prewitt filter in Y axis to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Prewitt", file_ending="", option="y"
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISGaussianSmoothFilter(RSGISAbstractFilter):
    """A class to apply a Gaussian smoothing filter"""

    def __init__(
        self, filter_size, stddev_x, stddev_y, filter_angle, gdalformat, datatype
    ):
        self.filter_size = filter_size
        self.stddev_x = stddev_x
        self.stddev_y = stddev_y
        self.filter_angle = filter_angle
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Gaussian smoothing filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="GaussianSmooth",
                file_ending="",
                size=self.filter_size,
                stddev_x=self.stddev_x,
                stddev_y=self.stddev_y,
                angle=self.filter_angle,
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISGaussian1stDerivFilter(RSGISAbstractFilter):
    """A class to apply a Gaussian first derivative filter"""

    def __init__(
        self, filter_size, stddev_x, stddev_y, filter_angle, gdalformat, datatype
    ):
        self.filter_size = filter_size
        self.stddev_x = stddev_x
        self.stddev_y = stddev_y
        self.filter_angle = filter_angle
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Gaussian first derivative filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Gaussian1st",
                file_ending="",
                size=self.filter_size,
                stddev_x=self.stddev_x,
                stddev_y=self.stddev_y,
                angle=self.filter_angle,
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISGaussian2ndDerivFilter(RSGISAbstractFilter):
    """A class to apply a Gaussian second derivative filter"""

    def __init__(
        self, filter_size, stddev_x, stddev_y, filter_angle, gdalformat, datatype
    ):
        self.filter_size = filter_size
        self.stddev_x = stddev_x
        self.stddev_y = stddev_y
        self.filter_angle = filter_angle
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Gaussian second derivative filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Gaussian2nd",
                file_ending="",
                size=self.filter_size,
                stddev_x=self.stddev_x,
                stddev_y=self.stddev_y,
                angle=self.filter_angle,
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISLaplacianFilter(RSGISAbstractFilter):
    """A class to apply a Laplacian filter"""

    def __init__(self, filter_size, stddev, gdalformat, datatype):
        self.filter_size = filter_size
        self.stddev = stddev
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Laplacian filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Laplacian",
                file_ending="",
                size=self.filter_size,
                stddev=self.stddev,
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


class RSGISLeeFilter(RSGISAbstractFilter):
    """A class to apply a SAR Lee filter"""

    def __init__(self, filter_size, n_looks, gdalformat, datatype):
        self.filter_size = filter_size
        self.n_looks = n_looks
        self.gdalformat = gdalformat
        self.datatype = datatype

    def applyFilter(self, input_img, output_img):
        """Apply a Lee SAR filter to the specified input image."""
        out_image_base, out_img_ext = os.path.splitext(output_img)
        out_img_ext = out_img_ext.replace(".", "").strip()
        filters = []
        filters.append(
            imagefilter.FilterParameters(
                filter_type="Lee",
                file_ending="",
                size=self.filter_size,
                n_looks=self.n_looks,
            )
        )
        imagefilter.apply_filters(
            input_img,
            out_image_base,
            filters,
            self.gdalformat,
            out_img_ext,
            self.datatype,
        )


def _perform_filtering_func(filter_params):
    """
    Clump an image with values provides as an array for use within a
    multiprocessing Pool
    """
    filter_params[2].applyFilter(filter_params[0], filter_params[1])


def perform_tiled_img_filter(
    input_img,
    output_img,
    filter_inst,
    datatype=None,
    gdalformat="KEA",
    tmp_dir="tmp",
    width=2000,
    height=2000,
    n_cores=-1,
):
    """
    This function will perform a filtering of an input image where the input image
    will be tiled and the tiles executed on multiple processing cores. This function
    is primarily of use for larger images or when using very large filter windows
    otherwise the over head of tiling and mosaicking are not worth it.

    :param input_img: is the file name and path for the input image file.
    :param output_img: is the file name and path for the output image file.
    :param filter_inst: is an instance of a filter class available within
                        rsgislib.imagefilter.tiledfilter.
    :param datatype: is the output image data type (e.g., rsgislib.TYPE_32FLOAT;
                     Default is None). If None then data type of input image is used.
    :param gdalformat: string with the GDAL image format for the output image
                       (Default = KEA). NOTE. KEA is used as intermediate format
                       internally and therefore needs to be available.
    :param tmp_dir: the temporary directory where intermediate files will be written
                    (default is 'tmp'). Directory will be created and deleted if does
                    not exist.
    :param width: int for width of the image tiles used for processing (Default = 2000).
    :param height: int for height of the image tiles used for processing
                   (Default = 2000).
    :param n_cores: is an int specifying the number of cores to be used for processing.

    .. code:: python

        import rsgislib
        from rsgislib.imagefilter import tiledfilter
        from rsgislib import imageutils

        input_img = 'LandsatImg.kea'
        output_img = 'LandsatImgMedianFilter.kea'

        medianFilter = tiledfilter.RSGISMedianFilter(7, "KEA", rsgislib.TYPE_16UINT)
        tiledfilter.perform_tiled_img_filter(input_img, output_img, medianFilter,
                                             width=2000, height=2000)
        imageutils.pop_img_stats(output_img, False, 0, True)

    """
    import rsgislib.tools.utils

    createdTmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        createdTmp = True

    if n_cores <= 0:
        n_cores = rsgislib.tools.utils.num_process_cores()

    uidStr = rsgislib.tools.utils.uid_generator()
    if datatype is None:
        datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)

    baseName = os.path.splitext(os.path.basename(input_img))[0] + "_" + uidStr

    imgTilesDIR = os.path.join(tmp_dir, "imgTiles_" + uidStr)
    tilesFilterDIR = os.path.join(tmp_dir, "imgFilterTiles_" + uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)

    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)

    if not os.path.exists(tilesFilterDIR):
        os.makedirs(tilesFilterDIR)

    tileOverlap = math.ceil(filter_inst.getFilterHSize())

    imageutils.create_tiles(
        input_img,
        tilesImgBase,
        int(width),
        int(height),
        int(tileOverlap),
        False,
        "KEA",
        datatype,
        "kea",
    )
    imageTiles = glob.glob(tilesImgBase + "*.kea")

    filterImgsVals = []
    for tile in imageTiles:
        tileBaseName = os.path.splitext(os.path.basename(tile))[0]
        filterTile = os.path.join(tilesFilterDIR, tileBaseName + "_filter.kea")
        filterImgsVals.append([tile, filterTile, filter_inst])

    with Pool(n_cores) as p:
        p.map(_perform_filtering_func, filterImgsVals)

    imgFilterTiles = glob.glob(os.path.join(tilesFilterDIR, "*_filter.kea"))

    numOutBands = rsgislib.imageutils.get_img_band_count(input_img)

    imageutils.create_copy_img(
        input_img, output_img, numOutBands, 0, gdalformat, datatype
    )

    imageutils.include_imgs_with_overlap(output_img, imgFilterTiles, int(tileOverlap))

    shutil.rmtree(imgTilesDIR)
    shutil.rmtree(tilesFilterDIR)
    if createdTmp:
        shutil.rmtree(tmp_dir)


def perform_tiled_img_multi_filter(
    input_img,
    output_imgs,
    filter_insts,
    datatype=None,
    gdalformat="KEA",
    tmp_dir="tmp",
    width=2000,
    height=2000,
    n_cores=-1,
):
    """
    This function will perform the filtering using multiple filters of an input image
    where the input image will be tiled and the tiles executed on multiple processing
    cores. This function is primarily of use for larger images or when using very large
    filter windows otherwise the over head of tiling and mosaicking are not worth it.

    :param input_img: is the file name and path for the input image file.
    :param output_imgs: is a list of file names and paths for the output image files -
                       Note, must be the same length as filter_insts.
    :param filter_insts: is a list of filter instances of the classes available
                         within rsgislib.imagefilter.tiledfilter  - Note, must be the
                         same length as filter_insts.
    :param datatype: is the output image data type (e.g., rsgislib.TYPE_32FLOAT;
                     Default is None). If None then data type of input image is used.
    :param gdalformat: string with the GDAL image format for the output image
                       (Default = KEA). NOTE. KEA is used as intermediate format
                       internally and therefore needs to be available.
    :param tmp_dir: the temporary directory where intermediate files will be
                    written (default is 'tmp'). Directory will be created and
                    deleted if does not exist.
    :param width: int for width of the image tiles used for processing (Default = 2000).
    :param height: int for height of the image tiles used for processing
                   (Default = 2000).
    :param n_cores: is an int specifying the number of cores to be used for processing.

    .. code:: python

        import rsgislib
        from rsgislib.imagefilter import tiledfilter
        from rsgislib import imageutils

        input_img = 'LandsatImg.kea'
        outputImages = ['LandsatImgMedianFilter.kea', 'LandsatImgNormVarFilter.kea']

        filters = [tiledfilter.RSGISMedianFilter(7, "KEA", rsgislib.TYPE_16UINT),
                  tiledfilter.RSGISNormVarFilter(7, "KEA", rsgislib.TYPE_16UINT]
        tiledfilter.perform_tiled_img_multi_filter(input_img, outputImages,
                                                   filters, width=2000, height=2000)
        imageutils.pop_img_stats(output_img, False, 0, True)

    """
    import rsgislib.tools.utils

    if len(output_imgs) != len(filter_insts):
        raise rsgislib.RSGISPyException(
            "The same number of filters and output images need to be provided."
        )

    numFilters = len(output_imgs)

    createdTmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        createdTmp = True

    if n_cores <= 0:
        n_cores = rsgislib.tools.utils.num_process_cores()

    uidStr = rsgislib.tools.utils.uid_generator()
    if datatype is None:
        datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)

    baseName = os.path.splitext(os.path.basename(input_img))[0] + "_" + uidStr

    imgTilesDIR = os.path.join(tmp_dir, "imgTiles_" + uidStr)
    tilesFilterDIR = os.path.join(tmp_dir, "imgFilterTiles_" + uidStr)
    tilesImgBase = os.path.join(imgTilesDIR, baseName)

    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)

    first = True
    tileOverlap = 0

    for filter_inst in filter_insts:
        tmpOverlap = math.ceil(filter_inst.getFilterHSize())
        if first:
            tileOverlap = tmpOverlap
            first = False
        elif tmpOverlap > tileOverlap:
            tileOverlap = tmpOverlap

    imageutils.create_tiles(
        input_img,
        tilesImgBase,
        int(width),
        int(height),
        int(tileOverlap),
        False,
        "KEA",
        datatype,
        "kea",
    )
    imageTiles = glob.glob(tilesImgBase + "*.kea")

    for i in range(numFilters):
        filter_inst = filter_insts[i]
        output_img = output_imgs[i]

        if not os.path.exists(tilesFilterDIR):
            os.makedirs(tilesFilterDIR)

        filterImgsVals = []
        for tile in imageTiles:
            tileBaseName = os.path.splitext(os.path.basename(tile))[0]
            filterTile = os.path.join(tilesFilterDIR, tileBaseName + "_filter.kea")
            filterImgsVals.append([tile, filterTile, filter_inst])

        with Pool(n_cores) as p:
            p.map(_perform_filtering_func, filterImgsVals)

        imgFilterTiles = glob.glob(os.path.join(tilesFilterDIR, "*_filter.kea"))

        numOutBands = rsgislib.imageutils.get_img_band_count(input_img)

        imageutils.create_copy_img(
            input_img, output_img, numOutBands, 0, gdalformat, datatype
        )

        imageutils.include_imgs_with_overlap(
            output_img, imgFilterTiles, int(filter_inst.getFilterHSize())
        )

        shutil.rmtree(tilesFilterDIR)

    shutil.rmtree(imgTilesDIR)
    if createdTmp:
        shutil.rmtree(tmp_dir)
