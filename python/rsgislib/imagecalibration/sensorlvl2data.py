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


def create_stacked_ls8_cl2_lv2_img(
    input_file,
    out_dir,
    tmp_dir,
    scale_factor=10000,
    gdalformat="KEA",
    delete_inter_data=True,
):
    """
    A function which extracts the USGS collection-2 level-2 data and creates a single
    multi-band image file in the output directory. Note the output data type is
    32bit integer and the no data value is -9999.

    :param input_file: file name and path to the input TAR file.
    :param out_dir: output directory where the output file will be written.
    :param tmp_dir: a temporary directory where intermediate files will be written.
    :param scale_factor: the scale factor applied to the output reflectance values.
                         default is 10,000
    :param gdalformat: the output file format.
    :param delete_inter_data: delete any intermediate files created is True (Default)

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

    if delete_inter_data:
        import shutil

        shutil.rmtree(raw_dir)
        for img_band in sref_bands:
            rsgislib.imageutils.delete_gdal_layer(img_band)
