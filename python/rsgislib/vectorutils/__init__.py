#!/usr/bin/env python
"""
The vector utils module performs geometry / attribute table operations on vectors.
"""

import math
import os
import shutil
import subprocess
import sys
from typing import Dict, List, Union, Tuple

from osgeo import gdal, ogr, osr

# Import the RSGISLib module
import rsgislib

# Import the RSGISLib Image Utils module
import rsgislib.imageutils

# Import the RSGISLib RasterGIS module
import rsgislib.rastergis

# import the C++ extension into this level
from ._vectorutils import *

gdal.UseExceptions()


class VecColVar(object):
    """
    A class for using the the vector_math function specifying the input
    columns and the variable name to be used in the expression.

    :param name: the name of the variable to be used within the expression
    :param field_name: the name of the column in the attribute table.

    """

    def __init__(self, name: str, field_name: str):
        """

        :param name: the name of the variable to be used within the expression
        :param field_name: the name of the column in the attribute table.

        """
        self.name = name
        self.field_name = field_name


class VecLayersInfoObj(object):
    """
    This is a class to store the information associated within the
    rsgislib.vectorutils.merge_to_multi_layer_vec function.

    :param vec_file: input vector file.
    :param vec_lyr: input vector layer name
    :param vec_out_lyr: output vector layer name

    """

    def __init__(
        self, vec_file: str = None, vec_lyr: str = None, vec_out_lyr: str = None
    ):
        """

        :param vec_file: input vector file.
        :param vec_lyr: input vector layer name
        :param vec_out_lyr: output vector layer name

        """
        self.vec_file = vec_file
        self.vec_lyr = vec_lyr
        self.vec_out_lyr = vec_out_lyr


def delete_vector_file(vec_file: str, feedback: bool = True):
    """
    Function to delete an existing vector file.

    :param vec_file: vector file path
    :param feedback: Boolean specifying whether the function should print feedback to the console as files are delted.

    """
    from osgeo import gdal

    import rsgislib.tools.filetools

    if os.path.exists(vec_file):
        ds_in_vec = gdal.OpenEx(vec_file, gdal.OF_READONLY)
        if ds_in_vec is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))
        file_lst = ds_in_vec.GetFileList()
        for cfile in file_lst:
            if feedback:
                print("Deleting: {}".format(cfile))
            rsgislib.tools.filetools.delete_file_silent(cfile)


def check_format_name(vec_file_format: str) -> str:
    """
    A function which checks the format string for vector formats
    as functions which use Geopandas rather than GDAL directly as
    for analysis require the format string to be GeoJSON rather
    than GEOJSON. This function just helps to catch user input
    error and correct it.

    At the moment this function just changes GeoJSON, but if the input
    format isn't GEOJSON then the string is left unaltered and just
    passed back. Edits for more formats can be included in the future
    if they are found to be needed for Geopandas.

    :param vec_file_format: the input format string (e.g. GEOJSON, GPKG, ESRI Shapefile, etc.)
    :return: the format string, if the format is GEOJSON then it will be changes to
             GeoJSON.

    """
    out_format = vec_file_format
    if vec_file_format.lower() == "geojson":
        out_format = "GeoJSON"
    return out_format


def get_file_vec_extension(format: str):
    """
    A function to get the extension for a given file format
    (NOTE, currently only GPKG, GeoJSON, ESRI Shapefile, KML, KMZ, GML are supported).

    :param format: OGR string for the format.
    :return: string

    """
    if format.lower() == "gpkg":
        ext = "gpkg"
    elif format.lower() == "geojson":
        ext = "geojson"
    elif format.lower() == "esri shapefile":
        ext = "shp"
    elif format.lower() == "kml":
        ext = "kml"
    elif format.lower() == "gml":
        ext = "gml"
    else:
        raise rsgislib.RSGISPyException(
            f"The extension for the format ({format}) specified is unknown."
        )
    return ext


def get_proj_wkt_from_vec(vec_file: str, vec_lyr: str = None) -> str:
    """
    A function which gets the WKT projection from the inputted vector file.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is a string with the input vector layer name, if None then first layer read. (default: None)

    :return: WKT representation of projection

    """
    dataset = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if dataset is None:
        raise rsgislib.RSGISPyException("Could not open file: {}".format(vec_file))
    if vec_lyr is None:
        layer = dataset.GetLayer()
    else:
        layer = dataset.GetLayer(vec_lyr)
    if layer is None:
        raise rsgislib.RSGISPyException(
            "Could not open layer within file: {}".format(vec_file)
        )
    spatial_ref = layer.GetSpatialRef()
    return spatial_ref.ExportToWkt()


def get_proj_epsg_from_vec(vec_file: str, vec_lyr: str = None) -> int:
    """
    A function which gets the EPSG projection from the inputted vector file.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is a string with the input vector layer name, if None then first layer read. (default: None)

    :return: EPSG representation of projection

    """
    dataset = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if dataset is None:
        raise rsgislib.RSGISPyException("Could not open file: {}".format(vec_file))
    if vec_lyr is None:
        layer = dataset.GetLayer()
    else:
        layer = dataset.GetLayer(vec_lyr)
    if layer is None:
        raise rsgislib.RSGISPyException(
            "Could not open layer within file: {}".format(vec_file)
        )
    spatial_ref = layer.GetSpatialRef()
    spatial_ref.AutoIdentifyEPSG()
    epsg_str = spatial_ref.GetAuthorityCode(None)
    espg_rtn = None
    if epsg_str is not None:
        espg_rtn = int(epsg_str)
    return espg_rtn


def get_vec_feat_count(
    vec_file: str, vec_lyr: str = None, compute_count: bool = True
) -> int:
    """
    Get a count of the number of features in the vector layers.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is the layer for which extent is to be calculated (Default: None).
                    if None assume there is only one layer and that will be read.
    :param compute_count: is a boolean which specifies whether the layer extent
                         should be calculated (rather than estimated from header)
                         even if that operation is computationally expensive.

    :return: nfeats

    """

    in_data_source = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if vec_lyr is not None:
        in_layer = in_data_source.GetLayer(vec_lyr)
    else:
        in_layer = in_data_source.GetLayer()
    if in_layer is None:
        raise rsgislib.RSGISPyException("Check layer name as did not open layer.")
    n_feats = in_layer.GetFeatureCount(compute_count)
    return n_feats


def get_geom_type_name(geom_type: int) -> str:
    """
    A function which returns a string with a human-readable name of the
    geometry type.

    :param geom_type: the numerical type (e.g., rsgislib.GEOM_POLY)
    :return: the name of the geometry type

    """
    geom_type_name = ""
    if geom_type == rsgislib.GEOM_POLY:
        geom_type_name = "Polygon"
    elif geom_type == rsgislib.GEOM_PT:
        geom_type_name = "Point"
    elif geom_type == rsgislib.GEOM_LINE:
        geom_type_name = "Line"
    elif geom_type == rsgislib.GEOM_MPOLY:
        geom_type_name = "Multi-Polygon"
    elif geom_type == rsgislib.GEOM_MPT:
        geom_type_name = "Multi-Point"
    elif geom_type == rsgislib.GEOM_MLINE:
        geom_type_name = "Multi-Line"
    else:
        raise Exception(f"Do not recognise Geometry Type: '{geom_type}'")
    return geom_type_name


def get_vec_lyr_geom_type(vec_file: str, vec_lyr: str = None) -> int:
    """
    A function which returns an rsgislib.GEOM_XX value related to the vector
    geometry type.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is the layer for which extent is to be calculated (Default: None).
                    if None assume there is only one layer and that will be read.
    :return: int representing the geometry type (rsgislib.GEOM_XX)

    """
    vec_ds_obj = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if vec_ds_obj is None:
        raise rsgislib.RSGISPyException(f"Could not open input file: {vec_file}")

    if vec_lyr is None:
        vec_lyr_obj = vec_ds_obj.GetLayer()
    else:
        vec_lyr_obj = vec_ds_obj.GetLayer(vec_lyr)

    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException(
            f"Could not open vector layer ({vec_lyr}) in vector file {vec_file}"
        )

    vec_lyr_defn = vec_lyr_obj.GetLayerDefn()
    geom_type = vec_lyr_defn.GetGeomType()
    vec_ds_obj = None

    out_geom_type = 0
    if geom_type == ogr.wkbPolygon:
        out_geom_type = rsgislib.GEOM_POLY
    elif geom_type == ogr.wkbPolygon25D:
        out_geom_type = rsgislib.GEOM_POLY
    elif geom_type == ogr.wkbPoint:
        out_geom_type = rsgislib.GEOM_PT
    elif geom_type == ogr.wkbPoint25D:
        out_geom_type = rsgislib.GEOM_PT
    elif geom_type == ogr.wkbLineString:
        out_geom_type = rsgislib.GEOM_LINE
    elif geom_type == ogr.wkbLineString25D:
        out_geom_type = rsgislib.GEOM_LINE
    elif geom_type == ogr.wkbMultiPolygon:
        out_geom_type = rsgislib.GEOM_MPOLY
    elif geom_type == ogr.wkbMultiPolygon25D:
        out_geom_type = rsgislib.GEOM_MPOLY
    elif geom_type == ogr.wkbMultiPoint:
        out_geom_type = rsgislib.GEOM_MPT
    elif geom_type == ogr.wkbMultiPoint25D:
        out_geom_type = rsgislib.GEOM_MPT
    elif geom_type == ogr.wkbMultiLineString:
        out_geom_type = rsgislib.GEOM_MLINE
    elif geom_type == ogr.wkbMultiLineString25D:
        out_geom_type = rsgislib.GEOM_MLINE
    else:
        raise Exception(
            "Do not recognise Geometry Type: '{}'".format(
                ogr.GeometryTypeToName(geom_type)
            )
        )

    return out_geom_type


def count_feats_per_att_val(
    vec_file: str,
    vec_lyr: str,
    col_name: str,
    out_df_dict: bool = False,
) -> Dict:
    """
    A function which returns the count of features for each variable
    value.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param col_name: The column used to count the number of features per value.
    :param out_df_dict: if true then dict will be formatted to import into a
                        pandas dataframe. Otherwise, the output dict will use
                        the attribute values as the key and count as value.
    :return: either dict with keys of vals and count for import into pandas or
             with attribute value and number of features

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    unq_vals = base_gpdf[col_name].unique()

    out_dict = dict()
    for val in unq_vals:
        c_gpdf = base_gpdf.loc[base_gpdf[col_name] == val]
        out_dict[val] = len(c_gpdf)

    if out_df_dict:
        blt_df_dict = dict()
        blt_df_dict["vals"] = list()
        blt_df_dict["count"] = list()

        for val in out_dict:
            blt_df_dict["vals"].append(val)
            blt_df_dict["count"].append(out_dict[val])
        out_dict = blt_df_dict

    return out_dict


def merge_vectors_to_gpkg(
    in_vec_files: List[str], out_vec_file: str, out_vec_lyr: str, exists: bool = False
):
    """
    Function which will merge a list of vector files into an single output GeoPackage (GPKG) file using ogr2ogr.


    :param in_vec_files: is a list of input files.
    :param out_vec_file: is the output GPKG database (*.gpkg)
    :param out_vec_lyr: is the layer name in the output database (i.e., you can merge layers into single
                        layer or write a number of layers to the same database).
    :param exists: boolean which specifies whether the database file exists or not.
    """
    import rsgislib.tools.filetools

    if not rsgislib.tools.filetools.does_path_exists_or_creatable(out_vec_file):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {out_vec_file}"
        )
    first = True
    for inFile in in_vec_files:
        n_feat = get_vec_feat_count(inFile)
        print("Processing: " + inFile + " has " + str(n_feat) + " features.")
        if n_feat > 0:
            if first:
                if not exists:
                    cmd = [
                        "ogr2ogr",
                        "-f",
                        "GPKG",
                        "-lco",
                        "SPATIAL_INDEX=YES",
                        "-nln",
                        out_vec_lyr,
                        out_vec_file,
                        inFile,
                    ]
                    try:
                        subprocess.run(cmd, check=True)
                    except OSError as e:
                        raise rsgislib.RSGISPyException(
                            "Error running ogr2ogr: {}".format(cmd)
                        )
                else:
                    cmd = [
                        "ogr2ogr",
                        "-update",
                        "-f",
                        "GPKG",
                        "-lco",
                        "SPATIAL_INDEX=YES",
                        "-nln",
                        out_vec_lyr,
                        out_vec_file,
                        inFile,
                    ]
                    try:
                        subprocess.run(cmd, check=True)
                    except OSError as e:
                        raise rsgislib.RSGISPyException(
                            "Error running ogr2ogr: {}".format(cmd)
                        )
                first = False
            else:
                cmd = [
                    "ogr2ogr",
                    "-update",
                    "-append",
                    "-f",
                    "GPKG",
                    "-nln",
                    out_vec_lyr,
                    out_vec_file,
                    inFile,
                ]
                try:
                    subprocess.run(cmd, check=True)
                except OSError as e:
                    raise rsgislib.RSGISPyException(
                        "Error running ogr2ogr: {}".format(cmd)
                    )


def merge_vector_lyrs_to_gpkg(
    vec_file: str, out_vec_file: str, out_vec_lyr: str, exists: bool = False
):
    """
    Function which will merge all the layers in the input vector file into an
    single output GeoPackage (GPKG) file using ogr2ogr.


    :param vec_file: is a vector file which contains multiple layers which
                     are to be merged
    :param out_vec_file: is the output GPKG database (*.gpkg)
    :param out_vec_lyr: is the layer name in the output database (i.e., you can
                        merge layers into single layer or write a number of layers
                        to the same database).
    :param exists: boolean which specifies whether the database file exists or not.
    """
    import rsgislib.tools.filetools

    if not rsgislib.tools.filetools.does_path_exists_or_creatable(out_vec_file):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {out_vec_file}"
        )

    lyrs = get_vec_lyrs_lst(vec_file)
    first = True
    for lyr in lyrs:
        n_feat = get_vec_feat_count(vec_file, lyr)
        print("Processing: " + lyr + " has " + str(n_feat) + " features.")
        if n_feat > 0:
            if first:
                if not exists:
                    cmd = [
                        "ogr2ogr",
                        "-f",
                        "GPKG",
                        "-lco",
                        "SPATIAL_INDEX=YES",
                        "-nln",
                        out_vec_lyr,
                        out_vec_file,
                        vec_file,
                        lyr,
                    ]
                    try:
                        subprocess.run(cmd, check=True)
                    except OSError as e:
                        raise rsgislib.RSGISPyException(
                            "Error running ogr2ogr: {}".format(cmd)
                        )
                else:
                    cmd = [
                        "ogr2ogr",
                        "-update",
                        "-f",
                        "GPKG",
                        "-lco",
                        "SPATIAL_INDEX=YES",
                        "-nln",
                        out_vec_lyr,
                        out_vec_file,
                        vec_file,
                        lyr,
                    ]
                    try:
                        subprocess.run(cmd, check=True)
                    except OSError as e:
                        raise rsgislib.RSGISPyException(
                            "Error running ogr2ogr: {}".format(cmd)
                        )
                first = False
            else:
                cmd = [
                    "ogr2ogr",
                    "-update",
                    "-append",
                    "-f",
                    "GPKG",
                    "-nln",
                    out_vec_lyr,
                    out_vec_file,
                    vec_file,
                    lyr,
                ]
                try:
                    subprocess.run(cmd, check=True)
                except OSError as e:
                    raise rsgislib.RSGISPyException(
                        "Error running ogr2ogr: {}".format(cmd)
                    )


def merge_vectors_to_gpkg_ind_lyrs(
    in_vec_files: List,
    out_vec_file: str,
    rename_dup_lyrs: bool = False,
    geom_type: str = None,
):
    """
    Function which will merge a list of vector files into an single output GPKG file
    where each input file forms a new layer using the existing layer name. This
    function wraps the ogr2ogr command.

    :param in_vec_files: is a list of input files.
    :param out_vec_file: is the output GPKG database (*.gpkg)
    :param rename_dup_lyrs: If False an exception will be throw if any input layers
                            has the same name. If True a layer will be renamed - with
                            a random set of letters/numbers on the end.
    :param geom_type: Force the output vector to have a particular geometry type
                      (e.g., 'POLYGON'). Same options as ogr2ogr.

    """
    import rsgislib.tools.filetools
    import rsgislib.tools.utils

    if not rsgislib.tools.filetools.does_path_exists_or_creatable(out_vec_file):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {out_vec_file}"
        )

    if geom_type is not None:
        if geom_type not in [
            "GEOMETRY",
            "POINT",
            "LINESTRING",
            "POLYGON",
            "GEOMETRYCOLLECTION",
            "MULTIPOINT",
            "MULTIPOLYGON",
            "MULTILINESTRING",
            "PROMOTE_TO_MULTI",
            "CONVERT_TO_LINEAR",
        ]:
            raise rsgislib.RSGISPyException("The geom_type is not valid.")

    out_lyr_names = []
    for inFile in in_vec_files:
        inlyrs = get_vec_lyrs_lst(inFile)
        print(
            "Processing File: {0} has {1} layers to copy.".format(inFile, len(inlyrs))
        )
        for lyr in inlyrs:
            n_feat = get_vec_feat_count(inFile, lyr)
            out_lyr = lyr
            if lyr in out_lyr_names:
                if rename_dup_lyrs:
                    out_lyr = "{}_{}".format(lyr, rsgislib.tools.utils.uid_generator())
                else:
                    raise rsgislib.RSGISPyException(
                        "Input files have layers with the "
                        "same name, these will be over written."
                    )
            print(
                "Processing Layer: {0} has {1} features to "
                "copy - output layer name: {2}".format(lyr, n_feat, out_lyr)
            )
            if n_feat > 0:
                if geom_type is None:
                    cmd = [
                        "ogr2ogr",
                        "-overwrite",
                        "-f",
                        "GPKG",
                        "-lco",
                        "SPATIAL_INDEX=YES",
                        "-nln",
                        out_lyr,
                        out_vec_file,
                        inFile,
                        lyr,
                    ]
                else:
                    cmd = [
                        "ogr2ogr",
                        "-overwrite",
                        "-f",
                        "GPKG",
                        "-lco",
                        "SPATIAL_INDEX=YES",
                        "-nlt",
                        geom_type,
                        "-nln",
                        out_lyr,
                        out_vec_file,
                        inFile,
                        lyr,
                    ]
                print(cmd)
                try:
                    subprocess.run(cmd, check=True)
                except OSError as e:
                    raise rsgislib.RSGISPyException(
                        "Error running ogr2ogr: {}".format(cmd)
                    )
                out_lyr_names.append(out_lyr)


def get_vec_lyrs_lst(vec_file: str) -> List[str]:
    """
    A function which returns a list of available layers within the inputted vector file.

    :param vec_file: file name and path to input vector layer.
    :return: list of layer names (can be used with gdal.Dataset.GetLayerByName()).

    """
    gdal_dataset = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if gdal_dataset is None:
        raise rsgislib.RSGISPyException(f"Could not open input file: {vec_file}")
    layer_list = []
    for lyr_idx in range(gdal_dataset.GetLayerCount()):
        lyr = gdal_dataset.GetLayerByIndex(lyr_idx)
        t_lyr_name = lyr.GetName()
        if t_lyr_name not in layer_list:
            layer_list.append(t_lyr_name)
    gdal_dataset = None
    return layer_list


def get_vec_layer_extent(
    vec_file: str, vec_lyr: str = None, compute_if_exp: bool = True
) -> Tuple[float, float, float, float]:
    """
    Get the extent of the vector layer.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is the layer for which extent is to be calculated (Default: None)
                      if None assume there is only one layer and that will be read.
    :param compute_if_exp: is a boolean which specifies whether the layer extent
                         should be calculated (rather than estimated from header)
                         even if that operation is computationally expensive.

    :return: boundary box is returned (MinX, MaxX, MinY, MaxY)
    """

    # Get a Layer's Extent
    try:
        in_data_source = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    except Exception as e:
        raise rsgislib.RSGISPyException(f"Could not open vector file: {vec_file}")
    if in_data_source is None:
        raise rsgislib.RSGISPyException(f"Could not open vector file: {vec_file}")
    if vec_lyr is not None:
        in_layer = in_data_source.GetLayer(vec_lyr)
    else:
        in_layer = in_data_source.GetLayer()
    if in_layer is None:
        raise rsgislib.RSGISPyException(
            f"Could not open vector layer ({vec_lyr}) in vector file {vec_file}"
        )
    extent = in_layer.GetExtent(compute_if_exp)
    return extent


def split_vec_lyr(
    vec_file: str,
    vec_lyr: str,
    n_feats: int,
    out_format: str,
    out_dir: str,
    out_vec_base: str,
    out_vec_ext: str,
):
    """
    A function which splits the input vector layer into a number of output layers.

    :param vec_file: input vector file.
    :param vec_lyr: input layer name.
    :param n_feats: number of features within each output file.
    :param out_format: output file driver.
    :param out_dir: output directory for the created output files.
    :param out_vec_base: output layer name will be the same as the base file name.
    :param out_vec_ext: file ending (e.g., gpkg). Note don't include the dot,
                        so input gpkg rather than .gpkg.

    """

    datasrc = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    src_lyr = datasrc.GetLayer(vec_lyr)
    n_in_feats = src_lyr.GetFeatureCount(True)
    print(n_in_feats)

    n_out_files = math.floor(n_in_feats / n_feats)
    remain_feats = n_in_feats - (n_out_files * n_feats)
    print(n_out_files)
    print(remain_feats)

    out_driver = ogr.GetDriverByName(out_format)
    src_lyr_spat_ref = src_lyr.GetSpatialRef()

    c_feat_n = 0
    s_feat_n = 0
    e_feat_n = n_feats
    for i in range(n_out_files):
        outveclyr = "{0}{1}".format(out_vec_base, i + 1)
        outvecfile = os.path.join(out_dir, "{0}.{1}".format(outveclyr, out_vec_ext))
        print("Creating: {}".format(outvecfile))
        result_ds = out_driver.CreateDataSource(outvecfile)
        result_lyr = result_ds.CreateLayer(
            outveclyr, src_lyr_spat_ref, geom_type=src_lyr.GetGeomType()
        )

        src_layer_defn = src_lyr.GetLayerDefn()
        for i in range(src_layer_defn.GetFieldCount()):
            field_defn = src_layer_defn.GetFieldDefn(i)
            result_lyr.CreateField(field_defn)
        rslt_layer_defn = result_lyr.GetLayerDefn()

        c_feat_n = 0
        src_lyr.ResetReading()
        in_feat = src_lyr.GetNextFeature()
        result_lyr.StartTransaction()
        while in_feat:
            if (c_feat_n >= s_feat_n) and (c_feat_n < e_feat_n):
                geom = in_feat.GetGeometryRef()
                if geom is not None:
                    result_lyr.CreateFeature(in_feat)
            elif c_feat_n > e_feat_n:
                break
            in_feat = src_lyr.GetNextFeature()
            c_feat_n = c_feat_n + 1
        result_lyr.CommitTransaction()
        result_ds = None

        s_feat_n = s_feat_n + n_feats
        e_feat_n = e_feat_n + n_feats

    if remain_feats > 0:
        outveclyr = "{0}{1}".format(out_vec_base, n_out_files + 1)
        outvecfile = os.path.join(out_dir, "{0}.{1}".format(outveclyr, out_vec_ext))
        print("Creating: {}".format(outvecfile))
        result_ds = out_driver.CreateDataSource(outvecfile)
        result_lyr = result_ds.CreateLayer(
            outveclyr, src_lyr_spat_ref, geom_type=src_lyr.GetGeomType()
        )

        src_layer_defn = src_lyr.GetLayerDefn()
        for i in range(src_layer_defn.GetFieldCount()):
            field_defn = src_layer_defn.GetFieldDefn(i)
            result_lyr.CreateField(field_defn)
        rslt_layer_defn = result_lyr.GetLayerDefn()

        c_feat_n = 0
        src_lyr.ResetReading()
        in_feat = src_lyr.GetNextFeature()
        result_lyr.StartTransaction()
        while in_feat:
            if c_feat_n >= s_feat_n:
                geom = in_feat.GetGeometryRef()
                if geom is not None:
                    result_lyr.CreateFeature(in_feat)
            in_feat = src_lyr.GetNextFeature()
            c_feat_n = c_feat_n + 1
        result_lyr.CommitTransaction()
        result_ds = None
    datasrc = None


def reproj_vector_layer(
    vec_file: str,
    out_vec_file: str,
    out_proj_wkt: str,
    out_format: str = "GPKG",
    out_vec_lyr: str = None,
    vec_lyr: str = None,
    in_proj_wkt: str = None,
    del_exist_vec: bool = False,
):
    """
    A function which reprojects a vector layer. You might also consider using
    rsgislib.vectorutils.vector_translate, particularly if you are reprojecting the data
    and changing between coordinate units (e.g., degrees to meters)

    :param vec_file: is a string with name and path to input vector file.
    :param out_vec_file: is a string with name and path to output vector file.
    :param out_proj_wkt: is a string with the WKT string for the output vector file.
    :param out_format: is the output vector file format. Default is ESRI Shapefile.
    :param out_vec_lyr: is a string for the output layer name. If None then ignored and
                       assume there is just a single layer in the vector and layer name
                       is the same as the file name.
    :param vec_lyr: is a string for the input layer name. If None then ignored and
                      assume there is just a single layer in the vector.
    :param in_proj_wkt: is a string with the WKT string for the input shapefile
                      (Optional; taken from input file if not specified).
    """
    ## This code has been editted and updated for GDAL > version 2.0
    ## https://pcjericks.github.io/py-gdalogr-cookbook/projection.html#reproject-a-layer

    # get the input layer
    in_data_set = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if in_data_set is None:
        raise rsgislib.RSGISPyException(
            "Failed to open input vector file: {}".format(vec_file)
        )
    if vec_lyr is None:
        in_layer = in_data_set.GetLayer()
    else:
        in_layer = in_data_set.GetLayer(vec_lyr)

    # input SpatialReference
    in_spatial_ref = osr.SpatialReference()
    if in_proj_wkt is not None:
        in_spatial_ref.ImportFromWkt(in_proj_wkt)
    else:
        in_spatial_ref = in_layer.GetSpatialRef()

    # output SpatialReference
    out_spatial_ref = osr.SpatialReference()
    out_spatial_ref.ImportFromWkt(out_proj_wkt)

    # create the CoordinateTransformation
    coord_trans = osr.CoordinateTransformation(in_spatial_ref, out_spatial_ref)

    # Create shapefile driver
    driver = gdal.GetDriverByName(out_format)

    # create the output layer
    if os.path.exists(out_vec_file):
        if out_format == "ESRI Shapefile":
            if del_exist_vec:
                driver.DeleteDataSource(out_vec_file)
            else:
                raise rsgislib.RSGISPyException(
                    "Output shapefile already exists - stopping."
                )
            out_data_set = driver.Create(out_vec_file, 0, 0, 0, gdal.GDT_Unknown)
        else:
            out_data_set = gdal.OpenEx(out_vec_file, gdal.OF_UPDATE)
    else:
        out_data_set = driver.Create(out_vec_file, 0, 0, 0, gdal.GDT_Unknown)

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]
    out_layer = out_data_set.CreateLayer(out_vec_lyr, out_spatial_ref, in_layer.GetGeomType())

    # add fields
    in_layer_defn = in_layer.GetLayerDefn()
    for i in range(0, in_layer_defn.GetFieldCount()):
        field_defn = in_layer_defn.GetFieldDefn(i)
        out_layer.CreateField(field_defn)

    # get the output layer's feature definition
    out_layer_defn = out_layer.GetLayerDefn()

    open_transaction = False
    n_feats = in_layer.GetFeatureCount(True)
    step = math.floor(n_feats / 10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end="", flush=True)

    # loop through the input features
    in_feature = in_layer.GetNextFeature()
    while in_feature:
        if (n_feats > 10) and (counter == feedback_next):
            print(".{}.".format(feedback), end="", flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10

        if not open_transaction:
            out_layer.StartTransaction()
            open_transaction = True

        # get the input geometry
        geom = in_feature.GetGeometryRef()
        if geom is not None:
            # reproject the geometry
            geom.Transform(coord_trans)
            # create a new feature
            out_feature = ogr.Feature(out_layer_defn)
            # set the geometry and attribute
            out_feature.SetGeometry(geom)
            for i in range(0, out_layer_defn.GetFieldCount()):
                out_feature.SetField(
                    out_layer_defn.GetFieldDefn(i).GetNameRef(), in_feature.GetField(i)
                )
            # add the feature to the shapefile
            out_layer.CreateFeature(out_feature)
        # dereference the features and get the next input feature
        out_feature = None

        if ((counter % 20000) == 0) and open_transaction:
            out_layer.CommitTransaction()
            open_transaction = False

        in_feature = in_layer.GetNextFeature()
        counter = counter + 1

    if open_transaction:
        out_layer.CommitTransaction()
        open_transaction = False
    out_layer.SyncToDisk()
    print(" Completed")

    # Save and close the shapefiles
    in_data_set = None
    out_data_set = None


def reproj_vec_lyr_obj(
    vec_lyr_obj: ogr.Layer,
    out_vec_file: str,
    out_epsg: int,
    out_format: str = "MEMORY",
    out_vec_lyr: str = None,
    in_epsg: int = None,
    print_feedback: bool = True,
):
    """
    A function which reprojects a vector layer. You might also consider using
    rsgislib.vectorutils.vector_translate, particularly if you are reprojecting
    the data and changing between coordinate units (e.g., degrees to meters)

    :param vec_lyr_obj: is a GDAL vector layer object.
    :param out_vec_file: is a string with name and path to output vector file - is created.
    :param out_epsg: is an int with the EPSG code to which the input vector layer is to be reprojected to.
    :param out_format: is the output vector file format. Default is MEMORY - i.e., nothing written to disk.
    :param out_vec_lyr: is a string for the output layer name. If None then ignored and
                       assume there is just a single layer in the vector and layer name
                       is the same as the file name.
    :param in_epsg: is an int with the EPSG code for the input vector file
                      (Optional; taken from input file if not specified).
    :param print_feedback: is a boolean (Default True) specifying whether feedback should be printed to the console.
    :return: Returns the output datasource and layer objects (result_ds, result_lyr). datasource needs to be set to None
             once you have finished using to free memory and if written to disk to ensure the whole dataset is written.
    """
    ## This code has been editted from https://pcjericks.github.io/py-gdalogr-cookbook/projection.html#reproject-a-layer
    ## Updated for GDAL 2.0

    vec_lyr_obj.ResetReading()

    # input SpatialReference
    in_spat_ref = osr.SpatialReference()
    if in_epsg is not None:
        in_spat_ref.ImportFromEPSG(in_epsg)
    else:
        in_spat_ref = vec_lyr_obj.GetSpatialRef()

    # output SpatialReference
    out_spat_ref = osr.SpatialReference()
    out_spat_ref.ImportFromEPSG(out_epsg)

    # create the CoordinateTransformation
    coord_trans = osr.CoordinateTransformation(in_spat_ref, out_spat_ref)

    # Create shapefile driver
    driver = ogr.GetDriverByName(out_format)
    if driver is None:
        raise rsgislib.RSGISPyException("Driver has not be recognised.")

    # create the output layer
    result_ds = driver.CreateDataSource(out_vec_file)
    if result_ds is None:
        raise rsgislib.RSGISPyException(
            "The output vector data source was not created: {}".format(out_vec_file)
        )
    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]
    result_lyr = result_ds.CreateLayer(
        out_vec_lyr, out_spat_ref, geom_type=vec_lyr_obj.GetGeomType()
    )

    # add fields
    in_vec_lyr_defn = vec_lyr_obj.GetLayerDefn()
    for i in range(0, in_vec_lyr_defn.GetFieldCount()):
        fieldDefn = in_vec_lyr_defn.GetFieldDefn(i)
        result_lyr.CreateField(fieldDefn)

    # get the output layer's feature definition
    result_lyr_defn = result_lyr.GetLayerDefn()

    openTransaction = False
    nFeats = vec_lyr_obj.GetFeatureCount(True)
    step = math.floor(nFeats / 10)
    feedback = 10
    feedback_next = step
    counter = 0
    if print_feedback:
        print("Started .0.", end="", flush=True)

    # loop through the input features
    inFeature = vec_lyr_obj.GetNextFeature()
    while inFeature:
        if (nFeats > 10) and (counter == feedback_next):
            if print_feedback:
                print(".{}.".format(feedback), end="", flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10

        if not openTransaction:
            result_lyr.StartTransaction()
            openTransaction = True

        # get the input geometry
        geom = inFeature.GetGeometryRef()
        if geom is not None:
            # reproject the geometry
            trans_err_code = geom.Transform(coord_trans)
            if trans_err_code != ogr.OGRERR_NONE:
                raise rsgislib.RSGISPyException(
                    "Geometry transformation failed... Error Code: {}".format(
                        trans_err_code
                    )
                )
            # create a new feature
            outFeature = ogr.Feature(result_lyr_defn)
            # set the geometry and attribute
            outFeature.SetGeometry(geom)
            for i in range(0, result_lyr_defn.GetFieldCount()):
                outFeature.SetField(
                    result_lyr_defn.GetFieldDefn(i).GetNameRef(), inFeature.GetField(i)
                )
            # add the feature to the shapefile
            result_lyr.CreateFeature(outFeature)
        # dereference the features and get the next input feature
        outFeature = None

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        inFeature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    if print_feedback:
        print(" Completed")

    result_lyr.ResetReading()

    return result_ds, result_lyr


def get_att_lst_select_feats(
    vec_file: str, vec_lyr: str, att_names: List, vec_sel_file: str, vec_sel_lyr: str
) -> List[Dict]:
    """
    Function to get a list of attribute values from features which intersect
    with the select layer.

    :param vec_file: vector layer from which the attribute data comes from.
    :param vec_lyr: the layer name from which the attribute data comes from.
    :param att_names: a list of attribute names to be outputted.
    :param vec_sel_file: the vector file which will be intersected within the
                         vector file.
    :param vec_sel_lyr: the layer name which will be intersected within the vector file.
    :return: list of dictionaries with the output values.

    """
    try:
        dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
        if dsVecFile is None:
            raise rsgislib.RSGISPyException("Could not open '" + vec_file + "'")

        vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

        dsSelVecFile = gdal.OpenEx(vec_sel_file, gdal.OF_READONLY)
        if dsSelVecFile is None:
            raise rsgislib.RSGISPyException("Could not open '" + vec_sel_file + "'")

        lyrSelVecObj = dsSelVecFile.GetLayerByName(vec_sel_lyr)
        if lyrSelVecObj is None:
            raise rsgislib.RSGISPyException(
                "Could not find layer '" + vec_sel_lyr + "'"
            )

        lyrDefn = vec_lyr_obj.GetLayerDefn()
        feat_idxs = dict()
        feat_types = dict()
        found_atts = dict()
        for attName in att_names:
            found_atts[attName] = False

        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in att_names:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True

        for attName in att_names:
            if not found_atts[attName]:
                dsSelVecFile = None
                dsVecFile = None
                raise rsgislib.RSGISPyException(
                    "Could not find the attribute ({}) specified within "
                    "the vector layer.".format(attName)
                )

        mem_driver = ogr.GetDriverByName("MEMORY")

        mem_sel_ds = mem_driver.CreateDataSource("MemSelData")
        mem_sel_lyr = mem_sel_ds.CopyLayer(lyrSelVecObj, vec_sel_lyr, ["OVERWRITE=YES"])

        mem_result_ds = mem_driver.CreateDataSource("MemResultData")
        mem_result_lyr = mem_result_ds.CreateLayer(
            "MemResultLyr", geom_type=vec_lyr_obj.GetGeomType()
        )

        for attName in att_names:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))

        vec_lyr_obj.Intersection(mem_sel_lyr, mem_result_lyr)

        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        outvals = []
        while inFeat:
            outdict = dict()
            for attName in att_names:
                feat_idx = reslyrDefn.GetFieldIndex(attName)
                if feat_types[attName] == ogr.OFTString:
                    outdict[attName] = inFeat.GetFieldAsString(feat_idx)
                elif feat_types[attName] == ogr.OFTReal:
                    outdict[attName] = inFeat.GetFieldAsDouble(feat_idx)
                elif feat_types[attName] == ogr.OFTInteger:
                    outdict[attName] = inFeat.GetFieldAsInteger(feat_idx)
                else:
                    outdict[attName] = inFeat.GetField(feat_idx)
            outvals.append(outdict)
            inFeat = mem_result_lyr.GetNextFeature()

        dsSelVecFile = None
        dsVecFile = None
        mem_sel_ds = None
        mem_result_ds = None
    except Exception as e:
        raise e
    return outvals


def get_att_lst_select_feats_lyr_objs(
    vec_lyr_obj: ogr.Layer, att_names: List, vec_sel_lyr_obj: ogr.Layer
) -> List[Dict]:
    """
    Function to get a list of attribute values from features which intersect
    with the select layer.

    :param vec_lyr_obj: the OGR layer object from which the attribute data comes from.
    :param att_names: a list of attribute names to be outputted.
    :param vec_sel_lyr_obj: the OGR layer object which will be intersected within
                            the vector file.
    :return: list of dictionaries with the output values.

    """
    try:
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException(
                "The vector layer passed into the function was None."
            )

        if vec_sel_lyr_obj is None:
            raise rsgislib.RSGISPyException(
                "The select vector layer passed into the function was None."
            )

        lyrDefn = vec_lyr_obj.GetLayerDefn()
        feat_idxs = dict()
        feat_types = dict()
        found_atts = dict()
        for attName in att_names:
            found_atts[attName] = False

        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in att_names:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True

        for attName in att_names:
            if not found_atts[attName]:
                raise rsgislib.RSGISPyException(
                    "Could not find the attribute ({}) specified within "
                    "the vector layer.".format(attName)
                )

        mem_driver = ogr.GetDriverByName("MEMORY")

        mem_result_ds = mem_driver.CreateDataSource("MemResultData")
        mem_result_lyr = mem_result_ds.CreateLayer(
            "MemResultLyr", geom_type=vec_lyr_obj.GetGeomType()
        )

        for attName in att_names:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))

        vec_sel_lyr_obj.Intersection(vec_lyr_obj, mem_result_lyr)

        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        outvals = []
        while inFeat:
            outdict = dict()
            for attName in att_names:
                feat_idx = reslyrDefn.GetFieldIndex(attName)
                if feat_types[attName] == ogr.OFTString:
                    outdict[attName] = inFeat.GetFieldAsString(feat_idx)
                elif feat_types[attName] == ogr.OFTReal:
                    outdict[attName] = inFeat.GetFieldAsDouble(feat_idx)
                elif feat_types[attName] == ogr.OFTInteger:
                    outdict[attName] = inFeat.GetFieldAsInteger(feat_idx)
                else:
                    outdict[attName] = inFeat.GetField(feat_idx)
            outvals.append(outdict)
            inFeat = mem_result_lyr.GetNextFeature()

        mem_result_ds = None
    except Exception as e:
        raise e
    return outvals


def get_att_lst_select_bbox_feats(
    vec_file: str,
    vec_lyr: str,
    att_names: List,
    bbox: Union[Tuple[float, float, float, float], List[float]],
    bbox_epsg: int = None,
) -> List[Dict]:
    """
    Function to get a list of attribute values from features which intersect
    with the select layer.

    :param vec_file: the OGR file from which the attribute data comes from.
    :param vec_lyr: the layer name within the file from which the attribute data
                    comes from.
    :param att_names: a list of attribute names to be outputted.
    :param bbox: the bounding box for the region of interest (xMin, xMax, yMin, yMax).
    :param bbox_epsg: the projection of the BBOX (if None then ignore).
    :return: list of dictionaries with the output values.

    """
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if dsVecFile is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

    outvals = get_att_lst_select_bbox_feats_lyr_objs(
        vec_lyr_obj, att_names, bbox, bbox_epsg
    )
    dsVecFile = None

    return outvals


def get_att_lst_select_bbox_feats_lyr_objs(
    vec_lyr_obj: ogr.Layer,
    att_names: List,
    bbox: Union[Tuple[float, float, float, float], List[float]],
    bbox_epsg: int = None,
) -> List[Dict]:
    """
    Function to get a list of attribute values from features which intersect
    with the select layer.

    :param vec_lyr_obj: the OGR layer object from which the attribute data comes from.
    :param att_names: a list of attribute names to be outputted.
    :param bbox: the bounding box for the region of interest (xMin, xMax, yMin, yMax).
    :param bbox_epsg: the projection of the BBOX (if None then ignore).
    :return: list of dictionaries with the output values.

    """

    outvals = []
    try:
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException(
                "The vector layer passed into the function was None."
            )

        in_vec_lyr_spat_ref = vec_lyr_obj.GetSpatialRef()
        if bbox_epsg is not None:
            in_vec_lyr_spat_ref.AutoIdentifyEPSG()
            in_vec_lyr_epsg = in_vec_lyr_spat_ref.GetAuthorityCode(None)

            if (in_vec_lyr_epsg is not None) and (
                int(in_vec_lyr_epsg) != int(bbox_epsg)
            ):
                raise rsgislib.RSGISPyException(
                    "The EPSG codes for the BBOX and input vector "
                    "layer are not the same."
                )

        lyrDefn = vec_lyr_obj.GetLayerDefn()
        feat_idxs = dict()
        feat_types = dict()
        found_atts = dict()
        for attName in att_names:
            found_atts[attName] = False

        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in att_names:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True

        for attName in att_names:
            if not found_atts[attName]:
                raise rsgislib.RSGISPyException(
                    "Could not find the attribute ({}) specified within "
                    "the vector layer.".format(attName)
                )

        # Create in-memory layer for the BBOX layer.
        mem_driver = ogr.GetDriverByName("MEMORY")
        mem_bbox_ds = mem_driver.CreateDataSource("MemBBOXData")
        mem_bbox_lyr = mem_bbox_ds.CreateLayer(
            "MemBBOXLyr", in_vec_lyr_spat_ref, geom_type=ogr.wkbPolygon
        )
        mem_bbox_feat_defn = mem_bbox_lyr.GetLayerDefn()
        ring = ogr.Geometry(ogr.wkbLinearRing)
        ring.AddPoint(bbox[0], bbox[3])
        ring.AddPoint(bbox[1], bbox[3])
        ring.AddPoint(bbox[1], bbox[2])
        ring.AddPoint(bbox[0], bbox[2])
        ring.AddPoint(bbox[0], bbox[3])
        # Create polygon.
        poly = ogr.Geometry(ogr.wkbPolygon)
        poly.AddGeometry(ring)
        out_bbox_feat = ogr.Feature(mem_bbox_feat_defn)
        out_bbox_feat.SetGeometryDirectly(poly)
        mem_bbox_lyr.CreateFeature(out_bbox_feat)
        mem_bbox_lyr.ResetReading()

        mem_result_ds = mem_driver.CreateDataSource("MemResultData")
        mem_result_lyr = mem_result_ds.CreateLayer(
            "MemResultLyr", in_vec_lyr_spat_ref, geom_type=vec_lyr_obj.GetGeomType()
        )

        for attName in att_names:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))

        vec_lyr_obj.Intersection(mem_bbox_lyr, mem_result_lyr)

        mem_result_lyr.SyncToDisk()
        mem_result_lyr.ResetReading()

        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        while inFeat:
            outdict = dict()
            for attName in att_names:
                feat_idx = reslyrDefn.GetFieldIndex(attName)
                if feat_types[attName] == ogr.OFTString:
                    outdict[attName] = inFeat.GetFieldAsString(feat_idx)
                elif feat_types[attName] == ogr.OFTReal:
                    outdict[attName] = inFeat.GetFieldAsDouble(feat_idx)
                elif feat_types[attName] == ogr.OFTInteger:
                    outdict[attName] = inFeat.GetFieldAsInteger(feat_idx)
                else:
                    outdict[attName] = inFeat.GetField(feat_idx)
            outvals.append(outdict)
            inFeat = mem_result_lyr.GetNextFeature()

        mem_result_ds = None
        mem_bbox_ds = None
    except Exception as e:
        raise e
    return outvals


def select_intersect_feats(
    vec_file: str,
    vec_lyr: str,
    vec_roi_file: str,
    vec_roi_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    Function to select the features which intersect with region of interest (ROI)
    features which will be outputted into a new vector layer.

    :param vec_file: vector layer from which the attribute data comes from.
    :param vec_lyr: the layer name from which the attribute data comes from.
    :param vec_roi_file: the vector file which will be intersected within the
                         vector file.
    :param vec_roi_lyr: the layer name which will be intersected within the
                        vector file.
    :param out_vec_file: the vector file which will be outputted.
    :param out_vec_lyr: the layer name which will be outputted.
    :param out_format: output vector format (default GPKG)

    """

    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if dsVecFile is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

    in_vec_lyr_spat_ref = vec_lyr_obj.GetSpatialRef()

    dsROIVecFile = gdal.OpenEx(vec_roi_file, gdal.OF_READONLY)
    if dsROIVecFile is None:
        raise rsgislib.RSGISPyException("Could not open '" + vec_roi_file + "'")

    lyrROIVecObj = dsROIVecFile.GetLayerByName(vec_roi_lyr)
    if lyrROIVecObj is None:
        raise rsgislib.RSGISPyException("Could not find layer '" + vec_roi_lyr + "'")

    mem_driver = ogr.GetDriverByName("MEMORY")
    mem_roi_ds = mem_driver.CreateDataSource("MemSelData")
    mem_roi_lyr = mem_roi_ds.CopyLayer(lyrROIVecObj, vec_roi_lyr, ["OVERWRITE=YES"])

    out_driver = ogr.GetDriverByName(out_format)
    result_ds = out_driver.CreateDataSource(out_vec_file)
    result_lyr = result_ds.CreateLayer(
        out_vec_lyr, in_vec_lyr_spat_ref, geom_type=vec_lyr_obj.GetGeomType()
    )

    vec_lyr_obj.Intersection(mem_roi_lyr, result_lyr)

    dsVecFile = None
    dsROIVecFile = None
    mem_roi_ds = None
    result_ds = None


def export_spatial_select_feats(
    vec_file: str,
    vec_lyr: str,
    vec_sel_file: str,
    vec_sel_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str,
):
    """
    Function to get a list of attribute values from features which intersect
    with the select layer.

    :param vec_file: vector layer from which the attribute data comes from.
    :param vec_lyr: the layer name from which the attribute data comes from.
    :param vec_sel_file: the vector file which will be intersected within the
                         vector file.
    :param vec_sel_lyr: the layer name which will be intersected within the vector file.
    :param out_vec_file: output vector file/path
    :param out_vec_lyr: output vector layer
    :param out_format: the output vector layer type.

    """
    try:
        dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
        if dsVecFile is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

        lyr_spatial_ref = vec_lyr_obj.GetSpatialRef()

        vec_lyr_bbox = vec_lyr_obj.GetExtent(True)

        # Create polygon for bbox
        ring = ogr.Geometry(ogr.wkbLinearRing)
        ring.AddPoint(vec_lyr_bbox[0], vec_lyr_bbox[3])
        ring.AddPoint(vec_lyr_bbox[1], vec_lyr_bbox[3])
        ring.AddPoint(vec_lyr_bbox[1], vec_lyr_bbox[2])
        ring.AddPoint(vec_lyr_bbox[0], vec_lyr_bbox[2])
        ring.AddPoint(vec_lyr_bbox[0], vec_lyr_bbox[3])
        # Create polygon.
        vec_lyr_bbox_poly = ogr.Geometry(ogr.wkbPolygon)
        vec_lyr_bbox_poly.AddGeometry(ring)

        dsSelVecFile = gdal.OpenEx(vec_sel_file, gdal.OF_READONLY)
        if dsSelVecFile is None:
            raise rsgislib.RSGISPyException("Could not open '" + vec_sel_file + "'")

        lyrSelVecObj = dsSelVecFile.GetLayerByName(vec_sel_lyr)
        if lyrSelVecObj is None:
            raise rsgislib.RSGISPyException(
                "Could not find layer '" + vec_sel_lyr + "'"
            )

        geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
        for feat in lyrSelVecObj:
            geom = feat.GetGeometryRef()
            if geom is not None:
                if geom.Intersects(vec_lyr_bbox_poly):
                    geom_collect.AddGeometry(geom)

        out_driver = ogr.GetDriverByName(out_format)
        result_ds = out_driver.CreateDataSource(out_vec_file)
        result_lyr = result_ds.CreateLayer(
            out_vec_lyr, lyr_spatial_ref, geom_type=vec_lyr_obj.GetGeomType()
        )

        srcLayerDefn = vec_lyr_obj.GetLayerDefn()
        for i in range(srcLayerDefn.GetFieldCount()):
            fieldDefn = srcLayerDefn.GetFieldDefn(i)
            result_lyr.CreateField(fieldDefn)

        counter = 0
        openTransaction = False
        for feat in vec_lyr_obj:
            if not openTransaction:
                result_lyr.StartTransaction()
                openTransaction = True
            geom = feat.GetGeometryRef()
            if geom is not None:
                if geom.Intersects(geom_collect):
                    result_lyr.CreateFeature(feat)
                    counter = counter + 1
            if ((counter % 20000) == 0) and openTransaction:
                result_lyr.CommitTransaction()
                openTransaction = False

        if openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        dsSelVecFile = None
        dsVecFile = None
        result_ds = None
    except Exception as e:
        raise e


def spatial_select_gp(
    vec_in_file: str,
    vec_in_lyr: str,
    vec_roi_file: str,
    vec_roi_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    tmp_col_name: str = "tmp_sel_join_fid",
    vec_in_epsg: int = None,
    vec_roi_epsg: int = None,
):
    """
    A function which spatially selects features from the input vector layer which
    intersects the ROI vector layer. This function is implemented using geopandas
    and is generally faster than the export_spatial_select_feats or
    select_intersect_feats functions.

    Note, defining epsg codes for the datasets does not reproject the datasets
    but just makes sure that correct projection is being used.

    :param vec_in_file: the input vector file path.
    :param vec_in_lyr: the input vector layer name
    :param vec_roi_file: the roi vector file path
    :param vec_roi_lyr: the roi vector layer name
    :param out_vec_file: the output vector file path
    :param out_vec_lyr: the output vector layer name
    :param out_format: the output vector format (e.g., GPKG).
    :param tmp_col_name: The name of a temporary column added to the input layer
                         used to ensure there are no duplicated features in the output
                         layer. The default name is: "tmp_sel_join_fid".
    :param vec_in_epsg: Optionally provide the epsg code for the input vector layer.
    :param vec_roi_epsg: Optionally provide the epsg code for the roi vector layer.

    """
    import geopandas
    import numpy

    print("Read vector layers")
    in_gpdf = geopandas.read_file(vec_in_file, layer=vec_in_lyr)
    roi_gpdf = geopandas.read_file(vec_roi_file, layer=vec_roi_lyr)

    if vec_in_epsg is not None:
        in_gpdf = in_gpdf.set_crs(epsg=vec_in_epsg, allow_override=True)
    if vec_roi_epsg is not None:
        roi_gpdf = roi_gpdf.set_crs(epsg=vec_roi_epsg, allow_override=True)

    # Add column with unique id for each row.
    in_gpdf[tmp_col_name] = numpy.arange(1, (in_gpdf.shape[0]) + 1, 1, dtype=int)

    print("Perform Selection")
    in_sel_gpdf = geopandas.sjoin(
        in_gpdf, roi_gpdf, how="inner", predicate="intersects"
    )
    # Remove any duplicate features using the tmp column
    in_sel_gpdf.drop_duplicates(subset=[tmp_col_name], inplace=True)
    # Remove the tmp column
    in_sel_gpdf.drop(columns=[tmp_col_name, "index_right"], inplace=True)

    print("Export")
    if out_format == "GPKG":
        in_sel_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        in_sel_gpdf.to_file(out_vec_file, driver=out_format)


def get_vec_lyr_cols(vec_file: str, vec_lyr: str) -> List[str]:
    """
    A function which returns a list of columns from the input vector layer.

    :param vec_file: input vector file.
    :param vec_lyr: input vector layer
    :returns: list of column names

    """

    atts = []

    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if dsVecFile is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

    lyr_defn = vec_lyr_obj.GetLayerDefn()
    for i in range(lyr_defn.GetFieldCount()):
        atts.append(lyr_defn.GetFieldDefn(i).GetName())
    return atts


def subset_envs_vec_lyr_obj(
    vec_lyr_obj: ogr.Layer, bbox: List, epsg: int = None
) -> (ogr.DataSource, ogr.Layer):
    """
    Function to get an ogr vector layer for the defined bounding box. The returned
    layer is returned as an in memory ogr Layer object.

    :param vec_lyr_obj: OGR Layer Object.
    :param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].
    :param epsg: provide an EPSG code for the layer if not well defined by
                 the input layer.
    :return: OGR Layer and Dataset objects.

    """

    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException(
            "Vector layer object which was provided was None."
        )

    if epsg is not None:
        lyr_spatial_ref = osr.SpatialReference()
        lyr_spatial_ref.ImportFromEPSG(epsg)
    else:
        lyr_spatial_ref = vec_lyr_obj.GetSpatialRef()
    if lyr_spatial_ref is None:
        raise rsgislib.RSGISPyException(
            "The spatial reference for the layer is None - please provide EPSG code."
        )
    lyrDefn = vec_lyr_obj.GetLayerDefn()

    # Copy the Layer to a new in memory OGR Layer.
    mem_driver = ogr.GetDriverByName("MEMORY")
    mem_result_ds = mem_driver.CreateDataSource("MemResultData")
    mem_result_lyr = mem_result_ds.CreateLayer(
        "MemResultLyr", lyr_spatial_ref, geom_type=vec_lyr_obj.GetGeomType()
    )
    for i in range(lyrDefn.GetFieldCount()):
        fieldDefn = lyrDefn.GetFieldDefn(i)
        mem_result_lyr.CreateField(fieldDefn)

    openTransaction = False
    trans_step = 20000
    next_trans = trans_step
    nFeats = vec_lyr_obj.GetFeatureCount(True)
    step = math.floor(nFeats / 10)
    feedback = 10
    feedback_next = step
    counter = 0
    vec_lyr_obj.ResetReading()
    print("Started .0.", end="", flush=True)
    # loop through the input features
    inFeature = vec_lyr_obj.GetNextFeature()
    while inFeature:
        if (nFeats > 10) and (counter == feedback_next):
            print(".{}.".format(feedback), end="", flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10

        if not openTransaction:
            mem_result_lyr.StartTransaction()
            openTransaction = True

        if inFeature is not None:
            geom = inFeature.GetGeometryRef()
            if geom is not None:
                env = geom.GetEnvelope()

                if (
                    bbox[0] <= env[1]
                    and bbox[1] >= env[0]
                    and bbox[2] <= env[3]
                    and bbox[3] >= env[2]
                ):
                    mem_result_lyr.CreateFeature(inFeature)

        if (counter == next_trans) and openTransaction:
            mem_result_lyr.CommitTransaction()
            openTransaction = False
            next_trans = next_trans + trans_step

        inFeature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1
    print(" Completed")

    if openTransaction:
        mem_result_lyr.CommitTransaction()
        openTransaction = False

    return mem_result_ds, mem_result_lyr


def subset_veclyr_to_featboxs(
    vec_file_bbox: str,
    vec_lyr_bbox: str,
    vec_file_tosub: str,
    vec_lyr_tosub: str,
    out_lyr_name: str,
    out_file_base: str,
    out_file_end: str = "gpkg",
    out_format: str = "GPKG",
):
    """
    A function which subsets an input vector layer using the BBOXs of the features
    within another vector layer.

    :param vec_file_bbox: The vector file for the features which define the BBOXs
    :param vec_lyr_bbox: The vector layer for the features which define the BBOXs
    :param vec_file_tosub: The vector file for the layer which is to be subset.
    :param vec_lyr_tosub: The vector layer for the layer which is to be subset.
    :param out_lyr_name: The layer name for the output files - all output files will
                         have the same layer name.
    :param out_file_base: The base name for the output files. A numeric count 0-n will
                          be inserted between this and the ending.
    :param out_file_end: The output file ending (e.g., gpkg).
    :param out_format: The output file driver (e.g., GPKG).

    """
    import rsgislib.vectorgeoms

    bboxes = rsgislib.vectorgeoms.get_geoms_as_bboxs(vec_file_bbox, vec_lyr_bbox)
    print("There are {} bboxes to subset to.".format(len(bboxes)))
    for i in range(len(bboxes)):
        print(bboxes[i])
        grid_02d_ds, grid_02d_lyr = read_vec_lyr_to_mem(vec_file_tosub, vec_lyr_tosub)
        mem_result_ds, mem_result_lyr = subset_envs_vec_lyr_obj(grid_02d_lyr, bboxes[i])
        out_vec_file = "{0}{1}.{2}".format(out_file_base, i, out_file_end)
        write_vec_lyr_to_file(
            mem_result_lyr,
            out_vec_file,
            out_lyr_name,
            out_format,
            options=["OVERWRITE=YES"],
            replace=True,
        )
        mem_result_ds = None
        grid_02d_ds = None


def read_vec_lyr_to_mem(vec_file: str, vec_lyr: str) -> (ogr.DataSource, ogr.Layer):
    """
    A function which reads a vector layer to an OGR in memory layer.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer within the input file.
    :returns: ogr_dataset, ogr_layer

    """

    try:
        vecDS = gdal.OpenEx(vec_file, gdal.OF_READONLY)
        if vecDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

        mem_driver = ogr.GetDriverByName("MEMORY")

        mem_ds = mem_driver.CreateDataSource("MemSelData")
        mem_lyr = mem_ds.CopyLayer(vec_lyr_obj, vec_lyr, ["OVERWRITE=YES"])

    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        raise e
    return mem_ds, mem_lyr


def open_gdal_vec_lyr(
    vec_file: str, vec_lyr: str = None, readonly: bool = True
) -> (ogr.DataSource, ogr.Layer):
    """
    A function which opens a GDAL/OGR vector layer and returns
    the Dataset and Layer objects. Note, the file must be closed
    by setting the dataset to None.

    :param vec_file: the file path to the vector file.
    :param vec_lyr: the name of the vector layer. If None then first layer is returned.
    :param readonly: if False then the layer will be opened and allow editing of the
                   layer while if True (default) then it will be read only.
    :returns: GDAL dataset, GDAL Layer

    """
    if readonly:
        vec_obj_ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR | gdal.OF_READONLY)
    else:
        vec_obj_ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR | gdal.OF_UPDATE)
    if vec_obj_ds is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    if vec_lyr is None:
        lyr_obj = vec_obj_ds.GetLayer()
        if lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not find a layer.")
    else:
        lyr_obj = vec_obj_ds.GetLayerByName(vec_lyr)
        if lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

    return vec_obj_ds, lyr_obj


def get_mem_vec_lyr_subset(
    vec_file: str, vec_lyr: str, bbox: List
) -> (ogr.DataSource, ogr.Layer):
    """
    Function to get an ogr vector layer for the defined bounding box. The returned
    layer is returned as an in memory ogr Layer object.

    :param vec_file: vector layer from which the attribute data comes from.
    :param vec_lyr: the layer name from which the attribute data comes from.
    :param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].
    :returns: OGR Layer and Dataset objects.

    """

    try:
        dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
        if dsVecFile is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_lyr))

        mem_result_ds, mem_result_lyr = subset_envs_vec_lyr_obj(vec_lyr_obj, bbox)

    except Exception as e:
        print("Error: Layer: {} File: {}".format(vec_lyr, vec_file))
        raise e
    return mem_result_ds, mem_result_lyr


def write_vec_lyr_to_file(
    vec_lyr_obj: ogr.Layer,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str,
    options: List = [],
    replace: bool = False,
):
    """
    A function which reads a vector layer to an OGR in memory layer.

    :param vec_lyr_obj: OGR vector layer object
    :param out_vec_file: output vector file
    :param out_vec_lyr: output vector layer within the input file.
    :param out_format: the OGR driver for the output file.
    :param options: provide a list of driver specific options (e.g., 'OVERWRITE=YES');
                    see https://www.gdal.org/ogr_formats.html
    :param replace: if true the output file is replaced (i.e., overwritten to anything
                    in an existing file will be lost).

    """

    try:
        if os.path.exists(out_vec_file) and replace:
            delete_vector_file(out_vec_file)

        if os.path.exists(out_vec_file) and (not replace):
            print(
                "Warning: some drivers (e.g., GeoJSON) might have problems "
                "re-adding layer to existing file: write_vec_lyr_to_file."
            )
            vecDS = gdal.OpenEx(out_vec_file, gdal.GA_Update)
        else:
            outdriver = ogr.GetDriverByName(out_format)
            vecDS = outdriver.CreateDataSource(out_vec_file)

        if vecDS is None:
            raise rsgislib.RSGISPyException(
                "Could not open or create '{}'".format(out_vec_file)
            )

        vecDS_lyr = vecDS.CopyLayer(vec_lyr_obj, out_vec_lyr, options)
        vecDS = None

    except Exception as e:
        print("Error Vector File: {}".format(out_vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(out_vec_lyr), file=sys.stderr)
        raise e


def create_copy_vector_lyr(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str,
    options: List = [],
    replace: bool = False,
    in_memory: bool = False,
):
    """
    A function which creates a copy of the input vector layer.

    :param vec_file: the file path to the vector file.
    :param vec_lyr: the name of the vector layer. If None then first layer is returned.
    :param out_vec_file: output vector file
    :param out_vec_lyr: output vector layer within the input file.
    :param out_format: the OGR driver for the output file.
    :param options: provide a list of driver specific options (e.g., 'OVERWRITE=YES');
                    see https://www.gdal.org/ogr_formats.html
    :param replace: if true the output file is replaced (i.e., overwritten to anything
                    in an existing file will be lost).
    :param in_memory: If true vector layer will be read into memory and then outputted.

    """
    if in_memory:
        vec_obj_ds, vec_lyr_obj = read_vec_lyr_to_mem(vec_file, vec_lyr)
    else:
        vec_obj_ds, vec_lyr_obj = open_gdal_vec_lyr(vec_file, vec_lyr)

    write_vec_lyr_to_file(
        vec_lyr_obj, out_vec_file, out_vec_lyr, out_format, options, replace
    )

    vec_obj_ds = None


def get_ogr_vec_col_datatype_from_gdal_rat_col_datatype(rat_datatype: int) -> int:
    """
    Returns the data type to create a column in a OGR vector layer for equalivant to
    rat_datatype.

    :param rat_datatype: the datatype (GFT_Integer, GFT_Real, GFT_String) for
                         the RAT column.
    :returns: OGR datatype (OFTInteger, OFTReal, OFTString)

    """
    if rat_datatype == gdal.GFT_Integer:
        rtn_type = ogr.OFTInteger
    elif rat_datatype == gdal.GFT_Real:
        rtn_type = ogr.OFTReal
    elif rat_datatype == gdal.GFT_String:
        rtn_type = ogr.OFTString
    else:
        raise rsgislib.RSGISPyException("Do not recognise inputted datatype")
    return rtn_type


def copy_rat_cols_to_vector_lyr(
    vec_file: str,
    vec_lyr: str,
    rat_row_col: str,
    clumps_img: str,
    ratcols: List,
    out_col_names: List = None,
    out_col_types: List = None,
):
    """
    A function to copy columns from RAT to a vector layer. Note, the vector layer
    needs a column, which already exists, that specifies the row from the RAT the
    feature is related to. If you created the vector using the polygonise
    function then that column will have been created and called 'PXLVAL'.

    :param vec_file: The vector file to be used.
    :param vec_lyr: The name of the layer within the vector file.
    :param rat_row_col: The column in the layer which specifies the RAT row the
                        feature corresponds with.
    :param clumps_img: The clumps image with the RAT from which information
                       should be taken.
    :param ratcols: The names of the columns in the RAT to be copied.
    :param out_col_names: If you do not want the same column names as the RAT then
                          you can specify alternatives. If None then the names will
                          be the same as the RAT. (Default = None)
    :param out_col_types: The data types used for the columns in vector layer. If None
                          then matched to RAT. Default is None

    """

    from rios import rat

    if out_col_names is None:
        out_col_names = ratcols
    else:
        if len(out_col_names) != len(ratcols):
            raise rsgislib.RSGISPyException(
                "The output columns names list is not the same length ({}) as "
                "the length of the RAT columns list ({}) - they must be "
                "the same.".format(len(out_col_names), len(ratcols))
            )

    if out_col_types is not None:
        if len(out_col_names) == len(out_col_types):
            raise rsgislib.RSGISPyException(
                "Either specify the column types as None or the length of the "
                "list needs to be the same as the number output columns."
            )

    if not os.path.exists(vec_file):
        raise rsgislib.RSGISPyException(
            "Input vector does not exist, check path: {}".format(vec_file)
        )

    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise rsgislib.RSGISPyException(
            "Could not open the inputted clumps image: {}".format(clumps_img)
        )

    vecDS = gdal.OpenEx(vec_file, gdal.GA_Update)
    if vecDS is None:
        raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

    vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("Could not find layer '{}'".format(vec_file))

    rat_cols_all = rsgislib.rastergis.get_rat_columns_info(clumps_img)

    cols_exist = []
    for ratcol in ratcols:
        if ratcol not in rat_cols_all:
            raise rsgislib.RSGISPyException(
                "Column '{}' is not within the clumps image: {}".format(
                    ratcol, clumps_img
                )
            )
        cols_exist.append(False)

    if out_col_types is None:
        out_col_types = []
        for att_column in ratcols:
            rat_type = rat_cols_all[att_column]["type"]
            ogr_type = get_ogr_vec_col_datatype_from_gdal_rat_col_datatype(rat_type)
            out_col_types.append(ogr_type)

    lyrDefn = vec_lyr_obj.GetLayerDefn()

    rat_row_col_exists = False
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == rat_row_col.lower():
            rat_row_col_exists = True
            break
    if not rat_row_col_exists:
        raise rsgislib.RSGISPyException(
            "Could not find column '{}' within the vector layers.".format(rat_row_col)
        )

    for i in range(lyrDefn.GetFieldCount()):
        col_n = 0
        for att_column in out_col_names:
            if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
                cols_exist[col_n] = True
                break
            col_n = col_n + 1

    col_n = 0
    for att_column in out_col_names:
        if not cols_exist[col_n]:
            field_defn = ogr.FieldDefn(att_column, out_col_types[col_n])
            if vec_lyr_obj.CreateField(field_defn) != 0:
                raise rsgislib.RSGISPyException(
                    "Creating '{}' field failed; becareful with case, some drivers "
                    "are case insensitive but column might not be found.".format(
                        att_column
                    )
                )

            cols_exist[col_n] = True
        col_n = col_n + 1

    # Read in the RAT columns
    rat_cols_data = []
    for att_column in ratcols:
        rat_cols_data.append(rat.readColumn(clumps_img_ds, att_column))

    vec_lyr_obj.ResetReading()
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids = []
    for feat in vec_lyr_obj:
        fids.append(feat.GetFID())

    openTransaction = False
    vec_lyr_obj.ResetReading()
    i = 0
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    for fid in fids:
        if not openTransaction:
            vec_lyr_obj.StartTransaction()
            openTransaction = True
        feat = vec_lyr_obj.GetFeature(fid)
        if feat is not None:
            rat_row = feat.GetFieldAsInteger(rat_row_col)
            for n_col in range(len(out_col_names)):
                if out_col_types[n_col] == ogr.OFTInteger:
                    feat.SetField(
                        "{}".format(out_col_names[n_col]),
                        int(rat_cols_data[n_col][rat_row]),
                    )
                elif out_col_types[n_col] == ogr.OFTReal:
                    feat.SetField(
                        "{}".format(out_col_names[n_col]),
                        float(rat_cols_data[n_col][rat_row]),
                    )
                elif out_col_types[n_col] == ogr.OFTString:
                    feat.SetField(
                        "{}".format(out_col_names[n_col]),
                        "{}".format(rat_cols_data[n_col][rat_row]),
                    )
                else:
                    feat.SetField(
                        "{}".format(out_col_names[n_col]), rat_cols_data[n_col][rat_row]
                    )
            vec_lyr_obj.SetFeature(feat)
        if ((i % 20000) == 0) and openTransaction:
            vec_lyr_obj.CommitTransaction()
            openTransaction = False
        i = i + 1
    if openTransaction:
        vec_lyr_obj.CommitTransaction()
        openTransaction = False
    vec_lyr_obj.SyncToDisk()
    vecDS = None
    clumps_img_ds = None


def does_vmsk_img_intersect(
    input_vmsk_img: str,
    vec_roi_file: str,
    vec_roi_lyr: str,
    tmp_dir: str,
    vec_epsg: int = None,
):
    """
    This function checks whether the input binary raster mask intersects with the
    input vector layer. A check is first done as to whether the bounding boxes
    intersect, if they do then the intersection between the images is then calculated.
    The input image and vector can be in different projections but the projection
    needs to be well defined.

    :param input_vmsk_img: Input binary mask image file.
    :param vec_roi_file: The input vector file.
    :param vec_roi_lyr: The name of the input layer.
    :param tmp_dir: a temporary directory for files generated during processing.
    :param vec_epsg: If projection is poorly defined by the vector layer then
                     it can be specified.

    """
    import rsgislib.imagecalc
    import rsgislib.tools.filetools
    import rsgislib.tools.geometrytools
    import rsgislib.tools.utils
    import rsgislib.vectorutils.createrasters

    # Does the input image BBOX intersect the BBOX of the ROI vector?
    if vec_epsg is None:
        vec_epsg = get_proj_epsg_from_vec(vec_roi_file, vec_roi_lyr)
    img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(input_vmsk_img)
    if img_epsg == vec_epsg:
        img_bbox = rsgislib.imageutils.get_img_bbox(input_vmsk_img)
        projs_match = True
    else:
        img_bbox = rsgislib.imageutils.get_img_bbox_in_proj(input_vmsk_img, vec_epsg)
        projs_match = False
    vec_bbox = get_vec_layer_extent(vec_roi_file, vec_roi_lyr, compute_if_exp=True)

    img_intersect = False
    if rsgislib.tools.geometrytools.do_bboxes_intersect(img_bbox, vec_bbox):
        uid_str = rsgislib.tools.utils.uid_generator()
        base_vmsk_img = rsgislib.tools.filetools.get_file_basename(input_vmsk_img)

        tmp_file_dir = os.path.join(tmp_dir, "{}_{}".format(base_vmsk_img, uid_str))
        if not os.path.exists(tmp_file_dir):
            os.mkdir(tmp_file_dir)

        # Rasterise the vector layer to the input image extent.
        mem_ds, mem_lyr = get_mem_vec_lyr_subset(vec_roi_file, vec_roi_lyr, img_bbox)

        if not projs_match:
            mem_result_ds, mem_result_lyr = reproj_vec_lyr_obj(
                mem_lyr,
                "mem_vec",
                img_epsg,
                out_format="MEMORY",
                out_vec_lyr=None,
                in_epsg=None,
                print_feedback=False,
            )
            mem_ds = None
        else:
            mem_result_ds = mem_ds
            mem_result_lyr = mem_lyr

        roi_img = os.path.join(tmp_file_dir, "{}_roiimg.kea".format(base_vmsk_img))
        rsgislib.imageutils.create_copy_img(
            input_vmsk_img, roi_img, 1, 0, "KEA", rsgislib.TYPE_8UINT
        )
        rsgislib.vectorutils.createrasters.rasterise_vec_lyr_obj(
            mem_result_lyr,
            roi_img,
            burn_val=1,
            att_column=None,
            calc_stats=True,
            thematic=True,
            no_data_val=0,
        )
        mem_result_ds = None

        bandDefns = []
        bandDefns.append(rsgislib.imagecalc.BandDefn("vmsk", input_vmsk_img, 1))
        bandDefns.append(rsgislib.imagecalc.BandDefn("roi", roi_img, 1))
        intersect_img = os.path.join(
            tmp_file_dir, "{}_intersectimg.kea".format(base_vmsk_img)
        )
        rsgislib.imagecalc.band_math(
            intersect_img,
            "(vmsk==1) && (roi==1)?1:0",
            "KEA",
            rsgislib.TYPE_8UINT,
            bandDefns,
        )
        rsgislib.rastergis.pop_rat_img_stats(
            intersect_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
        )
        n_vals = rsgislib.imagecalc.count_pxls_of_val(intersect_img, vals=[1])
        if n_vals[0] > 0:
            img_intersect = True
        shutil.rmtree(tmp_file_dir)
    return img_intersect


def merge_to_multi_layer_vec(
    input_file_lyrs: List,
    out_vec_file: str,
    out_format: str = "GPKG",
    overwrite: bool = True,
):
    """
    A function which takes a list of vector files and layers (as VecLayersInfoObj
    objects) and merged them into a multi-layer vector file.

    :param input_file_lyrs: list of VecLayersInfoObj objects.
    :param out_vec_file: output vector file.
    :param out_format: output format Default='GPKG'.
    :param overwrite: bool (default = True) specifying whether the input file should be
                      overwritten if it already exists.
    """
    first = True
    for vec in input_file_lyrs:
        vec_ds_obj, vec_lyr_obj = open_gdal_vec_lyr(vec.vec_file, vec.vec_lyr)
        if first and overwrite:
            write_vec_lyr_to_file(
                vec_lyr_obj,
                out_vec_file,
                vec.vec_out_lyr,
                out_format,
                options=["OVERWRITE=YES"],
                replace=True,
            )
        else:
            write_vec_lyr_to_file(
                vec_lyr_obj, out_vec_file, vec.vec_out_lyr, out_format
            )
        vec_ds_obj = None
        first = False


def vector_translate(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str = None,
    out_format: str = "GPKG",
    drv_create_opts: List = [],
    lyr_create_opts: List = [],
    access_mode: str = None,
    src_srs: osr.SpatialReference = None,
    dst_srs: osr.SpatialReference = None,
    del_exist_vec: bool = False,
):
    """
    A function which translates a vector file to another format, similar to ogr2ogr.
    If you wish to reproject the input file then provide a destination srs
    (e.g., "EPSG:27700", or wkt string, or proj4 string).

    :param vec_file: the input vector file.
    :param vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same
                        as the input).
    :param out_format: the output vector file format (e.g., GPKG, GEOJSON, etc.)
    :param drv_create_opts: a list of options for the creation of the output file.
    :param lyr_create_opts: a list of options for the creation of the output layer.
    :param access_mode: default is None for creation but other but other options are:
                        [None (creation), 'update', 'append', 'overwrite']
    :param src_srs: provide a source spatial reference for the input vector file.
                    Default=None. can be used to provide a projection where none has
                    been specified or the information has gone missing. Can be used
                    without performing a reprojection.
    :param dst_srs: provide a spatial reference for the output vector file to be
                    reprojected to. (Default=None) If specified then the file will
                    be reprojected.
    :param del_exist_vec: remove output file if it exists.

    """
    from osgeo import gdal

    if access_mode is not None:
        if access_mode not in ["update", "append", "overwrite"]:
            raise rsgislib.RSGISPyException(
                "access_mode must be one of: [None (creation), "
                "'update', 'append', 'overwrite']"
            )

    if os.path.exists(out_vec_file) and del_exist_vec:
        delete_vector_file(out_vec_file)

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    if out_vec_lyr is None:
        out_vec_lyr = vec_lyr

    reproject_lyr = False
    if dst_srs is not None:
        reproject_lyr = True

    if src_srs is not None:
        if dst_srs is None:
            dst_srs = src_srs

    opts = gdal.VectorTranslateOptions(
        options=drv_create_opts,
        format=out_format,
        accessMode=access_mode,
        srcSRS=src_srs,
        dstSRS=dst_srs,
        reproject=reproject_lyr,
        layerCreationOptions=lyr_create_opts,
        layers=vec_lyr,
        layerName=out_vec_lyr,
        callback=callback,
    )
    in_vec_ds = gdal.OpenEx(vec_file)

    if os.path.exists(out_vec_file):
        out_vec_ds = gdal.OpenEx(out_vec_file, gdal.GA_Update)
        gdal.VectorTranslate(out_vec_ds, in_vec_ds, options=opts)
        out_vec_ds = None
    else:
        gdal.VectorTranslate(out_vec_file, in_vec_ds, options=opts)
    in_vec_ds = None


def reproj_wgs84_vec_to_utm(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str = None,
    use_hemi: bool = True,
    out_format: str = "GPKG",
    drv_create_opts: List = [],
    lyr_create_opts: List = [],
    access_mode: str = "overwrite",
    del_exist_vec: bool = False,
):
    """
    A function which reprojects an input file projected in WGS84 (EPSG:4326) to UTM,
    where the UTM zone is automatically identified using the mean x and y.

    :param vec_file: the input vector file.
    :param vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same
                        as the input).
    :param use_hemi: True differentiate between Southern and Northern hemisphere.
                     False use Northern hemisphere.
    :param out_format: the output vector file format (e.g., GPKG, GEOJSON, etc.)
    :param drv_create_opts: a list of options for the creation of the output file.
    :param lyr_create_opts: a list of options for the creation of the output layer.
    :param access_mode: by default the function overwrites the output file but other
                        options are: ['update', 'append', 'overwrite']
    :param del_exist_vec: remove output file if it exists.

    """
    import os

    import tqdm
    from osgeo import gdal

    import rsgislib.tools.projection
    import rsgislib.tools.utm
    import rsgislib.vectorgeoms

    if os.path.exists(out_vec_file):
        if del_exist_vec:
            delete_vector_file(out_vec_file)
        else:
            raise rsgislib.RSGISPyException(
                "The output vector file ({}) already exists, "
                "remove it and re-run.".format(out_vec_file)
            )

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    vec_ds_obj = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    vec_lyr_obj = vec_ds_obj.GetLayer(vec_lyr)

    pts_lst = list()
    n_feats = vec_lyr_obj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    counter = 0
    in_feature = vec_lyr_obj.GetNextFeature()
    while in_feature:
        geom = in_feature.GetGeometryRef()
        if geom is not None:
            rsgislib.vectorgeoms.get_geom_pts(geom, pts_lst)
        in_feature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1
        pbar.update(1)
    pbar.close()
    vec_ds_obj = None
    lon = 0.0
    lat = 0.0
    for pt in pts_lst:
        lon += pt[0]
        lat += pt[1]
    n_pts = len(pts_lst)

    lon = lon / n_pts
    lat = lat / n_pts

    print("{}, {}".format(lat, lon))

    utm_zone = rsgislib.tools.utm.latlon_to_zone_number(lat, lon)
    hemi = "N"
    if use_hemi and (lat < 0):
        hemi = "S"
    print("UTM Zone: {}{}".format(utm_zone, hemi))

    out_epsg = rsgislib.tools.utm.epsg_for_utm(utm_zone, hemi)
    print("EPSG: {}".format(out_epsg))

    src_srs_obj = rsgislib.tools.projection.get_osr_prj_obj(4326)
    dst_srs_obj = rsgislib.tools.projection.get_osr_prj_obj(out_epsg)

    # dst_srs_str = "EPSG:{}".format(out_epsg)
    vector_translate(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format,
        drv_create_opts,
        lyr_create_opts,
        access_mode,
        src_srs=src_srs_obj,
        dst_srs=dst_srs_obj,
    )


def spatial_select(
    vec_file: str,
    vec_lyr: str,
    vec_roi_file: str,
    vec_roi_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function to perform a spatial selection within the input vector using a
    ROI vector layer. This function uses geopandas so ensure that is installed.

    :param vec_file: Input vector file from which features are selected.
    :param vec_lyr: Input vector file layer from which features are selected.
    :param vec_roi_file: The ROI vector file used to select features within
                         the input file.
    :param vec_roi_lyr: The ROI vector layer used to select features within
                        the input file.
    :param out_vec_file: The output vector file with the selected features.
    :param out_vec_lyr: The output layer file with the selected features.
    :param out_format: the output vector format

    """
    import geopandas
    import numpy
    import tqdm

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    roi_gpdf = geopandas.read_file(vec_roi_file, layer=vec_roi_lyr)
    base_gpdf["msk_rsgis_sel"] = numpy.zeros((base_gpdf.shape[0]), dtype=bool)

    for i in tqdm.tqdm(range(roi_gpdf.shape[0])):
        inter = base_gpdf["geometry"].intersects(roi_gpdf.iloc[i]["geometry"])
        base_gpdf.loc[inter, "msk_rsgis_sel"] = True
    base_gpdf = base_gpdf[base_gpdf["msk_rsgis_sel"]]
    base_gpdf = base_gpdf.drop(["msk_rsgis_sel"], axis=1)
    if base_gpdf.shape[0] > 0:
        if out_format == "GPKG":
            base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            base_gpdf.to_file(out_vec_file, driver=out_format)
    else:
        raise rsgislib.RSGISPyException("No output file as no features intersect.")


def split_by_attribute(
    vec_file: str,
    vec_lyr: str,
    split_col_name: str,
    multi_layers: bool = True,
    out_vec_file: str = None,
    out_file_path: str = None,
    out_file_ext: str = None,
    out_format: str = "GPKG",
    dissolve: bool = False,
    chk_lyr_names: bool = True,
):
    """
    A function which splits a vector layer by an attribute value into either
    different layers or different output files.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param split_col_name: The column name by which the vector layer will be split.
    :param multi_layers: Boolean (default True). If True then a mulitple layer output
                         file will be created (e.g., GPKG). If False then individual
                         files will be outputted.
    :param out_vec_file: Output vector file - only used if multi_layers = True
    :param out_file_path: Output file path (directory) if multi_layers = False.
    :param out_file_ext: Output file extension is multi_layers = False
    :param out_format: The output format (e.g., GPKG, ESRI Shapefile).
    :param dissolve: Boolean (Default=False) if True then a dissolve on the specified
                     variable will be run as layers are separated.
    :param chk_lyr_names: If True (default) layer names (from split_col_name) will be
                          checked, which means punctuation removed and all characters
                          being ascii characters.

    """
    import os

    import geopandas
    import tqdm

    import rsgislib.tools.utils

    if multi_layers:
        if out_vec_file is None:
            raise rsgislib.RSGISPyException(
                "If a multiple layer output is specified then an output "
                "file needs to be specified to which the layer need to be added."
            )
    if not multi_layers:
        if (out_file_path is None) or (out_file_ext is None):
            raise rsgislib.RSGISPyException(
                "If a single layer output is specified then an output file path "
                "and file extension needs to be specified."
            )

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    unq_col = base_gpdf[split_col_name]
    unq_vals = unq_col.unique()

    for val in tqdm.tqdm(unq_vals):
        # Subset to value.
        c_gpdf = base_gpdf.loc[base_gpdf[split_col_name] == val]
        # Check for empty or NA geometries.
        c_gpdf = c_gpdf[~c_gpdf.is_empty]
        c_gpdf = c_gpdf[~c_gpdf.isna()]
        # Dissolve if requested.
        if dissolve:
            # Test resolve if an error thrown then it it probably a topological
            # error which can sometimes be solved using a 0 buffer, so try that
            # to see if it works.
            try:
                c_gpdf.dissolve(by=split_col_name)
            except:
                c_gpdf["geometry"] = c_gpdf.buffer(0)
                c_gpdf = c_gpdf.dissolve(by=split_col_name)
        # Write output to disk.
        val_str = f"{val}"
        if multi_layers and (out_format == "GPKG"):
            if chk_lyr_names:
                val_str = rsgislib.tools.utils.check_str(
                    val_str,
                    rm_non_ascii=True,
                    rm_dashs=True,
                    rm_spaces=False,
                    rm_punc=True,
                )
            c_gpdf.to_file(out_vec_file, layer=val_str, driver="GPKG")
        else:
            if chk_lyr_names:
                val_str = rsgislib.tools.utils.check_str(
                    val_str,
                    rm_non_ascii=True,
                    rm_dashs=True,
                    rm_spaces=False,
                    rm_punc=True,
                )
            out_vec_file = os.path.join(
                out_file_path, "vec_{}.{}".format(val_str, out_file_ext)
            )
            out_vec_lyr = "vec_{}".format(val_str)
            if out_format == "GPKG":
                c_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
            else:
                c_gpdf.to_file(out_vec_file, driver=out_format)


def subset_by_attribute(
    vec_file: str,
    vec_lyr: str,
    sub_col: str,
    sub_vals: List,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    match_type: str = "equals",
):
    """
    A function which subsets an input vector layer based on a list of values.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer
    :param sub_col: The column used to subset the layer.
    :param sub_vals: A list of values used to subset the layer. If using contains or
                     start then regular expressions supported by the re library can
                     be provided.
    :param out_vec_file: The output vector file
    :param out_vec_lyr: The output vector layer
    :param out_format: The output vector format.
    :param match_type: The type of match for the subset. Options: equals (default) -
                       the same value. contains - string is anywhere within attribute
                       value. start - string matches the start of the attribute value.

    """
    import geopandas
    import pandas

    match_type = match_type.lower()
    if match_type not in ["equals", "contains", "start"]:
        raise rsgislib.RSGISPyException(
            "The match_type must be either 'equals', 'contains' or 'start'"
        )

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    first = True
    for val in sub_vals:
        print(val)
        if match_type == "equals":
            tmp_gpdf = base_gpdf[base_gpdf[sub_col] == val]
        elif match_type == "contains":
            tmp_gpdf = base_gpdf[base_gpdf[sub_col].str.contains(val, na=False)]
        elif match_type == "start":
            tmp_gpdf = base_gpdf[base_gpdf[sub_col].str.match(val, na=False)]
        else:
            raise rsgislib.RSGISPyException(
                "The match_type must be either 'equals', 'contains' or 'start'"
            )

        if first:
            out_gpdf = tmp_gpdf.copy(deep=True)
            first = False
        else:
            out_gpdf = pandas.concat([out_gpdf, tmp_gpdf])

    if out_gpdf.shape[0] > 0:
        if out_format == "GPKG":
            out_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            out_gpdf.to_file(out_vec_file, driver=out_format)
    else:
        raise rsgislib.RSGISPyException("No output file as no features selected.")


def select_feats_str_search(
    vec_file: str,
    vec_lyr: str,
    select_col: str,
    select_val: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which select features from a vector layer based on a string value
    within an attribute column. For example, providing a select value 'River'
    would select all features which had river within the column specified such
    as 'River Amazon', 'River Seven' etc. Note, it is case-sensitive.

    :param vec_file: The input vector file path
    :param vec_lyr: the input vector layer name
    :param select_col: the column which is search within
    :param select_val: the value used to select features
    :param out_vec_file: the output file path
    :param out_vec_lyr: the output layer name
    :param out_format: the output format (Default: GPKG)

    """
    import geopandas

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    data_gdf[select_col] = data_gdf[select_col].fillna("")
    sel_data_gdf = data_gdf[data_gdf[select_col].str.contains(select_val)]

    if sel_data_gdf.shape[0] > 0:
        if out_format == "GPKG":
            sel_data_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            sel_data_gdf.to_file(out_vec_file, driver=out_format)
    else:
        raise rsgislib.RSGISPyException("No output file as no features selected.")


def drop_rows_by_attribute(
    vec_file: str,
    vec_lyr: str,
    sub_col: str,
    sub_vals: List,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which subsets an input vector layer based on a list of values.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer
    :param sub_col: The column used to subset the layer.
    :param sub_vals: A list of values used to subset the layer. If using contains or
                     start then regular expressions supported by the re library can
                     be provided.
    :param out_vec_file: The output vector file
    :param out_vec_lyr: The output vector layer
    :param out_format: The output vector format.

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    first = True
    for val in sub_vals:
        print(val)
        if first:
            out_gpdf = base_gpdf.drop(base_gpdf[base_gpdf[sub_col] == val].index)
            first = False
        else:
            out_gpdf = out_gpdf.drop(out_gpdf[out_gpdf[sub_col] == val].index)

    if out_gpdf.shape[0] > 0:
        if out_format == "GPKG":
            out_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            out_gpdf.to_file(out_vec_file, driver=out_format)
    else:
        raise rsgislib.RSGISPyException("No output file as no features selected.")


def merge_vector_files(
    vec_files: List[str],
    out_vec_file: str,
    out_vec_lyr: str = None,
    out_format: str = "GPKG",
    out_epsg: int = None,
    remove_cols: List[str] = None,
):
    """
    A function which merges the input files into a single output file using geopandas.
    If the input files have multiple layers they are all merged into the output file.

    :param vec_files: List of input files
    :param out_vec_file: output vector file.
    :param out_vec_lyr: output vector layer.
    :param out_format: output file format.
    :param out_epsg: if input layers are different projections then option can be
                     used to define the output projection.
    :param remove_cols: a list of columns to be removed during the merge.

    """
    import geopandas
    import pandas
    import tqdm

    import rsgislib.tools.filetools

    if len(vec_files) == 0:
        raise rsgislib.RSGISPyException("At least 1 layer(s) needs to be provided.")

    print("Read Data")
    gp_lyrs = list()
    for vec_file in tqdm.tqdm(vec_files):
        lyrs = get_vec_lyrs_lst(vec_file)
        for lyr in lyrs:
            data_gdf = geopandas.read_file(vec_file, layer=lyr)
            if out_epsg is not None:
                data_gdf = data_gdf.to_crs(epsg=out_epsg)
            if remove_cols is not None:
                df_col_names = data_gdf.columns.values.tolist()
                cols_to_rm = list()
                for col in remove_cols:
                    if col in df_col_names:
                        cols_to_rm.append(col)
                data_gdf = data_gdf.drop(labels=cols_to_rm, axis=1)
            if len(data_gdf) > 0:
                gp_lyrs.append(data_gdf)

    print("Perform Merge")
    if len(gp_lyrs) > 1:
        data_gdf = pandas.concat(gp_lyrs)
    elif len(gp_lyrs) == 1:
        data_gdf = gp_lyrs[0]
    else:
        raise rsgislib.RSGISPyException("No layers with data were provided")

    print("Export")
    if out_format == "GPKG":
        if out_vec_lyr is None:
            out_vec_lyr = rsgislib.tools.filetools.get_file_basename(
                out_vec_file, check_valid=True
            )
        data_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gdf.to_file(out_vec_file, driver=out_format)


def merge_vector_layers(
    vecs_dict: List,
    out_vec_file: str,
    out_vec_lyr: str = None,
    out_format: str = "GPKG",
    out_epsg: int = None,
    remove_cols: List[str] = None,
):
    """
    A function which merges the input vector layers into a single output
    file using geopandas.

    :param vecs_dict: List of dicts with keys [{'file': '/file/path/to/file.gpkg',
                         'layer': 'layer_name'}] providing the file paths and
                         layer names.
    :param out_vec_file: output vector file.
    :param out_vec_lyr: output vector layer.
    :param out_format: output file format.
    :param out_epsg: if input layers are different projections then option can be
                     used to define the output projection.
    :param remove_cols: a list of columns to be removed during the merge.

    """
    import geopandas
    import pandas
    import tqdm
    import rsgislib.tools.filetools

    gp_lyrs = list()
    for vec_info in tqdm.tqdm(vecs_dict):
        if ("file" in vec_info) and ("layer" in vec_info):
            data_gdf = geopandas.read_file(vec_info["file"], layer=vec_info["layer"])
            if out_epsg is not None:
                data_gdf = data_gdf.to_crs(epsg=out_epsg)
            if remove_cols is not None:
                df_col_names = data_gdf.columns.values.tolist()
                cols_to_rm = list()
                for col in remove_cols:
                    if col in df_col_names:
                        cols_to_rm.append(col)
                data_gdf = data_gdf.drop(labels=cols_to_rm, axis=1)
            if len(data_gdf) > 0:
                gp_lyrs.append(data_gdf)
        else:
            raise rsgislib.RSGISPyException(
                "The inputs should be a list of dicts with keys 'file' and 'layer'."
            )

    print("Perform Merge")
    if len(gp_lyrs) > 1:
        data_gdf = pandas.concat(gp_lyrs)
    elif len(gp_lyrs) == 1:
        data_gdf = gp_lyrs[0]
    else:
        raise rsgislib.RSGISPyException("No layers with data were provided")

    print("Export")
    if out_format == "GPKG":
        if out_vec_lyr is None:
            out_vec_lyr = rsgislib.tools.filetools.get_file_basename(
                out_vec_file, check_valid=True
            )
        data_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gdf.to_file(out_vec_file, driver=out_format)


def geopd_check_polys_wgs84_bounds_geometry(data_gdf, width_thres: float = 350):
    """
    A function which checks a polygons within the geometry of a geopanadas dataframe
    for specific case where they on the east/west edge (i.e., 180 / -180) and are
    therefore being wrapped around the world. For example, this function would change
    a longitude -179.91 to 180.01. The geopandas dataframe will be edit in place.

    This function will import the shapely library.

    :param data_gdf: geopandas dataframe.
    :param width_thres: The threshold (default 350 degrees) for the width of a
                        polygon for which the polygons will be checked, looping
                        through all the coordinates
    :returns: geopandas dataframe

    """
    import geopandas
    from shapely.geometry import LinearRing, Polygon

    out_gdf = geopandas.GeoDataFrame()
    out_gdf["geometry"] = None
    i_geom = 0

    for index, row in data_gdf.iterrows():
        n_east = 0
        n_west = 0
        row_bbox = row["geometry"].bounds
        row_width = row_bbox[2] - row_bbox[0]
        if row_width > width_thres:
            if row["geometry"].geom_type == "Polygon":
                for coord in row["geometry"].exterior.coords:
                    if coord[0] < 0:
                        n_west += 1
                    else:
                        n_east += 1
                east_focus = True
                if n_west > n_east:
                    east_focus = False

                out_coords = []
                for coord in row["geometry"].exterior.coords:
                    out_coord = [coord[0], coord[1]]
                    if east_focus:
                        if coord[0] < 0:
                            diff = coord[0] - -180
                            out_coord[0] = 180 + diff
                    else:
                        if coord[0] > 0:
                            diff = 180 - coord[0]
                            out_coord[0] = -180 - diff
                    out_coords.append(out_coord)

                out_holes = []
                for hole in row["geometry"].interiors:
                    hole_coords = []
                    for coord in hole.coords:
                        out_coord = [coord[0], coord[1]]
                        if east_focus:
                            if coord[0] < 0:
                                diff = coord[0] - -180
                                out_coord[0] = 180 + diff
                        else:
                            if coord[0] > 0:
                                diff = 180 - coord[0]
                                out_coord[0] = -180 - diff
                        hole_coords.append(out_coord)
                    out_holes.append(LinearRing(hole_coords))
                out_gdf.loc[i_geom, "geometry"] = Polygon(out_coords, holes=out_holes)
                i_geom += 1
            elif row["geometry"].geom_type == "MultiPolygon":
                for poly in row["geometry"].geoms:
                    for coord in poly.exterior.coords:
                        if coord[0] < 0:
                            n_west += 1
                        else:
                            n_east += 1
                    east_focus = True
                    if n_west > n_east:
                        east_focus = False

                    out_coords = []
                    for coord in poly.exterior.coords:
                        out_coord = [coord[0], coord[1]]
                        if east_focus:
                            if coord[0] < 0:
                                diff = coord[0] - -180
                                out_coord[0] = 180 + diff
                        else:
                            if coord[0] > 0:
                                diff = 180 - coord[0]
                                out_coord[0] = -180 - diff
                        out_coords.append(out_coord)

                    out_holes = []
                    for hole in poly.interiors:
                        hole_coords = []
                        for coord in hole.coords:
                            out_coord = [coord[0], coord[1]]
                            if east_focus:
                                if coord[0] < 0:
                                    diff = coord[0] - -180
                                    out_coord[0] = 180 + diff
                            else:
                                if coord[0] > 0:
                                    diff = 180 - coord[0]
                                    out_coord[0] = -180 - diff
                            hole_coords.append(out_coord)
                        out_holes.append(LinearRing(hole_coords))
                    out_gdf.loc[i_geom, "geometry"] = Polygon(
                        out_coords, holes=out_holes
                    )
                    i_geom += 1
        else:
            out_gdf.loc[i_geom, "geometry"] = row["geometry"]
            i_geom += 1

    return out_gdf


def merge_utm_vecs_wgs84(
    in_vec_files: List,
    out_vec_file: str,
    out_vec_lyr: str = None,
    out_format: str = "GPKG",
    n_hemi_utm_file: str = None,
    s_hemi_utm_file: str = None,
    width_thres: float = 350,
):
    """
    A function which merges input files in UTM projections to the WGS84 projection
    cutting polygons which wrap from one side of the world to other
    (i.e., 180/-180 boundary).

    :param in_vec_files: list of input files
    :param out_vec_file: output vector file.
    :param out_vec_lyr: output vector layer - only used if output format is GPKG
    :param out_format: output file format.
    :param n_hemi_utm_file: GPKG file with layer per zone (layer names: 01, 02, ...
                            59, 60) each projected in the northern hemisphere UTM
                            projections.
    :param s_hemi_utm_file: GPKG file with layer per zone (layer names: 01, 02, ...
                           59, 60) each projected in the southern hemisphere
                           UTM projections.
    :param width_thres: The threshold (default 350 degrees) for the width of a polygon
                        for which the polygons will be checked, looping through all
                        the coordinates

    """
    import geopandas
    import pandas
    import tqdm

    import rsgislib.tools.geometrytools
    import rsgislib.tools.utils
    import rsgislib.tools.utm
    import rsgislib.vectorgeoms

    if n_hemi_utm_file is None:
        n_hemi_utm_file = os.path.join(
            rsgislib.RSGISLIB_INSTALL_PREFIX,
            "share",
            "rsgislib",
            "utm_zone_boundaries_lyrs_north.gpkg",
        )
        if n_hemi_utm_file is None:
            raise rsgislib.RSGISPyException(
                "An input is needed for n_hemi_utm_file. The RSGISLib "
                "installed version was not be found."
            )
    if s_hemi_utm_file is None:
        s_hemi_utm_file = os.path.join(
            rsgislib.RSGISLIB_INSTALL_PREFIX,
            "share",
            "rsgislib",
            "utm_zone_boundaries_lyrs_south.gpkg",
        )
        if s_hemi_utm_file is None:
            raise rsgislib.RSGISPyException(
                "An input is needed for s_hemi_utm_file. The RSGISLib "
                "installed version was not be found."
            )

    out_gdfs_lst = list()
    for file in tqdm.tqdm(in_vec_files):
        lyrs = get_vec_lyrs_lst(file)
        for lyr in lyrs:
            bbox = get_vec_layer_extent(file, vec_lyr=lyr)
            bbox_area = rsgislib.tools.geometrytools.calc_bbox_area(bbox)
            if bbox_area > 0:
                vec_epsg = get_proj_epsg_from_vec(file, vec_lyr=lyr)
                zone, hemi = rsgislib.tools.utm.utm_from_epsg(int(vec_epsg))
                zone_str = rsgislib.tools.utils.zero_pad_num_str(
                    zone, str_len=2, round_num=False, round_n_digts=0, integerise=True
                )

                if hemi.upper() == "S":
                    utm_zones_file = s_hemi_utm_file
                else:
                    utm_zones_file = n_hemi_utm_file

                contained = rsgislib.vectorgeoms.vec_within_vec(
                    utm_zones_file, zone_str, file, lyr
                )
                if not contained:
                    data_gdf = geopandas.read_file(file, layer=lyr)
                    utm_gdf = geopandas.read_file(utm_zones_file, layer=zone_str)

                    data_inter_gdf = geopandas.overlay(
                        data_gdf, utm_gdf, how="intersection"
                    )
                    data_diff_gdf = geopandas.overlay(
                        data_gdf, utm_gdf, how="difference"
                    )
                    if (len(data_inter_gdf) > 0) and (len(data_diff_gdf) > 0):
                        data_split_gdf = pandas.concat([data_inter_gdf, data_diff_gdf])
                    elif len(data_diff_gdf) > 0:
                        data_split_gdf = data_diff_gdf
                    else:
                        data_split_gdf = data_inter_gdf

                    if len(data_split_gdf) > 0:
                        data_gdf = data_split_gdf.to_crs("EPSG:4326")
                else:
                    data_gdf = geopandas.read_file(file, layer=lyr)
                    if len(data_gdf) > 0:
                        data_gdf = data_gdf.to_crs("EPSG:4326")

                if len(data_gdf) > 0:
                    data_gdf_bounds = data_gdf.bounds
                    widths = data_gdf_bounds["maxx"] - data_gdf_bounds["minx"]
                    if widths.max() > width_thres:
                        data_gdf = geopd_check_polys_wgs84_bounds_geometry(
                            data_gdf, width_thres
                        )
                    out_gdfs_lst.append(data_gdf)

    if len(out_gdfs_lst) > 0:
        out_gdf = pandas.concat(out_gdfs_lst)
        if out_format == "GPKG":
            if out_vec_lyr is None:
                raise rsgislib.RSGISPyException(
                    "If output format is GPKG then an output layer is required."
                )
            out_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            out_gdf.to_file(out_vec_file, driver=out_format)


def split_feats_to_mlyrs(
    vec_file: str, vec_lyr: str, out_vec_file: str, out_format: str = "GPKG"
):
    """
    A function which splits an existing vector layer into multiple layers

    :param vec_file: input vector file
    :param vec_lyr: input vector layer
    :param out_vec_file: output file, note the format must be one which
                         supports multiple layers (e.g., GPKG).
    :param out_format: The output format of the output file.

    """
    import geopandas
    import tqdm

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    for i in tqdm.tqdm(range(base_gpdf.shape[0])):
        tmp_gp_series = base_gpdf.loc[i]
        tmp_gpdf = geopandas.GeoDataFrame([tmp_gp_series])
        vec_out_lyr = "veclyr_{}".format(i)
        tmp_gpdf.to_file(out_vec_file, layer=vec_out_lyr, driver=out_format)
        tmp_gpdf = None
    base_gpdf = None


def split_vec_lyr_random_subset(
    vec_file: str,
    vec_lyr: str,
    out_rmain_vec_file: str,
    out_rmain_vec_lyr: str,
    out_smpl_vec_file: str,
    out_smpl_vec_lyr: str,
    n_smpl: int,
    out_format: str = "GPKG",
    rnd_seed: int = None,
):
    """
    A function to split a vector layer into two subsets by randomly sampling the
    input file. This function uses geopandas so that library must therefore
    be installed.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer.
    :param out_rmain_vec_file: Output vector file with the 'remain' outputs
                              (i.e., the remainder once the sample if taken)
    :param out_rmain_vec_lyr: Output vector layer with the 'remain' outputs
                             (i.e., the remainder once the sample if taken)
    :param out_smpl_vec_file: Output vector file with the sampled outputs
    :param out_smpl_vec_lyr: Output vector layer with the sampled outputs
    :param n_smpl: the number of samples to be randomly selected
    :param out_format: The output format of the output file. (Default: GPKG)
    :param rnd_seed: A seed for the random number generator.

    """
    import geopandas

    n_feats = get_vec_feat_count(vec_file, vec_lyr)
    if n_smpl >= n_feats:
        raise rsgislib.RSGISPyException(
            "The number of samples must be less than the number of features in the file."
        )

    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    smpl_gpdf = base_gpdf.sample(n=n_smpl, random_state=rnd_seed)
    base_gpdf = base_gpdf.drop(smpl_gpdf.index)

    if out_format == "GPKG":
        base_gpdf.to_file(
            out_rmain_vec_file, layer=out_rmain_vec_lyr, driver=out_format
        )
        smpl_gpdf.to_file(out_smpl_vec_file, layer=out_smpl_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_rmain_vec_file, driver=out_format)
        smpl_gpdf.to_file(out_smpl_vec_file, driver=out_format)


def vec_lyr_random_subset(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    n_smpl: int,
    out_format: str = "GPKG",
    rnd_seed: int = None,
):
    """
    A function to create a random subset of features from a vector layer.
    This function uses geopandas so that library must therefore be installed.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer.
    :param out_vec_file: Output vector file with the random outputs
    :param out_vec_lyr: Output vector layer with the random outputs
    :param n_smpl: the number of samples to be randomly selected
    :param out_format: The output format of the output file. (Default: GPKG)
    :param rnd_seed: A seed for the random number generator.

    """
    import geopandas

    n_feats = get_vec_feat_count(vec_file, vec_lyr)
    if n_smpl >= n_feats:
        raise rsgislib.RSGISPyException(
            "The number of samples must be less than the number of features in the file."
        )

    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    smpl_gpdf = base_gpdf.sample(n=n_smpl, random_state=rnd_seed)

    if out_format == "GPKG":
        smpl_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        smpl_gpdf.to_file(out_vec_file, driver=out_format)


def create_train_test_smpls(
    vec_file: str,
    vec_lyr: str,
    out_train_vec_file: str,
    out_train_vec_lyr: str,
    out_test_vec_file: str,
    out_test_vec_lyr: str,
    out_format: str = "GPKG",
    prop_test: float = 0.2,
    tmp_dir: str = "tmp",
    rnd_seed: int = None,
):
    """
    A function for splitting a vector dataset into training and testing datasets.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer.
    :param out_train_vec_file: Output vector file with the training data.
    :param out_train_vec_lyr: Output vector layer with the training data.
    :param out_test_vec_file: Output vector file with the testing data.
    :param out_test_vec_lyr: Output vector layer with the testing data.
    :param out_format: The output format of the output file. (Default: GPKG)
    :param prop_test: Proportion of the dataset to be defined as a the test data
    :param tmp_dir: a temporary directory for intimediate outputs.
    :param rnd_seed: A seed for the random number generator.

    """
    import rsgislib.tools.filetools
    import rsgislib.vectorattrs

    created_tmp_dir = False
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
        created_tmp_dir = True

    basename = rsgislib.tools.filetools.get_file_basename(vec_file)

    vec_fid_file = os.path.join(tmp_dir, "{}_fid.gpkg".format(basename))
    rsgislib.vectorattrs.add_fid_col(vec_file, vec_lyr, vec_fid_file, vec_lyr)

    n_feats = get_vec_feat_count(vec_file, vec_lyr)

    n_test_feats = int(n_feats * prop_test)
    print("Number of Training Features: {}".format(n_feats - n_test_feats))
    print("Number of Testing Features: {}".format(n_test_feats))

    split_vec_lyr_random_subset(
        vec_fid_file,
        vec_lyr,
        out_train_vec_file,
        out_train_vec_lyr,
        out_test_vec_file,
        out_test_vec_lyr,
        n_test_feats,
        out_format=out_format,
        rnd_seed=rnd_seed,
    )

    if created_tmp_dir:
        import shutil

        shutil.rmtree(tmp_dir)


def create_train_test_valid_smpls(
    vec_file: str,
    vec_lyr: str,
    out_train_vec_file: str,
    out_train_vec_lyr: str,
    out_test_vec_file: str,
    out_test_vec_lyr: str,
    out_valid_vec_file: str,
    out_valid_vec_lyr: str,
    out_format: str = "GPKG",
    prop_test: float = 0.2,
    prop_valid: float = 0.2,
    rnd_seed: int = None,
):
    """
    A function for splitting a vector dataset into training, testing
    and validation datasets.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer.
    :param out_train_vec_file: Output vector file with the training data.
    :param out_train_vec_lyr: Output vector layer with the training data.
    :param out_test_vec_file: Output vector file with the testing data.
    :param out_test_vec_lyr: Output vector layer with the testing data.
    :param out_valid_vec_file: Output vector file with the validation data.
    :param out_valid_vec_lyr: Output vector layer with the validation data.
    :param out_format: The output format of the output file. (Default: GPKG)
    :param prop_test: Proportion of the dataset to be defined as a the test data
    :param prop_valid: Proportion of the dataset to be defined as a the validation data
    :param rnd_seed: A seed for the random number generator.

    """
    import geopandas

    print("Read Data:")
    smpl_pts_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    n_smpls = len(smpl_pts_gdf)
    print(f"Total number of samples: {n_smpls}")

    n_test_smpls = int(n_smpls * prop_test)
    print(f"Test Samples: {n_test_smpls}")
    n_valid_smpls = int(n_smpls * prop_valid)
    print(f"Validation Samples: {n_valid_smpls}")
    n_train_smpls = n_smpls - (n_test_smpls + n_valid_smpls)
    print(f"Train Samples: {n_train_smpls}")

    print("Shuffle Samples")
    # Shuffle the samples so they are random order.
    smpl_pts_gdf = smpl_pts_gdf.sample(frac=1, random_state=rnd_seed).reset_index(
        drop=True
    )

    print("Select Sub-Sample")
    test_smpl_pts_gdf = smpl_pts_gdf.sample(n=n_test_smpls, random_state=rnd_seed)
    smpl_pts_remain_gdf = smpl_pts_gdf.drop(test_smpl_pts_gdf.index)
    valid_smpl_pts_gdf = smpl_pts_remain_gdf.sample(
        n=n_valid_smpls, random_state=rnd_seed
    )
    smpl_pts_remain_gdf = smpl_pts_gdf.drop(valid_smpl_pts_gdf.index)

    print("Export")
    if out_format == "GPKG":
        test_smpl_pts_gdf.to_file(
            out_test_vec_file, layer=out_test_vec_lyr, driver=out_format
        )
        valid_smpl_pts_gdf.to_file(
            out_valid_vec_file, layer=out_valid_vec_lyr, driver=out_format
        )
        smpl_pts_remain_gdf.to_file(
            out_train_vec_file, layer=out_train_vec_lyr, driver=out_format
        )
    else:
        test_smpl_pts_gdf.to_file(out_test_vec_file, driver=out_format)
        valid_smpl_pts_gdf.to_file(out_valid_vec_file, driver=out_format)
        smpl_pts_remain_gdf.to_file(out_train_vec_file, driver=out_format)


def rm_feat_att_duplicates(
    vec_file: str,
    vec_lyr: str,
    col_name: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which uses the values within an attribute column
    to remove duplicate features from the vector layer.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param col_name: The column used to define unique features
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
    :param out_format: output file format (default GPKG).

    """
    import geopandas

    out_format = check_format_name(out_format)

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    unq_vals = base_gpdf[col_name].unique()

    if unq_vals.shape[0] != base_gpdf[col_name].shape[0]:
        base_gpdf.drop_duplicates(
            subset=col_name, keep="first", inplace=True, ignore_index=False
        )

    if out_format == "GPKG":
        base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        base_gpdf.to_file(out_vec_file, driver=out_format)


def match_closest_vec_pts(
    vec_base_file: str,
    vec_base_lyr: str,
    vec_match_file: str,
    vec_match_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GeoJSON",
    tolerance=None,
    cp_match_atts=False,
    out_x_col="x_match",
    out_y_col="y_match",
    out_dist_col="dist_match",
    out_att_prefix="match_",
):
    """
    A function which finds the closest point between two vectors of point geometry.
    There is an option to copy the attributes from the matched points to the output
    in which case this can be used as a form of spatial join.

    Note. this function is not intended to be used with large datasets and the full
    distance matrix (i.e., every point to every other point) is calculated.

    :param vec_base_file: Input base vector file which will be outputted
    :param vec_base_lyr: Input base vector layer name.
    :param vec_match_file: Input match vector file which will be matched to the base
                          vector
    :param vec_match_lyr: Input match vector layer name.
    :param out_vec_file: the output vector file path
    :param out_vec_lyr: the output vector layer name
    :param out_format: the output format (Default: GeoJSON)
    :param tolerance: a tolerance threshold where matches over this distance will not
                      be outputted (i.e., the input vector will be subsetted).
    :param cp_match_atts: Copy attributes from the matching vector layer.
    :param out_x_col: the output column name for the matched x coordinate from the
                      vec_match_lyr. Default: x_match
    :param out_y_col: the output column name for the matched y coordinate from the
                      vec_match_lyr. Default: y_match
    :param out_dist_col: the output column name for the distances between the base point
                         and the matched point.
    :param out_att_prefix: A prefix for matched attributes, if outputted
                           (i.e., cp_match_atts = True)
    :returns: boolean where True is outputted if some points are matched and
              False if no points are matched.

    """
    import geopandas
    import numpy

    import rsgislib.tools.geometrytools

    base_gpd_df = geopandas.read_file(vec_base_file, layer=vec_base_lyr)
    match_gpd_df = geopandas.read_file(vec_match_file, layer=vec_match_lyr)

    # calculate the distance matrix between all the points
    dist_matrix = match_gpd_df.geometry.apply(lambda g: base_gpd_df.distance(g))

    # find min distance for each feature in base
    min_dists = dist_matrix.min()

    out_x = list()
    out_y = list()
    out_dists = list()
    match_atts = dict()
    if cp_match_atts:
        for match_col in match_gpd_df.columns:
            if match_col != "geometry":
                match_atts[match_col] = list()

    for i, min_dist in enumerate(min_dists):
        join_feat = numpy.where(dist_matrix[i].values == min_dist)[0]
        if join_feat.shape[0] != 1:
            raise rsgislib.RSGISPyException(
                "Multiple features found - don't know what to do with that!"
            )
        join_feat = join_feat[0]

        base_x = base_gpd_df.iloc[[i]].geometry.x.values[0]
        base_y = base_gpd_df.iloc[[i]].geometry.y.values[0]

        join_x = match_gpd_df.iloc[[join_feat]].geometry.x.values[0]
        join_y = match_gpd_df.iloc[[join_feat]].geometry.y.values[0]
        dist_val = rsgislib.tools.geometrytools.calc_pt_distance(
            base_x, base_y, join_x, join_y
        )

        out_x.append(join_x)
        out_y.append(join_y)
        out_dists.append(dist_val)

        if cp_match_atts:
            for match_col in match_atts:
                match_atts[match_col].append(
                    match_gpd_df.iloc[[join_feat]][match_col].values[0]
                )

    base_gpd_df[out_x_col] = out_x
    base_gpd_df[out_y_col] = out_y
    base_gpd_df[out_dist_col] = out_dists

    if cp_match_atts:
        for match_col in match_atts:
            base_gpd_df[f"{out_att_prefix}{match_col}"] = match_atts[match_col]

    if tolerance is not None:
        base_gpd_df = base_gpd_df[base_gpd_df[out_dist_col] < tolerance]

    found_match = False
    if len(base_gpd_df) > 0:
        if out_format == "GPKG":
            base_gpd_df.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            base_gpd_df.to_file(out_vec_file, driver=out_format)
        found_match = True

    return found_match


def redefine_vec_lyr_proj(
    vec_file: str,
    vec_lyr: str,
    epsg_code: int,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which (re-)defines the projection of a vector layer without
    reprojecting. This is useful if for some reason the projection is either
    incorrectly represented or not properly defined for some reason.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param epsg_code: the epsg code for the projection you are defining the layer to.
    :param out_vec_file: the output vector file
    :param out_vec_lyr: the output vector layer
    :param out_format: the output vector format (Default: GPKG)

    """
    import geopandas

    data_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    data_gpdf = data_gpdf.set_crs(epsg=epsg_code, allow_override=True)

    if out_format == "GPKG":
        data_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gpdf.to_file(out_vec_file, driver=out_format)


def reproj_vec_lyr_gp(
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    epsg_code: int = None,
    wkt_str: str = None,
    out_format: str = "GPKG",
):
    """
    A function which re-projects of a vector layer to a new projection
    using GeoPandas. You must provide either project a ESPG code or WKT
    string representation of the projection.

    Note. this function loads the layer into memory you can use also use
    vector_translate for projection if you do not want that behaviour.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param out_vec_file: the output vector file
    :param out_vec_lyr: the output vector layer
    :param epsg_code: epsg code for the projection you are transforming the layer to.
    :param wkt_str: WKT string for the projection you are transforming the layer to.
    :param out_format: the output vector format (Default: GPKG)

    """
    import geopandas

    if (epsg_code is None) and (wkt_str is None):
        raise rsgislib.RSGISPyException(
            "You must provide either an EPSG code or WKT string for the projection."
        )
    elif (epsg_code is not None) and (wkt_str is not None):
        raise rsgislib.RSGISPyException(
            "You must provide only one of an EPSG code or "
            "WKT string for the projection and not both."
        )

    if epsg_code is not None:
        wkt_str = None
    elif wkt_str is not None:
        epsg_code = None

    data_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    data_gpdf = data_gpdf.to_crs(crs=wkt_str, epsg=epsg_code)

    if out_format == "GPKG":
        data_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gpdf.to_file(out_vec_file, driver=out_format)


def rm_attrib_duplicates(
    vec_file: str,
    vec_lyr: str,
    dup_col: Union[str, List[str]],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    keep_rows: str = "First",
):
    """
    A function which removes rows where a duplicate values within a column(s)
    within the attribute table exists. Either the First or Last feature can
    be kept or all duplicate features can be removed.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param dup_col: a single column or list of columns used to define whether
                    a feature is a duplicate or not.
    :param out_vec_file: Output vector file
    :param out_vec_lyr: Output vector layer
    :param out_format: output vector format (Default: GPKG)
    :param keep_rows: Whether to keep 'First', 'Last' or 'RemoveAll' when a
                      duplicate is found.

    """
    if keep_rows not in ["First", "Last", "RemoveAll"]:
        raise rsgislib.RSGISPyException(
            "keep_rows option must be one of: First, Last, RemoveAll"
        )

    import geopandas

    data_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    keep_val = "first"
    if keep_rows == "Last":
        keep_val = "last"
    elif keep_rows == "RemoveAll":
        keep_val = False

    data_gpdf.drop_duplicates(
        subset=dup_col, keep=keep_val, inplace=True, ignore_index=False
    )

    if out_format == "GPKG":
        data_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gpdf.to_file(out_vec_file, driver=out_format)


def spatial_select_bbox(
    vec_file: str,
    vec_lyr: str,
    bbox: List[float],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    vec_in_epsg: int = None,
):
    """
    A function which spatially subsets the vector layer to the bbox
    [xMin, xMax, yMin, yMax] provided. The function uses geopandas.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].
    :param out_vec_file: Output vector file
    :param out_vec_lyr: Output vector layer
    :param out_format: output vector format (Default: GPKG)
    :param vec_in_epsg: Optionally, the EPSG code of the input vector layer can be
                        specified to ensure the output file has the correct projection.

    """

    import geopandas

    print("Read vector layer")
    in_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    if vec_in_epsg is not None:
        in_gdf = in_gdf.set_crs(epsg=vec_in_epsg, allow_override=True)

    print("Subset")
    sub_gdf = in_gdf.cx[bbox[0] : bbox[1], bbox[2] : bbox[3]]

    print("Export")
    if out_format == "GPKG":
        sub_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        sub_gdf.to_file(out_vec_file, driver=out_format)


def create_n_random_subsets(
    vec_file: str,
    vec_lyr: str,
    out_vec_dir: str,
    out_vec_base: str,
    out_vec_ext: str,
    out_format: str = "GPKG",
    n_subs: int = 10,
    smpl_frac: float = 0.5,
    n_smpl: int = None,
    replacement: bool = False,
    rnd_seed: int = None,
):
    """
    A function which creates n random subsets of the features within a vector
    layer. This is useful when running a bootstrapping process or similar.

    :param vec_file: input vector file path
    :param vec_lyr: input vector layer name
    :param out_vec_dir: output directory path
    :param out_vec_base: output base file name
    :param out_vec_ext: output file extension for the vector files (e.g., gpkg)
    :param out_format: output vector file format (Default: GPKG)
    :param n_subs: the number of subsets to generate (Default: 10).
    :param smpl_frac: the fraction of the whole data to take as the subset. Note,
                      if n_smpl is defined it will be used over smpl_frac. (Defualt: 10)
    :param n_smpl: the number of samples to take from the input layer for each subset
                   (Default: None). Note, if n_smpl is defined (i.e., not None) then
                   smpl_frac will be ignored.
    :param replacement: Boolean specifying whether the random subset is selected with
                        replacement (Default: False)
    :param rnd_seed: A seed for the random selection. Default: None.

    """
    import geopandas
    import tqdm

    if not os.path.exists(out_vec_dir):
        os.mkdir(out_vec_dir)

    base_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    for i in tqdm.tqdm(range(n_subs)):
        if n_smpl is not None:
            sub_gdf = base_gdf.sample(
                n=n_subs, frac=None, replace=replacement, random_state=rnd_seed, axis=0
            )
        else:
            sub_gdf = base_gdf.sample(
                n=None,
                frac=smpl_frac,
                replace=replacement,
                random_state=rnd_seed,
                axis=0,
            )

        out_vec_lyr = f"{out_vec_base}_{i}"
        out_vec_file = os.path.join(out_vec_dir, f"{out_vec_lyr}.{out_vec_ext}")

        if out_format == "GPKG":
            sub_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            sub_gdf.to_file(out_vec_file, driver=out_format)


def drop_rows_by_multi_attributes(
    vec_file: str,
    vec_lyr: str,
    drop_atts: Dict[str, Union[float, int, str]],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which subsets an input vector layer based on a list of attributes
    and values.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer
    :param drop_atts: dictionary of column names and values to be dropped.
    :param out_vec_file: The output vector file
    :param out_vec_lyr: The output vector layer
    :param out_format: The output vector format.

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    first = True
    for col in drop_atts:
        print(col)
        if first:
            out_gpdf = base_gpdf.drop(base_gpdf[base_gpdf[col] == drop_atts[col]].index)
            first = False
        else:
            out_gpdf = out_gpdf.drop(out_gpdf[out_gpdf[col] == drop_atts[col]].index)

    if out_gpdf.shape[0] > 0:
        if out_format == "GPKG":
            out_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            out_gpdf.to_file(out_vec_file, driver=out_format)
    else:
        raise rsgislib.RSGISPyException("No output file as no features selected.")
