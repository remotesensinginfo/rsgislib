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
import random

import os
import os.path
import shutil
import random

from multiprocessing import Pool

from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier
from sklearn.cluster import MiniBatchKMeans

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



class SamplesInfoObj(object):
    """
    This is a class to store the information associated within the classification.
    
    * className - The name of the class
    * classID - Is the classification numeric ID (i.e., output pixel value)
    * maskImg - The input image mask from which samples are taken
    * maskPxlVal - The pixel value within the mask for the class
    * outSampImgFile - Temporary file which will store the sampled pixels.
    * numSamps - The number of samples required.
    * samplesH5File - File location for the HDF5 file with the input image values for training.
    * red - for visualisation red value.
    * green - for visualisation green value.
    * blue - for visualisation blue value.

    """
    def __init__(self, className=None, classID=None, maskImg=None, maskPxlVal=None, outSampImgFile=None, numSamps=None, samplesH5File=None, red=None, green=None, blue=None):
        """

        * className - The name of the class
        * classID - Is the classification numeric ID (i.e., output pixel value)
        * maskImg - The input image mask from which samples are taken
        * maskPxlVal - The pixel value within the mask for the class
        * outSampImgFile - Temporary file which will store the sampled pixels.
        * numSamps - The number of samples required.
        * samplesH5File - File location for the HDF5 file with the input image values for training.
        * red - for visualisation red value.
        * green - for visualisation green value.
        * blue - for visualisation blue value.

        """
        self.className = className
        self.classID = classID
        self.maskImg = maskImg
        self.maskPxlVal = maskPxlVal
        self.outSampImgFile = outSampImgFile
        self.numSamps = numSamps
        self.samplesH5File = samplesH5File
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
    
    if not preProcessor is None:
        dataArrSamp = reProcessor.fit_transform(dataArrSamp)
    
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
    
    
def applyClassifer(classTrainInfo, skClassifier, imgMask, imgMaskVal, imgFileInfo, outputImg, gdalformat, classClrNames=True):
    """
This function uses a trained classifier and applies it to the provided input image.

* classTrainInfo - dict (where the key is the class name) of ClassInfoObj objects which will be used to train the classifier (i.e., trainClassifier()), provide pixel value id and RGB class values.
* skClassifier - a trained instance of a scikit-learn classifier (e.g., use trainClassifier or findClassifierParametersAndTrain)
* imgMask - is an image file providing a mask to specify where should be classified. Simplest mask is all the valid data regions (rsgislib.imageutils.genValidMask)
* imgMaskVal - the pixel value within the imgMask to limit the region to which the classification is applied. Can be used to create a heirachical classification.
* imgFileInfo - a list of rsgislib.imageutils.ImageBandInfo objects (also used within rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to be used for the classification so it adheres to the training data. 
* outputImg - output image file with the classification. Note. by default a colour table and class names column is added to the image. If an error is produced use HFA or KEA formats.
* gdalformat - is the output image format - all GDAL supported formats are supported. 
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
    aControls.drivername = gdalformat
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
    
    
def performPerPxlMLClassShpTrain(imageBandInfo=[], classInfo=dict(), outputImg='classImg.kea', gdalformat='KEA', tmpPath='./tmp', skClassifier=RandomForestClassifier(), gridSearch=None, paramSearchSampNum=100):
    """
A function which performs a per-pixel based classification of a scene using a machine learning classifier from the scikit-learn
library where a single polygon shapefile per class is required to represent the training data. 

* imageBandInfo is a list of rsgislib.imageutils.ImageBandInfo objects specifying the images which should be used.
* classInfo is a dict of rsgislib.classification.classimgutils.ClassInfoObj objects where the key is the class name. The fileH5 field is used to define the file path to the shapefile with the training data.
* outputImg is the name and path to the output image file.
* gdalformat is the output image file format (e.g., KEA). 
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
    classimgutils.performPerPxlMLClassShpTrain(imageBandInfo, classInfo, outputImg='classImg.kea', gdalformat='KEA', tmpPath='./tmp', skClassifier=skClassifier)
    
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
        rsgislib.imageutils.genValidMask(inimages=imgInfo.fileName, outimage=vdmskFile, gdalformat='KEA', nodata=noDataVal)
        validMasks.append(vdmskFile)
    
    vdmskFile = os.path.join(baseNameTmpDir, baseName+'_vmsk.kea')
    if len(validMasks) > 1:
        rsgislib.imageutils.genValidMask(inimages=validMasks, outimage=vdmskFile, gdalformat='KEA', nodata=0.0)
    else:
        vdmskFile = validMasks[0]
    
    # Rasterise shapefiles to be used as training.
    rasterTrain = dict()
    for cName in classInfo:
        shpFile = classInfo[cName].fileH5
        tmpBaseName = os.path.splitext(os.path.basename(shpFile))[0]
        tmpFile = os.path.join(baseNameTmpDir, tmpBaseName+'_rasterzone.kea')
        rsgislib.vectorutils.rasterise2Image(shpFile, vdmskFile, tmpFile, gdalformat='KEA', burnVal=classInfo[cName].id, shpExt=False)
        rasterTrain[cName] = tmpFile
        tmpFileH5 = os.path.join(baseNameTmpDir, tmpBaseName+'_pxlVals.h5')
        rsgislib.imageutils.extractZoneImageBandValues2HDF(imageBandInfo, rasterTrain[cName], tmpFileH5, classInfo[cName].id)
        classInfo[cName].fileH5 = tmpFileH5
    
    if skClassifier is not None:
        trainClassifier(classInfo, skClassifier)
    else:
        skClassifier = findClassifierParametersAndTrain(classInfo, paramSearchSampNum=0, gridSearch=gridSearch)
        
    applyClassifer(classInfo, skClassifier, vdmskFile, 1, imageBandInfo, outputImg, gdalformat, classClrNames=True)
    
    # Clean up tempory files.
    shutil.rmtree(baseNameTmpDir, ignore_errors=True)
    if not tmpPresent:
        shutil.rmtree(tmpPath, ignore_errors=True)














def _applyClassification(inParams):
    """
    Internal function which is used by performVotingClassification
    """

    skClassifier = inParams['skClassifier']
    cTmpDIR = inParams['cTmpDIR']
    classAreaMask = inParams['classAreaMask']
    classMaskPxlVal = inParams['classMaskPxlVal']
    imgFileInfo = inParams['imgFileInfo']
    tmpClassImgOut = inParams['tmpClassImgOut']
    gdalformat = inParams['gdalformat']
    trainSamplesInfo = inParams['trainSamplesInfo']
    rndSeed = inParams['rndSeed']
    
    classTrainInfo = dict()
    for trainSamples in trainSamplesInfo:
        rsgislib.imageutils.performRandomPxlSampleInMaskLowPxlCount(inputImage=trainSamples.maskImg, outputImage=os.path.join(cTmpDIR, trainSamples.outSampImgFile), gdalformat=gdalformat, maskvals=[trainSamples.maskPxlVal], numSamples=trainSamples.numSamps, rndSeed=rndSeed)
        rsgislib.imageutils.extractZoneImageBandValues2HDF(imgFileInfo, os.path.join(cTmpDIR, trainSamples.outSampImgFile), os.path.join(cTmpDIR, trainSamples.samplesH5File), trainSamples.maskPxlVal)
        classTrainInfo[trainSamples.className] = ClassInfoObj(id=trainSamples.classID, fileH5=os.path.join(cTmpDIR, trainSamples.samplesH5File), red=trainSamples.red, green=trainSamples.green, blue=trainSamples.blue)
    
    trainClassifier(classTrainInfo, skClassifier)
    applyClassifer(classTrainInfo, skClassifier, classAreaMask, classMaskPxlVal, imgFileInfo, tmpClassImgOut, gdalformat)



def performVotingClassification(skClassifiers, trainSamplesInfo, imgFileInfo, classAreaMask, classMaskPxlVal, tmpDIR, tmpImgBase, outClassImg, gdalformat='KEA', numCores=-1):
    """
A function which will perform a number of classification creating a combined classification by a simple vote. 
The classifier parameters can be differed as a list of classifiers is provided (the length of the list is equal 
to the number of votes), where the training data is resampled for each classifier. The analysis can be performed
using multiple processing cores.

Where:

* skClassifiers a list of classifiers (from scikit-learn), the number of classifiers defined will be equal to the number of votes.
* trainSamplesInfo - a list of rsgislib.classification.classimgutils.SamplesInfoObj objects used to parameters the classifer and extract training data.
* imgFileInfo - a list of rsgislib.imageutils.ImageBandInfo objects (also used within rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to be used for the classification so it adheres to the training data. 
* classAreaMask - a mask image which is used to specified the areas of the scene which are to be classified.
* classMaskPxlVal - is the pixel value within the classAreaMask image for the areas of the image which are to be classified.
* tmpDIR - a temporary file location which will be created and removed during processing.
* tmpImgBase - the same name of files written to the tmpDIR
* outClassImg - the final output image file.
* gdalformat - the output file format for outClassImg
* numCores - is the number of processing cores to be used for the analysis (if -1 then all cores on the machine will be used).

Example::
    
    classVoteTemp = os.path.join(imgTmp, 'ClassVoteTemp')
    
    imgFileInfo = [rsgislib.imageutils.ImageBandInfo(img2010dB, 'sardb', [1,2]), rsgislib.imageutils.ImageBandInfo(imgSRTM, 'srtm', [1])]
    trainSamplesInfo = []
    trainSamplesInfo.append(PerformVotingClassifier.SamplesInfoObj(className='Water', classID=1, maskImg=classTrainRegionsMask, maskPxlVal=1, outSampImgFile='WaterSamples.kea', numSamps=500, samplesH5File='WaterSamples_pxlvals.h5', red=0, green=0, blue=255))
    trainSamplesInfo.append(PerformVotingClassifier.SamplesInfoObj(className='Land', classID=2, maskImg=classTrainRegionsMask, maskPxlVal=2, outSampImgFile='LandSamples.kea', numSamps=500, samplesH5File='LandSamples_pxlvals.h5', red=150, green=150, blue=150))
    trainSamplesInfo.append(PerformVotingClassifier.SamplesInfoObj(className='Mangroves', classID=3, maskImg=classTrainRegionsMask, maskPxlVal=3, outSampImgFile='MangroveSamples.kea', numSamps=500, samplesH5File='MangroveSamples_pxlvals.h5', red=0, green=153, blue=0))
    
    skClassifiers = []
    for i in range(5):
        skClassifiers.append(ExtraTreesClassifier(n_estimators=50))
        
    for i in range(5):
        skClassifiers.append(ExtraTreesClassifier(n_estimators=100))
        
    for i in range(5):
        skClassifiers.append(ExtraTreesClassifier(n_estimators=50), max_depth=2)
        
    for i in range(5):
        skClassifiers.append(ExtraTreesClassifier(n_estimators=100), max_depth=2)
    
    mangroveRegionClassImg = MangroveRegionClass.kea
    classimgutils.performVotingClassification(skClassifiers, trainSamplesInfo, imgFileInfo, classWithinMask, 1, classVoteTemp, 'ClassImgSample', mangroveRegionClassImg, gdalformat='KEA', numCores=-1)
    

    """
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    if type(skClassifiers) is not list:
        raise rsgislib.RSGISPyException("A list of classifiers must be provided")
        
    numOfVotes = len(skClassifiers)
    
    if numCores <= 0:
        numCores = rsgisUtils.numProcessCores()
    
    tmpPresent = True
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        tmpPresent = False
    
    outClassImgs = []
    mCoreParams = []
    dirs2DEL = []
    rndGen = random.seed()
    for i in range(numOfVotes):
        cTmpDIR = os.path.join(tmpDIR, str(i))
        if os.path.exists(cTmpDIR):
            shutil.rmtree(cTmpDIR, ignore_errors=True)
        os.makedirs(cTmpDIR)
        dirs2DEL.append(cTmpDIR)

        tmpClassImgOut = os.path.join(tmpDIR, tmpImgBase+'_'+str(i)+'.kea')
        outClassImgs.append(tmpClassImgOut)
        inParams = dict()
        inParams['skClassifier'] = skClassifiers[i]
        inParams['cTmpDIR'] = cTmpDIR
        inParams['classAreaMask'] = classAreaMask
        inParams['classMaskPxlVal'] = classMaskPxlVal
        inParams['imgFileInfo'] = imgFileInfo
        inParams['tmpClassImgOut'] = tmpClassImgOut
        inParams['gdalformat'] = 'KEA'
        inParams['trainSamplesInfo'] = trainSamplesInfo
        inParams['rndSeed'] = random.randrange(1000)
        mCoreParams.append(inParams)
    
    # Run processing on multiple cores.    
    mProccesPool = Pool(numCores)
    mProccesPool.map(_applyClassification, mCoreParams)
    
    # Combine results using MODE. 
    rsgislib.imagecalc.calcMultiImgBandStats(outClassImgs, outClassImg, rsgislib.SUMTYPE_MODE, gdalformat, rsgislib.TYPE_8UINT, 0, True)
    rsgislib.rastergis.populateStats(clumps=outClassImg, addclrtab=True, calcpyramids=True, ignorezero=True)
    
    # Colour output classification image.
    ratDataset = gdal.Open(outClassImg, gdal.GA_Update)
    red = rat.readColumn(ratDataset, 'Red')
    green = rat.readColumn(ratDataset, 'Green')
    blue = rat.readColumn(ratDataset, 'Blue')
    ClassName = numpy.empty_like(red, dtype=numpy.dtype('a255'))
    
    for trainSample in trainSamplesInfo:  
        print("Apply Colour to class \'" + trainSample.className + "\'")
        red[trainSample.classID] = trainSample.red
        green[trainSample.classID] = trainSample.green
        blue[trainSample.classID] = trainSample.blue
        ClassName[trainSample.classID] = trainSample.className
    
    rat.writeColumn(ratDataset, "Red", red)
    rat.writeColumn(ratDataset, "Green", green)
    rat.writeColumn(ratDataset, "Blue", blue)
    rat.writeColumn(ratDataset, "ClassName", ClassName)

    ratDataset = None
    
    if not tmpPresent:
        shutil.rmtree(tmpDIR, ignore_errors=True)
    else:
        for cDIR in dirs2DEL:
            shutil.rmtree(cDIR, ignore_errors=True)


def performPxlClustering(inputImg, outputImg, gdalformat='KEA', noDataVal=0, imgSamp=100, clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100), calcStats=True, useMeanShiftEstBandWidth=False):
    """
A function which allows a clustering to be performed using the algorithms available
within the scikit-learn library. The clusterer is trained on a sample of the input
image and then applied using the predict function (therefore this function is only
compatiable with clusterers which have the predict function implemented) to the whole
image.

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* noDataVal - no data value associated with the input image.
* imgSamp - the input image sampling. (e.g., 100 is every 100th pixel)
* clusterer - clusterer from scikit-learn which must have a predict function.
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* useMeanShiftEstBandWidth - use the mean-shift algorithm as the clusterer (pass None as the clusterer) where the bandwidth is calculated from the data itself.

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
    print('Started .0.', end='', flush=True)
    outCount = 10
    for (info, block) in reader:
        if info.getPercent() > outCount:
            print('.'+str(int(outCount))+'.', end='', flush=True)
            outCount = outCount + 10
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1]*blkShape[2]))).T
        ID = numpy.arange(blkBands.shape[0])
        outClusterVals = numpy.zeros((blkBands.shape[0]))
        
        finiteMskArr = numpy.isfinite(blkBands).all(axis=1)
        ID = ID[finiteMskArr]
        blkBands = blkBands[finiteMskArr]
        
        noDataValArr = numpy.logical_not(numpy.where(blkBands == noDataVal, True, False).all(axis=1))
        
        blkBandsNoData = blkBands[noDataValArr]
        ID = ID[noDataValArr]
        
        if ID.shape[0] > 0:
            outPred = clusterer.predict(blkBandsNoData)+1
            outClusterVals[ID] = outPred
        
        outClusterValsOutArr = outClusterVals.reshape([1,blkShape[1],blkShape[2]])
        
        if writer is None:
            writer = ImageWriter(outputImg, info=info, firstblock=outClusterValsOutArr, drivername=gdalformat, creationoptions=[])
        else:
            writer.write(outClusterValsOutArr)
    writer.close(calcStats=False)
    print('. Completed')
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def performPxlTiledClustering(inputImg, outputImg, gdalformat='KEA', noDataVal=0, clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100), calcStats=True, useMeanShiftEstBandWidth=False, tileXSize=200, tileYSize=200):
    """
A function which allows a clustering to be performed using the algorithms available
within the scikit-learn library. The clusterer is applied to a single tile at a time
and therefore produces tile boundaries in the result. However, memory is controlled 
such that usage isn't excessive which it could be when processing a whole image.

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* noDataVal - no data value associated with the input image.
* clusterer - clusterer from scikit-learn which must have a predict function.
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* useMeanShiftEstBandWidth - use the mean-shift algorithm as the clusterer (pass None as the clusterer) where the bandwidth is calculated from the data itself.
* tileXSize - tile size in the x-axis in pixels.
* tileYSize - tile size in the y-axis in pixels.

"""
    if useMeanShiftEstBandWidth:
        from sklearn.cluster import MeanShift, estimate_bandwidth
            
    reader = ImageReader(inputImg, windowxsize=tileXSize, windowysize=tileYSize)
    writer = None
    print('Started .0.', end='', flush=True)
    outCount = 10
    for (info, block) in reader:
        if info.getPercent() > outCount:
            print('.'+str(int(outCount))+'.', end='', flush=True)
            outCount = outCount + 10
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1]*blkShape[2]))).T
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
        
        outClusterValsOutArr = outClusterVals.reshape([1,blkShape[1],blkShape[2]])
        
        if writer is None:
            writer = ImageWriter(outputImg, info=info, firstblock=outClusterValsOutArr, drivername=gdalformat, creationoptions=[])
        else:
            writer.write(outClusterValsOutArr)
    writer.close(calcStats=False)
    print('. Completed')
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)



def performPxlWholeImgClustering(inputImg, outputImg, gdalformat='KEA', noDataVal=0, clusterer=MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100), calcStats=True, useMeanShiftEstBandWidth=False):
    """
A function which allows a clustering to be performed using the algorithms available
within the scikit-learn library. The clusterer is applied to the whole image in one
operation so therefore requires the whole image to be loaded into memory. However, 
if there is sufficent memory all the clustering algorithms within scikit-learn can be
applied without boundary artifacts. 

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* noDataVal - no data value associated with the input image.
* clusterer - clusterer from scikit-learn which must have a predict function.
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* useMeanShiftEstBandWidth - use the mean-shift algorithm as the clusterer (pass None as the clusterer) where the bandwidth is calculated from the data itself.

"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_16UINT)

    if useMeanShiftEstBandWidth:
        from sklearn.cluster import MeanShift, estimate_bandwidth
            
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    nPxls = gdalDS.RasterXSize * gdalDS.RasterYSize
    
    pxlVals = numpy.zeros((gdalDS.RasterCount, nPxls))
    
    for nBand in numpy.arange(gdalDS.RasterCount):
        gdalBand = gdalDS.GetRasterBand(int(nBand+1))
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


