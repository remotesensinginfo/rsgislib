#! /usr/bin/env python
############################################################################
#  classxgboost.py
#
#  Copyright 2020 RSGISLib.
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
# Date: 16/02/2020
# Version: 1.1
#
# History:
# Version 1.0 - Created.
# Version 1.1 - Changed parameter optimisation libraries.
#
###########################################################################

import gc
from typing import Dict, List, Union

import h5py
import numpy
from osgeo import gdal
from rios import applier, cuiprogress, rat

import rsgislib
import rsgislib.classification
import rsgislib.imagecalc
import rsgislib.imageutils
import rsgislib.rastergis
import rsgislib.tools.utils

HAVE_XGBOOST = True
try:
    import xgboost as xgb
except ImportError:
    HAVE_XGBOOST = False

from sklearn.metrics import accuracy_score, roc_auc_score


def optimise_xgboost_binary_classifier(
    out_params_file: str,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    mdl_cls_obj=None,
):
    """
    A function which performs a hyper-parameter optimisation for a binary
    xgboost classifier. Class 1 is the class which you are interested in
    and Class 2 is the 'other class'.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_params_file: The output JSON file with the identified parameters
    :param cls1_train_file: File path to the HDF5 file with the training samples
                            for class 1
    :param cls1_valid_file: File path to the HDF5 file with the validation samples
                            for class 1
    :param cls2_train_file: File Path to the HDF5 file with the training samples
                            for class 2
    :param cls2_valid_file: File path to the HDF5 file with the validation samples
                            for class 2
    :param op_mthd: The method used to optimise the parameters.
                    Default: rsgislib.OPT_MTHD_BAYESOPT
    :param n_opt_iters: The number of iterations (Default 100) used for the
                        optimisation. This parameter is ignored for skopt.
                        For bayes_opt there is a minimum of 10 and these are
                        added to that minimum so Default is therefore 110.
                        For optuna this is the number of iterations used.
    :param rnd_seed: A random seed for the optimisation. Default None. If None
                     there a different seed will be used each time the function
                     is run.
    :param n_threads: The number of threads used by xgboost
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """

    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    print("Reading Class 1 Training")
    f_h5 = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f_h5["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=numpy.dtype(int))

    print("Reading Class 1 Validation")
    f_h5 = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f_h5["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Training")
    f_h5 = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f_h5["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Validation")
    f_h5 = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f_h5["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=numpy.dtype(int))

    print("Finished Reading Data")

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(
        numpy.concatenate((train_cls2, train_cls1)),
        label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)),
    )

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _xgb_cls_bo_func(
            eta,
            gamma,
            max_depth,
            min_child_weight,
            max_delta_step,
            subsample,
            num_boost_round,
        ):
            params = {
                "eta": float(eta),
                "gamma": int(gamma),
                "max_depth": int(max_depth),
                "min_child_weight": int(min_child_weight),
                "max_delta_step": int(max_delta_step),
                "subsample": float(subsample),
                "nthread": n_threads,
                "eval_metric": "auc",
                "objective": "binary:logistic",
            }
            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                int(num_boost_round),
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            acc_score = roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
            gc.collect()
            return acc_score

        hyperparam_space = {
            "eta": (0.01, 0.9),
            "gamma": (0, 100),
            "max_depth": (2, 20),
            "min_child_weight": (1, 10),
            "max_delta_step": (0, 10),
            "subsample": (0.5, 1),
            "num_boost_round": (2, 100),
        }

        bo_opt_obj = BayesianOptimization(
            f=_xgb_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max
        params = {
            "eta": float(op_params["params"]["eta"]),
            "gamma": int(op_params["params"]["gamma"]),
            "max_depth": int(op_params["params"]["max_depth"]),
            "min_child_weight": int(op_params["params"]["min_child_weight"]),
            "max_delta_step": int(op_params["params"]["max_delta_step"]),
            "subsample": float(op_params["params"]["subsample"]),
            "nthread": n_threads,
            "eval_metric": "auc",
            "objective": "binary:logistic",
            "num_boost_round": int(op_params["params"]["num_boost_round"]),
        }

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _xgb_cls_optuna_func(trial):
            params = {
                "eta": trial.suggest_float("eta", 0.01, 0.9),
                "gamma": trial.suggest_int("gamma", 0, 100),
                "max_depth": trial.suggest_int("max_depth", 2, 20),
                "min_child_weight": trial.suggest_int("min_child_weight", 1, 10),
                "max_delta_step": trial.suggest_int("max_delta_step", 0, 10),
                "subsample": trial.suggest_float("subsample", 0.5, 1),
                "nthread": n_threads,
                "eval_metric": "auc",
                "objective": "binary:logistic",
            }
            num_boost_round_trial = trial.suggest_int("num_boost_round", 2, 100)

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round_trial,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            acc_score = roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(_xgb_cls_optuna_func, n_trials=n_opt_iters, timeout=600)

        optuna_opt_trial = optuna_opt_obj.best_trial
        params = {
            "eta": float(optuna_opt_trial.params["eta"]),
            "gamma": int(optuna_opt_trial.params["gamma"]),
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "min_child_weight": int(optuna_opt_trial.params["min_child_weight"]),
            "max_delta_step": int(optuna_opt_trial.params["max_delta_step"]),
            "subsample": float(optuna_opt_trial.params["subsample"]),
            "nthread": n_threads,
            "eval_metric": "auc",
            "objective": "binary:logistic",
            "num_boost_round": int(optuna_opt_trial.params["num_boost_round"]),
        }

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Real(0.01, 0.9, name="eta"),
            skopt.space.Integer(0, 100, name="gamma"),
            skopt.space.Integer(2, 20, name="max_depth"),
            skopt.space.Integer(1, 10, name="min_child_weight"),
            skopt.space.Integer(0, 10, name="max_delta_step"),
            skopt.space.Real(0.5, 1, name="subsample"),
            skopt.space.Integer(2, 100, name="num_boost_round"),
        ]

        def _xgb_cls_skop_func(values):
            params = {
                "eta": values[0],
                "gamma": values[1],
                "max_depth": values[2],
                "min_child_weight": values[3],
                "max_delta_step": values[4],
                "subsample": values[5],
                "nthread": n_threads,
                "eval_metric": "auc",
                "objective": "binary:logistic",
            }

            print("\nNext set of params.....", params)

            num_boost_round = values[6]
            print("num_boost_round = {}.".format(num_boost_round))

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            acc_score = -roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _xgb_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

        print("Start Training Find Classifier")
        params = {
            "eta": float(best_params[0]),
            "gamma": int(best_params[1]),
            "max_depth": int(best_params[2]),
            "min_child_weight": int(best_params[3]),
            "max_delta_step": int(best_params[4]),
            "subsample": float(best_params[5]),
            "nthread": n_threads,
            "eval_metric": "auc",
            "objective": "binary:logistic",
            "num_boost_round": int(best_params[6]),
        }
    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    rsgislib.tools.utils.write_dict_to_json(params, out_params_file)


def train_xgboost_binary_classifier(
    out_mdl_file: str,
    cls_params_file: str,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls1_test_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    cls2_test_file: str,
    n_threads: int = 1,
    mdl_cls_obj=None,
):
    """
A function which trains a binary lightgbm model using the parameters provided
    within a JSON file. The JSON file must provide values for the following
    parameters:

       * eta
       * gamma
       * max_depth
       * min_child_weight
       * max_delta_step
       * subsample
       * bagging_fraction
       * eval_metric
       * objective

    :param out_mdl_file: The file path for the output xgboost (*.h5) model which
                         can be loaded to perform a classification.
    :param cls_params_file: The file path to the JSON file with the classifier
                            parameters.
    :param cls1_train_file: File path to the HDF5 file with the training samples
                            for class 1
    :param cls1_valid_file: File path to the HDF5 file with the validation samples
                            for class 1
    :param cls1_test_file: File path to the HDF5 file with the testing samples
                           for class 1
    :param cls2_train_file: File path to the HDF5 file with the training samples
                            for class 2
    :param cls2_valid_file: File path to the HDF5 file with the validation samples
                            for class 2
    :param cls2_test_file: File path to the HDF5 file with the testing samples
                           for class 2
    :param n_threads: The number of threads used by lightgbm
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    print("Reading Class 1 Training")
    f_h5 = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f_h5["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=numpy.dtype(int))

    print("Reading Class 1 Validation")
    f_h5 = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f_h5["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=numpy.dtype(int))

    print("Reading Class 1 Testing")
    f_h5 = h5py.File(cls1_test_file, "r")
    num_cls1_test_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f_h5["DATA/DATA"])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Training")
    f_h5 = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f_h5["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Validation")
    f_h5 = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f_h5["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Testing")
    f_h5 = h5py.File(cls2_test_file, "r")
    num_cls2_test_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f_h5["DATA/DATA"])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=numpy.dtype(int))

    print("Finished Reading Data")

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(
        numpy.concatenate((train_cls2, train_cls1)),
        label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)),
    )

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))
    d_test = xgb.DMatrix(test_np, label=test_lbl_np)

    cls_params = rsgislib.tools.utils.read_json_to_dict(cls_params_file)

    print("Start Training Find Classifier")

    params = {
        "eta": cls_params["eta"],
        "gamma": cls_params["gamma"],
        "max_depth": cls_params["max_depth"],
        "min_child_weight": cls_params["min_child_weight"],
        "max_delta_step": cls_params["max_delta_step"],
        "subsample": cls_params["subsample"],
        "nthread": n_threads,
        "eval_metric": cls_params["eval_metric"],
        "objective": cls_params["objective"],
    }

    num_boost_round = cls_params["num_boost_round"]

    evals_results = {}
    watchlist = [(d_train, "train"), (d_valid, "validation")]
    model = xgb.train(
        params,
        d_train,
        num_boost_round,
        evals=watchlist,
        evals_result=evals_results,
        verbose_eval=False,
        xgb_model=mdl_cls_obj,
    )
    test_auc = roc_auc_score(test_lbl_np, model.predict(d_test))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model.save_model(out_mdl_file)

    pred_test = model.predict(d_test)
    for i in range(test_np.shape[0]):
        if pred_test[i] >= 0.5:
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def train_opt_xgboost_binary_classifier(
    out_mdl_file: str,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls1_test_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    cls2_test_file: str,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    mdl_cls_obj=None,
    out_params_file: str = None,
):
    """
    A function which performs a hyper-parameter optimisation for a binary
    xgboost classifier and then trains a model saving the model for future
    use. Class 1 is the class which you are interested in and Class 2 is
    the 'other class'.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_mdl_file: The file path for the output xgboost (*.h5) model which
                         can be loaded to perform a classification.
    :param cls1_train_file: File path to the HDF5 file with the training samples
                            for class 1
    :param cls1_valid_file: File path to the HDF5 file with the validation samples
                            for class 1
    :param cls1_test_file: File path to the HDF5 file with the testing samples
                           for class 1
    :param cls2_train_file: File path to the HDF5 file with the training samples
                            for class 2
    :param cls2_valid_file: File path to the HDF5 file with the validation samples
                            for class 2
    :param cls2_test_file: File path to the HDF5 file with the testing samples
                           for class 2
    :param op_mthd: The method used to optimise the parameters.
                    Default: rsgislib.OPT_MTHD_BAYESOPT
    :param n_opt_iters: The number of iterations (Default 100) used for the
                        optimisation. This parameter is ignored for skopt.
                        For bayes_opt there is a minimum of 10 and these are
                        added to that minimum so Default is therefore 110.
                        For optuna this is the number of iterations used.
    :param rnd_seed: A random seed for the optimisation. Default None. If None
                     there a different seed will be used each time the function
                     is run.
    :param n_threads: The number of threads used by xgboost
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).
    :param out_params_file: The output JSON file with the identified parameters.
                            If None (default) then no file is outputted.

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    print("Reading Class 1 Training")
    f_h5 = h5py.File(cls1_train_file, "r")
    num_cls1_train_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_train_rows = {}".format(num_cls1_train_rows))
    train_cls1 = numpy.array(f_h5["DATA/DATA"])
    train_cls1_lbl = numpy.ones(num_cls1_train_rows, dtype=numpy.dtype(int))

    print("Reading Class 1 Validation")
    f_h5 = h5py.File(cls1_valid_file, "r")
    num_cls1_valid_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_valid_rows = {}".format(num_cls1_valid_rows))
    valid_cls1 = numpy.array(f_h5["DATA/DATA"])
    valid_cls1_lbl = numpy.ones(num_cls1_valid_rows, dtype=numpy.dtype(int))

    print("Reading Class 1 Testing")
    f_h5 = h5py.File(cls1_test_file, "r")
    num_cls1_test_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls1_test_rows = {}".format(num_cls1_test_rows))
    test_cls1 = numpy.array(f_h5["DATA/DATA"])
    test_cls1_lbl = numpy.ones(num_cls1_test_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Training")
    f_h5 = h5py.File(cls2_train_file, "r")
    num_cls2_train_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_train_rows = {}".format(num_cls2_train_rows))
    train_cls2 = numpy.array(f_h5["DATA/DATA"])
    train_cls2_lbl = numpy.zeros(num_cls2_train_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Validation")
    f_h5 = h5py.File(cls2_valid_file, "r")
    num_cls2_valid_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_valid_rows = {}".format(num_cls2_valid_rows))
    valid_cls2 = numpy.array(f_h5["DATA/DATA"])
    valid_cls2_lbl = numpy.zeros(num_cls2_valid_rows, dtype=numpy.dtype(int))

    print("Reading Class 2 Testing")
    f_h5 = h5py.File(cls2_test_file, "r")
    num_cls2_test_rows = f_h5["DATA/DATA"].shape[0]
    print("num_cls2_test_rows = {}".format(num_cls2_test_rows))
    test_cls2 = numpy.array(f_h5["DATA/DATA"])
    test_cls2_lbl = numpy.zeros(num_cls2_test_rows, dtype=numpy.dtype(int))

    print("Finished Reading Data")

    vaild_np = numpy.concatenate((valid_cls2, valid_cls1))
    vaild_lbl_np = numpy.concatenate((valid_cls2_lbl, valid_cls1_lbl))
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(
        numpy.concatenate((train_cls2, train_cls1)),
        label=numpy.concatenate((train_cls2_lbl, train_cls1_lbl)),
    )

    test_np = numpy.concatenate((test_cls2, test_cls1))
    test_lbl_np = numpy.concatenate((test_cls2_lbl, test_cls1_lbl))
    d_test = xgb.DMatrix(test_np, label=test_lbl_np)

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _xgb_cls_bo_func(
            eta,
            gamma,
            max_depth,
            min_child_weight,
            max_delta_step,
            subsample,
            num_boost_round,
        ):
            params = {
                "eta": float(eta),
                "gamma": int(gamma),
                "max_depth": int(max_depth),
                "min_child_weight": int(min_child_weight),
                "max_delta_step": int(max_delta_step),
                "subsample": float(subsample),
                "nthread": n_threads,
                "eval_metric": "auc",
                "objective": "binary:logistic",
            }
            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                int(num_boost_round),
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            acc_score = roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
            gc.collect()
            return acc_score

        hyperparam_space = {
            "eta": (0.01, 0.9),
            "gamma": (0, 100),
            "max_depth": (2, 20),
            "min_child_weight": (1, 10),
            "max_delta_step": (0, 10),
            "subsample": (0.5, 1),
            "num_boost_round": (2, 100),
        }

        bo_opt_obj = BayesianOptimization(
            f=_xgb_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max
        params = {
            "eta": float(op_params["params"]["eta"]),
            "gamma": int(op_params["params"]["gamma"]),
            "max_depth": int(op_params["params"]["max_depth"]),
            "min_child_weight": int(op_params["params"]["min_child_weight"]),
            "max_delta_step": int(op_params["params"]["max_delta_step"]),
            "subsample": float(op_params["params"]["subsample"]),
            "nthread": n_threads,
            "eval_metric": "auc",
            "objective": "binary:logistic",
        }
        num_boost_round = int(op_params["params"]["num_boost_round"])

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _xgb_cls_optuna_func(trial):
            params = {
                "eta": trial.suggest_float("eta", 0.01, 0.9),
                "gamma": trial.suggest_int("gamma", 0, 100),
                "max_depth": trial.suggest_int("max_depth", 2, 20),
                "min_child_weight": trial.suggest_int("min_child_weight", 1, 10),
                "max_delta_step": trial.suggest_int("max_delta_step", 0, 10),
                "subsample": trial.suggest_float("subsample", 0.5, 1),
                "nthread": n_threads,
                "eval_metric": "auc",
                "objective": "binary:logistic",
            }
            num_boost_round_trial = trial.suggest_int("num_boost_round", 2, 100)

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round_trial,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            acc_score = roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(_xgb_cls_optuna_func, n_trials=n_opt_iters, timeout=600)

        optuna_opt_trial = optuna_opt_obj.best_trial
        params = {
            "eta": float(optuna_opt_trial.params["eta"]),
            "gamma": int(optuna_opt_trial.params["gamma"]),
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "min_child_weight": int(optuna_opt_trial.params["min_child_weight"]),
            "max_delta_step": int(optuna_opt_trial.params["max_delta_step"]),
            "subsample": float(optuna_opt_trial.params["subsample"]),
            "nthread": n_threads,
            "eval_metric": "auc",
            "objective": "binary:logistic",
        }
        num_boost_round = int(optuna_opt_trial.params["num_boost_round"])

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Real(0.01, 0.9, name="eta"),
            skopt.space.Integer(0, 100, name="gamma"),
            skopt.space.Integer(2, 20, name="max_depth"),
            skopt.space.Integer(1, 10, name="min_child_weight"),
            skopt.space.Integer(0, 10, name="max_delta_step"),
            skopt.space.Real(0.5, 1, name="subsample"),
            skopt.space.Integer(2, 100, name="num_boost_round"),
        ]

        def _xgb_cls_skop_func(values):
            params = {
                "eta": values[0],
                "gamma": values[1],
                "max_depth": values[2],
                "min_child_weight": values[3],
                "max_delta_step": values[4],
                "subsample": values[5],
                "nthread": n_threads,
                "eval_metric": "auc",
                "objective": "binary:logistic",
            }

            print("\nNext set of params.....", params)

            num_boost_round = values[6]
            print("num_boost_round = {}.".format(num_boost_round))

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            acc_score = -roc_auc_score(vaild_lbl_np, model_xgb.predict(d_valid))
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _xgb_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

        print("Start Training Find Classifier")
        params = {
            "eta": best_params[0],
            "gamma": best_params[1],
            "max_depth": best_params[2],
            "min_child_weight": best_params[3],
            "max_delta_step": best_params[4],
            "subsample": best_params[5],
            "nthread": n_threads,
            "eval_metric": "auc",
            "objective": "binary:logistic",
        }

        num_boost_round = best_params[6]
    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    if out_params_file is not None:
        rsgislib.tools.utils.write_dict_to_json(params, out_params_file)

    print("Start Training Find Classifier")

    evals_results = {}
    watch_list = [(d_train, "train"), (d_valid, "validation")]
    model = xgb.train(
        params,
        d_train,
        num_boost_round,
        evals=watch_list,
        evals_result=evals_results,
        verbose_eval=False,
        xgb_model=mdl_cls_obj,
    )
    test_auc = roc_auc_score(test_lbl_np, model.predict(d_test))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model.save_model(out_mdl_file)

    pred_test = model.predict(d_test)
    for i in range(test_np.shape[0]):
        if pred_test[i] >= 0.5:
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def apply_xgboost_binary_classifier(
    model_file: str,
    in_msk_img: str,
    img_mask_val: int,
    img_file_info: List,
    out_score_img: str,
    gdalformat: str = "KEA",
    out_class_img=None,
    class_thres: int = 5000,
    n_threads: int = 1,
):
    """
    A function for applying a trained binary xgboost model to a image or stack of
    image files.

    :param model_file: a trained lightgbm binary model which can be loaded
                       with the xgb.Booster function load_model(model_file).
    :param in_img_msk: is an image file providing a mask to specify where
                       should be classified. Simplest mask is all the valid
                       data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the imgMask to limit the region
                         to which the classification is applied.
                         Can be used to create a hierarchical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects
                          to identify which images and bands are to be used for
                          the classification so it adheres to the training data.
    :param out_score_img: output image file with the classification softmax score.
                          Note. this image is scaled by multiplying by 10000
                          therefore the range is between 0-10000.
    :param gdalformat: The output image format (Default: KEA).
    :param out_class_img: Optional output image which will contain the hard
                          classification, defined with a threshold on the
                          softmax score image.
    :param class_thres: The threshold used to define the hard classification.
                        Default is 5000 (i.e., softmax score of 0.5).
    :param n_threads: The number of threads used by xgboost

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    def _apply_xgb_classifier(info, inputs, outputs, otherargs):
        out_class_vals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            out_class_vals = out_class_vals.flatten()
            img_mask_vals = inputs.imageMask.flatten()
            class_vars = numpy.zeros(
                (out_class_vals.shape[0], otherargs.numClassVars), dtype=numpy.float32
            )
            # Array index which can be used to populate the output array following masking etc.
            id_arr = numpy.arange(img_mask_vals.shape[0])
            class_vars_idx = 0
            for img_file in otherargs.imgFileInfo:
                img_arr = inputs.__dict__[img_file.name]
                for band in img_file.bands:
                    class_vars[..., class_vars_idx] = img_arr[(band - 1)].flatten()
                    class_vars_idx = class_vars_idx + 1
            class_vars = class_vars[img_mask_vals == otherargs.mskVal]
            id_arr = id_arr[img_mask_vals == otherargs.mskVal]
            pred_class = numpy.around(
                otherargs.classifier.predict(xgb.DMatrix(class_vars)) * 10000
            )
            out_class_vals[id_arr] = pred_class
            out_class_vals = numpy.expand_dims(
                out_class_vals.reshape(
                    (inputs.imageMask.shape[1], inputs.imageMask.shape[2])
                ),
                axis=0,
            )
        outputs.outimage = out_class_vals

    classifier = xgb.Booster({"nthread": n_threads})
    classifier.load_model(model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = in_msk_img
    num_class_vars = 0
    for imgFile in img_file_info:
        infiles.__dict__[imgFile.name] = imgFile.file_name
        num_class_vars = num_class_vars + len(imgFile.bands)

    outfiles = applier.FilenameAssociations()
    outfiles.outimage = out_score_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = img_mask_val
    otherargs.numClassVars = num_class_vars
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
        _apply_xgb_classifier, infiles, outfiles, otherargs, controls=aControls
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
        if gdalformat == "KEA":
            rsgislib.rastergis.pop_rat_img_stats(
                out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
            )


def optimise_xgboost_multiclass_classifier(
    out_params_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    sub_train_smpls: Union[int, float] = None,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    mdl_cls_obj=None,
):
    """
    A function which performs a hyper-parameter optimisation for a multi-class
    xgboost classifier.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_params_file: The output JSON file with the identified parameters
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
    :param sub_train_smpls: Subset the training, if None or 0 then no sub-setting
                            will occur. If between 0-1 then a ratio subset
                            (e.g., 0.25 = 25 % subset) will be taken. If > 1 then
                            that number of points will be taken per class.
    :param op_mthd: The method used to optimise the parameters.
                    Default: rsgislib.OPT_MTHD_BAYESOPT
    :param n_opt_iters: The number of iterations (Default 100) used for the
                        optimisation. This parameter is ignored for skopt.
                        For bayes_opt there is a minimum of 10 and these are
                        added to that minimum so Default is therefore 110.
                        For optuna this is the number of iterations used.
    :param rnd_seed: A random seed for the optimisation. Default None. If None
                     there a different seed will be used each time the function
                     is run.
    :param n_threads: The number of threads used by xgboost
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    rnd_obj = numpy.random.RandomState(rnd_seed)

    n_classes = len(cls_info_dict)
    for cls_name in cls_info_dict:
        if cls_info_dict[cls_name].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive "
                "starting from 0.".format(cls_name, cls_info_dict[cls_name].id)
            )

    cls_data_dict = {}
    train_data_lst = []
    train_lbls_lst = []
    valid_data_lst = []
    valid_lbls_lst = []
    cls_ids = []
    n_classes = 0
    for cls_name in cls_info_dict:
        sgl_cls_info = {}
        print("Reading Class {} Training".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].train_file_h5, "r")
        sgl_cls_info["train_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["train_data"] = numpy.array(f_h5["DATA/DATA"])

        if (sub_train_smpls is not None) and (sub_train_smpls > 0):
            if sub_train_smpls < 1:
                sub_n_rows = int(sgl_cls_info["train_n_rows"] * sub_train_smpls)
            else:
                sub_n_rows = sub_train_smpls
            print("sub_n_rows = {}".format(sub_n_rows))
            if sub_n_rows > 0:
                sub_sel_rows = rnd_obj.choice(sgl_cls_info["train_n_rows"], sub_n_rows)
                sgl_cls_info["train_data"] = sgl_cls_info["train_data"][sub_sel_rows]
                sgl_cls_info["train_n_rows"] = sub_n_rows

        sgl_cls_info["train_data_lbls"] = numpy.zeros(
            sgl_cls_info["train_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["train_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()

        train_data_lst.append(sgl_cls_info["train_data"])
        train_lbls_lst.append(sgl_cls_info["train_data_lbls"])

        print("Reading Class {} Validation".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].valid_file_h5, "r")
        sgl_cls_info["valid_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["valid_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["valid_data_lbls"] = numpy.zeros(
            sgl_cls_info["valid_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["valid_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        valid_data_lst.append(sgl_cls_info["valid_data"])
        valid_lbls_lst.append(sgl_cls_info["valid_data_lbls"])

        cls_data_dict[cls_name] = sgl_cls_info
        cls_ids.append(cls_info_dict[cls_name].id)
        n_classes = n_classes + 1

    print("Finished Reading Data")

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbl_np = numpy.concatenate(valid_lbls_lst)
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(
        numpy.concatenate(train_data_lst), label=numpy.concatenate(train_lbls_lst)
    )

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _xgb_cls_bo_func(
            eta,
            gamma,
            max_depth,
            min_child_weight,
            max_delta_step,
            subsample,
            num_boost_round,
        ):
            params = {
                "eta": float(eta),
                "gamma": int(gamma),
                "max_depth": int(max_depth),
                "min_child_weight": int(min_child_weight),
                "max_delta_step": int(max_delta_step),
                "subsample": float(subsample),
                "nthread": n_threads,
                "eval_metric": "merror",
                "objective": "multi:softmax",
                "num_class": n_classes,
            }
            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                int(num_boost_round),
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            vld_preds_idxs = model_xgb.predict(d_valid)

            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        hyperparam_space = {
            "eta": (0.01, 0.9),
            "gamma": (0, 100),
            "max_depth": (2, 20),
            "min_child_weight": (1, 10),
            "max_delta_step": (0, 10),
            "subsample": (0.5, 1),
            "num_boost_round": (2, 100),
        }

        bo_opt_obj = BayesianOptimization(
            f=_xgb_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max
        params = {
            "eta": float(op_params["params"]["eta"]),
            "gamma": int(op_params["params"]["gamma"]),
            "max_depth": int(op_params["params"]["max_depth"]),
            "min_child_weight": int(op_params["params"]["min_child_weight"]),
            "max_delta_step": int(op_params["params"]["max_delta_step"]),
            "subsample": float(op_params["params"]["subsample"]),
            "nthread": n_threads,
            "eval_metric": "merror",
            "objective": "multi:softmax",
            "num_class": n_classes,
            "num_boost_round": int(op_params["params"]["num_boost_round"]),
        }
    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _xgb_cls_optuna_func(trial):
            params = {
                "eta": trial.suggest_float("eta", 0.01, 0.9),
                "gamma": trial.suggest_int("gamma", 0, 100),
                "max_depth": trial.suggest_int("max_depth", 2, 20),
                "min_child_weight": trial.suggest_int("min_child_weight", 1, 10),
                "max_delta_step": trial.suggest_int("max_delta_step", 0, 10),
                "subsample": trial.suggest_float("subsample", 0.5, 1),
                "nthread": n_threads,
                "eval_metric": "merror",
                "objective": "multi:softmax",
                "num_class": n_classes,
            }
            num_boost_round_trial = trial.suggest_int("num_boost_round", 2, 100)

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round_trial,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            vld_preds_idxs = model_xgb.predict(d_valid)

            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(_xgb_cls_optuna_func, n_trials=n_opt_iters, timeout=600)

        optuna_opt_trial = optuna_opt_obj.best_trial
        params = {
            "eta": float(optuna_opt_trial.params["eta"]),
            "gamma": int(optuna_opt_trial.params["gamma"]),
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "min_child_weight": int(optuna_opt_trial.params["min_child_weight"]),
            "max_delta_step": int(optuna_opt_trial.params["max_delta_step"]),
            "subsample": float(optuna_opt_trial.params["subsample"]),
            "nthread": n_threads,
            "eval_metric": "merror",
            "objective": "multi:softmax",
            "num_class": n_classes,
            "num_boost_round": int(optuna_opt_trial.params["num_boost_round"]),
        }
    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Real(0.01, 0.9, name="eta"),
            skopt.space.Integer(0, 100, name="gamma"),
            skopt.space.Integer(2, 20, name="max_depth"),
            skopt.space.Integer(1, 10, name="min_child_weight"),
            skopt.space.Integer(0, 10, name="max_delta_step"),
            skopt.space.Real(0.5, 1, name="subsample"),
            skopt.space.Integer(2, 100, name="num_boost_round"),
        ]

        def _xgb_cls_skop_func(values):
            params = {
                "eta": values[0],
                "gamma": values[1],
                "max_depth": values[2],
                "min_child_weight": values[3],
                "max_delta_step": values[4],
                "subsample": values[5],
                "nthread": n_threads,
                "eval_metric": "merror",
                "objective": "multi:softmax",
                "num_class": n_classes,
            }

            print("\nNext set of params.....", params)

            num_boost_round = values[6]
            print("num_boost_round = {}.".format(num_boost_round))

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            vld_preds_idxs = model_xgb.predict(d_valid)

            acc_score = -accuracy_score(vaild_lbl_np, vld_preds_idxs)
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _xgb_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

        print("Start Training Find Classifier")
        params = {
            "eta": best_params[0],
            "gamma": best_params[1],
            "max_depth": best_params[2],
            "min_child_weight": best_params[3],
            "max_delta_step": best_params[4],
            "subsample": best_params[5],
            "nthread": n_threads,
            "eval_metric": "merror",
            "objective": "multi:softmax",
            "num_class": n_classes,
            "num_boost_round": best_params[6],
        }
    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    rsgislib.tools.utils.write_dict_to_json(params, out_params_file)


def train_xgboost_multiclass_classifier(
    out_mdl_file: str,
    cls_params_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    n_threads: int = 1,
    mdl_cls_obj=None,
):
    """
    A function which trains a multiclass xgboost model using the parameters
    provided within a JSON file. The JSON file must provide values for the
    following parameters:

       * eta
       * gamma
       * max_depth
       * min_child_weight
       * max_delta_step
       * subsample
       * bagging_fraction
       * eval_metric
       * objective

    :param params_file: The file path to the JSON file with the classifier
                        parameters.
    :param out_mdl_file: The file path for the output xgboost (*.h5) model which
                         can be loaded to perform a classification.
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
    :param n_threads: The number of threads used by xgboost
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    n_classes = len(cls_info_dict)
    for cls_name in cls_info_dict:
        if cls_info_dict[cls_name].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive "
                "starting from 0.".format(cls_name, cls_info_dict[cls_name].id)
            )

    cls_data_dict = {}
    train_data_lst = []
    train_lbls_lst = []
    valid_data_lst = []
    valid_lbls_lst = []
    test_data_lst = []
    test_lbls_lst = []
    cls_ids = []
    n_classes = 0
    for cls_name in cls_info_dict:
        sgl_cls_info = {}
        print("Reading Class {} Training".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].train_file_h5, "r")
        sgl_cls_info["train_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["train_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["train_data_lbls"] = numpy.zeros(
            sgl_cls_info["train_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["train_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        train_data_lst.append(sgl_cls_info["train_data"])
        train_lbls_lst.append(sgl_cls_info["train_data_lbls"])

        print("Reading Class {} Validation".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].valid_file_h5, "r")
        sgl_cls_info["valid_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["valid_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["valid_data_lbls"] = numpy.zeros(
            sgl_cls_info["valid_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["valid_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        valid_data_lst.append(sgl_cls_info["valid_data"])
        valid_lbls_lst.append(sgl_cls_info["valid_data_lbls"])

        print("Reading Class {} Testing".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].test_file_h5, "r")
        sgl_cls_info["test_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["test_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["test_data_lbls"] = numpy.zeros(
            sgl_cls_info["test_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["test_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        test_data_lst.append(sgl_cls_info["test_data"])
        test_lbls_lst.append(sgl_cls_info["test_data_lbls"])

        cls_data_dict[cls_name] = sgl_cls_info
        cls_ids.append(cls_info_dict[cls_name].id)
        n_classes = n_classes + 1

    print("Finished Reading Data")

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbl_np = numpy.concatenate(valid_lbls_lst)
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(
        numpy.concatenate(train_data_lst), label=numpy.concatenate(train_lbls_lst)
    )

    test_np = numpy.concatenate(test_data_lst)
    test_lbl_np = numpy.concatenate(test_lbls_lst)
    d_test = xgb.DMatrix(test_np, label=test_lbl_np)

    cls_params = rsgislib.tools.utils.read_json_to_dict(cls_params_file)

    if n_classes != cls_params["num_class"]:
        raise rsgislib.RSGISPyException(
            "The number of classes used to optimise the "
            "classifier and the number for training are different!"
        )

    print("Start Training Find Classifier")

    params = {
        "eta": cls_params["eta"],
        "gamma": cls_params["gamma"],
        "max_depth": cls_params["max_depth"],
        "min_child_weight": cls_params["min_child_weight"],
        "max_delta_step": cls_params["max_delta_step"],
        "subsample": cls_params["subsample"],
        "nthread": n_threads,
        "eval_metric": cls_params["eval_metric"],
        "objective": cls_params["objective"],
        "num_class": n_classes,
    }

    num_boost_round = cls_params["num_boost_round"]

    watchlist = [(d_train, "train"), (d_valid, "validation")]
    evals_results = {}
    model_xgb = xgb.train(
        params,
        d_train,
        num_boost_round,
        evals=watchlist,
        evals_result=evals_results,
        verbose_eval=False,
        xgb_model=mdl_cls_obj,
    )
    model_xgb.save_model(out_mdl_file)

    vld_preds_idxs = model_xgb.predict(d_valid)
    valid_acc_scr = accuracy_score(vaild_lbl_np, vld_preds_idxs)
    print("Validate Accuracy: {}".format(valid_acc_scr))

    test_preds_idxs = model_xgb.predict(d_test)
    test_acc_scr = accuracy_score(test_lbl_np, test_preds_idxs)
    print("Testing Accuracy: {}".format(test_acc_scr))


def train_opt_xgboost_multiclass_classifier(
    out_mdl_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    mdl_cls_obj=None,
):
    """
    A function which performs a hyper-parameter optimisation for a multi-class
    xgboost classifier and then trains a model saving the model for future
    use.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_mdl_file: The file path for the output xgboost (*.h5) model which
                         can be loaded to perform a classification.
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
    :param op_mthd: The method used to optimise the parameters.
                    Default: rsgislib.OPT_MTHD_BAYESOPT
    :param n_opt_iters: The number of iterations (Default 100) used for the
                        optimisation. This parameter is ignored for skopt.
                        For bayes_opt there is a minimum of 10 and these are
                        added to that minimum so Default is therefore 110.
                        For optuna this is the number of iterations used.
    :param rnd_seed: A random seed for the optimisation. Default None. If None
                     there a different seed will be used each time the function
                     is run.
    :param n_threads: The number of threads used by xgboost
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    n_classes = len(cls_info_dict)
    for cls_name in cls_info_dict:
        if cls_info_dict[cls_name].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    cls_name, cls_info_dict[cls_name].id
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
    n_classes = 0
    for cls_name in cls_info_dict:
        sgl_cls_info = {}
        print("Reading Class {} Training".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].train_file_h5, "r")
        sgl_cls_info["train_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["train_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["train_data_lbls"] = numpy.zeros(
            sgl_cls_info["train_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["train_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        train_data_lst.append(sgl_cls_info["train_data"])
        train_lbls_lst.append(sgl_cls_info["train_data_lbls"])

        print("Reading Class {} Validation".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].valid_file_h5, "r")
        sgl_cls_info["valid_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["valid_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["valid_data_lbls"] = numpy.zeros(
            sgl_cls_info["valid_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["valid_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        valid_data_lst.append(sgl_cls_info["valid_data"])
        valid_lbls_lst.append(sgl_cls_info["valid_data_lbls"])

        print("Reading Class {} Testing".format(cls_name))
        f_h5 = h5py.File(cls_info_dict[cls_name].test_file_h5, "r")
        sgl_cls_info["test_n_rows"] = f_h5["DATA/DATA"].shape[0]
        sgl_cls_info["test_data"] = numpy.array(f_h5["DATA/DATA"])
        sgl_cls_info["test_data_lbls"] = numpy.zeros(
            sgl_cls_info["test_n_rows"], dtype=numpy.dtype(int)
        )
        sgl_cls_info["test_data_lbls"][...] = cls_info_dict[cls_name].id
        f_h5.close()
        test_data_lst.append(sgl_cls_info["test_data"])
        test_lbls_lst.append(sgl_cls_info["test_data_lbls"])

        cls_data_dict[cls_name] = sgl_cls_info
        cls_ids.append(cls_info_dict[cls_name].id)
        n_classes = n_classes + 1

    print("Finished Reading Data")

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbl_np = numpy.concatenate(valid_lbls_lst)
    d_valid = xgb.DMatrix(vaild_np, label=vaild_lbl_np)

    d_train = xgb.DMatrix(
        numpy.concatenate(train_data_lst), label=numpy.concatenate(train_lbls_lst)
    )

    test_np = numpy.concatenate(test_data_lst)
    test_lbl_np = numpy.concatenate(test_lbls_lst)
    d_test = xgb.DMatrix(test_np, label=test_lbl_np)

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _xgb_cls_bo_func(
            eta,
            gamma,
            max_depth,
            min_child_weight,
            max_delta_step,
            subsample,
            num_boost_round,
        ):
            params = {
                "eta": float(eta),
                "gamma": int(gamma),
                "max_depth": int(max_depth),
                "min_child_weight": int(min_child_weight),
                "max_delta_step": int(max_delta_step),
                "subsample": float(subsample),
                "nthread": n_threads,
                "eval_metric": "merror",
                "objective": "multi:softmax",
                "num_class": n_classes,
            }
            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                int(num_boost_round),
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            vld_preds_idxs = model_xgb.predict(d_valid)

            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        hyperparam_space = {
            "eta": (0.01, 0.9),
            "gamma": (0, 100),
            "max_depth": (2, 20),
            "min_child_weight": (1, 10),
            "max_delta_step": (0, 10),
            "subsample": (0.5, 1),
            "num_boost_round": (2, 100),
        }

        bo_opt_obj = BayesianOptimization(
            f=_xgb_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max
        params = {
            "eta": float(op_params["params"]["eta"]),
            "gamma": int(op_params["params"]["gamma"]),
            "max_depth": int(op_params["params"]["max_depth"]),
            "min_child_weight": int(op_params["params"]["min_child_weight"]),
            "max_delta_step": int(op_params["params"]["max_delta_step"]),
            "subsample": float(op_params["params"]["subsample"]),
            "nthread": n_threads,
            "eval_metric": "merror",
            "objective": "multi:softmax",
            "num_class": n_classes,
        }
        num_boost_round = int(op_params["params"]["num_boost_round"])

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _xgb_cls_optuna_func(trial):
            params = {
                "eta": trial.suggest_float("eta", 0.01, 0.9),
                "gamma": trial.suggest_int("gamma", 0, 100),
                "max_depth": trial.suggest_int("max_depth", 2, 20),
                "min_child_weight": trial.suggest_int("min_child_weight", 1, 10),
                "max_delta_step": trial.suggest_int("max_delta_step", 0, 10),
                "subsample": trial.suggest_float("subsample", 0.5, 1),
                "nthread": n_threads,
                "eval_metric": "merror",
                "objective": "multi:softmax",
                "num_class": n_classes,
            }
            num_boost_round_trial = trial.suggest_int("num_boost_round", 2, 100)

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round_trial,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            vld_preds_idxs = model_xgb.predict(d_valid)

            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(_xgb_cls_optuna_func, n_trials=n_opt_iters, timeout=600)

        optuna_opt_trial = optuna_opt_obj.best_trial
        params = {
            "eta": float(optuna_opt_trial.params["eta"]),
            "gamma": int(optuna_opt_trial.params["gamma"]),
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "min_child_weight": int(optuna_opt_trial.params["min_child_weight"]),
            "max_delta_step": int(optuna_opt_trial.params["max_delta_step"]),
            "subsample": float(optuna_opt_trial.params["subsample"]),
            "nthread": n_threads,
            "eval_metric": "merror",
            "objective": "multi:softmax",
            "num_class": n_classes,
        }
        num_boost_round = int(optuna_opt_trial.params["num_boost_round"])

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Real(0.01, 0.9, name="eta"),
            skopt.space.Integer(0, 100, name="gamma"),
            skopt.space.Integer(2, 20, name="max_depth"),
            skopt.space.Integer(1, 10, name="min_child_weight"),
            skopt.space.Integer(0, 10, name="max_delta_step"),
            skopt.space.Real(0.5, 1, name="subsample"),
            skopt.space.Integer(2, 100, name="num_boost_round"),
        ]

        def _xgb_cls_skop_func(values):
            params = {
                "eta": values[0],
                "gamma": values[1],
                "max_depth": values[2],
                "min_child_weight": values[3],
                "max_delta_step": values[4],
                "subsample": values[5],
                "nthread": n_threads,
                "eval_metric": "merror",
                "objective": "multi:softmax",
                "num_class": n_classes,
            }

            print("\nNext set of params.....", params)

            num_boost_round = values[6]
            print("num_boost_round = {}.".format(num_boost_round))

            watchlist = [(d_train, "train"), (d_valid, "validation")]
            evals_results = {}
            model_xgb = xgb.train(
                params,
                d_train,
                num_boost_round,
                evals=watchlist,
                evals_result=evals_results,
                verbose_eval=False,
                xgb_model=mdl_cls_obj,
            )

            vld_preds_idxs = model_xgb.predict(d_valid)

            acc_score = -accuracy_score(vaild_lbl_np, vld_preds_idxs)
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _xgb_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

        print("Start Training Find Classifier")
        params = {
            "eta": best_params[0],
            "gamma": best_params[1],
            "max_depth": best_params[2],
            "min_child_weight": best_params[3],
            "max_delta_step": best_params[4],
            "subsample": best_params[5],
            "nthread": n_threads,
            "eval_metric": "merror",
            "objective": "multi:softmax",
            "num_class": n_classes,
        }

        num_boost_round = best_params[6]
    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    watch_list = [(d_train, "train"), (d_valid, "validation")]
    evals_results = {}
    model_xgb = xgb.train(
        params,
        d_train,
        num_boost_round,
        evals=watch_list,
        evals_result=evals_results,
        verbose_eval=False,
        xgb_model=mdl_cls_obj,
    )
    model_xgb.save_model(out_mdl_file)

    vld_preds_idxs = model_xgb.predict(d_valid)
    valid_acc_scr = accuracy_score(vaild_lbl_np, vld_preds_idxs)
    print("Validate Accuracy: {}".format(valid_acc_scr))

    test_preds_idxs = model_xgb.predict(d_test)
    test_acc_scr = accuracy_score(test_lbl_np, test_preds_idxs)
    print("Testing Accuracy: {}".format(test_acc_scr))


def apply_xgboost_multiclass_classifier(
    model_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    in_mask_img: str,
    img_mask_val: int,
    img_file_info: List,
    out_class_img: str,
    gdalformat: str = "KEA",
    class_clr_names: bool = True,
    n_threads: int = 1,
):
    """
    A function for applying a trained multiclass xgboost model to a image or
    stack of image files.

    :param model_file: a trained xgboost multiclass model which can be loaded
                       with the xgb.Booster function load_model(model_file).
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
                          This is used to define the class names and colours
                          if class_clr_names is True.
    :param in_img_msk: is an image file providing a mask to specify where
                       should be classified. Simplest mask is all the valid
                       data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the imgMask to limit the region
                         to which the classification is applied.
                         Can be used to create a hierarchical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects
                          to identify which images and bands are to be used for
                          the classification so it adheres to the training data.
    :param out_class_img: The file path for the output classification image
    :param gdalformat: The output image format (Default: KEA).
    :param class_clr_names: default is True and therefore a colour table will the
                            colours specified in ClassInfoObj and a class_names
                            (from cls_info_dict) column will be added to the
                            output file. Note the output format needs to support
                            a raster attribute table (i.e., KEA).
    :param n_threads: The number of threads used by xgboost

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    def _applyXGBMClassifier(info, inputs, outputs, otherargs):
        out_class_id_vals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            n_pxls = inputs.imageMask.shape[1] * inputs.imageMask.shape[2]
            out_class_id_vals = out_class_id_vals.flatten()
            img_mask_vals = inputs.imageMask.flatten()
            class_vars = numpy.zeros(
                (n_pxls, otherargs.numClassVars), dtype=numpy.float32
            )
            # Array index which can be used to populate the output array following masking etc.
            id_arr = numpy.arange(img_mask_vals.shape[0])
            class_vars_idx = 0
            for img_file in otherargs.imgFileInfo:
                img_arr = inputs.__dict__[img_file.name]
                for band in img_file.bands:
                    class_vars[..., class_vars_idx] = img_arr[(band - 1)].flatten()
                    class_vars_idx = class_vars_idx + 1
            class_vars = class_vars[img_mask_vals == otherargs.mskVal]
            id_arr = id_arr[img_mask_vals == otherargs.mskVal]
            preds_idxs = otherargs.classifier.predict(xgb.DMatrix(class_vars))
            preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
            for cld_id, idx in zip(
                otherargs.cls_id_lut, numpy.arange(0, len(otherargs.cls_id_lut))
            ):
                preds_cls_ids[preds_idxs == idx] = cld_id

            out_class_id_vals[id_arr] = preds_cls_ids
            out_class_id_vals = numpy.expand_dims(
                out_class_id_vals.reshape(
                    (inputs.imageMask.shape[1], inputs.imageMask.shape[2])
                ),
                axis=0,
            )

        outputs.outclsimage = out_class_id_vals

    classifier = xgb.Booster({"nthread": n_threads})
    classifier.load_model(model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = in_mask_img
    num_class_vars = 0
    for imgFile in img_file_info:
        infiles.__dict__[imgFile.name] = imgFile.file_name
        num_class_vars = num_class_vars + len(imgFile.bands)

    n_classes = len(cls_info_dict)
    cls_id_lut = numpy.zeros(n_classes)
    for cls_name in cls_info_dict:
        if cls_info_dict[cls_name].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    cls_name, cls_info_dict[cls_name].id
                )
            )
        cls_id_lut[cls_info_dict[cls_name].id] = cls_info_dict[cls_name].out_id

    outfiles = applier.FilenameAssociations()
    outfiles.outclsimage = out_class_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = classifier
    otherargs.mskVal = img_mask_val
    otherargs.numClassVars = num_class_vars
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
    applier.apply(
        _applyXGBMClassifier, infiles, outfiles, otherargs, controls=aControls
    )
    print("Completed Classification")

    if class_clr_names:
        rsgislib.rastergis.pop_rat_img_stats(
            out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
        rat_dataset = gdal.Open(out_class_img, gdal.GA_Update)
        red = rat.readColumn(rat_dataset, "Red")
        green = rat.readColumn(rat_dataset, "Green")
        blue = rat.readColumn(rat_dataset, "Blue")
        class_names = numpy.empty_like(red, dtype=numpy.dtype("a255"))
        class_names[...] = ""

        for class_key in cls_info_dict:
            print("Apply Colour to class '" + class_key + "'")
            red[cls_info_dict[class_key].out_id] = cls_info_dict[class_key].red
            green[cls_info_dict[class_key].out_id] = cls_info_dict[
                class_key
            ].green
            blue[cls_info_dict[class_key].out_id] = cls_info_dict[class_key].blue
            class_names[cls_info_dict[class_key].out_id] = class_key

        rat.writeColumn(rat_dataset, "Red", red)
        rat.writeColumn(rat_dataset, "Green", green)
        rat.writeColumn(rat_dataset, "Blue", blue)
        rat.writeColumn(rat_dataset, "class_names", class_names)
        rat_dataset = None


def apply_xgboost_multiclass_classifier_rat(
    clumps_img: str,
    variables: List[str],
    model_file: str,
    cls_info_dict: Dict,
    out_col_int: str = "OutClass",
    out_col_str: str = "OutClassName",
    roi_col: str = None,
    roi_val: int = 1,
    class_colours: bool = True,
    n_threads: int = 1,
):
    """
    A function for applying a trained multiclass xgboost model to a raster
    attribute table.

    :param clumps_img: the file path for the input image with associated
                       raster attribute table (RAT) to which the classification
                       will be applied.
    :param variables: A list of column names within the RAT to be used for the
                      classification.
    :param model_file: a trained xgboost multiclass model which can be loaded
                       with the xgb.Booster function load_model(model_file).
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
                          Note, this is just used for the class names, int ID
                          and classification colours.
    :param out_col_int: is the output column name for the int class
                        representation (Default: 'OutClass')
    :param out_col_str: is the output column name for the class names
                        column (Default: 'OutClassName')
    :param roi_col: is a column name for a column which specifies the region to
                    be classified. If None ignored (Default: None)
    :param roi_val: is a int value used within the roi_col to select a region
                    to be classified (Default: 1)
    :param class_colours: is a boolean specifying whether the RAT colour table should
                          be updated using the classification colours (default: True)
    :param n_threads: The number of threads used by xgboost

    """
    if not HAVE_XGBOOST:
        raise rsgislib.RSGISPyException("Do not have xgboost module installed.")

    from rios import ratapplier

    def _apply_rat_classifier(info, inputs, outputs, otherargs):
        """
        This function is used internally within classify_within_rat_tiled using the RIOS ratapplier function
        """
        numpy_vars = []
        for var in otherargs.vars:
            var_vals = getattr(inputs.inrat, var)
            numpy_vars.append(var_vals)

        x_data = numpy.array(numpy_vars)
        x_data = x_data.transpose()

        id_arr = numpy.arange(x_data.shape[0])
        out_class_int_vals = numpy.zeros(x_data.shape[0], dtype=numpy.int16)
        out_class_names_vals = numpy.empty(x_data.shape[0], dtype=numpy.dtype("a255"))
        out_class_names_vals[...] = ""

        id_arr = id_arr[numpy.isfinite(x_data).all(axis=1)]
        v_data = x_data[numpy.isfinite(x_data).all(axis=1)]

        if otherargs.roiCol is not None:
            roi = getattr(inputs.inrat, otherargs.roiCol)
            roi = roi[numpy.isfinite(x_data).all(axis=1)]
            v_data = v_data[roi == otherargs.roiVal]
            id_arr = id_arr[roi == otherargs.roiVal]

        preds_idxs = otherargs.classifier.predict(xgb.DMatrix(v_data))

        preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
        for cld_id, idx in zip(
            otherargs.cls_id_lut, numpy.arange(0, len(otherargs.cls_id_lut))
        ):
            preds_cls_ids[preds_idxs == idx] = cld_id

        out_class_int_vals[id_arr] = preds_cls_ids
        setattr(outputs.outrat, otherargs.outColInt, out_class_int_vals)

        for cls_id in otherargs.cls_name_lut:
            out_class_names_vals[out_class_int_vals == cls_id] = otherargs.cls_name_lut[
                cls_id
            ]
        setattr(outputs.outrat, otherargs.outColStr, out_class_names_vals)

        if otherargs.class_colours:
            red = getattr(inputs.inrat, "Red")
            green = getattr(inputs.inrat, "Green")
            blue = getattr(inputs.inrat, "Blue")

            # Set Background to black
            red[...] = 0
            green[...] = 0
            blue[...] = 0

            # Set colours
            for class_name in otherargs.cls_info_dict:
                cls_id = otherargs.cls_info_dict[class_name].out_id
                red = numpy.where(
                    out_class_int_vals == cls_id,
                    otherargs.cls_info_dict[class_name].red,
                    red,
                )
                green = numpy.where(
                    out_class_int_vals == cls_id,
                    otherargs.cls_info_dict[class_name].green,
                    green,
                )
                blue = numpy.where(
                    out_class_int_vals == cls_id,
                    otherargs.cls_info_dict[class_name].blue,
                    blue,
                )

            setattr(outputs.outrat, "Red", red)
            setattr(outputs.outrat, "Green", green)
            setattr(outputs.outrat, "Blue", blue)

    classifier = xgb.Booster({"nthreads": n_threads})
    classifier.load_model(model_file)

    n_classes = len(cls_info_dict)
    cls_id_lut = numpy.zeros(n_classes)
    cls_name_lut = dict()
    for cls_name in cls_info_dict:
        if cls_info_dict[cls_name].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    cls_name, cls_info_dict[cls_name].id
                )
            )
        cls_id_lut[cls_info_dict[cls_name].id] = cls_info_dict[cls_name].out_id
        cls_name_lut[cls_info_dict[cls_name].out_id] = cls_name

    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()
    in_rats.inrat = ratapplier.RatHandle(clumps_img)
    out_rats.outrat = ratapplier.RatHandle(clumps_img)

    otherargs = ratapplier.OtherArguments()
    otherargs.vars = variables
    otherargs.classifier = classifier
    otherargs.outColInt = out_col_int
    otherargs.outColStr = out_col_str
    otherargs.roiCol = roi_col
    otherargs.roiVal = roi_val
    otherargs.n_classes = n_classes
    otherargs.cls_id_lut = cls_id_lut
    otherargs.cls_name_lut = cls_name_lut
    otherargs.class_colours = class_colours
    otherargs.cls_info_dict = cls_info_dict

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = ratapplier.RatApplierControls()
    aControls.progress = progress_bar

    ratapplier.apply(
        _apply_rat_classifier,
        in_rats,
        out_rats,
        otherargs=otherargs,
        controls=aControls,
    )
