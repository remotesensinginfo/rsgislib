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

import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis
from rsgislib.classification import ClassSimpleInfoObj

import os
import os.path
import shutil
import random

from multiprocessing import Pool

from sklearn.model_selection import GridSearchCV
from sklearn.ensemble import RandomForestClassifier

import h5py

from rios import applier
from rios import cuiprogress
from rios import rat


def train_sklearn_classifer_gridsearch(classTrainInfo, paramSearchSampNum=0, gridSearch=GridSearchCV(RandomForestClassifier(), {})):
    """
A function to find the 'optimal' parameters for classification using a Grid Search
(http://scikit-learn.org/stable/modules/grid_search.html).
The returned classifier instance will be trained using all the inputted data.

:param classTrainInfo: list of rsgislib.classification.ClassSimpleInfoObj objects which will be used to
                       train the classifier.
:param paramSearchSampNum: the number of samples that will be randomly sampled from the training data for each class
                           for applying the grid search (tend to use a small data sample as can take a long time).
                           A value of 500 would use 500 samples per class.
:param gridSearch: is an instance of the sklearn.model_selection.GridSearchCV with an instance of the choosen
                   classifier and parameters to be searched.

    """
    if len(classTrainInfo) < 2:
        rsgislib.RSGISPyException(
            "Need at least 2 classes to be worth running findClassifierParametersAndTrain function.")

    first = True
    numVars = 0
    numVals = 0
    for classInfoVal in classTrainInfo.values():
        dataShp = h5py.File(classInfoVal.file_h5, 'r')['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]

    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    classArr = numpy.zeros([numVals], dtype=int)

    if paramSearchSampNum != 0:
        dataArrSamp = numpy.zeros([(len(classTrainInfo) * paramSearchSampNum), numVars], dtype=float)
        classArrSamp = numpy.zeros([(len(classTrainInfo) * paramSearchSampNum)], dtype=int)

    rowInit = 0
    rowInitSamp = 0
    for key in classTrainInfo:
        # Open the dataset
        f = h5py.File(classTrainInfo[key].file_h5, 'r')
        numRows = f['DATA/DATA'].shape[0]
        # Copy data and populate classid array
        dataArr[rowInit:(rowInit + numRows)] = f['DATA/DATA']
        classArr[rowInit:(rowInit + numRows)] = classTrainInfo[key].id
        if paramSearchSampNum != 0:
            # Create random index to sample the whole dataset.
            sampleIdxs = numpy.random.randint(0, high=numRows, size=paramSearchSampNum)
            # Sample the input data for classifier optimisation.
            dataArrSamp[rowInitSamp:(rowInitSamp + paramSearchSampNum)] = dataArr[rowInit:(rowInit + numRows)][sampleIdxs]
            classArrSamp[rowInitSamp:(rowInitSamp + paramSearchSampNum)] = classTrainInfo[key].id
            rowInitSamp += paramSearchSampNum
        rowInit += numRows
        f.close()

    if paramSearchSampNum == 0:
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
    print('Classifier Train Score = {}%'.format(round(accVal * 100, 2)))

    return gridSearch.best_estimator_


def train_sklearn_classifier(classTrainInfo, skClassifier):
    """
This function trains the classifier.

:param classTrainInfo: list of rsgislib.classification.ClassSimpleInfoObj objects which will be used to
                       train the classifier.
:param skClassifier: an instance of a parameterised scikit-learn classifier
                     (http://scikit-learn.org/stable/supervised_learning.html)

    """
    if len(classTrainInfo) < 2:
        rsgislib.RSGISPyException("Need at least 2 classes to be worth running trainClassifier function.")

    first = True
    numVars = 0
    numVals = 0
    for classInfoVal in classTrainInfo.values():
        dataShp = h5py.File(classInfoVal.file_h5, 'r')['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        numVals += dataShp[0]

    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    classArr = numpy.zeros([numVals], dtype=int)

    rowInit = 0
    for key in classTrainInfo:
        # Open the dataset
        f = h5py.File(classTrainInfo[key].file_h5, 'r')
        numRows = f['DATA/DATA'].shape[0]
        # Copy data and populate classid array
        dataArr[rowInit:(rowInit + numRows)] = f['DATA/DATA']
        classArr[rowInit:(rowInit + numRows)] = classTrainInfo[key].id
        rowInit += numRows
        f.close()

    print("Training data size: {} x {}".format(dataArr.shape[0], dataArr.shape[1]))

    print('Training Classifier')
    skClassifier.fit(dataArr, classArr)
    print("Completed")

    print('Calc Classifier Accuracy')
    accVal = skClassifier.score(dataArr, classArr)
    print('Classifier Train Score = {}%'.format(round(accVal * 100, 2)))


def apply_sklearn_classifer(classTrainInfo, skClassifier, imgMask, imgMaskVal, imgFileInfo, outputImg, gdalformat, classClrNames=True, outScoreImg=None):
    """
This function uses a trained classifier and applies it to the provided input image.

:param classTrainInfo: dict (where the key is the class name) of rsgislib.classification.ClassSimpleInfoObj
                       objects which will be used to train the classifier (i.e., train_sklearn_classifier()),
                       provide pixel value id and RGB class values.
:param skClassifier: a trained instance of a scikit-learn classifier
                     (e.g., use train_sklearn_classifier or train_sklearn_classifer_gridsearch)
:param imgMask: is an image file providing a mask to specify where should be classified. Simplest mask is all
                the valid data regions (rsgislib.imageutils.gen_valid_mask)
:param imgMaskVal: the pixel value within the imgMask to limit the region to which the classification is applied.
                   Can be used to create a heirachical classification.
:param imgFileInfo: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                    rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to
                    be used for the classification so it adheres to the training data.
:param outputImg: output image file with the classification. Note. by default a colour table and class names column
                  is added to the image. If an error is produced use HFA or KEA formats.
:param gdalformat: is the output image format - all GDAL supported formats are supported.
:param classClrNames: default is True and therefore a colour table will the colours specified in classTrainInfo
                      and a ClassName column (from imgFileInfo) will be added to the output file.
:param outScoreImg: A file path for a score image. If None then not outputted. Note, this function uses the
                    predict_proba() function from the scikit-learn model which isn't available for all classifiers
                    and therefore might produce an error if called on a model which doesn't have this function. For
                    example, sklearn.svm.SVC.

    """
    out_score_img = False
    if outScoreImg is not None:
        out_score_img = True

    infiles = applier.FilenameAssociations()
    infiles.imageMask = imgMask
    numClassVars = 0
    for imgFile in imgFileInfo:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)

    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    if out_score_img:
        outfiles.out_score_img = outScoreImg
    otherargs = applier.OtherInputs()
    otherargs.classifier = skClassifier
    otherargs.mskVal = imgMaskVal
    otherargs.numClassVars = numClassVars
    otherargs.n_classes = len(classTrainInfo)
    otherargs.imgFileInfo = imgFileInfo
    otherargs.out_score_img = out_score_img

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    # RIOS function to apply classifer
    def _applySKClassifier(info, inputs, outputs, otherargs):
        """
        Internal function for rios applier. Used within applyClassifer.
        """
        outClassVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint32)

        if otherargs.out_score_img:
            outScoreVals = numpy.zeros((otherargs.n_classes, inputs.imageMask.shape[1], inputs.imageMask.shape[2]),
                                       dtype=numpy.float32)

        if numpy.any(inputs.imageMask == otherargs.mskVal):
            outClassVals = outClassVals.flatten()
            if otherargs.out_score_img:
                outScoreVals = outScoreVals.reshape(outClassVals.shape[0], otherargs.n_classes)
            imgMaskVals = inputs.imageMask.flatten()
            classVars = numpy.zeros((outClassVals.shape[0], otherargs.numClassVars), dtype=numpy.float)
            # Array index which can be used to populate the output array following masking etc.
            ID = numpy.arange(imgMaskVals.shape[0])
            classVarsIdx = 0
            for imgFile in otherargs.imgFileInfo:
                imgArr = inputs.__dict__[imgFile.name]
                for band in imgFile.bands:
                    classVars[..., classVarsIdx] = imgArr[(band - 1)].flatten()
                    classVarsIdx = classVarsIdx + 1
            classVars = classVars[imgMaskVals == otherargs.mskVal]
            ID = ID[imgMaskVals == otherargs.mskVal]
            predClass = otherargs.classifier.predict(classVars)
            outClassVals[ID] = predClass
            outClassVals = numpy.expand_dims(
                outClassVals.reshape((inputs.imageMask.shape[1], inputs.imageMask.shape[2])), axis=0)
            if otherargs.out_score_img:
                predClassScore = otherargs.classifier.predict_proba(classVars)
                outScoreVals[ID] = predClassScore
                outScoreVals = outScoreVals.T
                outScoreVals = outScoreVals.reshape(
                        (otherargs.n_classes, inputs.imageMask.shape[1], inputs.imageMask.shape[2]))
        outputs.outimage = outClassVals
        if otherargs.out_score_img:
            outputs.out_score_img = outScoreVals

    print("Applying the Classifier")
    applier.apply(_applySKClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")
    rsgislib.rastergis.pop_rat_img_stats(clumps_img=outputImg, add_clr_tab=True, calc_pyramids=True, ignore_zero=True)
    if out_score_img:
        rsgislib.imageutils.pop_img_stats(outScoreImg, usenodataval=True, nodataval=0, calcpyramids=True)

    if classClrNames:
        ratDataset = gdal.Open(outputImg, gdal.GA_Update)
        red = rat.readColumn(ratDataset, 'Red')
        green = rat.readColumn(ratDataset, 'Green')
        blue = rat.readColumn(ratDataset, 'Blue')
        ClassName = numpy.empty_like(red, dtype=numpy.dtype('a255'))
        ClassName[...] = ''

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



