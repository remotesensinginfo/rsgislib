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

import rsgislib
import math


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
    import rsgislib.tools.filetools
    import rsgislib.tools.sensors
    import rsgislib.tools.utils
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.rastergis
    import rsgislib.imagecalibration.sensorlvl2data

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
