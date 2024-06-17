#! /usr/bin/env python
############################################################################
#  classkeraschips.py
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
import h5py
import numpy
import tqdm

import rsgislib
import rsgislib.imagecalc
import rsgislib.rastergis


def train_keras_chips_pixel_classifier(
    cls_mdl, cls_info_dict, out_mdl_file=None, train_epochs=5, train_batch_size=32
):
    """
    A function which trains a neural network defined using the keras API for the classification of remotely sensed data.
    A dict of class information, as ClassInfoObj objects, is defined with the training data.

    This function requires that tensorflow and keras modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param cls_info_dict: dict (key is string with class name) of ClassInfoObj objects defining the training data.
    :param out_mdl_file: A file path to save the trained model as a hdf5 file. If None then ignored.
    :param train_epochs: The number of epochs to use for training
    :param train_batch_size: The batch size to use for training.

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
            sgl_cls_info["valid_n_rows"], dtype=int
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


def apply_keras_chips_pixel_classifier(
    class_train_info,
    keras_cls_mdl,
    in_img_mask,
    img_mask_val,
    img_file_info,
    chip_size,
    out_class_img,
    norm_function=None,
    gdalformat="KEA",
    class_clr_names=True,
):
    """
    This function applies a trained single pixel keras model to an image. The function train_keras_pixel_classifier
    can be used to train such as model. The output image will contain the hard membership of the predicted class.


    :param class_train_info: dict (where the key is the class name) of rsgislib.classification.ClassInfoObj
                           objects which will be used to train the classifier (i.e., train_keras_pixel_classifier()),
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
    :param norm_function: Normalisation function to apply before running classification
    :param chip_size: is the chip size to be extracted.
    :param gdalformat: is the output image format - all GDAL supported formats are supported.
    :param class_clr_names: default is True and therefore a colour table will the colours specified in ClassInfoObj
                          and a class_names (from classTrainInfo) column will be added to the output file.

    """
    from osgeo import gdal
    from rios import rat
    from rios.imagereader import ImageReader
    from rios.imagewriter import ImageWriter

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

    inImgs = list()
    inImgBands = list()

    inImgs.append(in_img_mask)
    inImgBands.append([1])
    n_img_bands = 0
    for inImgInfo in img_file_info:
        inImgs.append(inImgInfo.fileName)
        inImgBands.append(inImgInfo.bands)
        n_img_bands = n_img_bands + len(inImgInfo.bands)
    nImgs = len(img_file_info)

    scn_overlap = chip_size // 2

    writer = None
    reader = ImageReader(
        inImgs,
        windowxsize=chip_size,
        windowysize=chip_size,
        overlap=scn_overlap,
        layerselection=inImgBands,
    )
    feat2cls = None

    for info, block in tqdm.tqdm(reader):
        classMskArr = block[0]

        blk_shape = classMskArr.shape

        if feat2cls is None:
            feat2cls = numpy.zeros(
                [1, chip_size, chip_size, n_img_bands], dtype=numpy.float32
            )
        else:
            feat2cls[...] = 0

        chip_mask = classMskArr[
            0,
            scn_overlap : scn_overlap + chip_size,
            scn_overlap : scn_overlap + chip_size,
        ]
        # If there are no valid pixels in the block at all then no point running prediction
        # set all output valies to 0.
        if numpy.all(chip_mask != img_mask_val):
            out_cls_arr = numpy.zeros_like(classMskArr, dtype=numpy.uint16)
            out_cls_arr[...] = 0
        else:
            # Go through bands and reorder for prediction
            for nImg in range(nImgs):
                imgBlk = block[nImg + 1][:, :, :]
                for iBand in range(imgBlk.shape[0]):
                    feat2cls[0, :, :, iBand] = imgBlk[
                        iBand,
                        scn_overlap : scn_overlap + chip_size,
                        scn_overlap : scn_overlap + chip_size,
                    ]

            image_chip = feat2cls[0:1, :, :, :]
            # If a normalisation function has been specified then apply this
            if norm_function is not None:
                image_chip = norm_function(image_chip)

            predict_class = numpy.argmax(keras_cls_mdl.predict(image_chip), axis=1)

            # Update based on output class
            out_predict_class = numpy.empty_like(predict_class)
            for cld_id, idx in zip(cls_id_lut, numpy.arange(0, len(cls_id_lut))):
                out_predict_class[predict_class == idx] = cld_id

            # Assign all pixels in block to predicted class
            out_cls_arr = numpy.zeros_like(classMskArr, dtype=numpy.uint16)
            out_cls_arr[...] = out_predict_class

            # Apply mask to output pixels
            out_cls_arr[classMskArr != img_mask_val] = 0

        if writer is None:
            writer = ImageWriter(
                out_class_img, info=info, firstblock=out_cls_arr, drivername=gdalformat
            )
        else:
            writer.write(out_cls_arr)
    writer.close(calcStats=False)

    if class_clr_names:
        rsgislib.rastergis.pop_rat_img_stats(
            out_class_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
        max_val = rsgislib.imagecalc.get_img_band_min_max(out_class_img, 1, False, 0)[1]
        ratDataset = gdal.Open(out_class_img, gdal.GA_Update)

        max_cls_val = 0
        for classKey in class_train_info:
            if class_train_info[classKey].out_id > max_cls_val:
                max_cls_val = class_train_info[classKey].out_id

        if max_cls_val > max_val:
            red = numpy.random.randint(0, 255, max_cls_val + 1)
            green = numpy.random.randint(0, 255, max_cls_val + 1)
            blue = numpy.random.randint(0, 255, max_cls_val + 1)
        else:
            red = rat.readColumn(ratDataset, "Red")
            green = rat.readColumn(ratDataset, "Green")
            blue = rat.readColumn(ratDataset, "Blue")

        class_names = numpy.empty_like(red, dtype=numpy.dtype("a255"))
        class_names[...] = ""

        for classKey in class_train_info:
            print("Apply Colour to class '" + classKey + "'")
            red[class_train_info[classKey].out_id] = class_train_info[classKey].red
            green[class_train_info[classKey].out_id] = class_train_info[classKey].green
            blue[class_train_info[classKey].out_id] = class_train_info[classKey].blue
            class_names[class_train_info[classKey].out_id] = classKey

        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)
        rat.writeColumn(ratDataset, "class_names", class_names)
        ratDataset = None


def train_keras_chips_ref_classifier(
    cls_mdl,
    train_data_file,
    valid_data_file,
    test_data_file,
    n_classes,
    out_mdl_file=None,
    train_epochs=5,
    train_batch_size=32,
):
    """
    A function which trains a neural network defined using the keras API for the classification of remotely sensed data.

    This function requires that tensorflow and keras modules to be installed.

    :param cls_mdl: The Keras model which has already been defined.
    :param train_data_file: a HDF5 file with training data (needs to have elements /DATA/DATA and /DATA/REF)
    :param valid_data_file: a HDF5 file with validation data (needs to have elements /DATA/DATA and /DATA/REF)
    :param test_data_file: a HDF5 file with testing data (needs to have elements /DATA/DATA and /DATA/REF)
    :param n_classes: integer specifying the number of classes within the system
    :param out_mdl_file: A file path to save the trained model as a hdf5 file. If None then ignored.
    :param train_epochs: The number of epochs to use for training
    :param train_batch_size: The batch size to use for training.

    """
    try:
        from keras.utils import to_categorical
    except:
        from tensorflow.keras.utils import to_categorical

    f = h5py.File(train_data_file, "r")
    train_np = numpy.array(f["DATA/DATA"])
    train_lbls_np = numpy.array(f["DATA/REF"])
    f.close()
    train_lbls_keras = to_categorical(train_lbls_np, num_classes=n_classes)

    f = h5py.File(valid_data_file, "r")
    vaild_np = numpy.array(f["DATA/DATA"])
    vaild_lbls_np = numpy.array(f["DATA/REF"])
    f.close()
    vaild_lbls_keras = to_categorical(vaild_lbls_np, num_classes=n_classes)

    f = h5py.File(test_data_file, "r")
    test_np = numpy.array(f["DATA/DATA"])
    test_lbls_np = numpy.array(f["DATA/REF"])
    f.close()
    test_lbls_keras = to_categorical(test_lbls_np, num_classes=n_classes)
    print("Finished Reading Data")

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

    if out_mdl_file is not None:
        cls_mdl.save(out_mdl_file)

    loss_and_metrics = cls_mdl.evaluate(
        test_np, test_lbls_keras, batch_size=train_batch_size
    )
    eval_metric_names = cls_mdl.metrics_names
    for eval_name, eval_val in zip(eval_metric_names, loss_and_metrics):
        print("{} = {}".format(eval_name, eval_val))
