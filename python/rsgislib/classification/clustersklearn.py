#! /usr/bin/env python
############################################################################
#  clustersklearn.py
#
#  Copyright 2020 RSGISLib.
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
# Purpose:  Provide a set of utilities which combine commands to create
#           useful extra functionality and make it more easily available
#           to be reused.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 09/01/2020
# Version: 1.0
#
# History:
# Version 1.0 - Refactored from previous structure.
#
###########################################################################

from typing import List

import numpy
from osgeo import gdal
from sklearn.cluster import MiniBatchKMeans
from sklearn.base import BaseEstimator

import rsgislib
import rsgislib.imageutils
import rsgislib.rastergis

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False


def img_pixel_sample_cluster(
    input_img: str,
    output_img: str,
    gdalformat: str = "KEA",
    no_data_val: float = 0,
    n_img_smpl: int = 100,
    clusterer: BaseEstimator = MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    calc_stats: bool = True,
    use_mean_shift_est_band_width: bool = False,
):
    """
    A function which allows a clustering to be performed using the algorithms available
    within the scikit-learn library. The clusterer is trained on a sample of the input
    image and then applied using the predict function (therefore this function is only
    compatible with clustering algorithms which have the predict function implemented)
    to the whole image.

    :param input_img: input image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param no_data_val: no data value associated with the input image.
    :param n_img_smpl: the input image sampling. (e.g., 100 is every 100th pixel)
    :param clusterer: clusterer from scikit-learn which must have a predict function.
    :param calc_stats: calculate image pixel statistics, histogram and image pyramids -
                       note if you are not using a KEA file then the format needs to
                       support RATs for this option as histogram and colour table
                       are written to RAT.
    :param use_mean_shift_est_band_width: use the mean-shift algorithm as the clusterer
                                          (pass None as the clusterer) where the
                                          bandwidth is calculated from the data itself.
    """
    from rios import applier

    print("Sample Input Image:")
    data_samp = rsgislib.imageutils.extract_img_pxl_sample(
        input_img, n_img_smpl, no_data_val
    )

    if use_mean_shift_est_band_width:
        print("Using Mean-Shift predict bandwidth")
        from sklearn.cluster import MeanShift, estimate_bandwidth

        bandwidth = estimate_bandwidth(data_samp, quantile=0.2, n_samples=500)
        clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)

    print("Fit Clusterer")
    clusterer.fit(data_samp)
    print("Fitted Clusterer")

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img
    otherargs = applier.OtherInputs()
    otherargs.clusterer = clusterer
    otherargs.no_data_val = no_data_val
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    # RIOS function to apply clusterer
    def _apply_sk_clusterer(info, inputs, outputs, otherargs):
        """
        Internal function for rios applier. Used within img_pixel_sample_cluster.
        """
        img_shp = inputs.input_img.shape

        img_bands_arr = inputs.input_img.reshape(
            (img_shp[0], (img_shp[1] * img_shp[2]))
        ).T
        ID = numpy.arange(img_bands_arr.shape[0])
        out_cluster_vals = numpy.zeros((img_bands_arr.shape[0]))

        finite_msk_arr = numpy.isfinite(img_bands_arr).all(axis=1)
        ID = ID[finite_msk_arr]
        img_bands_arr = img_bands_arr[finite_msk_arr]

        no_data_val_arr = numpy.logical_not(
            numpy.where(img_bands_arr == otherargs.no_data_val, True, False).all(axis=1)
        )

        img_bands_vld_data_arr = img_bands_arr[no_data_val_arr]
        ID = ID[no_data_val_arr]

        if ID.shape[0] > 0:
            out_pred = otherargs.clusterer.predict(img_bands_vld_data_arr) + 1
            out_cluster_vals[ID] = out_pred

        out_cluster_vals = out_cluster_vals.astype(numpy.int32)
        outputs.output_img = out_cluster_vals.reshape([1, img_shp[1], img_shp[2]])

    print("Applying to Whole Image")
    applier.apply(_apply_sk_clusterer, infiles, outfiles, otherargs, controls=aControls)

    if calc_stats:
        if gdalformat == "KEA":
            rsgislib.rastergis.pop_rat_img_stats(
                clumps_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )
        else:
            rsgislib.imageutils.pop_thmt_img_stats(
                input_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )


def img_pixel_tiled_cluster(
    input_img: str,
    output_img: str,
    gdalformat: str = "KEA",
    no_data_val: float = 0,
    clusterer: BaseEstimator = MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    calc_stats: bool = True,
    use_mean_shift_est_band_width: bool = False,
    tile_x_size: int = 200,
    tile_y_size: int = 200,
):
    """
    A function which allows a clustering to be performed using the algorithms available
    within the scikit-learn library. The clusterer is applied to a single tile at a time
    and therefore produces tile boundaries in the result. However, memory is controlled
    such that usage isn't excessive which it could be when processing a whole image.

    :param input_img: input image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param no_data_val: no data value associated with the input image.
    :param clusterer: clusterer from scikit-learn which must have a predict function.
    :param calc_stats: calculate image pixel statistics, histogram and image pyramids -
                       note if you are not using a KEA file then the format needs to
                       support RATs for this option as histogram and colour table
                       are written to RAT.
    :param use_mean_shift_est_band_width: use the mean-shift algorithm as the clusterer
                                          (pass None as the clusterer) where the
                                          bandwidth is calculated from the data itself.
    :param tile_x_size: tile size in the x-axis in pixels.
    :param tile_y_size: tile size in the y-axis in pixels.
    """
    from rios import applier

    if use_mean_shift_est_band_width:
        from sklearn.cluster import MeanShift, estimate_bandwidth

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.input_img = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.clusterer = clusterer
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    aControls.windowxsize = tile_x_size
    aControls.windowysize = tile_y_size

    # RIOS function to apply clusterer
    def _apply_sk_tiled_clusterer(info, inputs, outputs, otherargs):
        """
        Internal function for rios applier. Used within img_pixel_tiled_cluster.
        """
        img_shp = inputs.input_img.shape

        img_bands_arr = inputs.input_img.reshape(
            (img_shp[0], (img_shp[1] * img_shp[2]))
        ).T
        ID = numpy.arange(img_bands_arr.shape[0])
        out_cluster_vals = numpy.zeros((img_bands_arr.shape[0]))

        finite_msk_arr = numpy.isfinite(img_bands_arr).all(axis=1)
        ID = ID[finite_msk_arr]
        img_bands_arr = img_bands_arr[finite_msk_arr]

        no_data_val_arr = numpy.logical_not(
            numpy.where(img_bands_arr == otherargs.no_data_val, True, False).all(axis=1)
        )

        img_bands_vld_data_arr = img_bands_arr[no_data_val_arr]
        ID = ID[no_data_val_arr]

        if ID.shape[0] > 0:
            if use_mean_shift_est_band_width:
                bandwidth = estimate_bandwidth(
                    img_bands_vld_data_arr, quantile=0.2, n_samples=1000
                )
                otherargs.clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)

            otherargs.clusterer.fit(img_bands_vld_data_arr)
            out_pred = otherargs.clusterer.predict(img_bands_vld_data_arr) + 1
            out_cluster_vals[ID] = out_pred

        out_cluster_vals = out_cluster_vals.astype(numpy.int32)
        outputs.output_img = out_cluster_vals.reshape([1, img_shp[1], img_shp[2]])

    applier.apply(
        _apply_sk_tiled_clusterer, infiles, outfiles, otherargs, controls=aControls
    )

    if calc_stats:
        if gdalformat == "KEA":
            rsgislib.rastergis.pop_rat_img_stats(
                clumps_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )
        else:
            rsgislib.imageutils.pop_thmt_img_stats(
                input_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )


def img_pixel_cluster(
    input_img: str,
    output_img: str,
    gdalformat: str = "KEA",
    no_data_val: float = 0,
    clusterer: BaseEstimator = MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    calc_stats: bool = True,
    use_mean_shift_est_band_width: bool = False,
):
    """
    A function which allows a clustering to be performed using the algorithms available
    within the scikit-learn library. The clusterer is applied to the whole image in one
    operation therefore requires the whole image to be loaded into memory. However,
    if there is sufficient memory all the clustering algorithms within scikit-learn can
    be applied without boundary artifacts.

    :param input_img: input image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param no_data_val: no data value associated with the input image.
    :param clusterer: clusterer from scikit-learn which must have a predict function.
    :param calc_stats: calculate image pixel statistics, histogram and image pyramids -
                       note if you are not using a KEA file then the format needs to
                       support RATs for this option as histogram and colour table
                       are written to RAT.
    :param use_mean_shift_est_band_width: use the mean-shift algorithm as the clusterer
                                          (pass None as the clusterer) where the
                                          bandwidth is calculated from the data itself.
    """
    # Create output image
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_16UINT
    )

    if use_mean_shift_est_band_width:
        from sklearn.cluster import MeanShift, estimate_bandwidth

    gdal_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
    n_pxls = gdal_ds.RasterXSize * gdal_ds.RasterYSize

    pxl_vals = numpy.zeros((gdal_ds.RasterCount, n_pxls))

    for n_band in numpy.arange(gdal_ds.RasterCount):
        gdal_band = gdal_ds.GetRasterBand(int(n_band + 1))
        img_arr = gdal_band.ReadAsArray().flatten()
        pxl_vals[n_band] = img_arr

    pxl_vals = pxl_vals.T

    ID = numpy.arange(pxl_vals.shape[0])
    out_cluster_vals = numpy.zeros((pxl_vals.shape[0]))

    finite_msk_arr = numpy.isfinite(pxl_vals).all(axis=1)
    ID = ID[finite_msk_arr]
    pxl_vals = pxl_vals[finite_msk_arr]

    no_data_val_arr = numpy.logical_not(
        numpy.where(pxl_vals == no_data_val, True, False).all(axis=1)
    )

    pxl_vals = pxl_vals[no_data_val_arr]
    ID = ID[no_data_val_arr]

    if ID.shape[0] > 0:
        if use_mean_shift_est_band_width:
            bandwidth = estimate_bandwidth(pxl_vals, quantile=0.2, n_samples=1000)
            clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)
        print("Perform Clustering")
        clusterer.fit(pxl_vals)
        print("Performed Clustering")
        out_pred = clusterer.labels_ + 1
        out_cluster_vals[ID] = out_pred

    out_cluster_vals_out_arr = out_cluster_vals.reshape(
        [gdal_ds.RasterYSize, gdal_ds.RasterXSize]
    )
    print(out_cluster_vals_out_arr.shape)

    gdal_out_ds = gdal.Open(output_img, gdal.GA_Update)
    gdal_out_band = gdal_out_ds.GetRasterBand(1)
    gdal_out_band.WriteArray(out_cluster_vals_out_arr)
    gdal_out_ds = None
    gdal_ds = None

    if calc_stats:
        if gdalformat == "KEA":
            rsgislib.rastergis.pop_rat_img_stats(
                clumps_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )
        else:
            rsgislib.imageutils.pop_thmt_img_stats(
                input_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )


def cluster_sklearn_rat(
    clumps_img: str,
    variables: List[str],
    sk_clusterer: BaseEstimator = MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    out_col: str = "OutClass",
    roi_col: str = None,
    roi_val: int = 1,
    sub_sample: float = None,
):
    """
    A function which will apply an scikit-learn clustering (i.e., unsupervised
    classification) within a Raster Attribute Table (RAT).

    :param clumps_img: is the clumps image on which the clustering is to
                       be performed
    :param variables: is an array of column names which are to be used for
                      the clustering
    :param sk_clusterer: an instance of a scikit-learn clustering algorithm

    :param out_col: is the output column
    :param roi_col: is a column name for a column which specifies the region
                    to be clustered. If None ignored (Default: None)
    :param roi_val: is a int value used within the roi_col to select a
                    region to be clustered (Default: 1)
    :param sub_sample: Subsample the data for fitting the clusterer. Provide
                       the proprotion (0-1) to be used for the clustering.

    """
    import math
    import numpy.random
    from rios import rat

    rat_cols = rsgislib.rastergis.get_rat_columns(clumps_img)
    if (roi_col is not None) and (roi_col not in rat_cols):
        raise Exception(f"The ROI column ({roi_col}) is not present.")

    for var in variables:
        if var not in rat_cols:
            raise Exception(f"Variable column ({var}) is not present.")

    if sub_sample is not None:
        if (sub_sample < 0) or (sub_sample > 1):
            raise Exception(f"Subsample ({sub_sample}) must be between 0 and 1.")

    rat_ds = gdal.Open(clumps_img, gdal.GA_Update)
    if rat_ds is None:
        raise rsgislib.RSGISPyException("Could not open the inputted clumps image.")

    vars_lst = list()
    for var in variables:
        var_arr = rat.readColumn(rat_ds, var)
        vars_lst.append(var_arr)

    vars_arr = numpy.vstack(vars_lst).T
    print(vars_arr.shape)

    row_id = numpy.arange(vars_lst[0].shape[0])
    out_cls_vals = numpy.zeros((vars_lst[0].shape[0]))
    print(row_id.shape)

    if roi_col is not None:
        roi_arr = rat.readColumn(rat_ds, roi_col)
        vars_arr = vars_arr[roi_arr == roi_val]
        row_id = row_id[roi_arr == roi_val]

    print(vars_arr.shape)
    print(row_id.shape)

    if sub_sample is not None:
        n_feats = row_id.shape[0]
        n_sampls_feats = int(math.floor(n_feats * sub_sample))
        vars_smpl_arr = numpy.random.choice(
            vars_arr, size=n_sampls_feats, replace=False
        )
    else:
        vars_smpl_arr = vars_arr

    print("Fit Clusterer")
    sk_clusterer.fit(vars_smpl_arr)

    print("Apply Clusterer")
    out_pred = sk_clusterer.predict(vars_arr) + 1

    if row_id.shape[0] > 0:
        out_cls_vals[row_id] = out_pred

    # Write output column
    rat.writeColumn(rat_ds, out_col, out_cls_vals.astype(int))
    rat_ds = None
