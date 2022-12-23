#!/usr/bin/env python
"""
Tools for developing and applying regression models.

"""

import rsgislib


def get_regression_stats(ref_data, pred_data, n_vars=1):
    """
    A function which calculates a set of accuracy metrics using a set of
    reference and predicted values.

    if n_vars == 1, then ref_data and pred_data can be flat arrays.

    :param ref_data: a numpy array of n x m (m = n_vars) with the reference values.
    :param pred_data: a numpy array of n x m (m = n_vars) with the predicted values.
    :param n_vars: the number of variables to be used.
    :return: list of dicts

    """
    import numpy
    import sklearn.metrics

    import rsgislib.tools.stats

    ref_data_shp = ref_data.shape
    pred_data_shp = ref_data.shape

    if ref_data_shp[0] != pred_data_shp[0]:
        raise rsgislib.RSGISPyException(
            "The length of the reference and predicted arrays must be the same."
        )

    if n_vars == 1:
        if len(ref_data_shp) == 1:
            ref_data = numpy.expand_dims(ref_data, axis=1)
        if len(pred_data_shp) == 1:
            pred_data = numpy.expand_dims(pred_data, axis=1)

    ref_data_shp = ref_data.shape
    pred_data_shp = ref_data.shape

    if ref_data_shp[1] != pred_data_shp[1]:
        raise rsgislib.RSGISPyException(
            "The number of variables for the reference and predicted arrays must be the same."
        )

    if ref_data_shp[1] != n_vars:
        raise rsgislib.RSGISPyException(
            "The number of variables for the reference and predicted arrays must be the same as the number of variables specified in n_vars"
        )

    out_acc_metrics = list()
    for i in range(n_vars):
        acc_metrics = dict()
        acc_metrics["r2"] = sklearn.metrics.r2_score(
            ref_data[..., i], pred_data[..., i]
        )
        acc_metrics[
            "explained_variance_score"
        ] = sklearn.metrics.explained_variance_score(
            ref_data[..., i], pred_data[..., i]
        )
        acc_metrics["median_absolute_error"] = sklearn.metrics.median_absolute_error(
            ref_data[..., i], pred_data[..., i]
        )
        acc_metrics["mean_absolute_error"] = sklearn.metrics.mean_absolute_error(
            ref_data[..., i], pred_data[..., i]
        )
        acc_metrics["mean_squared_error"] = sklearn.metrics.mean_squared_error(
            ref_data[..., i], pred_data[..., i]
        )
        rmse = numpy.sqrt(
            sklearn.metrics.mean_squared_error(ref_data[..., i], pred_data[..., i])
        )
        acc_metrics["root_mean_squared_error"] = rmse
        acc_metrics["norm_root_mean_squared_error"] = 100 * (
            rmse / numpy.mean(ref_data[..., i])
        )
        bias, norm_bias = rsgislib.tools.stats.bias_score(
            ref_data[..., i], pred_data[..., i]
        )
        acc_metrics["bias"] = bias
        acc_metrics["norm_bias"] = norm_bias
        (
            mse,
            bias_squared,
            variance,
            noise,
        ) = rsgislib.tools.stats.decompose_bias_variance(
            ref_data[..., i], pred_data[..., i]
        )
        acc_metrics["bias_squared"] = bias_squared
        acc_metrics["variance"] = variance
        acc_metrics["noise"] = noise
        out_acc_metrics.append(acc_metrics)

    return out_acc_metrics
