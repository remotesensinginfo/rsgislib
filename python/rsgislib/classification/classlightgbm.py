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
# Version: 1.1
#
# History:
# Version 1.0 - Created.
# Version 1.1 - Changed parameter optimisation libraries.
#
###########################################################################

import gc
import warnings
from typing import List, Dict

import h5py
import numpy
from osgeo import gdal
from rios import applier, cuiprogress, rat

import rsgislib
import rsgislib.imagecalc
import rsgislib.classification
import rsgislib.imageutils
import rsgislib.rastergis
import rsgislib.tools.utils

HAVE_LIGHTGBM = True
try:
    import lightgbm as lgb
except ImportError:
    HAVE_LIGHTGBM = False

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False

from sklearn.metrics import accuracy_score, roc_auc_score

warnings.filterwarnings("ignore")


def optimise_lightgbm_binary_classifier(
    out_params_file: str,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    unbalanced: bool = False,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    scale_pos_weight: float = None,
    early_stopping_rounds: int = None,
    num_iterations: int = 100,
    max_n_leaves: int = 50,
    learning_rate: float = 0.1,
    mdl_cls_obj=None,
):
    """
    A function which performs a hyper-parameter optimisation for a binary
    lightgbm classifier. Class 1 is the class which you are interested in
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
    :param unbalanced: Boolean (Default: False) specifying whether the training data
                       is unbalanced (i.e., a different number of samples for each
                       class).
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
    :param n_threads: The number of threads used by lightgbm
    :param scale_pos_weight: Optional, default is None. If None then a value will
                             automatically be calculated. Parameter used to balance
                             imbalanced training data.
    :param early_stopping_rounds: If not None then activates early stopping.
                                  The model will train until the validation score
                                  stops improving. Validation score needs to improve
                                  at least every early_stopping_rounds round(s)
                                  to continue training.
    :param num_iterations: The number of boosting iterations (Default: 100)
    :param max_n_leaves: The upper limited used within the optimisation search
                         for the maximum number of leaves used within the model.
                         Default: 50.
    :param learning_rate: Default 0.1 (constraint > 0.0) controlling the shrinkage rate
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

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

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _lgbm_cls_bo_func(
            max_depth,
            num_leaves,
            min_data_in_leaf,
            lambda_l1,
            lambda_l2,
            feature_fraction,
            bagging_fraction,
            min_split_gain,
            min_child_weight,
            reg_alpha,
            reg_lambda,
        ):
            params = {
                "max_depth": int(max_depth),
                "num_leaves": int(num_leaves),
                "min_data_in_leaf": int(min_data_in_leaf),
                "feature_pre_filter": False,
                "lambda_l1": float(lambda_l1),
                "lambda_l2": float(lambda_l2),
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": float(feature_fraction),
                "bagging_fraction": float(bagging_fraction),
                "min_split_gain": float(min_split_gain),
                "min_child_weight": float(min_child_weight),
                "reg_alpha": float(reg_alpha),
                "reg_lambda": float(reg_lambda),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }
            if not unbalanced:
                params["scale_pos_weight"] = scale_pos_weight

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            acc_score = roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
            gc.collect()
            return acc_score

        hyperparam_space = {
            "max_depth": (3, 10),
            "num_leaves": (6, max_n_leaves),
            "min_data_in_leaf": (3, 50),
            "lambda_l1": (0, 5),
            "lambda_l2": (0, 3),
            "feature_fraction": (0.1, 0.9),
            "bagging_fraction": (0.8, 1.0),
            "min_split_gain": (0.001, 0.1),
            "min_child_weight": (1, 50),
            "reg_alpha": (1, 1.2),
            "reg_lambda": (1, 1.4),
        }

        bo_opt_obj = BayesianOptimization(
            f=_lgbm_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max

        params = {
            "max_depth": int(op_params["params"]["max_depth"]),
            "num_leaves": int(op_params["params"]["num_leaves"]),
            "min_data_in_leaf": int(op_params["params"]["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(op_params["params"]["lambda_l1"]),
            "lambda_l2": float(op_params["params"]["lambda_l2"]),
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": float(op_params["params"]["feature_fraction"]),
            "bagging_fraction": float(op_params["params"]["bagging_fraction"]),
            "min_split_gain": float(op_params["params"]["min_split_gain"]),
            "min_child_weight": float(op_params["params"]["min_child_weight"]),
            "reg_alpha": float(op_params["params"]["reg_alpha"]),
            "reg_lambda": float(op_params["params"]["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }
        if not unbalanced:
            params["scale_pos_weight"] = scale_pos_weight

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _lgbm_cls_optuna_func(trial):
            params = {
                "max_depth": trial.suggest_int("max_depth", 3, 10),
                "num_leaves": trial.suggest_int("num_leaves", 6, max_n_leaves),
                "min_data_in_leaf": trial.suggest_int("min_data_in_leaf", 3, 50),
                "feature_pre_filter": False,
                "lambda_l1": trial.suggest_float("lambda_l1", 0, 5),
                "lambda_l2": trial.suggest_float("lambda_l2", 0, 3),
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": trial.suggest_float("feature_fraction", 0.1, 0.9),
                "bagging_fraction": trial.suggest_float("bagging_fraction", 0.8, 1.0),
                "min_split_gain": trial.suggest_float("min_split_gain", 0.001, 0.1),
                "min_child_weight": trial.suggest_float("min_child_weight", 1, 50),
                "reg_alpha": trial.suggest_float("reg_alpha", 1, 1.2),
                "reg_lambda": trial.suggest_float("reg_lambda", 1, 1.4),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }
            if not unbalanced:
                params["scale_pos_weight"] = scale_pos_weight

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            acc_score = roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(
            _lgbm_cls_optuna_func, n_trials=n_opt_iters, timeout=600
        )

        optuna_opt_trial = optuna_opt_obj.best_trial

        params = {
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "num_leaves": int(optuna_opt_trial.params["num_leaves"]),
            "min_data_in_leaf": int(optuna_opt_trial.params["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(optuna_opt_trial.params["lambda_l1"]),
            "lambda_l2": float(optuna_opt_trial.params["lambda_l2"]),
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": float(optuna_opt_trial.params["feature_fraction"]),
            "bagging_fraction": float(optuna_opt_trial.params["bagging_fraction"]),
            "min_split_gain": float(optuna_opt_trial.params["min_split_gain"]),
            "min_child_weight": float(optuna_opt_trial.params["min_child_weight"]),
            "reg_alpha": float(optuna_opt_trial.params["reg_alpha"]),
            "reg_lambda": float(optuna_opt_trial.params["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }
        if not unbalanced:
            params["scale_pos_weight"] = scale_pos_weight

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Integer(3, 10, name="max_depth"),
            skopt.space.Integer(6, max_n_leaves, name="num_leaves"),
            skopt.space.Integer(3, 50, name="min_data_in_leaf"),
            skopt.space.Real(0, 5, name="lambda_l1"),
            skopt.space.Real(0, 3, name="lambda_l2"),
            skopt.space.Real(0.1, 0.9, name="feature_fraction"),
            skopt.space.Real(0.8, 1.0, name="bagging_fraction"),
            skopt.space.Real(0.001, 0.1, name="min_split_gain"),
            skopt.space.Real(1, 50, name="min_child_weight"),
            skopt.space.Real(1, 1.2, name="reg_alpha"),
            skopt.space.Real(1, 1.4, name="reg_lambda"),
        ]

        def _lgbm_cls_skop_func(values):
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
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }
            if not unbalanced:
                params["scale_pos_weight"] = scale_pos_weight

            print("\nNext set of params.....", params)

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            acc_score = -roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _lgbm_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

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
            "verbose": -1,
            "verbosity": -1,
        }
        if not unbalanced:
            params["scale_pos_weight"] = scale_pos_weight
    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    rsgislib.tools.utils.write_dict_to_json(params, out_params_file)


def train_opt_lightgbm_binary_classifier(
    out_mdl_file: str,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls1_test_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    cls2_test_file: str,
    unbalanced: bool = False,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    scale_pos_weight: float = None,
    early_stopping_rounds: int = None,
    num_iterations: int = 100,
    max_n_leaves: int = 50,
    learning_rate: float = 0.1,
    mdl_cls_obj=None,
    out_params_file: str = None,
):
    """
    A function which performs a hyper-parameter optimisation for a binary
    lightgbm classifier and then trains a model saving the model for future
    use. Class 1 is the class which you are interested in and Class 2 is
    the 'other class'.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_mdl_file: The file path for the output lightgbm (*.txt) model which
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
    :param unbalanced: Boolean (Default: False) specifying whether the training data
                       is unbalanced (i.e., a different number of samples for each
                       class).
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
    :param n_threads: The number of threads used by lightgbm
    :param scale_pos_weight: Optional, default is None. If None then a value will
                             automatically be calculated. Parameter used to balance
                             imbalanced training data.
    :param early_stopping_rounds: If not None then activates early stopping.
                                  The model will train until the validation score
                                  stops improving. Validation score needs to improve
                                  at least every early_stopping_rounds round(s)
                                  to continue training.
    :param num_iterations: The number of boosting iterations (Default: 100)
    :param max_n_leaves: The upper limited used within the optimisation search
                         for the maximum number of leaves used within the model.
                         Default: 50.
    :param learning_rate: Default 0.1 (constraint > 0.0) controlling the shrinkage rate
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).
    :param out_params_file: The output JSON file with the identified parameters.
                            If None (default) then no file is outputted.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

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

    if scale_pos_weight is None:
        scale_pos_weight = num_cls2_train_rows / num_cls1_train_rows
        if scale_pos_weight < 1:
            scale_pos_weight = 1
    print("scale_pos_weight = {}".format(scale_pos_weight))

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _lgbm_cls_bo_func(
            max_depth,
            num_leaves,
            min_data_in_leaf,
            lambda_l1,
            lambda_l2,
            feature_fraction,
            bagging_fraction,
            min_split_gain,
            min_child_weight,
            reg_alpha,
            reg_lambda,
        ):
            params = {
                "max_depth": int(max_depth),
                "num_leaves": int(num_leaves),
                "min_data_in_leaf": int(min_data_in_leaf),
                "feature_pre_filter": False,
                "lambda_l1": float(lambda_l1),
                "lambda_l2": float(lambda_l2),
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": float(feature_fraction),
                "bagging_fraction": float(bagging_fraction),
                "min_split_gain": float(min_split_gain),
                "min_child_weight": float(min_child_weight),
                "reg_alpha": float(reg_alpha),
                "reg_lambda": float(reg_lambda),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }
            if not unbalanced:
                params["scale_pos_weight"] = scale_pos_weight

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )
            acc_score = roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
            gc.collect()
            return acc_score

        hyperparam_space = {
            "max_depth": (3, 10),
            "num_leaves": (6, max_n_leaves),
            "min_data_in_leaf": (3, 50),
            "lambda_l1": (0, 5),
            "lambda_l2": (0, 3),
            "feature_fraction": (0.1, 0.9),
            "bagging_fraction": (0.8, 1.0),
            "min_split_gain": (0.001, 0.1),
            "min_child_weight": (1, 50),
            "reg_alpha": (1, 1.2),
            "reg_lambda": (1, 1.4),
        }

        bo_opt_obj = BayesianOptimization(
            f=_lgbm_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max

        params = {
            "max_depth": int(op_params["params"]["max_depth"]),
            "num_leaves": int(op_params["params"]["num_leaves"]),
            "min_data_in_leaf": int(op_params["params"]["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(op_params["params"]["lambda_l1"]),
            "lambda_l2": float(op_params["params"]["lambda_l2"]),
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": float(op_params["params"]["feature_fraction"]),
            "bagging_fraction": float(op_params["params"]["bagging_fraction"]),
            "min_split_gain": float(op_params["params"]["min_split_gain"]),
            "min_child_weight": float(op_params["params"]["min_child_weight"]),
            "reg_alpha": float(op_params["params"]["reg_alpha"]),
            "reg_lambda": float(op_params["params"]["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }
        if not unbalanced:
            params["scale_pos_weight"] = scale_pos_weight

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _lgbm_cls_optuna_func(trial):
            params = {
                "max_depth": trial.suggest_int("max_depth", 3, 10),
                "num_leaves": trial.suggest_int("num_leaves", 6, max_n_leaves),
                "min_data_in_leaf": trial.suggest_int("min_data_in_leaf", 3, 50),
                "feature_pre_filter": False,
                "lambda_l1": trial.suggest_float("lambda_l1", 0, 5),
                "lambda_l2": trial.suggest_float("lambda_l2", 0, 3),
                "metric": "auc,binary_error",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "binary",
                "learning_rate": learning_rate,
                "feature_fraction": trial.suggest_float("feature_fraction", 0.1, 0.9),
                "bagging_fraction": trial.suggest_float("bagging_fraction", 0.8, 1.0),
                "min_split_gain": trial.suggest_float("min_split_gain", 0.001, 0.1),
                "min_child_weight": trial.suggest_float("min_child_weight", 1, 50),
                "reg_alpha": trial.suggest_float("reg_alpha", 1, 1.2),
                "reg_lambda": trial.suggest_float("reg_lambda", 1, 1.4),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }
            if not unbalanced:
                params["scale_pos_weight"] = scale_pos_weight

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            acc_score = roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(
            _lgbm_cls_optuna_func, n_trials=n_opt_iters, timeout=600
        )

        optuna_opt_trial = optuna_opt_obj.best_trial

        params = {
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "num_leaves": int(optuna_opt_trial.params["num_leaves"]),
            "min_data_in_leaf": int(optuna_opt_trial.params["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(optuna_opt_trial.params["lambda_l1"]),
            "lambda_l2": float(optuna_opt_trial.params["lambda_l2"]),
            "metric": "auc,binary_error",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "binary",
            "learning_rate": learning_rate,
            "feature_fraction": float(optuna_opt_trial.params["feature_fraction"]),
            "bagging_fraction": float(optuna_opt_trial.params["bagging_fraction"]),
            "min_split_gain": float(optuna_opt_trial.params["min_split_gain"]),
            "min_child_weight": float(optuna_opt_trial.params["min_child_weight"]),
            "reg_alpha": float(optuna_opt_trial.params["reg_alpha"]),
            "reg_lambda": float(optuna_opt_trial.params["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }
        if not unbalanced:
            params["scale_pos_weight"] = scale_pos_weight

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Integer(3, 10, name="max_depth"),
            skopt.space.Integer(6, max_n_leaves, name="num_leaves"),
            skopt.space.Integer(3, 50, name="min_data_in_leaf"),
            skopt.space.Real(0, 5, name="lambda_l1"),
            skopt.space.Real(0, 3, name="lambda_l2"),
            skopt.space.Real(0.1, 0.9, name="feature_fraction"),
            skopt.space.Real(0.8, 1.0, name="bagging_fraction"),
            skopt.space.Real(0.001, 0.1, name="min_split_gain"),
            skopt.space.Real(1, 50, name="min_child_weight"),
            skopt.space.Real(1, 1.2, name="reg_alpha"),
            skopt.space.Real(1, 1.4, name="reg_lambda"),
        ]

        def _lgbm_cls_skop_func(values):
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
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }
            if not unbalanced:
                params["scale_pos_weight"] = scale_pos_weight

            print("\nNext set of params.....", params)

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            acc_score = -roc_auc_score(vaild_lbl_np, model_lgb.predict(vaild_np))
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _lgbm_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

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
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }
        if not unbalanced:
            params["scale_pos_weight"] = scale_pos_weight
    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    if out_params_file is not None:
        rsgislib.tools.utils.write_dict_to_json(params, out_params_file)

    print("Start training final classifier")

    if early_stopping_rounds is not None:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
            callbacks=[lgb.early_stopping(stopping_rounds=early_stopping_rounds)],
        )
    else:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
        )

    test_auc = roc_auc_score(test_lbl_np, model_lgb.predict(test_np))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model_lgb.save_model(out_mdl_file)

    pred_test = model_lgb.predict(test_np)
    for i in range(test_np.shape[0]):
        if pred_test[i] >= 0.5:
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def train_lightgbm_binary_classifier(
    out_mdl_file: str,
    cls_params_file: str,
    cls1_train_file: str,
    cls1_valid_file: str,
    cls1_test_file: str,
    cls2_train_file: str,
    cls2_valid_file: str,
    cls2_test_file: str,
    unbalanced: bool = False,
    n_threads: int = 1,
    scale_pos_weight: float = None,
    early_stopping_rounds: int = None,
    num_iterations: int = 100,
    learning_rate: float = 0.1,
    mdl_cls_obj=None,
):
    """
    A function which trains a binary lightgbm model using the parameters provided
    within a JSON file. The JSON file must provide values for the following
    parameters:

       * max_depth
       * num_leaves
       * min_data_in_leaf
       * lambda_l1
       * lambda_l2
       * feature_fraction
       * bagging_fraction
       * min_split_gain
       * min_child_weight
       * reg_alpha
       * reg_lambda

    :param out_mdl_file: The file path for the output lightgbm (*.txt) model which
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
    :param unbalanced: Boolean (Default: False) specifying whether the training data
                       is unbalanced (i.e., a different number of samples for each
                       class).
    :param n_threads: The number of threads used by lightgbm
    :param scale_pos_weight: Optional, default is None. If None then a value will
                             automatically be calculated. Parameter used to balance
                             imbalanced training data.
    :param early_stopping_rounds: If not None then activates early stopping.
                                  The model will train until the validation score
                                  stops improving. Validation score needs to improve
                                  at least every early_stopping_rounds round(s)
                                  to continue training.
    :param num_iterations: The number of boosting iterations (Default: 100)
    :param learning_rate: Default 0.1 (constraint > 0.0) controlling the shrinkage rate
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

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

    cls_params = rsgislib.tools.utils.read_json_to_dict(cls_params_file)

    print("Start Training Find Classifier")
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
        "boost_from_average": True,
        "is_unbalance": unbalanced,
        "verbose": -1,
    }
    if not unbalanced:
        params["scale_pos_weight"] = scale_pos_weight

    if early_stopping_rounds is not None:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
            callbacks=[lgb.early_stopping(stopping_rounds=early_stopping_rounds)],
        )
    else:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
        )
    test_auc = roc_auc_score(test_lbl_np, model_lgb.predict(test_np))
    print("Testing AUC: {}".format(test_auc))
    print("Finish Training")

    model_lgb.save_model(out_mdl_file)

    pred_test = model_lgb.predict(test_np)
    for i in range(test_np.shape[0]):
        if pred_test[i] >= 0.5:
            pred_test[i] = 1
        else:
            pred_test[i] = 0
    len(pred_test)

    test_acc = accuracy_score(test_lbl_np, pred_test)
    print("Testing Accuracy: {}".format(test_acc))


def apply_lightgbm_binary_classifier(
    model_file: str,
    in_msk_img: str,
    img_msk_val: int,
    img_file_info: List[rsgislib.imageutils.ImageBandInfo],
    out_score_img: str,
    gdalformat: str = "KEA",
    out_class_img: str = None,
    class_thres: int = 5000,
):
    """
    A function for applying a trained binary lightgbm model to a image or stack of
    image files.

    :param model_file: a trained lightgbm binary model which can be loaded
                       with lgb.Booster(model_file=model_file).
    :param in_msk_img: is an image file providing a mask to specify where
                       should be classified. Simplest mask is all the valid
                       data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_msk_val: the pixel value within the in_msk_img to limit the region
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

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    def _applyLGBMClassifier(info, inputs, outputs, otherargs):
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
            pred_class = numpy.around(otherargs.classifier.predict(class_vars) * 10000)
            out_class_vals[id_arr] = pred_class
            out_class_vals = numpy.expand_dims(
                out_class_vals.reshape(
                    (inputs.imageMask.shape[1], inputs.imageMask.shape[2])
                ),
                axis=0,
            )
        outputs.outimage = out_class_vals

    classifier = lgb.Booster(model_file=model_file)

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
    otherargs.mskVal = img_msk_val
    otherargs.numClassVars = num_class_vars
    otherargs.imgFileInfo = img_file_info

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

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


def optimise_lightgbm_multiclass_classifier(
    out_params_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    unbalanced: bool = False,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    early_stopping_rounds: int = None,
    num_iterations: int = 100,
    max_n_leaves: int = 50,
    learning_rate: float = 0.1,
    mdl_cls_obj=None,
):
    """
    A function which performs a hyper-parameter optimisation for a multi-class
    lightgbm classifier.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_params_file: The output JSON file with the identified parameters
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
    :param unbalanced: Boolean (Default: False) specifying whether the training data
                       is unbalanced (i.e., a different number of samples for each
                       class).
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
    :param n_threads: The number of threads used by lightgbm
    :param early_stopping_rounds: If not None then activates early stopping.
                                  The model will train until the validation score
                                  stops improving. Validation score needs to improve
                                  at least every early_stopping_rounds round(s)
                                  to continue training.
    :param num_iterations: The number of boosting iterations (Default: 100)
    :param max_n_leaves: The upper limited used within the optimisation search
                         for the maximum number of leaves used within the model.
                         Default: 50.
    :param learning_rate: Default 0.1 (constraint > 0.0) controlling the shrinkage rate
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

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
    cls_ids = []
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

        cls_data_dict[cls_name] = sgl_cls_info
        cls_ids.append(cls_info_dict[cls_name].id)

    print("Finished Reading Data")

    d_train = lgb.Dataset(
        numpy.concatenate(train_data_lst), label=numpy.concatenate(train_lbls_lst)
    )
    d_valid = lgb.Dataset(
        numpy.concatenate(valid_data_lst), label=numpy.concatenate(valid_lbls_lst)
    )

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbl_np = numpy.concatenate(valid_lbls_lst)

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _lgbm_cls_bo_func(
            max_depth,
            num_leaves,
            min_data_in_leaf,
            lambda_l1,
            lambda_l2,
            feature_fraction,
            bagging_fraction,
            min_split_gain,
            min_child_weight,
            reg_alpha,
            reg_lambda,
        ):
            params = {
                "max_depth": int(max_depth),
                "num_leaves": int(num_leaves),
                "min_data_in_leaf": int(min_data_in_leaf),
                "feature_pre_filter": False,
                "lambda_l1": float(lambda_l1),
                "lambda_l2": float(lambda_l2),
                "metric": "multi_logloss",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "multiclass",
                "num_class": n_classes,
                "learning_rate": learning_rate,
                "feature_fraction": float(feature_fraction),
                "bagging_fraction": float(bagging_fraction),
                "min_split_gain": float(min_split_gain),
                "min_child_weight": float(min_child_weight),
                "reg_alpha": float(reg_alpha),
                "reg_lambda": float(reg_lambda),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        hyperparam_space = {
            "max_depth": (3, 10),
            "num_leaves": (6, max_n_leaves),
            "min_data_in_leaf": (3, 50),
            "lambda_l1": (0, 5),
            "lambda_l2": (0, 3),
            "feature_fraction": (0.1, 0.9),
            "bagging_fraction": (0.8, 1.0),
            "min_split_gain": (0.001, 0.1),
            "min_child_weight": (1, 50),
            "reg_alpha": (1, 1.2),
            "reg_lambda": (1, 1.4),
        }

        bo_opt_obj = BayesianOptimization(
            f=_lgbm_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max

        params = {
            "max_depth": int(op_params["params"]["max_depth"]),
            "num_leaves": int(op_params["params"]["num_leaves"]),
            "min_data_in_leaf": int(op_params["params"]["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(op_params["params"]["lambda_l1"]),
            "lambda_l2": float(op_params["params"]["lambda_l2"]),
            "metric": "multi_logloss",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "learning_rate": learning_rate,
            "feature_fraction": float(op_params["params"]["feature_fraction"]),
            "bagging_fraction": float(op_params["params"]["bagging_fraction"]),
            "min_split_gain": float(op_params["params"]["min_split_gain"]),
            "min_child_weight": float(op_params["params"]["min_child_weight"]),
            "reg_alpha": float(op_params["params"]["reg_alpha"]),
            "reg_lambda": float(op_params["params"]["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _lgbm_cls_optuna_func(trial):
            params = {
                "max_depth": trial.suggest_int("max_depth", 3, 10),
                "num_leaves": trial.suggest_int("num_leaves", 6, max_n_leaves),
                "min_data_in_leaf": trial.suggest_int("min_data_in_leaf", 3, 50),
                "feature_pre_filter": False,
                "lambda_l1": trial.suggest_float("lambda_l1", 0, 5),
                "lambda_l2": trial.suggest_float("lambda_l2", 0, 3),
                "metric": "multi_logloss",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "multiclass",
                "num_class": n_classes,
                "learning_rate": learning_rate,
                "feature_fraction": trial.suggest_float("feature_fraction", 0.1, 0.9),
                "bagging_fraction": trial.suggest_float("bagging_fraction", 0.8, 1.0),
                "min_split_gain": trial.suggest_float("min_split_gain", 0.001, 0.1),
                "min_child_weight": trial.suggest_float("min_child_weight", 1, 50),
                "reg_alpha": trial.suggest_float("reg_alpha", 1, 1.2),
                "reg_lambda": trial.suggest_float("reg_lambda", 1, 1.4),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(
            _lgbm_cls_optuna_func, n_trials=n_opt_iters, timeout=600
        )

        optuna_opt_trial = optuna_opt_obj.best_trial

        params = {
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "num_leaves": int(optuna_opt_trial.params["num_leaves"]),
            "min_data_in_leaf": int(optuna_opt_trial.params["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(optuna_opt_trial.params["lambda_l1"]),
            "lambda_l2": float(optuna_opt_trial.params["lambda_l2"]),
            "metric": "multi_logloss",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "learning_rate": learning_rate,
            "feature_fraction": float(optuna_opt_trial.params["feature_fraction"]),
            "bagging_fraction": float(optuna_opt_trial.params["bagging_fraction"]),
            "min_split_gain": float(optuna_opt_trial.params["min_split_gain"]),
            "min_child_weight": float(optuna_opt_trial.params["min_child_weight"]),
            "reg_alpha": float(optuna_opt_trial.params["reg_alpha"]),
            "reg_lambda": float(optuna_opt_trial.params["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Integer(3, 10, name="max_depth"),
            skopt.space.Integer(6, max_n_leaves, name="num_leaves"),
            skopt.space.Integer(3, 50, name="min_data_in_leaf"),
            skopt.space.Real(0, 5, name="lambda_l1"),
            skopt.space.Real(0, 3, name="lambda_l2"),
            skopt.space.Real(0.1, 0.9, name="feature_fraction"),
            skopt.space.Real(0.8, 1.0, name="bagging_fraction"),
            skopt.space.Real(0.001, 0.1, name="min_split_gain"),
            skopt.space.Real(1, 50, name="min_child_weight"),
            skopt.space.Real(1, 1.2, name="reg_alpha"),
            skopt.space.Real(1, 1.4, name="reg_lambda"),
        ]

        def _lgbm_cls_skop_func(values):
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
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }

            print("\nNext set of params.....", params)

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
            acc_score = -accuracy_score(vaild_lbl_np, vld_preds_idxs)
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _lgbm_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

        params = {
            "max_depth": int(best_params[0]),
            "num_leaves": int(best_params[1]),
            "min_data_in_leaf": int(best_params[2]),
            "feature_pre_filter": False,
            "lambda_l1": float(best_params[3]),
            "lambda_l2": float(best_params[4]),
            "metric": "multi_logloss",
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "feature_fraction": float(best_params[5]),
            "bagging_fraction": float(best_params[6]),
            "min_split_gain": float(best_params[7]),
            "min_child_weight": float(best_params[8]),
            "reg_alpha": float(best_params[9]),
            "reg_lambda": float(best_params[10]),
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }

    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    rsgislib.tools.utils.write_dict_to_json(params, out_params_file)


def train_opt_lightgbm_multiclass_classifier(
    out_mdl_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    unbalanced: bool = False,
    op_mthd: int = rsgislib.OPT_MTHD_BAYESOPT,
    n_opt_iters: int = 100,
    rnd_seed: int = None,
    n_threads: int = 1,
    early_stopping_rounds: int = None,
    num_iterations: int = 100,
    max_n_leaves: int = 50,
    learning_rate: float = 0.1,
    mdl_cls_obj=None,
    out_params_file: str = None,
    out_info_file: str = None,
):
    """
    A function which performs a hyper-parameter optimisation for a multi-class
    lightgbm classifier and then trains a model saving the model for future
    use.

    You have the option of using the bayes_opt (Default), optuna or skopt
    optimisation libraries. Before 5.1.0 skopt was the only option but this
    no longer appears to be maintained so the other options have been added.

    :param out_mdl_file: The file path for the output lightgbm (*.txt) model which
                         can be loaded to perform a classification.
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
    :param unbalanced: Boolean (Default: False) specifying whether the training data
                       is unbalanced (i.e., a different number of samples for each
                       class).
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
    :param n_threads: The number of threads used by lightgbm
    :param early_stopping_rounds: If not None then activates early stopping.
                                  The model will train until the validation score
                                  stops improving. Validation score needs to improve
                                  at least every early_stopping_rounds round(s)
                                  to continue training.
    :param num_iterations: The number of boosting iterations (Default: 100)
    :param max_n_leaves: The upper limited used within the optimisation search
                         for the maximum number of leaves used within the model.
                         Default: 50.
    :param learning_rate: Default 0.1 (constraint > 0.0) controlling the shrinkage rate
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).
    :param out_params_file: An output JSON file with the identified parameters.
                            If None (default) then no file is outputted.
    :param out_info_file: An output JSON file with the classification outputs
                          scores (e.g., training, testing). If None (default)
                          then no file is outputted.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

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

    if op_mthd == rsgislib.OPT_MTHD_BAYESOPT:
        print("Using: OPT_MTHD_BAYESOPT")
        from bayes_opt import BayesianOptimization

        def _lgbm_cls_bo_func(
            max_depth,
            num_leaves,
            min_data_in_leaf,
            lambda_l1,
            lambda_l2,
            feature_fraction,
            bagging_fraction,
            min_split_gain,
            min_child_weight,
            reg_alpha,
            reg_lambda,
        ):
            params = {
                "max_depth": int(max_depth),
                "num_leaves": int(num_leaves),
                "min_data_in_leaf": int(min_data_in_leaf),
                "feature_pre_filter": False,
                "lambda_l1": float(lambda_l1),
                "lambda_l2": float(lambda_l2),
                "metric": "multi_logloss",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "multiclass",
                "num_class": n_classes,
                "learning_rate": learning_rate,
                "feature_fraction": float(feature_fraction),
                "bagging_fraction": float(bagging_fraction),
                "min_split_gain": float(min_split_gain),
                "min_child_weight": float(min_child_weight),
                "reg_alpha": float(reg_alpha),
                "reg_lambda": float(reg_lambda),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        hyperparam_space = {
            "max_depth": (3, 10),
            "num_leaves": (6, max_n_leaves),
            "min_data_in_leaf": (3, 50),
            "lambda_l1": (0, 5),
            "lambda_l2": (0, 3),
            "feature_fraction": (0.1, 0.9),
            "bagging_fraction": (0.8, 1.0),
            "min_split_gain": (0.001, 0.1),
            "min_child_weight": (1, 50),
            "reg_alpha": (1, 1.2),
            "reg_lambda": (1, 1.4),
        }

        bo_opt_obj = BayesianOptimization(
            f=_lgbm_cls_bo_func,
            pbounds=hyperparam_space,
            random_state=rnd_seed,
            verbose=10,
        )

        bo_opt_obj.maximize(init_points=10, n_iter=n_opt_iters)

        op_params = bo_opt_obj.max

        params = {
            "max_depth": int(op_params["params"]["max_depth"]),
            "num_leaves": int(op_params["params"]["num_leaves"]),
            "min_data_in_leaf": int(op_params["params"]["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(op_params["params"]["lambda_l1"]),
            "lambda_l2": float(op_params["params"]["lambda_l2"]),
            "metric": "multi_logloss",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "learning_rate": learning_rate,
            "feature_fraction": float(op_params["params"]["feature_fraction"]),
            "bagging_fraction": float(op_params["params"]["bagging_fraction"]),
            "min_split_gain": float(op_params["params"]["min_split_gain"]),
            "min_child_weight": float(op_params["params"]["min_child_weight"]),
            "reg_alpha": float(op_params["params"]["reg_alpha"]),
            "reg_lambda": float(op_params["params"]["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }

    elif op_mthd == rsgislib.OPT_MTHD_OPTUNA:
        print("Using OPT_MTHD_OPTUNA")
        import optuna

        def _lgbm_cls_optuna_func(trial):
            params = {
                "max_depth": trial.suggest_int("max_depth", 3, 10),
                "num_leaves": trial.suggest_int("num_leaves", 6, max_n_leaves),
                "min_data_in_leaf": trial.suggest_int("min_data_in_leaf", 3, 50),
                "feature_pre_filter": False,
                "lambda_l1": trial.suggest_float("lambda_l1", 0, 5),
                "lambda_l2": trial.suggest_float("lambda_l2", 0, 3),
                "metric": "multi_logloss",
                "nthread": n_threads,
                "boosting_type": "gbdt",
                "objective": "multiclass",
                "num_class": n_classes,
                "learning_rate": learning_rate,
                "feature_fraction": trial.suggest_float("feature_fraction", 0.1, 0.9),
                "bagging_fraction": trial.suggest_float("bagging_fraction", 0.8, 1.0),
                "min_split_gain": trial.suggest_float("min_split_gain", 0.001, 0.1),
                "min_child_weight": trial.suggest_float("min_child_weight", 1, 50),
                "reg_alpha": trial.suggest_float("reg_alpha", 1, 1.2),
                "reg_lambda": trial.suggest_float("reg_lambda", 1, 1.4),
                "num_iterations": num_iterations,
                "boost_from_average": True,
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
            acc_score = accuracy_score(vaild_lbl_np, vld_preds_idxs)
            gc.collect()
            return acc_score

        optuna_opt_obj = optuna.create_study(direction="maximize")
        optuna_opt_obj.optimize(
            _lgbm_cls_optuna_func, n_trials=n_opt_iters, timeout=600
        )

        optuna_opt_trial = optuna_opt_obj.best_trial

        params = {
            "max_depth": int(optuna_opt_trial.params["max_depth"]),
            "num_leaves": int(optuna_opt_trial.params["num_leaves"]),
            "min_data_in_leaf": int(optuna_opt_trial.params["min_data_in_leaf"]),
            "feature_pre_filter": False,
            "lambda_l1": float(optuna_opt_trial.params["lambda_l1"]),
            "lambda_l2": float(optuna_opt_trial.params["lambda_l2"]),
            "metric": "multi_logloss",
            "nthread": n_threads,
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "learning_rate": learning_rate,
            "feature_fraction": float(optuna_opt_trial.params["feature_fraction"]),
            "bagging_fraction": float(optuna_opt_trial.params["bagging_fraction"]),
            "min_split_gain": float(optuna_opt_trial.params["min_split_gain"]),
            "min_child_weight": float(optuna_opt_trial.params["min_child_weight"]),
            "reg_alpha": float(optuna_opt_trial.params["reg_alpha"]),
            "reg_lambda": float(optuna_opt_trial.params["reg_lambda"]),
            "num_iterations": num_iterations,
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }

    elif op_mthd == rsgislib.OPT_MTHD_SKOPT:
        print("Using OPT_MTHD_SKOPT")
        import skopt
        import skopt.space

        space = [
            skopt.space.Integer(3, 10, name="max_depth"),
            skopt.space.Integer(6, max_n_leaves, name="num_leaves"),
            skopt.space.Integer(3, 50, name="min_data_in_leaf"),
            skopt.space.Real(0, 5, name="lambda_l1"),
            skopt.space.Real(0, 3, name="lambda_l2"),
            skopt.space.Real(0.1, 0.9, name="feature_fraction"),
            skopt.space.Real(0.8, 1.0, name="bagging_fraction"),
            skopt.space.Real(0.001, 0.1, name="min_split_gain"),
            skopt.space.Real(1, 50, name="min_child_weight"),
            skopt.space.Real(1, 1.2, name="reg_alpha"),
            skopt.space.Real(1, 1.4, name="reg_lambda"),
        ]

        def _lgbm_cls_skop_func(values):
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
                "is_unbalance": unbalanced,
                "verbose": -1,
                "verbosity": -1,
            }

            print("\nNext set of params.....", params)

            if early_stopping_rounds is not None:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                    callbacks=[
                        lgb.early_stopping(stopping_rounds=early_stopping_rounds)
                    ],
                )
            else:
                model_lgb = lgb.train(
                    params,
                    d_train,
                    valid_sets=[d_train, d_valid],
                    valid_names=["train", "valid"],
                    feval=None,
                    init_model=mdl_cls_obj,
                )

            vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
            acc_score = -accuracy_score(vaild_lbl_np, vld_preds_idxs)
            print("\nAccScore.....", -acc_score, ".....iter.....")
            gc.collect()
            return acc_score

        res_gp = skopt.gp_minimize(
            _lgbm_cls_skop_func, space, n_calls=20, random_state=0, n_random_starts=10
        )

        print("Best score={}".format(res_gp.fun))
        best_params = res_gp.x
        print("Best Params:\n{}".format(best_params))

        params = {
            "max_depth": int(best_params[0]),
            "num_leaves": int(best_params[1]),
            "min_data_in_leaf": int(best_params[2]),
            "feature_pre_filter": False,
            "lambda_l1": float(best_params[3]),
            "lambda_l2": float(best_params[4]),
            "metric": "multi_logloss",
            "boosting_type": "gbdt",
            "objective": "multiclass",
            "num_class": n_classes,
            "feature_fraction": float(best_params[5]),
            "bagging_fraction": float(best_params[6]),
            "min_split_gain": float(best_params[7]),
            "min_child_weight": float(best_params[8]),
            "reg_alpha": float(best_params[9]),
            "reg_lambda": float(best_params[10]),
            "boost_from_average": True,
            "is_unbalance": unbalanced,
            "verbose": -1,
            "verbosity": -1,
        }

    else:
        raise rsgislib.RSGISPyException(
            "Do not recognise or do not have implementation "
            "for the optimisation method specified."
        )

    if out_params_file is not None:
        rsgislib.tools.utils.write_dict_to_json(params, out_params_file)

    print("Start training final classifier")

    if early_stopping_rounds is not None:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
            callbacks=[lgb.early_stopping(stopping_rounds=early_stopping_rounds)],
        )
    else:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
        )

    vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
    valid_acc_scr = accuracy_score(vaild_lbl_np, vld_preds_idxs)
    print("Validate Accuracy: {}".format(valid_acc_scr))
    print("Finish Training")

    model_lgb.save_model(out_mdl_file)

    test_preds_idxs = numpy.argmax(model_lgb.predict(test_np), axis=1)
    test_acc_scr = accuracy_score(test_lbl_np, test_preds_idxs)
    print("Testing Accuracy: {}".format(test_acc_scr))

    if out_info_file is not None:
        out_info = dict()
        out_info["params"] = params
        out_info["test_accuracy"] = test_acc_scr
        rsgislib.tools.utils.write_dict_to_json(out_info, out_info_file)


def train_lightgbm_multiclass_classifier(
    params_file: str,
    out_mdl_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    unbalanced: bool = False,
    n_threads: int = 1,
    early_stopping_rounds: int = None,
    num_iterations: int = 100,
    learning_rate: float = 0.1,
    mdl_cls_obj=None,
    out_info_file: str = None,
):
    """
    A function which trains a multiclass lightgbm model using the parameters
    provided within a JSON file. The JSON file must provide values for the
    following parameters:

       * max_depth
       * num_leaves
       * min_data_in_leaf
       * lambda_l1
       * lambda_l2
       * feature_fraction
       * bagging_fraction
       * min_split_gain
       * min_child_weight
       * reg_alpha
       * reg_lambda

    :param params_file: The file path to the JSON file with the classifier
                        parameters.
    :param out_mdl_file: The file path for the output lightgbm (*.txt) model which
                         can be loaded to perform a classification.
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
    :param unbalanced: Boolean (Default: False) specifying whether the training data
                       is unbalanced (i.e., a different number of samples for each
                       class).
    :param n_threads: The number of threads used by lightgbm
    :param early_stopping_rounds: If not None then activates early stopping.
                                  The model will train until the validation score
                                  stops improving. Validation score needs to improve
                                  at least every early_stopping_rounds round(s)
                                  to continue training.
    :param num_iterations: The number of boosting iterations (Default: 100)
    :param learning_rate: Default 0.1 (constraint > 0.0) controlling the shrinkage rate
    :param mdl_cls_obj: An optional (Default None) lightgbm model which will be
                        used as the basis model from which training will be
                        continued (i.e., transfer learning).
    :param out_info_file: An output JSON file with the classification outputs
                          scores (e.g., training, testing). If None (default)
                          then no file is outputted.

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

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

    cls_params = rsgislib.tools.utils.read_json_to_dict(params_file)

    params = {
        "max_depth": cls_params["max_depth"],
        "num_leaves": cls_params["num_leaves"],
        "min_data_in_leaf": cls_params["min_data_in_leaf"],
        "feature_pre_filter": False,
        "lambda_l1": cls_params["lambda_l1"],
        "lambda_l2": cls_params["lambda_l2"],
        "metric": "multi_logloss",
        "nthread": n_threads,
        "boosting_type": "gbdt",
        "objective": "multiclass",
        "num_class": n_classes,
        "learning_rate": learning_rate,
        "feature_fraction": cls_params["feature_fraction"],
        "bagging_fraction": cls_params["bagging_fraction"],
        "min_split_gain": cls_params["min_split_gain"],
        "min_child_weight": cls_params["min_child_weight"],
        "reg_alpha": cls_params["reg_alpha"],
        "reg_lambda": cls_params["reg_lambda"],
        "num_iterations": num_iterations,
        "boost_from_average": True,
        "is_unbalance": unbalanced,
        "verbose": -1,
    }

    if early_stopping_rounds is not None:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
            callbacks=[lgb.early_stopping(stopping_rounds=early_stopping_rounds)],
        )
    else:
        model_lgb = lgb.train(
            params,
            d_train,
            valid_sets=[d_train, d_valid],
            valid_names=["train", "valid"],
            feval=None,
            init_model=mdl_cls_obj,
        )

    vld_preds_idxs = numpy.argmax(model_lgb.predict(vaild_np), axis=1)
    valid_acc_scr = accuracy_score(vaild_lbl_np, vld_preds_idxs)
    print("Validate Accuracy: {}".format(valid_acc_scr))
    print("Finish Training")

    model_lgb.save_model(out_mdl_file)

    test_preds_idxs = numpy.argmax(model_lgb.predict(test_np), axis=1)
    test_acc_scr = accuracy_score(test_lbl_np, test_preds_idxs)
    print("Testing Accuracy: {}".format(test_acc_scr))

    if out_info_file is not None:
        out_info = dict()
        out_info["params"] = params
        out_info["test_accuracy"] = test_acc_scr
        rsgislib.tools.utils.write_dict_to_json(out_info, out_info_file)


def apply_lightgbm_multiclass_classifier(
    model_file: str,
    cls_info_dict: Dict[str, rsgislib.classification.ClassInfoObj],
    in_msk_img: str,
    img_msk_val: int,
    img_file_info: List[rsgislib.imageutils.ImageBandInfo],
    out_class_img: str,
    gdalformat: str = "KEA",
    class_clr_names: bool = True,
):
    """
    A function for applying a trained multiclass lightgbm model to a image or
    stack of image files.

    :param model_file: a trained lightgbm multiclass model which can be loaded
                       with lgb.Booster(model_file=model_file).
    :param cls_info_dict: a dict where the key is string with class name
                          of ClassInfoObj objects defining the training data.
                          This is used to define the class names and colours
                          if class_clr_names is True.
    :param in_msk_img: is an image file providing a mask to specify where
                       should be classified. Simplest mask is all the valid
                       data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_msk_val: the pixel value within the in_msk_img to limit the region
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

    """
    if not HAVE_LIGHTGBM:
        raise rsgislib.RSGISPyException("Do not have lightgbm module installed.")

    def _applyLGMClassifier(info, inputs, outputs, otherargs):
        out_class_vals = numpy.zeros(
            (otherargs.n_classes, inputs.imageMask.shape[1], inputs.imageMask.shape[2]),
            dtype=numpy.uint16,
        )
        out_class_id_vals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            n_pxls = inputs.imageMask.shape[1] * inputs.imageMask.shape[2]
            out_class_vals = out_class_vals.reshape((n_pxls, otherargs.n_classes))
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
            pred_class_probs = numpy.around(
                otherargs.classifier.predict(class_vars) * 10000
            )
            preds_idxs = numpy.argmax(pred_class_probs, axis=1)
            if otherargs.n_classes != pred_class_probs.shape[1]:
                raise rsgislib.RSGISPyException(
                    "The number of classes expected and the number provided by the classifier do not match."
                )
            out_class_vals[id_arr] = pred_class_probs
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

    classifier = lgb.Booster(model_file=model_file)

    infiles = applier.FilenameAssociations()
    infiles.imageMask = in_msk_img
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
    otherargs.mskVal = img_msk_val
    otherargs.numClassVars = num_class_vars
    otherargs.imgFileInfo = img_file_info
    otherargs.n_classes = n_classes
    otherargs.cls_id_lut = cls_id_lut

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

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
        rat_dataset = gdal.Open(out_class_img, gdal.GA_Update)
        red = rat.readColumn(rat_dataset, "Red")
        green = rat.readColumn(rat_dataset, "Green")
        blue = rat.readColumn(rat_dataset, "Blue")
        class_names = numpy.empty_like(red, dtype=numpy.dtype("a255"))
        class_names[...] = ""

        for class_key in cls_info_dict:
            print("Apply Colour to class '" + class_key + "'")
            red[cls_info_dict[class_key].out_id] = cls_info_dict[class_key].red
            green[cls_info_dict[class_key].out_id] = cls_info_dict[class_key].green
            blue[cls_info_dict[class_key].out_id] = cls_info_dict[class_key].blue
            class_names[cls_info_dict[class_key].out_id] = class_key

        rat.writeColumn(rat_dataset, "Red", red)
        rat.writeColumn(rat_dataset, "Green", green)
        rat.writeColumn(rat_dataset, "Blue", blue)
        rat.writeColumn(rat_dataset, "class_names", class_names)
        rat_dataset = None
