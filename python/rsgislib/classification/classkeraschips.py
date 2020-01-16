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

import rsgislib
import rsgislib.rastergis
import rsgislib.imagecalc

import tqdm

import numpy

import h5py

import osgeo.gdal as gdal

from rios.imagereader import ImageReader
from rios.imagewriter import ImageWriter
from rios import rat

import keras.utils


def train_keras_chips_pixel_classifer(cls_mdl, clsinfodict, out_mdl_file=None, train_epochs=5, train_batch_size=32):
    """
    A function which trains a neural network defined using the keras API for the classification of remotely sensed data.
    A dict of class information, as ClassInfoObj objects, is defined with the training data.

    This function requires that tensorflow and keras modules to be installed.

    :param out_mdl_file: The output model which can be loaded to perform a classification.
    :param clsinfodict: dict (key is string with class name) of ClassInfoObj objects defining the training data.
    :param out_mdl_file: A file path to save the trained model as a hdf5 file. If None then ignored.
    :param train_epochs: The number of epochs to use for training
    :param train_batch_size: The batch size to use for training.

    """
    n_classes = len(clsinfodict)
    for clsname in clsinfodict:
        if clsinfodict[clsname].id >= n_classes:
            raise ("ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(clsname,
                                                                                          clsinfodict[clsname].id))

    cls_data_dict = {}
    train_data_lst = []
    train_lbls_lst = []
    valid_data_lst = []
    valid_lbls_lst = []
    test_data_lst = []
    test_lbls_lst = []
    cls_ids = []
    for clsname in clsinfodict:
        sgl_cls_info = {}
        print("Reading Class {} Training".format(clsname))
        f = h5py.File(clsinfodict[clsname].trainfileH5, 'r')
        sgl_cls_info['train_n_rows'] = f['DATA/DATA'].shape[0]
        sgl_cls_info['train_data'] = numpy.array(f['DATA/DATA'])
        sgl_cls_info['train_data_lbls'] = numpy.zeros(sgl_cls_info['train_n_rows'], dtype=int)
        sgl_cls_info['train_data_lbls'][...] = clsinfodict[clsname].id
        f.close()
        train_data_lst.append(sgl_cls_info['train_data'])
        train_lbls_lst.append(sgl_cls_info['train_data_lbls'])

        print("Reading Class {} Validation".format(clsname))
        f = h5py.File(clsinfodict[clsname].validfileH5, 'r')
        sgl_cls_info['valid_n_rows'] = f['DATA/DATA'].shape[0]
        sgl_cls_info['valid_data'] = numpy.array(f['DATA/DATA'])
        sgl_cls_info['valid_data_lbls'] = numpy.zeros(sgl_cls_info['valid_n_rows'], dtype=int)
        sgl_cls_info['valid_data_lbls'][...] = clsinfodict[clsname].id
        f.close()
        valid_data_lst.append(sgl_cls_info['valid_data'])
        valid_lbls_lst.append(sgl_cls_info['valid_data_lbls'])

        print("Reading Class {} Testing".format(clsname))
        f = h5py.File(clsinfodict[clsname].testfileH5, 'r')
        sgl_cls_info['test_n_rows'] = f['DATA/DATA'].shape[0]
        sgl_cls_info['test_data'] = numpy.array(f['DATA/DATA'])
        sgl_cls_info['test_data_lbls'] = numpy.zeros(sgl_cls_info['valid_n_rows'], dtype=int)
        sgl_cls_info['test_data_lbls'][...] = clsinfodict[clsname].id
        f.close()
        test_data_lst.append(sgl_cls_info['test_data'])
        test_lbls_lst.append(sgl_cls_info['test_data_lbls'])

        cls_data_dict[clsname] = sgl_cls_info
        cls_ids.append(clsinfodict[clsname].id)

    print("Finished Reading Data")

    train_np = numpy.concatenate(train_data_lst)
    train_lbls_np = numpy.concatenate(train_lbls_lst)
    train_lbls_keras = keras.utils.to_categorical(train_lbls_np, num_classes=n_classes)

    vaild_np = numpy.concatenate(valid_data_lst)
    vaild_lbls_np = numpy.concatenate(valid_lbls_lst)
    vaild_lbls_keras = keras.utils.to_categorical(vaild_lbls_np, num_classes=n_classes)

    test_np = numpy.concatenate(test_data_lst)
    test_lbls_np = numpy.concatenate(test_lbls_lst)
    test_lbls_keras = keras.utils.to_categorical(test_lbls_np, num_classes=n_classes)

    print("Start Training Model")
    cls_mdl.fit(train_np, train_lbls_keras, epochs=train_epochs, batch_size=train_batch_size,
                validation_data=(vaild_np, vaild_lbls_keras))
    print("Finished Training Model")
    cls_mdl.summary()

    loss_and_metrics = cls_mdl.evaluate(test_np, test_lbls_keras, batch_size=train_batch_size)
    eval_metric_names = cls_mdl.metrics_names
    for eval_name, eval_val in zip(eval_metric_names, loss_and_metrics):
        print("{} = {}".format(eval_name, eval_val))

    if out_mdl_file is not None:
        cls_mdl.save(out_mdl_file)


def apply_keras_chips_pixel_classifier(classTrainInfo, keras_cls_mdl, imgMask, imgMaskVal, imgFileInfo,
                                       chip_h_size, outClassImg, gdalformat, pred_batch_size=128,
                                       pred_max_queue_size=10, pred_workers=1, pred_use_multiprocessing=False,
                                       classClrNames=True):
    """
This function applies a trained single pixel keras model to an image. The function train_keras_pixel_classifer
can be used to train such as model. The output image will contain the hard membership of the predicted class.

For pred_batch_size, pred_max_queue_size, pred_workers and pred_use_multiprocessing options see the keras
documentation https://keras.io/models/model/

:param classTrainInfo: dict (where the key is the class name) of rsgislib.classification.ClassInfoObj
                       objects which will be used to train the classifier (i.e., train_keras_pixel_classifer()),
                       provide pixel value id and RGB class values.
:param keras_cls_mdl: a trained keras model object, with a input dimensions equivlent to the number of image
                      bands specified in the imgFileInfo input and output layer which provides an output array
                      of the length of the number of classes.
:param imgMask: is an image file providing a mask to specify where should be classified. Simplest mask is all the
                valid data regions (rsgislib.imageutils.genValidMask)
:param imgMaskVal: the pixel value within the imgMask to limit the region to which the classification is applied.
                   Can be used to create a heirachical classification.
:param imgFileInfo: a list of rsgislib.imageutils.ImageBandInfo objects (also used within
                    rsgislib.imageutils.extractZoneImageBandValues2HDF) to identify which images and bands are to
                    be used for the classification so it adheres to the training data.
:param outClassImg: Output image which will contain the hard classification.
:param chip_h_size: is half the chip size to be extracted (i.e., 10 with output image chips 21x21,
                    10 pixels either size of the one of interest).
:param gdalformat: is the output image format - all GDAL supported formats are supported.
:param pred_batch_size: the batch size used for the classification prediction.
:param pred_max_queue_size: the max queue size used for the classification prediction
:param pred_workers: the number of workers used for the classification prediction
:param pred_use_multiprocessing: whether to use a multiprocessing option for the classification prediction
:param classClrNames: default is True and therefore a colour table will the colours specified in ClassInfoObj
                      and a ClassName (from classTrainInfo) column will be added to the output file.

    """
    n_classes = len(classTrainInfo)
    cls_id_lut = numpy.zeros(n_classes)
    for clsname in classTrainInfo:
        if classTrainInfo[clsname].id >= n_classes:
            raise ("ClassInfoObj '{}' id ({}) is not consecutive starting from 0.".format(clsname,
                                                                                          classTrainInfo[clsname].id))
        cls_id_lut[classTrainInfo[clsname].id] = classTrainInfo[clsname].out_id

    inImgs = list()
    inImgBands = list()

    inImgs.append(imgMask)
    inImgBands.append([1])
    n_img_bands = 0
    for inImgInfo in imgFileInfo:
        inImgs.append(inImgInfo.fileName)
        inImgBands.append(inImgInfo.bands)
        n_img_bands = n_img_bands + len(inImgInfo.bands)
    nImgs = len(imgFileInfo)

    scn_overlap = chip_h_size
    chip_size = (chip_h_size * 2) + 1

    writer = None
    reader = ImageReader(inImgs, windowxsize=200, windowysize=200, overlap=scn_overlap, layerselection=inImgBands)
    for (info, block) in tqdm.tqdm(reader):
        classMskArr = block[0]
        blkShape = classMskArr.shape

        vld_cls_arr = numpy.zeros_like(classMskArr, dtype=int)

        xSize = blkShape[2] - (scn_overlap * 2)
        ySize = blkShape[1] - (scn_overlap * 2)
        xRange = numpy.arange(scn_overlap, scn_overlap + xSize, 1)
        yRange = numpy.arange(scn_overlap, scn_overlap + ySize, 1)
        n_vld_pxls = 0
        for y in yRange:
            for x in xRange:
                if classMskArr[0][y][x] == imgMaskVal:
                    n_vld_pxls = n_vld_pxls + 1
                    vld_cls_arr[0][y][x] = 1

        feat2cls = numpy.zeros([n_vld_pxls, n_img_bands, chip_size, chip_size], dtype=numpy.float32)
        iFeat = 0
        for y in yRange:
            yMin = y - scn_overlap
            yMax = y + scn_overlap + 1
            for x in xRange:
                xMin = x - scn_overlap
                xMax = x + scn_overlap + 1
                if classMskArr[0][y][x] == imgMaskVal:
                    for nImg in range(nImgs):
                        imgBlk = block[nImg + 1][..., yMin:yMax, xMin:xMax]
                        for iBand in range(imgBlk.shape[0]):
                            numpy.copyto(feat2cls[iFeat, iBand], imgBlk[iBand], casting='safe')
                        iFeat = iFeat + 1

        preds_idxs = numpy.argmax(
            keras_cls_mdl.predict(feat2cls, batch_size=pred_batch_size, max_queue_size=pred_max_queue_size,
                                  workers=pred_workers, use_multiprocessing=pred_use_multiprocessing), axis=1)
        feat2cls = None

        out_cls_arr = numpy.zeros_like(classMskArr, dtype=numpy.uint16)
        out_cls_arr = out_cls_arr.flatten()
        vld_cls_arr = vld_cls_arr.flatten()
        ID = numpy.arange(out_cls_arr.shape[0])
        ID = ID[vld_cls_arr == 1]

        preds_cls_ids = numpy.zeros_like(preds_idxs, dtype=numpy.uint16)
        for cld_id, idx in zip(cls_id_lut, numpy.arange(0, len(cls_id_lut))):
            preds_cls_ids[preds_idxs == idx] = cld_id

        out_cls_arr[ID] = preds_cls_ids
        out_cls_arr = numpy.expand_dims(out_cls_arr.reshape((classMskArr.shape[1], classMskArr.shape[2])), axis=0)

        if writer is None:
            writer = ImageWriter(outClassImg, info=info, firstblock=out_cls_arr, drivername=gdalformat)
        else:
            writer.write(out_cls_arr)
    writer.close(calcStats=False)

    if classClrNames:
        rsgislib.rastergis.populateStats(outClassImg, addclrtab=True, calcpyramids=True, ignorezero=True)
        max_val = rsgislib.imagecalc.getImageBandMinMax(outClassImg, 1, False, 0)[1]
        ratDataset = gdal.Open(outClassImg, gdal.GA_Update)

        max_cls_val = 0
        for classKey in classTrainInfo:
            if classTrainInfo[classKey].out_id > max_cls_val:
                max_cls_val = classTrainInfo[classKey].out_id

        if max_cls_val > max_val:
            red = numpy.random.randint(0, 255, max_cls_val + 1)
            green = numpy.random.randint(0, 255, max_cls_val + 1)
            blue = numpy.random.randint(0, 255, max_cls_val + 1)
        else:
            red = rat.readColumn(ratDataset, 'Red')
            green = rat.readColumn(ratDataset, 'Green')
            blue = rat.readColumn(ratDataset, 'Blue')

        ClassName = numpy.empty_like(red, dtype=numpy.dtype('a255'))
        ClassName[...] = ""

        for classKey in classTrainInfo:
            print("Apply Colour to class \'" + classKey + "\'")
            red[classTrainInfo[classKey].out_id] = classTrainInfo[classKey].red
            green[classTrainInfo[classKey].out_id] = classTrainInfo[classKey].green
            blue[classTrainInfo[classKey].out_id] = classTrainInfo[classKey].blue
            ClassName[classTrainInfo[classKey].out_id] = classKey

        rat.writeColumn(ratDataset, "Red", red)
        rat.writeColumn(ratDataset, "Green", green)
        rat.writeColumn(ratDataset, "Blue", blue)
        rat.writeColumn(ratDataset, "ClassName", ClassName)
        ratDataset = None

