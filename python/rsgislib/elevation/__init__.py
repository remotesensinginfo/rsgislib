#!/usr/bin/env python
"""
The elevation module contains functions for performing analysis on elevation data.
"""

# import the C++ extension into this level
from ._elevation import *


def fft_dem_fusion(highResDEMImg, lowResDEMImg, fusedDEMImg, cSize=20):
    """
    This function merges DEMs (any single band file) using a fourier
    transformation where the low frequency component is taken from the
    lower resolution DEM and the high frequency component is taken from
    the higher resolution DEM.

    This is an implemenetation ideas from:

    Karkee, M., Steward, B. L., & Aziz, S. A. (2008). Improving quality
    of public domain digital elevation models through data fusion.
    Biosystems Engineering, 101(3), 293–305.

    :param high_res_DEM_img: is an input image file for the high resolution DEM file.
    :param low_res_DEM_img: is an input image file for the low resolution DEM file.
    :param fused_DEM_img: is an output image file for the resulting fused DEM.
    :param c_size: is the threshold (int) defining the high and low frequencies.

    Example::

    import rsgislib
    from rsgislib import elevation

    high_res_DEM_img = "N09E009_TDX_12m_EGM08.tif"
    low_res_DEM_img = "N09E009_SRTM_30m_EGM08.tif"
    fused_DEM_img = "NO9E009_SRTM_TDX_EGM08_fusion.tif"
    c_size = 10

    elevation.fft_dem_fusion(highResDEMImg, lowResDEMImg, fusedDEMImg, cSize=20)
    """
    # TODO Add checks that images are covering the same geographic area etc. Same number of pixels as well?
    import numpy
    from osgeo import gdal
    import rsgislib
    import rsgislib.imageutils

    try:
        srcDSHighRes = gdal.Open(highResDEMImg)
        srcbandHighRes = srcDSHighRes.GetRasterBand(1)
        srcbandHighResArr = srcbandHighRes.ReadAsArray()
    except Exception as e:
        print('Unable to open "' + highResDEMImg + '"')
        raise e

    try:
        srcDSLowRes = gdal.Open(lowResDEMImg)
        srcbandLowRes = srcDSLowRes.GetRasterBand(1)
        srcbandLowResArr = srcbandLowRes.ReadAsArray()
    except Exception as e:
        print('Unable to open "' + lowResDEMImg + '"')
        raise e

    fftHighRes = numpy.fft.fft2(srcbandHighResArr)
    fshiftHighRes = numpy.fft.fftshift(fftHighRes)
    magSpectHighRes = 20 * numpy.log(numpy.abs(fshiftHighRes))

    fftLowRes = numpy.fft.fft2(srcbandLowResArr)
    fshiftLowRes = numpy.fft.fftshift(fftLowRes)
    magSpectLowRes = 20 * numpy.log(numpy.abs(fshiftLowRes))

    rows, cols = srcbandLowResArr.shape
    crow, ccol = int(rows / 2), int(cols / 2)
    fshiftLowRes[
        crow - cSize : crow + cSize, ccol - cSize : ccol + cSize
    ] = fshiftHighRes[crow - cSize : crow + cSize, ccol - cSize : ccol + cSize]
    f_ishift = numpy.fft.ifftshift(fshiftLowRes)
    img_back = numpy.fft.ifft2(f_ishift)
    img_back = numpy.abs(img_back)

    rsgislib.imageutils.create_copy_img(
        highResDEMImg, fusedDEMImg, 1, -9999, "KEA", rsgislib.TYPE_32FLOAT
    )

    try:
        srcDSOutput = gdal.Open(fusedDEMImg, gdal.GA_Update)
        srcbandOutput = srcDSOutput.GetRasterBand(1)
        srcbandOutput.WriteArray(img_back)
    except Exception as e:
        print('Unable to open "' + fusedDEMImg + '"')
        raise e
    print("Completed Fusion")
