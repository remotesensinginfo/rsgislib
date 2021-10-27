#!/usr/bin/env python
"""
The vector attributes module performs attribute table operations on vectors.
"""

import os
import math

import osgeo.gdal as gdal
import osgeo.ogr
import osgeo.ogr as ogr


def write_vec_column(
    out_vec_file: str,
    out_vec_lyr: str,
    att_column: str,
    att_col_datatype: int,
    att_col_data: list,
):
    """
    A function which will write a column to a vector file

    :param out_vec_file: The file / path to the vector data 'file'.
    :param out_vec_lyr: The layer to which the data is to be added.
    :param att_column: Name of the output column
    :param att_col_datatype: ogr data type for the output column
                            (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
    :param att_col_data: A list of the same length as the number of features
                        in vector file.

    """
    gdal.UseExceptions()

    ds = gdal.OpenEx(out_vec_file, gdal.OF_UPDATE)
    if ds is None:
        raise Exception("Could not open '{}'".format(out_vec_file))

    lyr = ds.GetLayerByName(out_vec_lyr)
    if lyr is None:
        raise Exception("Could not find layer '{}'".format(out_vec_lyr))

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
                "Creating '{}' field failed; be careful with case, "
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


def write_vec_column_to_layer(
    out_vec_lyr_obj: osgeo.ogr.Layer,
    att_column: str,
    att_col_datatype: int,
    att_col_data: list,
):
    """
    A function which will write a column to a vector layer.

    :param out_vec_lyr_obj: GDAL/OGR vector layer object
    :param att_column: Name of the output column
    :param att_col_datatype: ogr data type for the output column
                            (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
    :param att_col_data: A list of the same length as the number of features
                         in vector file.

    """
    gdal.UseExceptions()

    if out_vec_lyr_obj is None:
        raise Exception("The layer passed in is None...")

    numFeats = out_vec_lyr_obj.GetFeatureCount()
    if not len(att_col_data) == numFeats:
        print("Number of Features: {}".format(numFeats))
        print("Length of Data: {}".format(len(att_col_data)))
        raise Exception(
            "The number of features and size of the input data is not equal."
        )

    colExists = False
    lyrDefn = out_vec_lyr_obj.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn(att_column, att_col_datatype)
        if out_vec_lyr_obj.CreateField(field_defn) != 0:
            raise Exception(
                "Creating '{}' field failed; be careful with case, some "
                "drivers are case insensitive but column might not "
                "be found.".format(att_column)
            )

    out_vec_lyr_obj.ResetReading()
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids = []
    for feat in out_vec_lyr_obj:
        fids.append(feat.GetFID())

    openTransaction = False
    out_vec_lyr_obj.ResetReading()
    i = 0
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    for fid in fids:
        if not openTransaction:
            out_vec_lyr_obj.StartTransaction()
            openTransaction = True
        feat = out_vec_lyr_obj.GetFeature(fid)
        if feat is not None:
            feat.SetField(att_column, att_col_data[i])
            out_vec_lyr_obj.SetFeature(feat)
        if ((i % 20000) == 0) and openTransaction:
            out_vec_lyr_obj.CommitTransaction()
            openTransaction = False
        i = i + 1
    if openTransaction:
        out_vec_lyr_obj.CommitTransaction()
        openTransaction = False


def read_vec_column(vec_file: str, vec_lyr: str, att_column: str):
    """
    A function which will reads a column from a vector file

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be read from.
    :param att_column: Name of the input column
    :returns: a list with the column values.

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


def read_vec_columns(vec_file: str, vec_lyr: str, att_columns: list):
    """
    A function which will reads a number of column from a vector file

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be read from.
    :param att_columns: List of input attribute column names to be read in.
    :returns: dict with the column names as keys and a list of values for each column

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


def pop_bbox_cols(
    vec_file: str,
    vec_lyr: str,
    x_min_col: str = "xmin",
    x_max_col: str = "xmax",
    y_min_col: str = "ymin",
    y_max_col: str = "ymax",
):
    """
    A function which adds a polygons boundary bbox as attributes to each feature.

    :param vec_file: vector file.
    :param vec_lyr: layer within the vector file.
    :param x_min_col: output column name.
    :param x_max_col: output column name.
    :param y_min_col: output column name.
    :param y_max_col: output column name.

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
        if lyrDefn.GetFieldDefn(i).GetName() == x_min_col:
            xminCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == x_max_col:
            xmaxCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == y_min_col:
            yminCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == y_max_col:
            ymaxCol_exists = True

    if not xminCol_exists:
        xmin_field_defn = ogr.FieldDefn(x_min_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(xmin_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(x_min_col))

    if not xmaxCol_exists:
        xmax_field_defn = ogr.FieldDefn(x_max_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(xmax_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(x_max_col))

    if not yminCol_exists:
        ymin_field_defn = ogr.FieldDefn(y_min_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(ymin_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(y_min_col))

    if not ymaxCol_exists:
        ymax_field_defn = ogr.FieldDefn(y_max_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(ymax_field_defn) != 0:
            raise Exception("Creating '{}' field failed.".format(y_max_col))

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
    print("Started .0.", end="", flush=True)
    vec_lyr_obj.ResetReading()
    for fid in fids:
        # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
        feat = vec_lyr_obj.GetFeature(fid)
        if (nFeats > 10) and (counter == feedback_next):
            print(".{}.".format(feedback), end="", flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10

        if not openTransaction:
            vec_lyr_obj.StartTransaction()
            openTransaction = True

        geom = feat.GetGeometryRef()
        if geom is not None:
            env = geom.GetEnvelope()
            feat.SetField(x_min_col, env[0])
            feat.SetField(x_max_col, env[1])
            feat.SetField(y_min_col, env[2])
            feat.SetField(y_max_col, env[3])
        else:
            feat.SetField(x_min_col, 0.0)
            feat.SetField(x_max_col, 0.0)
            feat.SetField(y_min_col, 0.0)
            feat.SetField(y_max_col, 0.0)
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


def create_name_col(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    out_col: str = "names",
    x_col: str = "MinX",
    y_col: str = "MaxY",
    prefix: str = "",
    postfix: str = "",
    coords_lat_lon: bool = True,
    int_coords: bool = True,
    zero_x_pad: int = 0,
    zero_y_pad: int = 0,
    round_n_digts: int = 0,
    non_neg: bool = False,
):
    """
    A function which creates a column in the vector layer which can define a name
    using coordinates associated with the feature. Often this is useful if a tiling
    has been created and from this a set of images are to generated for example.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer name
    :param out_vec_file: output vector file
    :param out_vec_lyr: output vector layer name
    :param out_format: The output format of the output file. (Default: GPKG)
    :param out_col: The name of the output column
    :param x_col: The column with the x coordinate
    :param y_col: The column with the y coordinate
    :param prefix: A prefix to the name
    :param postfix: A postfix to the name
    :param coords_lat_lon: A boolean specifying if the coordinates are lat / long
    :param int_coords: A boolean specifying whether to integerise the coordinates.
    :param zero_x_pad: If larger than zero then the X coordinate will be zero padded.
    :param zero_y_pad: If larger than zero then the Y coordinate will be zero padded.
    :param round_n_digts: If larger than zero then the coordinates will be rounded
                          to n significant digits
    :param non_neg: boolean specifying whether an negative coordinates should be
                    made positive. (Default: False)

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
            x_col_val_str = rsgislib.tools.utils.zero_pad_num_str(
                x_col_val,
                str_len=zero_x_pad,
                round_num=False,
                round_n_digts=round_n_digts,
                integerise=int_coords,
            )
        else:
            x_col_val = int(x_col_val)
            x_col_val_str = "{}".format(x_col_val)

        if zero_y_pad > 0:
            y_col_val_str = rsgislib.tools.utils.zero_pad_num_str(
                y_col_val,
                str_len=zero_y_pad,
                round_num=False,
                round_n_digts=round_n_digts,
                integerise=int_coords,
            )
        else:
            y_col_val = int(y_col_val)
            y_col_val_str = "{}".format(y_col_val)

        if coords_lat_lon:
            hemi = "N"
            if y_col_val_neg:
                hemi = "S"
            east_west = "E"
            if x_col_val_neg:
                east_west = "W"
            name = "{}{}{}{}{}{}".format(
                prefix, hemi, y_col_val_str, east_west, x_col_val_str, postfix
            )
        else:
            name = "{}E{}N{}{}".format(prefix, x_col_val_str, y_col_val_str, postfix)

        names.append(name)

    base_gpdf[out_col] = numpy.array(names)

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def add_unq_numeric_col(
    vec_file: str,
    vec_lyr: str,
    unq_col: str,
    out_col: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which adds a numeric column based off an existing column in
    the vector file.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param unq_col: The column within which the unique values will be identified.
    :param out_col: The output numeric column
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
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

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def add_numeric_col_lut(
    vec_file: str,
    vec_lyr: str,
    ref_col: str,
    val_lut: dict,
    out_col: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which adds a numeric column based off an existing column in the
    vector file, using an dict LUT to define the values.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param ref_col: The column within which the unique values will be identified.
    :param val_lut: A dict LUT (key should be value in ref_col and value be the
                    value outputted to out_col).
    :param out_col: The output numeric column
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
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

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def add_numeric_col(
    vec_file: str,
    vec_lyr: str,
    out_col: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_val: float = 1,
    out_format: str = "GPKG",
    out_col_int: float = False,
):
    """
    A function which adds a numeric column with the same value for all the features.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param out_col: The output numeric column
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
    :param out_val: output numeric value
    :param out_format: output file format (default GPKG).
    :param out_col_int: Specify whether the output column should be an int datatype.
                        If True (default: False) then the output column will be of
                        type int. If False then it will be type float.

    """
    import geopandas
    import numpy

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    if out_col_int:
        base_gpdf[out_col] = numpy.full((base_gpdf.shape[0]), out_val, dtype=int)
    else:
        base_gpdf[out_col] = numpy.full((base_gpdf.shape[0]), out_val, dtype=float)

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def add_string_col(
    vec_file: str,
    vec_lyr: str,
    out_col: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_val: str = "str_val",
    out_format: str = "GPKG",
):
    """
    A function which adds a string column with the same value for all the features.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param out_col: The output numeric column
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
    :param out_val: output numeric value
    :param out_format: output file format (default GPKG).

    """
    import geopandas
    import numpy

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    str_col = numpy.empty((base_gpdf.shape[0]), dtype=object)
    str_col[...] = out_val

    base_gpdf[out_col] = str_col

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def get_unq_col_values(vec_file: str, vec_lyr: str, col_name: str):
    """
    A function which splits a vector layer by an attribute value into either
    different layers or different output files.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param col_name: The column name for which a list of unique values will be returned.
    :returns: a numpy array as a list of the unique within the column.

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    unq_vals = base_gpdf[col_name].unique()
    base_gpdf = None
    return unq_vals


def add_fid_col(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    out_col: str = "fid",
):
    """
    A function which adds a numeric feature ID (FID) column with unique values per
    feature within the file.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
    :param out_format: output file format (default GPKG).
    :param out_col: The output FID column name (Default: fid)

    """
    import geopandas
    import numpy

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    base_gpdf[out_col] = numpy.arange(1, (base_gpdf.shape[0]) + 1, 1, dtype=int)

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def get_vec_cols_as_array(
    vec_file: str,
    vec_lyr: str,
    cols: list,
    lower_limit: float = None,
    upper_limit: float = None,
):
    """
    A function returns an n x m numpy array with the values for the columns specified.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param cols: list of columns to be read and returned.
    :param no_data_val: no data value used within the column values. Rows with
                        a no data value will be dropped. If None then ignored
                        (Default: None)
    :param lower_limit: Optional lower limit to define valid values. Note the same
                        value is used for all the columns listed. If a value is found
                        to be outside of the threshold the whole row is removed.
    :param upper_limit: Optional upper limit to define valid values. Note the same
                        value is used for all the columns listed. If a value is found
                        to be outside of the threshold the whole row is removed.
    :returns: a numpy array with the column values.

    """
    import geopandas
    import rsgislib.tools.stats

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    sub_base_gpdf = base_gpdf.loc[:, cols]
    out_arr = sub_base_gpdf.values
    out_arr = out_arr.astype(float)

    out_arr = rsgislib.tools.stats.mask_data_to_valid(out_arr, lower_limit, upper_limit)

    return out_arr
