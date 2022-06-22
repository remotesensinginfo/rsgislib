#!/usr/bin/env python
############################################################################
#  sensorlvl2data.py
#
#  Copyright 2021 RSGISLib.
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
# Date: 10/10/2021
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
############################################################################

import rsgislib
import numpy


def create_stacked_ls8_cl2_lv2_img(
    input_file: str,
    out_dir: str,
    tmp_dir: str,
    scale_factor: int = 10000,
    gdalformat: str = "KEA",
    delete_inter_data: bool = True,
    inc_full_qa_pxl_img: bool = False,
):
    """
    A function which extracts the USGS collection-2 level-2 data and creates a single
    multi-band image file in the output directory. Note the output data type is
    32bit integer and the no data value is -9999. A cloud mask and valid area images
    are also provided and extracted from the pixel QA image provided in the collection
    2 package.

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

        import rsgislib.imagecalibration.sensorlvl2data
        rsgislib.imagecalibration.sensorlvl2data.create_stacked_ls8_cl2_lv2_img("LC08_L2SP_135046_20210319_20210328_02_T1.tar", "outputs", "tmp", gdalformat="GTIFF")

    """

    import os
    import rsgislib.tools.filetools
    import rsgislib.tools.sensors
    import rsgislib.tools.utils
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.rastergis

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
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_1"],
    )
    b2_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_2"],
    )
    b3_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_3"],
    )
    b4_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_4"],
    )
    b5_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_5"],
    )
    b6_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_6"],
    )
    b7_multi = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_MULT_BAND_7"],
    )

    b1_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_1"],
    )
    b2_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_2"],
    )
    b3_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_3"],
    )
    b4_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_4"],
    )
    b5_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_5"],
    )
    b6_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_6"],
    )
    b7_add = rsgislib.tools.utils.dict_struct_get_numeric_value(
        ls8_head_info,
        ["LEVEL2_SURFACE_REFLECTANCE_PARAMETERS", "REFLECTANCE_ADD_BAND_7"],
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
        out_img_band = os.path.join(
            tmp_dir, "{}_b{}_sref.kea".format(ls_prod_id, i + 1)
        )
        rsgislib.imagecalc.image_math(
            band["img"],
            out_img_band,
            "b1==0?-9999:((b1*{})+{})*{}".format(
                band["multi"], band["add"], scale_factor
            ),
            "KEA",
            rsgislib.TYPE_32INT,
        )
        sref_bands.append(out_img_band)

    band_names = ["coastal", "blue", "green", "red", "nir", "swir1", "swir2"]
    out_img_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)
    output_img = os.path.join(out_dir, "{}_sref.{}".format(ls_prod_id, out_img_ext))
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
    parse_landsat_c2_qa_pixel_img(qa_pxl_img_path, qa_pxl_mband_img, qa_pxl_format)

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


def parse_landsat_c2_qa_pixel_img(
    input_img: str, output_img: str, gdalformat: str = "KEA"
):
    """
    A function which will parse the bit QA Pixel image from Landsat Collection 2
    and create a multi-band output - 1 band per variable. While this takes up more
    disk space than the bit encoded image it is much easier to use!

    The output bands are:
     * Fill - No data regions (0-1)
     * DilatedCloud - Cloud dilation regions (0-1)
     * Cirrus - Cirrus clouds present (0-1)
     * Cloud - Clouds present (0-1)
     * CloudShadow - Cloud shadow present (0-1)
     * Snow - Snow present (0-1)
     * Clear - Clear sky pixel (0-1)
     * Water - Water present (0-1)
     * CloudConfidence - Confidence of cloud classification (1-4)
     * CloudShadowConfidence - Confidence of cloud shadows classification (1-4)
     * SnowIceConfidence - Confidence of snow and ice classification (1-4)
     * CirrusConfidence - Confidence of cirrus classification (1-4)

    :param input_img: The input landsat pixel QA image.
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
        qa_lut[val]["Fill"] = 0
        qa_lut[val]["DilatedCloud"] = 0
        qa_lut[val]["Cirrus"] = 0
        qa_lut[val]["Cloud"] = 0
        qa_lut[val]["CloudShadow"] = 0
        qa_lut[val]["Snow"] = 0
        qa_lut[val]["Clear"] = 0
        qa_lut[val]["Water"] = 0
        qa_lut[val]["CloudConfidence"] = 0
        qa_lut[val]["CloudShadowConfidence"] = 0
        qa_lut[val]["SnowIceConfidence"] = 0
        qa_lut[val]["CirrusConfidence"] = 0

        val_bin = numpy.flip(
            numpy.unpackbits(numpy.flip(numpy.array([val]).view(numpy.uint8)))
        )
        # print("{} = {}".format(val, val_bin))
        if val_bin[0] == 1:
            qa_lut[val]["Fill"] = 1
        if val_bin[1] == 1:
            qa_lut[val]["DilatedCloud"] = 1
        if val_bin[2] == 1:
            qa_lut[val]["Cirrus"] = 1
        if val_bin[3] == 1:
            qa_lut[val]["Cloud"] = 1
        if val_bin[4] == 1:
            qa_lut[val]["CloudShadow"] = 1
        if val_bin[5] == 1:
            qa_lut[val]["Snow"] = 1
        if val_bin[6] == 1:
            qa_lut[val]["Clear"] = 1
        if val_bin[7] == 1:
            qa_lut[val]["Water"] = 1

        if (val_bin[8] == 0) & (val_bin[9] == 1):
            qa_lut[val]["CloudConfidence"] = 2
        elif (val_bin[8] == 1) & (val_bin[9] == 0):
            qa_lut[val]["CloudConfidence"] = 3
        elif (val_bin[8] == 1) & (val_bin[9] == 1):
            qa_lut[val]["CloudConfidence"] = 4
        else:
            qa_lut[val]["CloudConfidence"] = 1

        if (val_bin[10] == 0) & (val_bin[11] == 1):
            qa_lut[val]["CloudShadowConfidence"] = 2
        elif (val_bin[10] == 1) & (val_bin[11] == 0):
            qa_lut[val]["CloudShadowConfidence"] = 3
        elif (val_bin[10] == 1) & (val_bin[11] == 1):
            qa_lut[val]["CloudShadowConfidence"] = 4
        else:
            qa_lut[val]["CloudShadowConfidence"] = 1

        if (val_bin[12] == 0) & (val_bin[13] == 1):
            qa_lut[val]["SnowIceConfidence"] = 2
        elif (val_bin[12] == 1) & (val_bin[13] == 0):
            qa_lut[val]["SnowIceConfidence"] = 3
        elif (val_bin[12] == 1) & (val_bin[13] == 1):
            qa_lut[val]["SnowIceConfidence"] = 4
        else:
            qa_lut[val]["SnowIceConfidence"] = 1

        if (val_bin[14] == 0) & (val_bin[15] == 1):
            qa_lut[val]["CirrusConfidence"] = 2
        elif (val_bin[14] == 1) & (val_bin[15] == 0):
            qa_lut[val]["CirrusConfidence"] = 3
        elif (val_bin[14] == 1) & (val_bin[15] == 1):
            qa_lut[val]["CirrusConfidence"] = 4
        else:
            qa_lut[val]["CirrusConfidence"] = 1

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.qa_lut = qa_lut
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _apply_gen_ls_c2_qa_img(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        img_dims = inputs.image.shape
        img_dims_lst = list(img_dims)
        img_dims_lst[0] = 12
        # print(img_dims_lst)
        outputs.outimage = numpy.zeros(img_dims_lst, dtype=numpy.uint8)

        for val in otherargs.qa_lut:
            outputs.outimage[0][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["Fill"]
            outputs.outimage[1][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["DilatedCloud"]
            outputs.outimage[2][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["Cirrus"]
            outputs.outimage[3][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["Cloud"]
            outputs.outimage[4][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["CloudShadow"]
            outputs.outimage[5][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["Snow"]
            outputs.outimage[6][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["Clear"]
            outputs.outimage[7][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["Water"]
            outputs.outimage[8][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["CloudConfidence"]
            outputs.outimage[9][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["CloudShadowConfidence"]
            outputs.outimage[10][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["SnowIceConfidence"]
            outputs.outimage[11][numpy.squeeze(inputs.image) == val] = otherargs.qa_lut[
                val
            ]["CirrusConfidence"]

    applier.apply(
        _apply_gen_ls_c2_qa_img, infiles, outfiles, otherargs, controls=aControls
    )

    rsgislib.imageutils.set_band_names(
        output_img,
        band_names=[
            "Fill",
            "DilatedCloud",
            "Cirrus",
            "Cloud",
            "CloudShadow",
            "Snow",
            "Clear",
            "Water",
            "CloudConfidence",
            "CloudShadowConfidence",
            "SnowIceConfidence",
            "CirrusConfidence",
        ],
    )
    rsgislib.imageutils.pop_img_stats(
        output_img, use_no_data=True, no_data_val=0, calc_pyramids=True
    )
