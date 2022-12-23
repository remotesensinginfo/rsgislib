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

import numpy
import tqdm
from osgeo import gdal
from rios.imagereader import ImageReader
from rios.imagewriter import ImageWriter
from sklearn.cluster import MiniBatchKMeans

import rsgislib
import rsgislib.imageutils
import rsgislib.rastergis


def img_pixel_sample_cluster(
    input_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    n_img_smpl=100,
    clusterer=MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    calc_stats=True,
    use_mean_shift_est_band_width=False,
):
    """
    A function which allows a clustering to be performed using the algorithms available
    within the scikit-learn library. The clusterer is trained on a sample of the input
    image and then applied using the predict function (therefore this function is only
    compatible with clusterers which have the predict function implemented) to the
    whole image.

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
    print("Sample input image:")
    dataSamp = rsgislib.imageutils.extract_img_pxl_sample(
        input_img, n_img_smpl, no_data_val
    )

    if use_mean_shift_est_band_width:
        print("Using Mean-Shift predict bandwidth")
        from sklearn.cluster import MeanShift, estimate_bandwidth

        bandwidth = estimate_bandwidth(dataSamp, quantile=0.2, n_samples=500)
        clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)

    print("Fit Clusterer")
    clusterer.fit(dataSamp)
    print("Fitted Clusterer")

    print("Apply to whole image:")
    reader = ImageReader(input_img, windowxsize=200, windowysize=200)
    writer = None
    for (info, block) in tqdm.tqdm(reader):
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1] * blkShape[2]))).T
        ID = numpy.arange(blkBands.shape[0])
        outClusterVals = numpy.zeros((blkBands.shape[0]))

        finiteMskArr = numpy.isfinite(blkBands).all(axis=1)
        ID = ID[finiteMskArr]
        blkBands = blkBands[finiteMskArr]

        noDataValArr = numpy.logical_not(
            numpy.where(blkBands == no_data_val, True, False).all(axis=1)
        )

        blkBandsNoData = blkBands[noDataValArr]
        ID = ID[noDataValArr]

        if ID.shape[0] > 0:
            outPred = clusterer.predict(blkBandsNoData) + 1
            outClusterVals[ID] = outPred

        outClusterValsOutArr = outClusterVals.reshape([1, blkShape[1], blkShape[2]])

        if writer is None:
            writer = ImageWriter(
                output_img,
                info=info,
                firstblock=outClusterValsOutArr,
                drivername=gdalformat,
                creationoptions=[],
            )
        else:
            writer.write(outClusterValsOutArr)
    writer.close(calcStats=False)

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )


def img_pixel_tiled_cluster(
    input_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    clusterer=MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    calc_stats=True,
    use_mean_shift_est_band_width=False,
    tile_x_size=200,
    tile_y_size=200,
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
    if use_mean_shift_est_band_width:
        from sklearn.cluster import MeanShift, estimate_bandwidth

    reader = ImageReader(input_img, windowxsize=tile_x_size, windowysize=tile_y_size)
    writer = None
    for (info, block) in tqdm.tqdm(reader):
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1] * blkShape[2]))).T
        ID = numpy.arange(blkBands.shape[0])
        outClusterVals = numpy.zeros((blkBands.shape[0]))

        finiteMskArr = numpy.isfinite(blkBands).all(axis=1)
        ID = ID[finiteMskArr]
        blkBands = blkBands[finiteMskArr]

        noDataValArr = numpy.logical_not(
            numpy.where(blkBands == no_data_val, True, False).all(axis=1)
        )

        blkBandsNoData = blkBands[noDataValArr]
        ID = ID[noDataValArr]

        if ID.shape[0] > 0:
            if use_mean_shift_est_band_width:
                bandwidth = estimate_bandwidth(
                    blkBandsNoData, quantile=0.2, n_samples=1000
                )
                clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)

            clusterer.fit(blkBandsNoData)
            outPred = clusterer.labels_ + 1
            outClusterVals[ID] = outPred

        outClusterValsOutArr = outClusterVals.reshape([1, blkShape[1], blkShape[2]])

        if writer is None:
            writer = ImageWriter(
                output_img,
                info=info,
                firstblock=outClusterValsOutArr,
                drivername=gdalformat,
                creationoptions=[],
            )
        else:
            writer.write(outClusterValsOutArr)
    writer.close(calcStats=False)

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )


def img_pixel_cluster(
    input_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    clusterer=MiniBatchKMeans(
        n_clusters=60, init="k-means++", max_iter=100, batch_size=100
    ),
    calc_stats=True,
    use_mean_shift_est_band_width=False,
):
    """
    A function which allows a clustering to be performed using the algorithms available
    within the scikit-learn library. The clusterer is applied to the whole image in one
    operation so therefore requires the whole image to be loaded into memory. However,
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

    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    nPxls = gdalDS.RasterXSize * gdalDS.RasterYSize

    pxlVals = numpy.zeros((gdalDS.RasterCount, nPxls))

    for nBand in numpy.arange(gdalDS.RasterCount):
        gdalBand = gdalDS.GetRasterBand(int(nBand + 1))
        imgArr = gdalBand.ReadAsArray().flatten()
        pxlVals[nBand] = imgArr

    pxlVals = pxlVals.T

    ID = numpy.arange(pxlVals.shape[0])
    outClusterVals = numpy.zeros((pxlVals.shape[0]))

    finiteMskArr = numpy.isfinite(pxlVals).all(axis=1)
    ID = ID[finiteMskArr]
    pxlVals = pxlVals[finiteMskArr]

    noDataValArr = numpy.logical_not(
        numpy.where(pxlVals == no_data_val, True, False).all(axis=1)
    )

    pxlVals = pxlVals[noDataValArr]
    ID = ID[noDataValArr]

    if ID.shape[0] > 0:
        if use_mean_shift_est_band_width:
            bandwidth = estimate_bandwidth(pxlVals, quantile=0.2, n_samples=1000)
            clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)
        print("Perform Clustering")
        clusterer.fit(pxlVals)
        print("Performed Clustering")
        outPred = clusterer.labels_ + 1
        outClusterVals[ID] = outPred

    outClusterValsOutArr = outClusterVals.reshape(
        [gdalDS.RasterYSize, gdalDS.RasterXSize]
    )
    print(outClusterValsOutArr.shape)

    gdalOutDS = gdal.Open(output_img, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(outClusterValsOutArr)
    gdalOutDS = None
    gdalDS = None

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
