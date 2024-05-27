#! /usr/bin/env python
############################################################################
#  calcindices.py
#
#  Copyright 2017 RSGISLib.
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
# Purpose:  Provides helper functions for calculating commmon image band
#           indices.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 03/10/2017
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import rsgislib
import rsgislib.imagecalc
import rsgislib.imageutils


def calc_ndvi(
    input_img: str,
    img_red_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalised difference Vegetation Index (NDVI)
    ((NIR-RED)/(NIR+RED)). Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    .. code:: python

        import rsgislib
        from rsgislib import imagecalc

        input_image = "L1C_T45RYH_A019936_20201230T044924.tif"
        red_band = 4
        nir_band = 5
        out_image = "L1C_T45RYH_A019936_20201230T044924_NIR.kea:
        calc_stats = True
        gdalformat = "KEA"

        calc_ndvi(input_image, red_band, nir_band, out_image, calc_stats, gdalformat)

    """
    expression = "(nir+red)!=0?(nir-red)/(nir+red):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_wbi(
    input_img: str,
    img_blue_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Water Band Index (WBI) (Blue/NIR.
    Note, the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                          (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats
                       should be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "nir!=0?blue/nir:-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )
    # Set no data value

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ndwi(
    input_img: str,
    img_nir_band: int,
    img_swir1_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalised Difference Water Index (NDWI)
     ((NIR-SWIR)/(NIR+SWIR)), note the output no data value is -999.

    See: Xu, H. (2006). Modification of normalised difference water index (NDWI)
    to enhance open water features in remotely sensed imagery. International Journal
    of Remote Sensing, 27(14), 3025–3033. http://doi.org/10.1080/01431160600589179

    :param input_img: is a string specifying the input image file.
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param img_swir1_band: is an int specifying the swir band (e.g., Landsat TM Band 5)
                           in the input image (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(nir+swir)!=0?(nir-swir)/(nir+swir):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir1_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_gndwi(
    input_img: str,
    img_green_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Green-Normalised Water Different Index (NDWI)
    ((GREEN-NIR)/(GREEN+NIR)), note the output no data value is -999.

    See: Xu, H. (2006). Modification of normalised difference water index (NDWI) to
    enhance open water features in remotely sensed imagery. International Journal of
    Remote Sensing, 27(14), 3025–3033. http://doi.org/10.1080/01431160600589179

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the green band in the input image
                          (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(green+nir)!=0?(green-nir)/(green+nir):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_gmndwi(
    input_img: str,
    img_green_band: int,
    img_swir1_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Green/SWIR-Normalised Difference Water Index (NDWI)
    ((GREEN-SWIR)/(GREEN+SWIR)). Note, the output no data value is -999.

    See: Xu, H. (2006). Modification of normalised difference water index (NDWI) to
    enhance open water features in remotely sensed imagery. International Journal
    of Remote Sensing, 27(14), 3025–3033. http://doi.org/10.1080/01431160600589179

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the green band in the input image
                           (band indexing starts at 1)
    :param img_swir1_band: is an int specifying the swir band (e.g., Landsat TM Band 5)
                           in the input image (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(green+swir)!=0?(green-swir)/(green+swir):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir1_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_whiteness(
    input_img: str,
    img_blue_band: int,
    img_green_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate whiteness, note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                          (band indexing starts at 1)
    :param img_green_band: is an int specifying the green band in the input image
                           (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(blue+green+red)!=0?(abs(blue-((blue+green+red)/3)) + abs(green-((blue+green+red)/3)) + abs(red-((blue+green+red)/3)))/((blue+green+red)/3):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_brightness(
    input_img: str,
    img_blue_band: int,
    img_green_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    scale_factor: int = 1000,
):
    """
    Helper function to calculate visible brightness, note the output no data
    value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                          (band indexing starts at 1)
    :param img_green_band: is an int specifying the green band in the input image
                           (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats
                       should be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param scale_factor: is a float which can be used retrieved reflectance between
                         0-1 (Default: 1000 to match rsgislib/arcsi)

    """
    expression = "(blue+green+red)!=0?((blue/{})+(green/{})+(red/{}))/3:-999".format(
        scale_factor, scale_factor, scale_factor
    )
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_brightness_scaled(
    input_img: str,
    img_blue_band: int,
    img_green_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    scale_factor: int = 1000,
):
    """
    Helper function to calculate visible brightness, note the output no data
    value is -999. The difference between this function calc_brightness is
    that the output image is rescaled so the maximum value is 1.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                          (band indexing starts at 1)
    :param img_green_band: is an int specifying the green band in the input image
                           (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats
                       should be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param scale_factor: is a float which can be used retrieved reflectance between
                         0-1 (Default: 1000 to match rsgislib/arcsi)

    """
    import rsgislib.tools.filetools
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator()
    basename = rsgislib.tools.filetools.get_file_basename(output_img)
    file_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    tmp_img = "{}_tmp_{}.{}".format(basename, uid_str, file_ext)
    expression = "(blue+green+red)!=0?((blue/{})+(green/{})+(red/{}))/3:-999".format(
        scale_factor, scale_factor, scale_factor
    )
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    rsgislib.imagecalc.band_math(
        tmp_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )
    rsgislib.imageutils.normalise_img_pxl_vals(
        input_img=tmp_img,
        output_img=output_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        in_no_data_val=-999,
        out_no_data_val=-999,
        out_min=0,
        out_max=1,
        stretch_type=rsgislib.imageutils.STRETCH_LINEARSTDDEV,
        stretch_param=2,
    )
    rsgislib.tools.filetools.delete_file_with_basename(tmp_img)

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ctvi(
    input_img: str,
    img_red_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Corrected Transformed Vegetation Index
    ((NDVI + 0.5)/sqrt(abs(NDVI + 0.5))), note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_red_band: is an int specifying the blue band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = (
        "(nir+red)!=0?(((nir-red)/(nir+red))+0.5)/"
        "sqrt(abs(((nir-red)/(nir+red))+0.5)):-999"
    )
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ndsi(
    input_img: str,
    img_green_band: int,
    img_swir1_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalised Difference Snow Index (NDSI)
    ((Green-SWIR)/(Green+SWIR)), note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the green band in the input image
                           (band indexing starts at 1)
    :param img_swir1_band: is an int specifying the swir band (e.g., Landsat TM Band 5)
                           in the input image (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats
                       should be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(green+swir)!=0?(green-swir)/(green+swir):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir1_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_nbr(
    input_img: str,
    img_nir_band: int,
    img_swir2_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalised Burn Ratio (NBR)
    ((NIR-SWIR#2)/(NIR+SWIR#2)). Note, the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param img_swir2_band: is an int specifying the swir #2 band (e.g., Landsat
                           TM Band 7) in the input image (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats
                       should be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(nir+swir)!=0?(nir-swir)/(nir+swir):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir2_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_bai(
    input_img: str,
    img_nir_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    scale_factor: int = 1000,
):
    """
    Helper function to calculate Burn Area Index (BAI)
    1/((0.1 - red)*(0.1 - red) + (0.06 - nir)*(0.06 - nir)).
    Note, the output no data value is -999.

    Burn Area Index (BAI): Chuvieco, E.; Martín, M.P.; Palacios, A. Assessment of
    different spectral indices in the red-near-infrared spectral domain for burned
    land discrimination. Int. J. Remote Sens. 2002, 23, 5103–5110.

    :param input_img: is a string specifying the input image file.
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param scale_factor: is a float which can be used retrieved reflectance
                         between 0-1 (Default: 1000 to match rsgislib/arcsi)

    """
    expression = (
        "(nir+red)!=0?(1/((0.1 - (red/{}))*(0.1 - (red/{})) + "
        "(0.06 - (nir/{}))*(0.06 - (nir/{})))):-999".format(
            scale_factor, scale_factor, scale_factor, scale_factor
        )
    )
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_mvi(
    input_img: str,
    img_green_band: int,
    img_nir_band: int,
    img_swir1_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Mangrove Vegetation Index (MFI)
    ((NIR-Green)/(SWIR-Green)). Note, the output no data value is -999.

    Baloloya, A.B, Blancoab, A.C, Ana, R.R.C.S,  Nadaokac, K (2020). Development
    and application of a new mangrove vegetation index (MVI) for rapid and accurate
    mangrove mapping. ISPRS Journal of Photogrammetry and Remote Sensing. 166.
    pp95-177. https://doi.org/10.1016/j.isprsjprs.2020.06.001

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the red band in the input image
                           (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param img_swir1_band: is an int specifying the swir band in the input image
                           (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "swir-green!=0?nir-green/swir-green:-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir1_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_evi(
    input_img: str,
    img_blue_band: int,
    img_red_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    g: float = 2.5,
    c1: float = 6.0,
    c2: float = 7.5,
    l: float = 1,
    scale_factor: int = 1000,
):
    """
    Helper function to calculate Enhanced Vegetation Index (EVI)
    G * ((NIR - Red) / (NIR + C1 * Red – C2 * BLue + L)).
    Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param g: g coefficient for EVI (Default: 2.5)
    :param c1: c1 coefficient for EVI (Default: 6.0)
    :param c2: c2 coefficient for EVI (Default: 7.5)
    :param l: l coefficient for EVI (Default: 1)
    :param scale_factor: is a float which can be used retrieved reflectance between
                         0-1 (Default: 1000 to match rsgislib/arcsi)

    """
    expression = f"( (nir/{scale_factor}) + ({c1} * (red/{scale_factor})) - " \
                 f"({c2} * (blue/{scale_factor})) + {l} ) !=0? " \
                 f"{g}*( ( (nir/{scale_factor})-(red/{scale_factor}) ) / " \
                 f"( (nir/{scale_factor}) + ({c1} * (red/{scale_factor})) - " \
                 f"({c2} * (blue/{scale_factor})) + {l} ) ):-999"

    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_evi2(
    input_img: str,
    img_red_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    g: float = 2.5,
    c: float = 2.4,
    l: float = 1,
    scale_factor: int = 1000,
):
    """
    Helper function to calculate Enhanced Vegetation Index 2 (EVI2)
    G * (nir - red)/(nir + 2.4 * red + 1).
    Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param g: g coefficient for EVI2 (Default: 2.5)
    :param c: c coefficient for EVI2 (Default: 2.4)
    :param l: l coefficient for EVI2 (Default: 1)
    :param scale_factor: is a float which can be used retrieved reflectance between
                         0-1 (Default: 1000 to match rsgislib/arcsi)

    """
    expression = f"((nir/{scale_factor})+({c}*(red/{scale_factor}))+{l})!=0?" \
                 f"{g}*( (nir/{scale_factor})-(red/{scale_factor})) / " \
                 f"((nir/{scale_factor})+ ({c}*(red/{scale_factor}))+{l}):-999"

    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_gndvi(
    input_img: str,
    img_green_band: int,
    img_nir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Green Normalised difference Vegetation Index (GNDVI)
    ((NIR-RED)/(NIR+RED)). Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the green band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(nir+green)!=0?(nir-green)/(nir+green):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_remi(
    input_img: str,
    img_green_band: int,
    img_red_band: int,
    img_re_band: int,
    img_swir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Red-Edge Mangrove Index (REMI)
    ((REDEDGE-RED)/(SWIR1+GREEN)). Note the output no data value is -999.
    Red-edge band is typically the 6th (5 ignoring coastal Band) Sentinel-2
    band (740 nm).

    Chen, Z., Zhang, M., Zhang, H., Liu, Y., 2023. Mapping mangrove using a
    red-edge mangrove index (REMI) based on Sentinel-2 multispectral images.
    IEEE Trans. Geosci. Remote Sens. PP, 1–1.
    https://doi.org/10.1109/tgrs.2023.3323741

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the green band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param img_re_band: is an int specifying the red-edge band in the input image
                         (band indexing starts at 1)
    :param img_swir_band: is an int specifying the swir1 band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(swir+green)!=0?(re-red)/(swir+green):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("re", input_img, img_re_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_bsi(
    input_img: str,
    img_blue_band: int,
    img_red_band: int,
    img_nir_band: int,
    img_swir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Bare Soil Index (BSI)
    ((SWIR1+RED) - (NIR + BLUE))  / ((SWIR1+RED) + (NIR + BLUE)).
    Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param img_swir_band: is an int specifying the swir1 band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = (
        "(swir+red)+(nir+blue)!=0?(swir+red)-(nir+blue)/(swir+red)+(nir+blue):-999"
    )
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_si(
    input_img: str,
    img_blue_band: int,
    img_green_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    scale_factor: int = 1000,
):
    """
    Helper function to calculate Shadow Index (SI)
    ((1-BLUE) * (1-GREEN) * (1-BLUE))^(1/3).
    Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                         (band indexing starts at 1)
    :param img_green_band: is an int specifying the green band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param scale_factor: is a float which can be used retrieved reflectance between
                         0-1 (Default: 1000 to match rsgislib/arcsi)

    """
    expression = f"(red==0)||(green==0)||(blue==0)?-999:" \
                 f"( (1-(blue/{scale_factor})) * (1-(green/{scale_factor})) " \
                 f"* (1-(red/{scale_factor})))^(1/3)"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ndgi(
    input_img: str,
    img_green_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalized Difference Glacier Index (NDGI)
    (GREEN - RED)/(GREEN + RED). Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_green_band: is an int specifying the green band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(green+red)!=0?(green-red)/(green+red):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ndmi(
    input_img: str,
    img_nir_band: int,
    img_swir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalized Difference Moisture Index (NDMI)
    (NIR - SWIR1)/(NIR + SWIR1). Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_nir_band: is an int specifying the NIR band in the input image
                         (band indexing starts at 1)
    :param img_swir_band: is an int specifying the SWIR1 band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(nir+swir)!=0?(nir-swir)/(nir+swir):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_npcri(
    input_img: str,
    img_red_band: int,
    img_blue_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Normalized Pigment Chlorophyll Ratio Index (NPCRI)
    (NIR - SWIR1)/(NIR + SWIR1). Note the output no data value is -999.

    :param input_img: is a string specifying the input image file.
    :param img_red_band: is an int specifying the NIR band in the input image
                         (band indexing starts at 1)
    :param img_blue_band: is an int specifying the SWIR1 band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(red+blue)!=0?(red-blue)/(red+blue):-999"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ndbsi(
    input_img: str,
    img_blue_band: int,
    img_green_band: int,
    img_red_band: int,
    img_nir_band: int,
    img_swir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
    scale_factor: int = 1000,
    tmp_dir: str = None,
):
    """
    Helper function to calculate Normalized Difference Bare Soil Index (NDBSI).
    Note the output no data value is -999.

    Liu, Y., Meng, Q., Zhang, L., Wu, C., 2022. NDBSI: A normalized difference bare
    soil index for remote sensing to improve bare soil mapping accuracy in urban and
    rural areas. CATENA 214, 106265. https://doi.org/10.1016/j.catena.2022.106265

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                         (band indexing starts at 1)
    :param img_green_band: is an int specifying the green band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the ref band in the input image
                         (band indexing starts at 1)
    :param img_nir_band: is an int specifying the NIR band in the input image
                         (band indexing starts at 1)
    :param img_swir_band: is an int specifying the SWIR1 band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)
    :param scale_factor: is a float which can be used retrieved reflectance between
                         0-1 (Default: 1000 to match rsgislib/arcsi)
    :param tmp_dir: directory where intermediate files will be outputted. If input
                    is None then a directory will be created in the output image
                    folder and then deleted at the end of the processing.

    """
    import os
    import shutil
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    uid_str = rsgislib.tools.utils.uid_generator()

    tmp_dir_created = False
    if tmp_dir is None:
        tmp_dir = os.path.dirname(os.path.abspath(output_img))
        tmp_dir = os.path.join(tmp_dir, f"tmp_{uid_str}")
        if not os.path.exists(tmp_dir):
            os.mkdir(tmp_dir)
            tmp_dir_created = True

    basename = rsgislib.tools.filetools.get_file_basename(output_img)
    file_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    tmp_v_img = os.path.join(tmp_dir, f"{basename}_tmp_{uid_str}_v.{file_ext}")
    tmp_r_img = os.path.join(tmp_dir, f"{basename}_tmp_{uid_str}_r.{file_ext}")
    tmp_k_img = os.path.join(tmp_dir, f"{basename}_tmp_{uid_str}_k.{file_ext}")

    # Calc v
    expression = (
        f"(red==0)||(green==0)?-999:((red/{scale_factor})-(green/{scale_factor}))"
    )
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("green", input_img, img_green_band))
    rsgislib.imagecalc.band_math(
        tmp_v_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    # Calc r
    expression = f"(red==0)||(nir==0)||(swir==0)?-999:" \
                 f"1-( ( (swir/{scale_factor}) - (nir/{scale_factor}) ) / " \
                 f"( 3 * abs( (nir/{scale_factor}) - (red/{scale_factor}) ) ) )"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir_band))
    rsgislib.imagecalc.band_math(
        tmp_r_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    # Calc k
    expression = f"(r==-999)||(v==-999)?-999:v*r"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("r", tmp_r_img, 1))
    band_defns.append(rsgislib.imagecalc.BandDefn("v", tmp_v_img, 1))
    rsgislib.imagecalc.band_math(
        tmp_k_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    expression = f"k==-999?-999:k<0?abs((swir/{scale_factor})-(blue/{scale_factor})) " \
                 f"/ ((swir/{scale_factor})+(blue/{scale_factor}))*(-1):" \
                 f"((swir/{scale_factor})-(blue/{scale_factor})) / " \
                 f"((swir/{scale_factor})+(blue/{scale_factor}))"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("k", tmp_k_img, 1))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)

    if tmp_dir_created:
        shutil.rmtree(tmp_dir)
    else:
        rsgislib.imageutils.delete_gdal_layer(tmp_v_img)
        rsgislib.imageutils.delete_gdal_layer(tmp_r_img)
        rsgislib.imageutils.delete_gdal_layer(tmp_k_img)


def calc_cmr(
    input_img: str,
    img_swir1_band: int,
    img_swir2_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Clay Minerals Ratio (CMR)
    (swir1/swir2)). Note the output no data value is -999.

    Dogan, H., 2009. "Mineral composite assessment of Kelkit River Basin in
    Turkey by means of remote sensing," Journal of Earth System Science,
    Vol. 118, 701-710.

    :param input_img: is a string specifying the input image file.
    :param img_swir1_band: is an int specifying the swir1 band in the input image
                         (band indexing starts at 1)
    :param img_swir2_band: is an int specifying the swir2 band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(swir1==0)||(swir2==0)?-999:(swir1/swir2)"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("swir1", input_img, img_swir1_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir2", input_img, img_swir2_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_fmr(
    input_img: str,
    img_nir_band: int,
    img_swir_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Ferrous Minerals Ratio (FMR)
    (swir/nir)). Note the output no data value is -999.

    Segal, D. "Theoretical Basis for Differentiation of Ferric-Iron Bearing Minerals,
    Using Landsat MSS Data." Proceedings of Symposium for Remote Sensing of Environment,
    2nd Thematic Conference on Remote Sensing for Exploratory Geology, Fort Worth, TX
    (1982): pp. 949-951.

    :param input_img: is a string specifying the input image file.
    :param img_nir_band: is an int specifying the nir band in the input image
                         (band indexing starts at 1)
    :param img_swir_band: is an int specifying the swir band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(nir==0)||(swir==0)?-999:(swir/nir)"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("nir", input_img, img_nir_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("swir", input_img, img_swir_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


def calc_ior(
    input_img: str,
    img_blue_band: int,
    img_red_band: int,
    output_img: str,
    calc_stats: bool = True,
    gdalformat: str = "KEA",
):
    """
    Helper function to calculate Iron Oxide Ratio (IOR)
    (red/blue)). Note the output no data value is -999.

    Segal, D. "Theoretical Basis for Differentiation of Ferric-Iron Bearing Minerals,
    Using Landsat MSS Data." Proceedings of Symposium for Remote Sensing of Environment,
    2nd Thematic Conference on Remote Sensing for Exploratory Geology, Fort Worth, TX
    (1982): pp. 949-951.

    :param input_img: is a string specifying the input image file.
    :param img_blue_band: is an int specifying the blue band in the input image
                         (band indexing starts at 1)
    :param img_red_band: is an int specifying the red band in the input image
                         (band indexing starts at 1)
    :param output_img: is a string specifying the output image file.
    :param calc_stats: is a boolean specifying whether pyramids and stats should
                       be calculated (Default: True)
    :param gdalformat: is a string specifying the output image file format
                       (Default: KEA)

    """
    expression = "(red==0)||(blue==0)?-999:(red/blue)"
    band_defns = []
    band_defns.append(rsgislib.imagecalc.BandDefn("blue", input_img, img_blue_band))
    band_defns.append(rsgislib.imagecalc.BandDefn("red", input_img, img_red_band))
    rsgislib.imagecalc.band_math(
        output_img, expression, gdalformat, rsgislib.TYPE_32FLOAT, band_defns
    )

    if calc_stats:
        rsgislib.imageutils.pop_img_stats(output_img, True, -999.0, True)
    else:
        rsgislib.imageutils.set_img_no_data_value(output_img, -999.0)


# red-edge mangrove index (REMI) (red edge-red)/(SWIR1-green)

"""
http://bleutner.github.io/RStoolbox/rstbx-docu/spectralIndices.html
Index	Description	Source	Bands	Formula

GEMI	Global Environmental Monitoring Index	Pinty1992	red, nir	(((nir^2 - red^2) * 2 + (nir * 1.5) + (red * 0.5))/(nir + red + 0.5)) * (1 - ((((nir^2 - red^2) * 2 + (nir * 1.5) + (red * 0.5))/(nir + red + 0.5)) * 0.25)) - ((red - 0.125)/(1 - red))
MNDWI	Modified Normalised Difference Water Index	Xu2006	green, swir2	(green - swir2)/(green + swir2)
MSAVI	Modified Soil Adjusted Vegetation Index	Qi1994	red, nir	nir + 0.5 - (0.5 * sqrt((2 * nir + 1)^2 - 8 * (nir - (2 * red))))
MSAVI2	Modified Soil Adjusted Vegetation Index 2	Qi1994	red, nir	(2 * (nir + 1) - sqrt((2 * nir + 1)^2 - 8 * (nir - red)))/2
NBRI	Normalised Burn Ratio Index	Garcia1991	nir, swir3	(nir - swir3)/(nir + swir3)
NDVIC	Corrected Normalised Difference Vegetation Index	Nemani1993	red, nir, swir2	(nir - red)/(nir + red) * (1 - ((swir2 - swir2ccc)/(swir2coc - swir2ccc)))
NDWI	Normalised Difference Water Index	McFeeters1996	green, nir	(green - nir)/(green + nir)
NDWI2	Normalised Difference Water Index	Gao1996	nir, swir2	(nir - swir2)/(nir + swir2)
NRVI	Normalised Ratio Vegetation Index	Baret1991	red, nir	(red/nir - 1)/(red/nir + 1)
RVI	Ratio Vegetation Index		red, nir	red/nir
SATVI	Soil Adjusted Total Vegetation Index	Marsett2006	red, swir2, swir3	(swir2 - red)/(swir2 + red + L) * (1 + L) - (swir3/2)
SAVI	Soil Adjusted Vegetation Index	Huete1988	red, nir	(nir - red) * (1 + L)/(nir + red + L)
SLAVI	Specific Leaf Area Vegetation Index	Lymburger2000	red, nir, swir2	nir/(red + swir2)
SR	Simple Ratio Vegetation Index	Birth1968	red, nir	nir/red
TVI	Transformed Vegetation Index	Deering1975	red, nir	sqrt((nir - red)/(nir + red) + 0.5)
TTVI	Thiam's Transformed Vegetation Index	Thiam1997	red, nir	sqrt(abs((nir - red)/(nir + red) + 0.5))
WDVI	Weighted Difference Vegetation Index	Richardson1977	red, nir	nir - s * red
Some indices require additional parameters, such as the slope of the soil line wich are specified via a list to the coefs argument. Although the defaults are sensible values, values like the soil brigthness factor L for SAVI should be adapted depending on the characteristics of the scene. The coefficients are:

Coefficient	Description	Affected Indices
s	slope of the soil line	DVI, WDVI
L_evi, C1, C2, G	various	EVI
L	soil brightness factor	SAVI, SATVI
swir2ccc	minimum swir2 value (completely closed forest canopy)	NDVIC
swir2coc	maximum swir2 value (completely open canopy)	NDVIC
The wavelength band names are defined following Schowengertd 2007, p10. The last column shows exemplarily which Landsat 5 TM bands correspond to which wavelength range definition.



Automated Water Extraction Index (AWEI) - for landsat.
AWEI = 4 * (Green - SWIR2) - (0.25 * NIR + 2.75 * SWIR1)
https://www.sciencedirect.com/science/article/abs/pii/S0034425713002873

Two versions in this article:
https://www.tandfonline.com/doi/full/10.1080/10095020.2015.1017911


More indices: https://www.geo.university/pages/blog?p=spectral-indices-with-multispectral-satellite-data

"""
