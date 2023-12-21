#! /usr/bin/env python
############################################################################
#  calc_pt_win_smpls.py
#
#  Copyright 2023 RSGISLib.
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
# Purpose:  Provides functions and class to calculate image summary values
#           for a set of vector points (e.g., DEM roughness metrics).
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 19/11/2023
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

from typing import List, Dict
from abc import ABCMeta, abstractmethod
import math
import os

import rsgislib
import rsgislib.imageutils
import rsgislib.tools.geometrytools
import rsgislib.tools.affine

from osgeo import gdal
import tqdm


class RSGISCalcSumVals:
    """
    Abstract class for calculating summary values for a GDAL image Dataset.
    Provided to calc_pt_smpl_img_vals function.

    :param self.n_out_vals: the number of output values. Length of list of names.
    :param self.out_val_names: list of output names. Defined in implementation.
    """

    __metaclass__ = ABCMeta

    def __init__(self):
        self.n_out_vals = 1
        self.out_val_names = ["value"]

    @abstractmethod
    def calcVals(self, smpl_idx: int, in_img_ds_obj: gdal.Dataset) -> Dict[str, float]:
        """
        Abstract function to calculate summary values

        :param smpl_idx: a unique index for the sample being processed.
        :param in_img_ds_obj: input GDAL dataset.
        :return: returns dict of value name and value.
        """
        pass

    def getNOutVals(self) -> int:
        """
        :return: return the number of output values
        """
        return self.n_out_vals

    def getOutValNames(self) -> List[str]:
        """
        :return: return the list of output names
        """
        return self.out_val_names


def calc_pt_smpl_img_vals(
    input_img: str,
    vec_file: str,
    vec_lyr: str,
    calc_objs: List[RSGISCalcSumVals],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str,
    interp_method: int = rsgislib.INTERP_CUBIC,
    angle_col: str = None,
    x_box_col: str = "xbox",
    y_box_col: str = "ybox",
    no_data_val: float = None,
):
    """
    A function which calculates, for each point within the input vector layer,
    summary values for a window of image data around the point. The image window
    size is defined in the attribute table of the vector layer and optionally an
    angle (relative to north; 0.0; default) to reorientate image window. The
    summary values calculated are using an implementation of the RSGISCalcSumVals
    class.

    :param input_img: input image file.
    :param vec_file: input vector file - needs to be a point type.
    :param vec_lyr: input vector layer name.
    :param calc_objs: An implementation of the RSGISCalcSumVals class to calculate
                     the summary values for the image data.
    :param out_vec_file: output vector file path.
    :param out_vec_lyr: output vector layer name.
    :param out_format: output vector file format (e.g., GeoJSON)
    :param interp_method: the interpolation method used when reorientating the image
                          data. Default: rsgislib.INTERP_CUBIC
    :param angle_col: name of the column within the vector attribute table defining the
                      rotation (relative to north; 0 = North) for each point. If
                      None (Default) then no rotation applied. (Unit is degrees)
    :param x_box_col: name of the column within the vector attribute table defining
                      the size of the bbox in the x axis. Note, this is half the bbox
                      width. (Unit is image pixels)
    :param y_box_col: name of the column within the vector attribute table defining
                      the size of the bbox in the y axis. Note, this is half the bbox
                      height. (Unit is image pixels)
    :param no_data_val: the image no data value. If None then taken from the input
                        image header.

    """
    import geopandas

    img_x_res, img_y_res = rsgislib.imageutils.get_img_res(input_img)
    img_y_res_abs = math.fabs(img_y_res)
    # img_x_size, img_y_size = rsgislib.imageutils.get_img_size(input_img)
    img_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    # img_rsgislib_dtype = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
    img_gdal_dtype = rsgislib.imageutils.get_gdal_datatype_from_img(input_img)
    img_wkt_str = rsgislib.imageutils.get_wkt_proj_from_img(input_img)
    img_n_bands = rsgislib.imageutils.get_img_band_count(input_img)

    out_col_names = list()
    for calc_obj in calc_objs:
        out_col_names_tmp = calc_obj.getOutValNames()
        out_vals_dict = dict()
        for out_col in out_col_names_tmp:
            out_vals_dict[out_col] = []
        out_col_names.extend(out_col_names_tmp)

    gdal_interp_method = rsgislib.get_gdal_interp_type(interp_method)

    gdal_driver = gdal.GetDriverByName("MEM")

    pts_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    n_pt_smpls = len(pts_gdf)
    for i, row in tqdm.tqdm(pts_gdf.iterrows(), total=n_pt_smpls):
        #####################################
        # Get Info From Vector Row...
        pt = row["geometry"]
        pt_lst = [pt.x, pt.y]
        # Put point on to the raster grid of the image.
        pt_grid = rsgislib.tools.geometrytools.find_point_on_whole_num_grid(
            pt_lst, img_bbox, img_x_res, img_y_res_abs
        )

        if angle_col is not None:
            pt_angle = row[angle_col]
        else:
            pt_angle = 0.0

        if pt_angle < 0.0:
            raise rsgislib.RSGISPyException("The angle needs to be > 0.0.")

        pt_xbox = row[x_box_col]
        pt_ybox = row[y_box_col]
        # print(f"{pt_grid}: {pt_angle} - ({pt_xbox} x {pt_ybox})")
        #####################################
        #####################################
        # Define the BBOX of the ROI
        img_xbox = pt_xbox * img_x_res
        img_ybox = pt_ybox * img_y_res_abs

        img_max_box = img_xbox
        if img_ybox > img_max_box:
            img_max_box = img_ybox

        pt_x_min = pt_grid[0] - img_xbox
        pt_x_max = pt_grid[0] + img_xbox
        pt_y_min = pt_grid[1] - img_ybox
        pt_y_max = pt_grid[1] + img_ybox

        pt_img_bbox = [pt_x_min, pt_x_max, pt_y_min, pt_y_max]
        # print(pt_img_bbox)
        #####################################

        #####################################
        # Buffered the BBOX so extract larger area to remove image artifacts.
        pt_img_buf_bbox = rsgislib.tools.geometrytools.buffer_bbox(
            pt_img_bbox, buf=(img_max_box * 2)
        )
        # print(pt_img_buf_bbox)

        # Make sure BBOX is within the image bounds.
        pt_img_bound_bbox = rsgislib.tools.geometrytools.bbox_intersection(
            pt_img_bbox, img_bbox
        )
        # print(pt_img_bound_bbox)
        pt_img_bound_buf_bbox = rsgislib.tools.geometrytools.bbox_intersection(
            pt_img_buf_bbox, img_bbox
        )
        # print(pt_img_bound_buf_bbox)
        #####################################

        # Extract the image subset from the whole image as an in-memory GDAL dataset
        pt_img_ds_obj = rsgislib.imageutils.create_inmem_gdal_ds_subset(
            input_img, pt_img_bound_buf_bbox, no_data_val=no_data_val
        )

        ###########################################
        # Apply Rotation to the in memory image:
        pt_img_width, pt_img_height = (
            pt_img_ds_obj.RasterXSize,
            pt_img_ds_obj.RasterYSize,
        )

        pt_img_pxl_x_centre = pt_img_width / 2
        pt_img_pxl_y_centre = pt_img_height / 2

        pt_img_src_transform = pt_img_ds_obj.GetGeoTransform()

        pt_img_affine_src = rsgislib.tools.affine.Affine.from_gdal(
            *pt_img_src_transform
        )
        pt_img_affine_dst = pt_img_affine_src * pt_img_affine_src.rotation(
            pt_angle, pivot=(pt_img_pxl_x_centre, pt_img_pxl_y_centre)
        )
        pt_img_dst_transform = pt_img_affine_dst.to_gdal()

        pt_img_ds_obj.SetGeoTransform(pt_img_dst_transform)
        ###########################################

        ###########################################
        # Create the ROI image
        roi_x_coords = pt_img_bound_bbox[1] - pt_img_bound_bbox[0]
        roi_y_coords = pt_img_bound_bbox[3] - pt_img_bound_bbox[2]

        roi_width = int(math.ceil((roi_x_coords / img_x_res)+0.5))
        roi_height = int(math.ceil((roi_y_coords / img_y_res_abs)+0.5))

        roi_img_ds_obj = gdal_driver.Create(
            "MEM", roi_width, roi_height, img_n_bands, img_gdal_dtype
        )
        roi_img_ds_obj.SetGeoTransform(
            (pt_img_bound_bbox[0], img_x_res, 0, pt_img_bound_bbox[3], 0, img_y_res)
        )
        roi_img_ds_obj.SetProjection(img_wkt_str)
        ##########################################

        ##########################################
        # Warp the image data to the ROI - applying the rotation...
        wrp_opts = gdal.WarpOptions(
            resampleAlg=gdal_interp_method,
            srcNodata=no_data_val,
            dstNodata=no_data_val,
            multithread=False,
            callback=None,
        )
        gdal.Warp(roi_img_ds_obj, pt_img_ds_obj, options=wrp_opts)
        ##########################################

        for calc_obj in calc_objs:
            out_col_names_tmp = calc_obj.getOutValNames()
            rtn_vals = calc_obj.calcVals(i, roi_img_ds_obj)
            for out_col in out_col_names_tmp:
                out_vals_dict[out_col].append(rtn_vals[out_col])

        pt_img_ds_obj = None
        roi_img_ds_obj = None

    for out_col in out_col_names:
        pts_gdf[out_col] = out_vals_dict[out_col]

    if out_format == "GPKG":
        pts_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        pts_gdf.to_file(out_vec_file, driver=out_format)


class RSGISDebugExportImg(RSGISCalcSumVals):

    """
    Debug class which exports the data to an out image.
    """

    def __init__(self, out_path: str, img_name: str):
        super().__init__()
        self.n_out_vals = 1
        self.out_val_names = ["debug"]
        self.out_path = out_path
        self.img_name = img_name

    def calcVals(self, smpl_idx: int, in_img_ds_obj: gdal.Dataset) -> Dict[str, float]:
        """
        Abstract function to calculate summary values

        :param smpl_idx: a unique index for the sample being processed.
        :param in_img_ds_obj: input GDAL dataset.
        :return: returns dict of value name and value.
        """
        out_img = os.path.join(self.out_path, f"{smpl_idx}_{self.img_name}.tif")
        dset_tiff_out = gdal.GetDriverByName("GTiff")
        dset_tiff_out.CreateCopy(out_img, in_img_ds_obj)
        return {"debug": 0.0}
