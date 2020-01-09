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

import rsgislib
import rsgislib.imageutils
import rsgislib.rastergis

import tqdm

import numpy

import osgeo.gdal as gdal

from sklearn.cluster import MiniBatchKMeans

from rios.imagereader import ImageReader
from rios.imagewriter import ImageWriter

def img_pixel_sample_cluster(inputImg, outputImg, gdalformat='KEA', noDataVal=0, imgSamp=100,
                             clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100),
                             calcStats=True, useMeanShiftEstBandWidth=False):
    """
A function which allows a clustering to be performed using the algorithms available
within the scikit-learn library. The clusterer is trained on a sample of the input
image and then applied using the predict function (therefore this function is only
compatiable with clusterers which have the predict function implemented) to the whole
image.

:param inputImg: input image file.
:param outputImg: output image file.
:param gdalformat: output image file format.
:param noDataVal: no data value associated with the input image.
:param imgSamp: the input image sampling. (e.g., 100 is every 100th pixel)
:param clusterer: clusterer from scikit-learn which must have a predict function.
:param calcStats: calculate image pixel statistics, histogram and image pyramids - note if you are not using a
                  KEA file then the format needs to support RATs for this option as histogram and colour table
                  are written to RAT.
:param useMeanShiftEstBandWidth: use the mean-shift algorithm as the clusterer (pass None as the clusterer) where
                                 the bandwidth is calculated from the data itself.

"""
    print('Sample input image:')
    dataSamp = rsgislib.imageutils.extractImgPxlSample(inputImg, imgSamp, noDataVal)

    if useMeanShiftEstBandWidth:
        print('Using Mean-Shift predict bandwidth')
        from sklearn.cluster import MeanShift, estimate_bandwidth
        bandwidth = estimate_bandwidth(dataSamp, quantile=0.2, n_samples=500)
        clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)

    print('Fit Clusterer')
    outClust = clusterer.fit(dataSamp)
    print('Fitted Clusterer')

    print('Apply to whole image:')
    reader = ImageReader(inputImg, windowxsize=200, windowysize=200)
    writer = None
    for (info, block) in tqdm.tqdm(reader):
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1 ] *blkShape[2]))).T
        ID = numpy.arange(blkBands.shape[0])
        outClusterVals = numpy.zeros((blkBands.shape[0]))

        finiteMskArr = numpy.isfinite(blkBands).all(axis=1)
        ID = ID[finiteMskArr]
        blkBands = blkBands[finiteMskArr]

        noDataValArr = numpy.logical_not(numpy.where(blkBands == noDataVal, True, False).all(axis=1))

        blkBandsNoData = blkBands[noDataValArr]
        ID = ID[noDataValArr]

        if ID.shape[0] > 0:
            outPred = clusterer.predict(blkBandsNoData ) +1
            outClusterVals[ID] = outPred

        outClusterValsOutArr = outClusterVals.reshape([1 ,blkShape[1] ,blkShape[2]])

        if writer is None:
            writer = ImageWriter(outputImg, info=info, firstblock=outClusterValsOutArr, drivername=gdalformat, creationoptions=[])
        else:
            writer.write(outClusterValsOutArr)
    writer.close(calcStats=False)

    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def img_pixel_tiled_cluster(inputImg, outputImg, gdalformat='KEA', noDataVal=0, 
                            clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100), 
                            calcStats=True, useMeanShiftEstBandWidth=False, tileXSize=200, tileYSize=200):
    """
A function which allows a clustering to be performed using the algorithms available
within the scikit-learn library. The clusterer is applied to a single tile at a time
and therefore produces tile boundaries in the result. However, memory is controlled
such that usage isn't excessive which it could be when processing a whole image.

:param inputImg: input image file.
:param outputImg: output image file.
:param gdalformat: output image file format.
:param noDataVal: no data value associated with the input image.
:param clusterer: clusterer from scikit-learn which must have a predict function.
:param calcStats: calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
:param useMeanShiftEstBandWidth: use the mean-shift algorithm as the clusterer (pass None as the clusterer) where the bandwidth is calculated from the data itself.
:param tileXSize: tile size in the x-axis in pixels.
:param tileYSize: tile size in the y-axis in pixels.

"""
    if useMeanShiftEstBandWidth:
        from sklearn.cluster import MeanShift, estimate_bandwidth

    reader = ImageReader(inputImg, windowxsize=tileXSize, windowysize=tileYSize)
    writer = None
    for (info, block) in tqdm.tqdm(reader):
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1 ] *blkShape[2]))).T
        ID = numpy.arange(blkBands.shape[0])
        outClusterVals = numpy.zeros((blkBands.shape[0]))

        finiteMskArr = numpy.isfinite(blkBands).all(axis=1)
        ID = ID[finiteMskArr]
        blkBands = blkBands[finiteMskArr]

        noDataValArr = numpy.logical_not(numpy.where(blkBands == noDataVal, True, False).all(axis=1))

        blkBandsNoData = blkBands[noDataValArr]
        ID = ID[noDataValArr]

        if ID.shape[0] > 0:
            if useMeanShiftEstBandWidth:
                bandwidth = estimate_bandwidth(blkBandsNoData, quantile=0.2, n_samples=1000)
                clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)

            clusterer.fit(blkBandsNoData)
            outPred = clusterer.labels_ + 1
            outClusterVals[ID] = outPred

        outClusterValsOutArr = outClusterVals.reshape([1 ,blkShape[1] ,blkShape[2]])

        if writer is None:
            writer = ImageWriter(outputImg, info=info, firstblock=outClusterValsOutArr, drivername=gdalformat, creationoptions=[])
        else:
            writer.write(outClusterValsOutArr)
    writer.close(calcStats=False)

    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)



def img_pixel_cluster(inputImg, outputImg, gdalformat='KEA', noDataVal=0, 
                      clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100), 
                      calcStats=True, useMeanShiftEstBandWidth=False):
    """
A function which allows a clustering to be performed using the algorithms available
within the scikit-learn library. The clusterer is applied to the whole image in one
operation so therefore requires the whole image to be loaded into memory. However,
if there is sufficent memory all the clustering algorithms within scikit-learn can be
applied without boundary artifacts.

:param inputImg: input image file.
:param outputImg: output image file.
:param gdalformat: output image file format.
:param noDataVal: no data value associated with the input image.
:param clusterer: clusterer from scikit-learn which must have a predict function.
:param calcStats: calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
:param useMeanShiftEstBandWidth: use the mean-shift algorithm as the clusterer (pass None as the clusterer) where the bandwidth is calculated from the data itself.

"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_16UINT)

    if useMeanShiftEstBandWidth:
        from sklearn.cluster import MeanShift, estimate_bandwidth

    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    nPxls = gdalDS.RasterXSize * gdalDS.RasterYSize

    pxlVals = numpy.zeros((gdalDS.RasterCount, nPxls))

    for nBand in numpy.arange(gdalDS.RasterCount):
        gdalBand = gdalDS.GetRasterBand(int(nBand +1))
        imgArr = gdalBand.ReadAsArray().flatten()
        pxlVals[nBand] = imgArr

    pxlVals = pxlVals.T

    ID = numpy.arange(pxlVals.shape[0])
    outClusterVals = numpy.zeros((pxlVals.shape[0]))

    finiteMskArr = numpy.isfinite(pxlVals).all(axis=1)
    ID = ID[finiteMskArr]
    pxlVals = pxlVals[finiteMskArr]

    noDataValArr = numpy.logical_not(numpy.where(pxlVals == noDataVal, True, False).all(axis=1))

    pxlVals = pxlVals[noDataValArr]
    ID = ID[noDataValArr]

    if ID.shape[0] > 0:
        if useMeanShiftEstBandWidth:
            bandwidth = estimate_bandwidth(pxlVals, quantile=0.2, n_samples=1000)
            clusterer = MeanShift(bandwidth=bandwidth, bin_seeding=True)
        print('Perform Clustering')
        clusterer.fit(pxlVals)
        print('Performed Clustering')
        outPred = clusterer.labels_ + 1
        outClusterVals[ID] = outPred

    outClusterValsOutArr = outClusterVals.reshape([gdalDS.RasterYSize, gdalDS.RasterXSize])
    print(outClusterValsOutArr.shape)

    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(outClusterValsOutArr)
    gdalOutDS = None
    gdalDS = None

    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)




img_pixel_cluster('S2_UVD_27sept_27700_sub.kea', 'S2_UVD_27sept_27700_sub_clusters_tiled.kea', gdalformat='KEA', noDataVal=0,
                             clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100),
                             calcStats=True, useMeanShiftEstBandWidth=False)

