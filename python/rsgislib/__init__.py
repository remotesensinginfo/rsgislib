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

Constants specifying how bands should be treated when sharpening
(see rsgislib.imageutils)

    * SHARP_RES_IGNORE = 0
    * SHARP_RES_LOW = 1
    * SHARP_RES_HIGH = 2

Options for interpolating raster data:

    * INTERP_NEAREST_NEIGHBOUR = 0
    * INTERP_BILINEAR = 1
    * INTERP_CUBIC = 2
    * INTERP_CUBICSPLINE = 3
    * INTERP_LANCZOS = 4
    * INTERP_AVERAGE = 5
    * INTERP_MODE = 6

Options for image stretching / normalisation:

    * IMG_STRETCH_NONE = 0
    * IMG_STRETCH_USER = 1
    * IMG_STRETCH_LINEAR = 2
    * IMG_STRETCH_STDEV = 3
    * IMG_STRECTH_CUMULATIVE = 4

Options for vector geometry types:

    * GEOM_PT = 1
    * GEOM_LINE = 2
    * GEOM_POLY = 3
    * GEOM_MPT = 4
    * GEOM_MLINE = 5
    * GEOM_MPOLY = 6

"""
from __future__ import print_function

import time
import datetime
import sys
import pathlib

from osgeo import gdal

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

METHOD_SAMPLES = 0  # as calculated by ML
METHOD_AREA = 1  # priors set by the relative area
METHOD_EQUAL = 2  # priors all equal
METHOD_USERDEFINED = 3  # priors passed in to function
METHOD_WEIGHTED = 4  # priors by area but with a weight applied

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

INTERP_NEAREST_NEIGHBOUR = 0
INTERP_BILINEAR = 1
INTERP_CUBIC = 2
INTERP_CUBICSPLINE = 3
INTERP_LANCZOS = 4
INTERP_AVERAGE = 5
INTERP_MODE = 6

GEOM_PT = 1
GEOM_LINE = 2
GEOM_POLY = 3
GEOM_MPT = 4
GEOM_MLINE = 5
GEOM_MPOLY = 6

IMG_STRETCH_NONE = 0
IMG_STRETCH_USER = 1
IMG_STRETCH_LINEAR = 2
IMG_STRETCH_STDEV = 3
IMG_STRECTH_CUMULATIVE = 4


def get_install_base_path() -> pathlib.PurePath:
    """
    A function which returns the base path for the RSGISLib installation.
    This can be used to find files stored in share etc. installed alongside
    the RSGISLib source code.

    :return: a pathlib.PurePath object with the base path of the RSGISLib
             installation.

    """
    import site
    import platform

    site_path_str = site.getsitepackages()[0]
    site_path = pathlib.PurePath(site_path_str)

    base_path_comps = []
    for path_part in site_path.parts:
        if path_part != "lib":
            base_path_comps.append(path_part)
        else:
            break

    if platform.system() == "Windows":
        base_path = pathlib.PureWindowsPath(*base_path_comps)
    else:
        base_path = pathlib.PurePosixPath(*base_path_comps)

    return base_path


RSGISLIB_INSTALL_PREFIX = str(get_install_base_path())


def get_rsgislib_version() -> str:
    """
    Calls rsgis-config to get the version number.

    :return: string with the rsgislib version number

    """

    # Try calling rsgis-config to get minor version number
    try:
        import distutils.spawn

        if distutils.spawn.find_executable("rsgis-config") is not None:
            import subprocess

            out = subprocess.run(
                ["rsgis-config", "--version"],
                capture_output=True,
                text=True,
                check=True,
            )
            version_str = out.stdout
            version_str = version_str.split("\n")[0]
        else:
            version_str = "NA.NA"
    except Exception:
        version_str = "NA.NA"
    return version_str


__version__ = get_rsgislib_version()

py_sys_version_str = "{}.{}".format(sys.version_info.major, sys.version_info.minor)
py_sys_version_flt = float(py_sys_version_str)


class RSGISPyException(Exception):
    """
    A class representing the RSGISLib exception.
    """

    def __init__(self, value: str):
        """
        Init for the RSGISPyException class
        """
        self.value = value

    def __str__(self):
        """
        Return a string representation of the exception
        """
        return repr(self.value)


class RSGISGDALErrorHandler:
    """
    A class representing a generic GDAL Error Handler which
    can be used to pick up GDAL warnings rather than just
    failure errors.
    """

    def __init__(self):
        """
        Init for RSGISGDALErrorHandler. Class attributes are
        err_level, err_no and err_msg

        """
        self.err_level = gdal.CE_None
        self.err_no = 0
        self.err_msg = ""

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


def get_rsgislib_datatype(gdal_type: int) -> int:
    """
    Convert from GDAL data type string to RSGISLib data type int.

    :return: int

    """
    gdal_type = gdal_type.lower()
    if gdal_type == "int8":
        out_dt = TYPE_8INT
    elif gdal_type == "int16":
        out_dt = TYPE_16INT
    elif gdal_type == "int32":
        out_dt = TYPE_32INT
    elif gdal_type == "int64":
        out_dt = TYPE_64INT
    elif gdal_type == "byte" or gdal_type == "uint8":
        out_dt = TYPE_8UINT
    elif gdal_type == "uint16":
        out_dt = TYPE_16UINT
    elif gdal_type == "uint32":
        out_dt = TYPE_32UINT
    elif gdal_type == "uint64":
        out_dt = TYPE_64UINT
    elif gdal_type == "float32":
        out_dt = TYPE_32FLOAT
    elif gdal_type == "float64":
        out_dt = TYPE_64FLOAT
    else:
        raise RSGISPyException(
            f"The data type '{gdal_type}' is unknown / not supported."
        )
    return out_dt


def get_gdal_datatype(rsgislib_datatype: int) -> int:
    """
    Convert from RSGIS data type to GDAL data type int.

    :return: int

    """
    if rsgislib_datatype == TYPE_16INT:
        out_dt = gdal.GDT_Int16
    elif rsgislib_datatype == TYPE_32INT:
        out_dt = gdal.GDT_Int32
    elif rsgislib_datatype == TYPE_8UINT:
        out_dt = gdal.GDT_Byte
    elif rsgislib_datatype == TYPE_16UINT:
        out_dt = gdal.GDT_UInt16
    elif rsgislib_datatype == TYPE_32UINT:
        out_dt = gdal.GDT_UInt32
    elif rsgislib_datatype == TYPE_32FLOAT:
        out_dt = gdal.GDT_Float32
    elif rsgislib_datatype == TYPE_64FLOAT:
        out_dt = gdal.GDT_Float64
    else:
        raise RSGISPyException(
            f"The data type '{rsgislib_datatype}' is unknown / not supported."
        )
    return out_dt


def get_numpy_datatype(rsgislib_datatype: int) -> int:
    """
    Convert from RSGISLib data type to numpy datatype

    :param rsgis_datatype:
    :return: numpy datatype

    """
    import numpy

    if rsgislib_datatype == TYPE_8INT:
        numpy_dt = numpy.int8
    elif rsgislib_datatype == TYPE_16INT:
        numpy_dt = numpy.int16
    elif rsgislib_datatype == TYPE_32INT:
        numpy_dt = numpy.int32
    elif rsgislib_datatype == TYPE_64INT:
        numpy_dt = numpy.int64
    elif rsgislib_datatype == TYPE_8UINT:
        numpy_dt = numpy.uint8
    elif rsgislib_datatype == TYPE_16UINT:
        numpy_dt = numpy.uint16
    elif rsgislib_datatype == TYPE_32UINT:
        numpy_dt = numpy.uint32
    elif rsgislib_datatype == TYPE_64UINT:
        numpy_dt = numpy.uint64
    elif rsgislib_datatype == TYPE_32FLOAT:
        numpy_dt = numpy.float32
    elif rsgislib_datatype == TYPE_64FLOAT:
        numpy_dt = numpy.float64
    else:
        raise RSGISPyException("Datatype was not recognised.")
    return numpy_dt


def get_numpy_char_codes_datatype(rsgislib_datatype: int) -> str:
    """
    Convert from RSGISLib data type to numpy datatype

    :param rsgis_datatype:
    :return: numpy character code datatype

    """
    import numpy

    if rsgislib_datatype == TYPE_8INT:
        numpy_dt = numpy.dtype(numpy.int8).char
    elif rsgislib_datatype == TYPE_16INT:
        numpy_dt = numpy.dtype(numpy.int16).char
    elif rsgislib_datatype == TYPE_32INT:
        numpy_dt = numpy.dtype(numpy.int32).char
    elif rsgislib_datatype == TYPE_64INT:
        numpy_dt = numpy.dtype(numpy.int64).char
    elif rsgislib_datatype == TYPE_8UINT:
        numpy_dt = numpy.dtype(numpy.uint8).char
    elif rsgislib_datatype == TYPE_16UINT:
        numpy_dt = numpy.dtype(numpy.uint16).char
    elif rsgislib_datatype == TYPE_32UINT:
        numpy_dt = numpy.dtype(numpy.uint32).char
    elif rsgislib_datatype == TYPE_64UINT:
        numpy_dt = numpy.dtype(numpy.uint64).char
    elif rsgislib_datatype == TYPE_32FLOAT:
        numpy_dt = numpy.dtype(numpy.float32).char
    elif rsgislib_datatype == TYPE_64FLOAT:
        numpy_dt = numpy.dtype(numpy.float64).char
    else:
        raise RSGISPyException("Datatype was not recognised.")
    return numpy_dt


def is_notebook():
    """
    A function to find if running within a python notebook. If
    running within a jupyter notebook then can use a different
    progress bar.

    :return: boolean: True: within notebook

    """
    try:
        shell = get_ipython().__class__.__name__
        if shell == "ZMQInteractiveShell":
            return True  # Jupyter notebook or qtconsole
        elif shell == "TerminalInteractiveShell":
            return False  # Terminal running IPython
        else:
            return False  # Other type (?)
    except NameError:
        return False  # Probably standard Python interpreter


class RSGISTime:
    """
    Class to calculate run time for a function, format and print out.

    Need to call start before running function and end immediately after.

    .. code:: python

        t = RSGISTime()
        t.start()
        rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile,
                                    gdalformat, False, 0)
        t.end()

    Note, this is only designed to provide some general feedback, for
    benchmarking the timeit module is better suited.

    """

    def __init__(self):
        self.start_time = time.time()
        self.end_time = time.time()

    def start(self, print_start_time: bool = False):
        """
        Start timer, optionally printing start time

        :param print_start_time: A boolean specifying whether the start time
                                 should be printed to console.

        """
        self.start_time = time.time()
        if print_start_time:
            print(time.strftime("Start Time: %H:%M:%S, %a %b %m %Y."))

    def end(self, report_diff: bool = True, precede_str: str = "", post_str: str = ""):
        """
        End timer and optionally print difference.
        If precedeStr or postStr have a value then they will be used instead
        of the generic wording around the time.

        "{precede_str} {time} {post_str}"

        :param report_diff: A boolean specifying whether time difference should
                            be printed to console.
        :param precede_str: A string which is printed ahead of time difference
        :param post_str: A string which is printed after the time difference

        """
        self.end_time = time.time()
        if report_diff:
            self.calc_diff(precede_str, post_str)

    def calc_diff(self, precede_str: str = "", post_str: str = ""):
        """
        Calculate time difference, format and print.

        :param precede_str: A string which is printed ahead of time difference
        :param post_str: A string which is printed after the time difference

        """
        time_diff = self.end_time - self.start_time

        use_custom_mss = False
        if (len(precede_str) > 0) or (len(post_str) > 0):
            use_custom_mss = True

        if time_diff <= 1:
            if use_custom_mss:
                print(f"{precede_str} in less than a second {post_str}")
            else:
                print("Algorithm Completed in less than a second.")
        else:
            time_obj = datetime.datetime.utcfromtimestamp(time_diff)
            time_diff_str = time_obj.strftime("%H:%M:%S")
            if use_custom_mss:
                print(f"{precede_str} {time_diff_str} {post_str}")
            else:
                print(f"Algorithm Completed in {time_diff_str}.")


class TQDMProgressBar:
    """
    Uses TQDM TermProgress to print a progress bar to the terminal
    """

    def __init__(self):
        self.lprogress = 0

    def setTotalSteps(self, steps: int):
        import tqdm

        if is_notebook():
            import tqdm.notebook

            self.pbar = tqdm.notebook.tqdm(total=steps)
        else:
            self.pbar = tqdm.tqdm(total=steps)
        self.lprogress = 0

    def setProgress(self, progress: int):
        step = progress - self.lprogress
        self.pbar.update(step)
        self.lprogress = progress

    def reset(self):
        self.pbar.close()
        import tqdm

        if is_notebook():
            import tqdm.notebook

            self.pbar = tqdm.notebook.tqdm(total=100)
        else:
            self.pbar = tqdm.tqdm(total=100)
        self.lprogress = 0

    def setLabelText(self, text: str):
        sys.stdout.write("\n%s\n" % text)

    def wasCancelled(self):
        return False

    def displayException(self, trace):
        sys.stdout.write(trace)

    def displayWarning(self, text: str):
        sys.stdout.write("Warning: %s\n" % text)

    def displayError(self, text: str):
        sys.stdout.write("Error: %s\n" % text)

    def displayInfo(self, text: str):
        sys.stdout.write("Info: %s\n" % text)
