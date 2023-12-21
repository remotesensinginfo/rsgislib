#!/usr/bin/env python
"""
A module to calculate roughness metrics for elevation data.
"""

from typing import Dict
import rsgislib.imagecalc.calc_pt_win_smpls
import numpy
import math
from osgeo import gdal


class CalcProfileRoughMetrics(rsgislib.imagecalc.calc_pt_win_smpls.RSGISCalcSumVals):
    def __init__(self, detrend: bool = True, detrend_poly_order: int = 3):
        """

        :param detrend: Boolean to specify whether to detrend the surface or not.
                        Default: True
        :param detrend_poly_order: the order of the polynomial used to detrend
                                   the surface.
        """
        super().__init__()

        self.detrend = detrend
        self.detrend_poly_order = detrend_poly_order
        self.out_val_names = [
            "x_rr",
            "y_rr",
            "sum_rr",
            "avg_rr",
            "x_slp",
            "y_slp",
            "sum_slp",
            "avg_slp",
            "x_MIF",
            "y_MIF",
            "sum_MIF",
            "avg_MIF",
        ]
        self.n_out_vals = len(self.out_val_names)

    def calcVals(self, smpl_idx: int, in_img_ds_obj: gdal.Dataset) -> Dict[str, float]:
        """
        A function to calculate roughness metrics in x and y axis of the
        window of data provided. The metrics calculated are:

            * Random roughness (RR)
            * Sum of absolute slopes of a transect (SLP)
            * The microrelief index (MIF)

        :param smpl_idx: a unique index for the sample being processed.
        :param in_img_ds_obj: input GDAL dataset.
        :return: returns dict of value name and value.
        """
        out_dict = dict()
        in_band_obj = in_img_ds_obj.GetRasterBand(1)

        geotransform = in_img_ds_obj.GetGeoTransform()
        x_res = abs(geotransform[1])
        y_res = abs(geotransform[5])

        img_arr = in_band_obj.ReadAsArray()
        img_arr_shp = img_arr.shape

        y_size = img_arr_shp[0]
        x_size = img_arr_shp[1]

        mid_x = math.ceil(x_size / 2)
        mid_y = math.ceil(y_size / 2)

        x_orig_arr = img_arr[..., mid_x]
        y_orig_arr = img_arr[mid_y, ...]

        x_loc_arr = numpy.arange(y_size) * x_res
        y_loc_arr = numpy.arange(x_size) * y_res

        if self.detrend:
            x_poly_coef = numpy.polyfit(x_loc_arr, x_orig_arr, self.detrend_poly_order)
            y_poly_coef = numpy.polyfit(y_loc_arr, y_orig_arr, self.detrend_poly_order)

            x_pred_arr = numpy.polyval(x_poly_coef, x_loc_arr)
            y_pred_arr = numpy.polyval(y_poly_coef, y_loc_arr)

            x_arr = x_orig_arr - x_pred_arr
            y_arr = y_orig_arr - y_pred_arr
        else:
            x_arr = x_orig_arr
            y_arr = y_orig_arr

        ###############################################################
        # Calculate random roughness (RR) (standard deviation of
        # the elevations from the mean surface) of a transect
        x_mean = numpy.mean(x_arr)
        y_mean = numpy.mean(y_arr)

        out_dict["x_rr"] = numpy.std(x_arr - x_mean)
        out_dict["y_rr"] = numpy.std(y_arr - y_mean)
        out_dict["sum_rr"] = out_dict["x_rr"] + out_dict["y_rr"]
        out_dict["avg_rr"] = out_dict["sum_rr"] / 2
        ###############################################################

        ###############################################################
        # Calculate sum of absolute slopes of a transect (RM)
        # PB Comment: Really this is the sum of the elevation changes.
        x_arr_dx = numpy.diff(x_arr)
        # x_arr_dy = numpy.diff(x_loc_arr)
        # x_arr_slps = x_arr_dx / x_arr_dy
        x_arr_slps_abs = numpy.abs(x_arr_dx)

        y_arr_dx = numpy.diff(y_arr)
        # y_arr_dy = numpy.diff(y_loc_arr)
        # y_arr_slps = y_arr_dx / y_arr_dy
        y_arr_slps_abs = numpy.abs(y_arr_dx)

        out_dict["x_slp"] = numpy.sum(x_arr_slps_abs)
        out_dict["y_slp"] = numpy.sum(y_arr_slps_abs)
        out_dict["sum_slp"] = out_dict["x_slp"] + out_dict["y_slp"]
        out_dict["avg_slp"] = out_dict["sum_slp"] / 2
        ###############################################################

        ###############################################################
        # The product of the microrelief index (MIF) (mean absolute
        # deviation of elevation from a reference plane) and the peak
        # frequency (number of elevation peaks per unit transect length)
        x_arr_base = x_arr[0:-2]
        x_diff_p1 = x_arr[1:-1] - x_arr_base
        x_diff_p2 = x_arr[2:] - x_arr_base
        x_mif_pxls = numpy.logical_and((x_diff_p1 > 0), (x_diff_p2 > 0))

        y_arr_base = y_arr[0:-2]
        y_diff_p1 = y_arr[1:-1] - y_arr_base
        y_diff_p2 = y_arr[2:] - y_arr_base
        y_mif_pxls = numpy.logical_and((y_diff_p1 > 0), (y_diff_p2 > 0))

        out_dict["x_MIF"] = numpy.sum(x_mif_pxls)
        out_dict["y_MIF"] = numpy.sum(y_mif_pxls)
        out_dict["sum_MIF"] = out_dict["x_MIF"] + out_dict["y_MIF"]
        out_dict["avg_MIF"] = out_dict["sum_MIF"] / 2
        ###############################################################

        return out_dict


def calc_simple_roughness_profile_metrics(
    input_img: str,
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    interp_method: int = rsgislib.INTERP_CUBIC,
    angle_col: str = None,
    x_box_col: str = "xbox",
    y_box_col: str = "ybox",
    no_data_val: float = None,
    detrend: bool = True,
    detrend_poly_order: int = 3,
):
    """
    A function which uses the CalcProfileRoughMetrics class to calculate
    roughness metrics in x and y axis of the window of data provided.
    The metrics calculated are:

            * Random roughness (RR)
            * Sum of absolute slopes of a transect (SLP)
            * The microrelief index (MIF)

    :param input_img: input image file.
    :param vec_file: input vector file - needs to be a point type.
    :param vec_lyr: input vector layer name.
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
    :param detrend: Boolean to specify whether to detrend the surface or not.
                    Default: True
    :param detrend_poly_order: the order of the polynomial used to detrend
                               the surface.

    """
    rough_profile_obj = CalcProfileRoughMetrics(
        detrend=detrend, detrend_poly_order=detrend_poly_order
    )
    rsgislib.imagecalc.calc_pt_win_smpls.calc_pt_smpl_img_vals(
        input_img,
        vec_file,
        vec_lyr,
        calc_objs=[rough_profile_obj],
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format=out_format,
        interp_method=interp_method,
        angle_col=angle_col,
        x_box_col=x_box_col,
        y_box_col=y_box_col,
        no_data_val=no_data_val,
    )
