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
# Purpose: Provide a set of functions for using the scikit-learn
#          for classifying remote sensing imagery
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

from typing import List, Dict

import numpy

from osgeo import gdal

import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis
from rsgislib.classification import ClassInfoObj
from rsgislib.imageutils import ImageBandInfo


from sklearn.model_selection._search import BaseSearchCV
from sklearn.base import BaseEstimator

import h5py

from rios import applier
from rios import cuiprogress
from rios import rat
from rios import ratapplier


def perform_sklearn_classifier_param_search(
    cls_train_info: Dict[str, ClassInfoObj], search_obj: BaseSearchCV,
) -> BaseEstimator:
    """
    A function to find the 'optimal' parameters for classification using a grid search
    or random search (http://scikit-learn.org/stable/modules/grid_search.html).
    The validation data will be used to identify the optimal parameters and the
    returned classifier will be initialised with those parameters but not trained

    :param cls_train_info: list of rsgislib.classification.ClassInfoObj objects
                           which will be used to train the classifier.
    :param search_obj: is an instance of the sklearn.model_selection.BaseSearchCV
                       (e.g., GridSearchCV or RandomizedSearchCV) object parameterised
                       with an instance of the classifier and associated parameters
                       to be searched.
    :return: Instance of

    """
    if len(cls_train_info) < 2:
        raise rsgislib.RSGISPyException(
            "Need at least 2 classes to be worth running "
            "train_sklearn_classifier_param_search function."
        )

    first = True
    num_vars = 0
    num_vals = 0
    for classInfoVal in cls_train_info.values():
        data_shp = h5py.File(classInfoVal.valid_file_h5, "r")["DATA/DATA"].shape
        if first:
            num_vars = data_shp[1]
            first = False
        num_vals += data_shp[0]

    vld_data_arr = numpy.zeros([num_vals, num_vars], dtype=float)
    vld_class_arr = numpy.zeros([num_vals], dtype=int)

    row_init = 0
    for key in cls_train_info:
        # Open the dataset
        f = h5py.File(cls_train_info[key].valid_file_h5, "r")
        num_rows = f["DATA/DATA"].shape[0]
        # Copy data and populate classid array
        vld_data_arr[row_init : (row_init + num_rows)] = f["DATA/DATA"]
        vld_class_arr[row_init : (row_init + num_rows)] = cls_train_info[key].id
        row_init += num_rows
        f.close()

    print(
        "Training data size: {} x {}".format(
            vld_data_arr.shape[0], vld_data_arr.shape[1]
        )
    )

    search_obj.fit(vld_data_arr, vld_class_arr)
    if not search_obj.refit:
        raise rsgislib.RSGISPyException("Search did no find a fit therefore failed...")

    print(
        "Best score was {} and has parameters {}.".format(
            search_obj.best_score_, search_obj.best_params_
        )
    )

    return search_obj.best_estimator_


def train_sklearn_classifier(
    cls_train_info: Dict[str, ClassInfoObj], sk_classifier: BaseEstimator
) -> (float, float):
    """
    This function trains the classifier.

    :param cls_train_info: list of rsgislib.classification.ClassInfoObj objects
                           which will be used to train and test the classifier.
    :param sk_classifier: an instance of a parameterised scikit-learn classifier
                         (http://scikit-learn.org/stable/supervised_learning.html)
    :return training and testing accuracies (between 0-1)

    """
    if len(cls_train_info) < 2:
        raise rsgislib.RSGISPyException(
            "Need at least 2 classes to be worth "
            "running train_sklearn_classifier function."
        )

    first = True
    num_train_vars = 0
    num_train_vals = 0
    num_test_vars = 0
    num_test_vals = 0
    for class_info_val in cls_train_info.values():
        data_train_shp = h5py.File(class_info_val.train_file_h5, "r")["DATA/DATA"].shape
        data_test_shp = h5py.File(class_info_val.test_file_h5, "r")["DATA/DATA"].shape
        if first:
            num_train_vars = data_train_shp[1]
            num_test_vars = data_test_shp[1]
            first = False
        num_train_vals += data_train_shp[0]
        num_test_vals += data_test_shp[0]

    data_train_arr = numpy.zeros([num_train_vals, num_train_vars], dtype=float)
    class_train_arr = numpy.zeros([num_train_vals], dtype=int)

    data_test_arr = numpy.zeros([num_test_vals, num_test_vars], dtype=float)
    class_test_arr = numpy.zeros([num_test_vals], dtype=int)

    row_train_init = 0
    row_test_init = 0
    for key in cls_train_info:
        # Open the dataset
        f = h5py.File(cls_train_info[key].train_file_h5, "r")
        num_rows = f["DATA/DATA"].shape[0]
        # Copy data and populate classid array
        data_train_arr[row_train_init : (row_train_init + num_rows)] = f["DATA/DATA"]
        class_train_arr[row_train_init : (row_train_init + num_rows)] = cls_train_info[
            key
        ].id
        row_train_init += num_rows
        f.close()

        # Open the dataset
        f = h5py.File(cls_train_info[key].test_file_h5, "r")
        num_rows = f["DATA/DATA"].shape[0]
        # Copy data and populate class_test_arr array
        data_test_arr[row_test_init : (row_test_init + num_rows)] = f["DATA/DATA"]
        class_test_arr[row_test_init : (row_test_init + num_rows)] = cls_train_info[
            key
        ].id
        row_test_init += num_rows
        f.close()

    print(
        "Training data size: {} x {}".format(
            data_train_arr.shape[0], data_train_arr.shape[1]
        )
    )
    print(
        "Testing data size: {} x {}".format(
            data_test_arr.shape[0], data_test_arr.shape[1]
        )
    )

    print("Training Classifier")
    sk_classifier.fit(data_train_arr, class_train_arr)
    print("Completed")

    print("Calc Classifier Accuracy")
    train_acc_val = sk_classifier.score(data_train_arr, class_train_arr)
    print("Classifier Train Score = {}%".format(round(train_acc_val * 100, 2)))

    test_acc_val = sk_classifier.score(data_test_arr, class_test_arr)
    print("Classifier Test Score = {}%".format(round(test_acc_val * 100, 2)))

    return train_acc_val, test_acc_val


def apply_sklearn_classifier(
    cls_train_info: Dict[str, ClassInfoObj],
    sk_classifier: BaseEstimator,
    in_img_mask: str,
    img_mask_val: int,
    img_file_info: List[ImageBandInfo],
    output_img: str,
    gdalformat: str = "KEA",
    class_clr_names: bool = True,
    out_score_img: str = None,
    ignore_consec_cls_ids: bool = False,
):
    """
    This function uses a trained classifier and applies it to the provided input image.

    :param cls_train_info: dict (where the key is the class name) of
                           rsgislib.classification.ClassInfoObj objects which will be
                           used to train the classifier provide pixel value id and
                           RGB class values.
    :param sk_classifier: a trained instance of a scikit-learn classifier
    :param in_img_mask: is an image file providing a mask to specify where should be
                        classified. Simplest mask is all the valid data regions
                        (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the imgMask to limit the region to
                         which the classification is applied. Can be used to create a
                         hierarchical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects to
                          identify which images and bands are to be used for the
                          classification so it adheres to the training data.
    :param output_img: output image file with the classification. Note. by default
                       a colour table and class names column is added to the image
                       if the gdalformat is KEA.
    :param gdalformat: is the output image format
    :param class_clr_names: default is True and therefore a colour table will the
                            colours specified in classTrainInfo and a class_names_col
                            column (from imgFileInfo) will be added to the output file.
    :param out_score_img: A file path for a score image. If None then not outputted.
                          Note, this function uses the predict_proba() function from
                          the scikit-learn model which isn't available for all
                          classifiers and therefore might produce an error if called
                          on a model which doesn't have this function. For example,
                          sklearn.svm.SVC.
    :param ignore_consec_cls_ids: A boolean to specify whether to ignore that the
                                  class ids should be consecutive and the out_ids
                                  used to specify other non-consecutive ids. This
                                  has some risks but allows more flexibility when using
                                  the function.

    """
    create_out_score_img = False
    if out_score_img is not None:
        create_out_score_img = True

    n_classes = len(cls_train_info)
    cls_id_lut = numpy.zeros(n_classes)
    for cls_name in cls_train_info:
        if not ignore_consec_cls_ids:
            if cls_train_info[cls_name].id >= n_classes:
                raise rsgislib.RSGISPyException(
                    "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                        cls_name, cls_train_info[cls_name].id
                    )
                )
        cls_id_lut[cls_train_info[cls_name].id] = cls_train_info[cls_name].out_id

    in_files = applier.FilenameAssociations()
    in_files.image_mask = in_img_mask
    num_class_vars = 0
    for img_file in img_file_info:
        in_files.__dict__[img_file.name] = img_file.file_name
        num_class_vars = num_class_vars + len(img_file.bands)

    outfiles = applier.FilenameAssociations()
    outfiles.out_image = output_img
    if create_out_score_img:
        outfiles.out_score_img = out_score_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = sk_classifier
    otherargs.msk_val = img_mask_val
    otherargs.num_class_vars = num_class_vars
    otherargs.n_classes = n_classes
    otherargs.img_file_info = img_file_info
    otherargs.out_score_img = out_score_img
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

    # RIOS function to apply classifier
    def _apply_sk_classifier(info, inputs, outputs, otherargs):
        """
        Internal function for rios applier. Used within apply_sklearn_classifier.
        """
        out_class_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.uint32)

        if otherargs.out_score_img:
            out_score_vals = numpy.zeros(
                (
                    otherargs.n_classes,
                    inputs.image_mask.shape[1],
                    inputs.image_mask.shape[2],
                ),
                dtype=numpy.float32,
            )

        if numpy.any(inputs.image_mask == otherargs.msk_val):
            out_class_vals = out_class_vals.flatten()
            if otherargs.out_score_img:
                out_score_vals = out_score_vals.reshape(
                    out_class_vals.shape[0], otherargs.n_classes
                )
            img_mask_vals = inputs.image_mask.flatten()
            class_vars = numpy.zeros(
                (out_class_vals.shape[0], otherargs.num_class_vars), dtype=numpy.float32
            )
            # Array index which can be used to populate the output
            # array following masking etc.
            pxl_id = numpy.arange(img_mask_vals.shape[0])
            class_vars_idx = 0
            for img_file in otherargs.img_file_info:
                img_arr = inputs.__dict__[img_file.name]
                for band in img_file.bands:
                    class_vars[..., class_vars_idx] = img_arr[(band - 1)].flatten()
                    class_vars_idx = class_vars_idx + 1
            class_vars = class_vars[img_mask_vals == otherargs.msk_val]
            pxl_id = pxl_id[img_mask_vals == otherargs.msk_val]

            # Perform classification
            preds_idxs = otherargs.classifier.predict(class_vars)

            # Use the LUT to update the output class ids
            preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
            for cld_id, idx in zip(
                otherargs.cls_id_lut, numpy.arange(0, otherargs.n_classes)
            ):
                preds_cls_ids[preds_idxs == idx] = cld_id

            # Write the output classifiction to the output array.
            out_class_vals[pxl_id] = preds_cls_ids
            out_class_vals = numpy.expand_dims(
                out_class_vals.reshape(
                    (inputs.image_mask.shape[1], inputs.image_mask.shape[2])
                ),
                axis=0,
            )
            if otherargs.out_score_img:
                pred_class_score = otherargs.classifier.predict_proba(class_vars)
                out_score_vals[pxl_id] = pred_class_score
                out_score_vals = out_score_vals.T
                out_score_vals = out_score_vals.reshape(
                    (
                        otherargs.n_classes,
                        inputs.image_mask.shape[1],
                        inputs.image_mask.shape[2],
                    )
                )
        outputs.out_image = out_class_vals
        if otherargs.out_score_img:
            outputs.out_score_img = out_score_vals

    print("Applying the Classifier")
    applier.apply(
        _apply_sk_classifier, in_files, outfiles, otherargs, controls=aControls
    )
    print("Completed")
    if gdalformat == "KEA":
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
    else:
        rsgislib.imageutils.pop_thmt_img_stats(
            output_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )

    if create_out_score_img:
        rsgislib.imageutils.pop_img_stats(
            out_score_img, use_no_data=True, no_data_val=0, calc_pyramids=True
        )

    if class_clr_names and (gdalformat == "KEA"):
        rat_dataset = gdal.Open(output_img, gdal.GA_Update)
        red = rat.readColumn(rat_dataset, "Red")
        green = rat.readColumn(rat_dataset, "Green")
        blue = rat.readColumn(rat_dataset, "Blue")
        class_names_col = numpy.empty_like(red, dtype=numpy.dtype("a255"))
        class_names_col[...] = ""

        for class_key in cls_train_info:
            print(f"Apply Colour to class '{class_key}'")
            red[cls_train_info[class_key].out_id] = cls_train_info[class_key].red
            green[cls_train_info[class_key].out_id] = cls_train_info[class_key].green
            blue[cls_train_info[class_key].out_id] = cls_train_info[class_key].blue
            class_names_col[cls_train_info[class_key].out_id] = class_key

        rat.writeColumn(rat_dataset, "Red", red)
        rat.writeColumn(rat_dataset, "Green", green)
        rat.writeColumn(rat_dataset, "Blue", blue)
        rat.writeColumn(rat_dataset, "class_names", class_names_col)
        rat_dataset = None


def apply_sklearn_classifier_rat(
    clumps_img: str,
    variables: List[str],
    sk_classifier: BaseEstimator,
    cls_train_info: Dict[str, ClassInfoObj],
    out_col_int: str = "OutClass",
    out_col_str: str = "OutClassName",
    roi_col: str = None,
    roi_val: int = 1,
    class_colours: bool = True,
):
    """
    A function which will apply an scikit-learn classifier within a Raster
    Attribute Table (RAT).

    :param clumps_img: is the clumps image on which the classification is to
                       be performed
    :param variables: is an array of column names which are to be used for
                      the classification
    :param sk_classifier: a trained instance of a scikit-learn classifier
    :param cls_train_info: dict (where the key is the class name) of
                             rsgislib.classification.ClassInfoObj objects which will be
                             used to train the classifier provide pixel value
                             id and RGB class values.
    :param out_col_int: is the output column name for the int class
                        representation (Default: 'OutClass')
    :param out_col_str: is the output column name for the class names
                        column (Default: 'OutClassName')
    :param roi_col: is a column name for a column which specifies the region
                    to be classified. If None ignored (Default: None)
    :param roi_val: is a int value used within the roi_col to select a
                    region to be classified (Default: 1)
    :param class_colours: is a boolean specifying whether the RAT colour table should
                          be updated using the classification colours (default: True)

    """

    def _apply_rat_classifier(info, inputs, outputs, otherargs):
        """
        This function is used internally within apply_sklearn_classifier_rat
        """
        numpy_vars = []
        for var in otherargs.vars:
            var_vals = getattr(inputs.inrat, var)
            numpy_vars.append(var_vals)

        x_data = numpy.array(numpy_vars)
        x_data = x_data.transpose()

        row_id = numpy.arange(x_data.shape[0])
        out_class_int_vals = numpy.zeros(x_data.shape[0], dtype=numpy.int16)
        out_class_names_vals = numpy.empty(x_data.shape[0], dtype=numpy.dtype("a255"))
        out_class_names_vals[...] = ""

        row_id = row_id[numpy.isfinite(x_data).all(axis=1)]
        v_data = x_data[numpy.isfinite(x_data).all(axis=1)]

        if otherargs.roi_col is not None:
            roi = getattr(inputs.inrat, otherargs.roi_col)
            roi = roi[numpy.isfinite(x_data).all(axis=1)]
            v_data = v_data[roi == otherargs.roi_val]
            row_id = row_id[roi == otherargs.roi_val]

        preds_idxs = otherargs.classifier.predict(v_data)

        preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
        for cld_id, idx in zip(
            otherargs.cls_id_lut, numpy.arange(0, len(otherargs.cls_id_lut))
        ):
            preds_cls_ids[preds_idxs == idx] = cld_id

        out_class_int_vals[row_id] = preds_cls_ids
        setattr(outputs.outrat, otherargs.out_col_int, out_class_int_vals)

        for cls_id in otherargs.cls_name_lut:
            out_class_names_vals[out_class_int_vals == cls_id] = otherargs.cls_name_lut[
                cls_id
            ]
        setattr(outputs.outrat, otherargs.out_col_str, out_class_names_vals)

        if otherargs.class_colours:
            red = getattr(inputs.inrat, "Red")
            green = getattr(inputs.inrat, "Green")
            blue = getattr(inputs.inrat, "Blue")

            # Set Background to black
            red[...] = 0
            green[...] = 0
            blue[...] = 0

            # Set colours
            for class_name in otherargs.class_train_info:
                cls_id = otherargs.class_train_info[class_name].out_id
                red = numpy.where(
                    out_class_int_vals == cls_id,
                    otherargs.class_train_info[class_name].red,
                    red,
                )
                green = numpy.where(
                    out_class_int_vals == cls_id,
                    otherargs.class_train_info[class_name].green,
                    green,
                )
                blue = numpy.where(
                    out_class_int_vals == cls_id,
                    otherargs.class_train_info[class_name].blue,
                    blue,
                )

            setattr(outputs.outrat, "Red", red)
            setattr(outputs.outrat, "Green", green)
            setattr(outputs.outrat, "Blue", blue)

    n_classes = len(cls_train_info)
    cls_id_lut = numpy.zeros(n_classes)
    cls_name_lut = dict()
    for cls_name in cls_train_info:
        if cls_train_info[cls_name].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    cls_name, cls_train_info[cls_name].id
                )
            )
        cls_id_lut[cls_train_info[cls_name].id] = cls_train_info[cls_name].out_id
        cls_name_lut[cls_train_info[cls_name].out_id] = cls_name

    in_rats = ratapplier.RatAssociations()
    out_rats = ratapplier.RatAssociations()
    in_rats.inrat = ratapplier.RatHandle(clumps_img)
    out_rats.outrat = ratapplier.RatHandle(clumps_img)

    otherargs = ratapplier.OtherArguments()
    otherargs.vars = variables
    otherargs.classifier = sk_classifier
    otherargs.out_col_int = out_col_int
    otherargs.out_col_str = out_col_str
    otherargs.roi_col = roi_col
    otherargs.roi_val = roi_val
    otherargs.n_classes = n_classes
    otherargs.cls_id_lut = cls_id_lut
    otherargs.cls_name_lut = cls_name_lut
    otherargs.class_colours = class_colours
    otherargs.class_train_info = cls_train_info

    try:
        import tqdm

        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    ratapplier.apply(
        _apply_rat_classifier, in_rats, out_rats, otherargs=otherargs, controls=None
    )
