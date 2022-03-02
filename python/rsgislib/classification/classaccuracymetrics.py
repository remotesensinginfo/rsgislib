#! /usr/bin/env python
############################################################################
#  classaccuracymetrics.py
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
# Purpose:  Provide a set of functions to calculate the accuracy of a
#           classification.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 03/02/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import math
import os
from typing import List, Tuple, Dict

import numpy

import rsgislib
import rsgislib.tools.utils


def cls_quantity_accuracy(
    y_true: numpy.array, y_pred: numpy.array, cls_area: numpy.array
) -> dict:
    """
    A function to calculate quantity allocation & disagreement for a
    land cover classification. The labels must be integers from 1 - N,
    where N is the number of classes.

    :param y_true: A list or 1D numpy array of true labels.
    :param y_pred: A list or 1D numpy array of predicted labels.
    :param cls_area: A dict or 1D numpy array of area/n_pixels identified by the
                     classifier. len(cls_area) == numpy.unique(y_true).

    :return: dict with 'Quantity Disagreement (Q)',
                       'Allocation Disagreement (A)',
                       'Proportion Correct (C)',
                       'Total Disagreement (D)'.

    Reference: Pontius, R. G., Jr, & Millones, M. (2011). Death to Kappa: birth
    of quantity disagreement and allocation disagreement for accuracy assessment.
    International Journal of Remote Sensing, 32(15), 4407–4429.

    """
    from sklearn.metrics import confusion_matrix

    # check inputs:
    if not isinstance(y_true, numpy.ndarray):
        y_true = numpy.array(y_true)
    if not isinstance(y_pred, numpy.ndarray):
        y_pred = numpy.array(y_pred)
    if not isinstance(cls_area, numpy.ndarray):
        cls_area = numpy.array(cls_area)

    for arr in [y_true, y_pred, cls_area]:
        if arr.ndim != 1:
            raise SystemExit("Error: All input arrays must be one dimensional.")

    if numpy.unique(y_true).size != cls_area.size:
        raise SystemExit("Error: Number of classes != Number of classes in area.")

    # create confusion matrix:
    cm = confusion_matrix(y_true, y_pred)

    # convert absolute areas into proportional areas:
    prop_area = (cls_area / cls_area.sum()).reshape(
        -1, 1
    )  # same as Comparison Total (see Ref.)

    # normalise the confusion matrix by proportional area:
    norm_cm = (
        cm.astype(float)
        / cm.sum(axis=1)[
            :,
        ].reshape(-1, 1)
    )
    norm_cm = norm_cm * prop_area
    comp_total = norm_cm.sum(axis=1)  # same as proportional area
    ref_total = norm_cm.sum(axis=0)

    quantity_disagreement = sum(numpy.abs(ref_total - comp_total)) / 2
    commission = [(row.sum() - row[idx]) for idx, row in enumerate(norm_cm)]
    omission = ref_total - numpy.diag(norm_cm)
    allocation_disagreement = (
        sum(2 * numpy.min(numpy.array([commission, omission]), axis=0)) / 2
    )
    prop_correct = sum(numpy.diag(norm_cm)) / numpy.sum(norm_cm)
    disagreement = quantity_disagreement + allocation_disagreement

    out_dict = dict()
    out_dict["Quantity Disagreement (Q)"] = quantity_disagreement
    out_dict["Allocation Disagreement (A)"] = allocation_disagreement
    out_dict["Proportion Correct (C)"] = prop_correct
    out_dict["Total Disagreement (D)"] = disagreement

    return out_dict


def calc_class_accuracy_metrics(
    ref_samples: numpy.array,
    pred_samples: numpy.array,
    cls_area: numpy.array,
    cls_names: numpy.array,
) -> dict:
    """
    A function which calculates a set of classification accuracy metrics for a set
    of reference and predicted samples. the area classified for each
    class is used to allow further metrics to be calculated.

    :param ref_samples: a 1d array of reference samples represented by a
                        numeric class id
    :param pred_samples: a 1d array of predicted samples represented by a
                         numeric class id
    :param cls_area: a 1d array with the area of each class classified
                    (i.e., pixel count)
    :param cls_names: a 1d list of the class names (labels) in the order of
                      the class ids.
    :return: dict with classification accuracy metrics

    """
    import sklearn.metrics

    cls_names.sort()

    acc_metrics = sklearn.metrics.classification_report(
        ref_samples, pred_samples, target_names=cls_names, output_dict=True
    )

    cohen_kappa = sklearn.metrics.cohen_kappa_score(ref_samples, pred_samples)
    acc_metrics["cohen_kappa"] = cohen_kappa

    # Calculate weighted f1-score using area mapped
    sum_area = 0.0
    sum_f1 = 0.0
    sum_recall = 0.0
    sum_precision = 0.0
    for clsname, clsarea in zip(cls_names, cls_area):
        sum_area += clsarea
        sum_f1 += acc_metrics[clsname]["f1-score"] * clsarea
        sum_recall += acc_metrics[clsname]["recall"] * clsarea
        sum_precision += acc_metrics[clsname]["precision"] * clsarea

    weighted_area_f1 = sum_f1 / sum_area
    recall_area_f1 = sum_recall / sum_area
    precision_area_f1 = sum_precision / sum_area

    acc_metrics["weighted area avg"] = {
        "precision": precision_area_f1,
        "recall": recall_area_f1,
        "f1-score": weighted_area_f1,
        "support": sum_area,
    }

    cm = sklearn.metrics.confusion_matrix(ref_samples, pred_samples)
    user_accuracy = [(row[idx] / row.sum()) * 100 for idx, row in enumerate(cm)]
    producer_accuracy = [(col[idx] / col.sum()) * 100 for idx, col in enumerate(cm.T)]
    cls_conf_intervals = dict()
    conf_int_consts = [1.64, 1.96, 2.33, 2.58]
    sum_all_smpls = 0
    for idx, col in enumerate(cm.T):
        cls_acc = producer_accuracy[idx] / 100
        cls_err = 1 - cls_acc
        sum_all_smpls += col.sum()
        conf_int_part1 = math.sqrt((cls_acc * cls_err) / col.sum())
        cls_conf_interval = []
        for conf_int_const in conf_int_consts:
            cls_conf_interval.append(conf_int_const * conf_int_part1)
        cls_conf_intervals[cls_names[idx]] = cls_conf_interval

    acc_metrics["cls_confidence_intervals"] = cls_conf_intervals

    overall_acc = acc_metrics["accuracy"]
    overall_err = 1 - overall_acc
    conf_int_part1 = math.sqrt((overall_acc * overall_err) / sum_all_smpls)
    overall_acc_conf_interval = []
    for conf_int_const in conf_int_consts:
        overall_acc_conf_interval.append(conf_int_const * conf_int_part1)
    acc_metrics["accuracy_conf_interval"] = overall_acc_conf_interval

    # convert absolute areas into proportional areas:
    prop_area = (cls_area / cls_area.sum()).reshape(
        -1, 1
    )  # same as Comparison Total (see Ref.)
    # normalise the confusion matrix by proportional area:
    norm_cm = (
        cm.astype(float)
        / cm.sum(axis=1)[
            :,
        ].reshape(-1, 1)
    )
    norm_cm = norm_cm * prop_area
    comp_total = norm_cm.sum(axis=1)  # same as proportional area
    ref_total = norm_cm.sum(axis=0)
    commission = [(row.sum() - row[idx]) for idx, row in enumerate(norm_cm)]
    omission = ref_total - numpy.diag(norm_cm)
    # Sum the normalised cm columns to estimate the proportion of scene for each class.
    cls_area_prop = numpy.sum(norm_cm, axis=0)

    acc_metrics["confusion_matrix"] = cm.tolist()
    acc_metrics["user_accuracy"] = user_accuracy
    acc_metrics["producer_accuracy"] = producer_accuracy

    acc_metrics["norm_confusion_matrix"] = norm_cm.tolist()
    acc_metrics["commission"] = commission
    acc_metrics["omission"] = omission.tolist()
    acc_metrics["est_prop_cls_area"] = cls_area_prop.tolist()

    quantity_metrics = cls_quantity_accuracy(ref_samples, pred_samples, cls_area)
    acc_metrics["quantity_metrics"] = quantity_metrics

    overall_c = quantity_metrics["Proportion Correct (C)"]
    overall_d = quantity_metrics["Total Disagreement (D)"]
    c_conf_int_part1 = math.sqrt((overall_c * overall_d) / sum_all_smpls)
    overall_quantity_conf_interval = []
    for conf_int_const in conf_int_consts:
        overall_quantity_conf_interval.append(conf_int_const * c_conf_int_part1)
    acc_metrics["quantity_metrics"]["C Conf Interval"] = overall_quantity_conf_interval

    return acc_metrics


def calc_class_pt_accuracy_metrics(
    ref_samples: numpy.array, pred_samples: numpy.array, cls_names: numpy.array
) -> dict:
    """
    A function which calculates a set of classification accuracy metrics for a set
    of reference and predicted samples.

    :param ref_samples: a 1d array of reference samples represented by a
                        numeric class id
    :param pred_samples: a 1d array of predicted samples represented by a
                         numeric class id
    :param cls_names: a 1d list of the class names (labels) in the order of
                      the class ids.
    :return: dict with classification accuracy metrics

    """
    import sklearn.metrics

    cls_names.sort()

    acc_metrics = sklearn.metrics.classification_report(
        ref_samples, pred_samples, target_names=cls_names, output_dict=True
    )

    cohen_kappa = sklearn.metrics.cohen_kappa_score(ref_samples, pred_samples)
    acc_metrics["cohen_kappa"] = cohen_kappa

    cm = sklearn.metrics.confusion_matrix(ref_samples, pred_samples)
    user_accuracy = [(row[idx] / row.sum()) * 100 for idx, row in enumerate(cm)]
    producer_accuracy = [(col[idx] / col.sum()) * 100 for idx, col in enumerate(cm.T)]
    cls_conf_intervals = dict()
    conf_int_consts = [1.64, 1.96, 2.33, 2.58]
    sum_all_smpls = 0
    for idx, col in enumerate(cm.T):
        cls_acc = producer_accuracy[idx] / 100
        cls_err = 1 - cls_acc
        sum_all_smpls += col.sum()
        conf_int_part1 = math.sqrt((cls_acc * cls_err) / col.sum())
        cls_conf_interval = []
        for conf_int_const in conf_int_consts:
            cls_conf_interval.append(conf_int_const * conf_int_part1)
        cls_conf_intervals[cls_names[idx]] = cls_conf_interval

    acc_metrics["cls_confidence_intervals"] = cls_conf_intervals

    overall_acc = acc_metrics["accuracy"]
    overall_err = 1 - overall_acc
    conf_int_part1 = math.sqrt((overall_acc * overall_err) / sum_all_smpls)
    overall_acc_conf_interval = []
    for conf_int_const in conf_int_consts:
        overall_acc_conf_interval.append(conf_int_const * conf_int_part1)
    acc_metrics["accuracy_conf_interval"] = overall_acc_conf_interval

    acc_metrics["confusion_matrix"] = cm.tolist()
    acc_metrics["user_accuracy"] = user_accuracy
    acc_metrics["producer_accuracy"] = producer_accuracy

    return acc_metrics


def calc_acc_metrics_vecsamples(
    vec_file: str,
    vec_lyr: str,
    ref_col: str,
    cls_col: str,
    cls_img: str,
    img_cls_name_col: str = "ClassName",
    img_hist_col: str = "Histogram",
    out_json_file: str = None,
    out_csv_file: str = None,
):
    """
    A function which calculates classification accuracy metrics using a set of
    reference samples in a vector file and the classification image defining
    the area classified.
    This would be often be used alongside the ClassAccuracy QGIS plugin.

    :param vec_file: the input vector file with the reference points
    :param vec_lyr: the input vector layer name with the reference points.
    :param ref_col: the name of the reference classification column in the
                    input vector file.
    :param cls_col: the name of the classification column in the input vector file.
    :param cls_img: an image of the classification from which the area
                    (pixel counts) of each class are extracted to normalise the
                    confusion matrix. Should have a RAT with class names and histogram.
    :param img_cls_name_col: The name of the column in the image attribute table which
                             specifies the class name.
    :param img_hist_col: The name of the column in the image attribute table which
                         contains the histogram (i.e., number of pixels within
                         the class).
    :param out_json_file: if specified the generated metrics and confusion matrix
                          are written to a JSON file (Default=None).
    :param out_csv_file: if specified the generated metrics and confusion matrix
                         are written to a CSV file (Default=None).

    Example:

        import rsgislib
        from rsgislib.classification import classaccuracymetrics

        vec_file = "Sonoma_county_classification_refPoints.gpkg"
        vec_lyr = "ref_points"
        ref_col = "reference_classes"
        cls_col = "classes"
        cls_img = "Sonoma_county_Landsat8_2015_utm_RandomForest.kea"
        img_cls_name_col = "RF_classes"
        img_hist_col = "Histogram"
        out_json_file = "Sonoma_county_class_acc_metrics.json"

        classaccuracymetrics.calc_acc_metrics_vecsamples(in_vec_file, in_vec_lyr,
                                                         ref_col, cls_col, cls_img,
                                                         img_cls_name_col, img_hist_col,
                                                         out_json_file)

    """
    import rsgislib.tools.utils
    import rsgislib.vectorattrs
    import rsgislib.rastergis
    import rsgislib.imageutils

    # Read columns from vector file.
    ref_vals = numpy.array(
        rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, ref_col)
    )
    cls_vals = numpy.array(
        rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, cls_col)
    )

    # Find unique class values
    unq_cls_names = numpy.unique(
        numpy.concatenate((numpy.unique(ref_vals), numpy.unique(cls_vals)))
    )
    unq_cls_names.sort()

    # Create LUTs assigning each class a unique int ID.
    cls_name_lut = dict()
    cls_id_lut = dict()
    for cls_id, cls_name in enumerate(unq_cls_names):
        cls_name_lut[cls_name] = cls_id
        cls_id_lut[cls_id] = cls_name

    # Create cls_id arrays
    ref_int_vals = numpy.zeros_like(ref_vals, dtype=int)
    cls_int_vals = numpy.zeros_like(cls_vals, dtype=int)
    for cls_name in unq_cls_names:
        ref_int_vals[ref_vals == cls_name] = cls_name_lut[cls_name]
        cls_int_vals[cls_vals == cls_name] = cls_name_lut[cls_name]

    try:
        rat_cols = rsgislib.rastergis.get_rat_columns(cls_img)
    except:
        raise rsgislib.RSGISPyException("The input image does not have a RAT...")

    if img_cls_name_col not in rat_cols:
        raise rsgislib.RSGISPyException(
            "The RAT does not contain the class name column specified ('{}')".format(
                img_cls_name_col
            )
        )
    if img_hist_col not in rat_cols:
        raise rsgislib.RSGISPyException(
            "The RAT does not contain the histogram column specified ('{}')".format(
                img_hist_col
            )
        )

    img_hist_data = rsgislib.rastergis.get_column_data(cls_img, img_hist_col)
    img_clsname_data = rsgislib.rastergis.get_column_data(cls_img, img_cls_name_col)
    img_clsname_data[0] = ""

    pxl_size_x, pxl_size_y = rsgislib.imageutils.get_img_res(cls_img, abs_vals=True)
    pxl_area = pxl_size_x * pxl_size_y

    # Find the class areas (pixel counts)
    cls_pxl_count_dict = dict()
    cls_area_dict = dict()
    tot_area = 0.0
    cls_pxl_counts = numpy.zeros_like(unq_cls_names, dtype=int)
    for i, cls_name in enumerate(img_clsname_data):
        cls_name_str = str(cls_name.decode())
        cls_name_str = rsgislib.tools.utils.check_str(cls_name_str, rm_non_ascii=True)
        if (i > 0) and (len(cls_name_str) > 0):
            if cls_name_str not in unq_cls_names:
                raise rsgislib.RSGISPyException(
                    "Class ('{}') found in image which was "
                    "not in point samples...".format(cls_name_str)
                )
            cls_pxl_counts[cls_name_lut[cls_name_str]] = img_hist_data[i]
            cls_pxl_count_dict[cls_name_str] = img_hist_data[i]
            cls_area_dict[cls_name_str] = img_hist_data[i] * pxl_area
            tot_area = tot_area + (img_hist_data[i] * pxl_area)

    acc_metrics = calc_class_accuracy_metrics(
        ref_int_vals, cls_int_vals, cls_pxl_counts, unq_cls_names
    )

    acc_metrics["pixel_count"] = cls_pxl_count_dict
    acc_metrics["pixel_area"] = cls_area_dict

    if out_json_file is not None:
        import rsgislib.tools.utils

        rsgislib.tools.utils.write_dict_to_json(acc_metrics, out_json_file)

    if out_csv_file is not None:
        import csv

        with open(out_csv_file, mode="w") as out_csv_file_obj:
            acc_metrics_writer = csv.writer(
                out_csv_file_obj,
                delimiter=",",
                quotechar='"',
                quoting=csv.QUOTE_MINIMAL,
            )

            # Overall Accuracy
            acc_metrics_writer.writerow(["overall accuracy", acc_metrics["accuracy"]])
            acc_metrics_writer.writerow(["cohen kappa", acc_metrics["cohen_kappa"]])
            acc_metrics_writer.writerow([""])

            # Overall Accuracy Confidence Intervals
            acc_metrics_writer.writerow(
                ["Confidence Interval", "90%", "95%", "98%", "99%"]
            )
            overall_acc_conf_out = ["Overall Accuracy"]
            for conf_interval in acc_metrics["accuracy_conf_interval"]:
                overall_acc_conf_out.append(conf_interval)
            acc_metrics_writer.writerow(overall_acc_conf_out)
            acc_metrics_writer.writerow([""])

            # Quantity Metrics
            acc_metrics_writer.writerow(
                [
                    "Allocation Disagreement (A)",
                    acc_metrics["quantity_metrics"]["Allocation Disagreement (A)"],
                ]
            )
            acc_metrics_writer.writerow(
                [
                    "Quantity Disagreement (Q)",
                    acc_metrics["quantity_metrics"]["Quantity Disagreement (Q)"],
                ]
            )
            acc_metrics_writer.writerow(
                [
                    "Proportion Correct (C)",
                    acc_metrics["quantity_metrics"]["Proportion Correct (C)"],
                ]
            )
            acc_metrics_writer.writerow(
                [
                    "Total Disagreement (D)",
                    acc_metrics["quantity_metrics"]["Total Disagreement (D)"],
                ]
            )
            acc_metrics_writer.writerow([""])

            # Area Normalised Overall Accuracy Confidence Intervals
            acc_metrics_writer.writerow(
                ["Confidence Interval", "90%", "95%", "98%", "99%"]
            )
            overall_acc_conf_out = ["Area Norm Accuracy"]
            for conf_interval in acc_metrics["quantity_metrics"]["C Conf Interval"]:
                overall_acc_conf_out.append(conf_interval)
            acc_metrics_writer.writerow(overall_acc_conf_out)
            acc_metrics_writer.writerow([""])

            # Individual Class Scores
            acc_metrics_writer.writerow(
                ["class", "f1-score", "precision", "recall", "support"]
            )
            for cls_name in unq_cls_names:
                acc_metrics_writer.writerow(
                    [
                        cls_name,
                        acc_metrics[cls_name]["f1-score"],
                        acc_metrics[cls_name]["precision"],
                        acc_metrics[cls_name]["recall"],
                        acc_metrics[cls_name]["support"],
                    ]
                )
            # Overall macro and weighted
            acc_metrics_writer.writerow([""])
            acc_metrics_writer.writerow(
                [
                    "macro avg",
                    acc_metrics["macro avg"]["f1-score"],
                    acc_metrics["macro avg"]["precision"],
                    acc_metrics["macro avg"]["recall"],
                    acc_metrics["macro avg"]["support"],
                ]
            )
            acc_metrics_writer.writerow(
                [
                    "weighted (pixel) avg",
                    acc_metrics["weighted avg"]["f1-score"],
                    acc_metrics["weighted avg"]["precision"],
                    acc_metrics["weighted avg"]["recall"],
                    acc_metrics["weighted avg"]["support"],
                ]
            )
            acc_metrics_writer.writerow(
                [
                    "weighted (area) avg",
                    acc_metrics["weighted area avg"]["f1-score"],
                    acc_metrics["weighted area avg"]["precision"],
                    acc_metrics["weighted area avg"]["recall"],
                    acc_metrics["weighted area avg"]["support"],
                ]
            )
            acc_metrics_writer.writerow([""])

            acc_metrics_writer.writerow(
                ["Confidence Interval", "90%", "95%", "98%", "99%"]
            )
            for cls_name in unq_cls_names:
                cls_acc_conf_out = [cls_name]
                for conf_interval in acc_metrics["cls_confidence_intervals"][cls_name]:
                    cls_acc_conf_out.append(conf_interval)
                acc_metrics_writer.writerow(cls_acc_conf_out)
            acc_metrics_writer.writerow([""])

            # Output the confusion matrix
            acc_metrics_writer.writerow(["Point Count Confusion Matrix"])
            cm_top_row = [""]
            for cls_name in unq_cls_names:
                cm_top_row.append(cls_name)
            cm_top_row.append("User Acc")
            acc_metrics_writer.writerow(cm_top_row)
            for cls_name, cm_row, user_acc in zip(
                unq_cls_names,
                acc_metrics["confusion_matrix"],
                acc_metrics["user_accuracy"],
            ):
                row = [cls_name]
                for val in cm_row:
                    row.append(val)
                row.append(user_acc)
                acc_metrics_writer.writerow(row)
            cm_bot_row = ["Producer"]
            for prod_val in acc_metrics["producer_accuracy"]:
                cm_bot_row.append(prod_val)
            acc_metrics_writer.writerow(cm_bot_row)
            acc_metrics_writer.writerow([""])

            acc_metrics_writer.writerow(["Normalised Confusion Matrix"])
            # Output the normalised confusion matrix
            cm_top_row = [""]
            for cls_name in unq_cls_names:
                cm_top_row.append(cls_name)
            acc_metrics_writer.writerow(cm_top_row)
            for cls_name, cm_row in zip(
                unq_cls_names, acc_metrics["norm_confusion_matrix"]
            ):
                row = [cls_name]
                for val in cm_row:
                    row.append(val)
                acc_metrics_writer.writerow(row)

            acc_metrics_writer.writerow([""])
            acc_metrics_writer.writerow(["class", "commission", "omission"])
            for i, cls_name in enumerate(unq_cls_names):
                acc_metrics_writer.writerow(
                    [
                        cls_name,
                        acc_metrics["commission"][i],
                        acc_metrics["omission"][i],
                    ]
                )

            acc_metrics_writer.writerow([""])
            acc_metrics_writer.writerow(
                ["class", "pixel count", "pixel area", "Est. Prop. Area", "Est. Area"]
            )
            for i, cls_name in enumerate(unq_cls_names):
                acc_metrics_writer.writerow(
                    [
                        cls_name,
                        cls_pxl_count_dict[cls_name],
                        cls_area_dict[cls_name],
                        acc_metrics["est_prop_cls_area"][i],
                        (tot_area * acc_metrics["est_prop_cls_area"][i]),
                    ]
                )

    if (out_json_file is None) and (out_csv_file is None):
        import pprint

        pprint.pprint(acc_metrics)


def calc_acc_ptonly_metrics_vecsamples(
    vec_file: str,
    vec_lyr: str,
    ref_col: str,
    cls_col: str,
    out_json_file: str = None,
    out_csv_file: str = None,
):
    """
    A function which calculates classification accuracy metrics using a set of
    reference samples in a vector file.
    This would be often be used alongside the ClassAccuracy QGIS plugin.

    :param vec_file: the input vector file with the reference points
    :param vec_lyr: the input vector layer name with the reference points.
    :param ref_col: the name of the reference classification column in the input
                    vector file.
    :param cls_col: the name of the classification column in the input vector file.
    :param out_json_file: if specified the generated metrics and confusion matrix
                          are written to a JSON file (Default=None).
    :param out_csv_file: if specified the generated metrics and confusion matrix
                         are written to a CSV file (Default=None).

    .. code:: python

        vec_file = "Sonoma_county_classification_refPoints.gpkg"
        vec_lyr = "ref_points"
        ref_col = "reference_classes"
        cls_col = "classes"
        out_json_file = "Sonoma_county_class_acc_metrics.json"

        import rsgislib
        from rsgislib.classification import classaccuracymetrics

        classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples(vec_file, vec_lyr,
                                                                ref_col, cls_col,
                                                                out_json_file=None,
                                                                out_csv_file=None)

    """
    import rsgislib.vectorattrs

    # Read columns from vector file.
    ref_vals = numpy.array(
        rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, ref_col)
    )
    cls_vals = numpy.array(
        rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, cls_col)
    )

    # Find unique class values
    unq_cls_names = numpy.unique(
        numpy.concatenate((numpy.unique(ref_vals), numpy.unique(cls_vals)))
    )
    unq_cls_names.sort()

    # Create LUTs assigning each class a unique int ID.
    cls_name_lut = dict()
    cls_id_lut = dict()
    for cls_id, cls_name in enumerate(unq_cls_names):
        cls_name_lut[cls_name] = cls_id
        cls_id_lut[cls_id] = cls_name

    # Create cls_id arrays
    ref_int_vals = numpy.zeros_like(ref_vals, dtype=int)
    cls_int_vals = numpy.zeros_like(cls_vals, dtype=int)
    for cls_name in unq_cls_names:
        ref_int_vals[ref_vals == cls_name] = cls_name_lut[cls_name]
        cls_int_vals[cls_vals == cls_name] = cls_name_lut[cls_name]

    acc_metrics = calc_class_pt_accuracy_metrics(
        ref_int_vals, cls_int_vals, unq_cls_names
    )

    if out_json_file is not None:
        import rsgislib.tools.utils

        rsgislib.tools.utils.write_dict_to_json(acc_metrics, out_json_file)

    if out_csv_file is not None:
        import csv

        with open(out_csv_file, mode="w") as out_csv_file_obj:
            acc_metrics_writer = csv.writer(
                out_csv_file_obj,
                delimiter=",",
                quotechar='"',
                quoting=csv.QUOTE_MINIMAL,
            )

            # Overall Accuracy
            acc_metrics_writer.writerow(["overall accuracy", acc_metrics["accuracy"]])
            acc_metrics_writer.writerow(["cohen kappa", acc_metrics["cohen_kappa"]])
            acc_metrics_writer.writerow([""])

            # Overall Accuracy Confidence Intervals
            acc_metrics_writer.writerow(
                ["Confidence Interval", "90%", "95%", "98%", "99%"]
            )
            overall_acc_conf_out = ["Overall Accuracy"]
            for conf_interval in acc_metrics["accuracy_conf_interval"]:
                overall_acc_conf_out.append(conf_interval)
            acc_metrics_writer.writerow(overall_acc_conf_out)
            acc_metrics_writer.writerow([""])

            # Individual Class Scores
            acc_metrics_writer.writerow(
                ["class", "f1-score", "precision", "recall", "support"]
            )
            for cls_name in unq_cls_names:
                acc_metrics_writer.writerow(
                    [
                        cls_name,
                        acc_metrics[cls_name]["f1-score"],
                        acc_metrics[cls_name]["precision"],
                        acc_metrics[cls_name]["recall"],
                        acc_metrics[cls_name]["support"],
                    ]
                )
            # Overall macro and weighted
            acc_metrics_writer.writerow([""])
            acc_metrics_writer.writerow(
                [
                    "macro avg",
                    acc_metrics["macro avg"]["f1-score"],
                    acc_metrics["macro avg"]["precision"],
                    acc_metrics["macro avg"]["recall"],
                    acc_metrics["macro avg"]["support"],
                ]
            )
            acc_metrics_writer.writerow(
                [
                    "weighted (pixel) avg",
                    acc_metrics["weighted avg"]["f1-score"],
                    acc_metrics["weighted avg"]["precision"],
                    acc_metrics["weighted avg"]["recall"],
                    acc_metrics["weighted avg"]["support"],
                ]
            )
            acc_metrics_writer.writerow([""])

            acc_metrics_writer.writerow(
                ["Confidence Interval", "90%", "95%", "98%", "99%"]
            )
            for cls_name in unq_cls_names:
                cls_acc_conf_out = [cls_name]
                for conf_interval in acc_metrics["cls_confidence_intervals"][cls_name]:
                    cls_acc_conf_out.append(conf_interval)
                acc_metrics_writer.writerow(cls_acc_conf_out)
            acc_metrics_writer.writerow([""])

            # Output the confusion matrix
            acc_metrics_writer.writerow(["Point Count Confusion Matrix"])
            cm_top_row = [""]
            for cls_name in unq_cls_names:
                cm_top_row.append(cls_name)
            cm_top_row.append("User Acc")
            acc_metrics_writer.writerow(cm_top_row)
            for cls_name, cm_row, user_acc in zip(
                unq_cls_names,
                acc_metrics["confusion_matrix"],
                acc_metrics["user_accuracy"],
            ):
                row = [cls_name]
                for val in cm_row:
                    row.append(val)
                row.append(user_acc)
                acc_metrics_writer.writerow(row)
            cm_bot_row = ["Producer"]
            for prod_val in acc_metrics["producer_accuracy"]:
                cm_bot_row.append(prod_val)
            acc_metrics_writer.writerow(cm_bot_row)
            acc_metrics_writer.writerow([""])

    if (out_json_file is None) and (out_csv_file is None):
        import pprint

        pprint.pprint(acc_metrics)


def calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval(
    vec_file: str,
    vec_lyr: str,
    ref_col: str,
    cls_col: str,
    out_json_file: str = None,
    sample_frac: float = 0.2,
    sample_n_smps: int = None,
    bootstrap_n: int = 1000,
) -> dict:
    """
    A function which calculates classification accuracy metric confidence intervals
    using a bootstrapping approach. This function uses a set of reference samples in
    a vector file and would be often be used alongside the ClassAccuracy QGIS plugin.

    :param vec_file: the input vector file with the reference points
    :param vec_lyr: the input vector layer name with the reference points.
    :param ref_col: the name of the reference classification column in the input
                    vector file.
    :param cls_col: the name of the classification column in the input vector file.
    :param out_json_file: if specified the generated metrics and confusion matrix
                          are written to a JSON file (Default=None).
    :param sample_frac: The fraction of the whole dataset selected for each
                        bootstrap iteration.
    :param bootstrap_n: The number of bootstrap iterations.
    :return: dict with mean/median and bootstrap intervals.

    """
    import tqdm
    import numpy.random
    import rsgislib.vectorattrs

    # Read columns from vector file.
    ref_vals = numpy.array(
        rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, ref_col)
    )
    cls_vals = numpy.array(
        rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, cls_col)
    )

    # Find unique class values
    unq_cls_names = numpy.unique(
        numpy.concatenate((numpy.unique(ref_vals), numpy.unique(cls_vals)))
    )
    # sort()

    # Create LUTs assigning each class a unique int ID.
    cls_name_lut = dict()
    cls_id_lut = dict()
    for cls_id, cls_name in enumerate(unq_cls_names):
        cls_name_lut[cls_name] = cls_id
        cls_id_lut[cls_id] = cls_name

    # Create cls_id arrays
    ref_int_vals = numpy.zeros_like(ref_vals, dtype=int)
    cls_int_vals = numpy.zeros_like(cls_vals, dtype=int)
    for cls_name in unq_cls_names:
        ref_int_vals[ref_vals == cls_name] = cls_name_lut[cls_name]
        cls_int_vals[cls_vals == cls_name] = cls_name_lut[cls_name]

    if sample_n_smps is None:
        n_samples = int(ref_int_vals.shape[0] * sample_frac)
    elif sample_n_smps < ref_int_vals.shape[0]:
        n_samples = sample_n_smps
    else:
        n_samples = ref_int_vals.shape[0]
    smp_idx = numpy.arange(0, ref_int_vals.shape[0], dtype=int)

    acc_metrics_runs = list()

    for i in tqdm.tqdm(range(bootstrap_n)):
        bs_sel_idx = numpy.random.choice(smp_idx, n_samples, replace=True)
        ref_int_vals_smpls = ref_int_vals[bs_sel_idx]
        cls_int_vals_smpls = cls_int_vals[bs_sel_idx]
        acc_metrics_runs.append(
            calc_class_pt_accuracy_metrics(
                ref_int_vals_smpls, cls_int_vals_smpls, unq_cls_names
            )
        )

    accuracy_scores = list()
    cohen_kappa_scores = list()
    macro_avg_f1_scores = list()
    macro_avg_precision = list()
    macro_avg_recall = list()
    weighted_avg_f1_scores = list()
    weighted_avg_precision = list()
    weighted_avg_recall = list()
    cls_stats = dict()
    for cls in unq_cls_names:
        cls_stats[cls] = dict()
        cls_stats[cls]["f1-score"] = list()
        cls_stats[cls]["precision"] = list()
        cls_stats[cls]["recall"] = list()
        cls_stats[cls]["support"] = list()
        cls_stats[cls]["producer_accuracy"] = list()
        cls_stats[cls]["user_accuracy"] = list()

    for acc_metrics in acc_metrics_runs:
        accuracy_scores.append(acc_metrics["accuracy"])
        cohen_kappa_scores.append(acc_metrics["cohen_kappa"])
        macro_avg_f1_scores.append(acc_metrics["macro avg"]["f1-score"])
        macro_avg_precision.append(acc_metrics["macro avg"]["precision"])
        macro_avg_recall.append(acc_metrics["macro avg"]["recall"])
        weighted_avg_f1_scores.append(acc_metrics["weighted avg"]["f1-score"])
        weighted_avg_precision.append(acc_metrics["weighted avg"]["precision"])
        weighted_avg_recall.append(acc_metrics["weighted avg"]["recall"])

        for i, cls in enumerate(unq_cls_names):
            cls_stats[cls]["f1-score"].append(acc_metrics[cls]["f1-score"])
            cls_stats[cls]["precision"].append(acc_metrics[cls]["precision"])
            cls_stats[cls]["recall"].append(acc_metrics[cls]["recall"])
            cls_stats[cls]["support"].append(acc_metrics[cls]["support"])
            cls_stats[cls]["producer_accuracy"].append(
                acc_metrics["producer_accuracy"][i]
            )
            cls_stats[cls]["user_accuracy"].append(acc_metrics["user_accuracy"][i])

    conf_inters = [0.5, 2.5, 5, 50, 95, 97.5, 99.5]

    accuracy_scores_inters = numpy.percentile(accuracy_scores, conf_inters)
    accuracy_scores_mean = numpy.mean(accuracy_scores)

    cohen_kappa_scores_inters = numpy.percentile(cohen_kappa_scores, conf_inters)
    cohen_kappa_scores_mean = numpy.mean(cohen_kappa_scores)

    macro_avg_f1_scores_inters = numpy.percentile(macro_avg_f1_scores, conf_inters)
    macro_avg_f1_scores_mean = numpy.mean(macro_avg_f1_scores)

    macro_avg_precision_inters = numpy.percentile(macro_avg_precision, conf_inters)
    macro_avg_precision_mean = numpy.mean(macro_avg_precision)

    macro_avg_recall_inters = numpy.percentile(macro_avg_recall, conf_inters)
    macro_avg_recall_mean = numpy.mean(macro_avg_recall)

    weighted_avg_f1_scores_inters = numpy.percentile(
        weighted_avg_f1_scores, conf_inters
    )
    weighted_avg_f1_scores_mean = numpy.mean(weighted_avg_f1_scores)

    weighted_avg_precision_inters = numpy.percentile(
        weighted_avg_precision, conf_inters
    )
    weighted_avg_precision_mean = numpy.mean(weighted_avg_precision)

    weighted_avg_recall_inters = numpy.percentile(weighted_avg_recall, conf_inters)
    weighted_avg_recall_mean = numpy.mean(weighted_avg_recall)

    cls_inter_stats = dict()
    for cls in unq_cls_names:
        cls_inter_stats[cls] = dict()
        cls_inter_stats[cls]["f1-score inters"] = numpy.percentile(
            cls_stats[cls]["f1-score"], conf_inters
        )
        cls_inter_stats[cls]["f1-score mean"] = numpy.mean(cls_stats[cls]["f1-score"])

        cls_inter_stats[cls]["precision inters"] = numpy.percentile(
            cls_stats[cls]["precision"], conf_inters
        )
        cls_inter_stats[cls]["precision mean"] = numpy.mean(cls_stats[cls]["precision"])

        cls_inter_stats[cls]["recall inters"] = numpy.percentile(
            cls_stats[cls]["recall"], conf_inters
        )
        cls_inter_stats[cls]["recall mean"] = numpy.mean(cls_stats[cls]["recall"])

        cls_inter_stats[cls]["support inters"] = numpy.percentile(
            cls_stats[cls]["support"], conf_inters
        )
        cls_inter_stats[cls]["support mean"] = numpy.mean(cls_stats[cls]["support"])

        cls_inter_stats[cls]["producer_accuracy inters"] = numpy.percentile(
            cls_stats[cls]["producer_accuracy"], conf_inters
        )
        cls_inter_stats[cls]["producer_accuracy mean"] = numpy.mean(
            cls_stats[cls]["producer_accuracy"]
        )

        cls_inter_stats[cls]["user_accuracy inters"] = numpy.percentile(
            cls_stats[cls]["user_accuracy"], conf_inters
        )
        cls_inter_stats[cls]["user_accuracy mean"] = numpy.mean(
            cls_stats[cls]["user_accuracy"]
        )

    out_interv_stats = dict()
    out_interv_stats["accuracy"] = dict()
    out_interv_stats["accuracy"]["mean"] = float(accuracy_scores_mean)
    out_interv_stats["accuracy"]["median"] = float(accuracy_scores_inters[3])
    out_interv_stats["accuracy"]["90th"] = [
        float(accuracy_scores_inters[2]),
        float(accuracy_scores_inters[4]),
    ]
    out_interv_stats["accuracy"]["95th"] = [
        float(accuracy_scores_inters[1]),
        float(accuracy_scores_inters[5]),
    ]
    out_interv_stats["accuracy"]["99th"] = [
        float(accuracy_scores_inters[0]),
        float(accuracy_scores_inters[6]),
    ]

    out_interv_stats["cohen_kappa"] = dict()
    out_interv_stats["cohen_kappa"]["mean"] = float(cohen_kappa_scores_mean)
    out_interv_stats["cohen_kappa"]["median"] = float(cohen_kappa_scores_inters[3])
    out_interv_stats["cohen_kappa"]["90th"] = [
        float(cohen_kappa_scores_inters[2]),
        float(cohen_kappa_scores_inters[4]),
    ]
    out_interv_stats["cohen_kappa"]["95th"] = [
        float(cohen_kappa_scores_inters[1]),
        float(cohen_kappa_scores_inters[5]),
    ]
    out_interv_stats["cohen_kappa"]["99th"] = [
        float(cohen_kappa_scores_inters[0]),
        float(cohen_kappa_scores_inters[6]),
    ]

    out_interv_stats["macro avg"] = dict()
    out_interv_stats["macro avg"]["f1-score"] = dict()
    out_interv_stats["macro avg"]["f1-score"]["mean"] = float(macro_avg_f1_scores_mean)
    out_interv_stats["macro avg"]["f1-score"]["median"] = float(
        macro_avg_f1_scores_inters[3]
    )
    out_interv_stats["macro avg"]["f1-score"]["90th"] = [
        float(macro_avg_f1_scores_inters[2]),
        float(macro_avg_f1_scores_inters[4]),
    ]
    out_interv_stats["macro avg"]["f1-score"]["95th"] = [
        float(macro_avg_f1_scores_inters[1]),
        float(macro_avg_f1_scores_inters[5]),
    ]
    out_interv_stats["macro avg"]["f1-score"]["99th"] = [
        float(macro_avg_f1_scores_inters[0]),
        float(macro_avg_f1_scores_inters[6]),
    ]

    out_interv_stats["macro avg"]["precision"] = dict()
    out_interv_stats["macro avg"]["precision"]["mean"] = float(macro_avg_precision_mean)
    out_interv_stats["macro avg"]["precision"]["median"] = float(
        macro_avg_precision_inters[3]
    )
    out_interv_stats["macro avg"]["precision"]["90th"] = [
        float(macro_avg_precision_inters[2]),
        float(macro_avg_precision_inters[4]),
    ]
    out_interv_stats["macro avg"]["precision"]["95th"] = [
        float(macro_avg_precision_inters[1]),
        float(macro_avg_precision_inters[5]),
    ]
    out_interv_stats["macro avg"]["precision"]["99th"] = [
        float(macro_avg_precision_inters[0]),
        float(macro_avg_precision_inters[6]),
    ]

    out_interv_stats["macro avg"]["recall"] = dict()
    out_interv_stats["macro avg"]["recall"]["mean"] = float(macro_avg_recall_mean)
    out_interv_stats["macro avg"]["recall"]["median"] = float(
        macro_avg_recall_inters[3]
    )
    out_interv_stats["macro avg"]["recall"]["90th"] = [
        float(macro_avg_recall_inters[2]),
        float(macro_avg_recall_inters[4]),
    ]
    out_interv_stats["macro avg"]["recall"]["95th"] = [
        float(macro_avg_recall_inters[1]),
        float(macro_avg_recall_inters[5]),
    ]
    out_interv_stats["macro avg"]["recall"]["99th"] = [
        float(macro_avg_recall_inters[0]),
        float(macro_avg_recall_inters[6]),
    ]

    out_interv_stats["weighted avg"] = dict()
    out_interv_stats["weighted avg"]["f1-score"] = dict()
    out_interv_stats["weighted avg"]["f1-score"]["mean"] = float(
        weighted_avg_f1_scores_mean
    )
    out_interv_stats["weighted avg"]["f1-score"]["median"] = float(
        weighted_avg_f1_scores_inters[3]
    )
    out_interv_stats["weighted avg"]["f1-score"]["90th"] = [
        float(weighted_avg_f1_scores_inters[2]),
        float(weighted_avg_f1_scores_inters[4]),
    ]
    out_interv_stats["weighted avg"]["f1-score"]["95th"] = [
        float(weighted_avg_f1_scores_inters[1]),
        float(weighted_avg_f1_scores_inters[5]),
    ]
    out_interv_stats["weighted avg"]["f1-score"]["99th"] = [
        float(weighted_avg_f1_scores_inters[0]),
        float(weighted_avg_f1_scores_inters[6]),
    ]

    out_interv_stats["weighted avg"]["precision"] = dict()
    out_interv_stats["weighted avg"]["precision"]["mean"] = float(
        weighted_avg_precision_mean
    )
    out_interv_stats["weighted avg"]["precision"]["median"] = float(
        weighted_avg_precision_inters[3]
    )
    out_interv_stats["weighted avg"]["precision"]["90th"] = [
        float(weighted_avg_precision_inters[2]),
        float(weighted_avg_precision_inters[4]),
    ]
    out_interv_stats["weighted avg"]["precision"]["95th"] = [
        float(weighted_avg_precision_inters[1]),
        float(weighted_avg_precision_inters[5]),
    ]
    out_interv_stats["weighted avg"]["precision"]["99th"] = [
        float(weighted_avg_precision_inters[0]),
        float(weighted_avg_precision_inters[6]),
    ]

    out_interv_stats["weighted avg"]["recall"] = dict()
    out_interv_stats["weighted avg"]["recall"]["mean"] = float(weighted_avg_recall_mean)
    out_interv_stats["weighted avg"]["recall"]["median"] = float(
        weighted_avg_recall_inters[3]
    )
    out_interv_stats["weighted avg"]["recall"]["90th"] = [
        float(weighted_avg_recall_inters[2]),
        float(weighted_avg_recall_inters[4]),
    ]
    out_interv_stats["weighted avg"]["recall"]["95th"] = [
        float(weighted_avg_recall_inters[1]),
        float(weighted_avg_recall_inters[5]),
    ]
    out_interv_stats["weighted avg"]["recall"]["99th"] = [
        float(weighted_avg_recall_inters[0]),
        float(weighted_avg_recall_inters[6]),
    ]

    for cls in unq_cls_names:
        out_interv_stats[cls] = dict()
        out_interv_stats[cls]["f1-score"] = dict()
        out_interv_stats[cls]["f1-score"]["mean"] = float(
            cls_inter_stats[cls]["f1-score mean"]
        )
        out_interv_stats[cls]["f1-score"]["median"] = float(
            cls_inter_stats[cls]["f1-score inters"][3]
        )
        out_interv_stats[cls]["f1-score"]["90th"] = [
            float(cls_inter_stats[cls]["f1-score inters"][2]),
            float(cls_inter_stats[cls]["f1-score inters"][4]),
        ]
        out_interv_stats[cls]["f1-score"]["95th"] = [
            float(cls_inter_stats[cls]["f1-score inters"][1]),
            float(cls_inter_stats[cls]["f1-score inters"][5]),
        ]
        out_interv_stats[cls]["f1-score"]["99th"] = [
            float(cls_inter_stats[cls]["f1-score inters"][0]),
            float(cls_inter_stats[cls]["f1-score inters"][6]),
        ]

        out_interv_stats[cls]["precision"] = dict()
        out_interv_stats[cls]["precision"]["mean"] = float(
            cls_inter_stats[cls]["precision mean"]
        )
        out_interv_stats[cls]["precision"]["median"] = float(
            cls_inter_stats[cls]["precision inters"][3]
        )
        out_interv_stats[cls]["precision"]["90th"] = [
            float(cls_inter_stats[cls]["precision inters"][2]),
            float(cls_inter_stats[cls]["precision inters"][4]),
        ]
        out_interv_stats[cls]["precision"]["95th"] = [
            float(cls_inter_stats[cls]["precision inters"][1]),
            float(cls_inter_stats[cls]["precision inters"][5]),
        ]
        out_interv_stats[cls]["precision"]["99th"] = [
            float(cls_inter_stats[cls]["precision inters"][0]),
            float(cls_inter_stats[cls]["precision inters"][6]),
        ]

        out_interv_stats[cls]["recall"] = dict()
        out_interv_stats[cls]["recall"]["mean"] = float(
            cls_inter_stats[cls]["recall mean"]
        )
        out_interv_stats[cls]["recall"]["median"] = float(
            cls_inter_stats[cls]["recall inters"][3]
        )
        out_interv_stats[cls]["recall"]["90th"] = [
            float(cls_inter_stats[cls]["recall inters"][2]),
            float(cls_inter_stats[cls]["recall inters"][4]),
        ]
        out_interv_stats[cls]["recall"]["95th"] = [
            float(cls_inter_stats[cls]["recall inters"][1]),
            float(cls_inter_stats[cls]["recall inters"][5]),
        ]
        out_interv_stats[cls]["recall"]["99th"] = [
            float(cls_inter_stats[cls]["recall inters"][0]),
            float(cls_inter_stats[cls]["recall inters"][6]),
        ]

        out_interv_stats[cls]["support"] = dict()
        out_interv_stats[cls]["support"]["mean"] = float(
            cls_inter_stats[cls]["support mean"]
        )
        out_interv_stats[cls]["support"]["median"] = float(
            cls_inter_stats[cls]["support inters"][3]
        )
        out_interv_stats[cls]["support"]["90th"] = [
            float(cls_inter_stats[cls]["support inters"][2]),
            float(cls_inter_stats[cls]["support inters"][4]),
        ]
        out_interv_stats[cls]["support"]["95th"] = [
            float(cls_inter_stats[cls]["support inters"][1]),
            float(cls_inter_stats[cls]["support inters"][5]),
        ]
        out_interv_stats[cls]["support"]["99th"] = [
            float(cls_inter_stats[cls]["support inters"][0]),
            float(cls_inter_stats[cls]["support inters"][6]),
        ]

        out_interv_stats[cls]["producer_accuracy"] = dict()
        out_interv_stats[cls]["producer_accuracy"]["mean"] = float(
            cls_inter_stats[cls]["producer_accuracy mean"]
        )
        out_interv_stats[cls]["producer_accuracy"]["median"] = float(
            cls_inter_stats[cls]["producer_accuracy inters"][3]
        )
        out_interv_stats[cls]["producer_accuracy"]["90th"] = [
            float(cls_inter_stats[cls]["producer_accuracy inters"][2]),
            float(cls_inter_stats[cls]["producer_accuracy inters"][4]),
        ]
        out_interv_stats[cls]["producer_accuracy"]["95th"] = [
            float(cls_inter_stats[cls]["producer_accuracy inters"][1]),
            float(cls_inter_stats[cls]["producer_accuracy inters"][5]),
        ]
        out_interv_stats[cls]["producer_accuracy"]["99th"] = [
            float(cls_inter_stats[cls]["producer_accuracy inters"][0]),
            float(cls_inter_stats[cls]["producer_accuracy inters"][6]),
        ]

        out_interv_stats[cls]["user_accuracy"] = dict()
        out_interv_stats[cls]["user_accuracy"]["mean"] = float(
            cls_inter_stats[cls]["user_accuracy mean"]
        )
        out_interv_stats[cls]["user_accuracy"]["median"] = float(
            cls_inter_stats[cls]["user_accuracy inters"][3]
        )
        out_interv_stats[cls]["user_accuracy"]["90th"] = [
            float(cls_inter_stats[cls]["user_accuracy inters"][2]),
            float(cls_inter_stats[cls]["user_accuracy inters"][4]),
        ]
        out_interv_stats[cls]["user_accuracy"]["95th"] = [
            float(cls_inter_stats[cls]["user_accuracy inters"][1]),
            float(cls_inter_stats[cls]["user_accuracy inters"][5]),
        ]
        out_interv_stats[cls]["user_accuracy"]["99th"] = [
            float(cls_inter_stats[cls]["user_accuracy inters"][0]),
            float(cls_inter_stats[cls]["user_accuracy inters"][6]),
        ]

    if out_json_file is not None:
        import rsgislib.tools.utils

        rsgislib.tools.utils.write_dict_to_json(out_interv_stats, out_json_file)

    return out_interv_stats


def calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets(
    vec_files: list,
    vec_lyrs: list,
    ref_col: str,
    cls_col: str,
    tmp_dir: str,
    conf_inter: int = 95,
    conf_thres: float = 0.05,
    out_plot_file: str = None,
    sample_frac: float = 0.2,
    sample_n_smps: int = None,
    bootstrap_n: int = 1000,
) -> (bool, int, list, list):
    """
    A function which calculates the f1-score and the confidence interval for each
    the point sets provided. Where the points a cumulatively combined
    increasing the number of points used for the analysis. Therefore, if there
    were 3 files in the input list vec_files, 3 f1-score and uncertainies would
    be calculated using the following point sets:

    1. vec_files[0]
    2. vec_files[0] + vec_files[1]
    3. vec_files[0] + vec_files[1] + vec_files[2]

    :param vec_files: list of input files which must be the same length as vec_lyrs
    :param vec_lyrs: list of input layer names which must be the same length
                     as vec_files
    :param ref_col: the name of the reference classification column in the input
                    vector file.
    :param cls_col: the name of the classification column in the input vector file.
    :param tmp_dir: A temporary directory where intermediate files can be written.
    :param conf_inter: The confidence interval to be used. Options are 90, 95 or 99.
                       The default is 95.
    :param conf_thres: the threshold used to defined whether the confidence interval
                         is below a user threshold. Value should be between 0-1. The
                         default is 0.05 (i.e., 5%).
    :param out_plot_file: Optionally an output plot of the f1-scores and upper and
                          lower confidence intervals can be outputted. If None
                          (default) then no plot will be produced. Otherwise, a file
                          path and name. File format can be PNG or PDF. Use file
                          extension of the output file to specify.
    :param sample_frac: The fraction of the whole dataset selected for each
                        bootstrap iteration.
    :param bootstrap_n: The number of bootstrap iterations.
    :return: (bool, int, list, list). 1. Did the confidence interval fall below the
             the confidence threshold. 2. the index of the point it first fell below
             the threshold. 3. list of f1-scores and 4. list of f1-score confidence
             intervals.

    """
    import rsgislib.tools.utils
    import rsgislib.vectorutils

    uid_str = rsgislib.tools.utils.uid_generator()

    if conf_inter not in [90, 95, 99]:
        raise rsgislib.RSGISPyException("conf_inter must have a value of 90, 95 or 99.")

    if not os.path.exists(tmp_dir):
        raise rsgislib.RSGISPyException("tmp_dir does not exist")

    if len(vec_files) != len(vec_lyrs):
        raise rsgislib.RSGISPyException(
            "vec_files and vec_lyrs have different lengths and must be the same."
        )

    f1_scores = list()
    f1_scr_intervals_rgn = list()
    f1_scr_intervals_min = list()
    f1_scr_intervals_max = list()
    n_pts = list()
    conf_thres_met = False
    conf_thres_met_idx = -1
    first = True
    i = 0
    tmp_vec_files = list()
    vecs_dict = list()
    for vec_file, vec_lyr in zip(vec_files, vec_lyrs):
        vecs_dict.append({"file": vec_file, "layer": vec_lyr})
        if first:
            c_vec_file = vec_file
            c_vec_lyr = vec_lyr
            first = False
        else:
            tmp_vec_file = os.path.join(tmp_dir, "tmp_vec_file_{}.gpkg".format(uid_str))
            tmp_vec_lyr = "lyr_{}_{}".format(uid_str, i)
            tmp_vec_files.append(tmp_vec_file)
            rsgislib.vectorutils.merge_vector_layers(
                vecs_dict,
                out_vec_file=tmp_vec_file,
                out_vec_lyr=tmp_vec_lyr,
                out_format="GPKG",
            )
            c_vec_file = tmp_vec_file
            c_vec_lyr = tmp_vec_lyr

        calc_metrics = calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval(
            c_vec_file,
            c_vec_lyr,
            ref_col,
            cls_col,
            out_json_file=None,
            sample_frac=sample_frac,
            sample_n_smps=sample_n_smps,
            bootstrap_n=bootstrap_n,
        )

        f1_scores.append(calc_metrics["macro avg"]["f1-score"]["median"])
        intervals = calc_metrics["macro avg"]["f1-score"]["95th"]
        if conf_inter == 90:
            intervals = calc_metrics["macro avg"]["f1-score"]["90th"]
        elif conf_inter == 99:
            intervals = calc_metrics["macro avg"]["f1-score"]["99th"]
        inter_rng = intervals[1] - intervals[0]
        f1_scr_intervals_rgn.append(inter_rng)
        f1_scr_intervals_min.append(intervals[0])
        f1_scr_intervals_max.append(intervals[1])
        n_pts.append(rsgislib.vectorutils.get_vec_feat_count(c_vec_file, c_vec_lyr))

        if (inter_rng < conf_thres) and (not conf_thres_met):
            conf_thres_met = True
            conf_thres_met_idx = i

        i = i + 1

    for tmp_vec_file in tmp_vec_files:
        if os.path.exists(tmp_vec_file):
            rsgislib.vectorutils.delete_vector_file(tmp_vec_file)

    if out_plot_file is not None:
        import matplotlib.pyplot as plt

        fig, ax = plt.subplots()
        ax.plot(n_pts, f1_scores, "-", color="black")
        ax.fill_between(
            n_pts, f1_scr_intervals_min, f1_scr_intervals_max, alpha=0.2, color="gray"
        )
        ax.set_xlabel("N Reference Plots")
        ax.set_ylabel("F1 Score (macro avg)")
        ax.set_title("F1 Score Confidence Intervals")
        fig.savefig(out_plot_file)

    return conf_thres_met, conf_thres_met_idx, f1_scores, f1_scr_intervals_rgn


def summarise_multi_acc_ptonly_metrics(
    acc_json_files: list, out_acc_json_sum_file: str
):
    """
    A function which takes a list of JSON files outputted from the
    calc_acc_ptonly_metrics_vecsamples function and creates a JSON with
    summary statistics the individual accuracy metrics. This is useful
    if you have calculated your accuracy using a number of individual
    plots and you want to compare the accuracies from the individual
    plots rather than just produce an overall summary.

    :param acc_json_files: list of input JSON files.
    :param out_acc_json_sum_file: file path the output JSON file.

    """
    import rsgislib.tools.utils

    out_dict = dict()
    out_dict["accuracy"] = {"values": []}
    out_dict["cohen_kappa"] = {"values": []}
    out_dict["macro avg"] = {
        "f1-score": {"values": []},
        "precision": {"values": []},
        "recall": {"values": []},
    }
    out_dict["weighted avg"] = {
        "f1-score": {"values": []},
        "precision": {"values": []},
        "recall": {"values": []},
    }

    first = True
    for acc_json_file in acc_json_files:
        acc_pt_dict = rsgislib.tools.utils.read_json_to_dict(acc_json_file)
        if first:
            cls_names = acc_pt_dict["cls_confidence_intervals"]
            for cls_name in cls_names:
                out_dict[cls_name] = {
                    "f1-score": {"values": []},
                    "precision": {"values": []},
                    "recall": {"values": []},
                }

            first = False

        out_dict["accuracy"]["values"].append(acc_pt_dict["accuracy"])
        out_dict["cohen_kappa"]["values"].append(acc_pt_dict["cohen_kappa"])
        out_dict["macro avg"]["f1-score"]["values"].append(
            acc_pt_dict["macro avg"]["f1-score"]
        )
        out_dict["macro avg"]["precision"]["values"].append(
            acc_pt_dict["macro avg"]["precision"]
        )
        out_dict["macro avg"]["recall"]["values"].append(
            acc_pt_dict["macro avg"]["recall"]
        )
        out_dict["weighted avg"]["f1-score"]["values"].append(
            acc_pt_dict["weighted avg"]["f1-score"]
        )
        out_dict["weighted avg"]["precision"]["values"].append(
            acc_pt_dict["weighted avg"]["precision"]
        )
        out_dict["weighted avg"]["recall"]["values"].append(
            acc_pt_dict["weighted avg"]["recall"]
        )
        for cls_name in cls_names:
            out_dict[cls_name]["f1-score"]["values"].append(
                acc_pt_dict[cls_name]["f1-score"]
            )
            out_dict[cls_name]["precision"]["values"].append(
                acc_pt_dict[cls_name]["precision"]
            )
            out_dict[cls_name]["recall"]["values"].append(
                acc_pt_dict[cls_name]["recall"]
            )

    def _calc_sum_stats(vals):
        arr_vals = numpy.array(vals)
        sum_stats_dict = dict()
        sum_stats_dict["mean"] = float(numpy.mean(arr_vals))
        sum_stats_dict["median"] = float(numpy.median(arr_vals))
        sum_stats_dict["min"] = float(numpy.min(arr_vals))
        sum_stats_dict["max"] = float(numpy.max(arr_vals))
        for percentile in [1, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 95, 99]:
            sum_stats_dict["percentile_{}".format(percentile)] = float(
                numpy.percentile(arr_vals, percentile)
            )

        return sum_stats_dict

    out_dict["accuracy"].update(_calc_sum_stats(out_dict["accuracy"]["values"]))
    out_dict["cohen_kappa"].update(_calc_sum_stats(out_dict["cohen_kappa"]["values"]))
    out_dict["macro avg"]["f1-score"].update(
        _calc_sum_stats(out_dict["macro avg"]["f1-score"]["values"])
    )
    out_dict["macro avg"]["precision"].update(
        _calc_sum_stats(out_dict["macro avg"]["precision"]["values"])
    )
    out_dict["macro avg"]["recall"].update(
        _calc_sum_stats(out_dict["macro avg"]["recall"]["values"])
    )
    out_dict["weighted avg"]["f1-score"].update(
        _calc_sum_stats(out_dict["weighted avg"]["f1-score"]["values"])
    )
    out_dict["weighted avg"]["precision"].update(
        _calc_sum_stats(out_dict["weighted avg"]["precision"]["values"])
    )
    out_dict["weighted avg"]["recall"].update(
        _calc_sum_stats(out_dict["weighted avg"]["recall"]["values"])
    )
    for cls_name in cls_names:
        out_dict[cls_name]["f1-score"].update(
            _calc_sum_stats(out_dict[cls_name]["f1-score"]["values"])
        )
        out_dict[cls_name]["precision"].update(
            _calc_sum_stats(out_dict[cls_name]["precision"]["values"])
        )
        out_dict[cls_name]["recall"].update(
            _calc_sum_stats(out_dict[cls_name]["recall"]["values"])
        )

    rsgislib.tools.utils.write_dict_to_json(out_dict, out_acc_json_sum_file)


def create_modelled_acc_pts(
    err_matrix: List[List[float]],
    cls_lst: List[str],
    n_pts: int,
    shuffle_pts: bool = True,
    rnd_seed: int = 42,
) -> Tuple[numpy.array, numpy.array]:
    """
    A function which generates a set of of modelled accuracy assessment points which
    would produce the error matrix passed to the function. The output of this function
    can be used with the classaccuracymetrics.calc_class_pt_accuracy_metrics
    function to calculate accuracy metrics for these points.

    The input error matrix is represented by n x n list of lists, where the
    first axis is the reference class and the second the 'classification'.

    :param err_matrix: a list of lists representing the error matrix which should be
                       square with the same number of classes and order as the cls_lst.
                       The error matrix should sum to 1 with the individual class values
                       relative to the proportion of the scene and class accuracy.
    :param cls_lst: A list of class names
    :param n_pts: the number of output points produced
    :param rnd_seed: a seed for the random generator which shuffles the output.
    :return: a tuple with two numpy arrays of size n where the first is the 'reference'
    and the second is the 'classification'.

    """
    import numpy.random

    n_classes = len(cls_lst)
    if len(err_matrix) != n_classes:
        raise rsgislib.RSGISPyException(
            "The number of classes in the list and error matrix do not match"
        )
    for i in range(n_classes):
        if len(err_matrix[i]) != n_classes:
            raise rsgislib.RSGISPyException(
                "The number of classes in the list and error matrix do not match"
            )

    err_mtx_unit_area_arr = numpy.array(err_matrix)

    if abs(err_mtx_unit_area_arr.sum() - 1) > 0.0001:
        raise rsgislib.RSGISPyException("The error matrix does not sum to 1")

    out_ref_arr = numpy.empty(n_pts, dtype=numpy.dtype("a255"))
    out_ref_arr[...] = ""
    out_cls_arr = numpy.empty(n_pts, dtype=numpy.dtype("a255"))
    out_cls_arr[...] = ""

    s_ref_idx = 0
    s_cls_idx = 0
    for i, ref_cls in enumerate(cls_lst):
        prop_pts = err_mtx_unit_area_arr[i].sum()
        n_cls_pts = int(n_pts * prop_pts)
        out_ref_arr[s_ref_idx : s_ref_idx + n_cls_pts] = ref_cls
        for j, cls_cls in enumerate(cls_lst):
            n_cls_cls_pts = int(n_pts * err_mtx_unit_area_arr[i][j])
            out_cls_arr[s_cls_idx : s_cls_idx + n_cls_cls_pts] = cls_cls
            s_cls_idx += n_cls_cls_pts
        s_ref_idx += n_cls_pts

    # Check that all points have a class value and remove those which don't
    # this can happen if the proportions don't perfectly divide into the
    # number of points requested.
    empty_pt = numpy.zeros_like(out_ref_arr, dtype=int)
    empty_pt[out_ref_arr == b""] = 1
    empty_pt[out_cls_arr == b""] = 1
    out_ref_arr = out_ref_arr[empty_pt == 0]
    out_cls_arr = out_cls_arr[empty_pt == 0]

    if shuffle_pts:
        # Randomly shuffle the points so they are in a random order.
        n_out_pts = out_cls_arr.shape[0]
        np_rng = numpy.random.default_rng(seed=rnd_seed)
        shuffle_idx = np_rng.permutation(n_out_pts)
        return out_ref_arr[shuffle_idx], out_cls_arr[shuffle_idx]
    return out_ref_arr, out_cls_arr


def calc_sampled_acc_metrics(
    ref_samples: numpy.array,
    pred_samples: numpy.array,
    cls_names: numpy.array,
    smpls_lst: List[int],
    out_metrics_file: str,
    n_repeats: int = 10,
    out_usr_metrics_plot: str = None,
    out_prod_metrics_plot: str = None,
    out_ref_usr_plot: str = None,
    out_ref_prod_plot: str = None,
    cls_colours: Dict[str, List[float]] = None,
    y_plt_usr_min=None,
    y_plt_usr_max=None,
    y_plt_prod_min=None,
    y_plt_prod_max=None,
    ref_line_clr: List = (0.0, 0.0, 0.0),
    in_loop=False,
):
    """
    A function which calculates users and producers accuracies for the inputted
    reference and predicted samples by under sampling the points (with bootstrapping)
    to try and estimate the number of points which are needed to get a reliable
    estimate of the whole population of samples.

    This function was original written alongside create_modelled_acc_pts to aid
    the estimation of the number of accuracy assessment points required.

    Be careful not to use under-sampling values which are too small as you maybe not
    sample all the classes and therefore get an error.

    :param ref_samples: a 1d array of reference samples represented by a
                        numeric class id
    :param pred_samples: a 1d array of predicted samples represented by a
                         numeric class id
    :param cls_names: a 1d list of the class names (labels) in the order of
                      the class ids.
    :param smpls_lst: list of n samples to use for under sampling the input data
                       (e.g., [400, 500, 600, 700]). Clearly the number of samples
                       cannot be more than the total number of points. Also, be
                       careful not to values which are too small as you maybe not
                       sample all the classes.
    :param out_metrics_file: an output json file which will have the calculated
                             statistics for future reference.
    :param n_repeats: the number of bootstrap repeats for the sub-sampling. This is
                      used to calculate the 95th confidence interval for each estimate.
    :param out_usr_metrics_plot: A file path for an optional plot for the users
                                 accuracies. (Default: None - no plot produced).
    :param out_prod_metrics_plot: A file path for an optional plot for the producers
                                  accuracies. (Default: None - no plot produced).
    :param out_ref_usr_plot: A file path for an optional plot for the users
                             reference accuracies. (Default: None - no plot produced).
    :param out_ref_prod_plot: A file path for an optional plot for the producers
                              reference accuracies. (Default: None - no plot produced).
    :param cls_colours: an optional dict with class colours. The key value should be
                        the class name while the value should be a list of 3 float
                        between 0-1 representing RGB values.
    :param y_plt_usr_min: Optional minimum y value for users plot.
    :param y_plt_usr_max: Optional maximum y value for users plot.
    :param y_plt_prod_min: Optional minimum y value for producers plot.
    :param y_plt_prod_max: Optional maximum y value for producers plot.
    :param ref_line_clr: The colour of the reference line added to the
                         out_usr_metrics_plot and out_prod_metrics_plot.
                         The default is black (0.0, 0.0, 0.0).
    :param in_loop: True is called within a loop so tqdm progress bar will
                    then be passed a position parameter of 1.

    """
    import rsgislib.classification.classaccuracymetrics
    import tqdm
    import matplotlib.pyplot as plt

    cls_names = sorted(cls_names, key=str.lower)
    n_tot_pts = ref_samples.shape[0]

    # Calculate the baseline statistics using all the data.
    # This will provide a 'Truth'.
    ref_metrics = calc_class_pt_accuracy_metrics(ref_samples, pred_samples, cls_names)

    ref_usr = ref_metrics["user_accuracy"]
    ref_prod = ref_metrics["producer_accuracy"]

    smp_idx = numpy.arange(ref_samples.shape[0])
    out_ref_metrics = dict()
    n_clses = len(cls_names)

    n_smpls = len(smpls_lst)
    tqdm_pos = 0
    if in_loop:
        tqdm_pos = 1
    for n_smps in tqdm.tqdm(smpls_lst, position=tqdm_pos):
        tmp_usr_vals = numpy.zeros((n_repeats, n_clses), dtype=float)
        tmp_prod_vals = numpy.zeros((n_repeats, n_clses), dtype=float)

        for i in range(n_repeats):
            sub_sel_idx = numpy.random.choice(smp_idx, n_smps, replace=False)
            sub_ref_samples = ref_samples[sub_sel_idx]
            sub_pred_samples = pred_samples[sub_sel_idx]
            tmp_metrics = calc_class_pt_accuracy_metrics(
                sub_ref_samples, sub_pred_samples, cls_names
            )
            for j in range(n_clses):
                tmp_usr_vals[i, j] = tmp_metrics["user_accuracy"][j]
                tmp_prod_vals[i, j] = tmp_metrics["producer_accuracy"][j]

        tmp_usr_vals[numpy.isnan(tmp_usr_vals)] = 0
        tmp_prod_vals[numpy.isnan(tmp_prod_vals)] = 0

        out_stats = dict()
        out_stats["users_mean"] = tmp_usr_vals.mean(axis=0)
        out_stats["users_lower"] = numpy.percentile(tmp_usr_vals, 5, axis=0)
        out_stats["users_lower"][numpy.isnan(out_stats["users_lower"])] = 0
        out_stats["users_upper"] = numpy.percentile(tmp_usr_vals, 95, axis=0)
        out_stats["users_upper"][numpy.isnan(out_stats["users_upper"])] = 100
        out_stats["users_conf_range"] = (
            out_stats["users_upper"] - out_stats["users_lower"]
        )
        out_stats["users_ref_diff"] = ref_usr - out_stats["users_mean"]
        out_stats["users_ref_diff_abs"] = numpy.abs(out_stats["users_ref_diff"])

        out_stats["producers_mean"] = tmp_prod_vals.mean(axis=0)
        out_stats["producers_lower"] = numpy.percentile(tmp_prod_vals, 5, axis=0)
        out_stats["producers_lower"][numpy.isnan(out_stats["producers_lower"])] = 0
        out_stats["producers_upper"] = numpy.percentile(tmp_prod_vals, 95, axis=0)
        out_stats["producers_upper"][numpy.isnan(out_stats["producers_upper"])] = 100
        out_stats["producers_conf_range"] = (
            out_stats["producers_upper"] - out_stats["producers_lower"]
        )
        out_stats["producers_ref_diff"] = ref_prod - out_stats["producers_mean"]
        out_stats["producers_ref_diff_abs"] = numpy.abs(out_stats["producers_ref_diff"])
        out_ref_metrics[n_smps] = out_stats

    # Write stats to output file.
    rsgislib.tools.utils.write_dict_to_json(out_ref_metrics, out_metrics_file)

    usr_mean = numpy.zeros((n_smpls, n_clses))
    usr_lower = numpy.zeros((n_smpls, n_clses))
    usr_upper = numpy.zeros((n_smpls, n_clses))
    usr_abs_diff = numpy.zeros((n_smpls, n_clses))
    prod_mean = numpy.zeros((n_smpls, n_clses))
    prod_lower = numpy.zeros((n_smpls, n_clses))
    prod_upper = numpy.zeros((n_smpls, n_clses))
    prod_abs_diff = numpy.zeros((n_smpls, n_clses))
    for i, n_smpls in enumerate(smpls_lst):
        for j in range(n_clses):
            usr_mean[i, j] = out_ref_metrics[n_smpls]["users_mean"][j]
            usr_lower[i, j] = out_ref_metrics[n_smpls]["users_lower"][j]
            usr_upper[i, j] = out_ref_metrics[n_smpls]["users_upper"][j]
            usr_abs_diff[i, j] = out_ref_metrics[n_smpls]["users_ref_diff_abs"][j]

            prod_mean[i, j] = out_ref_metrics[n_smpls]["producers_mean"][j]
            prod_lower[i, j] = out_ref_metrics[n_smpls]["producers_lower"][j]
            prod_upper[i, j] = out_ref_metrics[n_smpls]["producers_upper"][j]
            prod_abs_diff[i, j] = out_ref_metrics[n_smpls]["producers_ref_diff_abs"][j]

    if (
        (out_usr_metrics_plot is not None)
        or (out_prod_metrics_plot is not None)
        or (out_ref_usr_plot is not None)
        or (out_ref_prod_plot is not None)
    ) and (cls_colours is None):
        cls_colours = dict()
        np_rng = numpy.random.default_rng()
        for cls_name in cls_names:
            cls_colours[cls_name] = np_rng.random(3)

    if out_usr_metrics_plot is not None:
        if y_plt_usr_min is None:
            y_plt_usr_min = usr_lower.min() - 5

        if y_plt_usr_max is None:
            y_plt_usr_max = usr_upper.max() + 5

        x_axs = 1
        y_axs = n_clses
        if n_clses == 4:
            x_axs = 2
            y_axs = 2
        elif n_clses > 4:
            x_axs = 3
            y_axs = round((n_clses / 3) + 0.5)

        print(f"Plot Shape: {x_axs} x {y_axs}")

        fig_x_size = 12 * x_axs
        fig_y_size = 6 * y_axs

        fig, axs = plt.subplots(
            x_axs, y_axs, figsize=(fig_x_size, fig_y_size), sharex=True, sharey=True
        )

        if x_axs > 1:
            axs_flat = list()
            for x in range(x_axs):
                for y in range(y_axs):
                    axs_flat.append(axs[x][y])
        else:
            axs_flat = axs

        for j in range(n_clses):
            cls_name = cls_names[j]

            axs_flat[j].fill_between(
                smpls_lst,
                usr_lower[..., j],
                usr_upper[..., j],
                color=cls_colours[cls_name],
                alpha=0.25,
            )
            axs_flat[j].axhline(y=ref_usr[j], color=ref_line_clr, linestyle="-")
            axs_flat[j].plot(smpls_lst, usr_lower[..., j], color=cls_colours[cls_name])
            axs_flat[j].plot(smpls_lst, usr_upper[..., j], color=cls_colours[cls_name])
            axs_flat[j].plot(smpls_lst, usr_mean[..., j], color=cls_colours[cls_name])
            axs_flat[j].set_title(cls_name)
            axs_flat[j].set_ylim(y_plt_usr_min, y_plt_usr_max)
        fig.suptitle(f"Users Accuracy (n: {n_tot_pts})")
        fig.supxlabel("n samples")
        fig.supylabel("%")
        plt.tight_layout()
        plt.savefig(out_usr_metrics_plot)

    if out_prod_metrics_plot is not None:
        if y_plt_prod_min is None:
            y_plt_prod_min = prod_lower.min() - 5

        if y_plt_prod_max is None:
            y_plt_prod_max = prod_upper.max() + 5

        x_axs = 1
        y_axs = n_clses
        if n_clses == 4:
            x_axs = 2
            y_axs = 2
        elif n_clses > 4:
            x_axs = 3
            y_axs = round((n_clses / 3) + 0.5)

        print(f"Plot Shape: {x_axs} x {y_axs}")

        fig_x_size = 12 * x_axs
        fig_y_size = 6 * y_axs

        fig, axs = plt.subplots(
            x_axs, y_axs, figsize=(fig_x_size, fig_y_size), sharex=True, sharey=True
        )

        if x_axs > 1:
            axs_flat = list()
            for x in range(x_axs):
                for y in range(y_axs):
                    axs_flat.append(axs[x][y])
        else:
            axs_flat = axs

        for j in range(n_clses):
            cls_name = cls_names[j]

            axs_flat[j].fill_between(
                smpls_lst,
                prod_lower[..., j],
                prod_upper[..., j],
                color=cls_colours[cls_name],
                alpha=0.25,
            )
            axs_flat[j].axhline(y=ref_prod[j], color=ref_line_clr, linestyle="-")
            axs_flat[j].plot(smpls_lst, prod_lower[..., j], color=cls_colours[cls_name])
            axs_flat[j].plot(smpls_lst, prod_upper[..., j], color=cls_colours[cls_name])
            axs_flat[j].plot(smpls_lst, prod_mean[..., j], color=cls_colours[cls_name])
            axs_flat[j].set_title(cls_name)
            axs_flat[j].set_ylim(y_plt_prod_min, y_plt_prod_max)
        fig.suptitle(f"Producers Accuracy (n: {n_tot_pts})")
        fig.supxlabel("n samples")
        fig.supylabel("%")
        plt.tight_layout()
        plt.savefig(out_prod_metrics_plot)

    if out_ref_usr_plot is not None:
        y_plt_max = usr_abs_diff.max() + 5

        x_axs = 1
        y_axs = n_clses
        if n_clses == 4:
            x_axs = 2
            y_axs = 2
        elif n_clses > 4:
            x_axs = round((n_clses / 3) + 0.5)
            y_axs = 3

        print(f"Plot Shape: {x_axs} x {y_axs}")

        fig_x_size = 12 * x_axs
        fig_y_size = 6 * y_axs

        fig, axs = plt.subplots(
            x_axs, y_axs, figsize=(fig_x_size, fig_y_size), sharex=True, sharey=True
        )

        if x_axs > 1:
            axs_flat = list()
            for x in range(x_axs):
                for y in range(y_axs):
                    axs_flat.append(axs[x][y])
        else:
            axs_flat = axs

        for j in range(n_clses):
            cls_name = cls_names[j]
            axs_flat[j].plot(
                smpls_lst, usr_abs_diff[..., j], color=cls_colours[cls_name]
            )
            axs_flat[j].set_title(cls_name)
            axs_flat[j].set_ylim(0, y_plt_max)
        fig.suptitle(f"Users Reference (n: {n_tot_pts})")
        fig.supxlabel("n samples")
        fig.supylabel("%")
        plt.tight_layout()
        plt.savefig(out_ref_usr_plot)

    if out_ref_prod_plot is not None:
        y_plt_max = prod_abs_diff.max() + 5

        x_axs = 1
        y_axs = n_clses
        if n_clses == 4:
            x_axs = 2
            y_axs = 2
        elif n_clses > 4:
            x_axs = round((n_clses / 3) + 0.5)
            y_axs = 3

        print(f"Plot Shape: {x_axs} x {y_axs}")

        fig_x_size = 12 * x_axs
        fig_y_size = 6 * y_axs

        fig, axs = plt.subplots(
            x_axs, y_axs, figsize=(fig_x_size, fig_y_size), sharex=True, sharey=True
        )

        if x_axs > 1:
            axs_flat = list()
            for x in range(x_axs):
                for y in range(y_axs):
                    axs_flat.append(axs[x][y])
        else:
            axs_flat = axs

        for j in range(n_clses):
            cls_name = cls_names[j]
            axs_flat[j].plot(
                smpls_lst, prod_abs_diff[..., j], color=cls_colours[cls_name]
            )
            axs_flat[j].set_title(cls_name)
            axs_flat[j].set_ylim(0, y_plt_max)
        fig.suptitle(f"Producers Reference (n: {n_tot_pts})")
        fig.supxlabel("n samples")
        fig.supylabel("%")
        plt.tight_layout()
        plt.savefig(out_ref_prod_plot)


def create_norm_modelled_err_matrix(
    cls_areas: List[float], ref_smpl_accs: List[List[float]]
) -> List[List[float]]:
    """
    A function which creates a normalised error matrix (as required by
    create_modelled_acc_pts function) using the class areas and relative
    accuracies of the reference samples.

    :param cls_areas: a list of relative class areas (i.e., percentage are for each
                      class). The list must be either add up to 100 or 1. (e.g.,
                      [10, 40, 30, 20] would mean that there is 10% of the area
                      mapped as class 1, 40% for class2, 30 for class3 and 20 for
                      class4.
    :param ref_smpl_accs: The accuracy of the classes relative to the reference
                          samples. This is an n x n square matrix where n
                          is the number of classes. Each row is the relative
                          accuracy of the reference samples for the class. Each
                          row must either sum to 1 or 100.
    :return: an n x n square matrix which is normalised for the class areas.

    """
    cls_areas_arr = numpy.array(cls_areas, dtype=float)

    if abs(cls_areas_arr.sum() - 100) < 0.01:
        cls_areas_arr = cls_areas_arr / 100

    if abs(cls_areas_arr.sum() - 1) > 0.0001:
        raise rsgislib.RSGISPyException("The list of class areas must sum to 1 or 100.")

    n_clses = cls_areas_arr.shape[0]
    ref_smpl_accs_arr = numpy.array(ref_smpl_accs, dtype=float)

    for i in range(n_clses):
        if abs(ref_smpl_accs_arr[i].sum() - 1) > 0.0001:
            ref_smpl_accs_arr[i] = ref_smpl_accs_arr[i] / 100
            if abs(ref_smpl_accs_arr[i].sum() - 1) > 0.0001:
                raise rsgislib.RSGISPyException(f"Row {i} does not sum to 1 or 100.")

        ref_smpl_accs_arr[i] = ref_smpl_accs_arr[i] * cls_areas_arr[i]

    return ref_smpl_accs_arr.tolist()
