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
import rsgislib
import math
from rsgislib import rastergis

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False

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


def populateImageStats(inputImage, clumpsFile, outascii=None, threshold=0.0, calcMin=False, calcMax=False, calcSum=False, calcMean=False, calcStDev=False, calcMedian=False, calcCount=False, calcArea=False, calcLength=False, calcWidth=False, calcLengthWidth=False):
    
    """ Attribute RAT with statistics from from all bands in an input image.

Where:

* inputImage - input image to calculate statistics from, if band names are avaialble these will be used for attribute names in the output RAT.
* clumpsFile - input clumps file, statistics are added to RAT.
* threshold - float, values below this are ignored (default=0)
* outascii - string providing output CSV file (optional).
* calcMin - Calculate minimum
* calcMax - Calculate maximum
* calcSum - Calculate sum
* calcMean - Calculate mean
* calcStDev - Calculate standard deviation

Example::

    from rsgislib.rastergis import ratutils
    inputImage = 'jers1palsar_stack.kea'
    clumpsFile = 'jers1palsar_stack_clumps_elim_final.kea'
    ratutils.populateImageStats(inputImage, clumpsFile, calcMean=True)

    """
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    
    # Open image
    dataset = gdal.Open(inputImage, gdal.GA_ReadOnly)
    
    # Set up list to hold statistics to calculate
    stats2Calc = list()
    
    # Loop through number of bands in image
    nBands = dataset.RasterCount

    # Set up array to hold all column names (used when exporting to ASCII)
    outFieldsList = []
    
    for i in range(nBands):
        bandName = dataset.GetRasterBand(i+1).GetDescription()
        # If band name is not set set to bandN
        if bandName == '':
            bandName = 'Band' + str(i+1)

        # Initialise stats to calculate at None
        minName = None
        maxName = None
        sumName = None
        meanName = None
        stDevName = None
        medianName = None
        countName = None

        if calcMin:
            minName = bandName + 'Min'
            outFieldsList.append(minName)
        if calcMax:
            maxName = bandName + 'Max'
            outFieldsList.append(maxName)
        if calcSum:
            sumName = bandName + 'Sum'
            outFieldsList.append(sumName)
        if calcMean:
            meanName = bandName + 'Avg'
            outFieldsList.append(meanName)
        if calcStDev:
            stDevName = bandName + 'Std'
            outFieldsList.append(stDevName)
        if calcMedian:
            raise Exception('Median is not currently supported.')
            medianName = bandName + 'Med'
            outFieldsList.append(medianName)
        if calcCount:
            raise Exception('Count is not currently supported.')
            countName = bandName + 'Pix'
            outFieldsList.append(countName)

        stats2Calc.append(rastergis.BandAttStats(band=i+1, 
                    minField=minName, maxField=maxName, 
                    sumField=sumName, stdDevField=stDevName, 
                    meanField=meanName))
    
    # Calc stats
    print('''Calculating statistics for %i Bands'''%(nBands))
    t = rsgislib.RSGISTime()
    t.start(True)
    rastergis.populateRATWithStats(inputImage, clumpsFile, stats2Calc)
    t.end()

    # Calculate shapes, if required
    if calcArea or calcLength or calcWidth or calcLengthWidth:
        raise Exception('Shape features are not currently supported.')
        print("\nCalculating shape indices")
        shapes = list()
        if calcArea:
            shapes.append(rastergis.ShapeIndex(colName="Area", idx=rsgislib.SHAPE_SHAPEAREA))
            outFieldsList.append("Area")
        if calcLength:
            shapes.append(rastergis.ShapeIndex(colName="Length", idx=rsgislib.SHAPE_LENGTH))
            outFieldsList.append("Length")
        if calcWidth:
            shapes.append(rastergis.ShapeIndex(colName="Width", idx=rsgislib.SHAPE_WIDTH))
            outFieldsList.append("Width")
        if calcLengthWidth:
            shapes.append(rastergis.ShapeIndex(colName="LengthWidthRatio", idx=rsgislib.SHAPE_LENGTHWIDTH))
            outFieldsList.append("LengthWidthRatio")

        t.start(True)
        rastergis.calcShapeIndices(clumpsFile, shapes)
        t.end()
    
    # Export to ASCII if required
    if outascii is not None:
        print("\nExporting as ASCII")
        t.start(True)
        rastergis.export2Ascii(clumpsFile, outascii, outFieldsList)
        t.end()




def calcPlotGaussianHistoModel(clumpsFile, outGausH5File, outHistH5File, outPlotFile, varCol, binWidth, classColumn, classVal, plotTitle):
    """ Extracts a column from the RAT, masking by a class calculating the histogram and 
        fitting a Gaussian mixture model to the histogram. Outputs include a plot and HDF5
        files of the histogram and gaussian parameters.

Where:

* clumpsFile - input clumps file with populated RAT.
* outGausH5File - the output HDF5 file for the Gaussian Mixture Model
* outHistH5File - the output HDF5 file for the histogram.
* outPlotFile - the output PDF file for the plot
* varCol - Column within the RAT for the variable to be used for the histogram
* binWidth - Bin width for the histogram
* classColumn - Column where the classes are specified
* classVal - Class used to mask the input variable
* plotTitle - title for the plot

Example::

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
    
    ratutils.calcPlotGaussianHistoModel(clumpsFile, outGausH5File, outHistH5File, outPlotFile, varCol, binWidth, classColumn, classVal, plotTitle)

    """
    # Check numpy is available
    if not haveNumpy:
        raise Exception("The numpy module is required for this function could not be imported\n\t" + numErr)
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
    # Check matplotlib is available
    if not haveMatPlotLib:
        raise Exception("The matplotlib module is required for this function could not be imported\n\t" + pltErr)       
    # Check hdf5 is available
    if not haveHDF5:
        raise Exception("The hdf5 module is required for this function could not be imported\n\t" + h5Err)
        
    # Calculate histogram and fit Gaussian Mixture Model
    rastergis.fitHistGausianMixtureModel(clumps=clumpsFile, outH5File=outGausH5File, outHistFile=outHistH5File, varCol=varCol, binWidth=binWidth, classColumn=classColumn, classVal=classVal)
    
    
    if not h5py.is_hdf5(outGausH5File):
        raise Exception(outGausH5File + " is not a HDF5 file.")
        
    if not h5py.is_hdf5(outHistH5File):
        raise Exception(outHistH5File + " is not a HDF5 file.")

    gausFile = h5py.File(outGausH5File,'r')
    gausParams = gausFile['/DATA/DATA']
    
    
    histFile = h5py.File(outHistH5File,'r')
    histData = histFile['/DATA/DATA']
    
    xVals = []
    xValsHist = []
    histBins = []
    
    for histBin in histData:
        xValsHist.append(histBin[0]-(binWidth/2))
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
    histBars = ax.bar(xValsHist, histBins, width=binWidth, color='#A7A7A7', edgecolor='#A7A7A7')
    
    predVals = numpy.zeros(len(xVals))
    for i in range(len(xVals)):
        gNoiseVals.append(noiseVal)
        for j in range(len(gOffVals)):
            predVals[i] = predVals[i] + (gAmpVals[j] * math.exp((-1.0)*(pow(xVals[i] - gOffVals[j], 2)/(2.0 * pow(gFWHMVals[j], 2)))))
        predVals[i] = predVals[i] + noiseVal
    
    ax.plot(xVals, predVals, color='red')
    ax.plot(xVals, gNoiseVals, color='blue', linestyle='dashed')
    
    ax.set_ylabel('Freq.')
    ax.set_title(plotTitle)
    plt.savefig(outPlotFile, format='PDF')
    
    gausFile.close()
    histFile.close()
    
    
    
    
    
    
