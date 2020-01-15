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

import numpy

import h5py

import osgeo.gdal as gdal

from rios import applier
from rios import cuiprogress
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