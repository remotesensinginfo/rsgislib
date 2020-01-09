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

import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis

import numpy

import h5py

from rios import applier
from rios import cuiprogress

import lightgbm as lgb

from skopt.space import Real, Integer
from skopt import gp_minimize
from sklearn.metrics import roc_auc_score
from sklearn.metrics import accuracy_score

import gc


def train_lightgbm_binary_classifer(out_mdl_file, cls1_train_file, cls1_valid_file, cls1_test_file, cls2_train_file,
                                    cls2_valid_file, cls2_test_file, out_info_file=None, unbalanced=False, nthread=2,
                                    scale_pos_weight=None):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a binary lightgbm
    classifier. Class 1 is the class which you are interested in and Class 2 is the 'other class'.

    This function requires that lightgbm and skopt modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param cls1_train_file:
    :param cls1_valid_file:
    :param cls1_test_file:
    :param cls2_train_file:
    :param cls2_valid_file:
    :param cls2_test_file:
    :param out_info_file: An optional output JSON file with information about the classifier which has been created.
    :param unbalanced:
    :param nthread:
    :param scale_pos_weight:

    """
    print("Reading Class 1 Training")
    f = h5py.File(cls1_train_file, 'r')
    num_cls1_train_rows = f['DATA/DATA'].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f['DATA/DATA'])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=int)

    print("Reading Class 1 Validation")
    f = h5py.File(cls1_valid_file, 'r')
    num_cls1_valid_rows = f['DATA/DATA'].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f['DATA/DATA'])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=int)

    print("Reading Class 1 Testing")
    f = h5py.File(cls1_test_file, 'r')
    num_cls1_test_rows = f['DATA/DATA'].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f['DATA/DATA'])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=int)

    print("Reading Class 2 Training")
    f = h5py.File(cls2_train_file, 'r')
    num_cls2_train_rows = f['DATA/DATA'].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f['DATA/DATA'])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=int)

    print("Reading Class 2 Validation")
    f = h5py.File(cls2_valid_file, 'r')
    num_cls2_valid_rows = f['DATA/DATA'].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f['DATA/DATA'])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=int)

    print("Reading Class 2 Testing")
    f = h5py.File(cls2_test_file, 'r')
    num_cls2_test_rows = f['DATA/DATA'].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f['DATA/DATA'])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=int)

    print("Finished Reading Data")

    d_train = lgb.Dataset([train_cls2, train_cls1], label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)))
    d_valid = lgb.Dataset([valid_cls2, valid_cls1], label=numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl)))

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))

    space = [Integer(3, 10, name='max_depth'),
             Integer(6, 30, name='num_leaves'),
             Integer(50, 200, name='min_child_samples'),
             Real(0.6, 0.9, name='subsample'),
             Real(0.6, 0.9, name='colsample_bytree'),
             Real(0.05, 0.5, name='learning_rate'),
             Integer(50, 1000, name='max_bin'),
             Real(0, 8, name='min_child_weight'),
             Real(1, 1.2, name='reg_alpha'),
             Real(1, 1.4, name='reg_lambda'),
             Integer(10000, 250000, name='subsample_for_bin'),
             Integer(10, 200, name='early_stopping_rounds'),
             Integer(50, 1000, name='num_boost_round')
             ]

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    def _objective(values):
        if unbalanced:
            params = {'max_depth': values[0],
                      'num_leaves': values[1],
                      'min_child_samples': values[2],
                      'subsample': values[3],
                      'colsample_bytree': values[4],
                      'metric': 'auc,binary_error',
                      'nthread': nthread,
                      'boosting_type': 'gbdt',
                      'objective': 'binary',
                      'learning_rate': values[5],
                      'max_bin': values[6],
                      'min_child_weight': values[7],
                      'min_split_gain': 0,
                      'reg_alpha': values[8],
                      'reg_lambda': values[9],
                      'subsample_freq': 1,
                      'subsample_for_bin': values[10],
                      'boost_from_average': True,
                      'is_unbalance': True}
        else:
            params = {'max_depth': values[0],
                      'num_leaves': values[1],
                      'min_child_samples': values[2],
                      'scale_pos_weight': scale_pos_weight,
                      'subsample': values[3],
                      'colsample_bytree': values[4],
                      'metric': 'auc,binary_error',
                      'nthread': nthread,
                      'boosting_type': 'gbdt',
                      'objective': 'binary',
                      'learning_rate': values[5],
                      'max_bin': values[6],
                      'min_child_weight': values[7],
                      'min_split_gain': 0,
                      'reg_alpha': values[8],
                      'reg_lambda': values[9],
                      'subsample_freq': 1,
                      'subsample_for_bin': values[10],
                      'boost_from_average': True,
                      'is_unbalance': False}

        print('\nNext set of params.....', params)

        early_stopping_rounds = values[11]
        num_boost_round = values[12]
        print("early_stopping_rounds = {}. \t num_boost_round = {}.".format(early_stopping_rounds, num_boost_round))

        evals_results = {}
        model_lgb = lgb.train(params, d_train, valid_sets=[d_train, d_valid],
                              valid_names=['train', 'valid'],
                              evals_result=evals_results,
                              num_boost_round=num_boost_round,
                              early_stopping_rounds=early_stopping_rounds,
                              verbose_eval=None, feval=None)

        auc = -roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
        print('\nAUROC.....', -auc, ".....iter.....", model_lgb.current_iteration())
        gc.collect()
        return auc

    res_gp = gp_minimize(_objective, space, n_calls=20, random_state=0, n_random_starts=10)

    print("Best score={}".format(res_gp.fun))

    best_params = res_gp.x

    print("Best Params:\n{}".format(best_params))

    print("Start Training Find Classifier")

    if unbalanced:
        params = {'max_depth': best_params[0],
                  'num_leaves': best_params[1],
                  'min_child_samples': best_params[2],
                  'subsample': best_params[3],
                  'colsample_bytree': best_params[4],
                  'metric': 'auc',
                  'nthread': nthread,
                  'boosting_type': 'gbdt',
                  'objective': 'binary',
                  'learning_rate': best_params[5],
                  'max_bin': best_params[6],
                  'min_child_weight': best_params[7],
                  'min_split_gain': 0,
                  'reg_alpha': best_params[8],
                  'reg_lambda': best_params[9],
                  'subsample_freq': 1,
                  'subsample_for_bin': best_params[10],
                  'boost_from_average': True,
                  'is_unbalance': True}
    else:
        params = {'max_depth': best_params[0],
                  'num_leaves': best_params[1],
                  'min_child_samples': best_params[2],
                  'scale_pos_weight': scale_pos_weight,
                  'subsample': best_params[3],
                  'colsample_bytree': best_params[4],
                  'metric': 'auc',
                  'nthread': nthread,
                  'boosting_type': 'gbdt',
                  'objective': 'binary',
                  'learning_rate': best_params[5],
                  'max_bin': best_params[6],
                  'min_child_weight': best_params[7],
                  'min_split_gain': 0,
                  'reg_alpha': best_params[8],
                  'reg_lambda': best_params[9],
                  'subsample_freq': 1,
                  'subsample_for_bin': best_params[10],
                  'boost_from_average': True,
                  'is_unbalance': False}

    early_stopping_rounds = best_params[11]
    num_boost_round = best_params[12]

    evals_results = {}
    model = lgb.train(params, d_train, valid_sets=[d_train, d_valid], valid_names=['train', 'valid'],
                      evals_result=evals_results, num_boost_round=num_boost_round,
                      early_stopping_rounds=early_stopping_rounds, verbose_eval=None, feval=None)
    test_auc = roc_auc_score(test_lbl_np, model.predict(test_np))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model.save_model(out_mdl_file)

    pred_test = model.predict(test_np)
    for i in range(test_np.shape[0]):
        if (pred_test[i] >= 0.5):
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))

    if out_info_file is not None:
        out_info = dict()
        out_info['params'] = params
        out_info['test_auc_score'] = test_auc
        out_info['test_accuracy'] = test_acc
        with open(out_info_file, 'w') as outfile:
            import json
            json.dump(out_info, outfile, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)


def apply_lightgbm_binary_classifier(model_file, imgMask, imgMaskVal, imgFileInfo, outProbImg, gdalformat, outClassImg=None, class_thres=5000):
    """
This function applies a trained binary (i.e., two classes) lightgbm model. The function train_lightgbm_binary_classifer
can be used to train such as model. The output image will contain the probability of membership to the class of interest.
You will need to threshold this image to get a final hard classification. Alternative, a hard class output image and
threshold can be applied to this image.

:param model_file: a trained lightgbm binary model which can be loaded with lgb.Booster(model_file=model_file).
:param imgMask: is an image file providing a mask to specify where should be classified. Simplest mask is all the valid data regions (rsgislib.imageutils.genValidMask)
:param imgMaskVal: the pixel value within the imgMask to limit the region to which the classification is applied. Can be used to create a heirachical classification.
:param imgFileInfo: a list of rsgislib.imageutils.ImageBandInfo objects (also used within rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to be used for the classification so it adheres to the training data.
:param outProbImg: output image file with the classification probabilities - this image is scaled by multiplying by 10000.
:param gdalformat: is the output image format - all GDAL supported formats are supported.
:param outClassImg: Optional output image which will contain the hard classification, defined with a threshold on the
                    probability image.
:param class_thres: The threshold used to define the hard classification. Default is 5000 (i.e., probability of 0.5).

    """
    def _applyLGBMClassifier(info, inputs, outputs, otherargs):
        outClassVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
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
                    classVars[..., classVarsIdx] = imgArr[(band - 1)].flatten()
                    classVarsIdx = classVarsIdx + 1
            classVars = classVars[imgMaskVals == otherargs.mskVal]
            ID = ID[imgMaskVals == otherargs.mskVal]
            predClass = numpy.around(otherargs.classifier.predict(classVars) * 10000)
            outClassVals[ID] = predClass
            outClassVals = numpy.expand_dims(
                outClassVals.reshape((inputs.imageMask.shape[1], inputs.imageMask.shape[2])), axis=0)
        outputs.outimage = outClassVals

    classifier = lgb.Booster(model_file=model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = imgMask
    numClassVars = 0
    for imgFile in imgFileInfo:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)

    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outProbImg
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = imgMaskVal
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = imgFileInfo

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
    print("Applying the Classifier")
    applier.apply(_applyLGBMClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")
    rsgislib.imageutils.popImageStats(outProbImg, usenodataval=True, nodataval=0, calcpyramids=True)

    if outClassImg is not None:
        rsgislib.imagecalc.imageMath(outProbImg, outClassImg, 'b1>{}?1:0'.format(class_thres), gdalformat, rsgislib.TYPE_8UINT)
        rsgislib.rastergis.populateStats(outClassImg, addclrtab=True, calcpyramids=True, ignorezero=True)

