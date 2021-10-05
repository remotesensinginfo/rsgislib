#!/usr/bin/env python
"""
This namespace contains rsgislib Python bindings 

Please be aware that the following variables have been 
defined to match enums within RSGISLib.

Data Types for images:

    * TYPE_UNDEFINED = 0
    * TYPE_8INT = 1
    * TYPE_16INT = 2
    * TYPE_32INT = 3
    * TYPE_64INT = 4
    * TYPE_8UINT = 5
    * TYPE_16UINT = 6
    * TYPE_32UINT = 7
    * TYPE_64UINT = 8
    * TYPE_32FLOAT = 9
    * TYPE_64FLOAT = 10

Methods for the Maximum Likelihood Classifier:

    * METHOD_SAMPLES = 0        # as calculated by ML
    * METHOD_AREA = 1           # priors set by the relative area
    * METHOD_EQUAL = 2          # priors all equal
    * METHOD_USERDEFINED = 3    # priors passed in to function
    * METHOD_WEIGHTED = 4       # priors by area but with a weight applied

Shape indexes used with RasterGIS:

    * SHAPE_SHAPENA = 0
    * SHAPE_SHAPEAREA = 1
    * SHAPE_ASYMMETRY = 2
    * SHAPE_BORDERINDEX = 3
    * SHAPE_BORDERLENGTH = 4
    * SHAPE_COMPACTNESS = 5
    * SHAPE_DENSITY = 6
    * SHAPE_ELLIPTICFIT = 7
    * SHAPE_LENGTH = 8
    * SHAPE_LENGTHWIDTH = 9
    * SHAPE_WIDTH = 10
    * SHAPE_MAINDIRECTION = 11
    * SHAPE_RADIUSLARGESTENCLOSEDELLIPSE = 12
    * SHAPE_RADIUSSMALLESTENCLOSEDELLIPSE = 13
    * SHAPE_RECTANGULARFIT = 14
    * SHAPE_ROUNDNESS = 15
    * SHAPE_SHAPEINDEX = 16

Methods of initialising KMEANS:

    * INITCLUSTER_RANDOM = 0
    * INITCLUSTER_DIAGONAL_FULL = 1
    * INITCLUSTER_DIAGONAL_STDDEV = 2
    * INITCLUSTER_DIAGONAL_FULL_ATTACH = 3
    * INITCLUSTER_DIAGONAL_STDDEV_ATTACH = 4
    * INITCLUSTER_KPP = 5
    
    
Methods of calculating distance:

    * DIST_UNDEFINED = 0
    * DIST_EUCLIDEAN = 1
    * DIST_MAHALANOBIS = 2
    * DIST_MANHATTEN = 3
    * DIST_MINKOWSKI = 4
    * DIST_CHEBYSHEV = 5
    * DIST_MUTUALINFO = 6
    
Methods of summarising data:

    * SUMTYPE_UNDEFINED = 0
    * SUMTYPE_MODE = 1
    * SUMTYPE_MEAN = 2
    * SUMTYPE_MEDIAN = 3
    * SUMTYPE_MIN = 4
    * SUMTYPE_MAX = 5
    * SUMTYPE_STDDEV = 6
    * SUMTYPE_COUNT = 7
    * SUMTYPE_RANGE = 8
    * SUMTYPE_SUM = 9
    
Constants specifying how bands should be treated when sharpening (see rsgislib.imageutils)
    * SHARP_RES_IGNORE = 0
    * SHARP_RES_LOW = 1
    * SHARP_RES_HIGH = 2

"""
from __future__ import print_function

import os
import time
import datetime
import math
import sys

import osgeo.osr as osr
import osgeo.ogr as ogr
import osgeo.gdal as gdal

gdal.UseExceptions()

TYPE_UNDEFINED = 0
TYPE_8INT = 1
TYPE_16INT = 2
TYPE_32INT = 3
TYPE_64INT = 4
TYPE_8UINT = 5
TYPE_16UINT = 6
TYPE_32UINT = 7
TYPE_64UINT = 8
TYPE_32FLOAT = 9
TYPE_64FLOAT = 10

DIST_UNDEFINED = 0
DIST_EUCLIDEAN = 1
DIST_MAHALANOBIS = 2
DIST_MANHATTEN = 3
DIST_MINKOWSKI = 4
DIST_CHEBYSHEV = 5
DIST_MUTUALINFO = 6

SUMTYPE_UNDEFINED = 0
SUMTYPE_MODE = 1
SUMTYPE_MEAN = 2
SUMTYPE_MEDIAN = 3
SUMTYPE_MIN = 4
SUMTYPE_MAX = 5
SUMTYPE_STDDEV = 6
SUMTYPE_COUNT = 7
SUMTYPE_RANGE = 8
SUMTYPE_SUM = 9

METHOD_SAMPLES = 0      # as calculated by ML
METHOD_AREA = 1         # priors set by the relative area
METHOD_EQUAL = 2        # priors all equal
METHOD_USERDEFINED = 3  # priors passed in to function
METHOD_WEIGHTED = 4     # priors by area but with a weight applied

SHAPE_SHAPENA = 0
SHAPE_SHAPEAREA = 1
SHAPE_ASYMMETRY = 2
SHAPE_BORDERINDEX = 3
SHAPE_BORDERLENGTH = 4
SHAPE_COMPACTNESS = 5
SHAPE_DENSITY = 6
SHAPE_ELLIPTICFIT = 7
SHAPE_LENGTH = 8
SHAPE_LENGTHWIDTH = 9
SHAPE_WIDTH = 10
SHAPE_MAINDIRECTION = 11
SHAPE_RADIUSLARGESTENCLOSEDELLIPSE = 12
SHAPE_RADIUSSMALLESTENCLOSEDELLIPSE = 13
SHAPE_RECTANGULARFIT = 14
SHAPE_ROUNDNESS = 15
SHAPE_SHAPEINDEX = 16

INITCLUSTER_RANDOM = 0
INITCLUSTER_DIAGONAL_FULL = 1
INITCLUSTER_DIAGONAL_STDDEV = 2
INITCLUSTER_DIAGONAL_FULL_ATTACH = 3
INITCLUSTER_DIAGONAL_STDDEV_ATTACH = 4
INITCLUSTER_KPP = 5

SHARP_RES_IGNORE = 0
SHARP_RES_LOW = 1
SHARP_RES_HIGH = 2


def getRSGISLibVersion():
    """ Calls rsgis-config to get the version number. """

    # Try calling rsgis-config to get minor version number
    try:
        import subprocess
        out = subprocess.Popen('rsgis-config --version',shell=True,stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        (stdout, stderr) = out.communicate()
        versionStr = stdout.decode()
        versionStr = versionStr.split('\n')[0]
    except Exception:
        versionStr = 'NA'
    return(versionStr)

__version__ = getRSGISLibVersion()

py_sys_version_str = "{}.{}".format(sys.version_info.major, sys.version_info.minor)
py_sys_version_flt = float(py_sys_version_str)


class RSGISPyException(Exception):
    """
    A class representing the RSGIS exception.
    """
    
    def __init__(self, value):
        """
        Init for the RSGISPyException class
        """
        self.value = value
        
    def __str__(self):
        """
        Return a string representation of the exception
        """
        return repr(self.value)


class RSGISGDALErrorHandler(object):
    """
    A class representing a generic GDAL Error Handler which
    can be used to pick up GDAL warnings rather than just
    failure errors.
    """

    def __init__(self):
        """
        Init for RSGISGDALErrorHandler. Class attributes are err_level, err_no and err_msg

        """
        from osgeo import gdal
        self.err_level = gdal.CE_None
        self.err_no = 0
        self.err_msg = ''

    def handler(self, err_level, err_no, err_msg):
        """
        The handler function which is called with the error information.

        :param err_level: The level of the error
        :param err_no: The error number
        :param err_msg: The message (string) associated with the error.

        """
        self.err_level = err_level
        self.err_no = err_no
        self.err_msg = err_msg


def getRSGISLibDataType(gdaltype):
    """
    Convert from GDAL data type string to RSGISLib data type int.

    :return: int

    """
    gdaltype = gdaltype.lower()
    if gdaltype == 'int8':
        return TYPE_8INT
    elif gdaltype == 'int16':
        return TYPE_16INT
    elif gdaltype == 'int32':
        return TYPE_32INT
    elif gdaltype == 'int64':
        return TYPE_64INT
    elif gdaltype == 'byte' or gdaltype == 'uint8':
        return TYPE_8UINT
    elif gdaltype == 'uint16':
        return TYPE_16UINT
    elif gdaltype == 'uint32':
        return TYPE_32UINT
    elif gdaltype == 'uint64':
        return TYPE_64UINT
    elif gdaltype == 'float32':
        return TYPE_32FLOAT
    elif gdaltype == 'float64':
        return TYPE_64FLOAT
    else:
        raise RSGISPyException("The data type '" + str(gdaltype) + "' is unknown / not supported.")

def getGDALDataType(rsgislib_datatype):
    """
    Convert from RSGIS data type to GDAL data type int.

    :return: int

    """
    if rsgislib_datatype == TYPE_16INT:
        return gdal.GDT_Int16
    elif rsgislib_datatype == TYPE_32INT:
        return gdal.GDT_Int32
    elif rsgislib_datatype == TYPE_8UINT:
        return gdal.GDT_Byte
    elif rsgislib_datatype == TYPE_16UINT:
        return gdal.GDT_UInt16
    elif rsgislib_datatype == TYPE_32UINT:
        return gdal.GDT_UInt32
    elif rsgislib_datatype == TYPE_32FLOAT:
        return gdal.GDT_Float32
    elif rsgislib_datatype == TYPE_64FLOAT:
        return gdal.GDT_Float64
    else:
        raise RSGISPyException("The data type '" + str(rsgislib_datatype) + "' is unknown / not supported.")

def getNumpyDataType(rsgislib_datatype):
    """
    Convert from RSGISLib data type to numpy datatype

    :param rsgis_datatype:
    :return: numpy datatype
    """
    import numpy
    numpyDT = numpy.float32
    if rsgislib_datatype == TYPE_8INT:
        numpyDT = numpy.int8
    elif rsgislib_datatype == TYPE_16INT:
        numpyDT = numpy.int16
    elif rsgislib_datatype == TYPE_32INT:
        numpyDT = numpy.int32
    elif rsgislib_datatype == TYPE_64INT:
        numpyDT = numpy.int64
    elif rsgislib_datatype == TYPE_8UINT:
        numpyDT = numpy.uint8
    elif rsgislib_datatype == TYPE_16UINT:
        numpyDT = numpy.uint16
    elif rsgislib_datatype == TYPE_32UINT:
        numpyDT = numpy.uint32
    elif rsgislib_datatype == TYPE_64UINT:
        numpyDT = numpy.uint64
    elif rsgislib_datatype == TYPE_32FLOAT:
        numpyDT = numpy.float32
    elif rsgislib_datatype == TYPE_64FLOAT:
        numpyDT = numpy.float64
    else:
        raise Exception('Datatype was not recognised.')
    return numpyDT

def getNumpyCharCodesDataType(rsgislib_datatype):
    """
    Convert from RSGISLib data type to numpy datatype

    :param rsgis_datatype:
    :return: numpy character code datatype
    """
    import numpy
    numpyDT = numpy.dtype(numpy.float32).char
    if rsgislib_datatype == TYPE_8INT:
        numpyDT = numpy.dtype(numpy.int8).char
    elif rsgislib_datatype == TYPE_16INT:
        numpyDT = numpy.dtype(numpy.int16).char
    elif rsgislib_datatype == TYPE_32INT:
        numpyDT = numpy.dtype(numpy.int32).char
    elif rsgislib_datatype == TYPE_64INT:
        numpyDT = numpy.dtype(numpy.int64).char
    elif rsgislib_datatype == TYPE_8UINT:
        numpyDT = numpy.dtype(numpy.uint8).char
    elif rsgislib_datatype == TYPE_16UINT:
        numpyDT = numpy.dtype(numpy.uint16).char
    elif rsgislib_datatype == TYPE_32UINT:
        numpyDT = numpy.dtype(numpy.uint32).char
    elif rsgislib_datatype == TYPE_64UINT:
        numpyDT = numpy.dtype(numpy.uint64).char
    elif rsgislib_datatype == TYPE_32FLOAT:
        numpyDT = numpy.dtype(numpy.float32).char
    elif rsgislib_datatype == TYPE_64FLOAT:
        numpyDT = numpy.dtype(numpy.float64).char
    else:
        raise Exception('Datatype was not recognised.')
    return numpyDT


class RSGISTime (object):
    """
    Class to calculate run time for a function, format and print out (similar to for XML interface).

    Need to call start before running function and end immediately after.
    Example::

        t = RSGISTime()
        t.start()
        rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalformat, False, 0)
        t.end()

    Note, this is only designed to provide some general feedback, for benchmarking the timeit module
    is better suited.

    """

    def __init__(self):
        self.startTime = time.time()
        self.endTime = time.time()

    def start(self, print_start_time=False):
        """
        Start timer, optionally printing start time

        :param print_start_time: A boolean specifying whether the start time should be printed to console.

        """
        self.startTime = time.time()
        if print_start_time:
            print(time.strftime('Start Time: %H:%M:%S, %a %b %m %Y.'))

    def end(self, report_diff=True, precede_str="", post_str=""):
        """ 
        End timer and optionally print difference.
        If precedeStr or postStr have a value then they will be used instead of the generic wording around the time.
        
        precede_str + " " + time + " " + postStr

        :param report_diff: A boolean specifiying whether time difference should be printed to console.
        :param precede_str: A string which is printed ahead of time difference
        :param post_str: A string which is printed after the time difference

        """
        self.endTime = time.time()
        if report_diff:
            self.calcDiff(precede_str, post_str)

    def calcDiff(self, precede_str="", post_str=""):
        """
        Calculate time difference, format and print.

        :param precede_str: A string which is printed ahead of time difference
        :param post_str: A string which is printed after the time difference

        """
        timeDiff = self.endTime - self.startTime
        
        useCustomMss = False
        if (len(precede_str) > 0) or (len(post_str) > 0):
            useCustomMss = True
        
        if timeDiff <= 1:
            if useCustomMss:
                outStr = "{} in less than a second {}".format(precede_str, post_str)
                print(outStr)
            else:
                print("Algorithm Completed in less than a second.")
        else:
            timeObj = datetime.datetime.utcfromtimestamp(timeDiff)
            timeDiffStr = timeObj.strftime('%H:%M:%S')
            if useCustomMss:
                print("{} {} {}".format(precede_str, timeDiffStr, post_str))
            else:
                print('Algorithm Completed in {}.'.format(timeDiffStr))
        
class TQDMProgressBar(object):
    """
    Uses TQDM TermProgress to print a progress bar to the terminal
    """
    def __init__(self):
        self.lprogress = 0

    def setTotalSteps(self, steps):
        import tqdm
        self.pbar = tqdm.tqdm(total=steps)
        self.lprogress = 0

    def setProgress(self, progress):
        step = progress - self.lprogress
        self.pbar.update(step)
        self.lprogress = progress

    def reset(self):
        self.pbar.close()
        import tqdm
        self.pbar = tqdm.tqdm(total=100)
        self.lprogress = 0

    def setLabelText(self, text):
        sys.stdout.write('\n%s\n' % text)

    def wasCancelled(self):
        return False

    def displayException(self, trace):
        sys.stdout.write(trace)

    def displayWarning(self, text):
        sys.stdout.write("Warning: %s\n" % text)

    def displayError(self, text):
        sys.stdout.write("Error: %s\n" % text)

    def displayInfo(self, text):
        sys.stdout.write("Info: %s\n" % text)
