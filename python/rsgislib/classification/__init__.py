#!/usr/bin/env python
"""
The classification module provides classification functionality within RSGISLib.
"""

# import the C++ extension into this level
from ._classification import *


# Import the RSGISLib Image Utils module
from rsgislib import vectorutils

def generateTransectAccuracyPts(inputImage, inputLinesShp, outputPtsShp, classImgCol, classImgVecCol, classRefVecCol, lineStep, force=False):
    """ A tool for converting a set of lines in to point transects and 
populating with the information for undertaking an accuracy assessment.

Where:

:param inputImage: is a string specifying the input image file with classification.
:param inputLinesShp: is a string specifying the input lines shapefile path.
:param outputPtsShp: is a string specifying the output points shapefile path.
:param classImgCol: is a string speciyfing the name of the column in the image file containing the class names.
:param classImgVecCol: is a string specifiying the output column in the shapefile for the classified class names.
:param classRefVecCol: is an optional string specifiying an output column in the shapefile which can be used in the accuracy assessment for the reference data.
:param lineStep: is a double specifying the step along the lines between the points
:param force: is an optional boolean specifying whether the output shapefile should be deleted if is already exists (True and it will be deleted; Default is False)

    """
    vectorutils.createLinesOfPoints(inputLinesShp, outputPtsShp, lineStep, force)
    
    popClassInfoAccuracyPts(inputImage, outputPtsShp, classImgCol, classImgVecCol, classRefVecCol)
    
    
    