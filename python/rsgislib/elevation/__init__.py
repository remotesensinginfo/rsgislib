#!/usr/bin/env python
"""
The elevation module contains functions for performing analysis on elevation data.
"""

# import the C++ extension into this level
from ._elevation import *
import rsgislib


def fft_dem_fusion(
    high_res_dem_img: str, low_res_dem_img: str, fused_dem_img: str, c_size: int = 20, gdalformat: str = "KEA",
):
    """
    This function merges DEMs (any single band file) using a fourier
    transformation where the low frequency component is taken from the
    lower resolution DEM and the high frequency component is taken from
    the higher resolution DEM.

    This is an implementation ideas from:

    Karkee, M., Steward, B. L., & Aziz, S. A. (2008). Improving quality
    of public domain digital elevation models through data fusion.
    Biosystems Engineering, 101(3), 293–305.

    :param high_res_DEM_img: is an input image file for the high resolution DEM file.
    :param low_res_DEM_img: is an input image file for the low resolution DEM file.
    :param fused_DEM_img: is an output image file for the resulting fused DEM.
    :param c_size: is the threshold (int) defining the high and low frequencies.

    .. code:: python

        import rsgislib
        from rsgislib import elevation

        high_res_DEM_img = "N09E009_TDX_12m_EGM08.tif"
        low_res_DEM_img = "N09E009_SRTM_30m_EGM08.tif"
        fused_DEM_img = "NO9E009_SRTM_TDX_EGM08_fusion.tif"
        c_size = 10

        elevation.fft_dem_fusion(highResDEMImg, lowResDEMImg, fusedDEMImg, cSize=20)

    """
    # TODO Add checks that images are covering the same geographic area etc.
    #  Same number of pixels as well?
    import numpy
    from osgeo import gdal

    import rsgislib
    import rsgislib.imageutils

    try:
        srcDSHighRes = gdal.Open(high_res_dem_img)
        srcbandHighRes = srcDSHighRes.GetRasterBand(1)
        srcbandHighResArr = srcbandHighRes.ReadAsArray()
    except Exception as e:
        print('Unable to open "' + high_res_dem_img + '"')
        raise e

    try:
        srcDSLowRes = gdal.Open(low_res_dem_img)
        srcbandLowRes = srcDSLowRes.GetRasterBand(1)
        srcbandLowResArr = srcbandLowRes.ReadAsArray()
    except Exception as e:
        print('Unable to open "' + low_res_dem_img + '"')
        raise e

    fftHighRes = numpy.fft.fft2(srcbandHighResArr)
    fshiftHighRes = numpy.fft.fftshift(fftHighRes)
    # magSpectHighRes = 20 * numpy.log(numpy.abs(fshiftHighRes))

    fftLowRes = numpy.fft.fft2(srcbandLowResArr)
    fshiftLowRes = numpy.fft.fftshift(fftLowRes)
    # magSpectLowRes = 20 * numpy.log(numpy.abs(fshiftLowRes))

    rows, cols = srcbandLowResArr.shape
    crow, ccol = int(rows / 2), int(cols / 2)
    fshiftLowRes[crow - c_size : crow + c_size, ccol - c_size : ccol + c_size] = (
        fshiftHighRes[crow - c_size : crow + c_size, ccol - c_size : ccol + c_size]
    )
    f_ishift = numpy.fft.ifftshift(fshiftLowRes)
    img_back = numpy.fft.ifft2(f_ishift)
    img_back = numpy.abs(img_back)

    rsgislib.imageutils.create_copy_img(
        high_res_dem_img, fused_dem_img, 1, -9999, gdalformat, rsgislib.TYPE_32FLOAT
    )

    try:
        srcDSOutput = gdal.Open(fused_dem_img, gdal.GA_Update)
        srcbandOutput = srcDSOutput.GetRasterBand(1)
        srcbandOutput.WriteArray(img_back)
    except Exception as e:
        print('Unable to open "' + fused_dem_img + '"')
        raise e
    print("Completed Fusion")


def resampling_detread_dem(
    input_img: str,
    output_img: str,
    resmp_res: int,
    gdalformat: str,
    no_data_val: float = None,
    datatype: int = None,
    tmp_dir: str = None,
    low_resmp_mthd: int = rsgislib.INTERP_AVERAGE,
    up_smpl_mthd: int = rsgislib.INTERP_CUBICSPLINE,
):
    """
    A function which using image resampling to down sample the input DEM (image)
    to the specified resolution and then up samples it back to the original resolution.

    :param input_img: Input image - note only band 1 is used
    :param output_img: Output image path
    :param resmp_res: The intermediate down sample resolution.
    :param gdalformat: output image format (e.g., KEA, GTIFF).
    :param no_data_val: the DEM no data value. If None then read from
                        input image header.
    :param datatype: The output image data type. If None then the same as the
                     input image.
    :param tmp_dir: A temp directory for intermediate outputs. If None then one is
                    created.
    :param low_resmp_mthd: The method used to down sample the input image.
                           Default: rsgislib.INTERP_AVERAGE
    :param up_smpl_mthd: The method used to up sample the down sampled image.
                         Default: rsgislib.INTERP_CUBICSPLINE

    """
    import os
    import math
    import rsgislib.imageutils
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    uid_str = rsgislib.tools.utils.uid_generator()

    if tmp_dir is None:
        tmp_dir = f"tmp_{uid_str}"

    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)

    if datatype is None:
        datatype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)

    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    if no_data_val is None:
        raise rsgislib.RSGISPyException(
            "A no data value has not been defined. Either set in image header "
            "or pass to function."
        )

    int_base_name = rsgislib.tools.filetools.get_file_basename(input_img)

    in_img_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    coord_width = in_img_bbox[1] - in_img_bbox[0]
    coord_height = in_img_bbox[3] - in_img_bbox[2]

    img_width = int(math.ceil(coord_width / resmp_res) + 1)
    img_height = int(math.ceil(coord_height / resmp_res) + 1)

    wkt_str = rsgislib.imageutils.get_wkt_proj_from_img(input_img)

    base_img = os.path.join(tmp_dir, f"{int_base_name}_base_{uid_str}.kea")
    rsgislib.imageutils.create_blank_img(
        base_img,
        n_bands=1,
        width=img_width,
        height=img_height,
        tl_x=in_img_bbox[0],
        tl_y=in_img_bbox[3],
        res_x=resmp_res,
        res_y=(resmp_res * (-1)),
        pxl_val=0,
        wkt_file="",
        wkt_str=wkt_str,
        gdalformat="KEA",
        datatype=datatype,
    )

    low_res_img = os.path.join(tmp_dir, f"{int_base_name}_low_res_{uid_str}.kea")
    rsgislib.imageutils.resample_img_to_match(
        base_img,
        input_img,
        low_res_img,
        gdalformat="KEA",
        interp_method=low_resmp_mthd,
        datatype=datatype,
        no_data_val=no_data_val,
        multicore=False,
    )

    rsgislib.imageutils.resample_img_to_match(
        input_img,
        low_res_img,
        output_img,
        gdalformat=gdalformat,
        interp_method=up_smpl_mthd,
        datatype=datatype,
        no_data_val=no_data_val,
        multicore=False,
    )
