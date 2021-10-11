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


def extract_ls8_col2_lvl2(in_tar_file, output_dir, gdalformat, tmp_dir, delete_extracted_data=True):
    """
    A function which extracts the USGS collection-2 level-2 data and creates a single
    multi-data image file in the output directory.

    :param in_tar_file: file name and path to the input TAR file.
    :param output_dir: output directory where the output file will be saved.
    :param gdalformat: output file format.
    :param tmp_dir: tmp directory where the tar will be extracted to.
    :param delete_extracted_data: If True (default) then the extracted data will be
                                  deleted when analysis is finished.

    """
    import os
    import shutil

    import rsgislib
    import rsgislib.imageutils
    import rsgislib.imagecalc
    import rsgislib.tools.filetools
    import rsgislib.tools.sensors
    import rsgislib.tools.utils

    if gdalformat == 'GTIFF':
        rsgislib.imageutils.set_env_vars_lzw_gtiff_outs(bigtiff=False)

    extracted_dir = rsgislib.tools.filetools.untar_file(in_tar_file, tmp_dir)
    mtl_txt_file = rsgislib.tools.filetools.find_file_none(extracted_dir, "*MTL.txt")
    if mtl_txt_file is None:
        raise rsgislib.RSGISPyException("Could not find MTL TXT file.")

    mtl_dict = rsgislib.tools.sensors.read_landsat_mtl_to_dict(mtl_txt_file)

    spacecraft = rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["IMAGE_ATTRIBUTES", "SPACECRAFT_ID"])
    if spacecraft != "LANDSAT_8":
        raise rsgislib.RSGISPyException("Expecting Landsat 8 imagery")

    l2_processing = rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["LEVEL2_PROCESSING_RECORD", "PROCESSING_LEVEL"])
    if l2_processing != "L2SP":
        raise rsgislib.RSGISPyException("Expecting Level 2 landsat imagery")

    acq_date = rsgislib.tools.utils.dict_struct_get_date_value(mtl_dict, ["IMAGE_ATTRIBUTES", "DATE_ACQUIRED"], date_format="%Y-%m-%d")

    wrs_row = int(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["IMAGE_ATTRIBUTES", "WRS_ROW"]))
    wrs_path = int(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["IMAGE_ATTRIBUTES", "WRS_PATH"]))

    ll_lat = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_LL_LAT_PRODUCT"]))
    ll_lon = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_LL_LON_PRODUCT"]))

    lr_lat = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_LR_LAT_PRODUCT"]))
    lr_lon = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_LR_LON_PRODUCT"]))

    ul_lat = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_UL_LAT_PRODUCT"]))
    ul_lon = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_UL_LON_PRODUCT"]))

    ur_lat = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_UR_LAT_PRODUCT"]))
    ur_lon = float(rsgislib.tools.utils.dict_struct_get_numeric_value(mtl_dict, ["PROJECTION_ATTRIBUTES", "CORNER_UR_LON_PRODUCT"]))

    l_lon_mid = ll_lon + (lr_lon - ll_lon) / 2
    u_lon_mid = ul_lon + (ur_lon - ul_lon) / 2
    lon_mid = min(l_lon_mid, u_lon_mid) + (max(l_lon_mid, u_lon_mid) - min(l_lon_mid, u_lon_mid)) / 2

    l_lat_mid = ll_lat + (ul_lat - ll_lat) / 2
    r_lat_mid = lr_lat + (ur_lat - lr_lat) / 2
    lat_mid = min(l_lat_mid, r_lat_mid) + (max(l_lat_mid, r_lat_mid) - min(l_lat_mid, r_lat_mid)) / 2

    pos_str = rsgislib.tools.projection.get_deg_coord_as_str(lat_mid, lon_mid, n_chars=3)

    acq_date_str = acq_date.strftime('%Y%m%d')
    basename = 'ls8_{}_{}_r{}p{}'.format(pos_str, acq_date_str, wrs_row, wrs_path)
    print(basename)

    img_band1 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_1"]))
    img_band2 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_2"]))
    img_band3 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_3"]))
    img_band4 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_4"]))
    img_band5 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_5"]))
    img_band6 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_6"]))
    img_band7 = os.path.join(extracted_dir, rsgislib.tools.utils.dict_struct_get_str_value(mtl_dict, ["PRODUCT_CONTENTS", "FILE_NAME_BAND_7"]))
    band_names = ['coastal', 'blue', 'green', 'red', 'nir', 'swir1', 'swir2']

    img_ext = rsgislib.imageutils.get_file_img_extension(gdalformat)

    stacked_sref_scaled_img = os.path.join(extracted_dir, "{}_sref_scaled.{}".format(basename, img_ext))
    rsgislib.imageutils.stack_img_bands([img_band1, img_band2, img_band3, img_band4, img_band5, img_band6, img_band7], band_names, stacked_sref_scaled_img, skip_value=None, no_data_val=0, gdalformat=gdalformat, datatype=rsgislib.TYPE_16UINT)

    out_sref_img = os.path.join(output_dir, "{}_sref.{}".format(basename, img_ext))
    rsgislib.imagecalc.image_math(stacked_sref_scaled_img, out_sref_img, "b1==0?0:((b1*0.0000275)-0.2)<0.0?1:((b1*0.0000275)-0.2)*1000", gdalformat, rsgislib.TYPE_16UINT)
    rsgislib.imageutils.set_band_names(out_sref_img, band_names)
    rsgislib.imageutils.pop_img_stats(out_sref_img, use_no_data=True, no_data_val=0, calc_pyramids=True)

    if delete_extracted_data:
        shutil.rmtree(extracted_dir)

