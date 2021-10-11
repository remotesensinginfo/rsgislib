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
from rios import ratapplier

import rsgislib

def extract_rat_col_data(clumps_img, cols, sel_col, sel_col_val, out_h5_file, datatype=None, rat_band=1):
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
    sel_col_vals = rat.readColumn(rat_dataset, sel_col)
    sel_col_sub_vals = sel_col_vals[sel_col_vals == sel_col_val]

    n_feats = sel_col_sub_vals.shape[0]
    print("n Feats: {}".format(n_feats))

    n_cols = len(cols)
    print("N Cols: {}".format(n_cols))

    out_vars_arr = numpy.zeros((n_feats, n_cols), dtype=h5_dtype)

    for i, col in enumerate(cols):
        col_data = rat.readColumn(rat_dataset, col)
        col_data = col_data[sel_col_vals == sel_col_val]
        out_vars_arr[..., i] = col_data

    rat_dataset = None

    chunk_len = 1000
    if n_feats < chunk_len:
        chunk_len = n_feats

    fH5Out = h5py.File(out_h5_file, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=out_vars_arr, chunks=(chunk_len, n_cols), compression="gzip", shuffle=True, dtype=h5_dtype)
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S255")
    describDS[0] = 'Extracted from: '.format(os.path.basename(clumps_img)).encode()
    fH5Out.close()





