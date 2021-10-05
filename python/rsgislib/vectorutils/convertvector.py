#!/usr/bin/env python
"""
The vector conversion tools for converting between raster and vector
"""

import os
import math

import osgeo.gdal as gdal
import osgeo.ogr as ogr
import osgeo.osr as osr

import tqdm

import rsgislib

gdal.UseExceptions()

def rasteriseVecLyr(vec_file, vec_lyr, inputImage, outImage, gdalformat="KEA",
                    burnVal=1, datatype=rsgislib.TYPE_8UINT, vecAtt=None, vecExt=False,
                    thematic=True, nodata=0):
    """
    A utillity to rasterise a vector layer to an image covering the same region and at the same resolution as the input image.

    Where:

    :param vec_file: is a string specifying the input vector file
    :param vec_lyr: is a string specifying the input vector layer name.
    :param inputImage: is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation (if None and vecExt is False them assumes output image already exists and just uses it as is burning vector into it)
    :param outImage: is a string specifying the output image for the rasterised shapefile
    :param gdalformat: is the output image format (Default: KEA).
    :param burnVal: is the value for the output image pixels if no attribute is provided.
    :param datatype: of the output file, default is rsgislib.TYPE_8UINT
    :param vecAtt: is a string specifying the attribute to be rasterised, value of None creates a binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.
    :param vecExt: is a boolean specifying that the output image should be cut to the same extent as the input shapefile (Default is False and therefore output image will be the same as the input).
    :param thematic: is a boolean (default True) specifying that the output image is an thematic dataset so a colour table will be populated.
    :param nodata: is a float specifying the no data value associated with a continous output image.

    Example::

        from rsgislib import vectorutils

        inputVector = 'crowns.shp'
        inputVectorLyr = 'crowns'
        inputImage = 'injune_p142_casi_sub_utm.kea'
        outputImage = 'psu142_crowns.kea'
        vectorutils.rasteriseVecLyr(inputVector, inputVectorLyr, inputImage, outputImage, 'KEA', vecAtt='FID')

    """
    import rsgislib.imageutils
    try:
        if vecExt:
            xRes, yRes = rsgislib.imageutils.getImageRes(inputImage)
            if yRes < -1:
                yRes = yRes * (-1)
            outRes = xRes
            if xRes > yRes:
                outRes = yRes

            rsgislib.imageutils.createCopyImageVecExtentSnap2Grid(vec_file, vec_lyr,
                                                                  outImage, outRes, 1,
                                                                  gdalformat, datatype)
        elif inputImage is None:
            print("Assuming output image is already created so just using.")
        else:
            print("Creating output image using input image")
            rsgislib.imageutils.createCopyImage(inputImage, outImage, 1, 0, gdalformat, datatype)

        print("Running Rasterise now...")
        out_img_ds = gdal.Open(outImage, gdal.GA_Update)
        if out_img_ds is None:
            raise Exception("Could not open '{}'".format(outImage))

        vec_ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
        if vec_ds is None:
            raise Exception("Could not open '{}'".format(vec_file))

        vec_lyr_obj = vec_ds.GetLayerByName(vec_lyr)
        if vec_lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))

        # Run the algorithm.
        err = 0
        if vecAtt is None:
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr_obj, burn_values=[burnVal])
        else:
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr_obj, options=["ATTRIBUTE=" + vecAtt])
        if err != 0:
            raise Exception("Rasterisation Error: " + str(err))

        out_img_ds = None
        vec_ds = None

        if thematic:
            import rsgislib.rastergis
            rsgislib.rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
        else:
            rsgislib.imageutils.popImageStats(outImage, True, nodata, True)
    except Exception as e:
        raise e


def rasteriseVecLyrObj(vec_lyr_obj, outImage, burnVal=1, vecAtt=None, calcstats=True, thematic=True, nodata=0):
    """
    A utility to rasterise a vector layer to an image covering the same region.

    Where:

    :param vec_lyr_obj: is a OGR Vector Layer Object
    :param outImage: is a string specifying the output image, this image must already exist and intersect within the input vector layer.
    :param burnVal: is the value for the output image pixels if no attribute is provided.
    :param vecAtt: is a string specifying the attribute to be rasterised, value of None creates a binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.
    :param calcstats: is a boolean specifying whether image stats and pyramids should be calculated.
    :param thematic: is a boolean (default True) specifying that the output image is an thematic dataset so a colour table will be populated.
    :param nodata: is a float specifying the no data value associated with a continous output image.

    """
    try:
        if vec_lyr_obj is None:
            raise Exception("The vec_lyr_obj passed to the function was None.")

        print("Running Rasterise now...")
        out_img_ds = gdal.Open(outImage, gdal.GA_Update)
        if out_img_ds is None:
            raise Exception("Could not open '{}'".format(outImage))

        # Run the algorithm.
        err = 0
        if vecAtt is None:
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr_obj, burn_values=[burnVal])
        else:
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr_obj, options=["ATTRIBUTE=" + vecAtt])
        if err != 0:
            raise Exception("Rasterisation Error: {}".format(err))

        out_img_ds = None

        if calcstats:
            if thematic:
                import rsgislib.rastergis
                rsgislib.rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
            else:
                import rsgislib.imageutils
                rsgislib.imageutils.popImageStats(outImage, True, nodata, True)
    except Exception as e:
        print('Failed rasterising: {}'.format(outImage))
        raise e



def polygoniseRaster2VecLyr(out_vec_file: str, out_vec_lyr: str, out_format: str, input_img: str, img_band: int =1,
                            mask_img: str =None, mask_band: int =1, replace_file: bool =True, replace_lyr: bool =True,
                            pxl_val_fieldname: str ='PXLVAL', use_8_conn: bool =True):
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
:param use_8_conn: is a bool specifying whether 8 connectedness or 4 connectedness should be used (8 is RSGISLib default but 4 is GDAL default)

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





def copyVec2RAT(vec_file, vec_lyr, inputImage, outputImage):
    """
    A utility to create raster copy of a shapefile. The output image is a KEA file and the attribute table has the attributes from the shapefile.

    Where:

    :param vec_file: is a string specifying the input vector file
    :param vec_lyr: is a string specifying the layer within the input vector file
    :param inputImage: is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation
    :param outputImage: is a string specifying the output KEA image for the rasterised shapefile

    Example::

        from rsgislib import vectorutils

        inputVector = 'crowns.shp'
        inputImage = 'injune_p142_casi_sub_utm.kea'
        outputImage = 'psu142_crowns.kea'

        vectorutils.copyVec2RAT(inputVector, 'crowns', inputImage, outputImage)

    """
    import rsgislib.rastergis
    rasteriseVecLyr(vec_file, vec_lyr, inputImage, outputImage, gdalformat="KEA",
                    datatype=rsgislib.TYPE_32UINT, vecAtt="FID", vecExt=False,
                    thematic=True, nodata=0)
    rsgislib.rastergis.importVecAtts(outputImage, vec_file, vec_lyr, 'pxlval', None)



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
            rsgislib.vectorutils.deleteVectorFile(vec_out_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vec_out_file))

    if out_epsg_code is None:
        out_epsg_code = rsgislib.imageutils.getEPSGProjFromImage(input_img)

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


def extractImageFootprint(inputImg, outVec, tmpDIR='./tmp', rePrjTo=None):
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

    uidStr = rsgislib.tools.utils.uidGenerator()

    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True

    inImgBase = os.path.splitext(os.path.basename(inputImg))[0]

    validOutImg = os.path.join(tmpDIR, inImgBase + '_' + uidStr + '_validimg.kea')
    inImgNoData = rsgislib.imageutils.getImageNoDataValue(inputImg)
    rsgislib.imageutils.genValidMask(inimages=inputImg, outimage=validOutImg,
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
    rsgislib.vectorutils.writeVecColumn(outVecTmpFile, vecLayerName, 'FileName',
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

