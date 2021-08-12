#!/usr/bin/env python
"""
The vector attributes module performs attribute table operations on vectors.
"""

import os
import math

import osgeo.gdal as gdal
import osgeo.ogr as ogr


def writeVecColumn(vec_file, vec_lyr, att_column, att_col_datatype, att_col_data):
    """
    A function which will write a column to a vector file

    Where:

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be added.
    :param att_column: Name of the output column
    :param att_col_datatype: ogr data type for the output column
                            (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
    :param att_col_data: A list of the same length as the number of features
                        in vector file.

    """
    gdal.UseExceptions()

    ds = gdal.OpenEx(vec_file, gdal.OF_UPDATE)
    if ds is None:
        raise Exception("Could not open '{}'".format(vec_file))

    lyr = ds.GetLayerByName(vec_lyr)
    if lyr is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    numFeats = lyr.GetFeatureCount()
    if not len(att_col_data) == numFeats:
        print("Number of Features: {}".format(numFeats))
        print("Length of Data: {}".format(len(att_col_data)))
        raise Exception(
            "The number of features and size of " "the input data is not equal."
        )

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn(att_column, att_col_datatype)
        if lyr.CreateField(field_defn) != 0:
            raise Exception(
                "Creating '{}' field failed; becareful with case, "
                "some drivers are case insensitive but column might "
                "not be found.".format(att_column)
            )

    lyr.ResetReading()
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids = []
    for feat in lyr:
        fids.append(feat.GetFID())

    openTransaction = False
    lyr.ResetReading()
    i = 0
    try:
        # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
        for fid in fids:
            if not openTransaction:
                lyr.StartTransaction()
                openTransaction = True
            feat = lyr.GetFeature(fid)
            if feat is not None:
                feat.SetField(att_column, att_col_data[i])
                lyr.SetFeature(feat)
            if ((i % 20000) == 0) and openTransaction:
                lyr.CommitTransaction()
                openTransaction = False
            i = i + 1
        if openTransaction:
            lyr.CommitTransaction()
            openTransaction = False
        lyr.SyncToDisk()
        ds = None
    except Exception as e:
        if i < numFeats:
            print(
                "Data type of the value being "
                "written is '{}'".format(type(att_col_data[i]))
            )
        raise e


def writeVecColumn2Layer(lyr, att_column, att_col_datatype, att_col_data):
    """
    A function which will write a column to a vector layer.

    Where:

    :param lyr: GDAL/OGR vector layer object
    :param att_column: Name of the output column
    :param att_col_datatype: ogr data type for the output column
                            (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
    :param att_col_data: A list of the same length as the number of features
                         in vector file.
    """
    gdal.UseExceptions()

    if lyr is None:
        raise Exception("The layer passed in is None...")

    numFeats = lyr.GetFeatureCount()
    if not len(att_col_data) == numFeats:
        print("Number of Features: {}".format(numFeats))
        print("Length of Data: {}".format(len(att_col_data)))
        raise Exception(
            "The number of features and size of the input data is not equal."
        )

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn(att_column, att_col_datatype)
        if lyr.CreateField(field_defn) != 0:
            raise Exception(
                "Creating '{}' field failed; be careful with case, some "
                "drivers are case insensitive but column might not "
                "be found.".format(att_column)
            )

    lyr.ResetReading()
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids = []
    for feat in lyr:
        fids.append(feat.GetFID())

    openTransaction = False
    lyr.ResetReading()
    i = 0
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    for fid in fids:
        if not openTransaction:
            lyr.StartTransaction()
            openTransaction = True
        feat = lyr.GetFeature(fid)
        if feat is not None:
            feat.SetField(att_column, att_col_data[i])
            lyr.SetFeature(feat)
        if ((i % 20000) == 0) and openTransaction:
            lyr.CommitTransaction()
            openTransaction = False
        i = i + 1
    if openTransaction:
        lyr.CommitTransaction()
        openTransaction = False


def readVecColumn(vec_file, vec_lyr, att_column):
    """
    A function which will reads a column from a vector file

    Where:

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be read from.
    :param att_column: Name of the input column
    """
    gdal.UseExceptions()

    ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if ds is None:
        raise Exception("Could not open '{}'".format(vec_file))

    lyr = ds.GetLayerByName(vec_lyr)
    if lyr is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName() == att_column:
            colExists = True
            break

    if not colExists:
        ds = None
        raise Exception(
            "The specified column does not exist in the input layer; "
            "check case as some drivers are case sensitive."
        )

    outVal = list()
    lyr.ResetReading()
    for feat in lyr:
        outVal.append(feat.GetField(att_column))
    ds = None

    return outVal


def readVecColumns(vec_file, vec_lyr, att_columns):
    """
    A function which will reads a column from a vector file

    Where:

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be read from.
    :param att_columns: List of input attribute column names to be read in.
    """
    gdal.UseExceptions()

    ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if ds is None:
        raise Exception("Could not open '{}'".format(vec_file))

    lyr = ds.GetLayerByName(vec_lyr)
    if lyr is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    lyrDefn = lyr.GetLayerDefn()

    feat_idxs = dict()
    feat_types = dict()
    found_atts = dict()
    for attName in att_columns:
        found_atts[attName] = False

    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName() in att_columns:
            attName = lyrDefn.GetFieldDefn(i).GetName()
            feat_idxs[attName] = i
            feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
            found_atts[attName] = True

    for attName in att_columns:
        if not found_atts[attName]:
            ds = None
            raise Exception(
                "Could not find the attribute ({}) specified "
                "within the vector layer.".format(attName)
            )

    outvals = []
    lyr.ResetReading()
    for feat in lyr:
        outdict = dict()
        for attName in att_columns:
            if feat_types[attName] == ogr.OFTString:
                outdict[attName] = feat.GetFieldAsString(feat_idxs[attName])
            elif feat_types[attName] == ogr.OFTReal:
                outdict[attName] = feat.GetFieldAsDouble(feat_idxs[attName])
            elif feat_types[attName] == ogr.OFTInteger:
                outdict[attName] = feat.GetFieldAsInteger(feat_idxs[attName])
            else:
                outdict[attName] = feat.GetField(feat_idxs[attName])
        outvals.append(outdict)
    ds = None

    return outvals


def popBBOXCols(vec_file, vec_lyr, xminCol='xmin', xmaxCol='xmax', yminCol='ymin',
                ymaxCol='ymax'):
    """
    A function which adds a polygons boundary bbox as attributes to each feature.

    :param vec_file: vector file.
    :param vec_lyr: layer within the vector file.
    :param xminCol: column name.
    :param xmaxCol: column name.
    :param yminCol: column name.
    :param ymaxCol: column name.

    """
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_UPDATE)
    if dsVecFile is None:
        raise Exception("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    xminCol_exists = False
    xmaxCol_exists = False
    yminCol_exists = False
    ymaxCol_exists = False

    lyrDefn = vec_lyr_obj.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName() == xminCol:
            xminCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == xmaxCol:
            xmaxCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == yminCol:
            yminCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == ymaxCol:
            ymaxCol_exists = True

    if not xminCol_exists:
        xmin_field_defn = ogr.FieldDefn(xminCol, ogr.OFTReal)
        if vec_lyr_obj.CreateField(xmin_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(xminCol))

    if not xmaxCol_exists:
        xmax_field_defn = ogr.FieldDefn(xmaxCol, ogr.OFTReal)
        if vec_lyr_obj.CreateField(xmax_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(xmaxCol))

    if not yminCol_exists:
        ymin_field_defn = ogr.FieldDefn(yminCol, ogr.OFTReal)
        if vec_lyr_obj.CreateField(ymin_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(yminCol))

    if not ymaxCol_exists:
        ymax_field_defn = ogr.FieldDefn(ymaxCol, ogr.OFTReal)
        if vec_lyr_obj.CreateField(ymax_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(ymaxCol))

    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids = []
    for feat in vec_lyr_obj:
        fids.append(feat.GetFID())

    openTransaction = False
    nFeats = vec_lyr_obj.GetFeatureCount(True)
    step = math.floor(nFeats / 10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    vec_lyr_obj.ResetReading()
    for fid in fids:
        # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
        feat = vec_lyr_obj.GetFeature(fid)
        if (nFeats > 10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10

        if not openTransaction:
            vec_lyr_obj.StartTransaction()
            openTransaction = True

        geom = feat.GetGeometryRef()
        if geom is not None:
            env = geom.GetEnvelope()
            feat.SetField(xminCol, env[0])
            feat.SetField(xmaxCol, env[1])
            feat.SetField(yminCol, env[2])
            feat.SetField(ymaxCol, env[3])
        else:
            feat.SetField(xminCol, 0.0)
            feat.SetField(xmaxCol, 0.0)
            feat.SetField(yminCol, 0.0)
            feat.SetField(ymaxCol, 0.0)
        rtn_val = vec_lyr_obj.SetFeature(feat)
        if rtn_val != ogr.OGRERR_NONE:
            raise Exception("An error has occurred setting a feature on a layer.")
        if ((counter % 20000) == 0) and openTransaction:
            vec_lyr_obj.CommitTransaction()
            openTransaction = False
        counter = counter + 1
    if openTransaction:
        vec_lyr_obj.CommitTransaction()
        openTransaction = False
    vec_lyr_obj.SyncToDisk()
    dsVecFile = None
    print(" Completed")


def createNameCol(vec_file, vec_lyr, vec_out_file, vec_out_lyr, out_format='GPKG', out_col='names', x_col='MinX',
                  y_col='MaxY', prefix='', postfix='', latlong=True, int_coords=True, zero_x_pad=0, zero_y_pad=0,
                  round_n_digts=0, non_neg=False):
    """
    A function which creates a column in the vector layer which can define a name using coordinates associated
    with the feature. Often this is useful if a tiling has been created and from this a set of images are to
    generated for example.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer name
    :param vec_out_file: output vector file
    :param vec_out_lyr: output vector layer name
    :param out_format: The output format of the output file. (Default: GPKG)
    :param out_col: The name of the output column
    :param x_col: The column with the x coordinate
    :param y_col: The column with the y coordinate
    :param prefix: A prefix to the name
    :param postfix: A postfix to the name
    :param latlong: A boolean specifying if the coordinates are lat / long
    :param int_coords: A boolean specifying whether to integise the coordinates.
    :param zero_x_pad: An integer, if larger than zero then the X coordinate will be zero padded.
    :param zero_y_pad: An integer, if larger than zero then the Y coordinate will be zero padded.
    :param round_n_digts: An integer, if larger than zero then the coordinates will be rounded to n significant digits
    :param non_neg: boolean specifying whether an negative coordinates should be made positive. (Default: False)

    """
    import geopandas
    import numpy
    import tqdm
    import rsgislib.tools.utils

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    names = list()
    for i in tqdm.tqdm(range(base_gpdf.shape[0])):
        x_col_val = base_gpdf.loc[i][x_col]
        y_col_val = base_gpdf.loc[i][y_col]

        x_col_val_neg = False
        y_col_val_neg = False
        if non_neg:
            if x_col_val < 0:
                x_col_val_neg = True
                x_col_val = x_col_val * (-1)
            if y_col_val < 0:
                y_col_val_neg = True
                y_col_val = y_col_val * (-1)

        if zero_x_pad > 0:
            x_col_val_str = rsgislib.tools.utils.zero_pad_num_str(x_col_val, str_len=zero_x_pad, round_num=False,
                                                         round_n_digts=round_n_digts, integerise=int_coords)
        else:
            x_col_val = int(x_col_val)
            x_col_val_str = '{}'.format(x_col_val)

        if zero_y_pad > 0:
            y_col_val_str = rsgislib.tools.utils.zero_pad_num_str(y_col_val, str_len=zero_y_pad, round_num=False,
                                                         round_n_digts=round_n_digts, integerise=int_coords)
        else:
            y_col_val = int(y_col_val)
            y_col_val_str = '{}'.format(y_col_val)

        if latlong:
            hemi = 'N'
            if y_col_val_neg:
                hemi = 'S'
            east_west = 'E'
            if x_col_val_neg:
                east_west = 'W'
            name = '{}{}{}{}{}{}'.format(prefix, hemi, y_col_val_str, east_west, x_col_val_str, postfix)
        else:
            name = '{}E{}N{}{}'.format(prefix, x_col_val_str, y_col_val_str, postfix)

        names.append(name)

    base_gpdf[out_col] = numpy.array(names)

    if out_format == 'GPKG':
        base_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        base_gpdf.to_file(vec_out_file, driver=out_format)




def add_unq_numeric_col(vec_file, vec_lyr, unq_col, out_col, vec_out_file, vec_out_lyr, out_format='GPKG'):
    """
    A function which adds a numeric column based off an existing column in the vector file.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param unq_col: The column within which the unique values will be identified.
    :param out_col: The output numeric column
    :param vec_out_file: Output vector file
    :param vec_out_lyr: output vector layer name.
    :param out_format: output file format (default GPKG).

    """
    import geopandas
    import numpy

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    unq_vals = base_gpdf[unq_col].unique()

    base_gpdf[out_col] = numpy.zeros((base_gpdf.shape[0]), dtype=int)
    num_unq_val = 1
    for unq_val in unq_vals:
        sel_rows = base_gpdf[unq_col] == unq_val
        base_gpdf.loc[sel_rows, out_col] = num_unq_val
        num_unq_val += 1

    if out_format == 'GPKG':
        base_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        base_gpdf.to_file(vec_out_file, driver=out_format)


def add_numeric_col_lut(vec_file, vec_lyr, ref_col, val_lut, out_col, vec_out_file, vec_out_lyr, out_format='GPKG'):
    """
    A function which adds a numeric column based off an existing column in the vector file,
    using an dict LUT to define the values.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param ref_col: The column within which the unique values will be identified.
    :param val_lut: A dict LUT (key should be value in ref_col and value be the value outputted to out_col).
    :param out_col: The output numeric column
    :param vec_out_file: Output vector file
    :param vec_out_lyr: output vector layer name.
    :param out_format: output file format (default GPKG).

    """
    import geopandas
    import numpy
    # Open vector file
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    # Add output column
    base_gpdf[out_col] = numpy.zeros((base_gpdf.shape[0]), dtype=int)
    # Loop values in LUT
    for lut_key in val_lut:
        sel_rows = base_gpdf[ref_col] == lut_key
        base_gpdf.loc[sel_rows, out_col] = val_lut[lut_key]

    if out_format == 'GPKG':
        base_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        base_gpdf.to_file(vec_out_file, driver=out_format)


def add_numeric_col(vec_file, vec_lyr, out_col, vec_out_file, vec_out_lyr, out_val=1, out_format='GPKG'):
    """
    A function which adds a numeric column with the same value for all the features.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param out_col: The output numeric column
    :param vec_out_file: Output vector file
    :param vec_out_lyr: output vector layer name.
    :param out_val: output numeric value
    :param out_format: output file format (default GPKG).

    """
    import geopandas
    import numpy

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    base_gpdf[out_col] = numpy.zeros((base_gpdf.shape[0]), dtype=int)
    base_gpdf[out_col] = out_val

    if out_format == 'GPKG':
        base_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        base_gpdf.to_file(vec_out_file, driver=out_format)


def add_string_col(vec_file, vec_lyr, out_col, vec_out_file, vec_out_lyr, out_val='str_val', out_format='GPKG'):
    """
    A function which adds a string column with the same value for all the features.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param out_col: The output numeric column
    :param vec_out_file: Output vector file
    :param vec_out_lyr: output vector layer name.
    :param out_val: output numeric value
    :param out_format: output file format (default GPKG).

    """
    import geopandas
    import numpy

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    str_col = numpy.empty((base_gpdf.shape[0]), dtype=object)
    str_col[...] = out_val

    base_gpdf[out_col] = str_col

    if out_format == 'GPKG':
        base_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        base_gpdf.to_file(vec_out_file, driver=out_format)

