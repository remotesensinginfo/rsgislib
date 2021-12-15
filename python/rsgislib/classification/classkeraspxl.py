#! /usr/bin/env python
############################################################################
#  classkeraspxl.py
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
# Purpose:  Provide a set of functions to apply tensorflow keras classifiers
#           to remotely sensed imagery.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 14/01/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

from __future__ import print_function

import rsgislib
import rsgislib.rastergis

import numpy

import h5py

from osgeo import gdal

from rios import applier
from rios import cuiprogress
from rios import rat


def train_keras_pixel_classifer(
    cls_mdl, cls_info_dict, out_mdl_file=None, train_epochs=5, train_batch_size=32
):
    """
    A function which trains a neural network defined using the keras API for the classification of remotely sensed data.
    A dict of class information, as ClassInfoObj objects, is defined with the training data.

    This function requires that tensorflow and keras modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param cls_info_dict: dict (key is string with class name) of ClassInfoObj objects defining the training data.
    :param out_mdl_file: A file path to save the trained model as a hdf5 file. If None then ignored.

    """
    try:
        from keras.utils import to_categorical
    except:
        from tensorflow.keras.utils import to_categorical

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

    train_np = numpy.concatenate(train_data_lst)
    train_lbls_np = numpy.concatenate(train_lbls_lst)
    train_lbls_keras = to_categorical(train_lbls_np, num_classes=n_classes)

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbls_np = numpy.concatenate(valid_lbls_lst)
    vaild_lbls_keras = to_categorical(vaild_lbls_np, num_classes=n_classes)

    test_np = numpy.concatenate(test_data_lst)
    test_lbls_np = numpy.concatenate(test_lbls_lst)
    test_lbls_keras = to_categorical(test_lbls_np, num_classes=n_classes)

    print("Start Training Model")
    cls_mdl.fit(
        train_np,
        train_lbls_keras,
        epochs=train_epochs,
        batch_size=train_batch_size,
        validation_data=(vaild_np, vaild_lbls_keras),
    )
    print("Finished Training Model")
    cls_mdl.summary()

    loss_and_metrics = cls_mdl.evaluate(
        test_np, test_lbls_keras, batch_size=train_batch_size
    )
    eval_metric_names = cls_mdl.metrics_names
    for eval_name, eval_val in zip(eval_metric_names, loss_and_metrics):
        print("{} = {}".format(eval_name, eval_val))

    if out_mdl_file is not None:
        cls_mdl.save(out_mdl_file)


def apply_keras_pixel_classifier(
    class_train_info,
    keras_cls_mdl,
    in_img_mask,
    img_mask_val,
    img_file_info,
    out_class_img,
    gdalformat,
    pred_batch_size=32,
    class_clr_names=True,
):
    """
    This function applies a trained single pixel keras model to an image. The function train_keras_pixel_classifer
    can be used to train such as model. The output image will contain the hard membership of the predicted class.

    :param class_train_info: dict (where the key is the class name) of rsgislib.classification.ClassInfoObj
                           objects which will be used to train the classifier (i.e., train_keras_pixel_classifer()),
                           provide pixel value id and RGB class values.
    :param keras_cls_mdl: a trained keras model object, with a input dimensions equivlent to the number of image
                          bands specified in the imgFileInfo input and output layer which provides an output array
                          of the length of the number of classes.
    :param in_img_mask: is an image file providing a mask to specify where should be classified. Simplest mask is all the
                    valid data regions (rsgislib.imageutils.gen_valid_mask)
    :param img_mask_val: the pixel value within the imgMask to limit the region to which the classification is applied.
                       Can be used to create a heirachical classification.
    :param img_file_info: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                        rsgislib.zonalstats.extract_zone_img_band_values_to_hdf) to identify which images and bands are to
                        be used for the classification so it adheres to the training data.
    :param out_class_img: Output image which will contain the hard classification.
    :param gdalformat: is the output image format - all GDAL supported formats are supported.
    :param pred_batch_size: the batch size used for the classification.
    :param class_clr_names: default is True and therefore a colour table will the colours specified in ClassInfoObj
                          and a ClassName (from classTrainInfo) column will be added to the output file.

    """

    def _applyKerasPxlClassifier(info, inputs, outputs, otherargs):
        outClassIdVals = numpy.zeros_like(inputs.imageMask, dtype=numpy.uint16)
        if numpy.any(inputs.imageMask == otherargs.mskVal):
            n_pxls = inputs.imageMask.shape[1] * inputs.imageMask.shape[2]
            outClassIdVals = outClassIdVals.flatten()
            imgMaskVals = inputs.imageMask.flatten()
            classVars = numpy.zeros((n_pxls, otherargs.numClassVars), dtype=numpy.float)
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
            preds_idxs = numpy.argmax(
                otherargs.classifier.predict(
                    classVars, batch_size=otherargs.pred_batch_size
                ),
                axis=1,
            )
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

    infiles = applier.FilenameAssociations()
    infiles.imageMask = in_img_mask
    numClassVars = 0
    for imgFile in img_file_info:
        infiles.__dict__[imgFile.name] = imgFile.fileName
        numClassVars = numClassVars + len(imgFile.bands)

    n_classes = len(class_train_info)
    cls_id_lut = numpy.zeros(n_classes)
    for clsname in class_train_info:
        if class_train_info[clsname].id >= n_classes:
            raise rsgislib.RSGISPyException(
                "ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(
                    clsname, class_train_info[clsname].id
                )
            )
        cls_id_lut[class_train_info[clsname].id] = class_train_info[clsname].out_id

    outfiles = applier.FilenameAssociations()
    outfiles.outclsimage = out_class_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = keras_cls_mdl
    otherargs.pred_batch_size = pred_batch_size
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
    applier.apply(
        _applyKerasPxlClassifier, infiles, outfiles, otherargs, controls=aControls
    )
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

        for classKey in class_train_info:
            print("Apply Colour to class '" + classKey + "'")
            red[class_train_info[classKey].out_id] = class_train_info[classKey].red
            green[class_train_info[classKey].out_id] = class_train_info[classKey].green
            blue[class_train_info[classKey].out_id] = class_train_info[classKey].blue
            ClassName[class_train_info[classKey].out_id] = classKey

        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)
        rat.writeColumn(ratDataset, "ClassName", ClassName)
        ratDataset = None
