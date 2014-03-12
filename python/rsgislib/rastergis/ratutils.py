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
from rsgislib import rastergis

try:
    import osgeo.gdal as gdal
except ImportError:
    print("ERROR: Couldn't import GDAL python bindings")
    sys.exit()

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
* calcMedian - Calculate median
* calcCount - Calculate count
* calcArea - Calculate clump area 
* calcLength - Calculate clump lenght
* calcWidth - Calculate clump width

Example::

    from rsgislib.rastergis import ratutils
    inputImage = 'jers1palsar_stack.kea'
    clumpsFile = 'jers1palsar_stack_clumps_elim_final.kea'
    ratutils.populateImageStats(inputImage, clumpsFile, calcMean=True)

    """

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
            medianName = bandName + 'Med'
            outFieldsList.append(medianName)
        if calcCount:
            countName = bandName + 'Pix'
            outFieldsList.append(countName)

        stats2Calc.append(rastergis.BandAttStats(band=i+1, 
                    countField=countName, minField=minName, 
                    maxField=maxName, sumField=sumName, 
                    medianField=medianName, stdDevField=stDevName, 
                    meanField=meanName))
    
    # Calc stats
    print('''Calculating statistics for %i Bands'''%(nBands))
    t = rsgislib.RSGISTime()
    t.start(True)
    rastergis.populateRATWithStats(inputImage, clumpsFile, stats2Calc)
    t.end()

    # Calculate shapes, if required
    if calcArea or calcLength or calcWidth or calcLengthWidth:
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


