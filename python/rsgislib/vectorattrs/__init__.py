#!/usr/bin/env python
"""
The vector attributes module performs attribute table operations on vectors.
"""

import os

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
:param att_col_data: A list of the same length as the number of features in vector file.


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
        raise Exception("The number of features and size of "
                        "the input data is not equal.")

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn(att_column, att_col_datatype)
        if lyr.CreateField(field_defn) != 0:
            raise Exception("Creating '{}' field failed; becareful with case, "
                            "some drivers are case insensitive but column might "
                            "not be found.".format(att_column))

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
            print("Data type of the value being "
                  "written is '{}'".format(type(att_col_data[i])))
        raise e


def writeVecColumn2Layer(lyr, att_column, att_col_datatype, att_col_data):
    """
A function which will write a column to a vector layer.

Where:

:param lyr: GDAL/OGR vector layer object
:param att_column: Name of the output column
:param att_col_datatype: ogr data type for the output column
                        (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
:param att_col_data: A list of the same length as the number of features in vector file.

"""
    gdal.UseExceptions()

    if lyr is None:
        raise Exception("The layer passed in is None...")

    numFeats = lyr.GetFeatureCount()
    if not len(att_col_data) == numFeats:
        print("Number of Features: {}".format(numFeats))
        print("Length of Data: {}".format(len(att_col_data)))
        raise Exception(
            "The number of features and size of the input data is not equal.")

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn(att_column, att_col_datatype)
        if lyr.CreateField(field_defn) != 0:
            raise Exception("Creating '{}' field failed; be careful with case, some "
                            "drivers are case insensitive but column might not "
                            "be found.".format(att_column))

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
        raise Exception("The specified column does not exist in the input layer; "
                        "check case as some drivers are case sensitive.")

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
            raise Exception("Could not find the attribute ({}) specified "
                            "within the vector layer.".format(attName))

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
