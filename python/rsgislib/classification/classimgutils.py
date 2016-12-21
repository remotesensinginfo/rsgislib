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

import os
import os.path
import shutil

from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier

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



class ClassInfoObj(object):
    """
    This is a class to store the information associated within the classification.
    
    * id - Output pixel value for this class
    * fileH5 - hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the training data for the class
    * red - Red colour for visualisation (0-255)
    * green - Green colour for visualisation (0-255)
    * blue - Blue colour for visualisation (0-255)
    """
    def __init__(self, id=None, fileH5=None, red=None, green=None, blue=None):
        """
        * id - Output pixel value for this class
        * fileH5 - hdf5 file (from rsgislib.imageutils.extractZoneImageBandValues2HDF) with the training data for the class
        * red - Red colour for visualisation (0-255)
        * green - Green colour for visualisation (0-255)
        * blue - Blue colour for visualisation (0-255)
        """
        self.id = id
        self.fileH5 = fileH5
        self.red = red
        self.green = green
        self.blue = blue

def findClassifierParametersAndTrain(classTrainInfo, paramSearchSampNum=0, gridSearch=GridSearchCV(RandomForestClassifier(), {})):
    """
A function to find the optimal parameters for classification using a Grid Search (http://scikit-learn.org/stable/modules/grid_search.html). 
The returned classifier instance will be trained using the input data.

* classTrainInfo - list of ClassInfoObj objects which will be used to train the classifier.
* paramSearchSampNum - the number of samples that will be randomly sampled from the training data for each class for applying the grid search (tend to use a small data sample as can take a long time). A value of 500 would use 500 samples per class.
* gridSearch - is an instance of the sklearn.model_selection.GridSearchCV with an instance of the choosen classifier and parameters to be searched.
    """
    # Check h5py is available
    if not haveH5PY:
        raise Exception("The h5py module is required for this function could not be imported\n\t" + h5pyErr)
    
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
This function trains the classifier. 

* classTrainInfo - list of ClassInfoObj objects which will be used to train the classifier.
* skClassifier - an instance of a parameterised scikit-learn classifier (http://scikit-learn.org/stable/supervised_learning.html)
    """
     # Check h5py is available
    if not haveH5PY:
        raise Exception("The h5py module is required for this function could not be imported\n\t" + h5pyErr)
    
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
    
    
def applyClassifer(classTrainInfo, skClassifier, imgMask, imgMaskVal, imgFileInfo, outputImg, gdalFormat, classClrNames=True):
    """
This function uses a trained classifier and applies it to the provided input image.

* classTrainInfo - dict (where the key is the class name) of ClassInfoObj objects which will be used to train the classifier (i.e., trainClassifier()), provide pixel value id and RGB class values.
* skClassifier - a trained instance of a scikit-learn classifier (e.g., use trainClassifier or findClassifierParametersAndTrain)
* imgMask - is an image file providing a mask to specify where should be classified. Simplest mask is all the valid data regions (rsgislib.imageutils.genValidMask)
* imgMaskVal - the pixel value within the imgMask to limit the region to which the classification is applied. Can be used to create a heirachical classification.
* imgFileInfo - a list of rsgislib.imageutils.ImageBandInfo objects (also used within rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to be used for the classification so it adheres to the training data. 
* outputImg - output image file with the classification. Note. by default a colour table and class names column is added to the image. If an error is produced use HFA or KEA formats.
* gdalFormat - is the output image format - all GDAL supported formats are supported. 
* classClrNames - default is True and therefore a colour table will the colours specified in classTrainInfo and a ClassName column (from imgFileInfo) will be added to the output file.

    """
    if not haveRIOS:
        raise Exception("The rios module is required for this function could not be imported\n\t" + riosErr)
    
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
    
    if classClrNames:
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
    
    
def performPerPxlMLClassShpTrain(imageBandInfo=[], classInfo=dict(), outputImg='classImg.kea', gdalFormat='KEA', tmpPath='./tmp', skClassifier=RandomForestClassifier(), gridSearch=None, paramSearchSampNum=100):
    """
A function which performs a per-pixel based classification of a scene using a machine learning classifier from the scikit-learn
library where a single polygon shapefile per class is required to represent the training data. 

* imageBandInfo is a list of rsgislib.imageutils.ImageBandInfo objects specifying the images which should be used.
* classInfo is a dict of rsgislib.classification.classimgutils.ClassInfoObj objects where the key is the class name. The fileH5 field is used to define the file path to the shapefile with the training data.
* outputImg is the name and path to the output image file.
* gdalFormat is the output image file format (e.g., KEA). 
* tmpPath is a tempory file path which can be used during processing.
* skClassifier is an instance of a scikit-learn classifier appropriately parameterised. If None then the gridSearch object must not be None.
* gridSearch is an instance of a scikit-learn sklearn.model_selection.GridSearchCV object with the classifier and parameter search space specified. (If None then skClassifier will be used; if both not None then skClassifier will be used in preference to gridSearch)

Example::
    
    from rsgislib.classification import classimgutils
    from rsgislib import imageutils

    from sklearn.ensemble import ExtraTreesClassifier
    from sklearn.model_selection import GridSearchCV
    
    imageBandInfo=[imageutils.ImageBandInfo('./LS2MSS_19750620_lat10lon6493_r67p250_rad_srefdem_30m.kea', 'Landsat', [1,2,3,4])]
    classInfo=dict()
    classInfo['Forest'] = classimgutils.ClassInfoObj(id=1, fileH5='./ForestRegions.shp', red=0, green=255, blue=0)
    classInfo['Non-Forest'] = classimgutils.ClassInfoObj(id=2, fileH5='./NonForestRegions.shp', red=100, green=100, blue=100)
    
    
    skClassifier=ExtraTreesClassifier(n_estimators=20)
    classimgutils.performPerPxlMLClassShpTrain(imageBandInfo, classInfo, outputImg='classImg.kea', gdalFormat='KEA', tmpPath='./tmp', skClassifier=skClassifier)
    
"""
    if not haveH5PY:
        raise Exception("The h5py module is required for this function could not be imported\n\t" + h5pyErr)
    if not haveRIOS:
        raise Exception("The rios module is required for this function could not be imported\n\t" + riosErr)
    
    if (skClassifier is None) and (gridSearch is None):
        raise rsgislib.RSGISPyException("Both skClassifier and gridSearch cannot be None. You must provide an instance of one of them.")
    
    # Define base name
    rsgisUtils = rsgislib.RSGISPyUtils()
    baseName = "PerPxlClass_" + rsgisUtils.uidGenerator()
    
    # Create tmp path.
    tmpPresent = True
    if not os.path.exists(tmpPath):
        os.makedirs(tmpPath)
        tmpPresent = False

    # Create unique tempory directory
    baseNameTmpDir = os.path.join(tmpPath, baseName)
    os.makedirs(baseNameTmpDir)
    
    # Create valid data mask.
    validMasks = []
    for imgInfo in imageBandInfo:
        tmpBaseName = os.path.splitext(os.path.basename(imgInfo.fileName))[0]
        vdmskFile = os.path.join(baseNameTmpDir, tmpBaseName+'_vmsk.kea')
        noDataVal = rsgisUtils.getImageNoDataValue(imgInfo.fileName)
        rsgislib.imageutils.genValidMask(inimages=imgInfo.fileName, outimage=vdmskFile, format='KEA', nodata=noDataVal)
        validMasks.append(vdmskFile)
    
    vdmskFile = os.path.join(baseNameTmpDir, baseName+'_vmsk.kea')
    if len(validMasks) > 1:
        rsgislib.imageutils.genValidMask(inimages=validMasks, outimage=vdmskFile, format='KEA', nodata=0.0)
    else:
        vdmskFile = validMasks[0]
    
    # Rasterise shapefiles to be used as training.
    rasterTrain = dict()
    for cName in classInfo:
        shpFile = classInfo[cName].fileH5
        tmpBaseName = os.path.splitext(os.path.basename(shpFile))[0]
        tmpFile = os.path.join(baseNameTmpDir, tmpBaseName+'_rasterzone.kea')
        rsgislib.vectorutils.rasterise2Image(shpFile, vdmskFile, tmpFile, gdalFormat='KEA', burnVal=classInfo[cName].id, shpExt=False)
        rasterTrain[cName] = tmpFile
        tmpFileH5 = os.path.join(baseNameTmpDir, tmpBaseName+'_pxlVals.h5')
        rsgislib.imageutils.extractZoneImageBandValues2HDF(imageBandInfo, rasterTrain[cName], tmpFileH5, classInfo[cName].id)
        classInfo[cName].fileH5 = tmpFileH5
    
    if skClassifier is not None:
        trainClassifier(classInfo, skClassifier)
    else:
        skClassifier = findClassifierParametersAndTrain(classInfo, paramSearchSampNum=0, gridSearch=gridSearch)
        
    applyClassifer(classInfo, skClassifier, vdmskFile, 1, imageBandInfo, outputImg, gdalFormat, classClrNames=True)
    
    # Clean up tempory files.
    shutil.rmtree(baseNameTmpDir, ignore_errors=True)
    if not tmpPresent:
        shutil.rmtree(tmpPath, ignore_errors=True)

