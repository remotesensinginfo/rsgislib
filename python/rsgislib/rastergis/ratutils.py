#!/usr/bin/env python
############################################################################
#  ratutils.py
#
#  Copyright 2013 RSGISLib.
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
# Author: Dan Clewley
# Email: daniel.clewley@gmail.com
# Date: 16/11/2013
# Version: 1.1
#
# History:
# Version 1.0 - Created.
# Version 1.1 - Update to be included into RSGISLib python modules tree
#               (By Pete Bunting).
#
############################################################################

import sys
import math
import os
import os.path
import shutil
import glob
from multiprocessing import Pool
import multiprocessing

from enum import Enum
import rsgislib
from rsgislib import rastergis
from rsgislib import vectorutils
from rsgislib import imageutils
from rsgislib import segmentation

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False

haveGDALOGRPy = True
try:
    from osgeo import ogr
    from osgeo import osr
except ImportError as gdalogrErr:
    haveGDALOGRPy = False

haveMatPlotLib = True
try:
    import matplotlib.pyplot as plt
    import matplotlib.colors as mClrs
except ImportError as pltErr:
    haveMatPlotLib = False

haveNumpy = True
try:
    import numpy
except ImportError as numErr:
    haveNumpy = False

haveHDF5 = True
try:
    import h5py
except ImportError as h5Err:
    haveHDF5 = False

haveScipyStats = True
try:
    import scipy.stats
except ImportError as scipystatsErr:
    haveScipyStats = False

haveRIOSRat = True
try:
    from rios import rat
    from rios import ratapplier
except ImportError as riosRatErr:
    haveRIOSRat = False

haveSKLearnPCA = True
try:
    from sklearn.decomposition import PCA
except ImportError as sklearnPCAErr:
    haveSKLearnPCA = False


class RSGISRATThresMeasure(Enum):
    kurtosis = 1
    skewness = 2
    combined = 3
    auto = 4


class RSGISRATThresDirection(Enum):
    lower = 1
    upper = 2
    lowerupper = 3


def calc_plot_gaussian_histo_model(
    clumpsFile,
    outGausH5File,
    outHistH5File,
    outPlotFile,
    varCol,
    binWidth,
    classColumn,
    classVal,
    plotTitle,
):
    """
    Extracts a column from the RAT, masking by a class calculating the histogram and
    fitting a Gaussian mixture model to the histogram. Outputs include a plot and HDF5
    files of the histogram and gaussian parameters.

    :param clumpsFile: input clumps file with populated RAT.
    :param outGausH5File: the output HDF5 file for the Gaussian Mixture Model
    :param outHistH5File: the output HDF5 file for the histogram.
    :param outPlotFile: the output PDF file for the plot
    :param varCol: Column within the RAT for the variable to be used for the histogram
    :param binWidth: Bin width for the histogram
    :param classColumn: Column where the classes are specified
    :param classVal: Class used to mask the input variable
    :param plotTitle: title for the plot

    Example:

    .. code:: python

        from rsgislib.rastergis import ratutils

        clumpsFile = "FrenchGuiana_10_ALL_sl_HH_lee_UTM_mosaic_dB_segs.kea"
        outGausH5File = "gaufit.h5"
        outHistH5File = "histfile.h5"
        outPlotFile = "Plot.pdf"
        varCol = "HVdB"
        binWidth = 0.1
        classColumn = "Classes"
        classVal = "Mangrove"
        plotTitle = "HV dB Backscater from Mangroves; French Guiana"

        ratutils.calc_plot_gaussian_histo_model(clumpsFile, outGausH5File,
                                                outHistH5File, outPlotFile,
                                                varCol, binWidth, classColumn,
                                                classVal, plotTitle)

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check matplotlib is available
    if not haveMatPlotLib:
        raise Exception(
            "The matplotlib module is required for this function "
            "could not be imported\n\t" + pltErr
        )
    # Check hdf5 is available
    if not haveHDF5:
        raise Exception(
            "The hdf5 module is required for this function could not be imported\n\t"
            + h5Err
        )

    # Calculate histogram and fit Gaussian Mixture Model
    rastergis.fit_hist_gausian_mixture_model(
        clumps=clumpsFile,
        outH5File=outGausH5File,
        outHistFile=outHistH5File,
        varCol=varCol,
        binWidth=binWidth,
        classColumn=classColumn,
        classVal=classVal,
    )

    if not h5py.is_hdf5(outGausH5File):
        raise Exception(outGausH5File + " is not a HDF5 file.")

    if not h5py.is_hdf5(outHistH5File):
        raise Exception(outHistH5File + " is not a HDF5 file.")

    gausFile = h5py.File(outGausH5File, "r")
    gausParams = gausFile["/DATA/DATA"]

    histFile = h5py.File(outHistH5File, "r")
    histData = histFile["/DATA/DATA"]

    xVals = []
    xValsHist = []
    histBins = []

    for histBin in histData:
        xValsHist.append(histBin[0] - (binWidth / 2))
        xVals.append(histBin[0])
        histBins.append(histBin[1])

    gAmpVals = []
    gFWHMVals = []
    gOffVals = []
    gNoiseVals = []
    noiseVal = 0.0

    for gausParam in gausParams:
        gOffVals.append(gausParam[0])
        gAmpVals.append(gausParam[1])
        gFWHMVals.append(gausParam[2])
        noiseVal = gausParam[3]

    fig, ax = plt.subplots()
    histBars = ax.bar(
        xValsHist, histBins, width=binWidth, color="#A7A7A7", edgecolor="#A7A7A7"
    )

    predVals = numpy.zeros(len(xVals))
    for i in range(len(xVals)):
        gNoiseVals.append(noiseVal)
        for j in range(len(gOffVals)):
            predVals[i] = predVals[i] + (
                gAmpVals[j]
                * math.exp(
                    (-1.0)
                    * (pow(xVals[i] - gOffVals[j], 2) / (2.0 * pow(gFWHMVals[j], 2)))
                )
            )
        predVals[i] = predVals[i] + noiseVal

    ax.plot(xVals, predVals, color="red")
    ax.plot(xVals, gNoiseVals, color="blue", linestyle="dashed")

    ax.set_ylabel("Freq.")
    ax.set_title(plotTitle)
    plt.savefig(outPlotFile, format="PDF")

    gausFile.close()
    histFile.close()


def find_change_clumps_hist_skew_kurt_test(
    inputClumps,
    inClassCol,
    classOfInterest,
    changeVarCol,
    outChangeFeatCol,
    noDataVals=[],
    thresMeasure=RSGISRATThresMeasure.auto,
    exportPlot=None,
    showAllThreshPlot=False,
):
    """
    This function identifies potential change features from both sides of the histogram
    by slicing the histogram and finding an optimal skewness and kurtosis.

    Where:

    :param inputClumps: input clumps file.
    :param inClassCol: The column specifiying the classes, one of which change will
                       be found.
    :param classOfInterest: The class (as defined in inClassCol) on which changed
                            is being found.
    :param changeVarCol: Variable(s) to be used to find change. Expecting column name.
                         Needs to be numeric. If a list of column names is provided
                         then they are combined using PCA and the first PC is used
                         for the change process.
    :param outChangeFeatCol: the output column. Regions lower than lower threshold
                             have value 1. Regions higher than upper threshold have
                             value 2. No change had threshold 0.
    :param noDataVals: list of no data values to be ignored.
    :param thresMeasure: needs to be of type RSGISRATThresMeasure (default is auto)
    :param exportPlot: file name for exporting a histogram plot with thresholds
                       annotated. No plot is create if None is passed (default is none).
    :param showAllThreshPlot: option when plotting to put all the thresholds on to the
                              plot rather than just the one being used.

    :returns: list of lower [0] and upper [1] thresholds used to define
             the no change region.

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )
    # Check scipy stats is available
    if not haveScipyStats:
        raise Exception(
            "The scipy stats is required for this function could not be imported\n\t"
            + scipystatsErr
        )
    if not exportPlot == None:
        # Check matplotlib is available
        if not haveMatPlotLib:
            raise Exception(
                "The matplotlib module is required for this function "
                "could not be imported\n\t" + pltErr
            )
    if type(changeVarCol) is list:
        if not haveSKLearnPCA:
            raise Exception(
                "The scikit learn library PCA module is required when a list "
                "of column variables is given\n\t" + sklearnPCAErr
            )

    ## Open the image file...
    ratDataset = gdal.Open(inputClumps, gdal.GA_Update)

    ## Read in columns
    classVals = rat.readColumn(ratDataset, inClassCol)
    outChangeFeats = numpy.zeros_like(classVals)
    ID = numpy.arange(classVals.shape[0])

    vals = None
    if type(changeVarCol) is list:
        numVars = len(changeVarCol)
        numRows = classVals.shape[0]
        varVals = numpy.zeros((numVars, numRows), dtype=numpy.float)
        i = 0
        for varCol in changeVarCol:
            colVals = rat.readColumn(ratDataset, varCol)
            varVals[i] = colVals
            i = i + 1
        varVals = varVals.transpose()

        ID = ID[classVals == classOfInterest]
        varVals = varVals[(classVals == classOfInterest)]

        ID = ID[numpy.isfinite(varVals).all(axis=1)]
        varVals = varVals[numpy.isfinite(varVals).all(axis=1)]

        for noDataVal in noDataVals:
            ID = ID[(varVals != noDataVal).all(axis=1)]
            varVals = varVals[(varVals != noDataVal).all(axis=1)]

        pca = PCA(n_components=1)
        fittedPCA = pca.fit(varVals)

        vals = fittedPCA.transform(varVals)[:, 0]
    else:
        vals = rat.readColumn(ratDataset, changeVarCol)

        ID = ID[classVals == classOfInterest]
        if ID.shape[0] == 0:
            rat.writeColumn(ratDataset, outChangeFeatCol, outChangeFeats)
            return
        vals = vals[classVals == classOfInterest]

        ID = ID[numpy.isfinite(vals)]
        vals = vals[numpy.isfinite(vals)]

        for noDataVal in noDataVals:
            ID = ID[vals != noDataVal]
            vals = vals[vals != noDataVal]

    n = vals.shape[0]
    lq = numpy.percentile(vals, 25)
    uq = numpy.percentile(vals, 75)
    iqr = uq - lq
    binSize = 2 * iqr * n ** (-1 / 3)
    print("Bin Size = ", binSize)
    numBins = int((numpy.max(vals) - numpy.min(vals)) / binSize) + 2
    print("num of bins = ", numBins)

    hist, bin_edges = numpy.histogram(vals, bins=numBins)

    print(hist.shape)
    print(bin_edges.shape)

    print("LQ = ", lq)
    print("UQ = ", uq)

    lqNumBins = int((lq - bin_edges[0]) / binSize) + 1
    uqNumBins = int((bin_edges[-1] - uq) / binSize) + 1

    print("lqNumBins = ", lqNumBins)
    print("uqNumBins = ", uqNumBins)

    kurtosisVals = numpy.zeros((lqNumBins, uqNumBins), dtype=numpy.float)
    skewnessVals = numpy.zeros((lqNumBins, uqNumBins), dtype=numpy.float)
    lowBins = numpy.zeros((lqNumBins, uqNumBins), dtype=numpy.int)
    upBins = numpy.zeros((lqNumBins, uqNumBins), dtype=numpy.int)

    for lowBin in range(lqNumBins):
        for upBin in range(uqNumBins):
            # print("Bin [" + str(lowBin) + ", " + str(numBins-upBin) + "]")
            histTmp = hist[lowBin : (numBins - upBin)]
            # print(histTmp)
            # print(histTmp.shape)
            lowBins[lowBin, upBin] = lowBin
            upBins[lowBin, upBin] = numBins - upBin

            kurtosisVals[lowBin, upBin] = scipy.stats.kurtosis(histTmp)
            skewnessVals[lowBin, upBin] = scipy.stats.skew(histTmp)

    # print(kurtosisVals)
    # print(skewnessVals)
    kurtosisValsAbs = numpy.absolute(kurtosisVals)
    skewnessValsAbs = numpy.absolute(skewnessVals)
    kurtosisValsNorm = (kurtosisValsAbs - numpy.min(kurtosisValsAbs)) / (
        numpy.max(kurtosisValsAbs) - numpy.min(kurtosisValsAbs)
    )
    skewnessValsNorm = (skewnessValsAbs - numpy.min(skewnessValsAbs)) / (
        numpy.max(skewnessValsAbs) - numpy.min(skewnessValsAbs)
    )

    combined = kurtosisValsNorm + skewnessValsNorm
    # combined = kurtosisValsAbs + skewnessValsAbs
    # print(combined)

    minKurt = numpy.unravel_index(
        numpy.argmin(kurtosisValsAbs, axis=None), kurtosisValsAbs.shape
    )
    minSkew = numpy.unravel_index(
        numpy.argmin(skewnessValsAbs, axis=None), skewnessValsAbs.shape
    )
    minComb = numpy.unravel_index(numpy.argmin(combined, axis=None), combined.shape)

    print("Kurtosis bin indexes: ", minKurt)
    print("Skewness bin indexes: ", minSkew)
    print("Combined bin indexes: ", minComb)

    lowBinKurt = minKurt[0]
    lowerThresKurt = bin_edges[lowBinKurt] + (binSize / 2)
    upBinKurt = numBins - minKurt[1]
    upperThresKurt = bin_edges[upBinKurt] + (binSize / 2)
    print(
        "No Change Data Range (Kurtosis): ["
        + str(lowerThresKurt)
        + ","
        + str(upperThresKurt)
        + "]"
    )

    lowBinSkew = minSkew[0]
    lowerThresSkew = bin_edges[lowBinSkew] + (binSize / 2)
    upBinSkew = numBins - minSkew[1]
    upperThresSkew = bin_edges[upBinSkew] + (binSize / 2)
    print(
        "No Change Data Range (Skewness): ["
        + str(lowerThresSkew)
        + ","
        + str(upperThresSkew)
        + "]"
    )

    lowBinComb = minComb[0]
    lowerThresComb = bin_edges[lowBinComb] + (binSize / 2)
    upBinComb = numBins - minComb[1]
    upperThresComb = bin_edges[upBinComb] + (binSize / 2)
    print(
        "No Change Data Range (Combined): ["
        + str(lowerThresComb)
        + ","
        + str(upperThresComb)
        + "]"
    )

    lowerThres = 0.0
    upperThres = 0.0
    if thresMeasure == RSGISRATThresMeasure.kurtosis:
        lowerThres = lowerThresKurt
        upperThres = upperThresKurt
    elif thresMeasure == RSGISRATThresMeasure.skewness:
        lowerThres = lowerThresSkew
        upperThres = upperThresSkew
    elif thresMeasure == RSGISRATThresMeasure.combined:
        lowerThres = lowerThresComb
        upperThres = upperThresComb
    elif thresMeasure == RSGISRATThresMeasure.auto:
        if (abs(lowerThresKurt - lowerThresSkew) > (uq - lq)) or (
            abs(upperThresKurt - upperThresSkew) > (uq - lq)
        ):
            lowerThres = lowerThresSkew
            upperThres = upperThresSkew
        else:
            lowerThres = lowerThresComb
            upperThres = upperThresComb
        print(
            "No Change Data Range (auto): ["
            + str(lowerThres)
            + ","
            + str(upperThres)
            + "]"
        )
    else:
        raise Exception(
            "Don't understand metric for threshold provided "
            "must be of type ThresMeasure"
        )

    if not exportPlot == None:
        center = (bin_edges[:-1] + bin_edges[1:]) / 2
        plt.bar(center, hist, align="center", width=binSize)
        if showAllThreshPlot:
            plt.vlines(
                lowerThresKurt,
                0,
                numpy.max(hist),
                color="y",
                linewidth=1,
                label="Kurtosis Lower",
            )
            plt.vlines(
                upperThresKurt,
                0,
                numpy.max(hist),
                color="y",
                linewidth=1,
                label="Kurtosis Upper",
            )
            plt.vlines(
                lowerThresSkew,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Skewness Lower",
            )
            plt.vlines(
                upperThresSkew,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Skewness Upper",
            )
            plt.vlines(
                lowerThresComb,
                0,
                numpy.max(hist),
                color="g",
                linewidth=1,
                label="Combined Lower",
            )
            plt.vlines(
                upperThresComb,
                0,
                numpy.max(hist),
                color="g",
                linewidth=1,
                label="Combined Upper",
            )
        else:
            plt.vlines(
                lowerThres,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Lower Threshold",
            )
            plt.vlines(
                upperThres,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Upper Threshold",
            )
        plt.grid(True)
        plt.legend(loc=0)
        plt.savefig(exportPlot)
        plt.close()

    ## Apply to RAT...
    changeFeats = numpy.where(vals < lowerThres, 1, 0)
    changeFeats = numpy.where(vals > upperThres, 2, changeFeats)

    outChangeFeats[ID] = changeFeats
    rat.writeColumn(ratDataset, outChangeFeatCol, outChangeFeats)

    ratDataset = None
    return [lowerThres, upperThres]


def find_change_clumps_hist_skew_kurt_test_lower(
    inputClumps,
    inClassCol,
    classOfInterest,
    changeVarCol,
    outChangeFeatCol,
    noDataVals=[],
    thresMeasure=RSGISRATThresMeasure.auto,
    exportPlot=None,
    showAllThreshPlot=False,
):
    """
    This function identifies potential change features from just the lower (left)
    side of the histogram by slicing the histogram and finding an optimal skewness
    and kurtosis.

    :param inputClumps: input clumps file.
    :param inClassCol: The column specifiying the classes, one of which change will
                       be found.
    :param classOfInterest: The class (as defined in inClassCol) on which changed
                            is being found.
    :param changeVarCol: changeVarCol - Variable(s) to be used to find change.
                         Expecting column name. Needs to be numeric. If a list
                         of column names is provided then they are combined using
                         PCA and the first PC is used for the change process.
    :param outChangeFeatCol: the output column. Regions lower than lower threshold
                             have value 1. Regions higher than upper threshold
                             have value 2. No change had threshold 0.
    :param noDataVals: list of no data values to be ignored.
    :param thresMeasure: needs to be of type RSGISRATThresMeasure (default is auto)
    :param exportPlot: file name for exporting a histogram plot with thresholds
                       annotated. No plot is create if None is passed (default is none).
    :param showAllThreshPlot: option when plotting to put all the thresholds on to
                              the plot rather than just the one being used.
    :returns: list of lower [0] and upper [1] thresholds used to define the no
              change region.

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this "
            "function could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )
    # Check scipy stats is available
    if not haveScipyStats:
        raise Exception(
            "The scipy stats is required for this function could not be imported\n\t"
            + scipystatsErr
        )
    if not exportPlot == None:
        # Check matplotlib is available
        if not haveMatPlotLib:
            raise Exception(
                "The matplotlib module is required for this function "
                "could not be imported\n\t" + pltErr
            )
    if type(changeVarCol) is list:
        if not haveSKLearnPCA:
            raise Exception(
                "The scikit learn library PCA module is required when a "
                "list of column variables is given\n\t" + sklearnPCAErr
            )
    ## Open the image file...
    ratDataset = gdal.Open(inputClumps, gdal.GA_Update)

    ## Read in columns
    classVals = rat.readColumn(ratDataset, inClassCol)
    outChangeFeats = numpy.zeros_like(classVals)
    ID = numpy.arange(classVals.shape[0])

    vals = None
    if type(changeVarCol) is list:
        numVars = len(changeVarCol)
        numRows = classVals.shape[0]
        varVals = numpy.zeros((numVars, numRows), dtype=numpy.float)
        i = 0
        for varCol in changeVarCol:
            colVals = rat.readColumn(ratDataset, varCol)
            varVals[i] = colVals
            i = i + 1
        varVals = varVals.transpose()

        ID = ID[classVals == classOfInterest]
        varVals = varVals[(classVals == classOfInterest)]

        ID = ID[numpy.isfinite(varVals).all(axis=1)]
        varVals = varVals[numpy.isfinite(varVals).all(axis=1)]

        for noDataVal in noDataVals:
            ID = ID[(varVals != noDataVal).all(axis=1)]
            varVals = varVals[(varVals != noDataVal).all(axis=1)]

        pca = PCA(n_components=1)
        fittedPCA = pca.fit(varVals)

        vals = fittedPCA.transform(varVals)[:, 0]
    else:
        vals = rat.readColumn(ratDataset, changeVarCol)

        ID = ID[classVals == classOfInterest]
        if ID.shape[0] == 0:
            rat.writeColumn(ratDataset, outChangeFeatCol, outChangeFeats)
            return
        vals = vals[classVals == classOfInterest]

        ID = ID[numpy.isfinite(vals)]
        vals = vals[numpy.isfinite(vals)]

        for noDataVal in noDataVals:
            ID = ID[vals != noDataVal]
            vals = vals[vals != noDataVal]

    n = vals.shape[0]
    lq = numpy.percentile(vals, 25)
    uq = numpy.percentile(vals, 75)
    iqr = uq - lq
    binSize = 2 * iqr * n ** (-1 / 3)
    print("Bin Size = ", binSize)
    numBins = int((numpy.max(vals) - numpy.min(vals)) / binSize) + 2
    print("num of bins = ", numBins)

    hist, bin_edges = numpy.histogram(vals, bins=numBins)

    print(hist.shape)
    print(bin_edges.shape)

    print("LQ = ", lq)
    print("UQ = ", uq)

    lqNumBins = int((lq - bin_edges[0]) / binSize) + 1

    print("lqNumBins = ", lqNumBins)

    kurtosisVals = numpy.zeros((lqNumBins), dtype=numpy.float)
    skewnessVals = numpy.zeros((lqNumBins), dtype=numpy.float)
    lowBins = numpy.zeros((lqNumBins), dtype=numpy.int)

    for lowBin in range(lqNumBins):
        histTmp = hist[lowBin:-1]
        lowBins[lowBin] = lowBin

        kurtosisVals[lowBin] = scipy.stats.kurtosis(histTmp)
        skewnessVals[lowBin] = scipy.stats.skew(histTmp)

    kurtosisValsAbs = numpy.absolute(kurtosisVals)
    skewnessValsAbs = numpy.absolute(skewnessVals)
    print(
        "Kurtosis Range: ["
        + str(numpy.min(kurtosisValsAbs))
        + ", "
        + str(numpy.max(kurtosisValsAbs))
        + "]"
    )
    print(
        "Skewness Range: ["
        + str(numpy.min(skewnessValsAbs))
        + ", "
        + str(numpy.max(skewnessValsAbs))
        + "]"
    )
    kurtosisValsNorm = (kurtosisValsAbs - numpy.min(kurtosisValsAbs)) / (
        numpy.max(kurtosisValsAbs) - numpy.min(kurtosisValsAbs)
    )
    skewnessValsNorm = (skewnessValsAbs - numpy.min(skewnessValsAbs)) / (
        numpy.max(skewnessValsAbs) - numpy.min(skewnessValsAbs)
    )

    combined = kurtosisValsNorm + skewnessValsNorm

    minKurt = numpy.argmin(kurtosisValsAbs)
    minSkew = numpy.argmin(skewnessValsAbs)
    minComb = numpy.argmin(combined)

    print("Kurtosis bin index: ", minKurt)
    print("Skewness bin index: ", minSkew)
    print("Combined bin index: ", minComb)

    lowBinKurt = minKurt
    lowerThresKurt = bin_edges[lowBinKurt] + (binSize / 2)
    print("Lower Threshold (Kurtosis): " + str(lowerThresKurt))

    lowBinSkew = minSkew
    lowerThresSkew = bin_edges[lowBinSkew] + (binSize / 2)
    print("Lower Threshold (Skewness): " + str(lowerThresSkew))

    lowBinComb = minComb
    lowerThresComb = bin_edges[lowBinComb] + (binSize / 2)
    print("Lower Threshold (Combined): " + str(lowerThresComb))

    lowerThres = 0.0
    upperThres = numpy.max(vals)
    if thresMeasure == RSGISRATThresMeasure.kurtosis:
        lowerThres = lowerThresKurt
    elif thresMeasure == RSGISRATThresMeasure.skewness:
        lowerThres = lowerThresSkew
    elif thresMeasure == RSGISRATThresMeasure.combined:
        lowerThres = lowerThresComb
    elif thresMeasure == RSGISRATThresMeasure.auto:
        if abs(lowerThresKurt - lowerThresSkew) > (uq - lq):
            lowerThres = lowerThresSkew
        else:
            lowerThres = lowerThresComb
        print("Lower Threshold (auto): " + str(lowerThres))
    else:
        raise Exception(
            "Don't understand metric for threshold provided "
            "must be of type ThresMeasure"
        )

    if not exportPlot == None:
        center = (bin_edges[:-1] + bin_edges[1:]) / 2
        plt.bar(center, hist, align="center", width=binSize)
        if showAllThreshPlot:
            plt.vlines(
                lowerThresKurt,
                0,
                numpy.max(hist),
                color="y",
                linewidth=1,
                label="Kurtosis Lower",
            )
            plt.vlines(
                lowerThresSkew,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Skewness Lower",
            )
            plt.vlines(
                lowerThresComb,
                0,
                numpy.max(hist),
                color="g",
                linewidth=1,
                label="Combined Lower",
            )
        else:
            plt.vlines(
                lowerThres,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Lower Threshold",
            )
        plt.grid(True)
        plt.legend(loc=0)
        plt.savefig(exportPlot)
        plt.close()

    ## Apply to RAT...
    changeFeats = numpy.where(vals < lowerThres, 1, 0)

    outChangeFeats[ID] = changeFeats
    rat.writeColumn(ratDataset, outChangeFeatCol, outChangeFeats)

    ratDataset = None
    return [lowerThres, upperThres]


def find_change_clumps_hist_skew_kurt_test_upper(
    inputClumps,
    inClassCol,
    classOfInterest,
    changeVarCol,
    outChangeFeatCol,
    noDataVals=[],
    thresMeasure=RSGISRATThresMeasure.auto,
    exportPlot=None,
    showAllThreshPlot=False,
):
    """
    This function identifies potential change features from just the upper
    (right) side of the histogram by slicing the histogram and finding an optimal
    skewness and kurtosis.

    Where:

    :param inputClumps: input clumps file.
    :param inClassCol: The column specifiying the classes, one of which change
                       will be found.
    :param classOfInterest: The class (as defined in inClassCol) on which changed
                            is being found.
    :param changeVarCol: changeVarCol - Variable(s) to be used to find change.
                         Expecting column name. Needs to be numeric. If a list of
                         column names is provided then they are combined using PCA
                         and the first PC is used for the change process.
    :param outChangeFeatCol: the output column. Regions lower than lower threshold
                             have value 1. Regions higher than upper threshold have
                             value 2. No change had threshold 0.
    :param noDataVals: list of no data values to be ignored.
    :param thresMeasure: needs to be of type RSGISRATThresMeasure (default is auto)
    :param exportPlot: file name for exporting a histogram plot with thresholds
                       annotated. No plot is create if None is passed (default is none).
    :param showAllThreshPlot: option when plotting to put all the thresholds on to the
                              plot rather than just the one being used.
    :returns: list of lower [0] and upper [1] thresholds used to define the
              no change region.

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function could "
            "not be imported\n\t" + riosRatErr
        )
    # Check scipy stats is available
    if not haveScipyStats:
        raise Exception(
            "The scipy stats is required for this function could not be imported\n\t"
            + scipystatsErr
        )
    if not exportPlot == None:
        # Check matplotlib is available
        if not haveMatPlotLib:
            raise Exception(
                "The matplotlib module is required for this function "
                "could not be imported\n\t" + pltErr
            )
    if type(changeVarCol) is list:
        if not haveSKLearnPCA:
            raise Exception(
                "The scikit learn library PCA module is required when a list of "
                "column variables is given\n\t" + sklearnPCAErr
            )
    ## Open the image file...
    ratDataset = gdal.Open(inputClumps, gdal.GA_Update)

    ## Read in columns
    classVals = rat.readColumn(ratDataset, inClassCol)
    outChangeFeats = numpy.zeros_like(classVals)
    ID = numpy.arange(classVals.shape[0])

    vals = None
    if type(changeVarCol) is list:
        numVars = len(changeVarCol)
        numRows = classVals.shape[0]
        varVals = numpy.zeros((numVars, numRows), dtype=numpy.float)
        i = 0
        for varCol in changeVarCol:
            colVals = rat.readColumn(ratDataset, varCol)
            varVals[i] = colVals
            i = i + 1
        varVals = varVals.transpose()

        ID = ID[classVals == classOfInterest]
        varVals = varVals[(classVals == classOfInterest)]

        ID = ID[numpy.isfinite(varVals).all(axis=1)]
        varVals = varVals[numpy.isfinite(varVals).all(axis=1)]

        for noDataVal in noDataVals:
            ID = ID[(varVals != noDataVal).all(axis=1)]
            varVals = varVals[(varVals != noDataVal).all(axis=1)]

        pca = PCA(n_components=1)
        fittedPCA = pca.fit(varVals)

        vals = fittedPCA.transform(varVals)[:, 0]
    else:
        vals = rat.readColumn(ratDataset, changeVarCol)

        ID = ID[classVals == classOfInterest]
        if ID.shape[0] == 0:
            rat.writeColumn(ratDataset, outChangeFeatCol, outChangeFeats)
            return
        vals = vals[classVals == classOfInterest]

        ID = ID[numpy.isfinite(vals)]
        vals = vals[numpy.isfinite(vals)]

        for noDataVal in noDataVals:
            ID = ID[vals != noDataVal]
            vals = vals[vals != noDataVal]

    n = vals.shape[0]
    lq = numpy.percentile(vals, 25)
    uq = numpy.percentile(vals, 75)
    iqr = uq - lq
    binSize = 2 * iqr * n ** (-1 / 3)
    print("Bin Size = ", binSize)
    numBins = int((numpy.max(vals) - numpy.min(vals)) / binSize) + 2
    print("num of bins = ", numBins)

    hist, bin_edges = numpy.histogram(vals, bins=numBins)

    print(hist.shape)
    print(bin_edges.shape)

    print("LQ = ", lq)
    print("UQ = ", uq)

    uqNumBins = int((bin_edges[-1] - uq) / binSize) + 1

    print("uqNumBins = ", uqNumBins)

    kurtosisVals = numpy.zeros((uqNumBins), dtype=numpy.float)
    skewnessVals = numpy.zeros((uqNumBins), dtype=numpy.float)
    upBins = numpy.zeros((uqNumBins), dtype=numpy.int)

    for upBin in range(uqNumBins):
        histTmp = hist[0 : (numBins - upBin)]
        upBins[upBin] = numBins - upBin

        kurtosisVals[upBin] = scipy.stats.kurtosis(histTmp)
        skewnessVals[upBin] = scipy.stats.skew(histTmp)

    kurtosisValsAbs = numpy.absolute(kurtosisVals)
    skewnessValsAbs = numpy.absolute(skewnessVals)
    print(
        "Kurtosis Range: ["
        + str(numpy.min(kurtosisValsAbs))
        + ", "
        + str(numpy.max(kurtosisValsAbs))
        + "]"
    )
    print(
        "Skewness Range: ["
        + str(numpy.min(skewnessValsAbs))
        + ", "
        + str(numpy.max(skewnessValsAbs))
        + "]"
    )
    kurtosisValsNorm = (kurtosisValsAbs - numpy.min(kurtosisValsAbs)) / (
        numpy.max(kurtosisValsAbs) - numpy.min(kurtosisValsAbs)
    )
    skewnessValsNorm = (skewnessValsAbs - numpy.min(skewnessValsAbs)) / (
        numpy.max(skewnessValsAbs) - numpy.min(skewnessValsAbs)
    )

    combined = kurtosisValsNorm + skewnessValsNorm

    minKurt = numpy.argmin(kurtosisValsAbs)
    minSkew = numpy.argmin(skewnessValsAbs)
    minComb = numpy.argmin(combined)

    print("Kurtosis bin index: ", minKurt)
    print("Skewness bin index: ", minSkew)
    print("Combined bin index: ", minComb)

    upBinKurt = numBins - minKurt
    upperThresKurt = bin_edges[upBinKurt] + (binSize / 2)
    print("Upper Threshold (Kurtosis): " + str(upperThresKurt))

    upBinSkew = numBins - minSkew
    upperThresSkew = bin_edges[upBinSkew] + (binSize / 2)
    print("Upper Threshold (Skewness): " + str(upperThresSkew))

    upBinComb = numBins - minComb
    upperThresComb = bin_edges[upBinComb] + (binSize / 2)
    print("Upper Threshold (Combined): " + str(upperThresComb))

    lowerThres = numpy.min(vals)
    upperThres = 0.0
    if thresMeasure == RSGISRATThresMeasure.kurtosis:
        upperThres = upperThresKurt
    elif thresMeasure == RSGISRATThresMeasure.skewness:
        upperThres = upperThresSkew
    elif thresMeasure == RSGISRATThresMeasure.combined:
        upperThres = upperThresComb
    elif thresMeasure == RSGISRATThresMeasure.auto:
        if abs(upperThresKurt - upperThresSkew) > (uq - lq):
            upperThres = upperThresSkew
        else:
            upperThres = upperThresComb
        print("Upper Threshold (auto): " + str(upperThres))
    else:
        raise Exception(
            "Don't understand metric for threshold provided "
            "must be of type ThresMeasure"
        )

    if not exportPlot == None:
        center = (bin_edges[:-1] + bin_edges[1:]) / 2
        plt.bar(center, hist, align="center", width=binSize)
        if showAllThreshPlot:
            plt.vlines(
                upperThresKurt,
                0,
                numpy.max(hist),
                color="y",
                linewidth=1,
                label="Kurtosis Upper",
            )
            plt.vlines(
                upperThresSkew,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Skewness Upper",
            )
            plt.vlines(
                upperThresComb,
                0,
                numpy.max(hist),
                color="g",
                linewidth=1,
                label="Combined Upper",
            )
        else:
            plt.vlines(
                upperThres,
                0,
                numpy.max(hist),
                color="r",
                linewidth=1,
                label="Upper Threshold",
            )
        plt.grid(True)
        plt.legend(loc=0)
        plt.savefig(exportPlot)
        plt.close()

    ## Apply to RAT...
    changeFeats = numpy.where(vals > upperThres, 1, 0)

    outChangeFeats[ID] = changeFeats
    rat.writeColumn(ratDataset, outChangeFeatCol, outChangeFeats)

    ratDataset = None
    return [lowerThres, upperThres]


class RSGISRATChangeVarInfo:
    """
    A class to store the change variable information required for some
    of the change functions.
    """

    def __init__(
        self,
        changeVarCol="",
        outChangeFeatCol="",
        noDataVals=[],
        thresMeasure=RSGISRATThresMeasure.auto,
        thresDirection=RSGISRATThresDirection.lower,
        exportPlot=None,
        showAllThreshPlot=False,
        lowerThreshold=0.0,
        upperThreshold=0.0,
    ):
        self.changeVarCol = changeVarCol
        self.outChangeFeatCol = outChangeFeatCol
        self.noDataVals = noDataVals
        self.thresMeasure = thresMeasure
        self.thresDirection = thresDirection
        self.exportPlot = exportPlot
        self.showAllThreshPlot = showAllThreshPlot
        self.lowerThreshold = lowerThreshold
        self.upperThreshold = upperThreshold


def find_change_clumps_hist_skew_kurt_test_vote_multi_vars(
    clumps_img, in_class_col, class_of_interest, out_change_feat_col, vars=[]
):
    """
    A function to call one of the find_change_clumps_hist_skew_kurt_test functions
    for multiple variables and then combine together by voting to find change features.

    Where:

    :param clumps_img: input clumps file.
    :param in_class_col: The column specifiying the classes, one of which change
                       will be found.
    :param class_of_interest: The class (as defined in inClassCol) on which changed
                            is being found.
    :param out_change_feat_col: the output column with the vote scores.
    :param vars: a list of RSGISRATChangeVarInfo objects used to specify the
                 variables and function to be called.

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for "
            "this function could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this "
            "function could not be imported\n\t" + riosRatErr
        )
    if len(vars) == 0:
        raise Exception("Need to provide a list of variables with parameters...")
    for var in vars:
        print(var.changeVarCol)
        if var.thresDirection == RSGISRATThresDirection.lower:
            outThres = find_change_clumps_hist_skew_kurt_test_lower(
                clumps_img,
                in_class_col,
                class_of_interest,
                var.changeVarCol,
                var.outChangeFeatCol,
                var.noDataVals,
                var.thresMeasure,
                var.exportPlot,
                var.showAllThreshPlot,
            )
            var.lowerThreshold = outThres[0]
            var.upperThreshold = outThres[1]
        elif var.thresDirection == RSGISRATThresDirection.upper:
            outThres = find_change_clumps_hist_skew_kurt_test_upper(
                clumps_img,
                in_class_col,
                class_of_interest,
                var.changeVarCol,
                var.outChangeFeatCol,
                var.noDataVals,
                var.thresMeasure,
                var.exportPlot,
                var.showAllThreshPlot,
            )
            var.lowerThreshold = outThres[0]
            var.upperThreshold = outThres[1]
        elif var.thresDirection == RSGISRATThresDirection.lowerupper:
            outThres = find_change_clumps_hist_skew_kurt_test(
                clumps_img,
                in_class_col,
                class_of_interest,
                var.changeVarCol,
                var.outChangeFeatCol,
                var.noDataVals,
                var.thresMeasure,
                var.exportPlot,
                var.showAllThreshPlot,
            )
            var.lowerThreshold = outThres[0]
            var.upperThreshold = outThres[1]
        else:
            raise Exception(
                "Direction must be of type RSGISRATThresDirection and "
                "only lower, upper and lowerupper are supported"
            )

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    classVals = rat.readColumn(ratDataset, in_class_col)
    changeVote = numpy.zeros_like(classVals, dtype=numpy.int)

    for var in vars:
        changeCol = rat.readColumn(ratDataset, var.outChangeFeatCol)
        if var.thresDirection == RSGISRATThresDirection.lowerupper:
            changeCol[changeCol == 2] = 1
        changeVote = changeVote + changeCol

    rat.writeColumn(ratDataset, out_change_feat_col, changeVote)
    ratDataset = None


def find_clumps_within_existing_thresholds(
    clumps_img, in_class_col, class_of_interest, out_feats_col, vars=[]
):
    """
    A function to use the thresholds stored in the RSGISRATChangeVarInfo objects (var)
    and populated from the find_change_clumps_hist_skew_kurt_test functions to assess
    another class creating a binary column as to whether a feature is within the
    threshold or now. Where multiple variables (i.e., len(var) > 1) then variables
    are combined with an and operation.

    Where:

    :param clumps_img: input clumps file.
    :param in_class_col: The column specifiying the classes, one of which change
                       will be found.
    :param class_of_interest: The class (as defined in inClassCol) on which changed
                            is being found.
    :param out_feats_col: the output binary column specifying whether a feature is
                        within the thresholds.
    :param vars: a list of RSGISRATChangeVarInfo objects used to specify the
                 variables and function to be called.

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function could not be imported\n\t"
            + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this "
            "function could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this "
            "function could not be imported\n\t" + riosRatErr
        )
    if len(vars) == 0:
        raise Exception("Need to provide a list of variables with parameters...")

    ## Open the image file...
    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)

    ## Read in columns
    classVals = rat.readColumn(ratDataset, in_class_col)
    outFeats = numpy.zeros_like(classVals)

    first = True
    for var in vars:
        print(var.changeVarCol)
        if first:
            varVals = rat.readColumn(ratDataset, var.changeVarCol)
            outFeats = numpy.where(
                (varVals > var.lowerThreshold)
                & (varVals < var.upperThreshold)
                & (classVals == class_of_interest),
                1,
                outFeats,
            )
            first = False
        else:
            varVals = rat.readColumn(ratDataset, var.changeVarCol)
            outFeats = numpy.where(
                (varVals > var.lowerThreshold)
                & (varVals < var.upperThreshold)
                & (classVals == class_of_interest),
                outFeats,
                0,
            )

    rat.writeColumn(ratDataset, out_feats_col, outFeats)
    ratDataset = None


def define_class_names(clumps_img, class_num_col, class_name_col, class_names_dict):
    """
    A function to create a class names column in a RAT based on segmented clumps
     where a number of clumps have the same number class.

    :param clumps_img: input clumps image.
    :param class_num_col: column specifying the class number (e.g., where clumps
                        are segments in a segmentation)
    :param class_name_col: the output column name where a string will be created if
                         it doesn't already exists.
    :param class_names_dict: Dictionary to look up the class names. The key needs to
                           the integer number for the class

    """
    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()

    in_rats.inrat = ratapplier.RatHandle(clumps_img)
    out_rats.outrat = ratapplier.RatHandle(clumps_img)

    otherargs = ratapplier.OtherArguments()
    otherargs.classNumCol = class_num_col
    otherargs.classNameCol = class_name_col
    otherargs.classNamesDict = class_names_dict

    def _ratapplier_defClassNames(info, inputs, outputs, otherargs):
        classNum = getattr(inputs.inrat, otherargs.classNumCol)

        classNames = numpy.empty_like(classNum, dtype=numpy.dtype("a255"))
        classNames[...] = ""

        for key in otherargs.classNamesDict:
            classNames = numpy.where(
                (classNum == key), otherargs.classNamesDict[key], classNames
            )

        setattr(outputs.outrat, otherargs.classNameCol, classNames)

    ratapplier.apply(_ratapplier_defClassNames, in_rats, out_rats, otherargs)


def set_class_names_colours(clumps_img, class_names_col, class_info_dict):
    """
    A function to define a class names column and define the class colours.

    classInfoDict = dict()
    classInfoDict[1] = {'classname':'Forest', 'red':0, 'green':255, 'blue':0}
    classInfoDict[2] = {'classname':'Water', 'red':0, 'green':0, 'blue':255}

    :param clumps_img: Input clumps image - expecting a classification
                      (rather than segmentation) where the number is the pixel value.
    :param class_names_col: The output column for the class names.
    :param class_info_dict: a dict where the key is the pixel value for the class.
    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    n_rows = rsgislib.rastergis.get_rat_length(clumps_img)
    col_names = rsgislib.rastergis.get_rat_columns(clumps_img)

    red_avail = False
    green_avail = False
    blue_avail = False
    if "Red" in col_names:
        red_avail = True
    if "Green" in col_names:
        green_avail = True
    if "Blue" in col_names:
        blue_avail = True

    class_names_col_avail = False
    if class_names_col in col_names:
        class_names_col_avail = True

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)

    if red_avail:
        red_arr = rat.readColumn(ratDataset, "Red")
    else:
        red_arr = numpy.zeros(n_rows, dtype=int)

    if green_avail:
        green_arr = rat.readColumn(ratDataset, "Green")
    else:
        green_arr = numpy.zeros(n_rows, dtype=int)

    if blue_avail:
        blue_arr = rat.readColumn(ratDataset, "Blue")
    else:
        blue_arr = numpy.zeros(n_rows, dtype=int)

    if class_names_col_avail:
        class_names_arr = rat.readColumn(ratDataset, class_names_col)
    else:
        class_names_arr = numpy.zeros(n_rows, dtype=numpy.dtype("a255"))

    for class_key in class_info_dict:
        if (class_key >= 0) and (class_key < n_rows):
            class_names_arr[class_key] = class_info_dict[class_key]["classname"]
            red_arr[class_key] = class_info_dict[class_key]["red"]
            green_arr[class_key] = class_info_dict[class_key]["green"]
            blue_arr[class_key] = class_info_dict[class_key]["blue"]
        else:
            print(
                "Class key ({}) was not within the number of rows in the RAT.".format(
                    class_key
                ),
                file=sys.stderr,
            )

    rat.writeColumn(ratDataset, class_names_col, class_names_arr)
    rat.writeColumn(ratDataset, "Red", red_arr)
    rat.writeColumn(ratDataset, "Green", green_arr)
    rat.writeColumn(ratDataset, "Blue", blue_arr)

    ratDataset = None


def create_clumps_shp_bbox(
    clumps_img,
    min_x_col,
    max_x_col,
    min_y_col,
    max_y_col,
    out_shp_lyr_name,
    round_int=False,
    ignore_first_row=False,
):
    """
    A function to create a shapefile of polygons with the bboxs of the clumps
    defined using the minX, maxX, minY and maxY coordinates for the features.

    :param clumps_img: input clumps file.
    :param min_x_col: the minX column in RAT.
    :param max_x_col: the maxX column in RAT.
    :param min_y_col: the minY column in RAT.
    :param max_y_col: the maxY column in RAT.
    :param out_shp_lyr_name: The output shapefile name (layer name do not include
                          the .shp it will be appended).
    :param round_int: Boolean specifying whether the coordinated should be rounded to
                     integers (Default: False)

    """

    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )
    # Check gdal ogr is available
    if not haveGDALOGRPy:
        raise Exception(
            "The GDAL OGR python bindings are required for this function "
            "could not be imported\n\t" + gdalogrErr
        )

    ratDataset = gdal.Open(clumps_img)

    minXVals = rat.readColumn(ratDataset, min_x_col)
    maxXVals = rat.readColumn(ratDataset, max_x_col)
    minYVals = rat.readColumn(ratDataset, min_y_col)
    maxYVals = rat.readColumn(ratDataset, max_y_col)

    fidVals = numpy.arange(maxYVals.shape[0])

    ## Remove First Row which is no data...
    if ignore_first_row:
        minXVals = numpy.delete(minXVals, (0), axis=0)
        maxXVals = numpy.delete(maxXVals, (0), axis=0)
        minYVals = numpy.delete(minYVals, (0), axis=0)
        maxYVals = numpy.delete(maxYVals, (0), axis=0)
        fidVals = numpy.delete(fidVals, (0), axis=0)

    ## Remove any features which are all zero (i.e., polygon not present...
    minXValsSub = minXVals[
        numpy.logical_not(
            (minXVals == 0) & (maxXVals == 0) & (minYVals == 0) & (maxYVals == 0)
        )
    ]
    maxXValsSub = maxXVals[
        numpy.logical_not(
            (minXVals == 0) & (maxXVals == 0) & (minYVals == 0) & (maxYVals == 0)
        )
    ]
    minYValsSub = minYVals[
        numpy.logical_not(
            (minXVals == 0) & (maxXVals == 0) & (minYVals == 0) & (maxYVals == 0)
        )
    ]
    maxYValsSub = maxYVals[
        numpy.logical_not(
            (minXVals == 0) & (maxXVals == 0) & (minYVals == 0) & (maxYVals == 0)
        )
    ]
    fidValsSub = fidVals[
        numpy.logical_not(
            (minXVals == 0) & (maxXVals == 0) & (minYVals == 0) & (maxYVals == 0)
        )
    ]

    if round_int:
        minXValsSub = numpy.rint(minXValsSub)
        maxXValsSub = numpy.rint(maxXValsSub)
        minYValsSub = numpy.rint(minYValsSub)
        maxYValsSub = numpy.rint(maxYValsSub)

    numFeats = minXValsSub.shape[0]
    print("Num Feats: ", numFeats)

    driver = ogr.GetDriverByName("ESRI Shapefile")
    if os.path.exists(out_shp_lyr_name + ".shp"):
        driver.DeleteDataSource(out_shp_lyr_name + ".shp")
    outDatasource = driver.CreateDataSource(out_shp_lyr_name + ".shp")
    raster_srs = osr.SpatialReference()
    raster_srs.ImportFromWkt(ratDataset.GetProjectionRef())
    outLayer = outDatasource.CreateLayer(out_shp_lyr_name, srs=raster_srs)

    fieldFIDDefn = ogr.FieldDefn("ID", ogr.OFTInteger)
    fieldFIDDefn.SetWidth(6)
    outLayer.CreateField(fieldFIDDefn)

    print("Create and Add Polygons...")
    for i in range(numFeats):
        wktStr = (
            "POLYGON(("
            + str(minXValsSub[i])
            + " "
            + str(maxYValsSub[i])
            + ", "
            + str(maxXValsSub[i])
            + " "
            + str(maxYValsSub[i])
            + ", "
            + str(maxXValsSub[i])
            + " "
            + str(minYValsSub[i])
            + ", "
            + str(minXValsSub[i])
            + " "
            + str(minYValsSub[i])
            + ", "
            + str(minXValsSub[i])
            + " "
            + str(maxYValsSub[i])
            + "))"
        )
        # print(str(i) + ": " + wktStr)
        poly = ogr.CreateGeometryFromWkt(wktStr)
        feat = ogr.Feature(outLayer.GetLayerDefn())
        feat.SetField("ID", float(fidValsSub[i]))
        feat.SetGeometry(poly)
        if outLayer.CreateFeature(feat) != 0:
            print(str(i) + ": " + wktStr)
            print("Failed to create feature in shapefile.\n")
            sys.exit(1)
        feat.Destroy()

    outDatasource.Destroy()
    ratDataset = None
    print("Completed")


def identify_small_units(
    clumps_img,
    class_col,
    tmp_dir,
    out_col_name,
    small_clumps_thres,
    use_tiled_clump=False,
    n_cores=1,
    tile_width=2000,
    tile_height=2000,
):
    """
    Identify small connected units within a classification. The threshold to define
    small is provided by the user in pixels. Note, the outColName and smallClumpsThres
    variables can be provided as lists to identify a number of thresholds of
    small units.

    :param clumps_img: string for the clumps image file containing input classification
    :param class_col: string for the column name representing the classification as
                     integer values
    :param tmp_dir: directory path where temporary layers are stored (if directory is
                    created within the function it will be deleted once function
                    is complete).
    :param out_col_name: a list of output column names (i.e., one for each threshold)
    :param small_clumps_thres: a list of thresholds for identifying small clumps.
    :param use_tiled_clump: a boolean to specify whether the tiled clumping algorithm
                          should be used (Default is False; select True for large
                          datasets)
    :param n_cores: if the tiled version of the clumping algorithm is being used then
                   there is an option to use multiple processing cores; specify the
                   number to be used (Default is 2).
    :param tile_width: is the width of the image tile (in pixels) if tiled clumping
                      is used.
    :param tile_height: is the height of the image tile (in pixels) if tiled clumping
                       is used.

    Example:

    .. code:: python
        from rsgislib.rastergis import ratutils

        clumpsImg = "LS2MSS_19750620_lat10lon6493_r67p250_rad_srefdem_30m_clumps.kea"
        tmpPath = "./tmp/"
        classCol = "OutClass"
        outColName = ["SmallUnits25", "SmallUnits50", "SmallUnits100"]
        smallClumpsThres = [25, 50, 100]
        rastergis.identify_small_units(clumpsImg, classCol, tmpPath,
                                       outColName, smallClumpsThres)

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    import rsgislib.tools.filetools

    if len(out_col_name) is not len(small_clumps_thres):
        print(
            "The number of threshold values and output "
            "column names should be the same."
        )
        sys.exit(-1)

    numThresholds = len(small_clumps_thres)

    createdDIR = False
    if not os.path.isdir(tmp_dir):
        os.makedirs(tmp_dir)
        createdDIR = True

    baseName = os.path.splitext(os.path.basename(clumps_img))[0]
    classMaskImg = os.path.join(tmp_dir, baseName + "_TmpClassMask.kea")
    classMaskClumps = os.path.join(tmp_dir, baseName + "_TmpClassMaskClumps.kea")
    smallClumpsMask = os.path.join(tmp_dir, baseName + "_SmallClassClumps.kea")

    rastergis.export_col_to_gdal_img(
        clumps_img, classMaskImg, "KEA", rsgislib.TYPE_16UINT, class_col
    )
    if use_tiled_clump:
        from rsgislib.segmentation import tiledclump

        if n_cores > 1:
            tiledclump.perform_clumping_multi_process(
                classMaskImg,
                classMaskClumps,
                tmpDIR=os.path.join(tmp_dir, baseName + "_ClumpTmp"),
                width=tile_width,
                height=tile_height,
                nCores=n_cores,
            )
        else:
            tiledclump.perform_clumping_single_thread(
                classMaskImg,
                classMaskClumps,
                tmpDIR=os.path.join(
                    tmp_dir,
                    baseName + "_ClumpTmp",
                    width=tile_width,
                    height=tile_height,
                ),
            )
    else:
        segmentation.clump(classMaskImg, classMaskClumps, "KEA", False, 0)
    rastergis.pop_rat_img_stats(classMaskClumps, False, False)

    for i in range(numThresholds):
        print(
            "Processing thresold "
            + str(small_clumps_thres[i])
            + " - "
            + out_col_name[i]
        )
        ratDataset = gdal.Open(classMaskClumps, gdal.GA_Update)
        Histogram = rat.readColumn(ratDataset, "Histogram")
        smallUnits = numpy.zeros_like(Histogram, dtype=numpy.int16)
        smallUnits[Histogram < small_clumps_thres[i]] = 1
        rat.writeColumn(ratDataset, "smallUnits", smallUnits)
        ratDataset = None

        rastergis.export_col_to_gdal_img(
            classMaskClumps, smallClumpsMask, "KEA", rsgislib.TYPE_8UINT, "smallUnits"
        )

        bs = []
        bs.append(rastergis.BandAttStats(band=1, maxField=out_col_name[i]))
        rastergis.populate_rat_with_stats(smallClumpsMask, clumps_img, bs)

    rsgislib.tools.filetools.delete_file_with_basename(classMaskImg)
    rsgislib.tools.filetools.delete_file_with_basename(classMaskClumps)
    rsgislib.tools.filetools.delete_file_with_basename(smallClumpsMask)
    if createdDIR:
        shutil.rmtree(tmp_dir)


def _computeProximityArrArgsFunc(arg_vals):
    """
    This function is used internally within calc_dist_to_classes for the multiprocessing Pool

    """
    classImgDS = gdal.Open(arg_vals[0], gdal.GA_ReadOnly)
    classImgBand = classImgDS.GetRasterBand(1)
    imageutils.create_copy_img(
        arg_vals[0], arg_vals[1], 1, arg_vals[3], arg_vals[4], rsgislib.TYPE_32FLOAT
    )
    distImgDS = gdal.Open(arg_vals[1], gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    gdal.ComputeProximity(classImgBand, distImgBand, arg_vals[2], callback=callback)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None


def calc_dist_to_classes(
    clumps_img,
    class_col,
    out_img_base,
    tmp_dir="./tmp",
    tile_size=2000,
    max_dist=1000,
    no_data_val=1000,
    n_cores=-1,
):
    """
    A function which will calculate proximity rasters for a set of classes
    defined within the RAT.

    :param clumps_img: is a string specifying the input image with the associated RAT
    :param class_col: is the column in the RAT which has the classification
    :param out_img_base: is the base name of the output image - output files will
                       be KEA files.
    :param tmp_dir: is a directory to be used for storing the image tiles and
                   other temporary files - if not directory does not exist it
                   will be created and deleted on completion (Default: ./tmp).
    :param tile_size: is an int specifying in pixels the size of the image tiles
                     used for processing (Default: 2000)
    :param max_dist: is the maximum distance in units of the geographic units of
                    the projection of the input image (Default: 1000).
    :param no_data_val: is the value applied to the pixels outside of the maxDist
                   threshold (Default: 1000; i.e., the same as maxDist).
    :param n_cores: is the number of processing cores which are available to be
                   used for this processing. If -1 all available cores will be
                   used. (Default: -1)

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        print("WARNING: '" + tmp_dir + "' directory does not exist so creating it...")
        os.makedirs(tmp_dir)
        tmpPresent = False

    if n_cores <= 0:
        n_cores = multiprocessing.cpu_count()

    uid = rsgislib.tools.utils.uid_generator()

    classesImg = os.path.join(tmp_dir, "ClassImg_" + uid + ".kea")
    rastergis.export_col_to_gdal_img(
        clumps_img, classesImg, "KEA", rsgislib.TYPE_32UINT, class_col
    )

    ratDataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    classColInt = rat.readColumn(ratDataset, class_col)
    ratDataset = None

    classIDs = numpy.unique(classColInt)

    xRes, yRes = rsgislib.imageutils.get_img_res(classesImg)

    # print("Image Res {} x {}".format(xRes, yRes))

    xMaxDistPxl = math.ceil(max_dist / xRes)
    yMaxDistPxl = math.ceil(max_dist / yRes)

    print("Max Dist Pxls X = {}, Y = {}".format(xMaxDistPxl, yMaxDistPxl))

    tileOverlap = xMaxDistPxl
    if yMaxDistPxl > xMaxDistPxl:
        tileOverlap = yMaxDistPxl

    classTilesDIR = os.path.join(tmp_dir, "ClassTiles_" + uid)
    classTilesDIRPresent = True
    if not os.path.exists(classTilesDIR):
        os.makedirs(classTilesDIR)
        classTilesDIRPresent = False

    classesImgTileBase = os.path.join(classTilesDIR, "ClassImgTile")
    imageutils.create_tiles(
        classesImg,
        classesImgTileBase,
        tile_size,
        tile_size,
        tileOverlap,
        0,
        "KEA",
        rsgislib.TYPE_32UINT,
        "kea",
    )
    imgTileFiles = glob.glob(classesImgTileBase + "*.kea")

    distTilesDIR = os.path.join(tmp_dir, "DistTiles_" + uid)
    distTilesDIRPresent = True
    if not os.path.exists(distTilesDIR):
        os.makedirs(distTilesDIR)
        distTilesDIRPresent = False

    proxOptionsBase = [
        "MAXDIST=" + str(max_dist),
        "DISTUNITS=GEO",
        "NODATA=" + str(no_data_val),
    ]

    for classID in classIDs:
        print("Class {}".format(classID))
        proxOptions = list(proxOptionsBase)
        proxOptions.append("VALUES=" + str(classID))

        distTiles = []
        distTileArgs = []
        for classTileFile in imgTileFiles:
            baseTileName = os.path.basename(classTileFile)
            distTileFile = os.path.join(distTilesDIR, baseTileName)
            tileArgs = [classTileFile, distTileFile, proxOptions, no_data_val, "KEA"]
            distTiles.append(distTileFile)
            distTileArgs.append(tileArgs)

        with Pool(n_cores) as p:
            p.map(_computeProximityArrArgsFunc, distTileArgs)

        distImage = out_img_base + "_" + str(classID) + ".kea"
        # Mosaic Tiles
        imageutils.create_img_mosaic(
            distTiles,
            distImage,
            no_data_val,
            no_data_val,
            1,
            1,
            "KEA",
            rsgislib.TYPE_32FLOAT,
        )
        imageutils.pop_img_stats(
            distImage, use_no_data=True, no_data_val=no_data_val, calc_pyramids=True
        )
        for imgFile in distTiles:
            rsgislib.tools.filetools.delete_file_with_basename(imgFile)

    if not classTilesDIRPresent:
        shutil.rmtree(classTilesDIR, ignore_errors=True)
    else:
        for classTileFile in imgTileFiles:
            rsgislib.tools.filetools.delete_file_with_basename(classTileFile)

    if not distTilesDIRPresent:
        shutil.rmtree(distTilesDIR, ignore_errors=True)

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)
    else:
        os.remove(classesImg)


def calc_dist_between_clumps(
    clumps_img, out_col_name, tmp_dir="./tmp", use_idx=False, max_dist_thres=10
):
    """
    Calculate the distance between all clumps

    :param clumps_img: image clumps for which the distance will be calculated.
    :param out_col_name: output column within the clumps image.
    :param tmp_dir: directory out tempory files will be outputted to.
    :param use_idx: use a spatial index when calculating the distance between
                   clumps (needed for large number of clumps).
    :param max_dist_thres: if using an index than an upper limit on the distance
                         between clumps can be defined.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        tmpPresent = False

    baseName = "{}_{}".format(
        rsgislib.tools.filetools.get_file_basename(clumps_img),
        rsgislib.tools.utils.uid_generator(),
    )

    polysShp = os.path.join(tmp_dir, baseName + "_shp.shp")
    vectorutils.polygoniseRaster(
        clumps_img, polysShp, imgBandNo=1, maskImg=clumps_img, imgMaskBandNo=1
    )

    print(
        "Calculating Distance - can take some time. Try using index "
        "and decreasing max distance threshold."
    )
    t = rsgislib.RSGISTime()
    t.start(True)
    polysShpGeomDist = os.path.join(tmp_dir, baseName + "_dist_shp.shp")
    vectorutils.dist2NearestGeom(
        polysShp, polysShpGeomDist, out_col_name, True, use_idx, max_dist_thres
    )
    t.end()

    rastergis.import_vec_atts(clumps_img, polysShpGeomDist, [out_col_name])

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def calc_dist_to_large_clumps(
    clumps_img,
    out_col_name,
    size_thres,
    tmp_dir="./tmp",
    use_idx=False,
    max_dist_thres=10,
):
    """
    Calculate the distance from each small clump to a large clump. Split defined by
    the threshold provided.

    :param clumps_img: image clumps for which the distance will be calculated.
    :param out_col_name: output column within the clumps image.
    :param size_thres: is a threshold to seperate the sets of large and small clumps.
    :param tmp_dir: directory out tempory files will be outputted to.
    :param use_idx: use a spatial index when calculating the distance between clumps
                   (needed for large number of clumps).
    :param max_dist_thres: if using an index than an upper limit on the distance
                         between clumps can be defined.

    """
    import rsgislib.tools.utils

    tmpPresent = True
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        tmpPresent = False

    uidStr = rsgislib.tools.utils.uid_generator()
    baseName = "{}_{}".format(
        rsgislib.tools.filetools.get_file_basename(clumps_img), uidStr
    )

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    Histogram = rat.readColumn(ratDataset, "Histogram")
    smallUnits = numpy.zeros_like(Histogram, dtype=numpy.int16)
    smallUnits[Histogram < size_thres] = 1
    rat.writeColumn(ratDataset, "smallUnits", smallUnits)

    ID = numpy.arange(Histogram.shape[0])

    smUnitIDs = ID[smallUnits == 1]
    smUnitIDs = smUnitIDs[smUnitIDs > 0]
    lrgUnitIDs = ID[smallUnits == 0]
    lrgUnitIDs = lrgUnitIDs[lrgUnitIDs > 0]

    print("There are {} small clumps.".format(smUnitIDs.shape[0]))
    print("There are {} large clumps.".format(lrgUnitIDs.shape[0]))

    smUnitFIDs = numpy.arange(smUnitIDs.shape[0])
    lrgUnitFIDs = numpy.arange(lrgUnitIDs.shape[0])

    smUnitClumpIDs = numpy.zeros_like(Histogram, dtype=numpy.int16)
    smUnitClumpIDs[smUnitIDs] = smUnitFIDs
    lrgUnitClumpIDs = numpy.zeros_like(Histogram, dtype=numpy.int16)
    lrgUnitClumpIDs[lrgUnitIDs] = lrgUnitFIDs

    rat.writeColumn(ratDataset, "SmUnits", smUnitClumpIDs)
    rat.writeColumn(ratDataset, "LrgUnits", lrgUnitClumpIDs)
    rat.writeColumn(ratDataset, "smallUnitsBin", smallUnits)

    smClumpsImg = os.path.join(tmp_dir, baseName + "_smclumps.kea")
    rastergis.export_col_to_gdal_img(
        clumps_img, smClumpsImg, "KEA", rsgislib.TYPE_32UINT, "SmUnits"
    )
    rastergis.pop_rat_img_stats(
        clumps_img=smClumpsImg, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )

    lrgClumpsImg = os.path.join(tmp_dir, baseName + "_lrgclumps.kea")
    rastergis.export_col_to_gdal_img(
        clumps_img, lrgClumpsImg, "KEA", rsgislib.TYPE_32UINT, "LrgUnits"
    )
    rastergis.pop_rat_img_stats(
        clumps_img=lrgClumpsImg, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )

    smPolysShp = os.path.join(tmp_dir, baseName + "_smClumps_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        smClumpsImg, smPolysShp, imgBandNo=1, maskImg=smClumpsImg, imgMaskBandNo=1
    )

    lgrPolysShp = os.path.join(tmp_dir, baseName + "_lgrClumps_shp.shp")
    rsgislib.vectorutils.polygoniseRaster(
        lrgClumpsImg, lgrPolysShp, imgBandNo=1, maskImg=lrgClumpsImg, imgMaskBandNo=1
    )

    print(
        "Calculating Distance - can take some time. Try using index and "
        "decreasing max distance threshold."
    )
    t = rsgislib.RSGISTime()
    t.start(True)
    smPolysDistShp = os.path.join(tmp_dir, baseName + "_smClumps_dist_shp.shp")
    rsgislib.vectorutils.dist2NearestSecGeomSet(
        smPolysShp,
        lgrPolysShp,
        smPolysDistShp,
        out_col_name,
        True,
        use_idx,
        max_dist_thres,
    )
    t.end()
    rsgislib.rastergis.import_vec_atts(smClumpsImg, smPolysDistShp, [out_col_name])

    smClumpsRATDataset = gdal.Open(smClumpsImg, gdal.GA_Update)
    minDistCol = rat.readColumn(smClumpsRATDataset, out_col_name)

    minDistSmlClumpsArr = numpy.zeros_like(Histogram, dtype=numpy.float32)
    minDistSmlClumpsArr[smUnitIDs] = minDistCol

    rat.writeColumn(ratDataset, out_col_name, minDistSmlClumpsArr)

    smClumpsRATDataset = None
    ratDataset = None

    if not tmpPresent:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def take_random_sample(
    clumps_img, in_col_name, in_col_val, out_col_name, sample_ratio, rnd_seed=0
):
    """
    A function to take a random sample of an input column.

    :param clumps_img: clumps image.
    :param in_col_name: input column name.
    :param in_col_val: numeric value for which the random sample is to be taken for.
    :param out_col_name: output column where value of 1 is selected within the random
                       sample and 0 is not selected.
    :param sample_ratio: the size of the sample (0 - 1.0; i.e., 10% = 0.1) to be
                        taken of the number of rows within input value.
    :param rnd_seed: is the seed for the random number generation (optional;
                     default is 0).
    """
    if (sample_ratio <= 0) or (sample_ratio >= 1):
        raise rsgislib.RSGISPyException("Input sample ratio must be between 0 and 1.")

    # Define the seed for the random number generation.
    numpy.random.seed(rnd_seed)

    # READ COL FROM RAT:
    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    inColVals = rat.readColumn(ratDataset, in_col_name)

    # Create an array for the original array indexes.
    ID = numpy.arange(inColVals.shape[0])

    # Cut array and array indexes to be only bins with a value of 1
    # (i.e., there is lidar data here).
    ID = ID[inColVals == in_col_val]

    # Sample of the subsetted input array where it equals inColVal.
    numOfSamples = int(ID.shape[0] * sample_ratio)
    IDSamples = numpy.arange(ID.shape[0])
    IDSampPerms = numpy.random.permutation(IDSamples)
    IDSampPermsSelect = IDSampPerms[0:numOfSamples]

    # Find the array indexes for the whole input array (i.e., the whole RAT).
    outArryIdxsSel = ID[IDSampPermsSelect]

    # Create output columns for writing to RAT
    outColVals = numpy.zeros_like(inColVals)

    # Populate columns where those selected have value 1.
    outColVals[outArryIdxsSel] = 1

    # WRITE COL TO RAT:
    rat.writeColumn(ratDataset, out_col_name, outColVals)
    ratDataset = None


def get_column_data(clumps_img, col_name):
    """
    A function to read a column of data from a RAT.

    :param clumps_img: Input clumps image
    :param col_name: Name of the column to be read.

    :return: numpy array with values from the clumpsImg
    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    col_names = rsgislib.rastergis.get_rat_columns(clumps_img)

    if col_name not in col_names:
        raise Exception("Column specified is not within the RAT.")

    ratDataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if ratDataset is None:
        raise Exception("The input image could not be opened.")

    col_data = rat.readColumn(ratDataset, col_name)
    ratDataset = None
    return col_data


def set_column_data(clumps_img, col_name, col_data):
    """
    A function to read a column of data from a RAT.

    :param clumps_img: Input clumps image
    :param col_name: Name of the column to be written.
    :param col_data: Data to be written to the column.
    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception(
            "The numpy module is required for this function "
            "could not be imported\n\t" + numErr
        )
    # Check gdal is available
    if not haveGDALPy:
        raise Exception(
            "The GDAL python bindings are required for this function "
            "could not be imported\n\t" + gdalErr
        )
    # Check rios rat is available
    if not haveRIOSRat:
        raise Exception(
            "The RIOS rat tools are required for this function "
            "could not be imported\n\t" + riosRatErr
        )

    rat_length = rsgislib.rastergis.get_rat_length(clumps_img)
    if rat_length != (col_data.shape[0]):
        raise Exception("The input data array is not the same length as the RAT.")

    ratDataset = gdal.Open(clumps_img, gdal.GA_Update)
    if ratDataset is None:
        raise Exception("The input image could not be opened.")

    rat.writeColumn(ratDataset, col_name, col_data)
    ratDataset = None


def create_uid_col(clumps_img, col_name="UID"):
    """
    A function which adds a unique ID value (starting at 0) to each clump within a RAT.

    :param clumps_img: Input clumps image
    :param col_name: The output column name (default is UID).

    """
    n_rows = rsgislib.rastergis.get_rat_length(clumps_img)
    uid_col = numpy.arange(0, n_rows, 1, dtype=numpy.uint32)
    set_column_data(clumps_img, col_name, uid_col)
