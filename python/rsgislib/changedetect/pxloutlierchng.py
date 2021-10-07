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
from rios import cuiprogress

import rsgislib
import rsgislib.rastergis
import rsgislib.imageutils


def find_class_outliers(pyod_obj, input_img, in_msk_img, out_lbls_img, out_scores_img=None, img_mask_val=1, img_bands=None,
                        gdalformat="KEA"):
    """
    This function uses the pyod (https://github.com/yzhao062/pyod) library to find outliers within a class.
    It is assumed that the input images are from a different date than the mask (classification) and therefore
    the outliners will related to class changes.

    :param pyod_obj: an instance of a pyod.models (e.g., pyod.models.knn.KNN) pass parameters to the constructor
    :param input_img: input image used for analysis
    :param in_msk_img: input image mask use to define the region of interest.
    :param out_lbls_img: output image with pixel over of 1 for within mask but not outlier and 2 for in mask and outlier.
    :param out_scores_img: output image (optional, None and won't be provided; Default None) providing the probability of
                           each pixel being an outlier
    :param img_mask_val: the pixel value within the mask image for the class of interest. (Default 1)
    :param img_bands: the image bands to be used for the analysis. If None then all used (Default: None)
    :param gdalformat: file format for the output image(s). Default KEA.

    """
    if img_bands is not None:
        if not ((type(img_bands) is list) or (type(img_bands) is tuple)):
            raise rsgislib.RSGISPyException("If provided then img_bands should be a list (or None)")
    else:
        n_bands = rsgislib.imageutils.get_image_band_count(input_img)
        img_bands = numpy.arange(1, n_bands + 1)
    num_vars = len(img_bands)
    img_val_no_data = rsgislib.imageutils.getImageNoDataValue(input_img)

    msk_arr_vals = rsgislib.imageutils.extractImgPxlValsInMsk(input_img, img_bands, in_msk_img, img_mask_val, img_val_no_data)
    print("There were {} pixels within the mask.".format(msk_arr_vals.shape[0]))

    print("Fitting oulier detector")
    pyod_obj.fit(msk_arr_vals)
    print("Fitted oulier detector")

    # RIOS function to apply classifer
    def _applyPyOB(info, inputs, outputs, otherargs):
        # Internal function for rios applier. Used within find_class_outliers.

        out_lbls_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.uint8)
        if otherargs.out_scores:
            out_scores_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.float)
        if numpy.any(inputs.image_mask == otherargs.msk_val):
            out_lbls_vals = out_lbls_vals.flatten()
            img_msk_vals = inputs.image_mask.flatten()
            if otherargs.out_scores:
                out_scores_vals = out_scores_vals.flatten()
            ID = numpy.arange(img_msk_vals.shape[0])

            img_shape = inputs.input_img.shape
            img_bands = inputs.input_img.reshape((img_shape[0], (img_shape[1] * img_shape[2])))

            band_lst = []
            for band in otherargs.img_bands:
                if (band > 0) and (band <= img_shape[0]):
                    band_lst.append(img_bands[band - 1])
                else:
                    raise Exception("Band ({}) specified is not within the image".format(band))
            img_bands_sel = numpy.stack(band_lst, axis=0)
            img_bands_trans = numpy.transpose(img_bands_sel)

            if otherargs.no_data_val is not None:
                ID = ID[(img_bands_trans != otherargs.no_data_val).all(axis=1)]
                img_msk_vals = img_msk_vals[(img_bands_trans != otherargs.no_data_val).all(axis=1)]
                img_bands_trans = img_bands_trans[(img_bands_trans != otherargs.no_data_val).all(axis=1)]

            ID = ID[img_msk_vals == otherargs.msk_val]
            img_bands_trans = img_bands_trans[img_msk_vals == otherargs.msk_val]

            if img_bands_trans.shape[0] > 0:
                pred_lbls = otherargs.pyod_obj.predict(img_bands_trans)
                pred_lbls = pred_lbls + 1
                out_lbls_vals[ID] = pred_lbls
            out_lbls_vals = numpy.expand_dims(
                out_lbls_vals.reshape((inputs.image_mask.shape[1], inputs.image_mask.shape[2])), axis=0)

            if otherargs.out_scores:
                if img_bands_trans.shape[0] > 0:
                    pred_probs = otherargs.pyod_obj.predict_proba(img_bands_trans, method='unify')
                    out_scores_vals[ID] = pred_probs[:, 1]
                out_scores_vals = numpy.expand_dims(
                    out_scores_vals.reshape((inputs.image_mask.shape[1], inputs.image_mask.shape[2])), axis=0)

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
    outfiles.out_lbls_img = out_lbls_img
    if out_scores_img is not None:
        outfiles.out_scores_img = out_scores_img
        otherargs.out_scores = True

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Outlier Detector")
    applier.apply(_applyPyOB, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")

    rsgislib.rastergis.pop_rat_img_stats(clumps=out_lbls_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True)
    if out_scores_img is not None:
        rsgislib.imageutils.pop_img_stats(out_scores_img, usenodataval=True, nodataval=0, calcpyramids=True)
