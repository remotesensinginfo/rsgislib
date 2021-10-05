#!/usr/bin/env python
"""
The zonal stats module provides functions to perform pixel-in-polygon or
point-in-pixel analysis.

For undertaking a pixel-in-polygon analysis you need to consider the size of the
polygons with respect to the size of the pixels being intersected.

Where the pixels are small with respect to the polygons so there is at least one pixel
within the polygon then the best function to use is:

* rsgislib.zonalstats.calcZonalBandStats

If the pixels are large with respect to the polygons then use the following function
which intersects the polygon centroid.

* rsgislib.zonalstats.calcZonalPolyPtsBandStats

If the pixel size in between and/or polygons are varied in size such that it is not
certain that all polygons will contain a pixel then the following function will first
attempt to intersect the polygon with the pixels and if there is not a pixel within
the polygon then the centriod is used.

* rsgislib.zonalstats.calcZonalBandStatsTestPolyPts

"""
# import the C++ extension into this level
from ._zonalstats import *

import math
import sys

import osgeo.gdal as gdal
import osgeo.ogr as ogr
import osgeo.osr as osr
import numpy
import tqdm

gdal.UseExceptions()

"""
Alternatively, the other functions are slower to execute but have more options with
respect to the method of intersection. The options for intersection are:

* METHOD_POLYCONTAINSPIXEL = 0           # Polygon completely contains pixel
* METHOD_POLYCONTAINSPIXELCENTER = 1     # Pixel center is within the polygon
* METHOD_POLYOVERLAPSPIXEL = 2           # Polygon overlaps the pixel
* METHOD_POLYOVERLAPSORCONTAINSPIXEL = 3 # Polygon overlaps or contains the pixel
* METHOD_PIXELCONTAINSPOLY = 4           # Pixel contains the polygon
* METHOD_PIXELCONTAINSPOLYCENTER = 5     # Polygon center is within pixel
* METHOD_ADAPTIVE = 6                    # The method is chosen based on relative areas
                                           of pixel and polygon.
* METHOD_ENVELOPE = 7                    # All pixels in polygon envelope chosen
* METHOD_PIXELAREAINPOLY = 8             # Percent of pixel area that is within
                                           the polygon
* METHOD_POLYAREAINPIXEL = 9             # Percent of polygon area that is within pixel


METHOD_POLYCONTAINSPIXEL = 0           # Polygon completely contains pixel
METHOD_POLYCONTAINSPIXELCENTER = 1     # Pixel center is within the polygon
METHOD_POLYOVERLAPSPIXEL = 2           # Polygon overlaps the pixel
METHOD_POLYOVERLAPSORCONTAINSPIXEL = 3 # Polygon overlaps or contains the pixel
METHOD_PIXELCONTAINSPOLY = 4           # Pixel contains the polygon
METHOD_PIXELCONTAINSPOLYCENTER = 5     # Polygon center is within pixel
METHOD_ADAPTIVE = 6                    # The method is chosen based on relative areas
                                         of pixel and polygon.
METHOD_ENVELOPE = 7                    # All pixels in polygon envelope chosen
METHOD_PIXELAREAINPOLY = 8             # Percent of pixel area that is within
                                         the polygon
METHOD_POLYAREAINPIXEL = 9             # Percent of polygon area that is within pixel
"""


def calcZonalBandStatsFile(
    vec_file,
    vec_lyr,
    input_img,
    img_band,
    min_thres,
    max_thres,
    out_no_data_val,
    min_field=None,
    max_field=None,
    mean_field=None,
    stddev_field=None,
    sum_field=None,
    count_field=None,
    mode_field=None,
    median_field=None,
    vec_def_epsg=None,
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
            raise Exception("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise Exception("Could not open layer '{}'".format(vec_lyr))

        calcZonalBandStats(
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


def calcZonalBandStats(
    vec_lyr_obj,
    input_img,
    img_band,
    min_thres,
    max_thres,
    out_no_data_val,
    min_field=None,
    max_field=None,
    mean_field=None,
    stddev_field=None,
    sum_field=None,
    count_field=None,
    mode_field=None,
    median_field=None,
    vec_def_epsg=None,
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
            raise Exception("The inputted vector layer was None")

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
            raise Exception(
                "At least one field needs to be specified for there is to an output."
            )

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise Exception("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise Exception("Could not find image band '{}'".format(img_band))
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
                raise Exception(
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
            raise Exception(
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


def calcZonalPolyPtsBandStatsFile(
    vec_file, vec_lyr, input_img, img_band, out_field, vec_def_epsg=None
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
            raise Exception("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise Exception("Could not open layer '{}'".format(vec_lyr))

        calcZonalPolyPtsBandStats(
            vec_lyr_obj, input_img, img_band, out_field, vec_def_epsg
        )

        vecDS = None
    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        print("Error Image File: {}".format(input_img), file=sys.stderr)
        raise e


def calcZonalPolyPtsBandStats(
    vec_lyr_obj, input_img, img_band, out_field, vec_def_epsg=None
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
            raise Exception("The inputted vector layer was None")

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise Exception("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise Exception("Could not find image band '{}'".format(img_band))
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
                raise Exception(
                    "Could not retrieve a projection object from the vector layer - "
                    "projection not might be be defined."
                )
            epsg_vec_spatial = veclyr_spatial_ref.GetAuthorityCode(None)
        else:
            epsg_vec_spatial = vec_def_epsg

        if epsg_vec_spatial != epsg_img_spatial:
            imgDS = None
            vecDS = None
            raise Exception(
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


def calcZonalBandStatsTestPolyPtsFile(
    vec_file,
    vec_lyr,
    input_img,
    img_band,
    min_thres,
    max_thres,
    out_no_data_val,
    percentile=None,
    percentile_field=None,
    min_field=None,
    max_field=None,
    mean_field=None,
    stddev_field=None,
    sum_field=None,
    count_field=None,
    mode_field=None,
    median_field=None,
    vec_def_epsg=None,
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
            raise Exception("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise Exception("Could not open layer '{}'".format(vec_lyr))

        calcZonalBandStatsTestPolyPts(
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


def calcZonalBandStatsTestPolyPts(
    vec_lyr_obj,
    input_img,
    img_band,
    min_thres,
    max_thres,
    out_no_data_val,
    percentile=None,
    percentile_field=None,
    min_field=None,
    max_field=None,
    mean_field=None,
    stddev_field=None,
    sum_field=None,
    count_field=None,
    mode_field=None,
    median_field=None,
    vec_def_epsg=None,
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
            raise Exception("The inputted vector layer was None")

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
            raise Exception(
                "At least one field needs to be specified for there is to an output."
            )

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise Exception("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise Exception("Could not find image band '{}'".format(img_band))
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
                raise Exception(
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
            raise Exception(
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
                                        fieldAttIdxs[percentile_field], perc_val
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
                                        fieldAttIdxs[percentile_field], perc_val
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


def extPointBandValuesFile(
    vec_file,
    vec_lyr,
    input_img,
    img_band,
    min_thres,
    max_thres,
    out_no_data_val,
    out_field,
    reproj_vec=False,
    vec_def_epsg=None,
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
            raise Exception("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise Exception("Could not open layer '{}'".format(vec_lyr))

        extPointBandValues(
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


def extPointBandValues(
    vec_lyr_obj,
    input_img,
    img_band,
    min_thres,
    max_thres,
    out_no_data_val,
    out_field,
    reproj_vec=False,
    vec_def_epsg=None,
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
            raise Exception("The inputted vector layer was None")

        if out_field is None:
            raise Exception("Output field specified as none, a name needs to be given.")
        elif out_field == "":
            raise Exception(
                "Output field specified as an empty string, a name needs to be given."
            )

        veclyrDefn = vec_lyr_obj.GetLayerDefn()
        lyr_geom_type = ogr.GeometryTypeToName(veclyrDefn.GetGeomType())
        if lyr_geom_type.lower() != "point":
            raise Exception("The layer geometry type must be point.")

        imgDS = gdal.OpenEx(input_img, gdal.GA_ReadOnly)
        if imgDS is None:
            raise Exception("Could not open '{}'".format(input_img))
        img_band_obj = imgDS.GetRasterBand(img_band)
        if img_band_obj is None:
            raise Exception("Could not find image band '{}'".format(img_band))
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
                raise Exception(
                    "Could not retrieve a projection object from the "
                    "vector layer - projection might be be defined."
                )
            epsg_vec_spatial = veclyr_spatial_ref.GetAuthorityCode(None)
        else:
            epsg_vec_spatial = vec_def_epsg
            veclyr_spatial_ref = osr.SpatialReference()
            veclyr_spatial_ref.ImportFromEPSG(int(vec_def_epsg))
        pt_reprj = False
        if epsg_vec_spatial != epsg_img_spatial:
            if reproj_vec:
                pt_reprj = True
            else:
                raise Exception(
                    "Input vector and image datasets are in different "
                    "projections (EPSG:{} / EPSG:{})."
                    "You can select option to reproject.".format(
                        epsg_vec_spatial, epsg_img_spatial
                    )
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
                        if veclyr_spatial_ref.EPSGTreatsAsLatLong():
                            x_pt, y_pt = rsgislib.tools.geometrytools.reprojPoint(
                                veclyr_spatial_ref, img_spatial_ref, y_pt, x_pt
                            )
                        else:
                            x_pt, y_pt = rsgislib.tools.geometrytools.reprojPoint(
                                veclyr_spatial_ref, img_spatial_ref, x_pt, y_pt
                            )

                    x_pt_off = float(x_pt - imgGeoTrans[0])
                    y_pt_off = float(y_pt - imgGeoTrans[3])

                    if x_pt_off == 0.0:
                        x_pxl = 0
                    else:
                        x_pxl = int(x_pt_off / pixel_width) - 1

                    if y_pt_off == 0.0:
                        y_pxl = 0
                    else:
                        y_pxl = int(y_pt_off / pixel_height) - 1

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
