#!/usr/bin/env python
"""
The elevation module contains functions for performing analysis on elevation data.
"""

# import the C++ extension into this level
from ._elevation import *

def fftDEMFusion(highResDEMImg, lowResDEMImg, fusedDEMImg, cSize=20):
    """
This function merges DEMs (any single band file) using a fourier 
transformation where the low frequency component is taken from the 
lower resolution DEM and the high frequency component is taken from
the higher resolution DEM. 

This is an implemenetation ideas from:

Karkee, M., Steward, B. L., & Aziz, S. A. (2008). Improving quality 
of public domain digital elevation models through data fusion. 
Biosystems Engineering, 101(3), 293–305.

:param highResDEMImg: is an input image file for the high resolution DEM file.
:param lowResDEMImg: is an input image file for the low resolution DEM file.
:param fusedDEMImg: is an output image file for the resulting fused DEM.
:param cSize: is the threshold (int) defining the high and low frequencies.

"""
    import numpy
    from osgeo import gdal
    import rsgislib
    import rsgislib.imageutils

    try:
        srcDSHighRes = gdal.Open( highResDEMImg )
        srcbandHighRes = srcDSHighRes.GetRasterBand(1)
        srcbandHighResArr = srcbandHighRes.ReadAsArray()
    except Exception as e:
        print('Unable to open \"'+ highResDEMImg + '\"')
        raise e
        
    try:
        srcDSLowRes = gdal.Open( lowResDEMImg )
        srcbandLowRes = srcDSLowRes.GetRasterBand(1)
        srcbandLowResArr = srcbandLowRes.ReadAsArray()
    except Exception as e:
        print('Unable to open \"'+ lowResDEMImg + '\"')
        raise e
    
    fftHighRes = numpy.fft.fft2(srcbandHighResArr)
    fshiftHighRes = numpy.fft.fftshift(fftHighRes)
    magSpectHighRes = 20*numpy.log(numpy.abs(fshiftHighRes))
    
    fftLowRes = numpy.fft.fft2(srcbandLowResArr)
    fshiftLowRes = numpy.fft.fftshift(fftLowRes)
    magSpectLowRes = 20*numpy.log(numpy.abs(fshiftLowRes))

    rows, cols = srcbandLowResArr.shape
    crow,ccol = int(rows/2) , int(cols/2)
    fshiftLowRes[crow-cSize:crow+cSize, ccol-cSize:ccol+cSize] = fshiftHighRes[crow-cSize:crow+cSize, ccol-cSize:ccol+cSize]
    f_ishift = numpy.fft.ifftshift(fshiftLowRes)
    img_back = numpy.fft.ifft2(f_ishift)
    img_back = numpy.abs(img_back)
        
    rsgislib.imageutils.createCopyImage(highResDEMImg, fusedDEMImg, 1, -9999, 'KEA', rsgislib.TYPE_32FLOAT)
    
    try:
        srcDSOutput = gdal.Open( fusedDEMImg, gdal.GA_Update )
        srcbandOutput = srcDSOutput.GetRasterBand(1)
        srcbandOutput.WriteArray(img_back)
    except Exception as e:
        print('Unable to open \"'+ fusedDEMImg + '\"')
        raise e
    print('Completed Fusion')



