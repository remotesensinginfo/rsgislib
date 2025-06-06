#! /usr/bin/env python
############################################################################
#  classrefpts.py
#
#  Copyright 2025 RSGISLib.
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
# Purpose:  Provide a set of functions to create classification reference points
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 14/02/2025
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import tqdm
from osgeo import gdal
import numpy
import numpy.random


def create_random_ref_smpls_darts(
    input_img: str,
    n_smpls: int,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    img_cls_col: str = "img_cls",
    ref_cls_col: str = "cls_ref",
    processed_col: str = "Processed",
    rnd_seed: int = None,
    img_band: int = 1,
    cls_no_data=None,
    rat_cls_name_col: str = None,
):
    """
    A function to create random reference point sets using random coordinate
    sampling of the input image. The points are intended to be used to assess
    the accuracy of a map, usually derived from an image classification.

    :param input_img: Path to the input image file.
    :param n_smpls: Number of random reference samples to generate.
    :param out_vec_file: Path to the output vector file.
    :param out_vec_lyr: Name of the layer in the output vector file.
    :param out_format: Format of the output vector file (default is "GPKG").
    :param img_cls_col: Name of the column in the output vector file for image class.
    :param ref_cls_col: Name of the column in the output vector file for
                        reference class.
    :param processed_col: Name of the column in the output vector file for
                          processed status.
    :param rnd_seed: Random seed for reproducibility (default is None).
    :param img_band: Band index of the input image to be used for sampling
                     (default is 1).
    :param cls_no_data: No data value for the image class (default is None).
    :param rat_cls_name_col: Name of the column containing class names in the
                             raster attribute table (default is None).

    """
    import secrets
    import pandas
    import geopandas
    import rsgislib.imageutils
    import rsgislib.rastergis

    img_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    x_range = img_bbox[1] - img_bbox[0]
    y_range = img_bbox[3] - img_bbox[2]

    img_res_x, img_res_y = rsgislib.imageutils.get_img_res(input_img, abs_vals=True)

    if cls_no_data is None:
        cls_no_data = rsgislib.imageutils.get_img_no_data_value(
            input_img, img_band=img_band
        )

    img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(input_img)

    if rnd_seed is None:
        rnd_seed = secrets.randbits(128)
    else:
        rnd_seed = abs(rnd_seed)

    rng = numpy.random.default_rng(rnd_seed)

    if rat_cls_name_col is not None:
        cls_names_arr = rsgislib.rastergis.get_column_data(input_img, rat_cls_name_col)
        n_cls = len(cls_names_arr)

    image_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the input image file: '{}'".format(input_img)
        )
    image_band = image_ds.GetRasterBand(img_band)
    if image_band is None:
        raise rsgislib.RSGISPyException("The image band wasn't opened")

    img_data = image_band.ReadAsArray()
    image_ds = None

    pxl_chk_arr = numpy.zeros_like(img_data, dtype=bool)

    x_coords_arr = numpy.zeros([n_smpls], dtype=float)
    y_coords_arr = numpy.zeros([n_smpls], dtype=float)
    cls_pxl_vals_arr = numpy.zeros([n_smpls], dtype=int)
    processed_col_arr = numpy.zeros([n_smpls], dtype=int)
    ref_cls_col_arr = numpy.zeros([n_smpls], dtype=int)

    img_cls_names = list()
    ref_cls_names = list()

    with tqdm.tqdm(total=n_smpls) as pbar:
        found_pts = 0
        while found_pts < n_smpls:
            x_coord = (rng.random() * x_range) + img_bbox[0]
            y_coord = (rng.random() * y_range) + img_bbox[2]

            x_pxl_loc = numpy.floor((x_coord - img_bbox[0]) / img_res_x).astype(int)
            y_pxl_loc = numpy.floor((img_bbox[3] - y_coord) / img_res_y).astype(int)
            if not pxl_chk_arr[y_pxl_loc, x_pxl_loc]:
                x_coord_pxl_grid = (
                    img_bbox[0] + (img_res_x * x_pxl_loc) + (img_res_x / 2)
                )
                y_coord_pxl_grid = (
                    img_bbox[3] - (img_res_y * y_pxl_loc) - (img_res_y / 2)
                )

                cls_pxl_val = img_data[y_pxl_loc, x_pxl_loc]

                if (cls_no_data is None) or (cls_pxl_val != cls_no_data):
                    x_coords_arr[found_pts] = x_coord_pxl_grid
                    y_coords_arr[found_pts] = y_coord_pxl_grid
                    cls_pxl_vals_arr[found_pts] = cls_pxl_val
                    ref_cls_col_arr[found_pts] = cls_pxl_val
                    processed_col_arr[found_pts] = 0
                    if rat_cls_name_col is not None:
                        if cls_pxl_val < n_cls:
                            img_cls_names.append(
                                cls_names_arr[cls_pxl_val].decode("utf-8")
                            )
                            ref_cls_names.append("")

                    found_pts += 1
                    pbar.update(1)

    data_dict = dict()
    data_dict["tmp_x"] = x_coords_arr
    data_dict["tmp_y"] = y_coords_arr
    if rat_cls_name_col is not None:
        data_dict[img_cls_col] = img_cls_names
        data_dict[ref_cls_col] = ref_cls_names
    else:
        data_dict[img_cls_col] = cls_pxl_vals_arr
        data_dict[ref_cls_col] = ref_cls_col_arr
    data_dict[processed_col] = processed_col_arr

    data_df = pandas.DataFrame(data=data_dict)

    pts_gdf = geopandas.GeoDataFrame(
        data_df,
        geometry=geopandas.points_from_xy(data_df.tmp_x, data_df.tmp_y),
        crs=f"EPSG:{img_epsg}",
    )
    pts_gdf = pts_gdf.drop(columns=["tmp_x", "tmp_y"])

    if out_format == "GPKG":
        pts_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        pts_gdf.to_file(out_vec_file, driver=out_format)


def create_stratified_random_ref_smpls_darts(
    input_img: str,
    n_smpls: int,
    out_vec_file: str,
    out_vec_lyr: str,
    eq_n_smpls: bool = False,
    min_cls_smpls: int = None,
    out_format: str = "GPKG",
    img_cls_col: str = "img_cls",
    ref_cls_col: str = "cls_ref",
    processed_col: str = "Processed",
    rnd_seed: int = None,
    img_band: int = 1,
    cls_no_data=None,
    rat_cls_name_col: str = None,
):
    """
    A function to create random reference point sets using random coordinate
    sampling of the input image. The points are intended to be used to assess
    the accuracy of a map, usually derived from an image classification.

    :param input_img: Path to the input image file.
    :param n_smpls: Number of random reference samples to generate.
    :param out_vec_file: Path to the output vector file.
    :param out_vec_lyr: Name of the layer in the output vector file.
    :param eq_n_smpls: Flag indicating if the samples should be equally distributed
                       among classes. Default is False and therefore the points
                       will be statified by the area mapped for each class.
    :param min_cls_smpls: Minimum number of samples per class if provided.
                          Default is None and therefore this parameter is
                          ignored.
    :param out_format: Format of the output vector file (default is "GPKG").
    :param img_cls_col: Name of the column in the output vector file for image class.
    :param ref_cls_col: Name of the column in the output vector file for
                        reference class.
    :param processed_col: Name of the column in the output vector file for
                          processed status.
    :param rnd_seed: Random seed for reproducibility (default is None).
    :param img_band: Band index of the input image to be used for sampling
                     (default is 1).
    :param cls_no_data: No data value for the image class (default is None).
    :param rat_cls_name_col: Name of the column containing class names in the
                             raster attribute table (default is None).

    """
    import secrets
    import pandas
    import geopandas
    import rsgislib.imageutils
    import rsgislib.rastergis

    img_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    x_range = img_bbox[1] - img_bbox[0]
    y_range = img_bbox[3] - img_bbox[2]

    img_res_x, img_res_y = rsgislib.imageutils.get_img_res(input_img, abs_vals=True)

    if cls_no_data is None:
        cls_no_data = rsgislib.imageutils.get_img_no_data_value(
            input_img, img_band=img_band
        )

    img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(input_img)

    if rnd_seed is None:
        rnd_seed = secrets.randbits(128)
    else:
        rnd_seed = abs(rnd_seed)

    rng = numpy.random.default_rng(rnd_seed)

    if rat_cls_name_col is not None:
        cls_names_arr = rsgislib.rastergis.get_column_data(input_img, rat_cls_name_col)
        n_cls = len(cls_names_arr)

    image_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    if image_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the input image file: '{}'".format(input_img)
        )
    image_band = image_ds.GetRasterBand(img_band)
    if image_band is None:
        raise rsgislib.RSGISPyException("The image band wasn't opened")

    img_data = image_band.ReadAsArray()
    image_ds = None

    img_data_smpls = img_data.flatten()
    if cls_no_data is not None:
        img_data_smpls = img_data_smpls[img_data_smpls != cls_no_data]
    n_pxls_tot = len(img_data_smpls)
    cls_unq_vals = numpy.unique(img_data_smpls)

    n_classes = len(cls_unq_vals)
    cls_count_out = dict()
    if eq_n_smpls:
        eq_n_smpls = int(numpy.ceil(n_smpls / n_classes))
        for cls_val in cls_unq_vals:
            cls_count_out[cls_val] = eq_n_smpls

    n_smpls_prop = n_smpls / n_pxls_tot
    cls_info_dict = dict()
    cls_info_dict["Class ID"] = list()
    cls_info_dict["N Image Pixels"] = list()
    cls_info_dict["N Ref Samples"] = list()
    cls_id_count = dict()
    n_out_smpls = 0
    for cls_val in cls_unq_vals:
        cls_id_count[cls_val] = len(img_data_smpls[img_data_smpls == cls_val])
        if eq_n_smpls:
            if cls_count_out[cls_val] > cls_id_count[cls_val]:
                cls_count_out[cls_val] = cls_id_count[cls_val]
        else:
            cls_count_out[cls_val] = int(
                numpy.ceil(cls_id_count[cls_val] * n_smpls_prop)
            )

        if min_cls_smpls is not None:
            if cls_count_out[cls_val] < min_cls_smpls:
                cls_count_out[cls_val] = min_cls_smpls

        cls_info_dict["Class ID"].append(cls_val)
        cls_info_dict["N Image Pixels"].append(cls_id_count[cls_val])
        cls_info_dict["N Ref Samples"].append(cls_count_out[cls_val])
        n_out_smpls += cls_count_out[cls_val]

    print("The number of output samples to be generated per-class:")
    try:
        import tabulate

        print(tabulate.tabulate(cls_info_dict, headers="keys", tablefmt="grid"))
    except ImportError:
        print("Class ID, N Pixels in Image, N Samples")
        for cls_val in cls_unq_vals:
            print(f"{cls_val}:{cls_id_count[cls_val]}:{cls_count_out[cls_val]}")
    print(f"In total {n_out_smpls} will be generated.")

    pxl_chk_arr = numpy.zeros_like(img_data, dtype=bool)

    x_coords_arr = numpy.zeros([n_out_smpls], dtype=float)
    y_coords_arr = numpy.zeros([n_out_smpls], dtype=float)
    cls_pxl_vals_arr = numpy.zeros([n_out_smpls], dtype=int)
    processed_col_arr = numpy.zeros([n_out_smpls], dtype=int)
    ref_cls_col_arr = numpy.zeros([n_out_smpls], dtype=int)

    img_cls_names = list()
    ref_cls_names = list()

    created_smpls_count = dict()
    for cls_val in cls_unq_vals:
        created_smpls_count[cls_val] = 0

    with tqdm.tqdm(total=n_out_smpls) as pbar:
        found_pts = 0
        while found_pts < n_out_smpls:
            x_coord = (rng.random() * x_range) + img_bbox[0]
            y_coord = (rng.random() * y_range) + img_bbox[2]

            x_pxl_loc = numpy.floor((x_coord - img_bbox[0]) / img_res_x).astype(int)
            y_pxl_loc = numpy.floor((img_bbox[3] - y_coord) / img_res_y).astype(int)
            if not pxl_chk_arr[y_pxl_loc, x_pxl_loc]:
                x_coord_pxl_grid = (
                    img_bbox[0] + (img_res_x * x_pxl_loc) + (img_res_x / 2)
                )
                y_coord_pxl_grid = (
                    img_bbox[3] - (img_res_y * y_pxl_loc) - (img_res_y / 2)
                )
                cls_pxl_val = img_data[y_pxl_loc, x_pxl_loc]

                if (cls_no_data is None) or (cls_pxl_val != cls_no_data):
                    if created_smpls_count[cls_pxl_val] < cls_count_out[cls_pxl_val]:
                        x_coords_arr[found_pts] = x_coord_pxl_grid
                        y_coords_arr[found_pts] = y_coord_pxl_grid
                        cls_pxl_vals_arr[found_pts] = cls_pxl_val
                        ref_cls_col_arr[found_pts] = cls_pxl_val
                        processed_col_arr[found_pts] = 0
                        if rat_cls_name_col is not None:
                            if cls_pxl_val < n_cls:
                                img_cls_names.append(
                                    cls_names_arr[cls_pxl_val].decode("utf-8")
                                )
                                ref_cls_names.append("")
                        created_smpls_count[cls_pxl_val] += 1
                        found_pts += 1
                        pbar.update(1)

    data_dict = dict()
    data_dict["tmp_x"] = x_coords_arr
    data_dict["tmp_y"] = y_coords_arr
    if rat_cls_name_col is not None:
        data_dict[img_cls_col] = img_cls_names
        data_dict[ref_cls_col] = ref_cls_names
    else:
        data_dict[img_cls_col] = cls_pxl_vals_arr
        data_dict[ref_cls_col] = ref_cls_col_arr
    data_dict[processed_col] = processed_col_arr

    data_df = pandas.DataFrame(data=data_dict)

    pts_gdf = geopandas.GeoDataFrame(
        data_df,
        geometry=geopandas.points_from_xy(data_df.tmp_x, data_df.tmp_y),
        crs=f"EPSG:{img_epsg}",
    )
    pts_gdf = pts_gdf.drop(columns=["tmp_x", "tmp_y"])

    if out_format == "GPKG":
        pts_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        pts_gdf.to_file(out_vec_file, driver=out_format)
