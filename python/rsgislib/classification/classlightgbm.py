#! /usr/bin/env python
############################################################################
#  classlightgbm.py
#
#  Copyright 2019 RSGISLib.
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
# Date: 05/11/2019
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

from osgeo import gdal

from rios import applier
from rios import cuiprogress
from rios import rat

HAVE_LIGHTGBM = True
try:
    import lightgbm as lgb
except ImportError:
    HAVE_LIGHTGBM = False

from sklearn.metrics import roc_auc_score
from sklearn.metrics import accuracy_score

import gc

import json


def optimise_lightgbm_binary_classifier(
    out_params_file,
    cls1_train_file,
    cls1_valid_file,
    cls2_train_file,
    cls2_valid_file,
    unbalanced=False,
    n_threads=1,
    scale_pos_weight=None,
    early_stopping_rounds=100,
    num_iterations=5000,
    num_boost_round=100,
    max_n_leaves=50,
    learning_rate=0.05,
    mdl_cls_obj=None,
):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a binary lightgbm
    classifier. Class 1 is the class which you are interested in and Class 2 is the 'other class'.

    This function requires that lightgbm and skopt modules to be installed.

    :param out_params_file: The output model parameters which have been optimised.
    :param cls1_train_file: Training samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_valid_file: Validation samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_test_file: Testing samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls2_train_file: Training samples HDF5 file for the 'other' class
    :param cls2_valid_file: Validation samples HDF5 file for the 'other' class
    :param cls2_test_file: Testing samples HDF5 file for the 'other' class
    :param unbalanced: Specify that the training data is unbalance (i.e., a different number of samples per class)
                       and LightGBM will try to take this into account during training.
    :param n_threads: The number of threads to use for the training.
    :param scale_pos_weight: Optional, default is None. If None then a value will automatically be calculated.
                             Parameter used to balance imbalanced training data.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")
    from skopt.space import Real, Integer
    from skopt import gp_minimize

    print("Reading Class 1 Training")
    f = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=int)

    print("Reading Class 1 Validation")
    f = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=int)

    print("Reading Class 2 Training")
    f = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=int)

    print("Reading Class 2 Validation")
    f = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=int)

    print("Finished Reading Data")

    d_train = lgb.Dataset(
        [train_cls2, train_cls1],
        label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)),
    )
    d_valid = lgb.Dataset(
        [valid_cls2, valid_cls1],
        label=numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl)),
    )

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))

    space = [
        Integer(3, 10, name="max_depth"),
        Integer(6, max_n_leaves, name="num_leaves"),
        Integer(3, 50, name="min_data_in_leaf"),
        Real(0, 5, name="lambda_l1"),
        Real(0, 3, name="lambda_l2"),
        Real(0.1, 0.9, name="feature_fraction"),
        Real(0.8, 1.0, name="bagging_fraction"),
        Real(0.001, 0.1, name="min_split_gain"),
        Real(1, 50, name="min_child_weight"),
        Real(1, 1.2, name="reg_alpha"),
        Real(1, 1.4, name="reg_lambda"),
    ]

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    def _objective(values):
        if unbalanced:
            params = {
                "max_depth": values[0],
                "num_leaves": values[1],
                "min_data_in_leaf": values[2],
                "feature_pre_filter": False,
                "lambda_l1": values[3],
                "lambda_l2": values[4],
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": values[5],
                "bagging_fraction": values[6],
                "min_split_gain": values[7],
                "min_child_weight": values[8],
                "reg_alpha": values[9],
                "reg_lambda": values[10],
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": True,
                "verbose": -1,
            }
        else:
            params = {
                "max_depth": values[0],
                "num_leaves": values[1],
                "min_data_in_leaf": values[2],
                "feature_pre_filter": False,
                "lambda_l1": values[3],
                "lambda_l2": values[4],
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": values[5],
                "bagging_fraction": values[6],
                "min_split_gain": values[7],
                "min_child_weight": values[8],
                "reg_alpha": values[9],
                "reg_lambda": values[10],
                "num_iterations": num_iterations,
                "scale_pos_weight": scale_pos_weight,
                "boost_from_average": True,
                "is_unbalance": False,
                "verbose": -1,
            }

        print("\nNext set of params.....", params)

        evals_results = {}
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            evals_result=evals_results,
            num_boost_round=num_boost_round,
            early_stopping_rounds=early_stopping_rounds,
            verbose_eval=None,
            feval=None,
            init_model=mdl_cls_obj,
        )

        auc = -roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
        print("\nAUROC.....", -auc, ".....iter.....", model_lgb.current_iteration())
        gc.collect()
        return auc

    res_gp = gp_minimize(
        _objective, space, n_calls=20, random_state=0, n_random_starts=10
    )

    print("Best score={}".format(res_gp.fun))

    best_params = res_gp.x

    params = {
        "max_depth": int(best_params[0]),
        "num_leaves": int(best_params[1]),
        "min_data_in_leaf": int(best_params[2]),
        "feature_pre_filter": False,
        "lambda_l1": float(best_params[3]),
        "lambda_l2": float(best_params[4]),
        "metric": "auc,binary_error",
        "boosting_type": "gbdt",
        "objective": "binary",
        "feature_fraction": float(best_params[5]),
        "bagging_fraction": float(best_params[6]),
        "min_split_gain": float(best_params[7]),
        "min_child_weight": float(best_params[8]),
        "reg_alpha": float(best_params[9]),
        "reg_lambda": float(best_params[10]),
        "scale_pos_weight": float(scale_pos_weight),
        "boost_from_average": True,
        "is_unbalance": unbalanced,
    }

    with open(out_params_file, "w") as fp:
        json.dump(
            params,
            fp,
            sort_keys=True,
            indent=4,
            separators=(",", ": "),
            ensure_ascii=False,
        )


def train_lightgbm_binary_classifier(
    out_mdl_file,
    cls_params_file,
    cls1_train_file,
    cls1_valid_file,
    cls1_test_file,
    cls2_train_file,
    cls2_valid_file,
    cls2_test_file,
    unbalanced=False,
    n_threads=1,
    scale_pos_weight=None,
    early_stopping_rounds=100,
    num_iterations=5000,
    num_boost_round=100,
    learning_rate=0.05,
    mdl_cls_obj=None,
):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a binary lightgbm
    classifier. Class 1 is the class which you are interested in and Class 2 is the 'other class'.

    This function requires that lightgbm and skopt modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param cls_params_file: A JSON file with the model parameters
    :param cls1_train_file: Training samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_valid_file: Validation samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_test_file: Testing samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls2_train_file: Training samples HDF5 file for the 'other' class
    :param cls2_valid_file: Validation samples HDF5 file for the 'other' class
    :param cls2_test_file: Testing samples HDF5 file for the 'other' class
    :param unbalanced: Specify that the training data is unbalance (i.e., a different number of samples per class)
                       and LightGBM will try to take this into account during training.
    :param n_threads: The number of threads to use for the training.
    :param scale_pos_weight: Optional, default is None. If None then a value will automatically be calculated.
                             Parameter used to balance imbalanced training data.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    print("Reading Class 1 Training")
    f = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=int)

    print("Reading Class 1 Validation")
    f = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=int)

    print("Reading Class 1 Testing")
    f = h5py.File(cls1_test_file, "r")
    num_cls1_test_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f["DATA/DATA"])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=int)

    print("Reading Class 2 Training")
    f = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=int)

    print("Reading Class 2 Validation")
    f = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=int)

    print("Reading Class 2 Testing")
    f = h5py.File(cls2_test_file, "r")
    num_cls2_test_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f["DATA/DATA"])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=int)

    print("Finished Reading Data")

    d_train = lgb.Dataset(
        [train_cls2, train_cls1],
        label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)),
    )
    d_valid = lgb.Dataset(
        [valid_cls2, valid_cls1],
        label=numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl)),
    )

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    with open(cls_params_file) as f:
        cls_params = json.load(f)

    print("Start Training Find Classifier")

    if unbalanced:
        params = {
            "max_depth": cls_params["max_depth"],
            "num_leaves": cls_params["num_leaves"],
            "min_data_in_leaf": cls_params["min_data_in_leaf"],
            "feature_pre_filter": False,
            "lambda_l1": cls_params["lambda_l1"],
            "lambda_l2": cls_params["lambda_l2"],
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": cls_params["feature_fraction"],
            "bagging_fraction": cls_params["bagging_fraction"],
            "min_split_gain": cls_params["min_split_gain"],
            "min_child_weight": cls_params["min_child_weight"],
            "reg_alpha": cls_params["reg_alpha"],
            "reg_lambda": cls_params["reg_lambda"],
            "num_iterations": num_iterations,
            "scale_pos_weight": scale_pos_weight,
            "boost_from_average": True,
            "is_unbalance": True,
            "verbose": -1,
        }
    else:
        params = {
            "max_depth": cls_params["max_depth"],
            "num_leaves": cls_params["num_leaves"],
            "min_data_in_leaf": cls_params["min_data_in_leaf"],
            "feature_pre_filter": False,
            "lambda_l1": cls_params["lambda_l1"],
            "lambda_l2": cls_params["lambda_l2"],
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": cls_params["feature_fraction"],
            "bagging_fraction": cls_params["bagging_fraction"],
            "min_split_gain": cls_params["min_split_gain"],
            "min_child_weight": cls_params["min_child_weight"],
            "reg_alpha": cls_params["reg_alpha"],
            "reg_lambda": cls_params["reg_lambda"],
            "num_iterations": num_iterations,
            "scale_pos_weight": scale_pos_weight,
            "boost_from_average": True,
            "is_unbalance": False,
            "verbose": -1,
        }

    evals_results = {}
    model = lgb.train(
        params,
        d_train,
        valid_sets=[d_train, d_valid],
        valid_names=["train", "valid"],
        evals_result=evals_results,
        num_boost_round=num_boost_round,
        early_stopping_rounds=early_stopping_rounds,
        verbose_eval=None,
        feval=None,
        init_model=mdl_cls_obj,
    )
    test_auc = roc_auc_score(test_lbl_np, model.predict(test_np))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model.save_model(out_mdl_file)

    pred_test = model.predict(test_np)
    for i in range(test_np.shape[0]):
        if pred_test[i] >= 0.5:
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def train_opt_lightgbm_binary_classifier(
    out_mdl_file,
    out_params_file,
    cls1_train_file,
    cls1_valid_file,
    cls1_test_file,
    cls2_train_file,
    cls2_valid_file,
    cls2_test_file,
    unbalanced=False,
    n_threads=1,
    scale_pos_weight=None,
    early_stopping_rounds=100,
    num_iterations=5000,
    num_boost_round=100,
    learning_rate=0.05,
    max_n_leaves=50,
    mdl_cls_obj=None,
):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a binary lightgbm
    classifier. Class 1 is the class which you are interested in and Class 2 is the 'other class'.

    This function requires that lightgbm and skopt modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param out_params_file: The output model parameters which have been optimised.
    :param cls1_train_file: Training samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_valid_file: Validation samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls1_test_file: Testing samples HDF5 file for the primary class (i.e., the one being classified)
    :param cls2_train_file: Training samples HDF5 file for the 'other' class
    :param cls2_valid_file: Validation samples HDF5 file for the 'other' class
    :param cls2_test_file: Testing samples HDF5 file for the 'other' class
    :param unbalanced: Specify that the training data is unbalance (i.e., a different number of samples per class)
                       and LightGBM will try to take this into account during training.
    :param n_threads: The number of threads to use for the training.
    :param scale_pos_weight: Optional, default is None. If None then a value will automatically be calculated.
                             Parameter used to balance imbalanced training data.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    from skopt.space import Real, Integer
    from skopt import gp_minimize

    print("Reading Class 1 Training")
    f = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=int)

    print("Reading Class 1 Validation")
    f = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=int)

    print("Reading Class 1 Testing")
    f = h5py.File(cls1_test_file, "r")
    num_cls1_test_rows = f["DATA/DATA"].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f["DATA/DATA"])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=int)

    print("Reading Class 2 Training")
    f = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=int)

    print("Reading Class 2 Validation")
    f = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=int)

    print("Reading Class 2 Testing")
    f = h5py.File(cls2_test_file, "r")
    num_cls2_test_rows = f["DATA/DATA"].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f["DATA/DATA"])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=int)

    print("Finished Reading Data")

    d_train = lgb.Dataset(
        [train_cls2, train_cls1],
        label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)),
    )
    d_valid = lgb.Dataset(
        [valid_cls2, valid_cls1],
        label=numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl)),
    )

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))

    space = [
        Integer(3, 10, name="max_depth"),
        Integer(6, max_n_leaves, name="num_leaves"),
        Integer(3, 50, name="min_data_in_leaf"),
        Real(0, 5, name="lambda_l1"),
        Real(0, 3, name="lambda_l2"),
        Real(0.1, 0.9, name="feature_fraction"),
        Real(0.8, 1.0, name="bagging_fraction"),
        Real(0.001, 0.1, name="min_split_gain"),
        Real(1, 50, name="min_child_weight"),
        Real(1, 1.2, name="reg_alpha"),
        Real(1, 1.4, name="reg_lambda"),
    ]

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    def _objective(values):
        if unbalanced:
            params = {
                "max_depth": values[0],
                "num_leaves": values[1],
                "min_data_in_leaf": values[2],
                "feature_pre_filter": False,
                "lambda_l1": values[3],
                "lambda_l2": values[4],
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": values[5],
                "bagging_fraction": values[6],
                "min_split_gain": values[7],
                "min_child_weight": values[8],
                "reg_alpha": values[9],
                "reg_lambda": values[10],
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": True,
                "verbose": -1,
            }
        else:
            params = {
                "max_depth": values[0],
                "num_leaves": values[1],
                "min_data_in_leaf": values[2],
                "feature_pre_filter": False,
                "lambda_l1": values[3],
                "lambda_l2": values[4],
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": values[5],
                "bagging_fraction": values[6],
                "min_split_gain": values[7],
                "min_child_weight": values[8],
                "reg_alpha": values[9],
                "reg_lambda": values[10],
                "num_iterations": num_iterations,
                "scale_pos_weight": scale_pos_weight,
                "boost_from_average": True,
                "is_unbalance": False,
                "verbose": -1,
            }

        print("\nNext set of params.....", params)

        evals_results = {}
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            evals_result=evals_results,
            num_boost_round=num_boost_round,
            early_stopping_rounds=early_stopping_rounds,
            verbose_eval=None,
            feval=None,
            init_model=mdl_cls_obj,
        )

        auc = -roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
        print("\nAUROC.....", -auc, ".....iter.....", model_lgb.current_iteration())
        gc.collect()
        return auc

    res_gp = gp_minimize(
        _objective, space, n_calls=20, random_state=0, n_random_starts=10
    )

    print("Best score={}".format(res_gp.fun))

    best_params = res_gp.x

    params = {
        "max_depth": int(best_params[0]),
        "num_leaves": int(best_params[1]),
        "min_data_in_leaf": int(best_params[2]),
        "feature_pre_filter": False,
        "lambda_l1": float(best_params[3]),
        "lambda_l2": float(best_params[4]),
        "metric": "auc,binary_error",
        "boosting_type": "gbdt",
        "objective": "binary",
        "feature_fraction": float(best_params[5]),
        "bagging_fraction": float(best_params[6]),
        "min_split_gain": float(best_params[7]),
        "min_child_weight": float(best_params[8]),
        "reg_alpha": float(best_params[9]),
        "reg_lambda": float(best_params[10]),
        "scale_pos_weight": float(scale_pos_weight),
        "boost_from_average": True,
        "is_unbalance": unbalanced,
    }

    with open(out_params_file, "w") as fp:
        json.dump(
            params,
            fp,
            sort_keys=True,
            indent=4,
            separators=(",", ": "),
            ensure_ascii=False,
        )

    print("Start Training Find Classifier")

    if unbalanced:
        params = {
            "max_depth": best_params[0],
            "num_leaves": best_params[1],
            "min_data_in_leaf": best_params[2],
            "feature_pre_filter": False,
            "lambda_l1": best_params[3],
            "lambda_l2": best_params[4],
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": best_params[5],
            "bagging_fraction": best_params[6],
            "min_split_gain": best_params[7],
            "min_child_weight": best_params[8],
            "reg_alpha": best_params[9],
            "reg_lambda": best_params[10],
            "num_iterations": num_iterations,
            "scale_pos_weight": scale_pos_weight,
            "boost_from_average": True,
            "is_unbalance": True,
            "verbose": -1,
        }
    else:
        params = {
            "max_depth": best_params[0],
            "num_leaves": best_params[1],
            "min_data_in_leaf": best_params[2],
            "feature_pre_filter": False,
            "lambda_l1": best_params[3],
            "lambda_l2": best_params[4],
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": best_params[5],
            "bagging_fraction": best_params[6],
            "min_split_gain": best_params[7],
            "min_child_weight": best_params[8],
            "reg_alpha": best_params[9],
            "reg_lambda": best_params[10],
            "num_iterations": num_iterations,
            "scale_pos_weight": scale_pos_weight,
            "boost_from_average": True,
            "is_unbalance": False,
            "verbose": -1,
        }

    evals_results = {}
    model = lgb.train(
        params,
        d_train,
        valid_sets=[d_train, d_valid],
        valid_names=["train", "valid"],
        evals_result=evals_results,
        num_boost_round=num_boost_round,
        early_stopping_rounds=early_stopping_rounds,
        verbose_eval=None,
        feval=None,
        init_model=mdl_cls_obj,
    )
    test_auc = roc_auc_score(test_lbl_np, model.predict(test_np))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model.save_model(out_mdl_file)

    pred_test = model.predict(test_np)
    for i in range(test_np.shape[0]):
        if pred_test[i] >= 0.5:
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def apply_lightgbm_binary_classifier(
    model_file,
    in_img_msk,
    img_mask_val,
    img_file_info,
    out_score_img,
    gdalformat,
    out_class_img=None,
    class_thres=5000,
):
    """
    This function applies a trained binary (i.e., two classes) lightgbm model. The function train_lightgbm_binary_classifier
    can be used to train such as model. The output image will contain the softmax score for the class of interest.
    You will need to threshold this image to get a final hard classification. Alternative, a hard class output
    image and threshold can be applied to this image. Note. the softmax score is not a probability.

    :param model_file: a trained lightgbm binary model which can be loaded with lgb.Booster(model_file=model_file).
    :param in_img_msk: is an image file providing a mask to specify where should be classified. Simplest mask is all the
                    valid data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the imgMask to limit the region to which the classification is applied.
                       Can be used to create a heirachical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                        rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) to identify which images and bands are to
                        be used for the classification so it adheres to the training data.
    :param out_score_img: output image file with the classification softmax score - this image is scaled by
                       multiplying by 10000.
    :param gdalformat: is the output image format - all GDAL supported formats are supported.
    :param out_class_img: Optional output image which will contain the hard classification, defined with a threshold on the
                        probability image.
    :param class_thres: The threshold used to define the hard classification. Default is 5000 (i.e., probability of 0.5).

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    def _applyLGBMClassifier(info, inputs, outputs, otherargs):
        outClassVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            outClassVals = outClassVals.flatten()
            imgMaskVals = inputs.imageMask.flatten()
            classVars = numpy.zeros(
                (outClassVals.shape[0], otherargs.numClassVars), dtype=numpy.float32
            )
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
                outClassVals.reshape(
                    (inputs.imageMask.shape[1], inputs.imageMask.shape[2])
                ),
                axis=0,
            )
        outputs.outimage = outClassVals

    classifier = lgb.Booster(model_file=model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = in_img_msk
    numClassVars = 0
    for imgFile in img_file_info:
        infiles.__dict__[imgFile.name] = imgFile.file_name
        numClassVars = numClassVars + len(imgFile.bands)

    outfiles = applier.FilenameAssociations()
    outfiles.outimage = out_score_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = img_mask_val
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = img_file_info

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
    applier.apply(
        _applyLGBMClassifier, infiles, outfiles, otherargs, controls=aControls
    )
    print("Completed")
    rsgislib.imageutils.pop_img_stats(
        out_score_img, use_no_data=True, no_data_val=0, calc_pyramids=True
    )

    if out_class_img is not None:
        rsgislib.imagecalc.image_math(
            out_score_img,
            out_class_img,
            "b1>{}?1:0".format(class_thres),
            gdalformat,
            rsgislib.TYPE_8UINT,
        )
        rsgislib.rastergis.pop_rat_img_stats(
            out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )


def train_lightgbm_multiclass_classifier(
    out_mdl_file,
    cls_info_dict,
    out_info_file=None,
    unbalanced=False,
    n_threads=1,
    early_stopping_rounds=100,
    num_iterations=5000,
    num_boost_round=100,
    learning_rate=0.05,
    mdl_cls_obj=None,
):
    """
    A function which performs a bayesian optimisation of the hyper-parameters for a multiclass lightgbm
    classifier. A dict of class information, as ClassInfoObj objects, is defined with the training data.

    This function requires that lightgbm and skopt modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param cls_info_dict: dict (key is string with class name) of ClassInfoObj objects defining the training data.
    :param out_info_file: An optional output JSON file with information about the classifier which has been created.
    :param unbalanced:
    :param n_threads:
    :param scale_pos_weight:

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    from skopt.space import Real, Integer
    from skopt import gp_minimize

    n_classes = len(cls_info_dict)
    for clsname in cls_info_dict:
        if cls_info_dict[clsname].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    clsname, cls_info_dict[clsname].id
                )
            )

    cls_data_dict = {}
    train_data_lst = []
    train_lbls_lst = []
    valid_data_lst = []
    valid_lbls_lst = []
    test_data_lst = []
    test_lbls_lst = []
    cls_ids = []
    for clsname in cls_info_dict:
        sgl_cls_info = {}
        print("Reading Class {} Training".format(clsname))
        f = h5py.File(cls_info_dict[clsname].train_file_h5, "r")
        sgl_cls_info["train_n_rows"] = f["DATA/DATA"].shape[0]
        sgl_cls_info["train_data"] = numpy.array(f["DATA/DATA"])
        sgl_cls_info["train_data_lbls"] = numpy.zeros(
            sgl_cls_info["train_n_rows"], dtype=int
        )
        sgl_cls_info["train_data_lbls"][...] = cls_info_dict[clsname].id
        f.close()
        train_data_lst.append(sgl_cls_info["train_data"])
        train_lbls_lst.append(sgl_cls_info["train_data_lbls"])

        print("Reading Class {} Validation".format(clsname))
        f = h5py.File(cls_info_dict[clsname].valid_file_h5, "r")
        sgl_cls_info["valid_n_rows"] = f["DATA/DATA"].shape[0]
        sgl_cls_info["valid_data"] = numpy.array(f["DATA/DATA"])
        sgl_cls_info["valid_data_lbls"] = numpy.zeros(
            sgl_cls_info["valid_n_rows"], dtype=int
        )
        sgl_cls_info["valid_data_lbls"][...] = cls_info_dict[clsname].id
        f.close()
        valid_data_lst.append(sgl_cls_info["valid_data"])
        valid_lbls_lst.append(sgl_cls_info["valid_data_lbls"])

        print("Reading Class {} Testing".format(clsname))
        f = h5py.File(cls_info_dict[clsname].test_file_h5, "r")
        sgl_cls_info["test_n_rows"] = f["DATA/DATA"].shape[0]
        sgl_cls_info["test_data"] = numpy.array(f["DATA/DATA"])
        sgl_cls_info["test_data_lbls"] = numpy.zeros(
            sgl_cls_info["test_n_rows"], dtype=int
        )
        sgl_cls_info["test_data_lbls"][...] = cls_info_dict[clsname].id
        f.close()
        test_data_lst.append(sgl_cls_info["test_data"])
        test_lbls_lst.append(sgl_cls_info["test_data_lbls"])

        cls_data_dict[clsname] = sgl_cls_info
        cls_ids.append(cls_info_dict[clsname].id)

    print("Finished Reading Data")

    d_train = lgb.Dataset(
        numpy.concatenate(train_data_lst), label=numpy.concatenate(train_lbls_lst)
    )
    d_valid = lgb.Dataset(
        numpy.concatenate(valid_data_lst), label=numpy.concatenate(valid_lbls_lst)
    )

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbl_np = numpy.concatenate(valid_lbls_lst)

    test_np = numpy.concatenate(test_data_lst)
    test_lbl_np = numpy.concatenate(test_lbls_lst)

    space = [
        Integer(3, 10, name="max_depth"),
        Integer(6, 50, name="num_leaves"),
        Integer(10, 50, name="min_data_in_leaf"),
        Real(0, 5, name="lambda_l1"),
        Real(0, 3, name="lambda_l2"),
        Real(0.1, 0.9, name="feature_fraction"),
        Real(0.8, 1.0, name="bagging_fraction"),
        Real(0.001, 0.1, name="min_split_gain"),
        Real(1, 50, name="min_child_weight"),
        Real(1, 1.2, name="reg_alpha"),
        Real(1, 1.4, name="reg_lambda"),
    ]

    def _objective(values):
        if unbalanced:
            params = {
                "max_depth": values[0],
                "num_leaves": values[1],
                "min_data_in_leaf": values[2],
                "feature_pre_filter": False,
                "lambda_l1": values[3],
                "lambda_l2": values[4],
                "metric": "multi_logloss",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "multiclass",
                "num_class": n_classes,
                "learning_rate": learning_rate,
                "feature_fraction": values[5],
                "bagging_fraction": values[6],
                "min_split_gain": values[7],
                "min_child_weight": values[8],
                "reg_alpha": values[9],
                "reg_lambda": values[10],
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": True,
            }
        else:
            params = {
                "max_depth": values[0],
                "num_leaves": values[1],
                "min_data_in_leaf": values[2],
                "feature_pre_filter": False,
                "lambda_l1": values[3],
                "lambda_l2": values[4],
                "metric": "multi_logloss",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "multiclass",
                "num_class": n_classes,
                "learning_rate": learning_rate,
                "feature_fraction": values[5],
                "bagging_fraction": values[6],
                "min_split_gain": values[7],
                "min_child_weight": values[8],
                "reg_alpha": values[9],
                "reg_lambda": values[10],
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": False,
            }

        print("\nNext set of params.....", params)

        evals_results = {}
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            evals_result=evals_results,
            num_boost_round=num_boost_round,
            early_stopping_rounds=early_stopping_rounds,
            verbose_eval=None,
            feval=None,
            init_model=mdl_cls_obj,
        )

        vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)

        acc_score = -accuracy_score(vaild_lbl_np, vld_preds_idxs)
        print(
            "\nAccScore.....",
            -acc_score,
            ".....iter.....",
            model_lgb.current_iteration(),
        )
        gc.collect()
        return acc_score

    res_gp = gp_minimize(
        _objective, space, n_calls=20, random_state=0, n_random_starts=10
    )

    print("Best score={}".format(res_gp.fun))
    best_params = res_gp.x
    print("Best Params:\n{}".format(best_params))

    print("Start Training Find Classifier")
    if unbalanced:
        params = {
            "max_depth": best_params[0],
            "num_leaves": best_params[1],
            "min_data_in_leaf": best_params[2],
            "feature_pre_filter": False,
            "lambda_l1": best_params[3],
            "lambda_l2": best_params[4],
            "metric": "multi_logloss",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "learning_rate": learning_rate,
            "feature_fraction": best_params[5],
            "bagging_fraction": best_params[6],
            "min_split_gain": best_params[7],
            "min_child_weight": best_params[8],
            "reg_alpha": best_params[9],
            "reg_lambda": best_params[10],
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": True,
        }
    else:
        params = {
            "max_depth": best_params[0],
            "num_leaves": best_params[1],
            "min_data_in_leaf": best_params[2],
            "feature_pre_filter": False,
            "lambda_l1": best_params[3],
            "lambda_l2": best_params[4],
            "metric": "multi_logloss",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "learning_rate": learning_rate,
            "feature_fraction": best_params[5],
            "bagging_fraction": best_params[6],
            "min_split_gain": best_params[7],
            "min_child_weight": best_params[8],
            "reg_alpha": best_params[9],
            "reg_lambda": best_params[10],
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": False,
        }

    evals_results = {}
    model = lgb.train(
        params,
        d_train,
        valid_sets=[d_train, d_valid],
        valid_names=["train", "valid"],
        evals_result=evals_results,
        num_boost_round=num_boost_round,
        early_stopping_rounds=early_stopping_rounds,
        verbose_eval=None,
        feval=None,
        init_model=mdl_cls_obj,
    )

    vld_preds_idxs = numpy.argmax(model.predict(vaild_np), axis=1)
    valid_acc_scr = accuracy_score(vaild_lbl_np, vld_preds_idxs)
    print("Validate Accuracy: {}".format(valid_acc_scr))
    print("Finish Training")

    model.save_model(out_mdl_file)

    test_preds_idxs = numpy.argmax(model.predict(test_np), axis=1)
    test_acc_scr = accuracy_score(test_lbl_np, test_preds_idxs)
    print("Testing Accuracy: {}".format(test_acc_scr))

    if out_info_file is not None:
        out_info = dict()
        out_info["params"] = params
        out_info["test_accuracy"] = test_acc_scr
        with open(out_info_file, "w") as outfile:
            import json

            json.dump(
                out_info,
                outfile,
                sort_keys=True,
                indent=4,
                separators=(",", ": "),
                ensure_ascii=False,
            )


def apply_lightgbm_multiclass_classifier(
    cls_train_info,
    model_file,
    in_img_mask,
    img_mask_val,
    img_file_info,
    out_class_img,
    gdalformat,
    class_clr_names=True,
):
    """
    This function applies a trained multiple classes lightgbm model. The function train_lightgbm_multiclass_classifier
    can be used to train such as model. The output image will be a final hard classification using the class with
    the maximum softmax score.

    :param cls_train_info: dict (where the key is the class name) of rsgislib.classification.ClassInfoObj
                           objects which will be used to train the classifier (i.e., train_lightgbm_multiclass_classifier()),
                           provide pixel value id and RGB class values.
    :param model_file: a trained lightgbm multiclass model which can be loaded with lgb.Booster(model_file=model_file).
    :param in_img_mask: is an image file providing a mask to specify where should be classified. Simplest mask is all the
                    valid data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the imgMask to limit the region to which the classification is applied.
                       Can be used to create a heirachical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                        rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) to identify which images and bands are to
                        be used for the classification so it adheres to the training data.
    :param out_class_img: Output image which will contain the hard classification defined as the maximum probability.
    :param gdalformat: is the output image format - all GDAL supported formats are supported.
    :param class_clr_names: default is True and therefore a colour table will the colours specified in ClassInfoObj
                          and a ClassName (from classTrainInfo) column will be added to the output file.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    def _applyLGMClassifier(info, inputs, outputs, otherargs):
        outClassVals = numpy.zeros(
            (otherargs.n_classes, inputs.imageMask.shape[1], inputs.imageMask.shape[2]),
            dtype=numpy.uint16,
        )
        outClassIdVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            n_pxls = inputs.imageMask.shape[1] * inputs.imageMask.shape[2]
            outClassVals = outClassVals.reshape((n_pxls, otherargs.n_classes))
            outClassIdVals = outClassIdVals.flatten()
            imgMaskVals = inputs.imageMask.flatten()
            classVars = numpy.zeros(
                (n_pxls, otherargs.numClassVars), dtype=numpy.float32
            )
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
            predClassProbs = numpy.around(
                otherargs.classifier.predict(classVars) * 10000
            )
            preds_idxs = numpy.argmax(predClassProbs, axis=1)
            if otherargs.n_classes != predClassProbs.shape[1]:
                raise rsgislib.RSGISPyException(
                    "The number of classes expected and the number provided by the classifier do not match."
                )
            outClassVals[ID] = predClassProbs
            preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
            for cld_id, idx in zip(
                otherargs.cls_id_lut, numpy.arange(0, len(otherargs.cls_id_lut))
            ):
                preds_cls_ids[preds_idxs == idx] = cld_id

            outClassIdVals[ID] = preds_cls_ids
            outClassIdVals = numpy.expand_dims(
                outClassIdVals.reshape(
                    (inputs.imageMask.shape[1], inputs.imageMask.shape[2])
                ),
                axis=0,
            )

        outputs.outclsimage = outClassIdVals

    classifier = lgb.Booster(model_file=model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = in_img_mask
    numClassVars = 0
    for imgFile in img_file_info:
        infiles.__dict__[imgFile.name] = imgFile.file_name
        numClassVars = numClassVars + len(imgFile.bands)

    n_classes = len(cls_train_info)
    cls_id_lut = numpy.zeros(n_classes)
    for clsname in cls_train_info:
        if cls_train_info[clsname].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    clsname, cls_train_info[clsname].id
                )
            )
        cls_id_lut[cls_train_info[clsname].id] = cls_train_info[clsname].out_id

    outfiles = applier.FilenameAssociations()
    outfiles.outclsimage = out_class_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = img_mask_val
    otherargs.numClassVars = numClassVars
    otherargs.imgFileInfo = img_file_info
    otherargs.n_classes = n_classes
    otherargs.cls_id_lut = cls_id_lut

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
    applier.apply(_applyLGMClassifier, infiles, outfiles, otherargs, controls=aControls)
    print("Completed Classification")

    if class_clr_names:
        rsgislib.rastergis.pop_rat_img_stats(
            out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
        ratDataset = gdal.Open(out_class_img, gdal.GA_Update)
        red = rat.readColumn(ratDataset, "Red")
        green = rat.readColumn(ratDataset, "Green")
        blue = rat.readColumn(ratDataset, "Blue")
        ClassName = numpy.empty_like(red, dtype=numpy.dtype("a255"))
        ClassName[...] = ""

        for classKey in cls_train_info:
            print("Apply Colour to class '" + classKey + "'")
            red[cls_train_info[classKey].out_id] = cls_train_info[classKey].red
            green[cls_train_info[classKey].out_id] = cls_train_info[classKey].green
            blue[cls_train_info[classKey].out_id] = cls_train_info[classKey].blue
            ClassName[cls_train_info[classKey].out_id] = classKey

        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)
        rat.writeColumn(ratDataset, "ClassName", ClassName)
        ratDataset = None
