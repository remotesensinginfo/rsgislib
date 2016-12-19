#! /usr/bin/env python

############################################################################
#  classimgutils.py
#
#  Copyright 2016 RSGISLib.
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
# Date: 17/12/2016
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

from __future__ import print_function

import numpy
import numpy.random

from osgeo import gdal

import rsgislib.rastergis
import rsgislib

haveH5PY = True
try:
    import h5py
except ImportError as h5pyErr:
    haveH5PY = False

haveRIOS = True
try:
    from rios import applier
    from rios import cuiprogress
    from rios import rat
except ImportError as riosErr:
    haveRIOS = False
    raise Exception("The RIOS tools are required for this module could not be imported\n\t {}".format(riosErr))

haveSKLearn = True
try:
    from sklearn.model_selection import GridSearchCV
    from sklearn.ensemble import RandomForestClassifier
except ImportError as sklearnErr:
    haveSKLearn = False
    raise Exception("The scikit-learn tools are required for this module could not be imported\n\t {}".format(sklearnErr))


class ClassInfoObj(object):
    def __init__(self, id=None, fileH5=None, red=None, green=None, blue=None):
        self.id = id
        self.fileH5 = fileH5
        self.red = red
        self.green = green
        self.blue = blue

def findClassifierParametersAndTrain(classTrainInfo, paramSearchSampNum=0, gridSearch=GridSearchCV(RandomForestClassifier(), {})):
    """
    """
    # Check h5py is available
    if not haveH5PY:
        raise Exception("The h5pt module is required for this function could not be imported\n\t" + h5pyErr)
    
    if len(classTrainInfo) < 2:
        rsgislib.RSGISPyException("Need at least 2 classes to be worth running findClassifierParametersAndTrain function.")
    
    first = True
    numVars = 0
    numVals = 0
    for classInfoVal in classTrainInfo.values():
        dataShp = h5py.File(classInfoVal.fileH5)['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]
    
    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    classArr = numpy.zeros([numVals], dtype=int)
    
    if paramSearchSampNum is not 0:
        dataArrSamp = numpy.zeros([(len(classTrainInfo)*paramSearchSampNum), numVars], dtype=float)
        classArrSamp = numpy.zeros([(len(classTrainInfo)*paramSearchSampNum)], dtype=int)
    
    rowInit = 0
    rowInitSamp = 0
    for key in classTrainInfo:
        # Open the dataset
        f = h5py.File(classTrainInfo[key].fileH5)
        numRows = f['DATA/DATA'].shape[0]
        # Copy data and populate classid array
        dataArr[rowInit:(rowInit+numRows)] = f['DATA/DATA']
        classArr[rowInit:(rowInit+numRows)] = classTrainInfo[key].id
        if paramSearchSampNum is not 0:
            # Create random index to sample the whole dataset.
            sampleIdxs = numpy.random.randint(0, high=numRows, size=paramSearchSampNum)
            # Sample the input data for classifier optimisation.
            dataArrSamp[rowInitSamp:(rowInitSamp+paramSearchSampNum)] = dataArr[rowInit:(rowInit+numRows)][sampleIdxs]
            classArrSamp[rowInitSamp:(rowInitSamp+paramSearchSampNum)] = classTrainInfo[key].id
            rowInitSamp += paramSearchSampNum
        rowInit += numRows
        f.close()
    
    if paramSearchSampNum is 0:
        dataArrSamp = dataArr
        classArrSamp = classArr
    
    print("Training data size: {} x {}".format(dataArr.shape[0], dataArr.shape[1]))
    print("Training Sample data size: {} x {}".format(dataArrSamp.shape[0], dataArrSamp.shape[1]))
    
    gridSearch.fit(dataArrSamp, classArrSamp)
    if not gridSearch.refit:
        raise Exception("Grid Search did no find a fit therefore failed...")
    
    print("Best score was {} and has parameters {}.".format(gridSearch.best_score_, gridSearch.best_params_))
    
    print('Training Classifier')
    gridSearch.best_estimator_.fit(dataArr, classArr)
    print("Completed")
    
    print('Calc Classifier Accuracy')
    accVal = gridSearch.best_estimator_.score(dataArr, classArr)
    print('Classifier Train Score = {}%'.format(round(accVal*100, 2)))   
    
    return gridSearch.best_estimator_

def trainClassifier(classTrainInfo, skClassifier):
    """
    """
     # Check h5py is available
    if not haveH5PY:
        raise Exception("The h5pt module is required for this function could not be imported\n\t" + h5pyErr)
    
    if len(classTrainInfo) < 2:
        rsgislib.RSGISPyException("Need at least 2 classes to be worth running trainClassifier function.")
    
    first = True
    numVars = 0
    numVals = 0
    for classInfoVal in classTrainInfo.values():
        dataShp = h5py.File(classInfoVal.fileH5)['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]
    
    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    classArr = numpy.zeros([numVals], dtype=int)
    
    rowInit = 0
    for key in classTrainInfo:
        # Open the dataset
        f = h5py.File(classTrainInfo[key].fileH5)
        numRows = f['DATA/DATA'].shape[0]
        # Copy data and populate classid array
        dataArr[rowInit:(rowInit+numRows)] = f['DATA/DATA']
        classArr[rowInit:(rowInit+numRows)] = classTrainInfo[key].id
        rowInit += numRows
        f.close()
        
    print("Training data size: {} x {}".format(dataArr.shape[0], dataArr.shape[1]))
    
    print('Training Classifier')
    skClassifier.fit(dataArr, classArr)
    print("Completed")
    
    print('Calc Classifier Accuracy')
    accVal = skClassifier.score(dataArr, classArr)
    print('Classifier Train Score = {}%'.format(round(accVal*100, 2)))



# RIOS function to apply classifer
def _applySKClassifier(info, inputs, outputs, otherargs):
    """
    Internal function for rios applier. Used within applyClassifer.
    """
    outClassVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint32)
    if numpy.any(inputs.imageMask == otherargs.mskVal):
        outClassVals = outClassVals.flatten()
        imgMaskVals = inputs.imageMask.flatten()
        classVars = numpy.zeros((outClassVals.shape[0], otherargs.numClassVars), dtype=numpy.float)
        # Array index which can be used to populate the output array following masking etc.
        ID = numpy.arange(imgMaskVals.shape[0])
        classVarsIdx = 0
        for imgFile in otherargs.imgFileInfo:
            imgArr = inputs.__dict__[imgFile.name]
            for band in imgFile.bands:
                classVars[...,classVarsIdx] = imgArr[(band-1)].flatten()
                classVarsIdx = classVarsIdx + 1
        classVars = classVars[imgMaskVals==otherargs.mskVal]
        ID = ID[imgMaskVals==otherargs.mskVal]
        predClass = otherargs.classifier.predict(classVars)        
        outClassVals[ID] = predClass
        outClassVals = numpy.expand_dims(outClassVals.reshape((inputs.imageMask.shape[1],inputs.imageMask.shape[2])), axis=0)
    outputs.outimage = outClassVals
    
    
def applyClassifer(classTrainInfo, skClassifier, imgMask, imgMaskVal, imgFileInfo, outputImg, gdalFormat):
    rsgisUtils = rsgislib.RSGISPyUtils()
    infiles = applier.FilenameAssociations()
    infiles.imageMask = imgMask
    numClassVars = 0
    for imgFile in imgFileInfo:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)
            
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    otherargs.classifier = skClassifier
    otherargs.mskVal = imgMaskVal
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = imgFileInfo
    
    aControls = applier.ApplierControls()
    aControls.progress = cuiprogress.CUIProgressBar()
    aControls.drivername = gdalFormat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Applying the Classifier")
    applier.apply(_applySKClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")
    rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)
    
    ratDataset = gdal.Open(outputImg, gdal.GA_Update)
    red = rat.readColumn(ratDataset, 'Red')
    green = rat.readColumn(ratDataset, 'Green')
    blue = rat.readColumn(ratDataset, 'Blue')
    ClassName = numpy.empty_like(red, dtype=numpy.dtype('a255'))
    
    for classKey in classTrainInfo:  
        print("Apply Colour to class \'" + classKey + "\'")
        red[classTrainInfo[classKey].id] = classTrainInfo[classKey].red
        green[classTrainInfo[classKey].id] = classTrainInfo[classKey].green
        blue[classTrainInfo[classKey].id] = classTrainInfo[classKey].blue
        ClassName[classTrainInfo[classKey].id] = classKey
    
    rat.writeColumn(ratDataset, "Red", red)
    rat.writeColumn(ratDataset, "Green", green)
    rat.writeColumn(ratDataset, "Blue", blue)
    rat.writeColumn(ratDataset, "ClassName", ClassName)

    ratDataset = None
    
    

