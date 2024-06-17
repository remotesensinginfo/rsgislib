#! /usr/bin/env python
############################################################################
#  pxloutlierchng.py
#
#  Copyright 2019 RSGISLib.
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
# Purpose:  Provide a set of utilities to perform per-pixel change detection
#           using outlier methods. These are sometime called map-to-image
#           methods of change detection.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 12/06/2019
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

from __future__ import print_function

import numpy
from rios import applier

import rsgislib
import rsgislib.imagecalc
import rsgislib.imageutils
import rsgislib.rastergis

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False


def find_class_pyod_outliers(
    pyod_obj,
    input_img,
    in_msk_img,
    output_img,
    out_scores_img=None,
    img_mask_val=1,
    img_bands=None,
    gdalformat="KEA",
):
    """
    This function uses the pyod (https://github.com/yzhao062/pyod) library to
    find outliers within a class. It is assumed that the input images are from a
    different date than the mask (classification) and therefore the outliers will
    be related to class changes.

    :param pyod_obj: an instance of a pyod.models (e.g., pyod.models.knn.KNN) pass
                     parameters to the constructor
    :param input_img: input image used for analysis
    :param in_msk_img: input image mask use to define the region of interest.
    :param output_img: output image with pixel over of 1 for within mask but
                         not outlier and 2 for in mask and outlier.
    :param out_scores_img: output image (optional, None and won't be provided;
                           Default None) providing the probability of
                           each pixel being an outlier
    :param img_mask_val: the pixel value within the mask image for the
                         class of interest. (Default 1)
    :param img_bands: the image bands to be used for the analysis.
                      If None then all used (Default: None)
    :param gdalformat: file format for the output image(s). Default KEA.

    """
    if img_bands is not None:
        if not (isinstance(img_bands, list) or isinstance(img_bands, tuple)):
            raise rsgislib.RSGISPyException(
                "If provided then img_bands should be a list (or None)"
            )
    else:
        n_bands = rsgislib.imageutils.get_img_band_count(input_img)
        img_bands = numpy.arange(1, n_bands + 1)

    img_val_no_data = rsgislib.imageutils.get_img_no_data_value(input_img)

    msk_arr_vals = rsgislib.imageutils.extract_img_pxl_vals_in_msk(
        input_img, img_bands, in_msk_img, img_mask_val, img_val_no_data
    )
    print("There were {} pixels within the mask.".format(msk_arr_vals.shape[0]))

    print("Fitting outlier detector")
    pyod_obj.fit(msk_arr_vals)
    print("Fitted outlier detector")

    # RIOS function to apply classifier
    def _applyPyOB(info, inputs, outputs, otherargs):
        # Internal function for rios applier. Used within find_class_outliers.

        out_lbls_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.uint8)
        if otherargs.out_scores:
            out_scores_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.float32)
        if numpy.any(inputs.image_mask == otherargs.msk_val):
            out_lbls_vals = out_lbls_vals.flatten()
            img_msk_vals = inputs.image_mask.flatten()
            if otherargs.out_scores:
                out_scores_vals = out_scores_vals.flatten()
            ID = numpy.arange(img_msk_vals.shape[0])

            img_shape = inputs.input_img.shape
            img_bands = inputs.input_img.reshape(
                (img_shape[0], (img_shape[1] * img_shape[2]))
            )

            band_lst = []
            for band in otherargs.img_bands:
                if (band > 0) and (band <= img_shape[0]):
                    band_lst.append(img_bands[band - 1])
                else:
                    raise rsgislib.RSGISPyException(
                        "Band ({}) specified is not within the image".format(band)
                    )
            img_bands_sel = numpy.stack(band_lst, axis=0)
            img_bands_trans = numpy.transpose(img_bands_sel)

            if otherargs.no_data_val is not None:
                ID = ID[(img_bands_trans != otherargs.no_data_val).all(axis=1)]
                img_msk_vals = img_msk_vals[
                    (img_bands_trans != otherargs.no_data_val).all(axis=1)
                ]
                img_bands_trans = img_bands_trans[
                    (img_bands_trans != otherargs.no_data_val).all(axis=1)
                ]

            ID = ID[img_msk_vals == otherargs.msk_val]
            img_bands_trans = img_bands_trans[img_msk_vals == otherargs.msk_val]

            if img_bands_trans.shape[0] > 0:
                pred_lbls = otherargs.pyod_obj.predict(img_bands_trans)
                pred_lbls = pred_lbls + 1
                out_lbls_vals[ID] = pred_lbls
            out_lbls_vals = numpy.expand_dims(
                out_lbls_vals.reshape(
                    (inputs.image_mask.shape[1], inputs.image_mask.shape[2])
                ),
                axis=0,
            )

            if otherargs.out_scores:
                if img_bands_trans.shape[0] > 0:
                    pred_probs = otherargs.pyod_obj.predict_proba(
                        img_bands_trans, method="unify"
                    )
                    out_scores_vals[ID] = pred_probs[:, 1]
                out_scores_vals = numpy.expand_dims(
                    out_scores_vals.reshape(
                        (inputs.image_mask.shape[1], inputs.image_mask.shape[2])
                    ),
                    axis=0,
                )

        outputs.out_lbls_img = out_lbls_vals
        if otherargs.out_scores:
            outputs.out_scores_img = out_scores_vals

    infiles = applier.FilenameAssociations()
    infiles.image_mask = in_msk_img
    infiles.input_img = input_img

    otherargs = applier.OtherInputs()
    otherargs.pyod_obj = pyod_obj
    otherargs.msk_val = img_mask_val
    otherargs.img_bands = img_bands
    otherargs.out_scores = False
    otherargs.no_data_val = img_val_no_data

    outfiles = applier.FilenameAssociations()
    outfiles.out_lbls_img = output_img
    if out_scores_img is not None:
        outfiles.out_scores_img = out_scores_img
        otherargs.out_scores = True

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Outlier Detector")
    applier.apply(_applyPyOB, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")

    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
        class_info_dict = dict()
        class_info_dict[1] = {"classname": "no_chng", "red": 0, "green": 255, "blue": 0}
        class_info_dict[2] = {"classname": "chng", "red": 255, "green": 0, "blue": 0}
        rsgislib.rastergis.set_class_names_colours(
            output_img, "chng_cls", class_info_dict
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            output_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if out_scores_img is not None:
        rsgislib.imageutils.pop_img_stats(
            out_scores_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )


def find_class_kurt_skew_outliers(
    input_img: str,
    in_msk_img: str,
    output_img: str,
    vld_min: float,
    vld_max: float,
    init_thres: float,
    low_thres: bool,
    contamination: float = 10.0,
    only_kurtosis: bool = False,
    img_mask_val: int = 1,
    img_band: int = 1,
    img_val_no_data: float = None,
    gdalformat: str = "KEA",
    plot_thres_file: str = None,
) -> float:
    """
    This function to find outliers within a class using an optimisation of the skewness
    and kurtosis. It is assumed that the input_img is from a different date than
    the mask (classification) and therefore the outliers will related to class changes.
    The skewness and kurtosis method assume that without change image values will
    be normally distributed.

    :param input_img: the input image for the analysis. Just a single band will be used.
    :param in_msk_img: input image mask use to define the region (class) of interest.
    :param output_img:  output image with pixel over of 1 for within mask but
                         not outlier and 2 for in mask and outlier.
    :param vld_min: the minimum threshold for valid image values.
    :param vld_max: the maximum threshold for valid image values.
    :param init_thres: an initial estimate of the change threshold
    :param low_thres: a boolean as to whether the threshold is on the upper or lower
                      side of the histogram. If True (default) then outliers will be
                      identified as values below the threshold. If False then outliers
                      will be above the threshold.
    :param contamination: An estimate of the amount of contamination (i.e., outliners)
                          which is within the scene.
    :param only_kurtosis: A boolean to specify that only the kurtosis should be used
                          to estimate the threshold. Default: False (i.e., both the
                        kurtosis and skewness are used.
    :param img_mask_val: the pixel value within the in_msk_img specifying the class
                         of interest.
    :param img_band: the input_img image band to be used for the analysis.
    :param img_val_no_data: the input_img image not data value. If None then the value
                            will be read from the image header.
    :param gdalformat: the output image file format. (Default: KEA)
    :param plot_thres_file: A file path for a plot of the histogram with the
                            threshold. If None then ignored.
    :return: The threshold identified.

    """
    import rsgislib.tools.stats

    if img_val_no_data is None:
        img_val_no_data = rsgislib.imageutils.get_img_no_data_value(input_img)

    msk_arr_vals = rsgislib.imageutils.extract_img_pxl_vals_in_msk(
        input_img, [img_band], in_msk_img, img_mask_val, img_val_no_data
    )
    print("There were {} pixels within the mask.".format(msk_arr_vals.shape[0]))

    chng_thres = rsgislib.tools.stats.calc_kurt_skew_threshold(
        msk_arr_vals[..., 0],
        vld_max,
        vld_min,
        init_thres,
        low_thres,
        contamination,
        only_kurtosis,
    )

    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("msk", in_msk_img, 1))
    band_defns.append(rsgislib.imagecalc.BandDefn("val", input_img, img_band))
    if low_thres:
        exp = (
            f"(val=={img_val_no_data})?0:(msk=={img_mask_val})&&"
            f"(val<{chng_thres})?2:(msk=={img_mask_val})?1:0"
        )
    else:
        exp = (
            f"(val=={img_val_no_data})?0:(msk=={img_mask_val})&&"
            f"(val>{chng_thres})?2:(msk=={img_mask_val})?1:0"
        )
    rsgislib.imagecalc.band_math(
        output_img, exp, gdalformat, rsgislib.TYPE_8UINT, band_defns
    )

    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
        class_info_dict = dict()
        class_info_dict[1] = {"classname": "no_chng", "red": 0, "green": 255, "blue": 0}
        class_info_dict[2] = {"classname": "chng", "red": 255, "green": 0, "blue": 0}
        rsgislib.rastergis.set_class_names_colours(
            output_img, "chng_cls", class_info_dict
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            output_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if plot_thres_file is not None:
        import rsgislib.tools.plotting

        rsgislib.tools.plotting.plot_histogram_threshold(
            msk_arr_vals[..., 0], plot_thres_file, chng_thres
        )

    return chng_thres


def find_class_otsu_outliers(
    input_img: str,
    in_msk_img: str,
    output_img: str,
    low_thres: bool,
    img_mask_val: int = 1,
    img_band: int = 1,
    img_val_no_data: float = None,
    gdalformat: str = "KEA",
    plot_thres_file: str = None,
) -> float:
    """
    This function to find outliers within a class using an otsu thresholding. It is
    assumed that the input_img is from a different date than the mask
    (classification) and therefore the outliers will related to class changes.

    :param input_img: the input image for the analysis. Just a single band will be used.
    :param in_msk_img: input image mask use to define the region (class) of interest.
    :param output_img:  output image with pixel over of 1 for within mask but
                         not outlier and 2 for in mask and outlier.
    :param low_thres: a boolean as to whether the threshold is on the upper or lower
                      side of the histogram. If True (default) then outliers will be
                      identified as values below the threshold. If False then outliers
                      will be above the threshold.
    :param img_mask_val: the pixel value within the in_msk_img specifying the class
                         of interest.
    :param img_band: the input_img image band to be used for the analysis.
    :param img_val_no_data: the input_img image not data value. If None then the value
                            will be read from the image header.
    :param gdalformat: the output image file format. (Default: KEA)
    :param plot_thres_file: A file path for a plot of the histogram with the
                            threshold. If None then ignored.
    :return: The threshold identified.

    """
    import rsgislib.tools.stats

    if img_val_no_data is None:
        img_val_no_data = rsgislib.imageutils.get_img_no_data_value(input_img)

    msk_arr_vals = rsgislib.imageutils.extract_img_pxl_vals_in_msk(
        input_img, [img_band], in_msk_img, img_mask_val, img_val_no_data
    )
    print("There were {} pixels within the mask.".format(msk_arr_vals.shape[0]))

    chng_thres = rsgislib.tools.stats.calc_otsu_threshold(msk_arr_vals[..., 0])

    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("msk", in_msk_img, 1))
    band_defns.append(rsgislib.imagecalc.BandDefn("val", input_img, img_band))
    if low_thres:
        exp = (
            f"(val=={img_val_no_data})?0:(msk=={img_mask_val})&&"
            f"(val<{chng_thres})?2:(msk=={img_mask_val})?1:0"
        )
    else:
        exp = (
            f"(val=={img_val_no_data})?0:(msk=={img_mask_val})&&"
            f"(val>{chng_thres})?2:(msk=={img_mask_val})?1:0"
        )
    rsgislib.imagecalc.band_math(
        output_img, exp, gdalformat, rsgislib.TYPE_8UINT, band_defns
    )

    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
        class_info_dict = dict()
        class_info_dict[1] = {"classname": "no_chng", "red": 0, "green": 255, "blue": 0}
        class_info_dict[2] = {"classname": "chng", "red": 255, "green": 0, "blue": 0}
        rsgislib.rastergis.set_class_names_colours(
            output_img, "chng_cls", class_info_dict
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            output_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if plot_thres_file is not None:
        import rsgislib.tools.plotting

        rsgislib.tools.plotting.plot_histogram_threshold(
            msk_arr_vals[..., 0], plot_thres_file, chng_thres
        )

    return chng_thres


def find_class_li_outliers(
    input_img: str,
    in_msk_img: str,
    output_img: str,
    low_thres: bool,
    tolerance: float = None,
    init_thres: float = None,
    img_mask_val: int = 1,
    img_band: int = 1,
    img_val_no_data: float = None,
    gdalformat: str = "KEA",
    plot_thres_file: str = None,
) -> float:
    """
    This function to find outliers within a class using Li's iterative Minimum Cross
    Entropy method. It is assumed that the input_img is from a different date than
    the mask (classification) and therefore the outliers will related to class changes.

    :param input_img: the input image for the analysis. Just a single band will be used.
    :param in_msk_img: input image mask use to define the region (class) of interest.
    :param output_img:  output image with pixel over of 1 for within mask but
                         not outlier and 2 for in mask and outlier.
    :param low_thres: a boolean as to whether the threshold is on the upper or lower
                      side of the histogram. If True (default) then outliers will be
                      identified as values below the threshold. If False then outliers
                      will be above the threshold.
    :param tolerance: float (optional) - Finish the computation when the
                      change in the threshold in an iteration is less than
                      this value. By default, this is half the smallest
                      difference between data values.
    :param init_thres: an initial estimate of the change threshold
    :param img_mask_val: the pixel value within the in_msk_img specifying the class
                         of interest.
    :param img_band: the input_img image band to be used for the analysis.
    :param img_val_no_data: the input_img image not data value. If None then the value
                            will be read from the image header.
    :param gdalformat: the output image file format. (Default: KEA)
    :param plot_thres_file: A file path for a plot of the histogram with the
                            threshold. If None then ignored.
    :return: The threshold identified.

    """
    import rsgislib.tools.stats

    if img_val_no_data is None:
        img_val_no_data = rsgislib.imageutils.get_img_no_data_value(input_img)

    msk_arr_vals = rsgislib.imageutils.extract_img_pxl_vals_in_msk(
        input_img, [img_band], in_msk_img, img_mask_val, img_val_no_data
    )
    print("There were {} pixels within the mask.".format(msk_arr_vals.shape[0]))

    chng_thres = rsgislib.tools.stats.calc_li_threshold(
        msk_arr_vals[..., 0],
        tolerance,
        init_thres,
    )

    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("msk", in_msk_img, 1))
    band_defns.append(rsgislib.imagecalc.BandDefn("val", input_img, img_band))
    if low_thres:
        exp = (
            f"(val=={img_val_no_data})?0:(msk=={img_mask_val})&&"
            f"(val<{chng_thres})?2:(msk=={img_mask_val})?1:0"
        )
    else:
        exp = (
            f"(val=={img_val_no_data})?0:(msk=={img_mask_val})&&"
            f"(val>{chng_thres})?2:(msk=={img_mask_val})?1:0"
        )
    rsgislib.imagecalc.band_math(
        output_img, exp, gdalformat, rsgislib.TYPE_8UINT, band_defns
    )

    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
        class_info_dict = dict()
        class_info_dict[1] = {"classname": "no_chng", "red": 0, "green": 255, "blue": 0}
        class_info_dict[2] = {"classname": "chng", "red": 255, "green": 0, "blue": 0}
        rsgislib.rastergis.set_class_names_colours(
            output_img, "chng_cls", class_info_dict
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            output_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if plot_thres_file is not None:
        import rsgislib.tools.plotting

        rsgislib.tools.plotting.plot_histogram_threshold(
            msk_arr_vals[..., 0], plot_thres_file, chng_thres
        )

    return chng_thres
