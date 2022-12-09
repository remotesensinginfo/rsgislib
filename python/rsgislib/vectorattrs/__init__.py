#!/usr/bin/env python
"""
The vector attributes module performs attribute table operations on vectors.
"""

from typing import Union, List, Dict
import math

from osgeo import gdal
import osgeo.ogr
from osgeo import ogr

import numpy

import rsgislib

gdal.UseExceptions()


def write_vec_column(
    out_vec_file: str,
    out_vec_lyr: str,
    att_column: str,
    att_col_datatype: int,
    att_col_data: List,
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
    ds = gdal.OpenEx(out_vec_file, gdal.OF_UPDATE)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(out_vec_file))

    lyr = ds.GetLayerByName(out_vec_lyr)
    if lyr is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(out_vec_lyr))

    numFeats = lyr.GetFeatureCount()
    if not len(att_col_data) == numFeats:
        print("Number of Features: {}".format(numFeats))
        print("Length of Data: {}".format(len(att_col_data)))
        raise rsgislib.RSGISPyException(
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
            raise rsgislib.RSGISPyException(
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
    att_col_data: List,
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
    if out_vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("The layer passed in is None...")

    numFeats = out_vec_lyr_obj.GetFeatureCount()
    if not len(att_col_data) == numFeats:
        print("Number of Features: {}".format(numFeats))
        print("Length of Data: {}".format(len(att_col_data)))
        raise rsgislib.RSGISPyException(
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
            raise rsgislib.RSGISPyException(
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


def read_vec_column(vec_file: str, vec_lyr: str, att_column: str) -> List:
    """
    A function which will reads a column from a vector file

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be read from.
    :param att_column: Name of the input column
    :returns: a list with the column values.

    """
    ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    lyr = ds.GetLayerByName(vec_lyr)
    if lyr is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName() == att_column:
            colExists = True
            break

    if not colExists:
        ds = None
        raise rsgislib.RSGISPyException(
            "The specified column does not exist in the input layer; "
            "check case as some drivers are case sensitive."
        )

    outVal = list()
    lyr.ResetReading()
    for feat in lyr:
        outVal.append(feat.GetField(att_column))
    ds = None

    return outVal


def read_vec_columns(vec_file: str, vec_lyr: str, att_columns: List[str]) -> List[Dict]:
    """
    A function which will reads a number of column from a vector file

    :param vec_file: The file / path to the vector data 'file'.
    :param vec_lyr: The layer to which the data is to be read from.
    :param att_columns: List of input attribute column names to be read in.
    :return: list of dicts with the column names as keys

    """
    ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    lyr = ds.GetLayerByName(vec_lyr)
    if lyr is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

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
            raise rsgislib.RSGISPyException(
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
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

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
            raise rsgislib.RSGISPyException(
                "Creating '{}' field failed.".format(x_min_col)
            )

    if not xmaxCol_exists:
        xmax_field_defn = ogr.FieldDefn(x_max_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(xmax_field_defn) != 0:
            raise rsgislib.RSGISPyException(
                "Creating '{}' field failed.".format(x_max_col)
            )

    if not yminCol_exists:
        ymin_field_defn = ogr.FieldDefn(y_min_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(ymin_field_defn) != 0:
            raise rsgislib.RSGISPyException(
                "Creating '{}' field failed.".format(y_min_col)
            )

    if not ymaxCol_exists:
        ymax_field_defn = ogr.FieldDefn(y_max_col, ogr.OFTReal)
        if vec_lyr_obj.CreateField(ymax_field_defn) != 0:
            raise rsgislib.RSGISPyException(
                "Creating '{}' field failed.".format(y_max_col)
            )

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
            raise rsgislib.RSGISPyException(
                "An error has occurred setting a feature on a layer."
            )
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


def add_geom_bbox_cols(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    min_x_col: str = "MinX",
    max_x_col: str = "MaxX",
    min_y_col: str = "MinY",
    max_y_col: str = "MaxY",
):
    """
    A function which adds columns to the vector layer with the bbox of each geometry.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer name
    :param out_vec_file: output vector file
    :param out_vec_lyr: output vector layer name
    :param out_format: The output format of the output file. (Default: GPKG)
    :param min_x_col: Name of the MinX column (Default: MinX)
    :param max_x_col: Name of the MaxX column (Default: MaxX)
    :param min_y_col: Name of the MinY column (Default: MinY)
    :param max_y_col: Name of the MaxY column (Default: MaxY)

    """
    import geopandas
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    # Get Geometry bounds
    geom_bounds = base_gpdf["geometry"].bounds

    # Add columns to the geodataframe
    base_gpdf[min_x_col] = geom_bounds["minx"]
    base_gpdf[max_x_col] = geom_bounds["maxx"]
    base_gpdf[min_y_col] = geom_bounds["miny"]
    base_gpdf[max_y_col] = geom_bounds["maxy"]

    # Output the file.
    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


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
    import tqdm
    import rsgislib.tools.utils
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

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
    lut_json_file: str = None,
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
    :param lut_json_file: an optional output LUT file.

    """
    import geopandas
    import rsgislib.vectorutils
    import rsgislib.tools.utils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    unq_vals = base_gpdf[unq_col].unique()

    base_gpdf[out_col] = numpy.zeros((base_gpdf.shape[0]), dtype=int)
    lut = dict()
    lut["id"] = dict()
    lut["val"] = dict()
    num_unq_val = 1
    for unq_val in unq_vals:
        sel_rows = base_gpdf[unq_col] == unq_val
        base_gpdf.loc[sel_rows, out_col] = num_unq_val
        lut["id"][num_unq_val] = unq_val
        lut["val"][unq_val] = num_unq_val
        num_unq_val += 1

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)

    if lut_json_file is not None:
        rsgislib.tools.utils.write_dict_to_json(lut, lut_json_file)


def add_numeric_col_lut(
    vec_file: str,
    vec_lyr: str,
    ref_col: str,
    val_lut: Dict,
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
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

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
    out_col_int: bool = False,
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
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

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
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    str_col = numpy.empty((base_gpdf.shape[0]), dtype=object)
    str_col[...] = out_val

    base_gpdf[out_col] = str_col

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def get_unq_col_values(vec_file: str, vec_lyr: str, col_name: str) -> numpy.array:
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
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    base_gpdf[out_col] = numpy.arange(1, (base_gpdf.shape[0]) + 1, 1, dtype=int)

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def get_vec_cols_as_array(
    vec_file: str,
    vec_lyr: str,
    cols: List[str],
    lower_limit: float = None,
    upper_limit: float = None,
) -> numpy.array:
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


def sort_vec_lyr(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    sort_by: Union[str, List[str]],
    ascending: Union[bool, List[bool]],
    out_format: str = "GPKG",
):
    """
    A function which sorts a vector layer based on the attributes of the layer.
    You can sort by either a single attribute or within multiple attributes
    if a list is provided. This function is implemented using geopandas.

    :param vec_file: the input vector file.
    :param vec_lyr: the input vector layer name.
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the output vector layer name.
    :param sort_by: either a string with the name of a single attribute or a list
                    of strings if multiple attributes are used for the sort.
    :param ascending: either a bool (True: ascending; False: descending) or list
                      of bools if a list of attributes was given.
    :param out_format: The output vector file format (Default: GPKG)

    """
    import geopandas
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

    if type(sort_by) is list:
        if type(ascending) is not list:
            raise rsgislib.RSGISPyException(
                "If sort_by is a list then ascending must be too."
            )

        if len(sort_by) != len(ascending):
            raise rsgislib.RSGISPyException(
                "If lists, the length of sort_by and ascending must be the same."
            )

    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    # sort layer.
    sorted_gpdf = base_gpdf.sort_values(by=sort_by, ascending=ascending)

    if out_format == "GPKG":
        sorted_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        sorted_gpdf.to_file(out_vec_file, driver=out_format)


def find_replace_str_vec_lyr(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    cols: List[str],
    find_replace: Dict[str, str],
    out_format: str = "GPKG",
):
    """
    A function which performs a find and replace on a string column(s) within the
    vector layer. For example, replacing a no data value (e.g., NA) with something
    more useful. This function is implemented using geopandas.

    :param vec_file: the input vector file.
    :param vec_lyr: the input vector layer name.
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the output vector layer name.
    :param cols: a list of strings with the names of the columns to which
                 the find and replace is to be applied.
    :param find_replace: the value pairs where the dict keys are the values
                         to be replaced and the value is the replacement
                         value.
    :param out_format: The output vector file format (Default: GPKG)

    """
    import geopandas
    import rsgislib.vectorutils

    out_format = rsgislib.vectorutils.check_format_name(out_format)

    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    # Perform find and replace
    for col in cols:
        for find_val in find_replace:
            base_gpdf[col] = base_gpdf[col].str.replace(
                find_val, find_replace[find_val]
            )

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def count_pt_intersects(
    vec_in_file: str,
    vec_in_lyr: str,
    vec_pts_file: str,
    vec_pts_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    out_count_col: str = "n_points",
    tmp_col_name: str = "tmp_join_fid",
    vec_in_epsg: int = None,
    vec_pts_epsg: int = None,
):
    """
    A function which counts the number of points intersecting a set of polygons
    adding the count to each polygon as a new column.

    Note, defining epsg codes for the datasets does not reproject the datasets
    but just makes sure that correct projection is being used.

    :param vec_in_file: the input polygons vector file path.
    :param vec_in_lyr: the input polygons vector layer name
    :param vec_pts_file: the points vector file path
    :param vec_pts_lyr: the points vector layer name
    :param out_vec_file: the output vector file path
    :param out_vec_lyr: the output vector layer name
    :param out_format: the output vector format (e.g., GPKG).
    :param out_count_col: the output column name (default: n_points)
    :param tmp_col_name: The name of a temporary column added to the input layer
                         used to ensure there are no duplicated features in the output
                         layer. The default name is: "tmp_sel_join_fid".
    :param vec_in_epsg: Optionally provide the epsg code for the input vector layer.
    :param vec_pts_epsg: Optionally provide the epsg code for the selection
                         vector layer.

    """
    import geopandas

    print("Read vector layers")
    in_gpdf = geopandas.read_file(vec_in_file, layer=vec_in_lyr)
    pts_gpdf = geopandas.read_file(vec_pts_file, layer=vec_pts_lyr)

    if vec_in_epsg is not None:
        in_gpdf = in_gpdf.set_crs(epsg=vec_in_epsg, allow_override=True)
    if vec_pts_epsg is not None:
        pts_gpdf = pts_gpdf.set_crs(epsg=vec_pts_epsg, allow_override=True)

    # Add column with unique id for each row.
    col_names = in_gpdf.columns.values.tolist()
    drop_col_names = []
    if "index_left" in col_names:
        drop_col_names.append("index_left")
    if "index_right" in col_names:
        drop_col_names.append("index_right")
    if len(drop_col_names) > 0:
        in_gpdf.drop(columns=drop_col_names, inplace=True)
    in_gpdf[tmp_col_name] = numpy.arange(1, (in_gpdf.shape[0]) + 1, 1, dtype=int)

    print("Perform Count")
    in_count_gpdf = in_gpdf.merge(
        in_gpdf.sjoin(pts_gpdf)
        .groupby(tmp_col_name)
        .size()
        .rename(out_count_col)
        .reset_index()
    )
    col_names = in_count_gpdf.columns.values.tolist()
    drop_col_names = []
    if "index_left" in col_names:
        drop_col_names.append("index_left")
    if "index_right" in col_names:
        drop_col_names.append("index_right")
    drop_col_names.append(tmp_col_name)
    in_count_gpdf.drop(columns=drop_col_names, inplace=True)

    print("Export")
    if out_format == "GPKG":
        in_count_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        in_count_gpdf.to_file(out_vec_file, driver=out_format)


def calc_npts_in_radius(
    vec_in_file: str,
    vec_in_lyr: str,
    radius: float,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    out_col_name: str = "n_pts_r",
    n_cores=1,
):
    """
    A function which calculate the number of points intersecting within a
    radius of each point.

    :param vec_in_file: Input vector file path (must be points geometry)
    :param vec_in_lyr: Input vector layer (must be points geometry)
    :param radius: the search radius
    :param out_vec_file: Output vector file path
    :param out_vec_lyr: Output vector layer
    :param out_format: output vector format (Default: GPKG)
    :param out_col_name: output column name (Default: n_pts_r)
    :param n_cores: the number of cores to be used for the query. If -1 is
                    passed then all available cores will be used.

    """
    import geopandas
    import scipy.spatial

    print("Read Vector")
    in_gpdf = geopandas.read_file(vec_in_file, layer=vec_in_lyr)

    print("Build Index")
    tree = scipy.spatial.KDTree(list(zip(in_gpdf.geometry.x, in_gpdf.geometry.y)))

    print("Perform Query")
    n_pts = tree.query_ball_point(
        list(zip(in_gpdf.geometry.x, in_gpdf.geometry.y)),
        r=radius,
        p=2.0,
        eps=0,
        workers=n_cores,
        return_sorted=None,
        return_length=True,
    )

    in_gpdf[out_col_name] = n_pts - 1  # -1 as each point will have found itself.

    print("Export")
    if out_format == "GPKG":
        in_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        in_gpdf.to_file(out_vec_file, driver=out_format)


def annotate_vec_selection(
    vec_in_file: str,
    vec_in_lyr: str,
    vec_sel_file: str,
    vec_sel_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_col_name: str = "sel_feats",
    out_format: str = "GPKG",
    tmp_col_name: str = "tmp_sel_join_fid",
    vec_in_epsg: int = None,
    vec_sel_epsg: int = None,
):
    """

    A function which spatial selects features from the input vector layer which
    intersects the selection vector layer populating a column within the output
    vector layer specifying which features intersect.

    Note, defining epsg codes for the datasets does not reproject the datasets
    but just makes sure that correct projection is being used.

    :param vec_in_file: the input vector file path.
    :param vec_in_lyr: the input vector layer name
    :param vec_sel_file: the selection vector file path
    :param vec_sel_lyr: the selection vector layer name
    :param out_vec_file: the output vector file path
    :param out_vec_lyr: the output vector layer name
    :param out_col_name: the output boolean column specifying those features which
                         intersect with the vec_sel_lyr layer.
    :param out_format: the output vector format (e.g., GPKG).
    :param tmp_col_name: The name of a temporary column added to the input layer
                         used to ensure there are no duplicated features in the output
                         layer. The default name is: "tmp_sel_join_fid".
    :param vec_in_epsg: Optionally provide the epsg code for the input vector layer.
    :param vec_sel_epsg: Optionally provide the epsg code for the selection
                         vector layer.

    """
    import geopandas
    import numpy

    print("Read vector layers")
    in_gpdf = geopandas.read_file(vec_in_file, layer=vec_in_lyr)
    sel_gpdf = geopandas.read_file(vec_sel_file, layer=vec_sel_lyr)

    if vec_in_epsg is not None:
        in_gpdf = in_gpdf.set_crs(epsg=vec_in_epsg, allow_override=True)
    if vec_sel_epsg is not None:
        sel_gpdf = sel_gpdf.set_crs(epsg=vec_sel_epsg, allow_override=True)

    # Add column with unique id for each row.
    in_gpdf[tmp_col_name] = numpy.arange(1, (in_gpdf.shape[0]) + 1, 1, dtype=int)

    print("Perform Selection")
    in_sel_gpdf = geopandas.sjoin(
        in_gpdf, sel_gpdf, how="inner", predicate="intersects"
    )
    # Remove any duplicate features using the tmp column
    in_sel_gpdf.drop_duplicates(subset=[tmp_col_name], inplace=True)
    # Create new column with the selection populated as True.
    in_gpdf[out_col_name] = in_gpdf[tmp_col_name].isin(in_sel_gpdf[tmp_col_name].values)
    # Remove the tmp column
    in_gpdf.drop(columns=[tmp_col_name], inplace=True)

    print("Export")
    if out_format == "GPKG":
        in_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        in_gpdf.to_file(out_vec_file, driver=out_format)



def perform_spatial_join(
    vec_base_file: str,
    vec_base_lyr: str,
    vec_join_file: str,
    vec_join_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    join_how: str = "inner",
    join_op: str = "within",
    vec_base_epsg: int = None,
    vec_join_epsg: int = None,
):
    """
    A function to perform a spatial join between two vector layers. This function
    uses geopandas so this needs to be installed. You also need to have the rtree
    package to generate the index used to perform the intersection.

    Note, defining epsg codes for the datasets does not reproject the datasets
    but just makes sure that correct projection is being used.

    For more information see: http://geopandas.org/mergingdata.html#spatial-joins

    :param vec_base_file: the base vector file with the geometries which will
                          be outputted.
    :param vec_base_lyr: the layer name for the base vector.
    :param vec_join_file: the vector with the attributes which will be joined to
                          the base vector geometries.
    :param vec_join_lyr: the layer name for the join vector.
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the layer name for the output vector.
    :param out_format: The output vector file format (Default GPKG)
    :param join_how: Specifies the type of join that will occur and which geometry
                     is retained. The options are [left, right, inner]. The default
                     is 'inner'
    :param join_op: Defines whether or not to join the attributes of one object
                    to another. The options are [intersects, within, contains]
                    and default is 'within'
    :param vec_base_epsg: Optionally provide the epsg code for the base vector layer.
    :param vec_join_epsg: Optionally provide the epsg code for the join vector layer.

    """
    import geopandas

    if join_how not in ["left", "right", "inner"]:
        raise rsgislib.RSGISPyException("The join_how specified is not valid.")
    if join_op not in ["intersects", "within", "contains"]:
        raise rsgislib.RSGISPyException("The join_op specified is not valid.")

    # Try importing rtree to provide useful error message as
    # will be used in sjoin but if not present
    # the error message is not very user friendly:
    # AttributeError: 'NoneType' object has no attribute 'intersection'
    try:
        import rtree
    except ImportError:
        raise rsgislib.RSGISPyException(
            "The rtree module was not available for "
            "import this is required by geopandas to "
            "perform a join."
        )

    base_gpd_df = geopandas.read_file(vec_base_file, layer=vec_base_lyr)
    join_gpd_df = geopandas.read_file(vec_join_file, layer=vec_join_lyr)

    if vec_base_epsg is not None:
        base_gpd_df = base_gpd_df.set_crs(epsg=vec_base_epsg, allow_override=True)
    if vec_join_epsg is not None:
        join_gpd_df = join_gpd_df.set_crs(epsg=vec_join_epsg, allow_override=True)

    join_gpd_df = geopandas.sjoin(base_gpd_df, join_gpd_df, how=join_how, op=join_op)

    if len(join_gpd_df) > 0:
        if out_format == "GPKG":
            join_gpd_df.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            join_gpd_df.to_file(out_vec_file, driver=out_format)


def drop_vec_cols(
    vec_file: str,
    vec_lyr: str,
    drop_cols: List[str],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    chk_cols_present: bool = True,
):
    """
    A function which allows vector columns to be removed from the layer.

    param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param drop_cols: List of columns to remove from layer
    :param out_vec_file: the output vector file
    :param out_vec_lyr: the output vector layer
    :param out_format: the output vector format (Default: GPKG)
    :param chk_cols_present: boolean (default: True) to check that the columns to be
                             removed are present and remove those from the list
                             which are not present.

    """
    import geopandas

    data_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    if chk_cols_present:
        drop_cols_chk = list()
        for col in data_gpdf.columns:
            if col in drop_cols:
                drop_cols_chk.append(col)
    else:
        drop_cols_chk = drop_cols

    if len(drop_cols_chk) > 0:
        data_gpdf.drop(columns=drop_cols_chk, inplace=True)

    if out_format == "GPKG":
        data_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gpdf.to_file(out_vec_file, driver=out_format)


def rename_vec_cols(
    vec_file: str,
    vec_lyr: str,
    rname_cols_lut: Dict[str, str],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which allows vector column to be renamed.

    param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param rname_cols_lut: dict look up for the columns to be renamed.
                          Format: {"orig_name": "new_name"}
    :param out_vec_file: the output vector file
    :param out_vec_lyr: the output vector layer
    :param out_format: the output vector format (Default: GPKG)

    """
    import geopandas

    data_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    data_gpdf.rename(columns=rname_cols_lut, inplace=True)

    if out_format == "GPKG":
        data_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gpdf.to_file(out_vec_file, driver=out_format)
