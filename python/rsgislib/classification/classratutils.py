#! /usr/bin/env python
############################################################################
#  classratutils.py
#
#  Copyright 2015 RSGISLib.
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
# Date: 28/11/2015
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################
import os

import osgeo.gdal as gdal
import numpy
from rios import rat

import rsgislib


def populate_clumps_with_class_training(
    clumps_img: str,
    classes_info: list,
    tmp_dir: str,
    classes_int_col: str,
    classes_name_col: str,
    rat_band: int = 1,
):
    """
    A function to populate a clumps file with training from a series of
    vector layers (1 per class)

    :param clumps_img: input clumps file.
    :param classes_info: A list of rsgislb.classification.ClassVecSamplesInfoObj
                         objects. Note, the file_h5 variable is not needed in
                         this function.
    :param tmp_dir: File path (which needs to exist) where files can
                    temporally be written.
    :param classes_int_col: Output column name for integer values representing
                            each class.
    :param classes_name_col: Output column name for string class names.
    :param rat_band: The band within the input image the RAT is associated with.

    """
    import rsgislib.imageutils
    import rsgislib.rastergis
    import rsgislib.vectorutils.createrasters
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import shutil

    createdDIR = False
    if not os.path.isdir(tmp_dir):
        os.makedirs(tmp_dir)
        createdDIR = True

    uid = rsgislib.tools.utils.uid_generator(10)

    classLayerSeq = list()
    tmpClassImgLayers = list()
    classNamesDict = dict()

    for cls_info in classes_info:

        tmp_cls_img = os.path.join(
            tmp_dir, "{}_{}.kea".format(cls_info.class_name, uid)
        )
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
            cls_info.vec_file,
            cls_info.vec_lyr,
            clumps_img,
            tmp_cls_img,
            gdalformat="KEA",
            burn_val=cls_info.id,
            datatype=rsgislib.TYPE_16UINT,
        )
        tmpClassImgLayers.append(tmp_cls_img)
        classNamesDict[cls_info.id] = cls_info.class_name

    combinedClassesImage = os.path.join(tmp_dir, "CombinedClasses_{}.kea".format(uid))
    rsgislib.imageutils.combine_imgs_to_band(
        tmpClassImgLayers, combinedClassesImage, "KEA", rsgislib.TYPE_8UINT, 0.0
    )

    rsgislib.rastergis.populate_rat_with_mode(
        input_img=combinedClassesImage,
        clumps_img=clumps_img,
        out_cols_name=classes_int_col,
        use_no_data=False,
        no_data_val=0,
        out_no_data=False,
        mode_band=1,
        rat_band=rat_band,
    )
    rsgislib.rastergis.define_class_names(
        clumps_img, classes_int_col, classes_name_col, classNamesDict
    )

    for file in tmpClassImgLayers:
        rsgislib.tools.filetools.delete_file_with_basename(file)
    rsgislib.tools.filetools.delete_file_with_basename(combinedClassesImage)

    if createdDIR:
        shutil.rmtree(tmp_dir)


def extract_rat_col_data(
    clumps_img: str,
    cols: list,
    sel_col: str,
    sel_col_val: str,
    out_h5_file: str,
    datatype: int = None,
    rat_band: int = 1,
):
    """
    A function which extracts column values to be used as training, testing, validation
    sets for building a classifier. The data will be saved within a HDF5 file. Note,
    this function reads each whole column into memory and then subsets it so for very
    large RATs this might cause problems.

    :param clumps_img: The inputted clumps file with the associated RAT.
    :param cols: a list of the columns to be exported. Note, they will be stored in the
                 HDF5 file in the order specified here and therefore this order is
                 important and needs to be maintained when this is used going forward
                 (e.g., when applying a classifer trained using this data.
    :param sel_col: The column within the RAT specifying which rows will be exported.
    :param sel_col_val: The value in the sel_col which indicated which rows are to be
                        exported.
    :param out_h5_file: The output HDF5 file the data will be exported to.
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.
    :param rat_band: The band within the input image the RAT is associated with.

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT
    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    rat_dataset = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    sel_col_vals = rat.readColumn(rat_dataset, sel_col, bandNumber=rat_band)
    sel_col_sub_vals = sel_col_vals[sel_col_vals == sel_col_val]

    n_feats = sel_col_sub_vals.shape[0]
    print("n feats: {}".format(n_feats))

    n_cols = len(cols)
    print("n cols: {}".format(n_cols))

    out_vars_arr = numpy.zeros((n_feats, n_cols), dtype=h5_dtype)

    for i, col in enumerate(cols):
        col_data = rat.readColumn(rat_dataset, col, bandNumber=rat_band)
        col_data = col_data[sel_col_vals == sel_col_val]
        out_vars_arr[..., i] = col_data

    rat_dataset = None

    chunk_len = 1000
    if n_feats < chunk_len:
        chunk_len = n_feats

    fH5Out = h5py.File(out_h5_file, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=out_vars_arr,
        chunks=(chunk_len, n_cols),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S255")
    describDS[0] = "Extracted from: ".format(os.path.basename(clumps_img)).encode()
    fH5Out.close()
