#!/usr/bin/env python
"""
A module to calculate roughness metrics for elevation data.
"""

from typing import Dict
import os
import rsgislib.imagecalc.calc_pt_win_smpls
import numpy
import math
from osgeo import gdal


class CalcProfileRoughMetrics(rsgislib.imagecalc.calc_pt_win_smpls.RSGISCalcSumVals):
    def __init__(
        self,
        detrend: bool = True,
        detrend_poly_order: int = 1,
        create_plots: bool = False,
        out_plot_path: str = None,
    ):
        """

        :param detrend: Boolean to specify whether to detrend the surface or not.
                        Default: True
        :param detrend_poly_order: the order of the polynomial used to detrend
                                   the surface.
        """
        super().__init__()

        self.detrend = detrend
        self.detrend_poly_order = detrend_poly_order
        self.create_plots = create_plots
        self.out_plot_path = out_plot_path
        self.out_val_names = [
            "x_ra",
            "y_ra",
            "sum_ra",
            "avg_ra",
            "x_rr",
            "y_rr",
            "sum_rr",
            "avg_rr",
            "x_rq",
            "y_rq",
            "sum_rq",
            "avg_rq",
            "x_MIF",
            "y_MIF",
            "sum_MIF",
            "avg_MIF",
            "x_rv",
            "y_rv",
            "sum_rv",
            "avg_rv",
            "x_rp",
            "y_rp",
            "sum_rp",
            "avg_rp",
            "x_rz",
            "y_rz",
            "sum_rz",
            "avg_rz",
            "x_rsk",
            "y_rsk",
            "sum_rsk",
            "avg_rsk",
            "x_rku",
            "y_rku",
            "sum_rku",
            "avg_rku",
        ]
        self.n_out_vals = len(self.out_val_names)

    def calcVals(self, smpl_idx: int, in_img_ds_obj: gdal.Dataset) -> Dict[str, float]:
        """
        A function to calculate roughness metrics in x and y axis of the
        window of data provided. The metrics calculated are:

            * Average roughness (RA): Average of profile height deviations from the mean
            * Random roughness (RR)
            * Quadratic mean roughness (RQ) - Sum of absolute slopes.
            * The microrelief index (MIF)
            * Maximum valley depth below the mean line (RV)
            * Maximum peak height above the mean line (RP)
            * Maximum peak to valley height of the profile (RZ)
            * Skewness, or measure of asymmetry of the profile about the mean line (RSK)
            * Kurtosis, or measure of peakedness (or tailedness) of the profile about the mean line. (RKU)

        See https://en.wikipedia.org/wiki/Surface_roughness for more information.

        :param smpl_idx: a unique index for the sample being processed.
        :param in_img_ds_obj: input GDAL dataset.
        :return: returns dict of value name and value.
        """
        import scipy.stats

        if self.create_plots:
            import matplotlib.pyplot as plt
            import matplotlib.gridspec as gridspec

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

        x_orig_arr = img_arr[mid_y, ...]
        y_orig_arr = img_arr[..., mid_x]

        x_loc_arr = numpy.arange(x_size) * x_res
        y_loc_arr = numpy.arange(y_size) * y_res

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

        # Calculate the mean of the line.
        x_mean = numpy.mean(x_arr)
        y_mean = numpy.mean(y_arr)

        ###############################################################
        # Calculate average roughness (RA): Average of profile
        # height deviations from the mean
        out_dict["x_ra"] = numpy.sum(numpy.abs(x_arr - x_mean)) / x_size
        out_dict["y_ra"] = numpy.sum(numpy.abs(y_arr - y_mean)) / y_size
        out_dict["sum_ra"] = out_dict["x_ra"] + out_dict["y_ra"]
        out_dict["avg_ra"] = out_dict["sum_ra"] / 2
        ###############################################################

        ###############################################################
        # Calculate random roughness (RR) (standard deviation of
        # the elevations from the mean surface) of a transect
        # Also referred to as the quadratic mean roughness (RQ):
        # or root mean square average of profile height deviations
        # from the mean line
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
        # x_arr_rqs = x_arr_dx / x_arr_dy
        x_arr_rqs_abs = numpy.abs(x_arr_dx)

        y_arr_dx = numpy.diff(y_arr)
        # y_arr_dy = numpy.diff(y_loc_arr)
        # y_arr_rqs = y_arr_dx / y_arr_dy
        y_arr_rqs_abs = numpy.abs(y_arr_dx)

        out_dict["x_rq"] = numpy.sum(x_arr_rqs_abs)
        out_dict["y_rq"] = numpy.sum(y_arr_rqs_abs)
        out_dict["sum_rq"] = out_dict["x_rq"] + out_dict["y_rq"]
        out_dict["avg_rq"] = out_dict["sum_rq"] / 2
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

        ###############################################################
        # Maximum valley depth below the mean line (RV)
        out_dict["x_rv"] = numpy.min(x_arr - x_mean)
        out_dict["y_rv"] = numpy.min(y_arr - y_mean)
        out_dict["sum_rv"] = out_dict["x_rv"] + out_dict["y_rv"]
        out_dict["avg_rv"] = out_dict["sum_rv"] / 2
        ###############################################################

        ###############################################################
        # Maximum peak height above the mean line (RP)
        out_dict["x_rp"] = numpy.max(x_arr - x_mean)
        out_dict["y_rp"] = numpy.max(y_arr - y_mean)
        out_dict["sum_rp"] = out_dict["x_rp"] + out_dict["y_rp"]
        out_dict["avg_rp"] = out_dict["sum_rp"] / 2
        ###############################################################

        ###############################################################
        # Maximum peak to valley height of the profile (RZ)
        out_dict["x_rz"] = out_dict["x_rp"] - out_dict["x_rv"]
        out_dict["y_rz"] = out_dict["y_rp"] - out_dict["y_rv"]
        out_dict["sum_rz"] = out_dict["x_rz"] + out_dict["y_rz"]
        out_dict["avg_rz"] = out_dict["sum_rz"] / 2
        ###############################################################

        ###############################################################
        # Skewness, or measure of asymmetry of the profile about the mean line (RSK)
        out_dict["x_rsk"] = scipy.stats.skew(x_arr - x_mean)
        out_dict["y_rsk"] = scipy.stats.skew(y_arr - y_mean)
        out_dict["sum_rsk"] = out_dict["x_rsk"] + out_dict["y_rsk"]
        out_dict["avg_rsk"] = out_dict["sum_rsk"] / 2
        ###############################################################

        ###############################################################
        # Kurtosis, or measure of peakedness (or tailedness) of the profile about the mean line (RKU)
        out_dict["x_rku"] = scipy.stats.kurtosis(x_arr - x_mean)
        out_dict["y_rku"] = scipy.stats.kurtosis(y_arr - y_mean)
        out_dict["sum_rku"] = out_dict["x_rku"] + out_dict["y_rku"]
        out_dict["avg_rku"] = out_dict["sum_rku"] / 2
        ###############################################################

        ###############################################################
        if self.create_plots:
            fig = plt.figure(figsize=(10, 5))
            gspec = gridspec.GridSpec(ncols=1, nrows=2, figure=fig)

            # Get the first axis
            ax1 = fig.add_subplot(gspec[0, 0])
            ax1.plot(x_loc_arr, x_arr)
            ax1.set_ylabel("Elevation (m)")
            ax1.set_title("X Axis Surface")

            # Get the second axis
            ax2 = fig.add_subplot(gspec[1, 0], sharex=ax1)
            ax2.plot(y_loc_arr, y_arr)
            ax2.set_ylabel("Distance (m)")
            ax2.set_ylabel("Elevation (m)")
            ax2.set_title("Y Axis Surface")

            plt.savefig(os.path.join(self.out_plot_path, f"profile_smp_{smpl_idx}.png"))
        ###############################################################

        return out_dict


class CalcMunroRoughnessMetric(rsgislib.imagecalc.calc_pt_win_smpls.RSGISCalcSumVals):
    def __init__(
        self,
        detrend=False,
        detrend_poly_order=1,
        create_plots: bool = False,
        out_plot_path: str = None,
    ):
        super().__init__()

        self.detrend = detrend
        self.detrend_poly_order = detrend_poly_order
        self.create_plots = create_plots
        self.out_plot_path = out_plot_path
        self.out_val_names = [
            "x_munro_peaks",
            "x_munro_area",
            "x_munro_density",
            "x_munro_z0",
            "y_munro_peaks",
            "y_munro_area",
            "y_munro_density",
            "y_munro_z0",
            "sum_munro_z0",
            "avg_munro_z0",
        ]
        self.n_out_vals = len(self.out_val_names)

    def calcVals(self, smpl_idx: int, in_img_ds_obj: gdal.Dataset) -> Dict[str, float]:
        if self.create_plots:
            import matplotlib.pyplot as plt
            import matplotlib.gridspec as gridspec

        out_dict = dict()
        in_band_obj = in_img_ds_obj.GetRasterBand(1)

        geotransform = in_img_ds_obj.GetGeoTransform()
        x_res = abs(geotransform[1])
        y_res = abs(geotransform[5])

        img_arr = in_band_obj.ReadAsArray()
        img_arr_shp = img_arr.shape

        y_size = img_arr_shp[0]
        x_size = img_arr_shp[1]

        x_profile_len = x_res * x_size
        y_profile_len = y_res * y_size

        mid_x = math.ceil(x_size / 2)
        mid_y = math.ceil(y_size / 2)

        x_orig_arr = img_arr[mid_y, ...]
        y_orig_arr = img_arr[..., mid_x]

        x_loc_arr = numpy.arange(x_size) * x_res
        y_loc_arr = numpy.arange(y_size) * y_res

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

        x_mean = numpy.mean(x_arr)
        y_mean = numpy.mean(y_arr)

        x_arr_0mean = x_arr - x_mean
        y_arr_0mean = y_arr - y_mean

        x_arr_base = x_arr_0mean[1:-1]
        x_diff_right = x_arr_0mean[:-2]
        x_diff_left = x_arr_0mean[2:]
        x_pos_cross = numpy.logical_and((x_diff_right < 0), (x_arr_base > 0))
        x_neg_cross = numpy.logical_and((x_arr_base > 0), (x_diff_left < 0))
        x_min_pt_locs = numpy.logical_and(
            (x_diff_right > x_arr_base), (x_diff_left > x_arr_base)
        )
        # Expand to original length
        x_min_pt_locs = numpy.concatenate(([True], x_min_pt_locs, [True]))
        x_pos_cross = numpy.concatenate(([False], x_pos_cross, [False]))
        x_neg_cross = numpy.concatenate(([False], x_neg_cross, [False]))

        x_n_peaks = numpy.max([numpy.sum(x_pos_cross), numpy.sum(x_neg_cross)])
        # h* is an effective height for the roughness elements,
        # from Munro 1989, h* can be  calculated as the standard deviation
        # of all the deviations from the detrended mean (i.e. residuals)
        # multiplied by two
        x_h_star = 2 * numpy.std(x_arr_0mean)
        # s is the silhouette area of roughness elements measured in a vertical plane
        x_little_s = (x_h_star * x_profile_len) / (2 * x_n_peaks)
        # S is their density, or frequency per unit area
        x_big_s = (x_profile_len / x_n_peaks) ** 2
        # z0 is the roughness length for wind speed
        x_munro_z0 = 0.5 * x_h_star * (x_little_s / x_big_s)

        y_arr_base = y_arr_0mean[1:-1]
        y_diff_right = y_arr_0mean[:-2]
        y_diff_left = y_arr_0mean[2:]
        y_pos_cross = numpy.logical_and((y_diff_right < 0), (y_arr_base > 0))
        y_neg_cross = numpy.logical_and((y_arr_base > 0), (y_diff_left < 0))
        y_min_pt_locs = numpy.logical_and(
            (y_diff_right > y_arr_base), (y_diff_left > y_arr_base)
        )
        # Expand to original length
        y_min_pt_locs = numpy.concatenate(([True], y_min_pt_locs, [True]))
        y_pos_cross = numpy.concatenate(([False], y_pos_cross, [False]))
        y_neg_cross = numpy.concatenate(([False], y_neg_cross, [False]))

        y_n_peaks = numpy.max([numpy.sum(y_pos_cross), numpy.sum(y_neg_cross)])
        # h* is an effective height for the roughness elements,
        # from Munro 1989, h* can be  calculated as the standard deviation
        # of all the deviations from the detrended mean (i.e. residuals)
        # multiplied by two
        y_h_star = 2 * numpy.std(y_arr_0mean)
        # s is the silhouette area of roughness elements measured in a vertical plane
        y_little_s = (y_h_star * y_profile_len) / (2 * y_n_peaks)
        # S is their density, or frequency per unit area
        y_big_s = (y_profile_len / y_n_peaks) ** 2
        # z0 is the roughness length for wind speed
        y_munro_z0 = 0.5 * y_h_star * (y_little_s / y_big_s)

        ###############################################################
        if self.create_plots:
            fig = plt.figure(figsize=(10, 5))
            gspec = gridspec.GridSpec(ncols=1, nrows=2, figure=fig)

            # Get the first axis
            ax1 = fig.add_subplot(gspec[0, 0])
            ax1.plot([x_loc_arr[0], x_loc_arr[-1]], [0.0, 0.0], color="black")
            ax1.plot(x_loc_arr, x_arr_0mean)
            ax1.scatter(x_loc_arr[x_pos_cross], x_arr_0mean[x_pos_cross])
            ax1.scatter(x_loc_arr[x_neg_cross], x_arr_0mean[x_neg_cross])
            ax1.plot(x_loc_arr[x_min_pt_locs], x_arr_0mean[x_min_pt_locs])
            ax1.set_ylabel("Elevation (m)")
            ax1.set_title("X Axis Surface")

            # Get the second axis
            ax2 = fig.add_subplot(gspec[1, 0], sharex=ax1)
            ax2.plot([y_loc_arr[0], y_loc_arr[-1]], [0.0, 0.0], color="black")
            ax2.plot(y_loc_arr, y_arr_0mean)
            ax2.scatter(y_loc_arr[y_pos_cross], y_arr_0mean[y_pos_cross])
            ax2.scatter(y_loc_arr[y_neg_cross], y_arr_0mean[y_neg_cross])
            ax2.plot(y_loc_arr[y_min_pt_locs], y_arr_0mean[y_min_pt_locs])
            ax2.set_ylabel("Distance (m)")
            ax2.set_ylabel("Elevation (m)")
            ax2.set_title("Y Axis Surface")

            plt.savefig(
                os.path.join(
                    self.out_plot_path, f"munro_rough_profile_smp_{smpl_idx}.png"
                )
            )

        out_dict["x_munro_peaks"] = x_n_peaks
        out_dict["x_munro_area"] = x_little_s
        out_dict["x_munro_density"] = x_big_s
        out_dict["x_munro_z0"] = x_munro_z0
        out_dict["y_munro_peaks"] = y_n_peaks
        out_dict["y_munro_area"] = y_little_s
        out_dict["y_munro_density"] = y_big_s
        out_dict["y_munro_z0"] = y_munro_z0
        out_dict["sum_munro_z0"] = out_dict["x_munro_z0"] + out_dict["y_munro_z0"]
        out_dict["avg_munro_z0"] = out_dict["sum_munro_z0"] / 2

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
    detrend_poly_order: int = 1,
    export_plot_path: str = None,
):
    """
    A function which uses the CalcProfileRoughMetrics class to calculate
    roughness metrics in x and y axis of the window of data provided.
    The metrics calculated are:

        * Average roughness (RA): Average of profile height deviations from the mean
        * Random roughness (RR)
        * Quadratic mean roughness (RQ) - Sum of absolute slopes.
        * The microrelief index (MIF)
        * Maximum valley depth below the mean line (RV)
        * Maximum peak height above the mean line (RP)
        * Maximum peak to valley height of the profile (RZ)
        * Skewness, or measure of asymmetry of the profile about the mean line (RSK)
        * Kurtosis, or measure of peakedness (or tailedness) of the profile about the mean line (RKU)

    See https://en.wikipedia.org/wiki/Surface_roughness for more information.

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
    :param export_plot_path: If a directory path is provided then plots will be
                             created for each of the sample windows. This is
                             expected to be a debugging and data exploration tool
                             as it significantly slows processing. If None (default)
                             then no plots will be exported.

    """
    create_plots = False
    if export_plot_path is not None:
        create_plots = True

    rough_profile_obj = CalcProfileRoughMetrics(
        detrend=detrend,
        detrend_poly_order=detrend_poly_order,
        create_plots=create_plots,
        out_plot_path=export_plot_path,
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


def calc_munro_roughness_profile_metrics(
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
    detrend_poly_order: int = 1,
    export_plot_path: str = None,
):
    """
    A function which uses the CalcMunroRoughnessMetric class to calculate
    the munro Z0 roughness metrics in x and y axis of the window of data provided.


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
    :param export_plot_path: If a directory path is provided then plots will be
                             created for each of the sample windows. This is
                             expected to be a debugging and data exploration tool
                             as it significantly slows processing. If None (default)
                             then no plots will be exported.

    """
    create_plots = False
    if export_plot_path is not None:
        create_plots = True

    rough_profile_obj = CalcMunroRoughnessMetric(
        detrend=detrend,
        detrend_poly_order=detrend_poly_order,
        create_plots=create_plots,
        out_plot_path=export_plot_path,
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


def calc_all_roughness_profile_metrics(
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
    detrend_poly_order: int = 1,
):
    """
    A function which uses the CalcProfileRoughMetrics and CalcMunroRoughnessMetric
    classes to calculate roughness metrics in x and y axis of the window of
    data provided.

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
    munro_profile_obj = CalcMunroRoughnessMetric(
        detrend=detrend, detrend_poly_order=detrend_poly_order
    )

    rsgislib.imagecalc.calc_pt_win_smpls.calc_pt_smpl_img_vals(
        input_img,
        vec_file,
        vec_lyr,
        calc_objs=[rough_profile_obj, munro_profile_obj],
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format=out_format,
        interp_method=interp_method,
        angle_col=angle_col,
        x_box_col=x_box_col,
        y_box_col=y_box_col,
        no_data_val=no_data_val,
    )
