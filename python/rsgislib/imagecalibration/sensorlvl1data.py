#!/usr/bin/env python
############################################################################
#  sensorlvl1data.py
#
#  Copyright 2022 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
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
# Purpose: Provide a set of utilities for using downloaded level 2 data
#          from data providers in an easy manner.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 24/6/2022
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

import math

import rsgislib

import numpy

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False

def parse_landsat_mss_c2_qa_radsat_img(
    input_img: str, output_img: str, gdalformat: str = "KEA"
):
    """
    A function which will parse the bit QA RADSAT image from Landsat Collection 2
    MSS and create a multi-band output - 1 band per variable. While this takes up more
    disk space than the bit encoded image it is much easier to use!

    The output bands are:
     * Band 4 Saturation (0-1)
     * Band 5 Saturation (0-1)
     * Band 6 Saturation (0-1)
     * Band 7 Saturation (0-1)
     * Dropped Pixel (0-1)
     * Terrain Occlusion (0-1)

    :param input_img: The input landsat RADSAT QA image.
    :param output_img: The output image path - the output image will have 12 bands.
    :param gdalformat: the output image format.

    """
    from rios import applier

    import rsgislib.imagecalc
    import rsgislib.imageutils

    unq_img_vals = rsgislib.imagecalc.get_unique_values(input_img, img_band=1)

    qa_lut = dict()
    for val in unq_img_vals:
        qa_lut[val] = dict()
        qa_lut[val]["B4SAT"] = 0
        qa_lut[val]["B5SAT"] = 0
        qa_lut[val]["B6SAT"] = 0
        qa_lut[val]["B7SAT"] = 0
        qa_lut[val]["DROPPED"] = 0

        val_bin = numpy.flip(
            numpy.unpackbits(numpy.flip(numpy.array([val]).view(numpy.uint8)))
        )
        # print("{} = {}".format(val, val_bin))
        if val_bin[3] == 1:
            qa_lut[val]["B4SAT"] = 1
        if val_bin[4] == 1:
            qa_lut[val]["B5SAT"] = 1
        if val_bin[5] == 1:
            qa_lut[val]["B6SAT"] = 1
        if val_bin[6] == 1:
            qa_lut[val]["B7SAT"] = 1
        if val_bin[9] == 1:
            qa_lut[val]["DROPPED"] = 1

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.qa_lut = qa_lut
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _apply_gen_ls_mss_c2_qa_img(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        img_dims = inputs.image.shape
        img_dims_lst = list(img_dims)
        img_dims_lst[0] = 5
        outputs.outimage = numpy.zeros(img_dims_lst, dtype=numpy.uint8)

        for val in otherargs.qa_lut:
            outputs.outimage[0][inputs.image[0] == val] = otherargs.qa_lut[val]["B4SAT"]
            outputs.outimage[1][inputs.image[0] == val] = otherargs.qa_lut[val]["B5SAT"]
            outputs.outimage[2][inputs.image[0] == val] = otherargs.qa_lut[val]["B6SAT"]
            outputs.outimage[3][inputs.image[0] == val] = otherargs.qa_lut[val]["B7SAT"]
            outputs.outimage[4][inputs.image[0] == val] = otherargs.qa_lut[val][
                "DROPPED"
            ]

    applier.apply(
        _apply_gen_ls_mss_c2_qa_img, infiles, outfiles, otherargs, controls=aControls
    )

    rsgislib.imageutils.set_band_names(
        output_img,
        band_names=[
            "B4SAT",
            "B5SAT",
            "B6SAT",
            "B7SAT",
            "DROPPED",
        ],
    )
    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
    )



def create_stacked_toa_ls_oli_cl2_lv1_img(
    input_file: str,
    out_dir: str,
    tmp_dir: str,
    scale_factor: int = 10000,
    gdalformat: str = "KEA",
    delete_inter_data: bool = True,
    inc_full_qa_pxl_img: bool = False,
):
    """
    A function which extracts the USGS collection-2 level-1 data and creates a single
    multi-band TOA image file in the output directory. Note the output data type is
    32bit integer and the no data value is -9999. A cloud mask and valid area images
    are also provided and extracted from the pixel QA image provided in the collection
    2 package. Note this function just works for Landsat 8 and 9 data.

    :param input_file: file name and path to the input TAR file.
    :param out_dir: output directory where the output file will be written.
    :param tmp_dir: a temporary directory where intermediate files will be written.
    :param scale_factor: the scale factor applied to the output reflectance values.
                         default is 10,000
    :param gdalformat: the output file format.
    :param delete_inter_data: delete any intermediate files created is True (Default)
    :param inc_full_qa_pxl_img: Include the full pixel QA image split into individual
                                bands in the output.

    .. code:: python

        from rsgislib.imagecalibration.sensorlvl2data import create_stacked_sref_ls_oli_cl2_lv2_img
        create_stacked_sref_ls_oli_cl2_lv2_img("LC08_L2SP_135046_20210319_20210328_02_T1.tar", "outputs", "tmp", gdalformat="GTIFF")

    """
    import os

    import rsgislib.imagecalc
    import rsgislib.imagecalibration.sensorlvl2data
    import rsgislib.imageutils
    import rsgislib.rastergis
    import rsgislib.tools.filetools
    import rsgislib.tools.sensors
    import rsgislib.tools.utils

    if not os.path.exists(out_dir):
        os.mkdir(out_dir)

    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)

    if gdalformat == "GTIFF":
        rsgislib.imageutils.set_env_vars_lzw_gtiff_outs()

    raw_dir = rsgislib.tools.filetools.untar_file(input_file, tmp_dir)

    mtl_header = rsgislib.tools.filetools.find_file_none(raw_dir, "*MTL.txt")

    if mtl_header is None:
        raise rsgislib.RSGISPyException("Could not find the MTL header")

    ls8_head_info = rsgislib.tools.sensors.read_landsat_mtl_to_dict(mtl_header)

    ls_prod_id = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "LANDSAT_PRODUCT_ID"]
    ).lower()

    b1_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_1"]
    )
    b2_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_2"]
    )
    b3_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_3"]
    )
    b4_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_4"]
    )
    b5_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_5"]
    )
    b6_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_6"]
    )
    b7_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_7"]
    )

    qa_pxl_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls8_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_QUALITY_L1_PIXEL"]
    )

    b1_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_1"],
    )
    b2_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_2"],
    )
    b3_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_3"],
    )
    b4_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_4"],
    )
    b5_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_5"],
    )
    b6_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_6"],
    )
    b7_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_7"],
    )

    b1_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_1"],
    )
    b2_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_2"],
    )
    b3_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_3"],
    )
    b4_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_4"],
    )
    b5_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_5"],
    )
    b6_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_6"],
    )
    b7_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_7"],
    )

    solor_elev = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info, ["IMAGE_ATTRIBUTES", "SUN_ELEVATION"]
    )

    band_info = []
    band_info.append(
        {"img": os.path.join(raw_dir, b1_img), "multi": b1_multi, "add": b1_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b2_img), "multi": b2_multi, "add": b2_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b3_img), "multi": b3_multi, "add": b3_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b4_img), "multi": b4_multi, "add": b4_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b5_img), "multi": b5_multi, "add": b5_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b6_img), "multi": b6_multi, "add": b6_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b7_img), "multi": b7_multi, "add": b7_add}
    )

    sref_bands = []
    for i, band in enumerate(band_info):
        out_img_band = os.path.join(tmp_dir, "{}_b{}_toa.kea".format(ls_prod_id, i + 1))
        exp = "b1==0?-9999:(((b1*{})+{})/{})*{}".format(
            band["multi"], band["add"], math.sin(math.radians(solor_elev)), scale_factor
        )
        rsgislib.imagecalc.image_math(
            band["img"],
            out_img_band,
            exp,
            "KEA",
            rsgislib.TYPE_32INT,
        )
        sref_bands.append(out_img_band)

    band_names = ["coastal", "blue", "green", "red", "nir", "swir1", "swir2"]
    out_img_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    output_img = os.path.join(out_dir, "{}_toa.{}".format(ls_prod_id, out_img_ext))
    rsgislib.imageutils.stack_img_bands(
        sref_bands, band_names, output_img, None, -9999, gdalformat, rsgislib.TYPE_32INT
    )
    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=True, no_data_val=-9999, calc_pyramids=True
    )

    if inc_full_qa_pxl_img:
        qa_pxl_mband_img = os.path.join(
            out_dir, "{}_pxl_qa.{}".format(ls_prod_id, out_img_ext)
        )
        qa_pxl_format = gdalformat
    else:
        qa_pxl_mband_img = os.path.join(tmp_dir, "{}_pxl_qa.kea".format(ls_prod_id))
        qa_pxl_format = "KEA"

    qa_pxl_img_path = os.path.join(raw_dir, qa_pxl_img)
    rsgislib.imagecalibration.sensorlvl2data.parse_landsat_c2_qa_pixel_img(
        qa_pxl_img_path, qa_pxl_mband_img, qa_pxl_format
    )

    cloud_msk_img = os.path.join(
        out_dir, "{}_clouds.{}".format(ls_prod_id, out_img_ext)
    )
    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("DilatedCloud", qa_pxl_mband_img, 2))
    band_defns.append(rsgislib.imagecalc.BandDefn("Cirrus", qa_pxl_mband_img, 3))
    band_defns.append(rsgislib.imagecalc.BandDefn("Cloud", qa_pxl_mband_img, 4))
    band_defns.append(rsgislib.imagecalc.BandDefn("CloudShadow", qa_pxl_mband_img, 5))
    rsgislib.imagecalc.band_math(
        cloud_msk_img,
        "DilatedCloud==1?1:Cirrus==1?1:Cloud==1?1:CloudShadow==1?2:0",
        gdalformat,
        rsgislib.TYPE_8UINT,
        band_defns,
    )
    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            cloud_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            cloud_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    vld_msk_img = os.path.join(out_dir, "{}_vld_msk.{}".format(ls_prod_id, out_img_ext))
    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("fill", qa_pxl_mband_img, 1))
    rsgislib.imagecalc.band_math(
        vld_msk_img, "fill==0?1:0", gdalformat, rsgislib.TYPE_8UINT, band_defns
    )
    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            vld_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            vld_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if delete_inter_data:
        import shutil

        shutil.rmtree(raw_dir)
        for img_band in sref_bands:
            rsgislib.imageutils.delete_gdal_layer(img_band)
        if not inc_full_qa_pxl_img:
            rsgislib.imageutils.delete_gdal_layer(qa_pxl_mband_img)



def create_stacked_toa_ls_mss_lv1_img(
    input_file: str,
    out_dir: str,
    tmp_dir: str,
    scale_factor: int = 10000,
    gdalformat: str = "GTIFF",
    delete_inter_data: bool = True,
    inc_full_qa_pxl_img: bool = False,
    inc_full_qa_radsat_img: bool = False,
    add_sat_pxls_to_msk: bool = False,
    dilate_cloud_msk: int = None,
):
    """
    A function which extracts the USGS Landsat MSS collection-2 level-1 data and
    creates a single multi-band TOA image file in the output directory. Note the
    output data type is 32bit integer and the no data value is -9999. A cloud mask
    and valid area images are also provided and extracted from the pixel QA image
    provided in the collection 2 package. Note this function just works for Landsat
    MSS data (Landsat 1-5).

    :param input_file: file name and path to the input TAR file.
    :param out_dir: output directory where the output file will be written.
    :param tmp_dir: a temporary directory where intermediate files will be written.
    :param scale_factor: the scale factor applied to the output reflectance values.
                         default is 10,000
    :param gdalformat: the output file format.
    :param delete_inter_data: delete any intermediate files created is True (Default)
    :param inc_full_qa_pxl_img: Include the full pixel QA image split into individual
                                bands in the output.
    :param inc_full_qa_radsat_img: Include the full RADSAT QA image split into
                                   individual bands in the output.
    :param add_sat_pxls_to_msk: Add pixels identified as saturated to the valid mask
                                so saturated pixels would be specified as invalid
                                pixels.
    :param dilate_cloud_msk: run a morphological dilation on the cloud mask to
                             expand the area within the cloud mask. If None then
                             ignore but otherwise provide an integer (e.g., 11) to
                             apply a dilation with a circular operator of that size.
                             Note, the operator must be an odd number (e.g., 5, 7, 9)

    .. code:: python

        from rsgislib.imagecalibration.sensorlvl1data import create_stacked_toa_ls_mss_lv1_img
        create_stacked_toa_ls_mss_lv1_img("LM01_L1TP_121062_19731004_20200909_02_T2.tar", "outputs", "tmp", gdalformat="GTIFF")

    """
    import os

    import rsgislib.imagecalc
    import rsgislib.imagecalibration.sensorlvl2data
    import rsgislib.imageutils
    import rsgislib.rastergis
    import rsgislib.tools.filetools
    import rsgislib.tools.sensors
    import rsgislib.tools.utils
    import rsgislib.imagemorphology

    if not os.path.exists(out_dir):
        os.mkdir(out_dir)

    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)

    if gdalformat == "GTIFF":
        rsgislib.imageutils.set_env_vars_deflate_gtiff_outs()

    raw_dir = rsgislib.tools.filetools.untar_file(input_file, tmp_dir)

    mtl_header = rsgislib.tools.filetools.find_file_none(raw_dir, "*MTL.txt")

    if mtl_header is None:
        raise rsgislib.RSGISPyException("Could not find the MTL header")

    ls_head_info = rsgislib.tools.sensors.read_landsat_mtl_to_dict(mtl_header)

    ls_prod_id = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info, ["PRODUCT_CONTENTS", "LANDSAT_PRODUCT_ID"]
    ).lower()

    b4_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_4"]
    )
    b5_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_5"]
    )
    b6_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_6"]
    )
    b7_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_7"]
    )

    qa_pxl_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info, ["PRODUCT_CONTENTS", "FILE_NAME_QUALITY_L1_PIXEL"]
    )

    qa_radsat_img = rsgislib.tools.utils.dict_struct_get_str_value(
        ls_head_info,
        ["PRODUCT_CONTENTS", "FILE_NAME_QUALITY_L1_RADIOMETRIC_SATURATION"],
    )

    b4_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_4"],
    )
    b5_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_5"],
    )
    b6_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_6"],
    )
    b7_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_MULT_BAND_7"],
    )

    b4_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_4"],
    )
    b5_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_5"],
    )
    b6_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_6"],
    )
    b7_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info,
        ["LEVEL1_RADIOMETRIC_RESCALING", "REFLECTANCE_ADD_BAND_7"],
    )

    solor_elev = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls_head_info, ["IMAGE_ATTRIBUTES", "SUN_ELEVATION"]
    )

    band_info = []
    band_info.append(
        {"img": os.path.join(raw_dir, b4_img), "multi": b4_multi, "add": b4_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b5_img), "multi": b5_multi, "add": b5_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b6_img), "multi": b6_multi, "add": b6_add}
    )
    band_info.append(
        {"img": os.path.join(raw_dir, b7_img), "multi": b7_multi, "add": b7_add}
    )

    sref_bands = []
    for i, band in enumerate(band_info):
        out_img_band = os.path.join(tmp_dir, "{}_b{}_toa.tif".format(ls_prod_id, i + 1))
        exp = "b1==0?-9999:(((b1*{})+{})/{})*{}".format(
            band["multi"], band["add"], math.sin(math.radians(solor_elev)), scale_factor
        )
        rsgislib.imagecalc.image_math(
            band["img"],
            out_img_band,
            exp,
            "GTIFF",
            rsgislib.TYPE_32INT,
        )
        sref_bands.append(out_img_band)

    band_names = ["green", "red", "nir1", "nir2"]
    out_img_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    output_img = os.path.join(out_dir, "{}_toa.{}".format(ls_prod_id, out_img_ext))
    rsgislib.imageutils.stack_img_bands(
        sref_bands, band_names, output_img, None, -9999, gdalformat, rsgislib.TYPE_32INT
    )
    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=True, no_data_val=-9999, calc_pyramids=True
    )

    if inc_full_qa_pxl_img:
        qa_pxl_mband_img = os.path.join(
            out_dir, "{}_pxl_qa.{}".format(ls_prod_id, out_img_ext)
        )
        qa_pxl_format = gdalformat
    else:
        qa_pxl_mband_img = os.path.join(tmp_dir, "{}_pxl_qa.tif".format(ls_prod_id))
        qa_pxl_format = "GTIFF"

    qa_pxl_img_path = os.path.join(raw_dir, qa_pxl_img)
    rsgislib.imagecalibration.sensorlvl2data.parse_landsat_c2_qa_pixel_img(
        qa_pxl_img_path, qa_pxl_mband_img, qa_pxl_format
    )

    if dilate_cloud_msk is not None:
        cloud_msk_img = os.path.join(
            tmp_dir, "{}_clouds.{}".format(ls_prod_id, out_img_ext)
        )
    else:
        cloud_msk_img = os.path.join(
            out_dir, "{}_clouds.{}".format(ls_prod_id, out_img_ext)
        )
    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("DilatedCloud", qa_pxl_mband_img, 2))
    band_defns.append(rsgislib.imagecalc.BandDefn("Cirrus", qa_pxl_mband_img, 3))
    band_defns.append(rsgislib.imagecalc.BandDefn("Cloud", qa_pxl_mband_img, 4))
    band_defns.append(rsgislib.imagecalc.BandDefn("CloudShadow", qa_pxl_mband_img, 5))
    rsgislib.imagecalc.band_math(
        cloud_msk_img,
        "DilatedCloud==1?1:Cirrus==1?1:Cloud==1?1:CloudShadow==1?2:0",
        gdalformat,
        rsgislib.TYPE_8UINT,
        band_defns,
    )

    if dilate_cloud_msk is not None:
        cloud_msk_tmp_img = cloud_msk_img
        cloud_msk_img = os.path.join(
            out_dir, "{}_clouds.{}".format(ls_prod_id, out_img_ext)
        )
        morph_op_file = os.path.join(tmp_dir, f"{ls_prod_id}_morph_op.gmtxt")
        rsgislib.imagemorphology.create_circular_op(
            morph_op_file, op_size=dilate_cloud_msk
        )

        rsgislib.imagemorphology.image_dilate(
            cloud_msk_tmp_img,
            cloud_msk_img,
            morph_op_file=morph_op_file,
            use_op_file=True,
            op_size=dilate_cloud_msk,
            gdalformat=gdalformat,
            datatype=rsgislib.TYPE_8UINT,
        )

    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            cloud_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            cloud_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if inc_full_qa_radsat_img:
        qa_radsat_mband_img = os.path.join(
            out_dir, "{}_radsat_qa.{}".format(ls_prod_id, out_img_ext)
        )
        qa_radsat_format = gdalformat
    else:
        qa_radsat_mband_img = os.path.join(
            tmp_dir, "{}_radsat_qa.tif".format(ls_prod_id)
        )
        qa_radsat_format = "GTIFF"

    qa_radsat_img_path = os.path.join(raw_dir, qa_radsat_img)
    parse_landsat_mss_c2_qa_radsat_img(
        qa_radsat_img_path, output_img=qa_radsat_mband_img, gdalformat=qa_radsat_format
    )

    vld_msk_img = os.path.join(out_dir, "{}_vld_msk.{}".format(ls_prod_id, out_img_ext))
    if add_sat_pxls_to_msk:
        band_defns = list()
        band_defns.append(rsgislib.imagecalc.BandDefn("fill", qa_pxl_mband_img, 1))
        band_defns.append(
            rsgislib.imagecalc.BandDefn("dropped", qa_radsat_mband_img, 5)
        )

        band_defns.append(rsgislib.imagecalc.BandDefn("b4sat", qa_radsat_mband_img, 1))
        band_defns.append(rsgislib.imagecalc.BandDefn("b5sat", qa_radsat_mband_img, 2))
        band_defns.append(rsgislib.imagecalc.BandDefn("b6sat", qa_radsat_mband_img, 3))
        band_defns.append(rsgislib.imagecalc.BandDefn("b7sat", qa_radsat_mband_img, 4))
        vld_msk_exp = "(b4sat==1)||(b5sat==1)||(b6sat==1)||(b7sat==1)?0:(fill==0)&&(dropped==0)?1:0"
    else:
        band_defns = list()
        band_defns.append(rsgislib.imagecalc.BandDefn("fill", qa_pxl_mband_img, 1))
        band_defns.append(
            rsgislib.imagecalc.BandDefn("dropped", qa_radsat_mband_img, 5)
        )
        vld_msk_exp = "(fill==0)&&(dropped==0)?1:0"
    rsgislib.imagecalc.band_math(
        vld_msk_img, vld_msk_exp, gdalformat, rsgislib.TYPE_8UINT, band_defns
    )
    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            vld_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            vld_msk_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if delete_inter_data:
        import shutil

        shutil.rmtree(raw_dir)
        for img_band in sref_bands:
            rsgislib.imageutils.delete_gdal_layer(img_band)
        if not inc_full_qa_pxl_img:
            rsgislib.imageutils.delete_gdal_layer(qa_pxl_mband_img)
        if not inc_full_qa_radsat_img:
            rsgislib.imageutils.delete_gdal_layer(qa_radsat_mband_img)
        if os.path.exists(morph_op_file):
            os.remove(morph_op_file)
        if os.path.exists(cloud_msk_tmp_img):
            rsgislib.imageutils.delete_gdal_layer(cloud_msk_tmp_img)
