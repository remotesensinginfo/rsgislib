#!/usr/bin/env python
"""
The zonal stats module provides functions to perform pixel-in-polygon or
point-in-pixel analysis.

For undertaking a pixel-in-polygon analysis you need to consider the size of the
polygons with respect to the size of the pixels being intersected.

Where the pixels are small with respect to the polygons so there is at least one pixel
within the polygon then the best function to use is:

* rsgislib.zonalstats.calc_zonal_band_stats

If the pixels are large with respect to the polygons then use the following function
which intersects the polygon centroid.

* rsgislib.zonalstats.calc_zonal_poly_pts_band_stats

If the pixel size in between and/or polygons are varied in size such that it is not
certain that all polygons will contain a pixel then the following function will first
attempt to intersect the polygon with the pixels and if there is not a pixel within
the polygon then the centriod is used.

* rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts


Alternatively, the other functions are slower to execute but have more options with
respect to the method of intersection. The options for intersection are:

* METHOD_POLYCONTAINSPIXEL = 0           # Polygon completely contains pixel
* METHOD_POLYCONTAINSPIXELCENTER = 1     # Pixel center is within the polygon
* METHOD_POLYOVERLAPSPIXEL = 2           # Polygon overlaps the pixel
* METHOD_POLYOVERLAPSORCONTAINSPIXEL = 3 # Polygon overlaps or contains the pixel
* METHOD_PIXELCONTAINSPOLY = 4           # Pixel contains the polygon
* METHOD_PIXELCONTAINSPOLYCENTER = 5     # Polygon center is within pixel
* METHOD_ADAPTIVE = 6                    # The method is chosen based on relative areas
                                         # of pixel and polygon.
* METHOD_ENVELOPE = 7                    # All pixels in polygon envelope chosen
* METHOD_PIXELAREAINPOLY = 8             # Percent of pixel area that is within
                                         # the polygon
* METHOD_POLYAREAINPIXEL = 9             # Percent of polygon area that is within pixel
"""

import math
import sys
from typing import List

import numpy
import tqdm
from osgeo import gdal, ogr, osr

import rsgislib
import rsgislib.imageutils

# import the C++ extension into this level
from ._zonalstats import *

gdal.UseExceptions()

METHOD_POLYCONTAINSPIXEL = 0  # Polygon completely contains pixel
METHOD_POLYCONTAINSPIXELCENTER = 1  # Pixel center is within the polygon
METHOD_POLYOVERLAPSPIXEL = 2  # Polygon overlaps the pixel
METHOD_POLYOVERLAPSORCONTAINSPIXEL = 3  # Polygon overlaps or contains the pixel
METHOD_PIXELCONTAINSPOLY = 4  # Pixel contains the polygon
METHOD_PIXELCONTAINSPOLYCENTER = 5  # Polygon center is within pixel
METHOD_ADAPTIVE = 6  # The method is chosen based on relative areas
# of pixel and polygon.
METHOD_ENVELOPE = 7  # All pixels in polygon envelope chosen
METHOD_PIXELAREAINPOLY = 8  # Percent of pixel area that is within
# the polygon
METHOD_POLYAREAINPIXEL = 9  # Percent of polygon area that is within pixel


def calc_zonal_band_stats_file(
    vec_file: str,
    vec_lyr: str,
    input_img: str,
    img_band: int,
    min_thres: float,
    max_thres: float,
    out_no_data_val: float,
    min_field: str = None,
    max_field: str = None,
    mean_field: str = None,
    stddev_field: str = None,
    sum_field: str = None,
    count_field: str = None,
    mode_field: str = None,
    median_field: str = None,
    vec_def_epsg: int = None,
):
    """
    A function which calculates zonal statistics for a particular image band.
    If you know that the pixels in the values image are small with respect to
    the polygons then use this function.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer within the input file which specifies the
                    features and where the output stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param min_thres: a lower threshold for values which will be included in
                      the stats calculation.
    :param max_thres: a upper threshold for values which will be included in
                      the stats calculation.
    :param out_no_data_val: output no data value if no valid pixels are within
                            the polygon.
    :param min_field: the name of the field for the min value (None or not specified
                      to be ignored).
    :param max_field: the name of the field for the max value (None or not specified
                      to be ignored).
    :param mean_field: the name of the field for the mean value (None or not specified
                       to be ignored).
    :param stddev_field: the name of the field for the standard deviation value (None
                         or not specified to be ignored).
    :param sum_field: the name of the field for the sum value (None or not specified to
                      be ignored).
    :param count_field: the name of the field for the count (of number of pixels) value
                       (None or not specified to be ignored).
    :param mode_field: the name of the field for the mode value (None or not specified
                       to be ignored).
    :param median_field: the name of the field for the median value (None or not
                         specified to be ignored).
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted
                         vector layer.

    """
    try:
        vecDS = gdal.OpenEx(vec_file, gdal.OF_VECTOR | gdal.OF_UPDATE)
        if vecDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not open layer '{}'".format(vec_lyr))

        calc_zonal_band_stats(
            vec_lyr_obj,
            input_img,
            img_band,
            min_thres,
            max_thres,
            out_no_data_val,
            min_field,
            max_field,
            mean_field,
            stddev_field,
            sum_field,
            count_field,
            mode_field,
            median_field,
            vec_def_epsg,
        )

        vecDS = None
    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def calc_zonal_band_stats(
    vec_lyr_obj: ogr.Layer,
    input_img: str,
    img_band: int,
    min_thres: float,
    max_thres: float,
    out_no_data_val: float,
    min_field: str = None,
    max_field: str = None,
    mean_field: str = None,
    stddev_field: str = None,
    sum_field: str = None,
    count_field: str = None,
    mode_field: str = None,
    median_field: str = None,
    vec_def_epsg: int = None,
):
    """
    A function which calculates zonal statistics for a particular image band.
    If you know that the pixels in the values image are small with respect to
    the polygons then use this function.

    :param vec_lyr_obj: OGR vector layer object containing the geometries being
                        processed and to which the stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param min_thres: a lower threshold for values which will be included in
                      the stats calculation.
    :param max_thres: a upper threshold for values which will be included in
                      the stats calculation.
    :param out_no_data_val: output no data value if no valid pixels are within
                            the polygon.
    :param min_field: the name of the field for the min value (None or not specified
                      to be ignored).
    :param max_field: the name of the field for the max value (None or not specified
                      to be ignored).
    :param mean_field: the name of the field for the mean value (None or not specified
                       to be ignored).
    :param stddev_field: the name of the field for the standard deviation value (None
                         or not specified to be ignored).
    :param sum_field: the name of the field for the sum value (None or not specified to
                      be ignored).
    :param count_field: the name of the field for the count (of number of pixels) value
                       (None or not specified to be ignored).
    :param mode_field: the name of the field for the mode value (None or not specified
                       to be ignored).
    :param median_field: the name of the field for the median value (None or not
                         specified to be ignored).
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted
                         vector layer.

    """
    if mode_field is not None:
        import scipy.stats.mstats

    try:
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("The inputted vector layer was None")

        if (
            (min_field is None)
            and (max_field is None)
            and (mean_field is None)
            and (stddev_field is None)
            and (sum_field is None)
            and (count_field is None)
            and (mode_field is None)
            and (median_field is None)
        ):
            raise rsgislib.RSGISPyException(
                "At least one field needs to be specified for there is to an output."
            )

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise rsgislib.RSGISPyException(
                "Could not find image band '{}'".format(img_band)
            )
        imgGeoTrans = imgDS.GetGeoTransform()
        img_wkt_str = imgDS.GetProjection()
        img_spatial_ref = osr.SpatialReference()
        img_spatial_ref.ImportFromWkt(img_wkt_str)
        img_spatial_ref.AutoIdentifyEPSG()
        epsg_img_spatial = img_spatial_ref.GetAuthorityCode(None)

        pixel_width = imgGeoTrans[1]
        pixel_height = imgGeoTrans[5]

        imgSizeX = imgDS.RasterXSize
        imgSizeY = imgDS.RasterYSize

        imgNoDataVal = img_band_obj.GetNoDataValue()

        if vec_def_epsg is None:
            veclyr_spatial_ref = vec_lyr_obj.GetSpatialRef()
            if veclyr_spatial_ref is None:
                raise rsgislib.RSGISPyException(
                    "Could not retrieve a projection object from the vector layer - "
                    "projection might not be be defined."
                )
            epsg_vec_spatial = veclyr_spatial_ref.GetAuthorityCode(None)
        else:
            epsg_vec_spatial = vec_def_epsg
            veclyr_spatial_ref = osr.SpatialReference()
            veclyr_spatial_ref.ImportFromEPSG(int(vec_def_epsg))

        if epsg_vec_spatial != epsg_img_spatial:
            imgDS = None
            vecDS = None
            raise rsgislib.RSGISPyException(
                "Inputted raster and vector layers have different "
                "projections: ('{0}' '{1}') ".format("Vector Layer Provided", input_img)
            )

        veclyrDefn = vec_lyr_obj.GetLayerDefn()

        outFieldAtts = [
            min_field,
            max_field,
            mean_field,
            stddev_field,
            sum_field,
            count_field,
            mode_field,
            median_field,
        ]
        for outattname in outFieldAtts:
            if outattname is not None:
                found = False
                for i in range(veclyrDefn.GetFieldCount()):
                    if (
                        veclyrDefn.GetFieldDefn(i).GetName().lower()
                        == outattname.lower()
                    ):
                        found = True
                        break
                if not found:
                    vec_lyr_obj.CreateField(
                        ogr.FieldDefn(outattname.lower(), ogr.OFTReal)
                    )

        fieldAttIdxs = dict()
        for outattname in outFieldAtts:
            if outattname is not None:
                fieldAttIdxs[outattname] = vec_lyr_obj.FindFieldIndex(
                    outattname.lower(), True
                )

        vec_mem_drv = ogr.GetDriverByName("Memory")
        img_mem_drv = gdal.GetDriverByName("MEM")

        # Iterate through features.
        openTransaction = False
        transactionStep = 20000
        nextTransaction = transactionStep
        nFeats = vec_lyr_obj.GetFeatureCount(True)
        pbar = tqdm.tqdm(total=nFeats)
        counter = 0
        vec_lyr_obj.ResetReading()
        feat = vec_lyr_obj.GetNextFeature()
        while feat is not None:
            if not openTransaction:
                vec_lyr_obj.StartTransaction()
                openTransaction = True

            if feat is not None:
                feat_geom = feat.geometry()
                if feat_geom is not None:
                    feat_bbox = feat_geom.GetEnvelope()
                    havepxls = True

                    x1Sp = float(feat_bbox[0] - imgGeoTrans[0])
                    x2Sp = float(feat_bbox[1] - imgGeoTrans[0])
                    y1Sp = float(feat_bbox[3] - imgGeoTrans[3])
                    y2Sp = float(feat_bbox[2] - imgGeoTrans[3])

                    if x1Sp == 0.0:
                        x1 = 0
                    else:
                        x1 = int(x1Sp / pixel_width) - 1

                    if x2Sp == 0.0:
                        x2 = 0
                    else:
                        x2 = int(x2Sp / pixel_width) + 1

                    if y1Sp == 0.0:
                        y1 = 0
                    else:
                        y1 = int(y1Sp / pixel_height) - 1

                    if y2Sp == 0.0:
                        y2 = 0
                    else:
                        y2 = int(y2Sp / pixel_height) + 1

                    if x1 < 0:
                        x1 = 0
                    elif x1 >= imgSizeX:
                        x1 = imgSizeX - 1

                    if x2 < 0:
                        x2 = 0
                    elif x2 >= imgSizeX:
                        x2 = imgSizeX - 1

                    if y1 < 0:
                        y1 = 0
                    elif y1 >= imgSizeY:
                        y1 = imgSizeY - 1

                    if y2 < 0:
                        y2 = 0
                    elif y2 >= imgSizeY:
                        y2 = imgSizeY - 1

                    xsize = x2 - x1
                    ysize = y2 - y1

                    if (xsize == 0) or (ysize == 0):
                        havepxls = False

                    # Define the image ROI for the feature
                    src_offset = (x1, y1, xsize, ysize)

                    if havepxls:
                        # Read the band array.
                        src_array = img_band_obj.ReadAsArray(*src_offset)
                    else:
                        src_array = None

                    if (src_array is not None) and havepxls:

                        # calculate new geotransform of the feature subset
                        subGeoTrans = (
                            (imgGeoTrans[0] + (src_offset[0] * imgGeoTrans[1])),
                            imgGeoTrans[1],
                            0.0,
                            (imgGeoTrans[3] + (src_offset[1] * imgGeoTrans[5])),
                            0.0,
                            imgGeoTrans[5],
                        )

                        # Create a temporary vector layer in memory
                        vec_mem_ds = vec_mem_drv.CreateDataSource("out")
                        vec_mem_lyr = vec_mem_ds.CreateLayer(
                            "poly", veclyr_spatial_ref, ogr.wkbPolygon
                        )
                        vec_mem_lyr.CreateFeature(feat.Clone())

                        # Rasterize the feature.
                        img_tmp_ds = img_mem_drv.Create(
                            "", src_offset[2], src_offset[3], 1, gdal.GDT_Byte
                        )
                        img_tmp_ds.SetGeoTransform(subGeoTrans)
                        img_tmp_ds.SetProjection(img_wkt_str)
                        gdal.RasterizeLayer(
                            img_tmp_ds, [1], vec_mem_lyr, burn_values=[1]
                        )
                        rv_array = img_tmp_ds.ReadAsArray()

                        # Mask the data vals array to feature.
                        mask_arr = numpy.ones_like(src_array, dtype=numpy.uint8)
                        if imgNoDataVal is not None:
                            mask_arr[src_array == imgNoDataVal] = 0
                            mask_arr[rv_array == 0] = 0
                            mask_arr[src_array < min_thres] = 0
                            mask_arr[src_array > max_thres] = 0
                        else:
                            mask_arr[rv_array == 0] = 0
                            mask_arr[src_array < min_thres] = 0
                            mask_arr[src_array > max_thres] = 0
                        mask_arr = mask_arr.flatten()
                        src_array_flat = src_array.flatten()
                        src_array_flat = src_array_flat[mask_arr == 1]

                        if src_array_flat.shape[0] > 0:
                            if min_field is not None:
                                min_val = float(src_array_flat.min())
                                feat.SetField(fieldAttIdxs[min_field], min_val)
                            if max_field is not None:
                                max_val = float(src_array_flat.max())
                                feat.SetField(fieldAttIdxs[max_field], max_val)
                            if mean_field is not None:
                                mean_val = float(src_array_flat.mean())
                                feat.SetField(fieldAttIdxs[mean_field], mean_val)
                            if stddev_field is not None:
                                stddev_val = float(src_array_flat.std())
                                feat.SetField(fieldAttIdxs[stddev_field], stddev_val)
                            if sum_field is not None:
                                sum_val = float(src_array_flat.sum())
                                feat.SetField(fieldAttIdxs[sum_field], sum_val)
                            if count_field is not None:
                                count_val = float(src_array_flat.shape[0])
                                feat.SetField(fieldAttIdxs[count_field], count_val)
                            if mode_field is not None:
                                mode_val, mode_count = scipy.stats.mstats.mode(
                                    src_array_flat
                                )
                                mode_val = float(mode_val)
                                feat.SetField(fieldAttIdxs[mode_field], mode_val)
                            if median_field is not None:
                                median_val = float(numpy.ma.median(src_array_flat))
                                feat.SetField(fieldAttIdxs[median_field], median_val)
                        else:
                            if min_field is not None:
                                feat.SetField(fieldAttIdxs[min_field], out_no_data_val)
                            if max_field is not None:
                                feat.SetField(fieldAttIdxs[max_field], out_no_data_val)
                            if mean_field is not None:
                                feat.SetField(fieldAttIdxs[mean_field], out_no_data_val)
                            if stddev_field is not None:
                                feat.SetField(
                                    fieldAttIdxs[stddev_field], out_no_data_val
                                )
                            if sum_field is not None:
                                feat.SetField(fieldAttIdxs[sum_field], out_no_data_val)
                            if count_field is not None:
                                feat.SetField(
                                    fieldAttIdxs[count_field], out_no_data_val
                                )
                            if mode_field is not None:
                                feat.SetField(fieldAttIdxs[mode_field], out_no_data_val)
                            if median_field is not None:
                                feat.SetField(
                                    fieldAttIdxs[median_field], out_no_data_val
                                )
                        # Write the updated feature to the vector layer.
                        vec_lyr_obj.SetFeature(feat)

                        vec_mem_ds = None
                        img_tmp_ds = None

            if (counter == nextTransaction) and openTransaction:
                vec_lyr_obj.CommitTransaction()
                openTransaction = False
                nextTransaction = nextTransaction + transactionStep

            feat = vec_lyr_obj.GetNextFeature()
            counter = counter + 1
            pbar.update(counter)
        if openTransaction:
            vec_lyr_obj.CommitTransaction()
            openTransaction = False
        pbar.close()

        imgDS = None
    except Exception as e:
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def calc_zonal_poly_pts_band_stats_file(
    vec_file: str,
    vec_lyr: str,
    input_img: str,
    img_band: int,
    out_field: str,
    vec_def_epsg: int = None,
):
    """
    A funtion which extracts zonal stats for a polygon using the polygon centroid.
    This is useful when you are intersecting a low resolution image with respect to
    the polygon resolution.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer within the input file which specifies the
                    features and where the output stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band will
                     be ignored.
    :param out_field: output field name within the vector layer.
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted
                         vector layer.

    """
    try:
        vecDS = gdal.OpenEx(vec_file, gdal.OF_VECTOR | gdal.OF_UPDATE)
        if vecDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not open layer '{}'".format(vec_lyr))

        calc_zonal_poly_pts_band_stats(
            vec_lyr_obj, input_img, img_band, out_field, vec_def_epsg
        )

        vecDS = None
    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def calc_zonal_poly_pts_band_stats(
    vec_lyr_obj: ogr.Layer,
    input_img: str,
    img_band: int,
    out_field: str,
    vec_def_epsg: int = None,
):
    """
    A funtion which extracts zonal stats for a polygon using the polygon centroid.
    This is useful when you are intesecting a low resolution image with respect to
    the polygon resolution.

    :param vec_lyr_obj: OGR vector layer object containing the geometries being
                        processed and to which the stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param out_field: output field name within the vector layer.
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is
                         the projection is not well defined within the inputted
                         vector layer.

    """
    try:
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("The inputted vector layer was None")

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise rsgislib.RSGISPyException(
                "Could not find image band '{}'".format(img_band)
            )
        imgGeoTrans = imgDS.GetGeoTransform()
        img_wkt_str = imgDS.GetProjection()
        img_spatial_ref = osr.SpatialReference()
        img_spatial_ref.ImportFromWkt(img_wkt_str)
        epsg_img_spatial = img_spatial_ref.GetAuthorityCode(None)

        pixel_width = imgGeoTrans[1]
        pixel_height = imgGeoTrans[5]

        imgSizeX = imgDS.RasterXSize
        imgSizeY = imgDS.RasterYSize

        if vec_def_epsg is None:
            veclyr_spatial_ref = vec_lyr_obj.GetSpatialRef()
            if veclyr_spatial_ref is None:
                raise rsgislib.RSGISPyException(
                    "Could not retrieve a projection object from the vector layer - "
                    "projection not might be be defined."
                )
            epsg_vec_spatial = veclyr_spatial_ref.GetAuthorityCode(None)
        else:
            epsg_vec_spatial = vec_def_epsg

        if epsg_vec_spatial != epsg_img_spatial:
            imgDS = None
            vecDS = None
            raise rsgislib.RSGISPyException(
                "Inputted raster and vector layers have different "
                "projections: ('{0}' '{1}') ".format("Vector Layer Provided", input_img)
            )

        veclyrDefn = vec_lyr_obj.GetLayerDefn()

        found = False
        for i in range(veclyrDefn.GetFieldCount()):
            if veclyrDefn.GetFieldDefn(i).GetName().lower() == out_field.lower():
                found = True
                break
        if not found:
            vec_lyr_obj.CreateField(ogr.FieldDefn(out_field.lower(), ogr.OFTReal))

        outfieldidx = vec_lyr_obj.FindFieldIndex(out_field.lower(), True)

        vec_mem_drv = ogr.GetDriverByName("Memory")
        img_mem_drv = gdal.GetDriverByName("MEM")

        # Iterate through features.
        openTransaction = False
        transactionStep = 20000
        nextTransaction = transactionStep
        nFeats = vec_lyr_obj.GetFeatureCount(True)
        pbar = tqdm.tqdm(total=nFeats)
        counter = 0
        vec_lyr_obj.ResetReading()
        feat = vec_lyr_obj.GetNextFeature()
        while feat is not None:
            if not openTransaction:
                vec_lyr_obj.StartTransaction()
                openTransaction = True

            if feat is not None:
                feat_geom = feat.geometry()
                if feat_geom is not None:
                    feat_bbox = feat_geom.GetEnvelope()
                    havepxls = True

                    x1Sp = float(feat_bbox[0] - imgGeoTrans[0])
                    x2Sp = float(feat_bbox[1] - imgGeoTrans[0])
                    y1Sp = float(feat_bbox[3] - imgGeoTrans[3])
                    y2Sp = float(feat_bbox[2] - imgGeoTrans[3])

                    if x1Sp == 0.0:
                        x1 = 0
                    else:
                        x1 = int(x1Sp / pixel_width) - 1

                    if x2Sp == 0.0:
                        x2 = 0
                    else:
                        x2 = int(x2Sp / pixel_width) + 1

                    if y1Sp == 0.0:
                        y1 = 0
                    else:
                        y1 = int(y1Sp / pixel_height) - 1

                    if y2Sp == 0.0:
                        y2 = 0
                    else:
                        y2 = int(y2Sp / pixel_height) + 1

                    if x1 < 0:
                        x1 = 0
                    elif x1 >= imgSizeX:
                        x1 = imgSizeX - 1

                    if x2 < 0:
                        x2 = 0
                    elif x2 >= imgSizeX:
                        x2 = imgSizeX - 1

                    if y1 < 0:
                        y1 = 0
                    elif y1 >= imgSizeY:
                        y1 = imgSizeY - 1

                    if y2 < 0:
                        y2 = 0
                    elif y2 >= imgSizeY:
                        y2 = imgSizeY - 1

                    xsize = x2 - x1
                    ysize = y2 - y1

                    if (xsize == 0) or (ysize == 0):
                        havepxls = False

                    # Define the image ROI for the feature
                    src_offset = (x1, y1, xsize, ysize)

                    if havepxls:
                        # Read the band array.
                        src_array = img_band_obj.ReadAsArray(*src_offset)
                    else:
                        src_array = None

                    if (src_array is not None) and havepxls:
                        subTLX = imgGeoTrans[0] + (src_offset[0] * imgGeoTrans[1])
                        subTLY = imgGeoTrans[3] + (src_offset[1] * imgGeoTrans[5])
                        resX = imgGeoTrans[1]
                        resY = imgGeoTrans[5]

                        ptx, pty, ptz = feat.GetGeometryRef().Centroid().GetPoint()

                        xOff = math.floor((ptx - subTLX) / resX)
                        yOff = math.floor((pty - subTLY) / resY)

                        if xOff < 0:
                            xOff = 0
                        if xOff >= xsize:
                            xOff = xsize - 1

                        if yOff < 0:
                            yOff = 0
                        if yOff >= ysize:
                            yOff = ysize - 1

                        out_val = float(src_array[yOff, xOff])
                        feat.SetField(outfieldidx, out_val)

                        vec_lyr_obj.SetFeature(feat)

                        vec_mem_ds = None
                        img_tmp_ds = None

            if (counter == nextTransaction) and openTransaction:
                vec_lyr_obj.CommitTransaction()
                openTransaction = False
                nextTransaction = nextTransaction + transactionStep

            feat = vec_lyr_obj.GetNextFeature()
            counter = counter + 1
            pbar.update(counter)

        if openTransaction:
            vec_lyr_obj.CommitTransaction()
            openTransaction = False
        pbar.close()

        imgDS = None
    except Exception as e:
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def calc_zonal_band_stats_test_poly_pts_file(
    vec_file: str,
    vec_lyr: str,
    input_img: str,
    img_band: int,
    min_thres: float,
    max_thres: float,
    out_no_data_val: float,
    percentile: float = None,
    percentile_field: str = None,
    min_field: str = None,
    max_field: str = None,
    mean_field: str = None,
    stddev_field: str = None,
    sum_field: str = None,
    count_field: str = None,
    mode_field: str = None,
    median_field: str = None,
    vec_def_epsg: int = None,
):
    """
    A function which calculates zonal statistics for a particular image band. If
    unsure then use this function. This function tests whether 1 or more pixels has
    been found within the polygon and if not then the centroid use used to find a
    value for the polygon.

    If you are unsure as to whether the pixels are small enough to be contained
    within all the polygons then use this function.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer within the input file which specifies the
                    features and where the output stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param min_thres: a lower threshold for values which will be included in
                      the stats calculation.
    :param max_thres: a upper threshold for values which will be included in
                      the stats calculation.
    :param out_no_data_val: output no data value if no valid pixels are within
                            the polygon.
    :param percentile: the percentile value to calculate (value between 0 and
                       100 inclusive).
    :param percentile_field: the name of the field for the percentile value (None or
                             not specified to be ignored).
    :param min_field: the name of the field for the min value (None or not specified
                      to be ignored).
    :param max_field: the name of the field for the max value (None or not specified
                      to be ignored).
    :param mean_field: the name of the field for the mean value (None or not specified
                       to be ignored).
    :param stddev_field: the name of the field for the standard deviation value (None
                         or not specified to be ignored).
    :param sum_field: the name of the field for the sum value (None or not specified to
                      be ignored).
    :param count_field: the name of the field for the count (of number of pixels) value
                       (None or not specified to be ignored).
    :param mode_field: the name of the field for the mode value (None or not specified
                       to be ignored).
    :param median_field: the name of the field for the median value (None or not
                         specified to be ignored).
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted
                         vector layer.

    """
    try:
        vecDS = gdal.OpenEx(vec_file, gdal.OF_VECTOR | gdal.OF_UPDATE)
        if vecDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not open layer '{}'".format(vec_lyr))

        calc_zonal_band_stats_test_poly_pts(
            vec_lyr_obj,
            input_img,
            img_band,
            min_thres,
            max_thres,
            out_no_data_val,
            percentile,
            percentile_field,
            min_field,
            max_field,
            mean_field,
            stddev_field,
            sum_field,
            count_field,
            mode_field,
            median_field,
            vec_def_epsg,
        )

        vecDS = None
    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def calc_zonal_band_stats_test_poly_pts(
    vec_lyr_obj: ogr.Layer,
    input_img: str,
    img_band: int,
    min_thres: float,
    max_thres: float,
    out_no_data_val: float,
    percentile: float = None,
    percentile_field: str = None,
    min_field: str = None,
    max_field: str = None,
    mean_field: str = None,
    stddev_field: str = None,
    sum_field: str = None,
    count_field: str = None,
    mode_field: str = None,
    median_field: str = None,
    vec_def_epsg: int = None,
):
    """
    A function which calculates zonal statistics for a particular image band. If
    unsure then use this function. This function tests whether 1 or more pixels has
    been found within the polygon and if not then the centroid use used to find a
    value for the polygon.

    If you are unsure as to whether the pixels are small enough to be contained
    within all the polygons then use this function.

    :param vec_lyr_obj: OGR vector layer object containing the geometries being
                        processed and to which the stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param min_thres: a lower threshold for values which will be included in
                      the stats calculation.
    :param max_thres: a upper threshold for values which will be included in
                      the stats calculation.
    :param out_no_data_val: output no data value if no valid pixels are within
                            the polygon.
    :param percentile: the percentile value to calculate (value between 0 and
                       100 inclusive).
    :param percentile_field: the name of the field for the percentile value (None or
                             not specified to be ignored).
    :param min_field: the name of the field for the min value (None or not specified
                      to be ignored).
    :param max_field: the name of the field for the max value (None or not specified
                      to be ignored).
    :param mean_field: the name of the field for the mean value (None or not specified
                       to be ignored).
    :param stddev_field: the name of the field for the standard deviation value (None
                         or not specified to be ignored).
    :param sum_field: the name of the field for the sum value (None or not specified to
                      be ignored).
    :param count_field: the name of the field for the count (of number of pixels) value
                       (None or not specified to be ignored).
    :param mode_field: the name of the field for the mode value (None or not specified
                       to be ignored).
    :param median_field: the name of the field for the median value (None or not
                         specified to be ignored).
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted
                         vector layer.

    """
    if mode_field is not None:
        import scipy.stats.mstats

    try:
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("The inputted vector layer was None")

        if (
            (min_field is None)
            and (max_field is None)
            and (mean_field is None)
            and (stddev_field is None)
            and (sum_field is None)
            and (count_field is None)
            and (mode_field is None)
            and (median_field is None)
            and (percentile_field is None)
        ):
            raise rsgislib.RSGISPyException(
                "At least one field needs to be specified for there is to an output."
            )

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise rsgislib.RSGISPyException(
                "Could not find image band '{}'".format(img_band)
            )
        imgGeoTrans = imgDS.GetGeoTransform()
        img_wkt_str = imgDS.GetProjection()
        img_spatial_ref = osr.SpatialReference()
        img_spatial_ref.ImportFromWkt(img_wkt_str)
        epsg_img_spatial = img_spatial_ref.GetAuthorityCode(None)

        pixel_width = imgGeoTrans[1]
        pixel_height = imgGeoTrans[5]

        imgSizeX = imgDS.RasterXSize
        imgSizeY = imgDS.RasterYSize

        imgNoDataVal = img_band_obj.GetNoDataValue()

        if vec_def_epsg is None:
            veclyr_spatial_ref = vec_lyr_obj.GetSpatialRef()
            if veclyr_spatial_ref is None:
                raise rsgislib.RSGISPyException(
                    "Could not retrieve a projection object from the vector "
                    "layer - projection might be be defined."
                )
            epsg_vec_spatial = veclyr_spatial_ref.GetAuthorityCode(None)
        else:
            epsg_vec_spatial = vec_def_epsg
            veclyr_spatial_ref = osr.SpatialReference()
            veclyr_spatial_ref.ImportFromEPSG(int(vec_def_epsg))

        if epsg_vec_spatial != epsg_img_spatial:
            imgDS = None
            vecDS = None
            raise rsgislib.RSGISPyException(
                "Inputted raster and vector layers have different "
                "projections: ('{0}' '{1}') ".format("Vector Layer Provided", input_img)
            )

        veclyrDefn = vec_lyr_obj.GetLayerDefn()

        outFieldAtts = [
            min_field,
            max_field,
            mean_field,
            stddev_field,
            sum_field,
            count_field,
            mode_field,
            median_field,
            percentile_field,
        ]
        for outattname in outFieldAtts:
            if outattname is not None:
                found = False
                for i in range(veclyrDefn.GetFieldCount()):
                    if (
                        veclyrDefn.GetFieldDefn(i).GetName().lower()
                        == outattname.lower()
                    ):
                        found = True
                        break
                if not found:
                    vec_lyr_obj.CreateField(
                        ogr.FieldDefn(outattname.lower(), ogr.OFTReal)
                    )

        fieldAttIdxs = dict()
        for outattname in outFieldAtts:
            if outattname is not None:
                fieldAttIdxs[outattname] = vec_lyr_obj.FindFieldIndex(
                    outattname.lower(), True
                )

        vec_mem_drv = ogr.GetDriverByName("Memory")
        img_mem_drv = gdal.GetDriverByName("MEM")

        # Iterate through features.
        openTransaction = False
        transactionStep = 20000
        nextTransaction = transactionStep
        nFeats = vec_lyr_obj.GetFeatureCount(True)
        pbar = tqdm.tqdm(total=nFeats)
        counter = 0
        vec_lyr_obj.ResetReading()
        feat = vec_lyr_obj.GetNextFeature()
        while feat is not None:
            if not openTransaction:
                vec_lyr_obj.StartTransaction()
                openTransaction = True

            if feat is not None:
                feat_geom = feat.geometry()
                if feat_geom is not None:
                    feat_bbox = feat_geom.GetEnvelope()

                    havepxls = True

                    x1Sp = float(feat_bbox[0] - imgGeoTrans[0])
                    x2Sp = float(feat_bbox[1] - imgGeoTrans[0])
                    y1Sp = float(feat_bbox[3] - imgGeoTrans[3])
                    y2Sp = float(feat_bbox[2] - imgGeoTrans[3])

                    if x1Sp == 0.0:
                        x1 = 0
                    else:
                        x1 = int(x1Sp / pixel_width) - 1

                    if x2Sp == 0.0:
                        x2 = 0
                    else:
                        x2 = int(x2Sp / pixel_width) + 1

                    if y1Sp == 0.0:
                        y1 = 0
                    else:
                        y1 = int(y1Sp / pixel_height) - 1

                    if y2Sp == 0.0:
                        y2 = 0
                    else:
                        y2 = int(y2Sp / pixel_height) + 1

                    if x1 < 0:
                        x1 = 0
                    elif x1 >= imgSizeX:
                        x1 = imgSizeX - 1

                    if x2 < 0:
                        x2 = 0
                    elif x2 >= imgSizeX:
                        x2 = imgSizeX - 1

                    if y1 < 0:
                        y1 = 0
                    elif y1 >= imgSizeY:
                        y1 = imgSizeY - 1

                    if y2 < 0:
                        y2 = 0
                    elif y2 >= imgSizeY:
                        y2 = imgSizeY - 1

                    xsize = x2 - x1
                    ysize = y2 - y1

                    if (xsize == 0) or (ysize == 0):
                        havepxls = False

                    # Define the image ROI for the feature
                    src_offset = (x1, y1, xsize, ysize)

                    if havepxls:
                        # Read the band array.
                        src_array = img_band_obj.ReadAsArray(*src_offset)
                    else:
                        src_array = None

                    if (src_array is not None) and havepxls:

                        # calculate new geotransform of the feature subset
                        subGeoTrans = (
                            (imgGeoTrans[0] + (src_offset[0] * imgGeoTrans[1])),
                            imgGeoTrans[1],
                            0.0,
                            (imgGeoTrans[3] + (src_offset[1] * imgGeoTrans[5])),
                            0.0,
                            imgGeoTrans[5],
                        )

                        # Create a temporary vector layer in memory
                        vec_mem_ds = vec_mem_drv.CreateDataSource("out")
                        vec_mem_lyr = vec_mem_ds.CreateLayer(
                            "poly", veclyr_spatial_ref, ogr.wkbPolygon
                        )
                        vec_mem_lyr.CreateFeature(feat.Clone())

                        # Rasterize the feature.
                        img_tmp_ds = img_mem_drv.Create(
                            "", src_offset[2], src_offset[3], 1, gdal.GDT_Byte
                        )
                        img_tmp_ds.SetGeoTransform(subGeoTrans)
                        img_tmp_ds.SetProjection(img_wkt_str)
                        gdal.RasterizeLayer(
                            img_tmp_ds, [1], vec_mem_lyr, burn_values=[1]
                        )
                        rv_array = img_tmp_ds.ReadAsArray()

                        # Mask the data vals array to feature
                        mask_arr = numpy.ones_like(src_array, dtype=numpy.uint8)
                        if imgNoDataVal is not None:
                            mask_arr[src_array == imgNoDataVal] = 0
                            mask_arr[rv_array == 0] = 0
                            mask_arr[src_array < min_thres] = 0
                            mask_arr[src_array > max_thres] = 0
                        else:
                            mask_arr[rv_array == 0] = 0
                            mask_arr[src_array < min_thres] = 0
                            mask_arr[src_array > max_thres] = 0
                        mask_arr = mask_arr.flatten()
                        src_array_flat = src_array.flatten()
                        src_array_flat = src_array_flat[mask_arr == 1]

                        if src_array_flat.shape[0] > 0:
                            if min_field is not None:
                                min_val = float(src_array_flat.min())
                                feat.SetField(fieldAttIdxs[min_field], min_val)
                            if max_field is not None:
                                max_val = float(src_array_flat.max())
                                feat.SetField(fieldAttIdxs[max_field], max_val)
                            if mean_field is not None:
                                mean_val = float(src_array_flat.mean())
                                feat.SetField(fieldAttIdxs[mean_field], mean_val)
                            if stddev_field is not None:
                                stddev_val = float(src_array_flat.std())
                                feat.SetField(fieldAttIdxs[stddev_field], stddev_val)
                            if sum_field is not None:
                                sum_val = float(src_array_flat.sum())
                                feat.SetField(fieldAttIdxs[sum_field], sum_val)
                            if count_field is not None:
                                count_val = float(src_array_flat.shape[0])
                                feat.SetField(fieldAttIdxs[count_field], count_val)
                            if mode_field is not None:
                                mode_val, mode_count = scipy.stats.mstats.mode(
                                    src_array_flat
                                )
                                mode_val = float(mode_val)
                                feat.SetField(fieldAttIdxs[mode_field], mode_val)
                            if median_field is not None:
                                median_val = float(numpy.ma.median(src_array_flat))
                                feat.SetField(fieldAttIdxs[median_field], median_val)
                            if percentile_field is not None:
                                perc_val = float(
                                    numpy.percentile(
                                        numpy.ma.compressed(src_array_flat),
                                        float(percentile),
                                    )
                                )
                                feat.SetField(fieldAttIdxs[percentile_field], perc_val)

                        else:
                            subTLX = imgGeoTrans[0] + (src_offset[0] * imgGeoTrans[1])
                            subTLY = imgGeoTrans[3] + (src_offset[1] * imgGeoTrans[5])
                            resX = imgGeoTrans[1]
                            resY = imgGeoTrans[5]

                            ptx, pty, ptz = feat.GetGeometryRef().Centroid().GetPoint()

                            xOff = math.floor((ptx - subTLX) / resX)
                            yOff = math.floor((pty - subTLY) / resY)

                            if xOff < 0:
                                xOff = 0
                            if xOff >= xsize:
                                xOff = xsize - 1

                            if yOff < 0:
                                yOff = 0
                            if yOff >= ysize:
                                yOff = ysize - 1

                            out_val = float(src_array[yOff, xOff])
                            invalid_val = False
                            if imgNoDataVal is not None:
                                if out_val == imgNoDataVal:
                                    invalid_val = True
                            if out_val < min_thres:
                                invalid_val = True
                            if out_val > max_thres:
                                invalid_val = True

                            if invalid_val:
                                if min_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[min_field], out_no_data_val
                                    )
                                if max_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[max_field], out_no_data_val
                                    )
                                if mean_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[mean_field], out_no_data_val
                                    )
                                if stddev_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[stddev_field], out_no_data_val
                                    )
                                if sum_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[sum_field], out_no_data_val
                                    )
                                if count_field is not None:
                                    feat.SetField(fieldAttIdxs[count_field], 0.0)
                                if mode_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[mode_field], out_no_data_val
                                    )
                                if median_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[median_field], out_no_data_val
                                    )
                                if percentile_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[percentile_field], out_no_data_val
                                    )
                            else:
                                if min_field is not None:
                                    feat.SetField(fieldAttIdxs[min_field], out_val)
                                if max_field is not None:
                                    feat.SetField(fieldAttIdxs[max_field], out_val)
                                if mean_field is not None:
                                    feat.SetField(fieldAttIdxs[mean_field], out_val)
                                if stddev_field is not None:
                                    feat.SetField(fieldAttIdxs[stddev_field], 0.0)
                                if sum_field is not None:
                                    feat.SetField(fieldAttIdxs[sum_field], out_val)
                                if count_field is not None:
                                    feat.SetField(fieldAttIdxs[count_field], 1.0)
                                if mode_field is not None:
                                    feat.SetField(fieldAttIdxs[mode_field], out_val)
                                if median_field is not None:
                                    feat.SetField(fieldAttIdxs[median_field], out_val)
                                if percentile_field is not None:
                                    feat.SetField(
                                        fieldAttIdxs[percentile_field], out_val
                                    )

                        # Write the updated feature to the vector layer.
                        vec_lyr_obj.SetFeature(feat)

                        vec_mem_ds = None
                        img_tmp_ds = None

            if (counter == nextTransaction) and openTransaction:
                vec_lyr_obj.CommitTransaction()
                openTransaction = False
                nextTransaction = nextTransaction + transactionStep

            feat = vec_lyr_obj.GetNextFeature()
            counter = counter + 1
            pbar.update(counter)
        if openTransaction:
            vec_lyr_obj.CommitTransaction()
            openTransaction = False
        pbar.close()

        imgDS = None
    except Exception as e:
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def ext_point_band_values_file(
    vec_file: str,
    vec_lyr: str,
    input_img: str,
    img_band: int,
    min_thres: float,
    max_thres: float,
    out_no_data_val: float,
    out_field: str,
    reproj_vec: bool = False,
    vec_def_epsg: int = None,
):
    """
    A function which extracts point values for an input vector file for a
    particular image band.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer within the input file which specifies the
                    features and where the output stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param min_thres: a lower threshold for values which will be included in the
                      stats calculation.
    :param max_thres: a upper threshold for values which will be included in the
                      stats calculation.
    :param out_no_data_val: output no data value if no valid pixels are within
                            the polygon.
    :param out_field: the name of the field in the vector layer where the pixel
                      values will be written.
    :param reproj_vec: boolean to specify whether the vector layer should be
                       reprojected on the fly during processing if the projections are
                       different. Default: False to ensure it is the users intention.
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted
                         vector layer.

    """
    try:
        vecDS = gdal.OpenEx(vec_file, gdal.OF_VECTOR | gdal.OF_UPDATE)
        if vecDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("Could not open layer '{}'".format(vec_lyr))

        ext_point_band_values(
            vec_lyr_obj,
            input_img,
            img_band,
            min_thres,
            max_thres,
            out_no_data_val,
            out_field,
            reproj_vec,
            vec_def_epsg,
        )

        vecDS = None
    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def ext_point_band_values(
    vec_lyr_obj: ogr.Layer,
    input_img: str,
    img_band: int,
    min_thres: float,
    max_thres: float,
    out_no_data_val: float,
    out_field: str,
    reproj_vec: bool = False,
    vec_def_epsg: int = None,
):
    """
    A function which extracts point values for an input vector file for a
    particular image band.

    :param vec_lyr_obj: OGR vector layer object containing the geometries being
                        processed and to which the stats will be written.
    :param input_img: the values image
    :param img_band: the index (starting at 1) of the image band for which the stats
                     will be calculated. If defined the no data value of the band
                     will be ignored.
    :param min_thres: a lower threshold for values which will be included in the
                      stats calculation.
    :param max_thres: a upper threshold for values which will be included in the
                      stats calculation.
    :param out_no_data_val: output no data value if no valid pixels are within
                            the polygon.
    :param out_field: the name of the field in the vector layer where the pixel
                      values will be written.
    :param reproj_vec: boolean to specify whether the vector layer should be
                       reprojected on the fly during processing if the projections are
                       different. Default: False to ensure it is the users intention.
    :param vec_def_epsg: an EPSG code can be specified for the vector layer is the
                         projection is not well defined within the inputted

    """
    import rsgislib.tools.geometrytools

    try:
        if vec_lyr_obj is None:
            raise rsgislib.RSGISPyException("The inputted vector layer was None")

        if out_field is None:
            raise rsgislib.RSGISPyException(
                "Output field specified as none, a name needs to be given."
            )
        elif out_field == "":
            raise rsgislib.RSGISPyException(
                "Output field specified as an empty string, a name needs to be given."
            )

        veclyrDefn = vec_lyr_obj.GetLayerDefn()
        lyr_geom_type = ogr.GeometryTypeToName(veclyrDefn.GetGeomType())
        if lyr_geom_type.lower() != "point":
            raise rsgislib.RSGISPyException("The layer geometry type must be point.")

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise rsgislib.RSGISPyException("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise rsgislib.RSGISPyException(
                "Could not find image band '{}'".format(img_band)
            )
        imgGeoTrans = imgDS.GetGeoTransform()
        img_wkt_str = imgDS.GetProjection()
        img_spatial_ref = osr.SpatialReference()
        img_spatial_ref.ImportFromWkt(img_wkt_str)
        img_spatial_ref.AutoIdentifyEPSG()
        epsg_img_spatial = img_spatial_ref.GetAuthorityCode(None)

        pixel_width = imgGeoTrans[1]
        pixel_height = imgGeoTrans[5]

        imgSizeX = imgDS.RasterXSize
        imgSizeY = imgDS.RasterYSize

        imgNoDataVal = img_band_obj.GetNoDataValue()
        out_no_data_val = float(out_no_data_val)

        if vec_def_epsg is None:
            veclyr_spatial_ref = vec_lyr_obj.GetSpatialRef()
            if veclyr_spatial_ref is None:
                raise rsgislib.RSGISPyException(
                    "Could not retrieve a projection object from the "
                    "vector layer - projection might be be defined."
                )
            epsg_vec_spatial = veclyr_spatial_ref.GetAuthorityCode(None)
        else:
            epsg_vec_spatial = vec_def_epsg
        veclyr_spatial_ref = osr.SpatialReference()
        veclyr_spatial_ref.ImportFromEPSG(int(epsg_vec_spatial))
        pt_reprj = False
        if int(epsg_vec_spatial) != int(epsg_img_spatial):
            if reproj_vec:
                pt_reprj = True
            else:
                raise rsgislib.RSGISPyException(
                    f"Input vector and image datasets are in different "
                    f"projections (EPSG:{epsg_vec_spatial} / EPSG:{epsg_img_spatial})."
                    f"You can select option to reproject."
                )

        found = False
        for i in range(veclyrDefn.GetFieldCount()):
            if veclyrDefn.GetFieldDefn(i).GetName().lower() == out_field.lower():
                found = True
                break
        if not found:
            vec_lyr_obj.CreateField(ogr.FieldDefn(out_field.lower(), ogr.OFTReal))

        out_field_idx = vec_lyr_obj.FindFieldIndex(out_field.lower(), True)

        # Iterate through features.
        openTransaction = False
        transactionStep = 20000
        nextTransaction = transactionStep
        nFeats = vec_lyr_obj.GetFeatureCount(True)
        pbar = tqdm.tqdm(total=nFeats)
        counter = 0
        vec_lyr_obj.ResetReading()
        feat = vec_lyr_obj.GetNextFeature()
        while feat is not None:
            if not openTransaction:
                vec_lyr_obj.StartTransaction()
                openTransaction = True

            if feat is not None:
                feat_geom = feat.geometry()
                if feat_geom is not None:
                    pt_in_img = True
                    x_pt = feat_geom.GetX()
                    y_pt = feat_geom.GetY()

                    if pt_reprj:
                        x_pt, y_pt = rsgislib.tools.geometrytools.reproj_point(
                            veclyr_spatial_ref, img_spatial_ref, x_pt, y_pt
                        )

                    x_pt_off = float(x_pt - imgGeoTrans[0])
                    y_pt_off = float(y_pt - imgGeoTrans[3])

                    if x_pt_off == 0.0:
                        x_pxl = 0
                    else:
                        x_pxl = int(math.floor(x_pt_off / pixel_width))# - 1

                    if y_pt_off == 0.0:
                        y_pxl = 0
                    else:
                        y_pxl = int(math.floor(y_pt_off / pixel_height))# - 1

                    if x_pxl < 0:
                        pt_in_img = False
                    elif x_pxl >= imgSizeX:
                        pt_in_img = False

                    if y_pxl < 0:
                        pt_in_img = False
                    elif y_pxl >= imgSizeY:
                        pt_in_img = False

                    if pt_in_img:
                        src_offset = (x_pxl, y_pxl, 1, 1)
                        src_array = img_band_obj.ReadAsArray(*src_offset)
                        pxl_val = src_array[0][0]
                        out_val = float(pxl_val)
                        if pxl_val == imgNoDataVal:
                            out_val = out_no_data_val
                        elif pxl_val < min_thres:
                            out_val = out_no_data_val
                        elif pxl_val > max_thres:
                            out_val = out_no_data_val

                        feat.SetField(out_field_idx, out_val)
                    else:
                        feat.SetField(out_field_idx, out_no_data_val)

                    vec_lyr_obj.SetFeature(feat)

            if (counter == nextTransaction) and openTransaction:
                vec_lyr_obj.CommitTransaction()
                openTransaction = False
                nextTransaction = nextTransaction + transactionStep

            feat = vec_lyr_obj.GetNextFeature()
            counter = counter + 1
            pbar.update(1)
        pbar.close()

        if openTransaction:
            vec_lyr_obj.CommitTransaction()
            openTransaction = False

        imgDS = None

    except Exception as e:
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def merge_extracted_hdf5_data(
    h5_files: List[str], out_h5_file: str, datatype: int = None
):
    """
    A function to merge a list of HDF files (e.g., from
    rsgislib.zonalstats.extractZoneImageBandValues2HDF)
    with the same number of variables (i.e., columns) into a single
    file. For example, if class training regions have been sourced
    from multiple images.

    :param h5_files: a list of input files.
    :param out_h5_file: the output file.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    .. code:: python

        inTrainSamples = ['MSS_CloudTrain1.h5', 'MSS_CloudTrain2.h5',
                          'MSS_CloudTrain3.h5']
        cloudTrainSamples = 'LandsatMSS_CloudTrainingSamples.h5'
        rsgislib.zonalstats.merge_extracted_hdf5_data(inTrainSamples, cloudTrainSamples)

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    first = True
    numVars = 0
    numVals = 0
    for h5File in h5_files:
        fH5 = h5py.File(h5File, "r")
        dataShp = fH5["DATA/DATA"].shape
        if first:
            numVars = dataShp[1]
            first = False
        elif numVars is not dataShp[1]:
            raise rsgislib.RSGISPyException(
                "The number of variables within the "
                "inputted HDF5 files was not the same."
            )
        numVals += dataShp[0]
        fH5.close()

    dataArr = numpy.zeros([numVals, numVars], dtype=float)

    rowInit = 0
    for h5File in h5_files:
        fH5 = h5py.File(h5File, "r")
        numRows = fH5["DATA/DATA"].shape[0]
        dataArr[rowInit : (rowInit + numRows)] = fH5["DATA/DATA"]
        rowInit += numRows
        fH5.close()

    chunk_len = 1000
    if numVals < chunk_len:
        chunk_len = numVals

    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    fH5Out = h5py.File(out_h5_file, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=dataArr,
        chunks=(chunk_len, numVars),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "Merged".encode()
    fH5Out.close()


def extract_chip_zone_image_band_values_to_hdf(
    input_image_info: List[rsgislib.imageutils.ImageBandInfo],
    image_mask: str,
    mask_value: int,
    chip_size: int,
    output_hdf: str,
    rotate_chips: List[float] = None,
    datatype: int = None,
):
    """
    A function which extracts a chip/window of image pixel values. The expectation is
    that this is used to train a classifier (see deep learning functions
    in classification) but it could be used to extract image 'chips' for other purposes.

    :param input_image_info: is a list of rsgislib.imageutils.ImageBandInfo objects
                             specifying the input images and bands
    :param image_mask: is a single band input image to specify the regions of interest
    :param mask_value: is the pixel value within the imageMask to specify the
                       region of interest
    :param chip_size: is the chip size .
    :param output_hdf: is the output HDF5 file. If it all ready exists then it
                       is overwritten.
    :param rotate_chips: specify whether you wish to have the image chips rotated
                         during extraction to increase the number of samples. Default
                         is None and will therefore be ignored. Otherwise, provide
                         a list of rotation angles in degrees (e.g., [30, 60, 90,
                         120, 180])
    :param datatype: is the data type used for the output HDF5 file (e.g.,
                     rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    # Import the RIOS image reader
    import h5py
    import tqdm
    from rios.imagereader import ImageReader

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    chip_size_odd = False
    if (chip_size % 2) != 0:
        chip_size_odd = True

    chipHSize = math.floor(chip_size / 2)

    rotate = False
    n_rotations = 0
    if rotate_chips is not None:
        import scipy.ndimage

        img_win_h_size = math.floor(
            math.sqrt((chipHSize * chipHSize) + (chipHSize * chipHSize))
        )
        img_win_size = img_win_h_size * 2
        rotate = True
        n_rotations = len(rotate_chips)
        minSub = img_win_h_size - chipHSize
        maxSub = img_win_size - minSub
    else:
        img_win_h_size = chipHSize
        img_win_size = chip_size

    ######################################################################
    # Count the number of features to extract so arrays can be initialised
    # at the correct size.
    ######################################################################
    nFeats = 0
    reader = ImageReader(image_mask, windowxsize=200, windowysize=200)
    for (info, block) in tqdm.tqdm(reader):
        nFeats = nFeats + numpy.sum(block[0] == mask_value)
    ######################################################################
    if rotate:
        nFeats = nFeats * (n_rotations + 1)
        print("There are {} pixel samples (inc. rotations) in the mask.".format(nFeats))
    else:
        print("There are {} pixel samples in the mask.".format(nFeats))

    ######################################################################
    # Initialise the numpy array for the feature data
    ######################################################################
    nBands = 0
    for inImgInfo in input_image_info:
        for band in inImgInfo.bands:
            nBands = nBands + 1
    featArr = numpy.zeros([nFeats, chip_size, chip_size, nBands], dtype=numpy.float32)
    sgl_feat_arr = numpy.zeros([nBands, chip_size, chip_size], dtype=numpy.float32)
    ######################################################################

    ######################################################################
    # Populate the feature arrays with the input data
    ######################################################################
    inImgs = list()
    inImgBands = list()

    inImgs.append(image_mask)
    inImgBands.append([1])

    for inImgInfo in input_image_info:
        inImgs.append(inImgInfo.file_name)
        inImgBands.append(inImgInfo.bands)
    nImgs = len(input_image_info)

    scnOverlap = img_win_h_size

    reader = ImageReader(
        inImgs,
        windowxsize=200,
        windowysize=200,
        overlap=scnOverlap,
        layerselection=inImgBands,
    )
    iFeat = 0
    for (info, block) in tqdm.tqdm(reader):
        classMskArr = block[0]
        blkShape = classMskArr.shape

        if rotate:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - chipHSize
                yMax = y + chipHSize
                if chip_size_odd:
                    yMax += 1
                yMinExt = y - scnOverlap
                yMaxExt = y + scnOverlap
                if chip_size_odd:
                    yMax += 1
                for x in xRange:
                    xMin = x - chipHSize
                    xMax = x + chipHSize
                    if chip_size_odd:
                        xMax += 1
                    xMinExt = x - scnOverlap
                    xMaxExt = x + scnOverlap
                    if chip_size_odd:
                        xMaxExt += 1
                    if classMskArr[0][y][x] == mask_value:
                        # Rotation 0...
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 1][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(
                                    sgl_feat_arr[iBand], imgBlk[iBand], casting="safe"
                                )
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting="safe")
                        iFeat = iFeat + 1
                        # Iterate through rotation angles
                        for rotate_angle in rotate_chips:
                            # Perform Rotate
                            sgl_feat_arr[...] = 0.0
                            for nImg in range(nImgs):
                                imgBlk = block[nImg + 1][
                                    ..., yMinExt:yMaxExt, xMinExt:xMaxExt
                                ]
                                # Perform Rotate
                                imgBlkRot = scipy.ndimage.rotate(
                                    imgBlk,
                                    rotate_angle,
                                    axes=[1, 2],
                                    reshape=False,
                                    output=numpy.float32,
                                    mode="nearest",
                                )
                                for iBand in range(imgBlk.shape[0]):
                                    numpy.copyto(
                                        sgl_feat_arr[iBand],
                                        imgBlkRot[iBand, minSub:maxSub, minSub:maxSub],
                                        casting="safe",
                                    )
                            numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting="safe")
                            iFeat = iFeat + 1
        else:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - scnOverlap
                yMax = y + scnOverlap
                if chip_size_odd:
                    yMax += 1
                for x in xRange:
                    xMin = x - scnOverlap
                    xMax = x + scnOverlap
                    if chip_size_odd:
                        xMax += 1
                    if classMskArr[0][y][x] == mask_value:
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 1][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(
                                    sgl_feat_arr[iBand], imgBlk[iBand], casting="safe"
                                )
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting="safe")
                        iFeat = iFeat + 1
    ######################################################################

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    fH5Out = h5py.File(output_hdf, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    # Chunk size needs to be less than number of data points
    if nFeats < 250:
        chunkFeatures = nFeats
    else:
        chunkFeatures = 250
    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)
    dataGrp.create_dataset(
        "DATA",
        data=featArr,
        chunks=(chunkFeatures, chip_size, chip_size, nBands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "IMAGE TILES".encode()
    fH5Out.close()
    ######################################################################


def split_sample_chip_hdf5_file(
    input_h5_file: str,
    sample_h5_file: str,
    remain_h5_file: str,
    sample_size: int,
    rnd_seed: int,
    datatype: int = None,
):
    """
    A function to split the HDF5 outputs from the
    rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf
    function into two sets by taking a random set with the defined sample
    size from the input file, saving the sample and the remainder to output HDF5 files.

    :param input_h5_file: The input HDF5 file to the split.
    :param sample_h5_file: The output HDF5 file with the sample outputted.
    :param remain_h5_file: The output HDF5 file with the remainder outputted.
    :param sample_size: An integer specifying the size of the sample to be taken.
    :param rnd_seed: An integer specifying the seed for the random number generator,
                     allowing the same 'random' sample to be taken.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, "r")
    n_rows = f["DATA/DATA"].shape[0]
    chip_size = f["DATA/DATA"].shape[1]
    n_bands = f["DATA/DATA"].shape[3]
    f.close()

    if sample_size > n_rows:
        raise rsgislib.RSGISPyException(
            "The requested sample is larger than the number samples in the input file."
        )

    rnd_obj = numpy.random.RandomState(rnd_seed)
    # Find sufficient unique sample indexes.
    smp_idxs = numpy.zeros(sample_size, dtype=int)
    while numpy.unique(smp_idxs).shape[0] != sample_size:
        tmp_idxs = rnd_obj.randint(0, n_rows, sample_size)
        tmp_uniq_idxs = numpy.unique(tmp_idxs)
        c_idx = 0
        if numpy.sum(smp_idxs) > 0:
            c_smp = smp_idxs[smp_idxs > 0]
            tmp_idxs = numpy.concatenate((c_smp, tmp_uniq_idxs))
            tmp_uniq_idxs = numpy.unique(tmp_idxs)

        max_idx = sample_size - 1
        if tmp_uniq_idxs.shape[0] < max_idx:
            max_idx = tmp_uniq_idxs.shape[0]
        smp_idxs[0:max_idx] = tmp_uniq_idxs[0:max_idx]
    smp_idxs = numpy.sort(smp_idxs)

    # Get the remaining indexes
    remain_idxs = numpy.arange(0, n_rows)
    remain_idxs = remain_idxs[
        numpy.isin(remain_idxs, smp_idxs, assume_unique=True, invert=True)
    ]

    # Read the input HDF5 file.
    f = h5py.File(input_h5_file, "r")
    in_samples = f["DATA/DATA"]
    out_samples = in_samples[smp_idxs]
    remain_samples = in_samples[remain_idxs]
    f.close()

    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    # Create an output HDF5 file and populate with sample data.
    if sample_size < 250:
        sample_chunks = sample_size
    else:
        sample_chunks = 250
    fSampleH5Out = h5py.File(sample_h5_file, "w")
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    dataSampleGrp.create_dataset(
        "DATA",
        data=out_samples,
        chunks=(sample_chunks, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = "IMAGE TILES".encode()
    fSampleH5Out.close()

    # Create an output HDF5 file and populate with remain data.
    if (n_rows - sample_size) < 250:
        sample_chunks = n_rows - sample_size
    else:
        sample_chunks = 250
    fSampleH5Out = h5py.File(remain_h5_file, "w")
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    dataSampleGrp.create_dataset(
        "DATA",
        data=remain_samples,
        chunks=(sample_chunks, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = "IMAGE TILES".encode()
    fSampleH5Out.close()


def merge_extracted_hdf5_chip_data(
    h5_files: List[str], out_h5_file: str, datatype: int = None
):
    """
    A function to merge a list of HDF files
    (e.g., from rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf)
    with the same number of variables (i.e., image bands) and chip size into
    a single file. For example, if class training regions have been sourced
    from multiple images.

    :param h5_files: a list of input files.
    :param out_h5_file: the output file.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    .. code:: python

        inTrainSamples = ['MSS_CloudTrain1.h5', 'MSS_CloudTrain2.h5',
                          'MSS_CloudTrain3.h5']
        cloudTrainSamples = 'LandsatMSS_CloudTrainingSamples.h5'
        rsgislib.zonalstats.merge_extracted_hdf5_chip_data(inTrainSamples,
                                                           cloudTrainSamples)

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    first = True
    n_feats = 0
    chip_size = 0
    n_bands = 0
    for h5File in h5_files:
        fH5 = h5py.File(h5File, "r")
        data_shp = fH5["DATA/DATA"].shape
        if first:
            n_bands = data_shp[3]
            chip_size = data_shp[1]
            first = False
        else:
            if n_bands != data_shp[3]:
                raise rsgislib.RSGISPyException(
                    "The number of bands (variables) within the inputted "
                    "HDF5 files was not the same."
                )
            if chip_size != data_shp[1]:
                raise rsgislib.RSGISPyException(
                    "The chip size within the inputted HDF5 files was not the same."
                )
        n_feats += data_shp[0]
        fH5.close()

    feat_arr = numpy.zeros(
        [n_feats, chip_size, chip_size, n_bands], dtype=numpy.float32
    )

    row_init = 0
    for h5File in h5_files:
        fH5 = h5py.File(h5File, "r")
        n_rows = fH5["DATA/DATA"].shape[0]
        feat_arr[row_init : (row_init + n_rows)] = fH5["DATA/DATA"]
        row_init += n_rows
        fH5.close()

    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    chunk_size = 250
    if n_feats < chunk_size:
        chunk_size = n_feats

    fH5Out = h5py.File(out_h5_file, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=feat_arr,
        chunks=(chunk_size, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "Merged".encode()
    fH5Out.close()


def extract_ref_chip_zone_image_band_values_to_hdf(
    input_image_info: List[rsgislib.imageutils.ImageBandInfo],
    ref_img: str,
    ref_img_band: int,
    image_mask: str,
    mask_value: int,
    chip_size: int,
    output_hdf: str,
    rotate_chips: List[float] = None,
    datatype: int = None,
):
    """
    A function which extracts a chip/window of image pixel values. The expectation is
    that this is used to train a classifier (see deep learning functions in
    classification) but it could be used to extract image 'chips' for other purposes.

    :param input_image_info: is a list of rsgislib.imageutils.ImageBandInfo objects
                             specifying the input images and bands
    :param ref_img: is an image file (same pixel size and projection as the other
                    input images) which is used as the class training
    :param ref_img_band: is the image band in the reference image to be used (only a
                         single reference band can be used).
    :param image_mask: is a single band input image to specify the regions of interest
    :param mask_value: is the pixel value within the imageMask to specify the
                       region of interest
    :param chip_size: is the chip size .
    :param output_hdf: is the output HDF5 file. If it all ready exists then it
                       is overwritten.
    :param rotate_chips: specify whether you wish to have the image chips rotated
                         during extraction to increase the number of samples. Default
                         is None and will therefore be ignored. Otherwise, provide a
                         list of rotation angles in degrees (e.g., [30, 60, 90,
                         120, 180])
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    # Import the RIOS image reader
    import h5py
    import tqdm
    from rios.imagereader import ImageReader

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    if (chip_size % 2) != 0:
        raise rsgislib.RSGISPyException("The chip size must be an even number.")

    chipHSize = math.floor(chip_size / 2)

    rotate = False
    n_rotations = 0
    if rotate_chips is not None:
        import scipy.ndimage

        img_win_h_size = math.floor(
            math.sqrt((chipHSize * chipHSize) + (chipHSize * chipHSize))
        )
        img_win_size = img_win_h_size * 2
        rotate = True
        n_rotations = len(rotate_chips)
        minSub = img_win_h_size - chipHSize
        maxSub = img_win_size - minSub
    else:
        img_win_h_size = chipHSize
        img_win_size = chip_size

    ######################################################################
    # Count the number of features to extract so arrays can be initialised
    # at the correct size.
    ######################################################################
    nFeats = 0
    reader = ImageReader(image_mask, windowxsize=200, windowysize=200)
    for (info, block) in tqdm.tqdm(reader):
        nFeats = nFeats + numpy.sum(block[0] == mask_value)
    ######################################################################
    if rotate:
        nFeats = nFeats * (n_rotations + 1)
        print("There are {} pixel samples (inc. rotations) in the mask.".format(nFeats))
    else:
        print("There are {} pixel samples in the mask.".format(nFeats))

    ######################################################################
    # Initialise the numpy array for the feature data
    ######################################################################
    nBands = 0
    for inImgInfo in input_image_info:
        for band in inImgInfo.bands:
            nBands = nBands + 1
    featArr = numpy.zeros(
        [nFeats, chip_size, chip_size, nBands], dtype=numpy.float32
    )  # [nFeats, chipSize, chipSize, nBands]
    sgl_feat_arr = numpy.zeros(
        [nBands, chip_size, chip_size], dtype=numpy.float32
    )  # [chipSize, chipSize, nBands]
    featRefArr = numpy.zeros([nFeats, chip_size, chip_size], dtype=numpy.uint16)
    ######################################################################

    ######################################################################
    # Populate the feature arrays with the input data
    ######################################################################
    inImgs = list()
    inImgBands = list()

    inImgs.append(image_mask)
    inImgBands.append([1])
    inImgs.append(ref_img)
    inImgBands.append([ref_img_band])

    for inImgInfo in input_image_info:
        inImgs.append(inImgInfo.file_name)
        inImgBands.append(inImgInfo.bands)
    nImgs = len(input_image_info)

    scnOverlap = img_win_h_size

    reader = ImageReader(
        inImgs,
        windowxsize=200,
        windowysize=200,
        overlap=scnOverlap,
        layerselection=inImgBands,
    )
    iFeat = 0
    for (info, block) in tqdm.tqdm(reader):
        classMskArr = block[0]
        blkShape = classMskArr.shape

        if rotate:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - chipHSize
                yMax = y + chipHSize
                yMinExt = y - scnOverlap
                yMaxExt = y + scnOverlap
                for x in xRange:
                    xMin = x - chipHSize
                    xMax = x + chipHSize
                    xMinExt = x - scnOverlap
                    xMaxExt = x + scnOverlap
                    if classMskArr[0][y][x] == mask_value:
                        # Rotation 0...
                        refImgBlk = block[1][0, yMin:yMax, xMin:xMax]
                        numpy.copyto(featRefArr[iFeat], refImgBlk, casting="safe")
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 2][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(
                                    sgl_feat_arr[iBand], imgBlk[iBand], casting="safe"
                                )
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting="safe")
                        iFeat = iFeat + 1
                        # Iterate through rotation angles
                        for rotate_angle in rotate_chips:
                            refImgBlk = block[1][0, yMinExt:yMaxExt, xMinExt:xMaxExt]
                            # Perform Rotate
                            refImgBlkRot = scipy.ndimage.rotate(
                                refImgBlk,
                                rotate_angle,
                                reshape=False,
                                output=numpy.uint16,
                                order=0,
                                mode="nearest",
                            )
                            numpy.copyto(
                                featRefArr[iFeat],
                                refImgBlkRot[minSub:maxSub, minSub:maxSub],
                                casting="safe",
                            )
                            sgl_feat_arr[...] = 0.0
                            for nImg in range(nImgs):
                                imgBlk = block[nImg + 2][
                                    ..., yMinExt:yMaxExt, xMinExt:xMaxExt
                                ]
                                # Perform Rotate
                                imgBlkRot = scipy.ndimage.rotate(
                                    imgBlk,
                                    rotate_angle,
                                    axes=[1, 2],
                                    reshape=False,
                                    output=numpy.float32,
                                    mode="nearest",
                                )
                                for iBand in range(imgBlk.shape[0]):
                                    numpy.copyto(
                                        sgl_feat_arr[iBand],
                                        imgBlkRot[iBand, minSub:maxSub, minSub:maxSub],
                                        casting="safe",
                                    )
                            numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting="safe")
                            iFeat = iFeat + 1
        else:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - scnOverlap
                yMax = y + scnOverlap
                for x in xRange:
                    xMin = x - scnOverlap
                    xMax = x + scnOverlap
                    if classMskArr[0][y][x] == mask_value:
                        refImgBlk = block[1][0, yMin:yMax, xMin:xMax]
                        numpy.copyto(featRefArr[iFeat], refImgBlk, casting="safe")
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 2][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(
                                    sgl_feat_arr[iBand], imgBlk[iBand], casting="safe"
                                )
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting="safe")
                        iFeat = iFeat + 1
    ######################################################################

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    fH5Out = h5py.File(output_hdf, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    # Chunk size needs to be less than number of data points
    if nFeats < 250:
        chunkFeatures = nFeats
    else:
        chunkFeatures = 250
    dataGrp.create_dataset(
        "DATA",
        data=featArr,
        chunks=(chunkFeatures, chip_size, chip_size, nBands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    dataGrp.create_dataset(
        "REF",
        data=featRefArr,
        chunks=(chunkFeatures, chip_size, chip_size),
        compression="gzip",
        shuffle=True,
        dtype="H",
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "IMAGE REF TILES".encode()
    fH5Out.close()
    ######################################################################


def split_sample_ref_chip_hdf5_file(
    input_h5_file: str,
    sample_h5_file: str,
    remain_h5_file: str,
    sample_size: int,
    rnd_seed: int,
    datatype: int = None,
):
    """
    A function to split the HDF5 outputs from the
    rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf
    function into two sets by taking a random set with the defined sample size
    from the input file, saving the sample and the remainder to output HDF5 files.

    :param input_h5_file: The input HDF5 file to the split.
    :param sample_h5_file: The output HDF5 file with the sample outputted.
    :param remain_h5_file: The output HDF5 file with the remainder outputted.
    :param sample_size: An integer specifying the size of the sample to be taken.
    :param rnd_seed: An integer specifying the seed for the random number generator,
                     allowing the same 'random' sample to be taken.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, "r")
    n_rows = f["DATA/REF"].shape[0]
    chip_size = f["DATA/REF"].shape[1]
    n_bands = f["DATA/DATA"].shape[3]
    f.close()

    if sample_size > n_rows:
        raise rsgislib.RSGISPyException(
            "The requested sample is larger than the number samples in the input file."
        )

    rnd_obj = numpy.random.RandomState(rnd_seed)
    # Find sufficient unique sample indexes.
    smp_idxs = numpy.zeros(sample_size, dtype=int)
    while numpy.unique(smp_idxs).shape[0] != sample_size:
        tmp_idxs = rnd_obj.randint(0, n_rows, sample_size)
        tmp_uniq_idxs = numpy.unique(tmp_idxs)
        c_idx = 0
        if numpy.sum(smp_idxs) > 0:
            c_smp = smp_idxs[smp_idxs > 0]
            tmp_idxs = numpy.concatenate((c_smp, tmp_uniq_idxs))
            tmp_uniq_idxs = numpy.unique(tmp_idxs)

        max_idx = sample_size - 1
        if tmp_uniq_idxs.shape[0] < max_idx:
            max_idx = tmp_uniq_idxs.shape[0]
        smp_idxs[0:max_idx] = tmp_uniq_idxs[0:max_idx]
    smp_idxs = numpy.sort(smp_idxs)

    # Get the remaining indexes
    remain_idxs = numpy.arange(0, n_rows)
    remain_idxs = remain_idxs[
        numpy.isin(remain_idxs, smp_idxs, assume_unique=True, invert=True)
    ]

    # Read the input HDF5 file.
    f = h5py.File(input_h5_file, "r")
    in_data_samples = f["DATA/DATA"]
    out_data_samples = in_data_samples[smp_idxs]
    remain_data_samples = in_data_samples[remain_idxs]
    in_ref_samples = f["DATA/REF"]
    out_ref_samples = in_ref_samples[smp_idxs]
    remain_ref_samples = in_ref_samples[remain_idxs]
    reman_smpl_size = remain_ref_samples.shape[0]
    f.close()

    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    # Create an output HDF5 file and populate with sample data.
    fSampleH5Out = h5py.File(sample_h5_file, "w")
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    smpl_chk_size = 250
    if sample_size < smpl_chk_size:
        smpl_chk_size = sample_size
    dataSampleGrp.create_dataset(
        "DATA",
        data=out_data_samples,
        chunks=(smpl_chk_size, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    dataSampleGrp.create_dataset(
        "REF",
        data=out_ref_samples,
        chunks=(smpl_chk_size, chip_size, chip_size),
        compression="gzip",
        shuffle=True,
        dtype="H",
    )
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = "IMAGE REF TILES".encode()
    fSampleH5Out.close()

    # Create an output HDF5 file and populate with remain data.
    fSampleH5Out = h5py.File(remain_h5_file, "w")
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    rman_chk_size = 250
    if reman_smpl_size < rman_chk_size:
        rman_chk_size = reman_smpl_size
    dataSampleGrp.create_dataset(
        "DATA",
        data=remain_data_samples,
        chunks=(rman_chk_size, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    dataSampleGrp.create_dataset(
        "REF",
        data=remain_ref_samples,
        chunks=(rman_chk_size, chip_size, chip_size),
        compression="gzip",
        shuffle=True,
        dtype="H",
    )
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = "IMAGE REF TILES".encode()
    fSampleH5Out.close()


def merge_extracted_hdf5_chip_ref_data(
    h5_files: List[str], out_h5_file: str, datatype: int = None
):
    """
    A function to merge a list of HDF files
    (e.g., from rsgislib.zonalstats.extract_ref_chip_zone_image_band_values_to_hdf)
    with the same number of variables (i.e., image bands) and chip size into
    a single file. For example, if class training regions have been sourced
    from multiple images.

    :param h5_files: a list of input files.
    :param out_h5_file: the output file.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    .. code:: python

        inTrainSamples = ['MSS_CloudTrain1.h5', 'MSS_CloudTrain2.h5',
                          'MSS_CloudTrain3.h5']
        cloudTrainSamples = 'LandsatMSS_CloudTrainingSamples.h5'
        rsgislib.zonalstats.merge_extracted_hdf5_chip_ref_data(inTrainSamples,
                                                               cloudTrainSamples)

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    first = True
    n_feats = 0
    chip_size = 0
    n_bands = 0
    for h5File in h5_files:
        fH5 = h5py.File(h5File, "r")
        data_shp = fH5["DATA/DATA"].shape
        if first:
            n_bands = data_shp[3]
            chip_size = data_shp[1]
            first = False
        else:
            if n_bands != data_shp[3]:
                raise rsgislib.RSGISPyException(
                    "The number of bands (variables) within the inputted "
                    "HDF5 files was not the same."
                )
            if chip_size != data_shp[1]:
                raise rsgislib.RSGISPyException(
                    "The chip size within the inputted HDF5 files was not the same."
                )
        n_feats += data_shp[0]
        fH5.close()

    feat_arr = numpy.zeros(
        [n_feats, chip_size, chip_size, n_bands], dtype=numpy.float32
    )
    feat_ref_arr = numpy.zeros([n_feats, chip_size, chip_size], dtype=numpy.uint16)

    row_init = 0
    for h5File in h5_files:
        fH5 = h5py.File(h5File, "r")
        n_rows = fH5["DATA/DATA"].shape[0]
        feat_arr[row_init : (row_init + n_rows)] = fH5["DATA/DATA"]
        feat_ref_arr[row_init : (row_init + n_rows)] = fH5["DATA/REF"]
        row_init += n_rows
        fH5.close()

    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    chunk_size = 250
    if n_feats < chunk_size:
        chunk_size = n_feats

    fH5Out = h5py.File(out_h5_file, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=feat_arr,
        chunks=(chunk_size, chip_size, chip_size, n_bands),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    dataGrp.create_dataset(
        "REF",
        data=feat_ref_arr,
        chunks=(chunk_size, chip_size, chip_size),
        compression="gzip",
        shuffle=True,
        dtype="H",
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "Merged".encode()
    fH5Out.close()


def msk_h5_smpls_to_finite_values(
    input_h5: str,
    output_h5: str,
    datatype: int = None,
    lower_limit: float = None,
    upper_limit: float = None,
):
    """
    A function to remove values from a HDF5 sample file which are not finite.
    Upper and lower values can also be specified.

    :param input_h5: Input HDF5 file.
    :param output_h5: Output HDF5 file.
    :param datatype: is the data type used for the output HDF5 file
                     (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.
    :param lower_limit: Optional lower value threshold (if None then not used).
    :param upper_limit: Optional upper value threshold (if None then not used).

    """
    import h5py

    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT
    h5_dtype = rsgislib.get_numpy_char_codes_datatype(datatype)

    fH5 = h5py.File(input_h5, "r")
    data_shp = fH5["DATA/DATA"].shape
    num_vars = data_shp[1]
    data = numpy.array(fH5["DATA/DATA"])
    data = data[numpy.isfinite(data).all(axis=1)]
    if lower_limit is not None:
        data = data[numpy.any(data > lower_limit, axis=1)]
    if upper_limit is not None:
        data = data[numpy.any(data < upper_limit, axis=1)]

    n_samples = data.shape[0]
    chunk_size = 1000
    if n_samples < 1000:
        chunk_size = n_samples

    fH5Out = h5py.File(output_h5, "w")
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset(
        "DATA",
        data=data,
        chunks=(chunk_size, num_vars),
        compression="gzip",
        shuffle=True,
        dtype=h5_dtype,
    )
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = "finite values".encode()
    fH5Out.close()


def get_var_from_hdf5_data(h5_files: List[str], var_idx: int = 0) -> numpy.array:
    """
    A function to get the data for a specific variable from a list of HDF files
     (e.g., from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf)

    :param h5_files: a list of input files.
    :param var_idx: the index for the variable of interest. Note array indexing
                    starts at 0. So if you want image band 2 then that will be
                    index 1 etc.
    :return: numpy array with the data or None is there is no data to return.

    """
    import h5py

    if var_idx < 0:
        raise rsgislib.RSGISPyException("The variable index must be greater than 0.")

    num_vals = 0
    for h5_file in h5_files:
        f_obj_h5 = h5py.File(h5_file, "r")
        data_shp = f_obj_h5["DATA/DATA"].shape
        if var_idx < data_shp[1]:
            num_vals += data_shp[0]
        f_obj_h5.close()

    if num_vals == 0:
        return None

    data_arr = numpy.zeros(num_vals, dtype=float)

    row_init = 0
    for h5_file in h5_files:
        f_obj_h5 = h5py.File(h5_file, "r")
        data_shp = f_obj_h5["DATA/DATA"].shape
        if var_idx < data_shp[1]:
            num_rows = f_obj_h5["DATA/DATA"].shape[0]
            data_arr[row_init : (row_init + num_rows)] = f_obj_h5["DATA/DATA"][
                ..., var_idx
            ]
            row_init += num_rows
        f_obj_h5.close()

    return data_arr


def get_hdf5_data(h5_files: List[str]) -> numpy.array:
    """
    A function to get the data from a list of HDF files
     (e.g., from rsgislib.zonalstats.extract_zone_img_band_values_to_hdf)

    :param h5_files: a list of input files.
    :return: numpy array with the data or None is there is no data to return.

    """
    import h5py

    num_vals = 0
    n_vars = 0
    first = True
    for h5_file in h5_files:
        f_obj_h5 = h5py.File(h5_file, "r")
        data_shp = f_obj_h5["DATA/DATA"].shape
        if first:
            n_vars = data_shp[1]
            first = False
        else:
            if n_vars != data_shp[1]:
                raise rsgislib.RSGISPyException(
                    "The input hdf5 files have a different number of input variables"
                )
        num_vals += data_shp[0]
        f_obj_h5.close()

    if num_vals == 0:
        return None

    data_arr = numpy.zeros([num_vals, n_vars], dtype=float)

    row_init = 0
    for h5_file in h5_files:
        f_obj_h5 = h5py.File(h5_file, "r")
        num_rows = f_obj_h5["DATA/DATA"].shape[0]
        data_arr[row_init : (row_init + num_rows)] = f_obj_h5["DATA/DATA"]
        row_init += num_rows
        f_obj_h5.close()

    return data_arr
