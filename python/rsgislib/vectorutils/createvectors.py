#!/usr/bin/env python
"""
Tools for creating vector layers.
"""

import os

import osgeo.gdal as gdal
import osgeo.ogr as ogr
import osgeo.osr as osr

import tqdm

import rsgislib

gdal.UseExceptions()



def polygonise_raster_to_vec_lyr(out_vec_file: str, out_vec_lyr: str, out_format: str, input_img: str, img_band: int =1,
                                 mask_img: str =None, mask_band: int =1, replace_file: bool =True, replace_lyr: bool =True,
                                 pxl_val_fieldname: str ='PXLVAL', use_8_conn: bool =False):
    """
A utility to polygonise a raster to a OGR vector layer. Recommended that you output with 8 connectedness
otherwise the resulting vector can be invalid and cause problems for further processing in GIS applications.

Where:

:param out_vec_file: is a string specifying the output vector file path. If it exists it will be deleted and overwritten.
:param out_vec_lyr: is a string with the name of the vector layer.
:param out_format: is a string with the driver
:param input_img: is a string specifying the input image file to be polygonised
:param img_band: is an int specifying the image band to be polygonised. (default = 1)
:param mask_img: is an optional string mask file specifying a no data mask (default = None)
:param mask_band: is an int specifying the image band to be used the mask (default = 1)
:param replace_file: is a boolean specifying whether the vector file should be replaced (i.e., overwritten). Default=True.
:param replace_lyr: is a boolean specifying whether the vector layer should be replaced (i.e., overwritten). Default=True.
:param pxl_val_fieldname: is a string to specify the name of the output column representing the pixel value within the input image.
:param use_8_conn: is a bool specifying whether 8 connectedness or 4 connectedness should be used (4 is RSGISLib/GDAL default)

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
        raise Exception("Could not open or create '{}'".format(out_vec_file))

    lcl_options = []
    if replace_lyr:
        lcl_options = ['OVERWRITE=YES']

    out_lyr_obj = vecDS.CreateLayer(out_vec_lyr, srs=imgsrs, options=lcl_options)
    if out_lyr_obj is None:
        raise Exception("Could not create layer: {}".format(out_vec_lyr))

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
        options.append('8CONNECTED=8')

    print("Polygonising...")
    gdal.Polygonize(imgBand, imgMaskBand, out_lyr_obj, dstFieldIdx, options, callback=callback )
    print("Completed")
    out_lyr_obj.SyncToDisk()
    vecDS = None
    gdalImgDS = None
    if mask_img is not None:
        gdalImgMaskDS = None


def vectorise_pxls_to_pts(input_img, img_band, img_msk_val, vec_out_file, vec_out_lyr=None, out_format='GPKG',
                          out_epsg_code=None, del_exist_vec=False):
    """
    Function which creates a new output vector file for the pixels within the input image file
    with the value specified. Pixel locations will be the centroid of the the pixel

    Where:

    :param input_img: the input image
    :param img_band: the band within the image to use
    :param img_msk_val: the image value selecting the pixels to be converted to points
    :param vec_out_file: Output vector file
    :param vec_out_lyr: output vector layer name.
    :param out_format: output file format (default GPKG).
    :param out_epsg_code: optionally provide an EPSG code for the output layer. If None then taken from input image.
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

    if os.path.exists(vec_out_file):
        if del_exist_vec:
            rsgislib.vectorutils.delete_vector_file(vec_out_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vec_out_file))

    if out_epsg_code is None:
        out_epsg_code = rsgislib.imageutils.get_epsg_proj_from_image(input_img)

    if out_epsg_code is None:
        raise Exception("The output ESPG code is None - tried to read from input image and "
                        "returned None. Suggest providing the EPSG code to the function.")

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

        gdf = geopandas.GeoDataFrame(geometry=geopandas.points_from_xy(pt_x_lst, pt_y_lst),
                                     crs="EPSG:{}".format(out_epsg_code))

        if out_format == "GPKG":
            if vec_out_lyr is None:
                raise Exception("If output format is GPKG then an output layer is required.")
            gdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
        else:
            gdf.to_file(vec_out_file, driver=out_format)


def extract_image_footprint(inputImg, outVec, tmpDIR='./tmp', rePrjTo=None):
    """
A function to extract an image footprint as a vector.

:param inputImg: the input image file for which the footprint will be extracted.
:param outVec: output shapefile path and name.
:param tmpDIR: temp directory which will be used during processing. It will be created and deleted once processing complete.
:param rePrjTo: optional command

"""
    gdal.UseExceptions()
    import rsgislib.tools.utils
    import rsgislib.imageutils
    import rsgislib.vectorutils

    uidStr = rsgislib.tools.utils.uid_generator()

    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True

    inImgBase = os.path.splitext(os.path.basename(inputImg))[0]

    validOutImg = os.path.join(tmpDIR, inImgBase + '_' + uidStr + '_validimg.kea')
    inImgNoData = rsgislib.imageutils.get_image_no_data_value(inputImg)
    rsgislib.imageutils.gen_valid_mask(inimages=inputImg, outimage=validOutImg,
                                     gdalformat='KEA', nodata=inImgNoData)

    outVecTmpFile = outVec
    if not (rePrjTo is None):
        outVecTmpFile = os.path.join(tmpDIR,
                                     inImgBase + '_' + uidStr + '_initVecOut.shp')

    rsgislib.vectorutils.polygoniseRaster(validOutImg, outVecTmpFile, imgBandNo=1,
                                          maskImg=validOutImg, imgMaskBandNo=1)
    vecLayerName = os.path.splitext(os.path.basename(outVecTmpFile))[0]
    ds = gdal.OpenEx(outVecTmpFile, gdal.OF_READONLY)
    if ds is None:
        raise Exception("Could not open '" + outVecTmpFile + "'")

    lyr = ds.GetLayerByName(vecLayerName)
    if lyr is None:
        raise Exception("Could not find layer '" + vecLayerName + "'")
    numFeats = lyr.GetFeatureCount()
    lyr = None
    ds = None

    fileName = []
    for i in range(numFeats):
        fileName.append(os.path.basename(inputImg))
    rsgislib.vectorutils.write_vec_column(outVecTmpFile, vecLayerName, 'FileName',
                                        ogr.OFTString, fileName)

    if not (rePrjTo is None):
        if os.path.exists(outVec):
            driver = ogr.GetDriverByName('ESRI Shapefile')
            driver.DeleteDataSource(outVec)

        cmd = 'ogr2ogr -f "ESRI Shapefile" -t_srs ' + rePrjTo + ' ' + outVec + ' ' + outVecTmpFile
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise Exception('Could not re-projection shapefile: ' + cmd)

    if createdTmp:
        shutil.rmtree(tmpDIR)
    else:
        if not (rePrjTo is None):
            driver = ogr.GetDriverByName('ESRI Shapefile')
            driver.DeleteDataSource(outVecTmpFile)


def createPolySHP4LstBBOXs(csvFile, outSHP, epsgCode, minXCol=0, maxXCol=1, minYCol=2, maxYCol=3, ignoreRows=0, del_exist_vec=False):
    """
This function takes a CSV file of bounding boxes (1 per line) and creates a polygon shapefile.

:param csvFile: input CSV file.
:param outSHP: output ESRI shapefile
:param epsgCode: EPSG code specifying the projection of the data (4326 is WSG84 Lat/Long).
:param minXCol: The index (starting at 0) for the column within the CSV file for the minimum X coordinate.
:param maxXCol: The index (starting at 0) for the column within the CSV file for the maximum X coordinate.
:param minYCol: The index (starting at 0) for the column within the CSV file for the minimum Y coordinate.
:param maxYCol: The index (starting at 0) for the column within the CSV file for the maximum Y coordinate.
:param ignoreRows: The number of rows to ignore from the start of the CSV file (i.e., column headings)
:param del_exist_vec: If the output file already exists delete it before proceeding.

"""
    gdal.UseExceptions()
    try:
        if os.path.exists(outSHP):
            if del_exist_vec:
                driver = ogr.GetDriverByName('ESRI Shapefile')
                driver.DeleteDataSource(outSHP)
            else:
                raise Exception("Output file already exists")
        # Create the output Driver
        outDriver = ogr.GetDriverByName('ESRI Shapefile')
        # create the spatial reference, WGS84
        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsgCode))
        # Create the output Shapefile
        outDataSource = outDriver.CreateDataSource(outSHP)
        outLayer = outDataSource.CreateLayer(os.path.splitext(os.path.basename(outSHP))[
                                                 0], srs, geom_type=ogr.wkbPolygon)
        # Get the output Layer's Feature Definition
        featureDefn = outLayer.GetLayerDefn()

        dataFile = open(csvFile, 'r')
        rowCount = 0
        for line in dataFile:
            if rowCount >= ignoreRows:
                line = line.strip()
                if line != "":
                    comps = line.split(',')
                    # Get values from CSV file.
                    minX = float(comps[minXCol])
                    maxX = float(comps[maxXCol])
                    minY = float(comps[minYCol])
                    maxY = float(comps[maxYCol])
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


def defineGrid(bbox, x_size, y_size, in_epsg_code, out_vec, out_vec_lyr, vec_drv='GPKG', out_epsg_code=None,
               utm_grid=False, utm_hemi=False):
    """
Define a grid of bounding boxes for a specified bounding box. The output grid can be in a different projection
to the inputted bounding box. Where a UTM grid is required and there are multiple UTM zones then the
layer name will be appended with utmXX[n|s]. Note. this only works with formats such as GPKG which support
multiple layers. A shapefile which only supports 1 layer will not work.

:param bbox: a bounding box (xMin, xMax, yMin, yMax)
:param x_size: Output grid size in X axis. If out_epsg_code or utm_grid defined then the grid size
               needs to be in the output unit.
:param y_size: Output grid size in Y axis. If out_epsg_code or utm_grid defined then the grid size
               needs to be in the output unit.
:param in_epsg_code: EPSG code for the projection of the bbox
:param out_vec: output vector file.
:param out_vec_lyr: output vector layer name.
:param vec_drv: output vector file format (see OGR codes). Default is GPKG.
:param out_epsg_code: if provided the output grid is reprojected to the projection defined by this EPSG code.
                      (note. the grid size needs to the in the unit of this projection). Default is None.
:param utm_grid: provide the output grid in UTM projection where grid might go across multiple UTM zones.
                 Default is False. grid size unit should be metres.
:param utm_hemi: if outputting a UTM projected grid then decided whether to use hemispheres or otherwise. If False
                 then everything will be projected northern hemisphere (e.g., as with landsat or sentinel-2).
                 Default is False.

"""
    import rsgislib.tools.utm
    import rsgislib.tools.geometrytools
    if (out_epsg_code is not None) and utm_grid:
        raise Exception("Cannot specify both new output projection and UTM grid.")
    elif utm_grid:
        wgs84_bbox = bbox
        if in_epsg_code != 4326:
            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(in_epsg_code)
            out_proj_obj = osr.SpatialReference()
            out_proj_obj.ImportFromEPSG(4326)
            wgs84_bbox = rsgislib.tools.geometrytools.reprojBBOX(bbox, in_proj_obj, out_proj_obj)

        multi_zones = False
        if (wgs84_bbox[0] < -180) and (wgs84_bbox[1] < -180):
            wgs84_bbox = [360 + wgs84_bbox[0], 360 + wgs84_bbox[1], wgs84_bbox[2],
                          wgs84_bbox[3]]
        elif (wgs84_bbox[0] > 180) and (wgs84_bbox[1] > 180):
            wgs84_bbox = [360 - wgs84_bbox[0], 360 - wgs84_bbox[1], wgs84_bbox[2],
                          wgs84_bbox[3]]
        elif (wgs84_bbox[0] < -180) or (wgs84_bbox[0] > 180):
            multi_zones = True

        if not multi_zones:
            utm_tl = rsgislib.tools.utm.from_latlon(wgs84_bbox[3], wgs84_bbox[0])
            utm_tr = rsgislib.tools.utm.from_latlon(wgs84_bbox[3], wgs84_bbox[1])
            utm_br = rsgislib.tools.utm.from_latlon(wgs84_bbox[2], wgs84_bbox[1])
            utm_bl = rsgislib.tools.utm.from_latlon(wgs84_bbox[2], wgs84_bbox[0])

            utm_top_hemi = 'N'
            if utm_hemi and (wgs84_bbox[3] < 0):
                utm_top_hemi = 'S'

        if (not multi_zones) and (utm_tl[2] == utm_tr[2] == utm_br[2] == utm_bl[2]):
            utm_zone = utm_tl[2]

            utm_proj_epsg = rsgislib.tools.utm.epsg_for_UTM(utm_zone, utm_top_hemi)

            defineGrid(bbox, x_size, y_size, in_epsg_code, out_vec, out_vec_lyr, vec_drv=vec_drv,
                       out_epsg_code=utm_proj_epsg, utm_grid=False, utm_hemi=False)
        else:
            multi_zones = True

            utm_zone_bboxs = []
            if (wgs84_bbox[0] < -180):
                wgs84_bbox_W = [-180, wgs84_bbox[1], wgs84_bbox[2], wgs84_bbox[3]]
                wgs84_bbox_E = [360 + wgs84_bbox[1], 180, wgs84_bbox[2], wgs84_bbox[3]]

                utm_zone_bboxs = utm_zone_bboxs + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_W)
                utm_zone_bboxs = utm_zone_bboxs + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_E)

            elif (wgs84_bbox[0] > 180):
                wgs84_bbox_W = [wgs84_bbox[0], 180, wgs84_bbox[2], wgs84_bbox[3]]
                wgs84_bbox_E = [-180, 360 - wgs84_bbox[1], wgs84_bbox[2], wgs84_bbox[3]]

                utm_zone_bboxs = utm_zone_bboxs + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_W)
                utm_zone_bboxs = utm_zone_bboxs + rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox_E)

            else:
                utm_zone_bboxs = rsgislib.tools.utm.split_wgs84_bbox_utm_zones(wgs84_bbox)

            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(4326)

            first = True
            for zone_roi in utm_zone_bboxs:
                utm_top_hemi = 'N'
                if utm_hemi:
                    if zone_roi[1][3] < 0:
                        utm_top_hemi = 'S'

                utm_proj_epsg = int(rsgislib.tools.utm.epsg_for_UTM(
                    zone_roi[0], utm_top_hemi))

                out_proj_obj = osr.SpatialReference()
                out_proj_obj.ImportFromEPSG(utm_proj_epsg)

                utm_bbox = rsgislib.tools.geometrytools.reprojBBOX(
                    zone_roi[1], in_proj_obj, out_proj_obj)
                bboxs = rsgislib.tools.geometrytools.getBBoxGrid(utm_bbox, x_size, y_size)

                utm_out_vec_lyr = out_vec_lyr + '_utm{0}{1}'.format(
                    zone_roi[0], utm_top_hemi.lower())
                create_poly_vec_bboxs(out_vec, utm_out_vec_lyr, vec_drv, utm_proj_epsg, bboxs, overwrite=first)
                first = False
    else:
        if out_epsg_code is not None:
            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(in_epsg_code)
            out_proj_obj = osr.SpatialReference()
            out_proj_obj.ImportFromEPSG(out_epsg_code)
            proj_bbox = rsgislib.tools.geometrytools.reprojBBOX(bbox, in_proj_obj, out_proj_obj)
        else:
            proj_bbox = bbox

        bboxs = rsgislib.tools.geometrytools.getBBoxGrid(proj_bbox, x_size, y_size)

        if out_epsg_code is None:
            create_poly_vec_bboxs(out_vec, out_vec_lyr, vec_drv, in_epsg_code, bboxs)
        else:
            create_poly_vec_bboxs(out_vec, out_vec_lyr, vec_drv, out_epsg_code, bboxs)


def create_poly_vec_bboxs(vec_file, vec_lyr, out_format, epsg_code, bboxs, atts=None, att_types=None, overwrite=True):
    """
This function creates a set of polygons for a set of bounding boxes.
When creating an attribute the available data types are ogr.OFTString, ogr.OFTInteger, ogr.OFTReal

:param vec_file: output vector file/path
:param vec_lyr: output vector layer
:param out_format: the output vector layer type.
:param epsg_code: EPSG code specifying the projection of the data (e.g., 4326 is WSG84 Lat/Long).
:param bboxs: is a list of bounding boxes ([xMin, xMax, yMin, yMax]) to be saved to the output vector.
:param atts: is a dict of lists of attributes with the same length as the bboxs list. The dict should be named
             the same as the attTypes['names'] list.
:param att_types: is a dict with a list of attribute names (attTypes['names']) and types (attTypes['types']).
                 The list must be the same length as one another and the number of atts. Example type: ogr.OFTString
:param overwrite: - overwrite the vector file specified if it exists. Use False for GPKG where you want to add multiple layers.

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
            if not 'names' in att_types:
                raise Exception('attTypes must include a list for "names"')
            nAtts = len(att_types['names'])
            if not 'types' in att_types:
                raise Exception('attTypes must include a list for "types"')
            if nAtts != len(att_types['types']):
                raise Exception('attTypes "names" and "types" lists must be the same length.')
            for i in range(nAtts):
                if att_types['names'][i] not in atts:
                    raise Exception('"{}" is not within atts'.format(
                        att_types['names'][i]))
                if len(atts[att_types['names'][i]]) != len(bboxs):
                    raise Exception('"{}" in atts does not have the same len as bboxs'.format(
                        att_types['names'][i]))

            for i in range(nAtts):
                field_defn = ogr.FieldDefn(att_types['names'][i], att_types['types'][i])
                if outLayer.CreateField(field_defn) != 0:
                    raise Exception("Creating '" + att_types['names'][
                        i] + "' field failed.\n")
            addAtts = True
        elif not ((atts is None) and (att_types is None)):
            raise Exception('If atts or attTypes is not None then the other should also not be none and equalivent in length.')

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
                        att_types['names'][i], atts[att_types['names'][i]][n])
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


def create_vector_grid(out_vec_file, out_format, out_vec_lyr, epsg_code, grid_x, grid_y, bbox):
    """
A function which creates a regular grid across a defined area.

:param out_vec_file: outout file
:param epsg_code: EPSG code of the output projection
:param grid_x: the size in the x axis of the grid cells.
:param grid_y: the size in the y axis of the grid cells.
:param bbox: the area for which cells will be defined (MinX, MaxX, MinY, MaxY).
:param out_format: the output vector layer type.
:param out_vec_lyr: output vector layer

"""
    minX = float(bbox[0])
    maxX = float(bbox[1])
    minY = float(bbox[2])
    maxY = float(bbox[3])
    grid_x = float(grid_x)
    grid_y = float(grid_y)

    nXCells = math.floor((maxX - minX) / grid_x)
    x_remain = (maxX - minX) - (grid_x * nXCells)

    nYCells = math.floor((maxY - minY) / grid_y)
    y_remain = (maxY - minY) - (grid_y * nYCells)

    print("Cells: [{0}, {1}]".format(nXCells, nYCells))

    bboxs = []
    for i in range(nYCells):
        cMaxY = maxY - (i * grid_y)
        cMinY = cMaxY - grid_y
        for j in range(nXCells):
            cMinX = minX + (j * grid_x)
            cMaxX = cMinX + grid_x
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
        if x_remain > 0:
            cMinX = minX + (nXCells * grid_x)
            cMaxX = cMinX + x_remain
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
    if y_remain > 0:
        cMaxY = maxY - (nYCells * grid_y)
        cMinY = cMaxY - y_remain
        for j in range(nXCells):
            cMinX = minX + (j * grid_x)
            cMaxX = cMinX + grid_x
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
        if x_remain > 0:
            cMinX = minX + (nXCells * grid_x)
            cMaxX = cMinX + x_remain
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])

    create_poly_vec_bboxs(out_vec_file, out_vec_lyr, out_format, epsg_code, bboxs)


def writePts2Vec(vec_file, vectorLyr, out_format, epsgCode, ptsX, ptsY, atts=None, attTypes=None, replace=True, file_opts=[], lyr_opts=[]):
    """
This function creates a set of polygons for a set of bounding boxes.
When creating an attribute the available data types are ogr.OFTString, ogr.OFTInteger, ogr.OFTReal

:param vec_file: output vector file/path
:param vectorLyr: output vector layer
:param out_format: the output vector layer type.
:param epsgCode: EPSG code specifying the projection of the data (e.g., 4326 is WSG84 Lat/Long).
:param ptsX: is a list of x coordinates.
:param ptsY: is a list of y coordinates.
:param atts: is a dict of lists of attributes with the same length as the ptsX & ptsY lists.
             The dict should be named the same as the attTypes['names'] list.
:param attTypes: is a dict with a list of attribute names (attTypes['names']) and types (attTypes['types']).
                 The list must be the same length as one another and the number of atts. Example type: ogr.OFTString
:param replace: if the output vector file exists overwrite.
:param file_opts: Options passed when creating the file. Default: []. Common value might be ["OVERWRITE=YES"]
:param lyr_opts: Options passed when create the layer Default: []. Common value might be ["OVERWRITE=YES"]

"""
    import osgeo.ogr as ogr
    import osgeo.gdal as gdal
    import osgeo.osr as osr

    try:
        if len(ptsX) != len(ptsY):
            raise Exception("The X and Y coordinates lists are not the same length.")
        nPts = len(ptsX)

        gdal.UseExceptions()

        if os.path.exists(vec_file) and (not replace):
            vecDS = gdal.OpenEx(vec_file, gdal.GA_Update)
        else:
            if os.path.exists(vec_file):
                delete_vector_file(vec_file)
            outdriver = ogr.GetDriverByName(out_format)
            vecDS = outdriver.CreateDataSource(vec_file, options=file_opts)

        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsgCode))
        outLayer = vecDS.CreateLayer(vectorLyr, srs, geom_type=ogr.wkbPoint, options=lyr_opts)

        addAtts = False
        if (atts is not None) and (attTypes is not None):
            nAtts = 0
            if not 'names' in attTypes:
                raise Exception('attTypes must include a list for "names"')
            nAtts = len(attTypes['names'])
            if not 'types' in attTypes:
                raise Exception('attTypes must include a list for "types"')
            if nAtts != len(attTypes['types']):
                raise Exception('attTypes "names" and "types" lists must be the same length.')
            for i in range(nAtts):
                if attTypes['names'][i] not in atts:
                    raise Exception('"{}" is not within atts'.format(
                        attTypes['names'][i]))
                if len(atts[attTypes['names'][i]]) != len(ptsX):
                    raise Exception('"{}" in atts does not have the same len as bboxs'.format(
                        attTypes['names'][i]))

            for i in range(nAtts):
                field_defn = ogr.FieldDefn(attTypes['names'][i], attTypes['types'][i])
                if outLayer.CreateField(field_defn) != 0:
                    raise Exception("Creating '" + attTypes['names'][
                        i] + "' field failed.\n")
            addAtts = True
        elif not ((atts is None) and (attTypes is None)):
            raise Exception('If atts or attTypes is not None then the other should also not be none and equivlent in length.')

        # Get the output Layer's Feature Definition
        featureDefn = outLayer.GetLayerDefn()

        openTransaction = False
        for n in range(nPts):
            if not openTransaction:
                outLayer.StartTransaction()
                openTransaction = True
            # Create Point
            pt = ogr.Geometry(ogr.wkbPoint)
            pt.AddPoint(float(ptsX[n]), float(ptsY[n]))
            # Add to output shapefile.
            outFeature = ogr.Feature(featureDefn)
            outFeature.SetGeometry(pt)
            if addAtts:
                # Add Attributes
                for i in range(nAtts):
                    outFeature.SetField(
                        attTypes['names'][i], atts[attTypes['names'][i]][n])
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


def getVecLyrAsPts(in_vec_file, in_vec_lyr):
    """
    Get a list of points from the vectors within an input file.

    :param in_vec_file: Input vector file
    :param in_vec_lyr: Input vector layer name
    :return: returns a list of points.

    """
    import rsgislib.vectorgeoms
    from osgeo import gdal
    import tqdm

    gdal.UseExceptions()
    vec_ds_obj = gdal.OpenEx(in_vec_file, gdal.OF_VECTOR)
    vec_lyr_obj = vec_ds_obj.GetLayer(in_vec_lyr)

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
    return pts_lst



