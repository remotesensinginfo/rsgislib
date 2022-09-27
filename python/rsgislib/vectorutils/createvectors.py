#!/usr/bin/env python
"""
Tools for creating vector layers.
"""

import os
from typing import List

from osgeo import gdal
from osgeo import ogr
from osgeo import osr

import tqdm

import rsgislib

gdal.UseExceptions()


def polygonise_raster_to_vec_lyr(
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str,
    input_img: str,
    img_band: int = 1,
    mask_img: str = None,
    mask_band: int = 1,
    replace_file: bool = True,
    replace_lyr: bool = True,
    pxl_val_fieldname: str = "PXLVAL",
    use_8_conn: bool = False,
):
    """
    A utility to polygonise a raster to a OGR vector layer.

    :param out_vec_file: is a string specifying the output vector file path. If it
                         exists it will be deleted and overwritten.
    :param out_vec_lyr: is a string with the name of the vector layer.
    :param out_format: is a string with the driver
    :param input_img: is a string specifying the input image file to be polygonised
    :param img_band: is an int specifying the image band to be
                     polygonised. (default = 1)
    :param mask_img: is an optional string mask file specifying a no data
                     mask (default = None)
    :param mask_band: is an int specifying the image band to be used the
                      mask (default = 1)
    :param replace_file: is a boolean specifying whether the vector file should be
                         replaced (i.e., overwritten). Default=True.
    :param replace_lyr: is a boolean specifying whether the vector layer should be
                        replaced (i.e., overwritten). Default=True.
    :param pxl_val_fieldname: is a string to specify the name of the output column
                              representing the pixel value within the input image.
    :param use_8_conn: is a bool specifying whether 8 connectedness or 4 connectedness
                       should be used (4 is RSGISLib/GDAL default)

    """
    gdalImgDS = gdal.Open(input_img)
    imgBand = gdalImgDS.GetRasterBand(img_band)
    imgsrs = osr.SpatialReference()
    imgsrs.ImportFromWkt(gdalImgDS.GetProjectionRef())

    gdalImgMaskDS = None
    imgMaskBand = None
    if mask_img is not None:
        gdalImgMaskDS = gdal.Open(mask_img)
        imgMaskBand = gdalImgMaskDS.GetRasterBand(mask_band)

    if os.path.exists(out_vec_file) and (not replace_file):
        vecDS = gdal.OpenEx(out_vec_file, gdal.GA_Update)
    else:
        outdriver = ogr.GetDriverByName(out_format)
        if os.path.exists(out_vec_file):
            outdriver.DeleteDataSource(out_format)
        vecDS = outdriver.CreateDataSource(out_vec_file)

    if vecDS is None:
        raise rsgislib.RSGISPyException(
            "Could not open or create '{}'".format(out_vec_file)
        )

    lcl_options = []
    if replace_lyr:
        lcl_options = ["OVERWRITE=YES"]

    out_lyr_obj = vecDS.CreateLayer(out_vec_lyr, srs=imgsrs, options=lcl_options)
    if out_lyr_obj is None:
        raise rsgislib.RSGISPyException(
            "Could not create layer: {}".format(out_vec_lyr)
        )

    newField = ogr.FieldDefn(pxl_val_fieldname, ogr.OFTInteger)
    out_lyr_obj.CreateField(newField)
    dstFieldIdx = out_lyr_obj.GetLayerDefn().GetFieldIndex(pxl_val_fieldname)

    try:
        import tqdm

        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    options = list()
    if use_8_conn:
        options.append("8CONNECTED=8")

    print("Polygonising...")
    gdal.Polygonize(
        imgBand, imgMaskBand, out_lyr_obj, dstFieldIdx, options, callback=callback
    )
    print("Completed")
    out_lyr_obj.SyncToDisk()
    vecDS = None
    gdalImgDS = None
    if mask_img is not None:
        gdalImgMaskDS = None


def vectorise_pxls_to_pts(
    input_img: str,
    img_band: int,
    img_msk_val: int,
    out_vec_file: str,
    out_vec_lyr: str = None,
    out_format: str = "GPKG",
    out_epsg_code: int = None,
    del_exist_vec: bool = False,
):
    """
    Function which creates a new output vector file for the pixels within the input
    image file with the value specified. Pixel locations will be the centroid of
    the the pixel

    :param input_img: the input image
    :param img_band: the band within the image to use
    :param img_msk_val: the image value selecting the pixels to be converted to points
    :param out_vec_file: Output vector file
    :param out_vec_lyr: output vector layer name.
    :param out_format: output file format (default GPKG).
    :param out_epsg_code: optionally provide an EPSG code for the output layer. If None
                          then taken from input image.
    :param del_exist_vec: remove output file if it exists.

    """
    from rios import applier
    import geopandas
    import rsgislib.imageutils
    import rsgislib.vectorutils

    try:
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress

        progress_bar = cuiprogress.GDALProgressBar()

    if os.path.exists(out_vec_file):
        if del_exist_vec:
            rsgislib.vectorutils.delete_vector_file(out_vec_file)
        else:
            raise rsgislib.RSGISPyException(
                "The output vector file ({}) already exists, "
                "remove it and re-run.".format(out_vec_file)
            )

    if out_epsg_code is None:
        out_epsg_code = rsgislib.imageutils.get_epsg_proj_from_img(input_img)

    if out_epsg_code is None:
        raise rsgislib.RSGISPyException(
            "The output ESPG code is None - tried to read from input image and "
            "returned None. Suggest providing the EPSG code to the function."
        )

    pt_x_lst = list()
    pt_y_lst = list()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.img_band = img_band
    otherargs.img_msk_val = img_msk_val
    otherargs.pt_x_lst = pt_x_lst
    otherargs.pt_y_lst = pt_y_lst
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _getXYPxlLocs(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        x_block, y_block = info.getBlockCoordArrays()
        msk_data = inputs.image[0, ...].flatten()
        x_block = x_block.flatten()
        y_block = y_block.flatten()

        x_block = x_block[msk_data == otherargs.img_msk_val]
        y_block = y_block[msk_data == otherargs.img_msk_val]

        otherargs.pt_x_lst.extend(x_block.tolist())
        otherargs.pt_y_lst.extend(y_block.tolist())

    applier.apply(_getXYPxlLocs, infiles, outfiles, otherargs, controls=aControls)

    if len(pt_x_lst) > 0:

        gdf = geopandas.GeoDataFrame(
            geometry=geopandas.points_from_xy(pt_x_lst, pt_y_lst),
            crs="EPSG:{}".format(out_epsg_code),
        )

        if out_format == "GPKG":
            if out_vec_lyr is None:
                raise rsgislib.RSGISPyException(
                    "If output format is GPKG then an output layer is required."
                )
            gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            gdf.to_file(out_vec_file, driver=out_format)


def extract_image_footprint(
    input_img: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    tmp_dir: str = "tmp",
    reproj_to: str = None,
    no_data_val=None,
):
    """
    A function to extract an image footprint as a vector.

    :param input_img: the input image file for which the footprint will be extracted.
    :param out_vec_file: output vector file path and name.
    :param out_vec_lyr: output vector layer name.
    :param tmp_dir: temp directory which will be used during processing. It will be
                    created and deleted once processing complete.
    :param reproj_to: optional if not None then an ogr2ogr command will be
                      run and the input here is what is to go into the ogr2ogr
                      command after -t_srs. E.g., -t_srs epsg:4326
    """
    gdal.UseExceptions()
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.imageutils
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    if not rsgislib.tools.filetools.does_path_exists_or_creatable(out_vec_file):
        raise rsgislib.RSGISPyException(
            f"Output file path is not creatable: {out_vec_file}"
        )

    uid_str = rsgislib.tools.utils.uid_generator()

    created_tmp = False
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
        created_tmp = True

    in_img_base = os.path.splitext(os.path.basename(input_img))[0]

    valid_out_img = os.path.join(tmp_dir, in_img_base + "_" + uid_str + "_validimg.kea")
    if no_data_val is None:
        no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    if no_data_val is None:
        raise rsgislib.RSGISPyException("A no data value has not been specified.")
    rsgislib.imageutils.gen_valid_mask(
        input_img, valid_out_img, gdalformat="KEA", no_data_val=no_data_val
    )

    out_vec_tmp_file = out_vec_file
    if not (reproj_to is None):
        out_vec_tmp_file = os.path.join(
            tmp_dir, in_img_base + "_" + uid_str + "_initVecOut.gpkg"
        )

    polygonise_raster_to_vec_lyr(
        out_vec_tmp_file, out_vec_lyr, out_format, valid_out_img, 1, valid_out_img, 1
    )

    ds = gdal.OpenEx(out_vec_tmp_file, gdal.OF_READONLY)
    if ds is None:
        raise rsgislib.RSGISPyException("Could not open '" + out_vec_tmp_file + "'")

    lyr = ds.GetLayerByName(out_vec_lyr)
    if lyr is None:
        raise rsgislib.RSGISPyException("Could not find layer '" + out_vec_lyr + "'")
    num_feats = lyr.GetFeatureCount()
    lyr = None
    ds = None

    file_name = []
    for i in range(num_feats):
        file_name.append(os.path.basename(input_img))
    rsgislib.vectorattrs.write_vec_column(
        out_vec_tmp_file, out_vec_lyr, "FileName", ogr.OFTString, file_name
    )

    if not (reproj_to is None):
        if os.path.exists(out_vec_file):
            rsgislib.vectorutils.delete_vector_file(out_vec_file)

        cmd = [
            "ogr2ogr",
            "-f",
            out_format,
            "-t_srs",
            reproj_to,
            out_vec_file,
            out_vec_tmp_file,
        ]
        print(cmd)
        try:
            import subprocess

            subprocess.run(cmd, check=True)
        except OSError as e:
            raise rsgislib.RSGISPyException(
                "Could not re-projection vector file: {}".format(cmd)
            )

    if created_tmp:
        import shutil

        shutil.rmtree(tmp_dir)
    else:
        if not (reproj_to is None):
            driver = ogr.GetDriverByName("ESRI Shapefile")
            driver.DeleteDataSource(out_vec_tmp_file)


def create_poly_vec_for_lst_bboxs(
    csv_file,
    out_vec_file,
    out_vec_lyr,
    out_format,
    epsg_code,
    min_x_col=0,
    max_x_col=1,
    min_y_col=2,
    max_y_col=3,
    ignore_rows=0,
    del_exist_vec=False,
):
    """
    This function takes a CSV file of bounding boxes (1 per line) and creates a
    polygon vector layer.

    :param csv_file: input CSV file.
    :param out_vec_file: output vector file
    :param out_vec_file: output vector layer
    :param out_format: output vector file format (e.g., GPKG)
    :param epsg_code: EPSG code specifying the projection of the data
                     (4326 is WSG84 Lat/Long).
    :param min_x_col: The index (starting at 0) for the column within the CSV file
                    for the minimum X coordinate.
    :param max_x_col: The index (starting at 0) for the column within the CSV file
                    for the maximum X coordinate.
    :param min_y_col: The index (starting at 0) for the column within the CSV file
                    for the minimum Y coordinate.
    :param max_y_col: The index (starting at 0) for the column within the CSV file
                    for the maximum Y coordinate.
    :param ignore_rows: The number of rows to ignore from the start of the CSV
                        file (i.e., column headings)
    :param del_exist_vec: If the output file already exists delete it before proceeding.
    """
    gdal.UseExceptions()
    try:
        if os.path.exists(out_vec_file):
            if del_exist_vec:
                driver = ogr.GetDriverByName(out_format)
                driver.DeleteDataSource(out_vec_file)
            else:
                raise rsgislib.RSGISPyException("Output file already exists")
        # Create the output Driver
        outDriver = ogr.GetDriverByName(out_format)
        # create the spatial reference, WGS84
        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsg_code))
        # Create the output Shapefile
        outDataSource = outDriver.CreateDataSource(out_vec_file)
        outLayer = outDataSource.CreateLayer(out_vec_lyr, srs, geom_type=ogr.wkbPolygon)
        # Get the output Layer's Feature Definition
        featureDefn = outLayer.GetLayerDefn()

        dataFile = open(csv_file, "r")
        rowCount = 0
        for line in dataFile:
            if rowCount >= ignore_rows:
                line = line.strip()
                if line != "":
                    comps = line.split(",")
                    # Get values from CSV file.
                    minX = float(comps[min_x_col])
                    maxX = float(comps[max_x_col])
                    minY = float(comps[min_y_col])
                    maxY = float(comps[max_y_col])
                    # Create Linear Ring
                    ring = ogr.Geometry(ogr.wkbLinearRing)
                    ring.AddPoint(minX, maxY)
                    ring.AddPoint(maxX, maxY)
                    ring.AddPoint(maxX, minY)
                    ring.AddPoint(minX, minY)
                    ring.AddPoint(minX, maxY)
                    # Create polygon.
                    poly = ogr.Geometry(ogr.wkbPolygon)
                    poly.AddGeometry(ring)
                    # Add to output shapefile.
                    outFeature = ogr.Feature(featureDefn)
                    outFeature.SetGeometry(poly)
                    outLayer.CreateFeature(outFeature)
                    outFeature = None
            rowCount = rowCount + 1
        dataFile.close()
        outDataSource = None
    except Exception as e:
        raise e


def define_grid(
    bbox,
    x_size,
    y_size,
    in_epsg_code,
    out_vec,
    out_vec_lyr,
    out_format="GPKG",
    out_epsg_code=None,
    utm_grid=False,
    utm_hemi=False,
):
    """
    Define a grid of bounding boxes for a specified bounding box. The output grid can
    be in a different projection to the inputted bounding box. Where a UTM grid is
    required and there are multiple UTM zones then the layer name will be appended
    with utmXX[n|s]. Note. this only works with formats such as GPKG which support
    multiple layers. A shapefile which only supports 1 layer will not work.

    :param bbox: a bounding box (xMin, xMax, yMin, yMax)
    :param x_size: Output grid size in X axis. If out_epsg_code or utm_grid defined
                   then the grid size needs to be in the output unit.
    :param y_size: Output grid size in Y axis. If out_epsg_code or utm_grid defined
                   then the grid size needs to be in the output unit.
    :param in_epsg_code: EPSG code for the projection of the bbox
    :param out_vec: output vector file.
    :param out_vec_lyr: output vector layer name.
    :param out_format: output vector file format (see OGR codes). Default is GPKG.
    :param out_epsg_code: if provided the output grid is reprojected to the projection
                          defined by this EPSG code. (note. the grid size needs to
                          the in the unit of this projection). Default is None.
    :param utm_grid: provide the output grid in UTM projection where grid might go
                     across multiple UTM zones. Default is False. grid size unit
                     should be metres.
    :param utm_hemi: if outputting a UTM projected grid then decided whether to use
                     hemispheres or otherwise. If False then everything will be
                     projected northern hemisphere (e.g., as with landsat or
                     sentinel-2). Default is False.
    """
    import rsgislib.tools.utm
    import rsgislib.tools.geometrytools

    if (out_epsg_code is not None) and utm_grid:
        raise rsgislib.RSGISPyException(
            "Cannot specify both new output projection and UTM grid."
        )
    elif utm_grid:
        wgs84_bbox = bbox
        if in_epsg_code != 4326:
            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(in_epsg_code)
            out_proj_obj = osr.SpatialReference()
            out_proj_obj.ImportFromEPSG(4326)
            wgs84_bbox = rsgislib.tools.geometrytools.reproj_bbox(
                bbox, in_proj_obj, out_proj_obj
            )

        multi_zones = False
        if (wgs84_bbox[0] < -180) and (wgs84_bbox[1] < -180):
            wgs84_bbox = [
                360 + wgs84_bbox[0],
                360 + wgs84_bbox[1],
                wgs84_bbox[2],
                wgs84_bbox[3],
            ]
        elif (wgs84_bbox[0] > 180) and (wgs84_bbox[1] > 180):
            wgs84_bbox = [
                360 - wgs84_bbox[0],
                360 - wgs84_bbox[1],
                wgs84_bbox[2],
                wgs84_bbox[3],
            ]
        elif (wgs84_bbox[0] < -180) or (wgs84_bbox[0] > 180):
            multi_zones = True

        if not multi_zones:
            utm_tl = rsgislib.tools.utm.from_latlon(wgs84_bbox[3], wgs84_bbox[0])
            utm_tr = rsgislib.tools.utm.from_latlon(wgs84_bbox[3], wgs84_bbox[1])
            utm_br = rsgislib.tools.utm.from_latlon(wgs84_bbox[2], wgs84_bbox[1])
            utm_bl = rsgislib.tools.utm.from_latlon(wgs84_bbox[2], wgs84_bbox[0])

            utm_top_hemi = "N"
            if utm_hemi and (wgs84_bbox[3] < 0):
                utm_top_hemi = "S"

        if (not multi_zones) and (utm_tl[2] == utm_tr[2] == utm_br[2] == utm_bl[2]):
            utm_zone = utm_tl[2]

            utm_proj_epsg = rsgislib.tools.utm.epsg_for_utm(utm_zone, utm_top_hemi)

            define_grid(
                bbox,
                x_size,
                y_size,
                in_epsg_code,
                out_vec,
                out_vec_lyr,
                out_format=out_format,
                out_epsg_code=utm_proj_epsg,
                utm_grid=False,
                utm_hemi=False,
            )
        else:
            multi_zones = True

            utm_zone_bboxs = []
            if wgs84_bbox[0] < -180:
                wgs84_bbox_W = [-180, wgs84_bbox[1], wgs84_bbox[2], wgs84_bbox[3]]
                wgs84_bbox_E = [360 + wgs84_bbox[1], 180, wgs84_bbox[2], wgs84_bbox[3]]

                utm_zone_bboxs = (
                    utm_zone_bboxs
                    + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_W)
                )
                utm_zone_bboxs = (
                    utm_zone_bboxs
                    + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_E)
                )

            elif wgs84_bbox[0] > 180:
                wgs84_bbox_W = [wgs84_bbox[0], 180, wgs84_bbox[2], wgs84_bbox[3]]
                wgs84_bbox_E = [-180, 360 - wgs84_bbox[1], wgs84_bbox[2], wgs84_bbox[3]]

                utm_zone_bboxs = (
                    utm_zone_bboxs
                    + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_W)
                )
                utm_zone_bboxs = (
                    utm_zone_bboxs
                    + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_E)
                )

            else:
                utm_zone_bboxs = rsgislib.tools.utm.split_wgs84_bbox_utm_zones(
                    wgs84_bbox
                )

            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(4326)

            first = True
            for zone_roi in utm_zone_bboxs:
                utm_top_hemi = "N"
                if utm_hemi:
                    if zone_roi[1][3] < 0:
                        utm_top_hemi = "S"

                utm_proj_epsg = int(
                    rsgislib.tools.utm.epsg_for_utm(zone_roi[0], utm_top_hemi)
                )

                out_proj_obj = osr.SpatialReference()
                out_proj_obj.ImportFromEPSG(utm_proj_epsg)

                utm_bbox = rsgislib.tools.geometrytools.reproj_bbox(
                    zone_roi[1], in_proj_obj, out_proj_obj
                )
                bboxs = rsgislib.tools.geometrytools.get_bbox_grid(
                    utm_bbox, x_size, y_size
                )

                utm_out_vec_lyr = out_vec_lyr + "_utm{0}{1}".format(
                    zone_roi[0], utm_top_hemi.lower()
                )
                create_poly_vec_bboxs(
                    out_vec,
                    utm_out_vec_lyr,
                    out_format,
                    utm_proj_epsg,
                    bboxs,
                    overwrite=first,
                )
                first = False
    else:
        if out_epsg_code is not None:
            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(in_epsg_code)
            out_proj_obj = osr.SpatialReference()
            out_proj_obj.ImportFromEPSG(out_epsg_code)
            proj_bbox = rsgislib.tools.geometrytools.reproj_bbox(
                bbox, in_proj_obj, out_proj_obj
            )
        else:
            proj_bbox = bbox

        bboxs = rsgislib.tools.geometrytools.get_bbox_grid(proj_bbox, x_size, y_size)

        if out_epsg_code is None:
            create_poly_vec_bboxs(out_vec, out_vec_lyr, out_format, in_epsg_code, bboxs)
        else:
            create_poly_vec_bboxs(
                out_vec, out_vec_lyr, out_format, out_epsg_code, bboxs
            )


def create_poly_vec_bboxs(
    vec_file,
    vec_lyr,
    out_format,
    epsg_code,
    bboxs,
    atts=None,
    att_types=None,
    overwrite=True,
):
    """
    This function creates a set of polygons for a set of bounding boxes.
    When creating an attribute the available data types are ogr.OFTString,
    ogr.OFTInteger, ogr.OFTReal

    :param vec_file: output vector file/path
    :param vec_lyr: output vector layer
    :param out_format: the output vector layer type.
    :param epsg_code: EPSG code specifying the projection of the data (e.g.,
                      4326 is WSG84 Lat/Long).
    :param bboxs: is a list of bounding boxes ([xMin, xMax, yMin, yMax]) to be
                  saved to the output vector.
    :param atts: is a dict of lists of attributes with the same length as the bboxs
                 list. The dict should be named the same as the attTypes['names'] list.
    :param att_types: is a dict with a list of attribute names (attTypes['names']) and
                     types (attTypes['types']). The list must be the same length as
                     one another and the number of atts. Example type: ogr.OFTString
    :param overwrite: overwrite the vector file specified if it exists. Use False
                      for GPKG where you want to add multiple layers.
    """
    try:
        gdal.UseExceptions()

        if os.path.exists(vec_file) and (not overwrite):
            # Open the output file.
            outDataSource = gdal.OpenEx(vec_file, gdal.GA_Update)
        else:
            # Create the output Driver
            outDriver = ogr.GetDriverByName(out_format)
            # Create the output vector file
            outDataSource = outDriver.CreateDataSource(vec_file)

        # create the spatial reference
        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsg_code))
        outLayer = outDataSource.CreateLayer(vec_lyr, srs, geom_type=ogr.wkbPolygon)

        addAtts = False
        if (atts is not None) and (att_types is not None):
            nAtts = 0
            if not "names" in att_types:
                raise rsgislib.RSGISPyException(
                    'attTypes must include a list for "names"'
                )
            nAtts = len(att_types["names"])
            if not "types" in att_types:
                raise rsgislib.RSGISPyException(
                    'attTypes must include a list for "types"'
                )
            if nAtts != len(att_types["types"]):
                raise rsgislib.RSGISPyException(
                    'attTypes "names" and "types" lists must be the same length.'
                )
            for i in range(nAtts):
                if att_types["names"][i] not in atts:
                    raise rsgislib.RSGISPyException(
                        '"{}" is not within atts'.format(att_types["names"][i])
                    )
                if len(atts[att_types["names"][i]]) != len(bboxs):
                    raise rsgislib.RSGISPyException(
                        '"{}" in atts does not have the same len as bboxs'.format(
                            att_types["names"][i]
                        )
                    )

            for i in range(nAtts):
                field_defn = ogr.FieldDefn(att_types["names"][i], att_types["types"][i])
                if outLayer.CreateField(field_defn) != 0:
                    raise rsgislib.RSGISPyException(
                        "Creating '" + att_types["names"][i] + "' field failed.\n"
                    )
            addAtts = True
        elif not ((atts is None) and (att_types is None)):
            raise rsgislib.RSGISPyException(
                "If atts or attTypes is not None then the other should also not be "
                "none and equalivent in length."
            )

        # Get the output Layer's Feature Definition
        featureDefn = outLayer.GetLayerDefn()

        openTransaction = False
        for n in range(len(bboxs)):
            if not openTransaction:
                outLayer.StartTransaction()
                openTransaction = True

            bbox = bboxs[n]
            # Create Linear Ring
            ring = ogr.Geometry(ogr.wkbLinearRing)
            ring.AddPoint(bbox[0], bbox[3])
            ring.AddPoint(bbox[1], bbox[3])
            ring.AddPoint(bbox[1], bbox[2])
            ring.AddPoint(bbox[0], bbox[2])
            ring.AddPoint(bbox[0], bbox[3])
            # Create polygon.
            poly = ogr.Geometry(ogr.wkbPolygon)
            poly.AddGeometry(ring)
            # Add to output shapefile.
            outFeature = ogr.Feature(featureDefn)
            outFeature.SetGeometry(poly)
            if addAtts:
                # Add Attributes
                for i in range(nAtts):
                    outFeature.SetField(
                        att_types["names"][i], atts[att_types["names"][i]][n]
                    )
            outLayer.CreateFeature(outFeature)
            outFeature = None
            if ((n % 20000) == 0) and openTransaction:
                outLayer.CommitTransaction()
                openTransaction = False

        if openTransaction:
            outLayer.CommitTransaction()
            openTransaction = False
        outDataSource = None
    except Exception as e:
        raise e


def write_pts_to_vec(
    out_vec_file,
    out_vec_lyr,
    out_format,
    epsg_code,
    pts_x,
    pts_y,
    atts=None,
    att_types=None,
    replace=True,
    file_opts=[],
    lyr_opts=[],
):
    """
    This function creates a set of polygons for a set of bounding boxes.
    When creating an attribute the available data types are ogr.OFTString,
    ogr.OFTInteger, ogr.OFTReal

    :param out_vec_file: output vector file/path
    :param out_vec_lyr: output vector layer
    :param out_format: the output vector layer type.
    :param epsg_code: EPSG code specifying the projection of the data
                      (e.g., 4326 is WSG84 Lat/Long).
    :param pts_x: is a list of x coordinates.
    :param pts_y: is a list of y coordinates.
    :param atts: is a dict of lists of attributes with the same length as the
                 ptsX & ptsY lists. The dict should be named the same as the
                 attTypes['names'] list.
    :param att_types: is a dict with a list of attribute names (attTypes['names'])
                      and types (attTypes['types']). The list must be the same length
                      as one another and the number of atts. Example type: ogr.OFTString
    :param replace: if the output vector file exists overwrite.
    :param file_opts: Options passed when creating the file. Default: [].
                      Common value might be ["OVERWRITE=YES"]
    :param lyr_opts: Options passed when create the layer Default: []. Common value
                     might be ["OVERWRITE=YES"]
    """
    import rsgislib.vectorutils
    from osgeo import ogr
    from osgeo import gdal
    from osgeo import osr

    try:
        if len(pts_x) != len(pts_y):
            raise rsgislib.RSGISPyException(
                "The X and Y coordinates lists are not the same length."
            )
        nPts = len(pts_x)

        gdal.UseExceptions()

        if os.path.exists(out_vec_file) and (not replace):
            vecDS = gdal.OpenEx(out_vec_file, gdal.GA_Update)
        else:
            if os.path.exists(out_vec_file):
                rsgislib.vectorutils.delete_vector_file(out_vec_file)
            outdriver = ogr.GetDriverByName(out_format)
            vecDS = outdriver.CreateDataSource(out_vec_file, options=file_opts)

        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsg_code))
        outLayer = vecDS.CreateLayer(
            out_vec_lyr, srs, geom_type=ogr.wkbPoint, options=lyr_opts
        )

        addAtts = False
        if (atts is not None) and (att_types is not None):
            nAtts = 0
            if not "names" in att_types:
                raise rsgislib.RSGISPyException(
                    'attTypes must include a list for "names"'
                )
            nAtts = len(att_types["names"])
            if not "types" in att_types:
                raise rsgislib.RSGISPyException(
                    'attTypes must include a list for "types"'
                )
            if nAtts != len(att_types["types"]):
                raise rsgislib.RSGISPyException(
                    'attTypes "names" and "types" lists must be the same length.'
                )
            for i in range(nAtts):
                if att_types["names"][i] not in atts:
                    raise rsgislib.RSGISPyException(
                        '"{}" is not within atts'.format(att_types["names"][i])
                    )
                if len(atts[att_types["names"][i]]) != len(pts_x):
                    raise rsgislib.RSGISPyException(
                        '"{}" in atts does not have the same len as pts_x'.format(
                            att_types["names"][i]
                        )
                    )

            for i in range(nAtts):
                field_defn = ogr.FieldDefn(att_types["names"][i], att_types["types"][i])
                if outLayer.CreateField(field_defn) != 0:
                    raise rsgislib.RSGISPyException(
                        "Creating '" + att_types["names"][i] + "' field failed.\n"
                    )
            addAtts = True
        elif not ((atts is None) and (att_types is None)):
            raise rsgislib.RSGISPyException(
                "If atts or attTypes is not None then the other should also "
                "not be none and equivlent in length."
            )

        # Get the output Layer's Feature Definition
        featureDefn = outLayer.GetLayerDefn()

        openTransaction = False
        for n in range(nPts):
            if not openTransaction:
                outLayer.StartTransaction()
                openTransaction = True
            # Create Point
            pt = ogr.Geometry(ogr.wkbPoint)
            pt.AddPoint(float(pts_x[n]), float(pts_y[n]))
            # Add to output shapefile.
            outFeature = ogr.Feature(featureDefn)
            outFeature.SetGeometry(pt)
            if addAtts:
                # Add Attributes
                for i in range(nAtts):
                    outFeature.SetField(
                        att_types["names"][i], atts[att_types["names"][i]][n]
                    )
            outLayer.CreateFeature(outFeature)
            outFeature = None
            if ((n % 20000) == 0) and openTransaction:
                outLayer.CommitTransaction()
                openTransaction = False

        if openTransaction:
            outLayer.CommitTransaction()
            openTransaction = False
        vecDS = None
    except Exception as e:
        raise e


def create_bboxs_for_pts(
    vec_file: str,
    vec_lyr: str,
    bbox_width: float,
    bbox_height: float,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    del_exist_vec: bool = False,
    epsg_code: int = None,
):
    """
    A function which takes a set of points (from the input vector layer) and creates
    a set of boxes with the same height and width, one for each point.

    Note, the geometry type for the input vector layer must be points.

    :param vec_file: the input vector file/path
    :param vec_lyr: the name of the input vector layer.
    :param bbox_width: width (in the units of the projection) for the output boxes
    :param bbox_height: height (in the units of the projection) for the output boxes
    :param out_vec_file: output vector file/path
    :param out_vec_lyr: output vector layer name
    :param out_format: output vector format.
    :param del_exist_vec: If the output file already exists delete it before proceeding.
    :param epsg_code: if not well defined specify the EPSG code for the projection.

    """
    import rsgislib.vectorutils

    if os.path.exists(out_vec_file):
        if del_exist_vec:
            rsgislib.vectorutils.delete_vector_file(out_vec_file)
        else:
            raise rsgislib.RSGISPyException(
                "The output vector file ({}) already exists, "
                "remove it and re-run.".format(out_vec_file)
            )

    h_width = bbox_width / 2.0
    h_height = bbox_height / 2.0

    vec_ds_obj = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if vec_ds_obj is None:
        raise rsgislib.RSGISPyException("The input vector file could not be opened.")
    vec_lyr_obj = vec_ds_obj.GetLayer(vec_lyr)
    if vec_lyr_obj is None:
        raise rsgislib.RSGISPyException("The input vector layer could not be opened.")
    if epsg_code is None:
        epsg_code = rsgislib.vectorutils.get_proj_epsg_from_vec(vec_file, vec_lyr)

    n_feats = vec_lyr_obj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)

    counter = 0
    in_feature = vec_lyr_obj.GetNextFeature()
    out_bboxs = list()
    while in_feature:

        geom = in_feature.GetGeometryRef()
        if geom is not None:
            pt_x = geom.GetX()
            pt_y = geom.GetY()

            x_min = pt_x - h_width
            x_max = pt_x + h_width
            y_min = pt_y - h_height
            y_max = pt_y + h_height

            out_bboxs.append([x_min, x_max, y_min, y_max])

        in_feature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    pbar.close()
    vec_ds_obj = None

    create_poly_vec_bboxs(
        out_vec_file,
        out_vec_lyr,
        out_format,
        epsg_code,
        out_bboxs,
        atts=None,
        att_types=None,
        overwrite=False,
    )


def create_wgs84_vector_grid(
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str,
    grid_x: int,
    grid_y: int,
    bbox: List[float],
    overlap: float = None,
    tile_names_col: str = "tile_names",
    tile_name_prefix: str = "",
):
    """
    A function which creates a regular grid across a defined area using the
    WGS84 (EPSG:4326) projection.

    :param out_vec_file: output vector file
    :param out_vec_lyr: output vector layer name
    :param out_format: the output vector file format.
    :param grid_x: the size in the x axis of the grid cells.
    :param grid_y: the size in the y axis of the grid cells.
    :param bbox: the area for which cells will be defined (MinX, MaxX, MinY, MaxY).
    :param overlap: the overlap added to each grid cell. If None then no overlap applied.
    :param tile_names_col: The output column name for the tile names.
    :param tile_name_prefix: A prefix for the tile names.

    """
    import math
    import rsgislib.tools.projection
    import rsgislib.vectorattrs

    epsg_code = 4326
    min_x = float(bbox[0])
    max_x = float(bbox[1])
    min_y = float(bbox[2])
    max_y = float(bbox[3])
    grid_x = float(grid_x)
    grid_y = float(grid_y)

    n_x_cells = math.floor((max_x - min_x) / grid_x)
    x_remain = (max_x - min_x) - (grid_x * n_x_cells)

    n_y_cells = math.floor((max_y - min_y) / grid_y)
    y_remain = (max_y - min_y) - (grid_y * n_y_cells)

    print("Cells: [{0}, {1}]".format(n_x_cells, n_y_cells))

    bboxs = []
    tile_names = []
    for i in range(n_y_cells):
        c_max_y = max_y - (i * grid_y)
        c_min_y = c_max_y - grid_y
        for j in range(n_x_cells):
            c_min_x = min_x + (j * grid_x)
            c_max_x = c_min_x + grid_x
            lat_lon_str_name = rsgislib.tools.projection.get_deg_coord_as_str(
                lat=c_min_x, lon=c_max_y, n_chars=4
            )
            tile_names.append(f"{tile_name_prefix}{lat_lon_str_name}")
            if overlap is None:
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            else:
                bboxs.append(
                    [
                        c_min_x - overlap,
                        c_max_x + overlap,
                        c_min_y - overlap,
                        c_max_y + overlap,
                    ]
                )
        if x_remain > 0:
            c_min_x = min_x + (n_x_cells * grid_x)
            c_max_x = c_min_x + x_remain
            lat_lon_str_name = rsgislib.tools.projection.get_deg_coord_as_str(
                lat=c_min_x, lon=c_max_y, n_chars=4
            )
            tile_names.append(f"{tile_name_prefix}{lat_lon_str_name}")
            if overlap is None:
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            else:
                bboxs.append(
                    [
                        c_min_x - overlap,
                        c_max_x + overlap,
                        c_min_y - overlap,
                        c_max_y + overlap,
                    ]
                )
    if y_remain > 0:
        c_max_y = max_y - (n_y_cells * grid_y)
        c_min_y = c_max_y - y_remain
        for j in range(n_x_cells):
            c_min_x = min_x + (j * grid_x)
            c_max_x = c_min_x + grid_x
            lat_lon_str_name = rsgislib.tools.projection.get_deg_coord_as_str(
                lat=c_min_x, lon=c_max_y, n_chars=4
            )
            tile_names.append(f"{tile_name_prefix}{lat_lon_str_name}")
            if overlap is None:
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            else:
                bboxs.append(
                    [
                        c_min_x - overlap,
                        c_max_x + overlap,
                        c_min_y - overlap,
                        c_max_y + overlap,
                    ]
                )
        if x_remain > 0:
            c_min_x = min_x + (n_x_cells * grid_x)
            c_max_x = c_min_x + x_remain
            lat_lon_str_name = rsgislib.tools.projection.get_deg_coord_as_str(
                lat=c_min_x, lon=c_max_y, n_chars=4
            )
            tile_names.append(f"{tile_name_prefix}{lat_lon_str_name}")
            if overlap is None:
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            else:
                bboxs.append(
                    [
                        c_min_x - overlap,
                        c_max_x + overlap,
                        c_min_y - overlap,
                        c_max_y + overlap,
                    ]
                )

    for bbox in bboxs:
        if bbox[2] < -180:
            bbox[2] = -180
        if bbox[3] > 180:
            bbox[3] = 180

    create_poly_vec_bboxs(out_vec_file, out_vec_lyr, out_format, epsg_code, bboxs)
    rsgislib.vectorattrs.write_vec_column(
        out_vec_file, out_vec_lyr, tile_names_col, ogr.OFTString, tile_names
    )


def create_vec_for_image(
    input_imgs: List,
    output_dir: str,
    out_format: str = "GeoJSON",
    geometry_type: int = rsgislib.GEOM_PT,
    out_name_replace: dict = None,
    out_file_ext=None,
    del_exist_vec=False,
):
    """
    A function which creates a simple (dummy) vector layer for each input images.
    This function is intended to save time creating vector layers where a vector
    layer is needed for a set of images but digitising some information.

    A single geometry is added to the layer, for a point this is the image centre,
    for a line this is from the TL to BR and for a polygon this is the bbox.

    :param input_imgs: a list of input images.
    :param output_dir: a directory where the output vector layers will be created
    :param out_format: the output format for the vector layers (Default: GeoJSON)
    :param geometry_type: the geometry type of the vector layers (rsgislib.GEOM_PT,
                          rsgislib.GEOM_LINE or rsgislib.GEOM_POLY)
                          Default: rsgislib.GEOM_PT
    :param out_name_replace: a dictionary of replacement values for editing the input
                             image file names. If None (default) then ignored. For
                             example, {"_ortho", ""} will remove '_ortho' from the
                             input file names.
    :param out_file_ext: the output extension for the output files (e.g., geojson)
                         If None (Default) then this will be created.
    :param del_exist_vec: delete the vector files if they already exist (Default: False)

    """
    import rsgislib.tools.filetools
    import rsgislib.imageutils
    import rsgislib.vectorutils
    from osgeo import ogr
    from osgeo import osr
    import tqdm

    if out_file_ext is None:
        out_file_ext = rsgislib.vectorutils.get_file_vec_extension(out_format)

    out_geom_type = ogr.wkbPoint
    if geometry_type == rsgislib.GEOM_PT:
        out_geom_type = ogr.wkbPoint
    elif geometry_type == rsgislib.GEOM_LINE:
        out_geom_type = ogr.wkbLineString
    elif geometry_type == rsgislib.GEOM_POLY:
        out_geom_type = ogr.wkbPolygon
    else:
        rsgislib.RSGISPyException(
            "Input geometry type was not recognised - only support "
            "rsgislib.GEOM_PT, rsgislib.GEOM_LINE, rsgislib.GEOM_POLY"
        )

    # Create the output driver
    out_driver = ogr.GetDriverByName(out_format)

    for img in tqdm.tqdm(input_imgs):
        basename = rsgislib.tools.filetools.get_file_basename(img, check_valid=True)
        if out_name_replace is not None:
            for rpl_in_str in out_name_replace:
                basename = basename.replace(rpl_in_str, out_name_replace[rpl_in_str])

        out_vec_file = os.path.join(output_dir, f"{basename}.{out_file_ext}")
        out_vec_lyr = basename

        if os.path.exists(out_vec_file):
            if del_exist_vec:
                driver = ogr.GetDriverByName(out_format)
                driver.DeleteDataSource(out_vec_file)
            else:
                raise rsgislib.RSGISPyException("Output file already exists")

        # create the spatial reference
        img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(img)
        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(img_epsg))

        # Create the output vector source and layer
        out_ds_obj = out_driver.CreateDataSource(out_vec_file)
        out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, srs, geom_type=out_geom_type)
        # Get the output Layer's Feature Definition
        feat_defn_obj = out_lyr_obj.GetLayerDefn()

        img_bbox = rsgislib.imageutils.get_img_bbox(img)

        if geometry_type == rsgislib.GEOM_PT:
            x_pt = img_bbox[0] + ((img_bbox[1] - img_bbox[0]) / 2)
            y_pt = img_bbox[2] + ((img_bbox[3] - img_bbox[2]) / 2)
            geom = ogr.Geometry(ogr.wkbPoint)
            geom.AddPoint(x_pt, y_pt)
        elif geometry_type == rsgislib.GEOM_LINE:
            geom = ogr.Geometry(ogr.wkbLineString)
            geom.AddPoint(img_bbox[0], img_bbox[3])
            geom.AddPoint(img_bbox[1], img_bbox[2])
        elif geometry_type == rsgislib.GEOM_POLY:
            ring = ogr.Geometry(ogr.wkbLinearRing)
            ring.AddPoint(img_bbox[0], img_bbox[3])
            ring.AddPoint(img_bbox[1], img_bbox[3])
            ring.AddPoint(img_bbox[1], img_bbox[2])
            ring.AddPoint(img_bbox[0], img_bbox[2])
            ring.AddPoint(img_bbox[0], img_bbox[3])
            geom = ogr.Geometry(ogr.wkbPolygon)
            geom.AddGeometry(ring)
        else:
            raise rsgislib.RSGISPyException("Do not recognise geometry type")

        out_feat_obj = ogr.Feature(feat_defn_obj)
        out_feat_obj.SetGeometry(geom)
        out_lyr_obj.CreateFeature(out_feat_obj)
        out_feat_obj = None
        out_lyr_obj.SyncToDisk()
        out_ds_obj = None
