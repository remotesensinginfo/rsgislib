#!/usr/bin/env python
"""
The vector utils module performs geometry / attribute table operations on vectors.
"""
from __future__ import print_function

# import the C++ extension into this level
from ._vectorutils import *

import os.path
import os
import sys
import shutil
import subprocess
import warnings
import math

import osgeo.gdal as gdal
import osgeo.osr as osr
import osgeo.ogr as ogr

# Import the RSGISLib module
import rsgislib

# Import the RSGISLib Image Utils module
import rsgislib.imageutils

# Import the RSGISLib RasterGIS module
import rsgislib.rastergis 

class VecLayersInfoObj(object):
    """
    This is a class to store the information associated within the rsgislib.vectorutils.merge_to_multi_layer_vec function.

    :param vecfile: input vector file.
    :param veclyr: input vector layer name
    :param outlyr: output vector layer name

    """

    def __init__(self, vecfile=None, veclyr=None, outlyr=None):
        """
        :param vecfile: input vector file.
        :param veclyr: input vector layer name
        :param outlyr: output vector layer name

        """
        self.vecfile = vecfile
        self.veclyr = veclyr
        self.outlyr = outlyr

def delete_vector_file(vec_file, feedback=True):
    from osgeo import gdal
    import os
    ds_in_vec = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if ds_in_vec is None:
        raise Exception("Could not open '{}'".format(vec_file))
    file_lst = ds_in_vec.GetFileList()
    for cfile in file_lst:
        if feedback:
            print("Deleting: {}".format(cfile))
        os.remove(cfile)

def rasterise2Image(inputVec, inputImage, outImage, gdalformat="KEA", burnVal=1, shpAtt=None, shpExt=False):
    """ 
*** Deprecated *** A utillity to rasterise a shapefile into an image covering the same region and at the same resolution as the input image. 

Where:

:param inputVec: is a string specifying the input vector (shapefile) file
:param inputImage: is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation (if None and shpExt is False them assumes output image already exists and just uses it as is burning vector into it)
:param outImage: is a string specifying the output image for the rasterised shapefile
:param gdalformat: is the output image format (Default: KEA).
:param burnVal: is the value for the output image pixels if no attribute is provided.
:param shpAtt: is a string specifying the attribute to be rasterised, value of None creates a binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.
:param shpExt: is a boolean specifying that the output image should be cut to the same extent as the input shapefile (Default is False and therefore output image will be the same as the input).

Example::

    from rsgislib import vectorutils
    
    inputVector = 'crowns.shp'
    inputImage = 'injune_p142_casi_sub_utm.kea'
    outputImage = 'psu142_crowns.kea'  
    vectorutils.rasterise2Image(inputVector, inputImage, outputImage, 'KEA', shpAtt='FID')

"""
    warnings.warn("Call to deprecated function rsgislib.vectorutils.rasterise2Image, use rsgislib.vectorutils.rasteriseVecLyr.", category=DeprecationWarning, stacklevel=2)
    try:
        gdal.UseExceptions()
        
        if shpExt:
            print("Creating output image from shapefile extent")
            rsgislib.imageutils.createCopyImageVecExtent(inputImage, inputVec, outImage, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
        elif inputImage is None:
            print("Assuming output image is already created so just using.")
        else:
            print("Creating output image using input image")
            rsgislib.imageutils.createCopyImage(inputImage, outImage, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
        
        if shpAtt == "FID":   
            tmpVector = os.path.splitext(inputVec)[0] + "_tmpFIDFile.shp"
            print("Added FID Column...")
            addFIDColumn(inputVec, tmpVector, True)
        else:
            tmpVector = inputVec
        
        print("Running Rasterise now...")
        outRasterDS = gdal.Open(outImage, gdal.GA_Update)
        if outRasterDS is None:
            raise Exception("Could not open '" + outImage + "'")
        
        inVectorDS = ogr.Open(tmpVector)
        if inVectorDS is None:
            raise Exception("Could not open '" + tmpVector + "'")

        inVectorLayer = inVectorDS.GetLayer(0)
        if inVectorLayer is None:
            raise Exception("Could not find layer in vector file.")
        
        # Run the algorithm.
        err = 0
        if shpAtt is None:
            err = gdal.RasterizeLayer(outRasterDS, [1], inVectorLayer, burn_values=[burnVal])
        else:
            err = gdal.RasterizeLayer(outRasterDS, [1], inVectorLayer, options=["ATTRIBUTE="+shpAtt])
        if err != 0:
            raise Exception("Rasterisation Error: " + str(err))
        
        outRasterDS = None
        inVectorDS = None
        
        if shpAtt == "FID":
            driver = ogr.GetDriverByName("ESRI Shapefile")
            if os.path.exists(tmpVector):
                driver.DeleteDataSource(tmpVector)
        
        print("Adding Colour Table")
        rsgislib.rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
        print("Completed")
    except Exception as e:
        raise e


def rasteriseVecLyr(inputVec, inputVecLyr, inputImage, outImage, gdalformat="KEA", burnVal=1, datatype=rsgislib.TYPE_8UINT, vecAtt=None, vecExt=False, thematic=True, nodata=0):
    """ 
A utillity to rasterise a vector layer to an image covering the same region and at the same resolution as the input image. 

Where:

:param inputVec: is a string specifying the input vector file
:param inputVecLyr: is a string specifying the input vector layer name.
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
    try:
        gdal.UseExceptions()
        
        if vecExt:
            print("Creating output image from shapefile extent")
            rsgisUtils = rsgislib.RSGISPyUtils()
            xRes, yRes = rsgisUtils.getImageRes(inputImage)
            if yRes < -1:
                yRes = yRes * (-1)
            outRes = xRes
            if xRes > yRes:
                outRes = yRes
            
            rsgislib.imageutils.createCopyImageVecExtentSnap2Grid(inputVec, inputVecLyr, outImage, outRes, 1, gdalformat, datatype)
        elif inputImage is None:
            print("Assuming output image is already created so just using.")
        else:
            print("Creating output image using input image")
            rsgislib.imageutils.createCopyImage(inputImage, outImage, 1, 0, gdalformat, datatype)
        
        print("Running Rasterise now...")
        out_img_ds = gdal.Open(outImage, gdal.GA_Update)
        if out_img_ds is None:
            raise Exception("Could not open '" + outImage + "'")
            
        vec_ds = gdal.OpenEx(inputVec, gdal.OF_VECTOR)
        if vec_ds is None:
            raise Exception("Could not open '" + inputVec + "'")
        
        vec_lyr = vec_ds.GetLayerByName( inputVecLyr )
        if vec_lyr is None:
            raise Exception("Could not find layer '" + inputVecLyr + "'")
        
        # Run the algorithm.
        err = 0
        if vecAtt is None:
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr, burn_values=[burnVal])
        else:
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr, options=["ATTRIBUTE="+vecAtt])
        if err != 0:
            raise Exception("Rasterisation Error: " + str(err))
        
        out_img_ds = None
        vec_ds = None
        
        if thematic:
            print("Adding Colour Table")
            rsgislib.rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
        else:
            rsgislib.imageutils.popImageStats(outImage, usenodataval=True, nodataval=nodata, calcpyramids=True)
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
        gdal.UseExceptions()
        
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
            err = gdal.RasterizeLayer(out_img_ds, [1], vec_lyr_obj, options=["ATTRIBUTE="+vecAtt])
        if err != 0:
            raise Exception("Rasterisation Error: {}".format(err))
        
        out_img_ds = None
                
        if calcstats:
            if thematic:
                rsgislib.rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
            else:
                rsgislib.imageutils.popImageStats(outImage, usenodataval=True, nodataval=nodata, calcpyramids=True)
    except Exception as e:
        print('Failed rasterising: {}'.format(outImage))
        raise e


def copyVec2RAT(vecfile, vecLyr, inputImage, outputImage):
    """ 
A utillity to create raster copy of a shapefile. The output image is a KEA file and the attribute table has the attributes from the shapefile. 
    
Where:

:param inputVec: is a string specifying the input vector (shapefile) file
:param inputImage: is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation
:param outputImage: is a string specifying the output KEA image for the rasterised shapefile

Example::

    from rsgislib import vectorutils
     
    inputVector = 'crowns.shp'
    inputImage = 'injune_p142_casi_sub_utm.kea'
    outputImage = 'psu142_crowns.kea'
        
    vectorutils.copyVec2RAT(inputVector, 'crowns', inputImage, outputImage)

"""
    try:
        rasteriseVecLyr(vecfile, vecLyr, inputImage, outputImage, gdalformat="KEA", datatype=rsgislib.TYPE_32UINT, vecAtt="FID", vecExt=False, thematic=True, nodata=0)
        rsgislib.rastergis.importVecAtts(outputImage, vecfile, vecLyr, 'pxlval', None)
    except Exception as e:
        raise e


def polygoniseRaster(inputImg, outShp, imgBandNo=1, maskImg=None, imgMaskBandNo=1 ):
    """ 
A utillity to polygonise a raster to a ESRI Shapefile. 
    
Where:

:param inputImg: is a string specifying the input image file to be polygonised
:param outShp: is a string specifying the output shapefile path. If it exists it will be deleted and overwritten.
:param imgBandNo: is an int specifying the image band to be polygonised. (default = 1)
:param maskImg: is an optional string mask file specifying a no data mask (default = None)
:param imgMaskBandNo: is an int specifying the image band to be used the mask (default = 1)

"""
    gdal.UseExceptions()
    
    gdalImgData = gdal.Open(inputImg)
    imgBand = gdalImgData.GetRasterBand(imgBandNo)
    imgsrs = osr.SpatialReference()
    imgsrs.ImportFromWkt(gdalImgData.GetProjectionRef())
    
    gdalImgMaskData = None
    imgMaskBand = None
    if maskImg is not None:
        gdalImgMaskData = gdal.Open(maskImg)
        imgMaskBand = gdalImgData.GetRasterBand(imgMaskBandNo)

    
    driver = ogr.GetDriverByName("ESRI Shapefile")
    if os.path.exists(outShp):
        driver.DeleteDataSource(outShp)
    outDatasource = driver.CreateDataSource(outShp)
    
    layerName = os.path.splitext(os.path.basename(outShp))[0]
    outLayer = outDatasource.CreateLayer(layerName, srs=imgsrs)
    
    newField = ogr.FieldDefn('PXLVAL', ogr.OFTInteger)
    outLayer.CreateField(newField)
    dstFieldIdx = outLayer.GetLayerDefn().GetFieldIndex('PXLVAL')

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    
    print("Polygonising...")
    gdal.Polygonize(imgBand, imgMaskBand, outLayer, dstFieldIdx, [], callback=callback )
    print("Completed")
    outLayer.SyncToDisk()
    outDatasource = None
    gdalImgData = None
    if maskImg is not None:
        gdalImgMaskData = None


def polygoniseRaster2VecLyr(outvec, outlyr, vecdrv, inputImg, imgBandNo=1, maskImg=None, imgMaskBandNo=1,
                            replace_file=True, replace_lyr=True, pxl_val_fieldname='PXLVAL'):
    """ 
A utility to polygonise a raster to a OGR vector layer.

Where:

:param outvec: is a string specifying the output vector file path. If it exists it will be deleted and overwritten.
:param outlyr: is a string with the name of the vector layer.
:param vecdrv: is a string with the driver
:param inputImg: is a string specifying the input image file to be polygonised
:param imgBandNo: is an int specifying the image band to be polygonised. (default = 1)
:param maskImg: is an optional string mask file specifying a no data mask (default = None)
:param imgMaskBandNo: is an int specifying the image band to be used the mask (default = 1)
:param replace_file: is a boolean specifying whether the vector file should be replaced (i.e., overwritten). Default=True.
:param replace_lyr: is a boolean specifying whether the vector layer should be replaced (i.e., overwritten). Default=True.
:param pxl_val_fieldname: is a string to specify the name of the output column representing the pixel value within the input image.

"""
    gdal.UseExceptions()

    gdalImgDS = gdal.Open(inputImg)
    imgBand = gdalImgDS.GetRasterBand(imgBandNo)
    imgsrs = osr.SpatialReference()
    imgsrs.ImportFromWkt(gdalImgDS.GetProjectionRef())

    gdalImgMaskDS = None
    imgMaskBand = None
    if maskImg is not None:
        gdalImgMaskDS = gdal.Open(maskImg)
        imgMaskBand = gdalImgMaskDS.GetRasterBand(imgMaskBandNo)

    if os.path.exists(outvec) and (not replace_file):
        vecDS = gdal.OpenEx(outvec, gdal.GA_Update)
    else:
        outdriver = ogr.GetDriverByName(vecdrv)
        if os.path.exists(vecdrv):
            outdriver.DeleteDataSource(vecdrv)
        vecDS = outdriver.CreateDataSource(outvec)

    if vecDS is None:
        raise Exception("Could not open or create '{}'".format(outvec))

    lcl_options = []
    if replace_lyr:
        lcl_options = ['OVERWRITE=YES']

    outLayer = vecDS.CreateLayer(outlyr, srs=imgsrs, options=lcl_options)

    newField = ogr.FieldDefn(pxl_val_fieldname, ogr.OFTInteger)
    outLayer.CreateField(newField)
    dstFieldIdx = outLayer.GetLayerDefn().GetFieldIndex(pxl_val_fieldname)

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    print("Polygonising...")
    gdal.Polygonize(imgBand, imgMaskBand, outLayer, dstFieldIdx, [], callback=callback )
    print("Completed")
    outLayer.SyncToDisk()
    vecDS = None
    gdalImgDS = None
    if maskImg is not None:
        gdalImgMaskDS = None


def writeVecColumn(vectorFile, vectorLayer, colName, colDataType, colData):
    """
A function which will write a column to a vector file

Where:

:param vectorFile: The file / path to the vector data 'file'.
:param vectorLayer: The layer to which the data is to be added.
:param colName: Name of the output column
:param colDataType: ogr data type (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
:param colData: A list of the same length as the number of features in vector file.

Example::

    from rsgislib import vectorutils
    import rsgislib
    import osgeo.ogr as ogr
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    requiredScenes = rsgisUtils.readTextFile2List("GMW_JERS-1_ScenesRequired.txt")
    requiredScenesShp = "JERS-1_Scenes_Requred_shp"
    vectorutils.writeVecColumn(requiredScenesShp+'.shp', requiredScenesShp, 'ScnName', ogr.OFTString, requiredScenes)

"""
    gdal.UseExceptions()
    
    ds = gdal.OpenEx(vectorFile, gdal.OF_UPDATE )
    if ds is None:
        raise Exception("Could not open '" + vectorFile + "'")

    lyr = ds.GetLayerByName( vectorLayer )
    if lyr is None:
        raise Exception("Could not find layer '" + vectorLayer + "'")

    numFeats = lyr.GetFeatureCount()
    if not len(colData) == numFeats:
        print("Number of Features: " + str(numFeats))
        print("Length of Data: " + str(len(colData)))
        raise Exception( "The number of features and size of the input data is not equal." )

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range( lyrDefn.GetFieldCount() ):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == colName.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn( colName, colDataType )
        if lyr.CreateField ( field_defn ) != 0:
            raise Exception("Creating '" + colName + "' field failed; becareful with case, some drivers are case insensitive but column might not be found.")

    lyr.ResetReading()
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids=[]
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
                feat.SetField(colName, colData[i])
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
            print("Data type of the value being written is '{}'".format(type(colData[i])))
        raise e
    
def writeVecColumn2Layer(lyr, colName, colDataType, colData):
    """
A function which will write a column to a vector layer.

Where:

:param lyr: GDAL/OGR vector layer object
:param colName: Name of the output column
:param colDataType: ogr data type (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
:param colData: A list of the same length as the number of features in vector file.

"""
    gdal.UseExceptions()
    
    if lyr is None:
        raise Exception("The layer passed in is None...")
    
    numFeats = lyr.GetFeatureCount()
    if not len(colData) == numFeats:
        print("Number of Features: " + str(numFeats))
        print("Length of Data: " + str(len(colData)))
        raise Exception( "The number of features and size of the input data is not equal." )

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range( lyrDefn.GetFieldCount() ):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == colName.lower():
            colExists = True
            break

    if not colExists:
        field_defn = ogr.FieldDefn( colName, colDataType )
        if lyr.CreateField ( field_defn ) != 0:
            raise Exception("Creating '" + colName + "' field failed; becareful with case, some drivers are case insensitive but column might not be found.\n")
    
    lyr.ResetReading()
    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids=[]
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
            feat.SetField(colName, colData[i])
            lyr.SetFeature(feat)
        if ((i % 20000) == 0) and openTransaction:
            lyr.CommitTransaction()
            openTransaction = False
        i = i + 1
    if openTransaction:
        lyr.CommitTransaction()
        openTransaction = False


def readVecColumn(vectorFile, vectorLayer, colName):
    """
A function which will reads a column from a vector file

Where:

:param vectorFile: The file / path to the vector data 'file'.
:param vectorLayer: The layer to which the data is to be read from.
:param colName: Name of the input column

"""
    gdal.UseExceptions()
    
    ds = gdal.OpenEx(vectorFile, gdal.OF_VECTOR )
    if ds is None:
        raise Exception("Could not open '" + vectorFile + "'")
    
    lyr = ds.GetLayerByName( vectorLayer )
    if lyr is None:
        raise Exception("Could not find layer '" + vectorLayer + "'")

    colExists = False
    lyrDefn = lyr.GetLayerDefn()
    for i in range( lyrDefn.GetFieldCount() ):
        if lyrDefn.GetFieldDefn(i).GetName() == colName:
            colExists = True
            break
    
    if not colExists:
        ds = None
        raise Exception("The specified column does not exist in the input layer; check case as some drivers are case sensitive.")
    
    outVal = list()
    lyr.ResetReading()
    for feat in lyr:
        outVal.append(feat.GetField(colName))
    ds = None
    
    return outVal


def readVecColumns(vectorFile, vectorLayer, attNames):
    """
A function which will reads a column from a vector file

Where:

:param vectorFile: The file / path to the vector data 'file'.
:param vectorLayer: The layer to which the data is to be read from.
:param attNames: List of input attribute column names to be read in.

"""
    gdal.UseExceptions()
    
    ds = gdal.OpenEx(vectorFile, gdal.OF_VECTOR )
    if ds is None:
        raise Exception("Could not open '{}'".format(vectorFile))
    
    lyr = ds.GetLayerByName( vectorLayer )
    if lyr is None:
        raise Exception("Could not find layer '{}'".format(vectorLayer))
    
    lyrDefn = lyr.GetLayerDefn()
    
    feat_idxs = dict()
    feat_types= dict()
    found_atts = dict()
    for attName in attNames:
        found_atts[attName] = False
    
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName() in attNames:
            attName = lyrDefn.GetFieldDefn(i).GetName()
            feat_idxs[attName] = i
            feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
            found_atts[attName] = True
            
    for attName in attNames:
        if not found_atts[attName]:
            ds = None
            raise Exception("Could not find the attribute ({}) specified within the vector layer.".format(attName))
    
    outvals = []
    lyr.ResetReading()
    for feat in lyr:
        outdict = dict()
        for attName in attNames:
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


def popBBOXCols(vecFile, vecLyr, xminCol='xmin', xmaxCol='xmax', yminCol='ymin', ymaxCol='ymax'):
    """
A function which adds a polygons boundary bbox as attributes to each feature.

:param vecFile: vector file.
:param vecLyr: layer within the vector file.
:param xminCol: column name.
:param xmaxCol: column name.
:param yminCol: column name.
:param ymaxCol: column name.

"""
    dsVecFile = gdal.OpenEx(vecFile, gdal.OF_UPDATE )
    if dsVecFile is None:
        raise Exception("Could not open '" + vecFile + "'")
        
    lyrVecObj = dsVecFile.GetLayerByName( vecLyr )
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vecLyr + "'")
    
    xminCol_exists = False
    xmaxCol_exists = False
    yminCol_exists = False
    ymaxCol_exists = False
    
    lyrDefn = lyrVecObj.GetLayerDefn()
    for i in range( lyrDefn.GetFieldCount() ):
        if lyrDefn.GetFieldDefn(i).GetName() == xminCol:
            xminCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == xmaxCol:
            xmaxCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == yminCol:
            yminCol_exists = True
        if lyrDefn.GetFieldDefn(i).GetName() == ymaxCol:
            ymaxCol_exists = True
    
    if not xminCol_exists:
        xmin_field_defn = ogr.FieldDefn( xminCol, ogr.OFTReal )
        if lyrVecObj.CreateField ( xmin_field_defn ) != 0:
            raise Exception("Creating '{}' field failed.".format(xminCol))
            
    if not xmaxCol_exists:
        xmax_field_defn = ogr.FieldDefn( xmaxCol, ogr.OFTReal )
        if lyrVecObj.CreateField ( xmax_field_defn ) != 0:
            raise Exception("Creating '{}' field failed.".format(xmaxCol))
    
    if not yminCol_exists:
        ymin_field_defn = ogr.FieldDefn( yminCol, ogr.OFTReal )
        if lyrVecObj.CreateField ( ymin_field_defn ) != 0:
            raise Exception("Creating '{}' field failed.".format(yminCol))
            
    if not ymaxCol_exists:
        ymax_field_defn = ogr.FieldDefn( ymaxCol, ogr.OFTReal )
        if lyrVecObj.CreateField ( ymax_field_defn ) != 0:
            raise Exception("Creating '{}' field failed.".format(ymaxCol))

    # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
    fids=[]
    for feat in lyrVecObj:
        fids.append(feat.GetFID())
    
    openTransaction = False
    nFeats = lyrVecObj.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    lyrVecObj.ResetReading()
    for fid in fids:
        # WORK AROUND AS SQLITE GETS STUCK IN LOOP ON FIRST FEATURE WHEN USE SETFEATURE.
        feat = lyrVecObj.GetFeature(fid)
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
        
        if not openTransaction:
            lyrVecObj.StartTransaction()
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
        rtn_val = lyrVecObj.SetFeature(feat)
        if rtn_val != ogr.OGRERR_NONE:
            raise Exception("An error has occurred setting a feature on a layer.")
        if ((counter % 20000) == 0) and openTransaction:
            lyrVecObj.CommitTransaction()
            openTransaction = False
        counter = counter + 1
    if openTransaction:
        lyrVecObj.CommitTransaction()
        openTransaction = False
    lyrVecObj.SyncToDisk()
    dsVecFile = None
    print(" Completed")


def extractImageFootprint(inputImg, outVec, tmpDIR='./tmp', rePrjTo=None):
    """
A function to extract an image footprint as a vector.

:param inputImg: the input image file for which the footprint will be extracted.
:param outVec: output shapefile path and name.
:param tmpDIR: temp directory which will be used during processing. It will be created and deleted once processing complete.
:param rePrjTo: optional command

"""
    gdal.UseExceptions()

    rsgisUtils = rsgislib.RSGISPyUtils()
    
    uidStr = rsgisUtils.uidGenerator()
    
    createdTmp = False
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
        createdTmp = True
    
    inImgBase = os.path.splitext(os.path.basename(inputImg))[0]
    
    validOutImg = os.path.join(tmpDIR, inImgBase+'_'+uidStr+'_validimg.kea')
    inImgNoData = rsgisUtils.getImageNoDataValue(inputImg)
    rsgislib.imageutils.genValidMask(inimages=inputImg, outimage=validOutImg, gdalformat='KEA', nodata=inImgNoData)
    
    outVecTmpFile = outVec
    if not (rePrjTo is None):
        outVecTmpFile = os.path.join(tmpDIR, inImgBase+'_'+uidStr+'_initVecOut.shp')
    
    rsgislib.vectorutils.polygoniseRaster(validOutImg, outVecTmpFile, imgBandNo=1, maskImg=validOutImg, imgMaskBandNo=1)
    vecLayerName = os.path.splitext(os.path.basename(outVecTmpFile))[0]
    ds = gdal.OpenEx(outVecTmpFile, gdal.OF_READONLY )
    if ds is None:
        raise Exception("Could not open '" + outVecTmpFile + "'")
    
    lyr = ds.GetLayerByName( vecLayerName )
    if lyr is None:
        raise Exception("Could not find layer '" + vecLayerName + "'")
    numFeats = lyr.GetFeatureCount()
    lyr = None
    ds = None
    
    fileName = []
    for i in range(numFeats):
        fileName.append(os.path.basename(inputImg))
    rsgislib.vectorutils.writeVecColumn(outVecTmpFile, vecLayerName, 'FileName', ogr.OFTString, fileName)
    
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


def getVecFeatCount(inVec, layerName=None, computeCount=True):
    """
Get a count of the number of features in the vector layers.

:param inVec: is a string with the input vector file name and path.
:param layerName: is the layer for which extent is to be calculated (Default: None). if None assume there is only one layer and that will be read.
:param computeCount: is a boolean which specifies whether the layer extent
                     should be calculated (rather than estimated from header)
                     even if that operation is computationally expensive.

:return: nfeats

"""
    gdal.UseExceptions()
    inDataSource = gdal.OpenEx(inVec, gdal.OF_VECTOR )
    if layerName is not None:
        inLayer = inDataSource.GetLayer(layerName)
    else:
        inLayer = inDataSource.GetLayer()
    if inLayer is None:
        raise Exception("Check layer name as did not open layer.")
    nFeats = inLayer.GetFeatureCount(computeCount)
    return nFeats


def mergeShapefiles(inFileList, outVecFile):
    """
Function which will merge a list of shapefiles into an single shapefile using ogr2ogr.

Where:

:param inFileList: is a list of input files.
:param outVecFile: is the output shapefile

"""
    if os.path.exists(outVecFile):
        driver = ogr.GetDriverByName('ESRI Shapefile')
        driver.DeleteDataSource(outVecFile)
    first = True
    for inFile in inFileList:
        nFeat = getVecFeatCount(inFile)
        print("Processing: " + inFile + " has " + str(nFeat) + " features.")
        if nFeat > 0:
            if first:
                cmd = 'ogr2ogr -f "ESRI Shapefile"  "' + outVecFile + '" "' + inFile + '"'
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)
                first = False
            else:
                cmd = 'ogr2ogr -update -append -f "ESRI Shapefile" "' + outVecFile + '" "' + inFile + '"'
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)


def mergeVectors2SQLiteDB(inFileList, outDBFile, lyrName, exists):
    """
Function which will merge a list of vector files into an single output SQLite database using ogr2ogr.

Where:

:param inFileList: is a list of input files.
:param outDBFile: is the output SQLite database (\*.sqlite)
:param lyrName: is the layer name in the output database (i.e., you can merge layers into single layer or write a number of layers to the same database).
:param exists: boolean which specifies whether the database file exists or not.

"""
    first = True
    for inFile in inFileList:
        nFeat = getVecFeatCount(inFile)
        print("Processing: " + inFile + " has " + str(nFeat) + " features.")
        if nFeat > 0:
            if first:
                if not exists:
                    cmd = 'ogr2ogr -f "SQLite" -lco COMPRESS_GEOM=YES -lco SPATIAL_INDEX=YES -nln '+lyrName+' "' + outDBFile + '" "' + inFile + '"'
                    try:
                        subprocess.check_call(cmd, shell=True)
                    except OSError as e:
                        raise Exception('Error running ogr2ogr: ' + cmd)
                else:
                    cmd = 'ogr2ogr -update -f "SQLite" -lco COMPRESS_GEOM=YES -lco SPATIAL_INDEX=YES -nln '+lyrName+' "' + outDBFile + '" "' + inFile + '"'
                    try:
                        subprocess.check_call(cmd, shell=True)
                    except OSError as e:
                        raise Exception('Error running ogr2ogr: ' + cmd)
                first = False
            else:
                cmd = 'ogr2ogr -update -append -f "SQLite" -nln '+lyrName+' "' + outDBFile + '" "' + inFile + '"'
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)


def mergeVectors2GPKG(inFileList, outFile, lyrName, exists):
    """
Function which will merge a list of vector files into an single output GeoPackage (GPKG) file using ogr2ogr.

Where:

:param inFileList: is a list of input files.
:param outFile: is the output GPKG database (\*.gpkg)
:param lyrName: is the layer name in the output database (i.e., you can merge layers into single layer or write a number of layers to the same database).
:param exists: boolean which specifies whether the database file exists or not.

"""
    first = True
    for inFile in inFileList:
        nFeat = getVecFeatCount(inFile)
        print("Processing: " + inFile + " has " + str(nFeat) + " features.")
        if nFeat > 0:
            if first:
                if not exists:
                    cmd = 'ogr2ogr -f "GPKG" -lco SPATIAL_INDEX=YES -nln '+lyrName+' "' + outFile + '" "' + inFile + '"'
                    try:
                        subprocess.check_call(cmd, shell=True)
                    except OSError as e:
                        raise Exception('Error running ogr2ogr: ' + cmd)
                else:
                    cmd = 'ogr2ogr -update -f "GPKG" -lco SPATIAL_INDEX=YES -nln '+lyrName+' "' + outFile + '" "' + inFile + '"'
                    try:
                        subprocess.check_call(cmd, shell=True)
                    except OSError as e:
                        raise Exception('Error running ogr2ogr: ' + cmd)
                first = False
            else:
                cmd = 'ogr2ogr -update -append -f "GPKG" -nln '+lyrName+' "' + outFile + '" "' + inFile + '"'
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)


def mergeVectorLyrs2GPKG(inputFile, outFile, lyrName, exists):
    """
Function which will merge all the layers in the input vector file into an single output GeoPackage (GPKG)
file using ogr2ogr.

Where:

:param inputFile: is a vector file which contains multiple layers which are to be merged
:param outFile: is the output GPKG database (\*.gpkg)
:param lyrName: is the layer name in the output database (i.e., you can merge layers into single layer or write a number of layers to the same database).
:param exists: boolean which specifies whether the database file exists or not.

"""
    lyrs = rsgislib.vectorutils.getVecLyrsLst(inputFile)
    first = True
    for lyr in lyrs:
        nFeat = rsgislib.vectorutils.getVecFeatCount(inputFile, lyr)
        print("Processing: " + lyr + " has " + str(nFeat) + " features.")
        if nFeat > 0:
            if first:
                if not exists:
                    cmd = 'ogr2ogr -f "GPKG" -lco SPATIAL_INDEX=YES -nln '+lyrName+' "' + outFile + '" "' + inputFile + '" "' + lyr + '"'
                    try:
                        subprocess.check_call(cmd, shell=True)
                    except OSError as e:
                        raise Exception('Error running ogr2ogr: ' + cmd)
                else:
                    cmd = 'ogr2ogr -update -f "GPKG" -lco SPATIAL_INDEX=YES -nln '+lyrName+' "' + outFile + '" "' + inputFile + '" "' + lyr + '"'
                    try:
                        subprocess.check_call(cmd, shell=True)
                    except OSError as e:
                        raise Exception('Error running ogr2ogr: ' + cmd)
                first = False
            else:
                cmd = 'ogr2ogr -update -append -f "GPKG" -nln '+lyrName+' "' + outFile + '" "' + inputFile + '" "' + lyr + '"'
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)


def mergeVectors2SQLiteDBIndLyrs(inFileList, outDBFile):
    """
Function which will merge a list of vector files into an single output SQLite database where each input 
file forms a new layer using the existing layer name. This function wraps the ogr2ogr command.

Where:

:param inFileList: is a list of input files.
:param outDBFile: is the output SQLite database (\*.sqlite)

"""
    for inFile in inFileList:
        inlyrs = getVecLyrsLst(inFile)
        print("Processing File: {0} has {1} layers to copy.".format(inFile, len(inlyrs)))
        for lyr in inlyrs:
            nFeat = getVecFeatCount(inFile, lyr)
            print("Processing Layer: {0} has {1} features to copy.".format(lyr, nFeat))
            if nFeat > 0:
                cmd = 'ogr2ogr -overwrite -f "SQLite" -lco COMPRESS_GEOM=YES -lco SPATIAL_INDEX=YES -nln {0} "{1}" "{2}" {0}'.format(lyr, outDBFile, inFile)
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)


def mergeVectors2GPKGIndLyrs(inFileList, outFile, rename_dup_lyrs=False, geom_type=None):
    """
Function which will merge a list of vector files into an single output GPKG file where each input
file forms a new layer using the existing layer name. This function wraps the ogr2ogr command.

Where:

:param inFileList: is a list of input files.
:param outFile: is the output GPKG database (\*.gpkg)
:param rename_dup_lyrs: If False an exception will be throw if any input layers has the same name.
                        If True a layer will be renamed - with a random set of letters/numbers on the end.
:param geom_type: Force the output vector to have a particular geometry type (e.g., 'POLYGON'). Same options as ogr2ogr.

"""
    rsgis_utils = rsgislib.RSGISPyUtils()
    out_lyr_names = []

    out_geom_type = ''
    if geom_type is not None:
        out_geom_type = ' -nlt {} '.format(geom_type)

    for inFile in inFileList:
        inlyrs = rsgislib.vectorutils.getVecLyrsLst(inFile)
        print("Processing File: {0} has {1} layers to copy.".format(inFile, len(inlyrs)))
        for lyr in inlyrs:
            nFeat = rsgislib.vectorutils.getVecFeatCount(inFile, lyr)
            out_lyr = lyr
            if lyr in out_lyr_names:
                if rename_dup_lyrs:
                    out_lyr = '{}_{}'.format(lyr, rsgis_utils.uidGenerator())
                else:
                    raise Exception("Input files have layers with the same name, these will be over written.")
            print("Processing Layer: {0} has {1} features to copy - output layer name: {2}".format(lyr, nFeat, out_lyr))
            if nFeat > 0:
                cmd = 'ogr2ogr -overwrite -f "GPKG" {4} -lco SPATIAL_INDEX=YES -nln {0} "{1}" "{2}" {3}'.format(out_lyr,
                                                                                                                outFile,
                                                                                                                inFile,
                                                                                                                lyr,
                                                                                                                out_geom_type)
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)
                out_lyr_names.append(out_lyr)


def createPolySHP4LstBBOXs(csvFile, outSHP, epsgCode, minXCol=0, maxXCol=1, minYCol=2, maxYCol=3, ignoreRows=0, force=False):
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
:param force: If the output file already exists delete it before proceeding.

"""
    gdal.UseExceptions()
    try:
        if os.path.exists(outSHP):
            if force:
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
        outLayer = outDataSource.CreateLayer(os.path.splitext(os.path.basename(outSHP))[0], srs, geom_type=ogr.wkbPolygon )
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


def getVecLyrsLst(vecFile):
    """
A function which returns a list of available layers within the inputted vector file.

:param vecFile: file name and path to input vector layer.

:return: list of layer names (can be used with gdal.Dataset.GetLayerByName()).

"""
    gdalDataset = gdal.OpenEx(vecFile, gdal.OF_VECTOR )
    layerList = []
    for lyr_idx in range(gdalDataset.GetLayerCount()):
        lyr = gdalDataset.GetLayerByIndex(lyr_idx)
        tLyrName = lyr.GetName()
        if not tLyrName in layerList:
            layerList.append(tLyrName)
    gdalDataset = None
    return layerList


def getVecLayerExtent(inVec, layerName=None, computeIfExp=True):
    """
Get the extent of the vector layer.

:param inVec: is a string with the input vector file name and path.
:param layerName: is the layer for which extent is to be calculated (Default: None)
                  if None assume there is only one layer and that will be read.
:param computeIfExp: is a boolean which specifies whether the layer extent
                     should be calculated (rather than estimated from header)
                     even if that operation is computationally expensive.

:return: boundary box is returned (MinX, MaxX, MinY, MaxY)

"""
    gdal.UseExceptions()
    # Get a Layer's Extent
    inDataSource = gdal.OpenEx(inVec, gdal.OF_VECTOR )
    if layerName is not None:
        inLayer = inDataSource.GetLayer(layerName)
    else:
        inLayer = inDataSource.GetLayer()
    extent = inLayer.GetExtent(computeIfExp)
    return extent


def getProjWKTFromVec(inVec, layerName=None):
    """
:param inVec: is a string with the input vector file name and path.

:return: WKT representation of projection

"""
    gdal.UseExceptions()
    # Get shapefile projection as WKT
    dataset = gdal.OpenEx(inVec, gdal.OF_VECTOR )
    if layerName is not None:
        layer = dataset.GetLayer(layerName)
    else:
        layer = dataset.GetLayer()
    spatialRef = layer.GetSpatialRef()
    return spatialRef.ExportToWkt()


def splitVecLyr(vecFile, vecLyr, nfeats, outVecDrvr, outdir, outvecbase, outvecend):
    """
A function which splits the input vector layer into a number of output layers.

:param vecFile: input vector file.
:param vecLyr: input layer name.
:param nfeats: number of features within each output file.
:param outVecDrvr: output file driver.
:param outdir: output directory for the created output files.
:param outvecbase: output layer name will be the same as the base file name.
:param outvecend: file ending (e.g., .shp).

"""
    gdal.UseExceptions()
    datasrc = gdal.OpenEx(vecFile, gdal.OF_VECTOR )
    srcLyr = datasrc.GetLayer(vecLyr)
    nInFeats = srcLyr.GetFeatureCount(True)
    print(nInFeats)
    
    nOutFiles = math.floor(nInFeats/nfeats)
    remainFeats = nInFeats - (nOutFiles*nfeats)
    print(nOutFiles)
    print(remainFeats)
    
    out_driver = ogr.GetDriverByName(outVecDrvr)
    src_lyr_spat_ref = srcLyr.GetSpatialRef()
    
    cFeatN = 0
    sFeatN = 0
    eFeatN = nfeats
    for i in range(nOutFiles):
        outveclyr = "{0}{1}".format(outvecbase, i+1)
        outvecfile = os.path.join(outdir, "{0}{1}".format(outveclyr, outvecend))
        print("Creating: {}".format(outvecfile))
        result_ds = out_driver.CreateDataSource(outvecfile)
        result_lyr = result_ds.CreateLayer(outveclyr, src_lyr_spat_ref, geom_type=srcLyr.GetGeomType())
        
        srcLayerDefn = srcLyr.GetLayerDefn()
        for i in range(srcLayerDefn.GetFieldCount()):
            fieldDefn = srcLayerDefn.GetFieldDefn(i)
            result_lyr.CreateField(fieldDefn)
        rsltLayerDefn = result_lyr.GetLayerDefn()
        
        cFeatN = 0
        srcLyr.ResetReading()
        inFeat = srcLyr.GetNextFeature()
        result_lyr.StartTransaction()
        while inFeat:
            if (cFeatN >= sFeatN) and (cFeatN < eFeatN):
                geom = inFeat.GetGeometryRef()
                if geom is not None:
                    result_lyr.CreateFeature(inFeat)
            elif cFeatN > eFeatN:
                break
            inFeat = srcLyr.GetNextFeature()
            cFeatN = cFeatN + 1
        result_lyr.CommitTransaction()
        result_ds = None
        
        sFeatN = sFeatN + nfeats
        eFeatN = eFeatN + nfeats
    
    if remainFeats > 0:
        outveclyr = "{0}{1}".format(outvecbase, nOutFiles+1)
        outvecfile = os.path.join(outdir, "{0}{1}".format(outveclyr, outvecend))
        print("Creating: {}".format(outvecfile))
        result_ds = out_driver.CreateDataSource(outvecfile)
        result_lyr = result_ds.CreateLayer(outveclyr, src_lyr_spat_ref, geom_type=srcLyr.GetGeomType())

        srcLayerDefn = srcLyr.GetLayerDefn()
        for i in range(srcLayerDefn.GetFieldCount()):
            fieldDefn = srcLayerDefn.GetFieldDefn(i)
            result_lyr.CreateField(fieldDefn)
        rsltLayerDefn = result_lyr.GetLayerDefn()
        
        cFeatN = 0
        srcLyr.ResetReading()
        inFeat = srcLyr.GetNextFeature()
        result_lyr.StartTransaction()
        while inFeat:
            if (cFeatN >= sFeatN):
                geom = inFeat.GetGeometryRef()
                if geom is not None:
                    result_lyr.CreateFeature(inFeat)
            inFeat = srcLyr.GetNextFeature()
            cFeatN = cFeatN + 1
        result_lyr.CommitTransaction()
        result_ds = None
    datasrc = None


def reProjVectorLayer(inputVec, outputVec, outProjWKT, outDriverName='ESRI Shapefile', outLyrName=None,
                      inLyrName=None, inProjWKT=None, force=False):
    """
A function which reprojects a vector layer. You might also consider using rsgislib.vectorutils.vector_translate,
particularly if you are reprojecting the data and changing between coordinate units (e.g., degrees to meters)

:param inputVec: is a string with name and path to input vector file.
:param outputVec: is a string with name and path to output vector file.
:param outProjWKT: is a string with the WKT string for the output vector file.
:param outDriverName: is the output vector file format. Default is ESRI Shapefile.
:param outLyrName: is a string for the output layer name. If None then ignored and
                   assume there is just a single layer in the vector and layer name
                   is the same as the file name.
:param inLyrName: is a string for the input layer name. If None then ignored and
                  assume there is just a single layer in the vector.
:param inProjWKT: is a string with the WKT string for the input shapefile
                  (Optional; taken from input file if not specified).

"""
    ## This code has been editted and updated for GDAL > version 2.0
    ## https://pcjericks.github.io/py-gdalogr-cookbook/projection.html#reproject-a-layer
    gdal.UseExceptions()
        
    # get the input layer
    inDataSet = gdal.OpenEx(inputVec, gdal.OF_VECTOR )
    if inDataSet is None:
        raise("Failed to open input shapefile\n") 
    if inLyrName is None:   
        inLayer = inDataSet.GetLayer()
    else:
        inLayer = inDataSet.GetLayer(inLyrName)
    
    # input SpatialReference
    inSpatialRef = osr.SpatialReference()
    if inProjWKT is not None:
        inSpatialRef.ImportFromWkt(inProjWKT)
    else:
        inSpatialRef = inLayer.GetSpatialRef()
    
    # output SpatialReference
    outSpatialRef = osr.SpatialReference()
    outSpatialRef.ImportFromWkt(outProjWKT)
    
    # create the CoordinateTransformation
    coordTrans = osr.CoordinateTransformation(inSpatialRef, outSpatialRef)
    
    # Create shapefile driver
    driver = gdal.GetDriverByName( outDriverName )
    
    # create the output layer
    if os.path.exists(outputVec):
        if (outDriverName == 'ESRI Shapefile'):
            if force:
                driver.DeleteDataSource(outputVec)
            else:
                raise Exception('Output shapefile already exists - stopping.')
            outDataSet = driver.Create(outputVec, 0, 0, 0, gdal.GDT_Unknown )
        else:
            outDataSet = gdal.OpenEx(outputVec, gdal.OF_UPDATE )
    else:
        outDataSet = driver.Create(outputVec, 0, 0, 0, gdal.GDT_Unknown )
    
    if outLyrName is None:
        outLyrName = os.path.splitext(os.path.basename(outputVec))[0]
    outLayer = outDataSet.CreateLayer(outLyrName, outSpatialRef, inLayer.GetGeomType() )
    
    # add fields
    inLayerDefn = inLayer.GetLayerDefn()
    for i in range(0, inLayerDefn.GetFieldCount()):
        fieldDefn = inLayerDefn.GetFieldDefn(i)
        outLayer.CreateField(fieldDefn)
    
    # get the output layer's feature definition
    outLayerDefn = outLayer.GetLayerDefn()
    
    openTransaction = False
    nFeats = inLayer.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    
    # loop through the input features
    inFeature = inLayer.GetNextFeature()
    while inFeature:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
        
        if not openTransaction:
            outLayer.StartTransaction()
            openTransaction = True
    
        # get the input geometry
        geom = inFeature.GetGeometryRef()
        if geom is not None:
            # reproject the geometry
            geom.Transform(coordTrans)
            # create a new feature
            outFeature = ogr.Feature(outLayerDefn)
            # set the geometry and attribute
            outFeature.SetGeometry(geom)
            for i in range(0, outLayerDefn.GetFieldCount()):
                outFeature.SetField(outLayerDefn.GetFieldDefn(i).GetNameRef(), inFeature.GetField(i))
            # add the feature to the shapefile
            outLayer.CreateFeature(outFeature)
        # dereference the features and get the next input feature
        outFeature = None
        
        if ((counter % 20000) == 0) and openTransaction:
            outLayer.CommitTransaction()
            openTransaction = False
        
        inFeature = inLayer.GetNextFeature()
        counter = counter + 1
    
    if openTransaction:
        outLayer.CommitTransaction()
        openTransaction = False
    outLayer.SyncToDisk()
    print(" Completed")
    
    # Save and close the shapefiles
    inDataSet = None
    outDataSet = None


def reproj_vec_lyr(in_vec_lyr, out_vec_file, out_epsg, out_vec_drv='MEMORY', out_lyr_name=None, in_epsg=None,
                      print_feedback=True):
    """
A function which reprojects a vector layer. You might also consider using rsgislib.vectorutils.vector_translate,
particularly if you are reprojecting the data and changing between coordinate units (e.g., degrees to meters)

:param in_vec_lyr: is a GDAL vector layer object.
:param out_vec_file: is a string with name and path to output vector file - is created.
:param out_epsg: is an int with the EPSG code to which the input vector layer is to be reprojected to.
:param out_vec_drv: is the output vector file format. Default is MEMORY - i.e., nothing written to disk.
:param out_lyr_name: is a string for the output layer name. If None then ignored and
                   assume there is just a single layer in the vector and layer name
                   is the same as the file name.
:param inLyrName: is a string for the input layer name. If None then ignored and
                  assume there is just a single layer in the vector.
:param in_epsg: is an int with the EPSG code for the input vector file
                  (Optional; taken from input file if not specified).
:param print_feedback: is a boolean (Default True) specifying whether feedback should be printed to the console.
:return: Returns the output datasource and layer objects (result_ds, result_lyr). datasource needs to be set to None
         once you have finished using to free memory and if written to disk to ensure the whole dataset is written.

"""
    ## This code has been editted from https://pcjericks.github.io/py-gdalogr-cookbook/projection.html#reproject-a-layer
    ## Updated for GDAL 2.0
    gdal.UseExceptions()

    in_vec_lyr.ResetReading()

    # input SpatialReference
    in_spat_ref = osr.SpatialReference()
    if in_epsg is not None:
        in_spat_ref.ImportFromEPSG(in_epsg)
    else:
        in_spat_ref = in_vec_lyr.GetSpatialRef()

    # output SpatialReference
    out_spat_ref = osr.SpatialReference()
    out_spat_ref.ImportFromEPSG(out_epsg)

    # create the CoordinateTransformation
    coord_trans = osr.CoordinateTransformation(in_spat_ref, out_spat_ref)

    # Create shapefile driver
    driver = ogr.GetDriverByName(out_vec_drv)
    if driver == None:
        raise Exception("Driver has not be recognised.")

    # create the output layer
    result_ds = driver.CreateDataSource(out_vec_file)
    if result_ds == None:
        raise Exception("The output vector data source was not created: {}".format(out_vec_file))
    if out_lyr_name == None:
        out_lyr_name = os.path.splitext(os.path.basename(out_vec_file))[0]
    result_lyr = result_ds.CreateLayer(out_lyr_name, out_spat_ref, geom_type=in_vec_lyr.GetGeomType())

    # add fields
    in_vec_lyr_defn = in_vec_lyr.GetLayerDefn()
    for i in range(0, in_vec_lyr_defn.GetFieldCount()):
        fieldDefn = in_vec_lyr_defn.GetFieldDefn(i)
        result_lyr.CreateField(fieldDefn)

    # get the output layer's feature definition
    result_lyr_defn = result_lyr.GetLayerDefn()

    openTransaction = False
    nFeats = in_vec_lyr.GetFeatureCount(True)
    step = math.floor(nFeats / 10)
    feedback = 10
    feedback_next = step
    counter = 0
    if print_feedback:
        print("Started .0.", end='', flush=True)

    # loop through the input features
    inFeature = in_vec_lyr.GetNextFeature()
    while inFeature:
        if (nFeats > 10) and (counter == feedback_next):
            if print_feedback:
                print(".{}.".format(feedback), end='', flush=True)
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
                raise Exception("Geometry transformation failed... Error Code: {}".format(trans_err_code))
            # create a new feature
            outFeature = ogr.Feature(result_lyr_defn)
            # set the geometry and attribute
            outFeature.SetGeometry(geom)
            for i in range(0, result_lyr_defn.GetFieldCount()):
                outFeature.SetField(result_lyr_defn.GetFieldDefn(i).GetNameRef(), inFeature.GetField(i))
            # add the feature to the shapefile
            result_lyr.CreateFeature(outFeature)
        # dereference the features and get the next input feature
        outFeature = None

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        inFeature = in_vec_lyr.GetNextFeature()
        counter = counter + 1

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    if print_feedback:
        print(" Completed")

    result_lyr.ResetReading()

    return result_ds, result_lyr


def getAttLstSelectFeats(vecFile, vecLyr, attNames, selVecFile, selVecLyr):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vecFile: vector layer from which the attribute data comes from.
:param vecLyr: the layer name from which the attribute data comes from.
:param attNames: a list of attribute names to be outputted.
:param selVecFile: the vector file which will be intersected within the vector file.
:param selVecLyr: the layer name which will be intersected within the vector file.
:return: list of dictionaries with the output values.

"""
    gdal.UseExceptions()
    att_vals = []
    try:
        dsVecFile = gdal.OpenEx(vecFile, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '" + vecFile + "'")
        
        lyrVecObj = dsVecFile.GetLayerByName( vecLyr )
        if lyrVecObj is None:
            raise Exception("Could not find layer '" + vecLyr + "'")
            
        dsSelVecFile = gdal.OpenEx(selVecFile, gdal.OF_READONLY )
        if dsSelVecFile is None:
            raise Exception("Could not open '" + selVecFile + "'")
        
        lyrSelVecObj = dsSelVecFile.GetLayerByName( selVecLyr )
        if lyrSelVecObj is None:
            raise Exception("Could not find layer '" + selVecLyr + "'")
        
        lyrDefn = lyrVecObj.GetLayerDefn()
        feat_idxs = dict()
        feat_types= dict()
        found_atts = dict()
        for attName in attNames:
            found_atts[attName] = False
        
        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in attNames:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True
                
        for attName in attNames:
            if not found_atts[attName]:
                dsSelVecFile = None            
                dsVecFile = None
                raise Exception("Could not find the attribute ({}) specified within the vector layer.".format(attName))
            
        mem_driver = ogr.GetDriverByName('MEMORY')
        
        mem_sel_ds = mem_driver.CreateDataSource('MemSelData')
        mem_sel_lyr = mem_sel_ds.CopyLayer(lyrSelVecObj, selVecLyr, ['OVERWRITE=YES'])
        
        mem_result_ds = mem_driver.CreateDataSource('MemResultData')
        mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", geom_type=lyrVecObj.GetGeomType())
        
        for attName in attNames:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))
        
        lyrVecObj.Intersection(mem_sel_lyr, mem_result_lyr)
        
        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        outvals = []
        while inFeat:
            outdict = dict()
            for attName in attNames:
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


def getAttLstSelectFeatsLyrObjs(vecLyrObj, attNames, selVecLyrObj):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vecLyrObj: the OGR layer object from which the attribute data comes from.
:param attNames: a list of attribute names to be outputted.
:param selVecLyrObj: the OGR layer object which will be intersected within the vector file.

:return: list of dictionaries with the output values.

"""
    gdal.UseExceptions()
    att_vals = []
    try:
        if vecLyrObj is None:
            raise Exception("The vector layer passed into the function was None.")
        
        if selVecLyrObj is None:
            raise Exception("The select vector layer passed into the function was None.")
        
        lyrDefn = vecLyrObj.GetLayerDefn()
        feat_idxs = dict()
        feat_types= dict()
        found_atts = dict()
        for attName in attNames:
            found_atts[attName] = False
        
        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in attNames:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True
                
        for attName in attNames:
            if not found_atts[attName]:
                raise Exception("Could not find the attribute ({}) specified within the vector layer.".format(attName))
            
        mem_driver = ogr.GetDriverByName('MEMORY')
                
        mem_result_ds = mem_driver.CreateDataSource('MemResultData')
        mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", geom_type=vecLyrObj.GetGeomType())
        
        for attName in attNames:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))
        
        vecLyrObj.Intersection(selVecLyrObj, mem_result_lyr)
        
        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        outvals = []
        while inFeat:
            outdict = dict()
            for attName in attNames:
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


def getAttLstSelectBBoxFeats(vec_file, vec_lyr, attNames, bbox, bbox_epsg=None):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vec_file: the OGR file from which the attribute data comes from.
:param vec_lyr: the layer name within the file from which the attribute data comes from.
:param attNames: a list of attribute names to be outputted.
:param bbox: the bounding box for the region of interest (xMin, xMax, yMin, yMax).
:param bbox_epsg: the projection of the BBOX (if None then ignore).

:return: list of dictionaries with the output values.

"""
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if dsVecFile is None:
        raise Exception("Could not open '" + vec_file + "'")

    lyrVecObj = dsVecFile.GetLayerByName(vec_lyr)
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vec_lyr + "'")

    outvals = getAttLstSelectBBoxFeatsLyrObjs(lyrVecObj, attNames, bbox, bbox_epsg)
    dsVecFile = None

    return outvals


def getAttLstSelectBBoxFeatsLyrObjs(vecLyrObj, attNames, bbox, bbox_epsg=None):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vecLyrObj: the OGR layer object from which the attribute data comes from.
:param attNames: a list of attribute names to be outputted.
:param bbox: the bounding box for the region of interest (xMin, xMax, yMin, yMax).
:param bbox_epsg: the projection of the BBOX (if None then ignore).

:return: list of dictionaries with the output values.

"""
    gdal.UseExceptions()
    outvals = []
    try:
        if vecLyrObj is None:
            raise Exception("The vector layer passed into the function was None.")

        in_vec_lyr_spat_ref = vecLyrObj.GetSpatialRef()
        if bbox_epsg is not None:
            in_vec_lyr_spat_ref.AutoIdentifyEPSG()
            in_vec_lyr_epsg = in_vec_lyr_spat_ref.GetAuthorityCode(None)

            if (in_vec_lyr_epsg is not None) and (int(in_vec_lyr_epsg) != int(bbox_epsg)):
                raise Exception("The EPSG codes for the BBOX and input vector layer are not the same.")

        lyrDefn = vecLyrObj.GetLayerDefn()
        feat_idxs = dict()
        feat_types = dict()
        found_atts = dict()
        for attName in attNames:
            found_atts[attName] = False

        for i in range(lyrDefn.GetFieldCount()):
            if lyrDefn.GetFieldDefn(i).GetName() in attNames:
                attName = lyrDefn.GetFieldDefn(i).GetName()
                feat_idxs[attName] = i
                feat_types[attName] = lyrDefn.GetFieldDefn(i).GetType()
                found_atts[attName] = True

        for attName in attNames:
            if not found_atts[attName]:
                raise Exception("Could not find the attribute ({}) specified within the vector layer.".format(attName))

        # Create in-memory layer for the BBOX layer.
        mem_driver = ogr.GetDriverByName('MEMORY')
        mem_bbox_ds = mem_driver.CreateDataSource('MemBBOXData')
        mem_bbox_lyr = mem_bbox_ds.CreateLayer("MemBBOXLyr", in_vec_lyr_spat_ref, geom_type=ogr.wkbPolygon)
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

        mem_result_ds = mem_driver.CreateDataSource('MemResultData')
        mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", in_vec_lyr_spat_ref,
                                                   geom_type=vecLyrObj.GetGeomType())

        for attName in attNames:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))

        vecLyrObj.Intersection(mem_bbox_lyr, mem_result_lyr)

        mem_result_lyr.SyncToDisk()
        mem_result_lyr.ResetReading()

        # loop through the input features
        reslyrDefn = mem_result_lyr.GetLayerDefn()
        inFeat = mem_result_lyr.GetNextFeature()
        while inFeat:
            outdict = dict()
            for attName in attNames:
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


def selectIntersectFeats(vecFile, vecLyr, roiVecFile, roiVecLyr, out_vec_file, out_vec_lyr, out_vec_format='GPKG'):
    """
Function to select the features which intersect with region of interest (ROI) features which will be outputted
into a new vector layer.

:param vecFile: vector layer from which the attribute data comes from.
:param vecLyr: the layer name from which the attribute data comes from.
:param roiVecFile: the vector file which will be intersected within the vector file.
:param roiVecLyr: the layer name which will be intersected within the vector file.
:param out_vec_file: the vector file which will be outputted.
:param out_vec_lyr: the layer name which will be outputted.
:param out_vec_format: output vector format (default GPKG)

"""
    gdal.UseExceptions()
    dsVecFile = gdal.OpenEx(vecFile, gdal.OF_READONLY)
    if dsVecFile is None:
        raise Exception("Could not open '" + vecFile + "'")

    lyrVecObj = dsVecFile.GetLayerByName(vecLyr)
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vecLyr + "'")

    in_vec_lyr_spat_ref = lyrVecObj.GetSpatialRef()

    dsROIVecFile = gdal.OpenEx(roiVecFile, gdal.OF_READONLY)
    if dsROIVecFile is None:
        raise Exception("Could not open '" + roiVecFile + "'")

    lyrROIVecObj = dsROIVecFile.GetLayerByName(roiVecLyr)
    if lyrROIVecObj is None:
        raise Exception("Could not find layer '" + roiVecLyr + "'")

    lyrDefn = lyrVecObj.GetLayerDefn()

    mem_driver = ogr.GetDriverByName('MEMORY')
    mem_roi_ds = mem_driver.CreateDataSource('MemSelData')
    mem_roi_lyr = mem_roi_ds.CopyLayer(lyrROIVecObj, roiVecLyr, ['OVERWRITE=YES'])

    out_driver = ogr.GetDriverByName(out_vec_format)
    result_ds = out_driver.CreateDataSource(out_vec_file)
    result_lyr = result_ds.CreateLayer(out_vec_lyr, in_vec_lyr_spat_ref, geom_type=lyrVecObj.GetGeomType())

    lyrVecObj.Intersection(mem_roi_lyr, result_lyr)

    dsVecFile = None
    dsROIVecFile = None
    mem_roi_ds = None
    result_ds = None


def exportSpatialSelectFeats(vecFile, vecLyr, selVecFile, selVecLyr, outputVec, outVecLyrName, outVecDrvr):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vecFile: vector layer from which the attribute data comes from.
:param vecLyr: the layer name from which the attribute data comes from.
:param selVecFile: the vector file which will be intersected within the vector file.
:param selVecLyr: the layer name which will be intersected within the vector file.
:param outputVec: output vector file/path
:param outVecLyrName: output vector layer
:param outVecDrvr: the output vector layer type.

"""
    gdal.UseExceptions()
    att_vals = []
    try:
        dsVecFile = gdal.OpenEx(vecFile, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '" + vecFile + "'")
        
        lyrVecObj = dsVecFile.GetLayerByName( vecLyr )
        if lyrVecObj is None:
            raise Exception("Could not find layer '" + vecLyr + "'")
        
        lyr_spatial_ref = lyrVecObj.GetSpatialRef()
        
        vec_lyr_bbox = lyrVecObj.GetExtent(True)
        
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
            
        dsSelVecFile = gdal.OpenEx(selVecFile, gdal.OF_READONLY )
        if dsSelVecFile is None:
            raise Exception("Could not open '" + selVecFile + "'")
        
        lyrSelVecObj = dsSelVecFile.GetLayerByName( selVecLyr )
        if lyrSelVecObj is None:
            raise Exception("Could not find layer '" + selVecLyr + "'")
            
        geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
        for feat in lyrSelVecObj:
            geom = feat.GetGeometryRef()
            if geom is not None:
                if geom.Intersects(vec_lyr_bbox_poly):
                    geom_collect.AddGeometry(geom)
        
        out_driver = ogr.GetDriverByName(outVecDrvr)        
        result_ds = out_driver.CreateDataSource(outputVec)
        result_lyr = result_ds.CreateLayer(outVecLyrName, lyr_spatial_ref, geom_type=lyrVecObj.GetGeomType()) 
        
        srcLayerDefn = lyrVecObj.GetLayerDefn()
        for i in range(srcLayerDefn.GetFieldCount()):
            fieldDefn = srcLayerDefn.GetFieldDefn(i)
            result_lyr.CreateField(fieldDefn)
        rsltLayerDefn = result_lyr.GetLayerDefn()   
        
        counter = 0
        openTransaction = False
        for feat in lyrVecObj:
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
    rsgis_utils = rsgislib.RSGISPyUtils()
    if (out_epsg_code is not None) and utm_grid:
        raise Exception("Cannot specify both new output projection and UTM grid.")
    elif utm_grid:
        wgs84_bbox = bbox
        if in_epsg_code != 4326:
            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(in_epsg_code)
            out_proj_obj = osr.SpatialReference()
            out_proj_obj.ImportFromEPSG(4326)
            wgs84_bbox = rsgis_utils.reprojBBOX(bbox, in_proj_obj, out_proj_obj)

        multi_zones = False
        if (wgs84_bbox[0] < -180) and (wgs84_bbox[1] < -180):
            wgs84_bbox = [360 + wgs84_bbox[0], 360 + wgs84_bbox[1], wgs84_bbox[2], wgs84_bbox[3]]
        elif (wgs84_bbox[0] > 180) and (wgs84_bbox[1] > 180):
            wgs84_bbox = [360 - wgs84_bbox[0], 360 - wgs84_bbox[1], wgs84_bbox[2], wgs84_bbox[3]]
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

                utm_proj_epsg = int(rsgislib.tools.utm.epsg_for_UTM(zone_roi[0], utm_top_hemi))

                out_proj_obj = osr.SpatialReference()
                out_proj_obj.ImportFromEPSG(utm_proj_epsg)

                utm_bbox = rsgis_utils.reprojBBOX(zone_roi[1], in_proj_obj, out_proj_obj)
                bboxs = rsgis_utils.getBBoxGrid(utm_bbox, x_size, y_size)

                utm_out_vec_lyr = out_vec_lyr + '_utm{0}{1}'.format(zone_roi[0], utm_top_hemi.lower())
                createPolyVecBBOXs(out_vec, utm_out_vec_lyr, vec_drv, utm_proj_epsg, bboxs, overwrite=first)
                first = False
    else:
        if out_epsg_code is not None:
            in_proj_obj = osr.SpatialReference()
            in_proj_obj.ImportFromEPSG(in_epsg_code)
            out_proj_obj = osr.SpatialReference()
            out_proj_obj.ImportFromEPSG(out_epsg_code)
            proj_bbox = rsgis_utils.reprojBBOX(bbox, in_proj_obj, out_proj_obj)
        else:
            proj_bbox = bbox

        bboxs = rsgis_utils.getBBoxGrid(proj_bbox, x_size, y_size)

        if out_epsg_code is None:
            createPolyVecBBOXs(out_vec, out_vec_lyr, vec_drv, in_epsg_code, bboxs)
        else:
            createPolyVecBBOXs(out_vec, out_vec_lyr, vec_drv, out_epsg_code, bboxs)


def createPolyVecBBOXs(vectorFile, vectorLyr, vecDriver, epsgCode, bboxs, atts=None, attTypes=None, overwrite=True):
    """
This function creates a set of polygons for a set of bounding boxes.
When creating an attribute the available data types are ogr.OFTString, ogr.OFTInteger, ogr.OFTReal

:param vectorFile: output vector file/path
:param vectorLyr: output vector layer
:param vecDriver: the output vector layer type.
:param epsgCode: EPSG code specifying the projection of the data (e.g., 4326 is WSG84 Lat/Long).
:param bboxs: is a list of bounding boxes ([xMin, xMax, yMin, yMax]) to be saved to the output vector.
:param atts: is a dict of lists of attributes with the same length as the bboxs list. The dict should be named
             the same as the attTypes['names'] list.
:param attTypes: is a dict with a list of attribute names (attTypes['names']) and types (attTypes['types']).
                 The list must be the same length as one another and the number of atts. Example type: ogr.OFTString
:param overwrite: - overwrite the vector file specified if it exists. Use False for GPKG where you want to add multiple layers.

"""
    try:
        gdal.UseExceptions()

        if os.path.exists(vectorFile) and (not overwrite):
            # Open the output file.
            outDataSource = gdal.OpenEx(vectorFile, gdal.GA_Update)
        else:
            # Create the output Driver
            outDriver = ogr.GetDriverByName(vecDriver)
            # Create the output vector file
            outDataSource = outDriver.CreateDataSource(vectorFile)

        # create the spatial reference
        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsgCode))
        outLayer = outDataSource.CreateLayer(vectorLyr, srs, geom_type=ogr.wkbPolygon)
        
        
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
                    raise Exception('"{}" is not within atts'.format(attTypes['names'][i]))
                if len(atts[attTypes['names'][i]]) != len(bboxs):
                    raise Exception('"{}" in atts does not have the same len as bboxs'.format(attTypes['names'][i]))
                    
            for i in range(nAtts):       
                field_defn = ogr.FieldDefn( attTypes['names'][i], attTypes['types'][i] )
                if outLayer.CreateField ( field_defn ) != 0:
                    raise Exception("Creating '" + attTypes['names'][i] + "' field failed.\n")
            addAtts = True
        elif not ((atts is None) and (attTypes is None)): 
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
                    outFeature.SetField(attTypes['names'][i], atts[attTypes['names'][i]][n])
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


def createVectorGrid(outputVec, vecDriver, vecLyrName, epsgCode, grid_x, grid_y, bbox):
    """
A function which creates a regular grid across a defined area.

:param outputVec: outout file
:param epsgCode: EPSG code of the output projection
:param grid_x: the size in the x axis of the grid cells.
:param grid_y: the size in the y axis of the grid cells.
:param bbox: the area for which cells will be defined (MinX, MaxX, MinY, MaxY).
:param vecDriver: the output vector layer type.
:param vecLyrName: output vector layer

"""    
    minX = float(bbox[0])
    maxX = float(bbox[1])
    minY = float(bbox[2])
    maxY = float(bbox[3])
    grid_x = float(grid_x)
    grid_y = float(grid_y)
    
    nXCells = math.floor((maxX-minX)/grid_x)
    x_remain = (maxX-minX) - (grid_x * nXCells)
    
    nYCells = math.floor((maxY-minY)/grid_y)
    y_remain = (maxY-minY) - (grid_y * nYCells)
    
    print("Cells: [{0}, {1}]".format(nXCells, nYCells))
    
    bboxs = []
    for i in range(nYCells):
        cMaxY = maxY - (i*grid_y)
        cMinY = cMaxY - grid_y
        for j in range(nXCells):
            cMinX = minX + (j*grid_x)
            cMaxX = cMinX + grid_x
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
        if x_remain > 0:
            cMinX = minX + (nXCells*grid_x)
            cMaxX = cMinX + x_remain
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
    if y_remain > 0:
        cMaxY = maxY - (nYCells*grid_y)
        cMinY = cMaxY - y_remain
        for j in range(nXCells):
            cMinX = minX + (j*grid_x)
            cMaxX = cMinX + grid_x
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
        if x_remain > 0:
            cMinX = minX + (nXCells*grid_x)
            cMaxX = cMinX + x_remain
            bboxs.append([cMinX, cMaxX, cMinY, cMaxY])
    
    createPolyVecBBOXs(outputVec, vecLyrName, vecDriver, epsgCode, bboxs)


def writePts2Vec(vectorFile, vectorLyr, vecDriver, epsgCode, ptsX, ptsY, atts=None, attTypes=None, replace=True, file_opts=[], lyr_opts=[]):
    """
This function creates a set of polygons for a set of bounding boxes.
When creating an attribute the available data types are ogr.OFTString, ogr.OFTInteger, ogr.OFTReal

:param vectorFile: output vector file/path
:param vectorLyr: output vector layer
:param vecDriver: the output vector layer type.
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

        if os.path.exists(vectorFile) and (not replace):
            vecDS = gdal.OpenEx(vectorFile, gdal.GA_Update )
        else:
            if os.path.exists(vectorFile):
                rsgislib.vectorutils.delete_vector_file(vectorFile)
            outdriver = ogr.GetDriverByName(vecDriver)
            vecDS = outdriver.CreateDataSource(vectorFile, options=file_opts)

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
                    raise Exception('"{}" is not within atts'.format(attTypes['names'][i]))
                if len(atts[attTypes['names'][i]]) != len(ptsX):
                    raise Exception('"{}" in atts does not have the same len as bboxs'.format(attTypes['names'][i]))

            for i in range(nAtts):
                field_defn = ogr.FieldDefn(attTypes['names'][i], attTypes['types'][i])
                if outLayer.CreateField(field_defn) != 0:
                    raise Exception("Creating '" + attTypes['names'][i] + "' field failed.\n")
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
                    outFeature.SetField(attTypes['names'][i], atts[attTypes['names'][i]][n])
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


def bboxIntersectsVecLyr(vectorFile, vectorLyr, bbox):
    """
A function which tests whether a feature within an inputted vector layer intersects
with a bounding box. 

:param vectorFile: vector file/path
:param vectorLyr: vector layer name
:param bbox: the bounding box (xMin, xMax, yMin, yMax). Same projection as vector layer.
:returns: boolean (True = Intersection)

"""
    dsVecFile = gdal.OpenEx(vectorFile, gdal.OF_READONLY )
    if dsVecFile is None:
        raise Exception("Could not open '" + vectorFile + "'")
    
    lyrVecObj = dsVecFile.GetLayerByName( vectorLyr )
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vectorLyr + "'")
    
    # Get a geometry collection object for shapefile.
    geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
    for feat in lyrVecObj:
        geom_collect.AddGeometry(feat.GetGeometryRef())
    
    # Create polygon for bbox
    ring = ogr.Geometry(ogr.wkbLinearRing)
    ring.AddPoint(bbox[0], bbox[3])
    ring.AddPoint(bbox[1], bbox[3])
    ring.AddPoint(bbox[1], bbox[2])
    ring.AddPoint(bbox[0], bbox[2])
    ring.AddPoint(bbox[0], bbox[3])
    # Create polygon.
    poly = ogr.Geometry(ogr.wkbPolygon)
    poly.AddGeometry(ring)
    
    # Do they intersect?
    intersect = poly.Intersects(geom_collect)
    return intersect


def create_rtree_index(vec_file, vec_lyr):
    """
    A function which creates a spatial index using the rtree package for the inputted vector file/layer.

    :param vec_file: Input vector file to be processed.
    :param vec_lyr: The layer within the vector file for which the index is to be built.

    """
    import osgeo.gdal as gdal
    import rtree
    import tqdm

    vec_file_obj = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if vec_file_obj is None:
        raise Exception("Could not open '{}'".format(vec_file))

    vec_lyr_obj = vec_file_obj.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    idx_obj = rtree.index.Index(interleaved=False)
    geom_lst = list()

    n_feats = vec_lyr_obj.GetFeatureCount(True)
    n_geom = 0
    pbar = tqdm.tqdm(total=n_feats)
    vec_lyr_obj.ResetReading()
    feat = vec_lyr_obj.GetNextFeature()
    while feat is not None:
        geom_obj = feat.GetGeometryRef()
        if geom_obj is not None:
            xmin, xmax, ymin, ymax = geom_obj.GetEnvelope()
            geom_lst.append(geom_obj.Clone())
            idx_obj.insert(n_geom, (xmin, xmax, ymin, ymax))
            n_geom = n_geom + 1
        pbar.update(1)
        feat = vec_lyr_obj.GetNextFeature()
    vec_file_obj = None
    return idx_obj, geom_lst


def bboxIntersectsIndex(rt_idx, geom_lst, bbox):
    """
    A function which tests for intersection between the geometries and the bounding box
    using a spatial index.
    
    :param rt_idx: the rtree spatial index object (created using the create_rtree_index function)
    :param geom_lst: the list of geometries as referenced in the index (created using the create_rtree_index function)
    :param bbox: the bounding box (xMin, xMax, yMin, yMax). Same projection as geometries in the index.
    :return: True there is an intersection. False there is not an intersection.

    """
    import osgeo.ogr as ogr
    ring = ogr.Geometry(ogr.wkbLinearRing)
    ring.AddPoint(bbox[0], bbox[3])
    ring.AddPoint(bbox[1], bbox[3])
    ring.AddPoint(bbox[1], bbox[2])
    ring.AddPoint(bbox[0], bbox[2])
    ring.AddPoint(bbox[0], bbox[3])
    # Create polygon.
    poly_bbox = ogr.Geometry(ogr.wkbPolygon)
    poly_bbox.AddGeometry(ring)

    bbox_intersects = False

    for geom_idx in list(rt_idx.intersection(bbox)):
        print(geom_idx)
        geom_obj = geom_lst[geom_idx]
        if poly_bbox.Intersects(geom_obj):
            bbox_intersects = True
            break
    return bbox_intersects


def createImgExtentLUT(imgList, vectorFile, vectorLyr, vecDriver, ignore_none_imgs=False, out_proj_wgs84=False,
                       overwrite_lut_file=False):
    """
Create a vector layer look up table (LUT) for a directory of images.

:param imgList: list of input images for the LUT. All input images should be the same projection/coordinate system.
:param vectorFile: output vector file/path
:param vectorLyr: output vector layer
:param vecDriver: the output vector layer type (e.g., GPKG).
:param ignore_none_imgs: if a NULL epsg is returned from an image then ignore and don't include in LUT else throw exception.
:param out_proj_wgs84: if True then the image bounding boxes will be re-projected to EPSG:4326.
:param overwrite_lut_file: if True then output file will be overwritten. If false then not, e.g., can add extra layer to GPKG

Example::

    import glob
    import rsgislib.vectorutils
    imgList = glob.glob('/Users/pete/Temp/GabonLandsat/Hansen*.kea')
    rsgislib.vectorutils.createImgExtentLUT(imgList, './ImgExtents.gpkg', 'HansenImgExtents', 'GPKG')

"""
    import tqdm
    gdal.UseExceptions()
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    bboxs = []
    atts=dict()
    atts['filename'] = []
    atts['path'] = []
    
    attTypes = dict()
    attTypes['types'] = [ogr.OFTString, ogr.OFTString]
    attTypes['names'] = ['filename', 'path']
    
    epsgCode = 0
    
    first = True
    baseImg = ''
    for img in tqdm.tqdm(imgList):
        epsgCodeTmp = rsgisUtils.getEPSGCode(img)
        epsg_found = True
        if epsgCodeTmp is None:
            epsg_found = False
            if not ignore_none_imgs:
                raise Exception("The EPSG code is None: '{}'".format(img))
        if epsg_found:
            if out_proj_wgs84:
                epsgCode = 4326
            else:
                epsgCodeTmp = int(epsgCodeTmp)
                if first:
                    epsgCode = epsgCodeTmp
                    baseImg = img
                    first = False
                else:
                    if epsgCodeTmp != epsgCode:
                        raise Exception("The EPSG codes ({0} & {1}) do not match. (Base: '{2}', Img: '{3}')".format(epsgCode, epsgCodeTmp, baseImg, img))

            if out_proj_wgs84:
                img_bbox = rsgisUtils.getImageBBOXInProj(img, 4326)
            else:
                img_bbox = rsgisUtils.getImageBBOX(img)

            bboxs.append(img_bbox)
            baseName = os.path.basename(img)
            filePath = os.path.dirname(img)
            atts['filename'].append(baseName)
            atts['path'].append(filePath)
    # Create vector layer
    createPolyVecBBOXs(vectorFile, vectorLyr, vecDriver, epsgCode, bboxs, atts, attTypes, overwrite=overwrite_lut_file)


def calcPolyCentroids(vecfile, veclyrname, outVecDrvr, vecoutfile, vecoutlyrname):
    """
Create a vector layer of the polygon centroids.

:param vecfile: input vector file
:param veclyrname: input vector layer within the input file.
:param outVecDrvr: the format driver for the output vector file (e.g., GPKG, ESRI Shapefile).
:param vecoutfile: output file path for the vector.
:param vecoutlyrname: output vector layer name.

"""
    from osgeo import gdal
    from osgeo import ogr
    import tqdm
    gdal.UseExceptions()

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile))

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))
    lyr_spat_ref = veclyr.GetSpatialRef()

    out_driver = ogr.GetDriverByName(outVecDrvr)
    result_ds = out_driver.CreateDataSource(vecoutfile)
    if result_ds is None:
        raise Exception("Could not open '{}'".format(vecoutfile))

    result_lyr = result_ds.CreateLayer(vecoutlyrname, lyr_spat_ref, geom_type=ogr.wkbPoint)
    if result_lyr is None:
        raise Exception("Could not open layer '{}'".format(vecoutlyrname))

    featDefn = result_lyr.GetLayerDefn()

    openTransaction = False
    veclyr.ResetReading()
    n_feats = veclyr.GetFeatureCount(True)
    print(n_feats)
    counter = 0
    pbar = tqdm.tqdm(total=n_feats)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if not openTransaction:
            result_lyr.StartTransaction()
            openTransaction = True

        pt = feat.GetGeometryRef().Centroid()
        outFeat = ogr.Feature(featDefn)
        outFeat.SetGeometry(pt)
        result_lyr.CreateFeature(outFeat)

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        feat = veclyr.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    pbar.close()

    vecDS = None
    result_ds = None


def lstveclyrcols(vecfile, veclyr):
    """
A function which returns a list of columns from the input vector layer.

:param vecfile: input vector file.
:param veclyr: input vector layer
:returns: list of column names

"""
    gdal.UseExceptions()
    atts = []
    
    dsVecFile = gdal.OpenEx(vecfile, gdal.OF_READONLY )
    if dsVecFile is None:
        raise Exception("Could not open '" + vecfile + "'")
    
    lyrVecObj = dsVecFile.GetLayerByName( veclyr )
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + veclyr + "'")
    
    lyrDefn = lyrVecObj.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        atts.append(lyrDefn.GetFieldDefn(i).GetName())
    return atts


def getFeatEnvs(vecFile, vecLyr):
    """
A function which returns a list of bounding boxes for each feature
within the vector layer.

:param vecFile: vector file.
:param vecLyr: layer within the vector file.
:returns: list of BBOXs

"""
    dsVecFile = gdal.OpenEx(vecFile, gdal.OF_VECTOR )
    if dsVecFile is None:
        raise Exception("Could not open '" + vecFile + "'")
        
    lyrVecObj = dsVecFile.GetLayerByName( vecLyr )
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vecLyr + "'")
    
    openTransaction = False
    nFeats = lyrVecObj.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    outenvs = []
    # loop through the input features
    inFeature = lyrVecObj.GetNextFeature()
    while inFeature:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
            
        # get the input geometry
        geom = inFeature.GetGeometryRef()
        if geom is not None:
            outenvs.append(geom.GetEnvelope())
        
        inFeature = lyrVecObj.GetNextFeature()
        counter = counter + 1
    print(" Completed")
    dsVecFile = None
    return outenvs


def subsetEnvsVecLyrObj(lyrVecObj, bbox, epsg=None):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

:param lyrVecObj: OGR Layer Object.
:param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].
:param epsg: provide an EPSG code for the layer if not well defined by the input layer.

:return: OGR Layer and Dataset objects.

"""
    gdal.UseExceptions()
    if lyrVecObj is None:
        raise Exception("Vector layer object which was provided was None.")

    if epsg is not None:
        lyr_spatial_ref = osr.SpatialReference()
        lyr_spatial_ref.ImportFromEPSG(epsg)
    else:
        lyr_spatial_ref = lyrVecObj.GetSpatialRef()
    if lyr_spatial_ref is None:
        raise Exception("The spatial reference for the layer is None - please provide EPSG code.")
    lyrDefn = lyrVecObj.GetLayerDefn()
    
    # Copy the Layer to a new in memory OGR Layer.
    mem_driver = ogr.GetDriverByName('MEMORY')
    mem_result_ds = mem_driver.CreateDataSource('MemResultData')    
    mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", lyr_spatial_ref, geom_type=lyrVecObj.GetGeomType())
    for i in range(lyrDefn.GetFieldCount()):
        fieldDefn = lyrDefn.GetFieldDefn(i)
        mem_result_lyr.CreateField(fieldDefn)
        
    openTransaction = False
    trans_step = 20000
    next_trans = trans_step
    nFeats = lyrVecObj.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    lyrVecObj.ResetReading()
    print("Started .0.", end='', flush=True)
    outenvs = []
    # loop through the input features
    inFeature = lyrVecObj.GetNextFeature()
    while inFeature:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
            
        if not openTransaction:
            mem_result_lyr.StartTransaction()
            openTransaction = True
        
        if inFeature is not None:
            geom = inFeature.GetGeometryRef()
            if geom is not None:
                env = geom.GetEnvelope()
            
                if bbox[0] <= env[1] and bbox[1] >= env[0] and bbox[2] <= env[3] and bbox[3] >= env[2]:
                    mem_result_lyr.CreateFeature(inFeature)
        
        if (counter == next_trans) and openTransaction:
            mem_result_lyr.CommitTransaction()
            openTransaction = False
            next_trans = next_trans + trans_step
        
        inFeature = lyrVecObj.GetNextFeature()
        counter = counter + 1
    print(" Completed")
    
    if openTransaction:
        mem_result_lyr.CommitTransaction()
        openTransaction = False
        
    return mem_result_ds, mem_result_lyr


def subset_veclyr_to_featboxs(vec_file_bbox, vec_lyr_bbox, vec_file_tosub, vec_lyr_tosub, out_lyr_name, out_file_base, out_file_end='.gpkg', out_file_driver='GPKG'):
    """
    A function which subsets an input vector layer using the BBOXs of the features within another vector
    layer.
    :param vec_file_bbox: The vector file for the features which define the BBOXs
    :param vec_lyr_bbox: The vector layer for the features which define the BBOXs
    :param vec_file_tosub: The vector file for the layer which is to be subset.
    :param vec_lyr_tosub: The vector layer for the layer which is to be subset.
    :param out_lyr_name: The layer name for the output files - all output files will have the same layer name.
    :param out_file_base: The base name for the output files.
                          A numeric count 0-n will be inserted between this and the ending.
    :param out_file_end: The output file ending (e.g., .gpkg).
    :param out_file_driver: The output file driver (e.g., GPKG).

    """
    bboxes = rsgislib.vectorutils.getFeatEnvs(vec_file_bbox, vec_lyr_bbox)
    print("There are {} bboxes to subset to.".format(len(bboxes)))
    for i in range(len(bboxes)):
        print(bboxes[i])
        grid_02d_ds, grid_02d_lyr = rsgislib.vectorutils.readVecLyr2Mem(vec_file_tosub, vec_lyr_tosub)
        mem_result_ds, mem_result_lyr = rsgislib.vectorutils.subsetEnvsVecLyrObj(grid_02d_lyr, bboxes[i])
        out_vec_file = '{0}{1}{2}'.format(out_file_base, i, out_file_end)
        rsgislib.vectorutils.writeVecLyr2File(mem_result_lyr, out_vec_file, out_lyr_name, out_file_driver, options=['OVERWRITE=YES'], replace=True)
        mem_result_ds = None
        grid_02d_ds = None


def readVecLyr2Mem(vecfile, veclyrname):
    """
A function which reads a vector layer to an OGR in memory layer.

:param vecfile: input vector file
:param veclyrname: input vector layer within the input file.

:return: ogr_dataset, ogr_layer

"""
    gdal.UseExceptions()
    try:
        vecDS = gdal.OpenEx(vecfile, gdal.OF_READONLY )
        if vecDS is None:
            raise Exception("Could not open '" + vecfile + "'")
        
        veclyr = vecDS.GetLayerByName( veclyrname )
        if veclyr is None:
            raise Exception("Could not find layer '" + veclyrname + "'")
            
        mem_driver = ogr.GetDriverByName('MEMORY')
        
        mem_ds = mem_driver.CreateDataSource('MemSelData')
        mem_lyr = mem_ds.CopyLayer(veclyr, veclyrname, ['OVERWRITE=YES'])
            
    except Exception as e:
        print("Error Vector File: {}".format(vecfile), file=sys.stderr)
        print("Error Vector Layer: {}".format(veclyrname), file=sys.stderr)
        raise e
    return mem_ds, mem_lyr


def open_gdal_vec_lyr(vec_file, vec_lyr=None):
    """
    A function which opens a GDAL/OGR vector layer and returns
    the Dataset and Layer objects. Note, the file must be closed
    by setting the dataset to None.

    :param vec_file: the file path to the vector file.
    :param vec_lyr: the name of the vector layer. If None then first layer is returned.
    :return: GDAL dataset, GDAL Layer
    """
    vec_obj_ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if vec_obj_ds is None:
        raise Exception("Could not open '{}'".format(vec_file))

    if vec_lyr == None:
        lyr_obj = vec_obj_ds.GetLayer()
        if lyr_obj is None:
            raise Exception("Could not find a layer.")
    else:
        lyr_obj = vec_obj_ds.GetLayerByName(vec_lyr)
        if lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))

    return vec_obj_ds, lyr_obj


def getMemVecLyrSubset(vecFile, vecLyr, bbox):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

:param vecFile: vector layer from which the attribute data comes from.
:param vecLyr: the layer name from which the attribute data comes from.
:param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].

:return: OGR Layer and Dataset objects.

"""
    gdal.UseExceptions()
    try:
        dsVecFile = gdal.OpenEx(vecFile, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '" + vecFile + "'")
        
        lyrVecObj = dsVecFile.GetLayerByName( vecLyr )
        if lyrVecObj is None:
            raise Exception("Could not find layer '" + vecLyr + "'")
            
        mem_result_ds, mem_result_lyr = subsetEnvsVecLyrObj(lyrVecObj, bbox)
        
    except Exception as e:
        print("Error: Layer: {} File: {}".format(vecLyr, vecFile))
        raise e
    return mem_result_ds, mem_result_lyr


def writeVecLyr2File(veclyr, vecfile, veclyrname, vecDriver, options=['OVERWRITE=YES'], replace=False):
    """
A function which reads a vector layer to an OGR in memory layer.

:param veclyr: OGR vector layer object
:param vecfile: output vector file
:param veclyrname: output vector layer within the input file.
:param vecDriver: the OGR driver for the output file.
:param options: provide a list of driver specific options; see https://www.gdal.org/ogr_formats.html
:param replace: if true the output file is replaced (i.e., overwritten to anything in an existing file will be lost).

"""
    gdal.UseExceptions()
    try:
        if os.path.exists(vecfile) and (not replace):
            vecDS = gdal.OpenEx(vecfile, gdal.GA_Update )
        else:            
            outdriver = ogr.GetDriverByName(vecDriver)
            vecDS = outdriver.CreateDataSource(vecfile)
        
        if vecDS is None:
            raise Exception("Could not open or create '{}'".format(vecfile))            

        vecDS_lyr = vecDS.CopyLayer(veclyr, veclyrname, options)
        vecDS = None
            
    except Exception as e:
        print("Error Vector File: {}".format(vecfile), file=sys.stderr)
        print("Error Vector Layer: {}".format(veclyrname), file=sys.stderr)
        raise e


def queryFileLUT(lut_file, lut_lyr, roi_file, roi_lyr, out_dest, targz_out, cp_cmds):
    """
    A function which allows the file LUT to be queried (intersection) and commands generated for completing operations.
    Must select (pass true) for either targz_out or cp_cmds not both. If both are False then the list of intersecting
    files will be returned.

    :param lut_file: OGR vector file with the LUT.
    :param lut_lyr: name of the layer within the LUT file.
    :param roi_file: region of interest OGR vector file.
    :param roi_lyr: layer name within the ROI file.
    :param out_dest: the destination for outputs from command (e.g., where are the files to be copied to or output
                     file name for tar.gz file.
    :param targz_out: boolean which specifies that the command for generating a tar.gz file should be generated.
    :param cp_cmds: boolean which specifies that the command for copying the LUT files to a out_dest should be generated.

    :return: returns a list of commands to be executed.

    """

    if lut_lyr is None:
        lut_lyr = os.path.splitext(os.path.basename(lut_file))[0]

    if roi_lyr is None:
        roi_lyr = os.path.splitext(os.path.basename(roi_file))[0]

    roi_mem_ds, roi_mem_lyr = rsgislib.vectorutils.readVecLyr2Mem(roi_file, roi_lyr)

    roi_bbox = roi_mem_lyr.GetExtent(True)

    lut_mem_ds, lut_mem_lyr = rsgislib.vectorutils.getMemVecLyrSubset(lut_file, lut_lyr, roi_bbox)

    fileListDict = rsgislib.vectorutils.getAttLstSelectFeatsLyrObjs(lut_mem_lyr, ['path', 'filename'], roi_mem_lyr)

    out_cmds = []
    if targz_out:
        cmd = 'tar -czf ' + out_dest
        for fileItem in fileListDict:
            filepath = os.path.join(fileItem['path'], fileItem['filename'])
            cmd = cmd + " " + filepath

        out_cmds.append(cmd)
    elif cp_cmds:
        for fileItem in fileListDict:
            filepath = os.path.join(fileItem['path'], fileItem['filename'])
            out_cmds.append("cp {0} {1}".format(filepath, out_dest))
    else:
        for fileItem in fileListDict:
            filepath = os.path.join(fileItem['path'], fileItem['filename'])
            out_cmds.append(filepath)

    return out_cmds


def ogrVectorColDataTypeFromGDALRATColType(rat_datatype):
    """
Returns the data type to create a column in a OGR vector layer for equalivant to
rat_datatype.

:param rat_datatype: the datatype (GFT_Integer, GFT_Real, GFT_String) for the RAT column.
:return: OGR datatype (OFTInteger, OFTReal, OFTString)

"""
    if rat_datatype == gdal.GFT_Integer:
        rtn_type = ogr.OFTInteger
    elif rat_datatype == gdal.GFT_Real:
        rtn_type = ogr.OFTReal
    elif rat_datatype == gdal.GFT_String:
        rtn_type = ogr.OFTString
    else:
        raise Exception("Do not recognise inputted datatype")
    return rtn_type


def copyRATCols2VectorLyr(vec_file, vec_lyr, rat_row_col, clumps_img, ratcols, outcolnames=None, outcoltypes=None):
    """
    A function to copy columns from RAT to a vector layer. Note, the vector layer needs a column, which already exists,
    that specifies the row from the RAT the feature is related to. If you created the vector using the polygonise
    function then that column will have been created and called 'PXLVAL'.

    :param vec_file: The vector file to be used.
    :param vec_lyr: The name of the layer within the vector file.
    :param rat_row_col: The column in the layer which specifies the RAT row the feature corresponds with.
    :param clumps_img: The clumps image with the RAT from which information should be taken.
    :param ratcols: The names of the columns in the RAT to be copied.
    :param outcolnames: If you do not want the same column names as the RAT then you can specify alternatives. If None
                        then the names will be the same as the RAT. (Default = None)
    :param outcoltypes: The data types used for the columns in vector layer. If None then matched to RAT.
                        Default is None

    """
    gdal.UseExceptions()
    from rios import rat

    if outcolnames is None:
        outcolnames = ratcols
    else:
        if len(outcolnames) != len(ratcols):
            raise Exception("The output columns names list is not the same length ({}) as the length of "
                            "the RAT columns list ({}) - they must be the same.".format(len(outcolnames), len(ratcols)))

    if outcoltypes is not None:
        if len(outcolnames) == len(outcoltypes):
            raise Exception("Either specify the column types as None or the length of the list needs to be "
                            "the same as the number output columns.")

    if not os.path.exists(vec_file):
        raise Exception("Input vector does not exist, check path: {}".format(vec_file))

    clumps_img_ds = gdal.Open(clumps_img, gdal.GA_ReadOnly)
    if clumps_img_ds is None:
        raise Exception("Could not open the inputted clumps image: {}".format(clumps_img))

    vecDS = gdal.OpenEx(vec_file, gdal.GA_Update)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vec_file))

    vec_lyr_obj = vecDS.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '" + vec_file + "'")

    rat_cols_all = rsgislib.rastergis.getRATColumnsInfo(clumps_img)

    cols_exist = []
    for ratcol in ratcols:
        if ratcol not in rat_cols_all:
            raise Exception("Column '{}' is not within the clumps image: {}".format(ratcol, clumps_img))
        cols_exist.append(False)

    if outcoltypes is None:
        outcoltypes = []
        for colname in ratcols:
            rat_type = rat_cols_all[colname]['type']
            ogr_type = ogrVectorColDataTypeFromGDALRATColType(rat_type)
            outcoltypes.append(ogr_type)

    lyrDefn = vec_lyr_obj.GetLayerDefn()

    rat_row_col_exists = False
    for i in range(lyrDefn.GetFieldCount()):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == rat_row_col.lower():
            rat_row_col_exists = True
            break
    if not rat_row_col_exists:
        raise Exception("Could not find column '{}' within the vector layers.".format(rat_row_col))

    for i in range(lyrDefn.GetFieldCount()):
        col_n = 0
        for colname in outcolnames:
            if lyrDefn.GetFieldDefn(i).GetName().lower() == colname.lower():
                cols_exist[col_n] = True
                break
            col_n = col_n + 1

    col_n = 0
    for colname in outcolnames:
        if not cols_exist[col_n]:
            field_defn = ogr.FieldDefn(colname, outcoltypes[col_n])
            if vec_lyr_obj.CreateField(field_defn) != 0:
                raise Exception(
                    "Creating '{}' field failed; becareful with case, some drivers are case insensitive but column might not be found.".format(
                        colname))

            cols_exist[col_n] = True
        col_n = col_n + 1

    # Read in the RAT columns
    rat_cols_data = []
    for colname in ratcols:
        rat_cols_data.append(rat.readColumn(clumps_img_ds, colname))

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
            for n_col in range(len(outcolnames)):
                if outcoltypes[n_col] == ogr.OFTInteger:
                    feat.SetField("{}".format(outcolnames[n_col]), int(rat_cols_data[n_col][rat_row]))
                elif outcoltypes[n_col] == ogr.OFTReal:
                    feat.SetField("{}".format(outcolnames[n_col]), float(rat_cols_data[n_col][rat_row]))
                elif outcoltypes[n_col] == ogr.OFTString:
                    feat.SetField("{}".format(outcolnames[n_col]), "{}".format(rat_cols_data[n_col][rat_row]))
                else:
                    feat.SetField("{}".format(outcolnames[n_col]), rat_cols_data[n_col][rat_row])
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


def performSpatialJoin(base_vec_file, join_vec_file, output_vec_file, base_lyr=None, join_lyr=None, output_lyr=None,
                           outVecDrvr=None, join_how="inner", join_op="within"):
    """
    A function to perform a spatial join between two vector layers. This function uses geopandas so this needs
    to be installed. You also need to have the rtree package to generate the index used to perform the intersection.

    For more information see: http://geopandas.org/mergingdata.html#spatial-joins

    :param base_vec_file: the base vector file with the geometries which will be outputted.
    :param join_vec_file: the vector with the attributes which will be joined to the base vector geometries.
    :param output_vec_file: the output vector file.
    :param base_lyr: the layer name for the base vector, not needed if input file is a shapefile (Default None).
    :param join_lyr: the layer name for the join vector, not needed if input file is a shapefile (Default None).
    :param output_lyr: the layer name for the output vector, not needed if input file is a shapefile (Default None).
    :param outVecDrvr: The output vector file format, if none then shapefile outputted (Default None)
    :param join_how: Specifies the type of join that will occur and which geometry is retained. The options are
                    [left, right, inner]. The default is 'inner'
    :param join_op: Defines whether or not to join the attributes of one object to another.
                   The options are [intersects, within, contains] and default is 'within'
    """
    if join_how not in ['left', 'right', 'inner']:
        raise Exception('The join_how specifed is not valid.')
    if join_op not in ['intersects', 'within', 'contains']:
        raise Exception('The join_op specifed is not valid.')

    import geopandas # Import geopandas module.
    import rtree # this is imported to provide useful error message as will be used in sjoin but if not present
                 # the error message is not very user friendly:
                 # AttributeError: 'NoneType' object has no attribute 'intersection'

    if base_lyr is None:
        base_gpd_df = geopandas.read_file(base_vec_file)
    else:
        base_gpd_df = geopandas.read_file(base_vec_file, layer=base_lyr)

    if join_lyr is None:
        join_gpg_df = geopandas.read_file(join_vec_file)
    else:
        join_gpg_df = geopandas.read_file(join_vec_file, layer=join_lyr)

    join_gpg_df = geopandas.sjoin(base_gpd_df, join_gpg_df, how=join_how, op=join_op)

    if outVecDrvr is None:
        join_gpg_df.to_file(output_vec_file)
    else:
        if output_lyr is None:
            join_gpg_df.to_file(output_vec_file, driver=outVecDrvr)
        else:
            join_gpg_df.to_file(output_vec_file, layer=output_lyr, driver=outVecDrvr)


def does_vmsk_img_intersect(input_vmsk_img, roi_vec_file, roi_vec_lyr, tmp_dir, vec_epsg=None):
    """
    This function checks whether the input binary raster mask intesects with the input vector
    layer. A check is first done as to whether the bounding boxes intersect, if they do then
    the intersection between the images is then calculated. The input image and vector can be
    in different projections but the projection needs to be well defined.

    :param input_vmsk_img: Input binary mask image file.
    :param roi_vec_file: The input vector file.
    :param roi_vec_lyr: The name of the input layer.
    :param tmp_dir: a temporary directory for files generated during processing.
    :param vec_epsg: If projection is poorly defined by the vector layer then it can be specified.
    """
    import rsgislib.imagecalc
    rsgis_utils = rsgislib.RSGISPyUtils()

    # Does the input image BBOX intersect the BBOX of the ROI vector?
    if vec_epsg is None:
        vec_epsg = rsgis_utils.getProjEPSGFromVec(roi_vec_file, roi_vec_lyr)
    img_epsg = rsgis_utils.getEPSGCode(input_vmsk_img)
    if img_epsg == vec_epsg:
        img_bbox = rsgis_utils.getImageBBOX(input_vmsk_img)
        projs_match = True
    else:
        img_bbox = rsgis_utils.getImageBBOXInProj(input_vmsk_img, vec_epsg)
        projs_match = False
    vec_bbox = rsgis_utils.getVecLayerExtent(roi_vec_file, roi_vec_lyr, computeIfExp=True)

    img_intersect = False
    if rsgis_utils.do_bboxes_intersect(img_bbox, vec_bbox):
        uid_str = rsgis_utils.uidGenerator()
        base_vmsk_img = rsgis_utils.get_file_basename(input_vmsk_img)

        tmp_file_dir = os.path.join(tmp_dir, "{}_{}".format(base_vmsk_img, uid_str))
        if not os.path.exists(tmp_file_dir):
            os.mkdir(tmp_file_dir)

        # Rasterise the vector layer to the input image extent.
        mem_ds, mem_lyr = getMemVecLyrSubset(roi_vec_file, roi_vec_lyr, img_bbox)

        if not projs_match:
            mem_result_ds, mem_result_lyr = reproj_vec_lyr(mem_lyr, 'mem_vec', img_epsg,
                                                           out_vec_drv='MEMORY', out_lyr_name=None,
                                                           in_epsg=None, print_feedback=False)
            mem_ds = None
        else:
            mem_result_ds = mem_ds
            mem_result_lyr = mem_lyr

        roi_img = os.path.join(tmp_file_dir, "{}_roiimg.kea".format(base_vmsk_img))
        rsgislib.imageutils.createCopyImage(input_vmsk_img, roi_img, 1, 0, 'KEA', rsgislib.TYPE_8UINT)
        rasteriseVecLyrObj(mem_result_lyr, roi_img, burnVal=1, vecAtt=None, calcstats=True, thematic=True, nodata=0)
        mem_result_ds = None

        bandDefns = []
        bandDefns.append(rsgislib.imagecalc.BandDefn('vmsk', input_vmsk_img, 1))
        bandDefns.append(rsgislib.imagecalc.BandDefn('roi', roi_img, 1))
        intersect_img = os.path.join(tmp_file_dir, "{}_intersectimg.kea".format(base_vmsk_img))
        rsgislib.imagecalc.bandMath(intersect_img, "(vmsk==1) && (roi==1)?1:0", 'KEA', rsgislib.TYPE_8UINT, bandDefns)
        rsgislib.rastergis.populateStats(intersect_img, addclrtab=True, calcpyramids=True, ignorezero=True)
        n_vals = rsgislib.imagecalc.countPxlsOfVal(intersect_img, vals=[1])
        if n_vals[0] > 0:
            img_intersect = True
        shutil.rmtree(tmp_file_dir)
    return img_intersect


def merge_to_multi_layer_vec(input_file_lyrs, output_file, format='GPKG', overwrite=True):
    """
    A function which takes a list of vector files and layers (as VecLayersInfoObj objects)
    and merged them into a multi-layer vector file.

    :param input_file_lyrs: list of VecLayersInfoObj objects.
    :param output_file: output vector file.
    :param format: output format Default='GPKG'.
    :param overwrite: bool (default = True) specifying whether the input file should be 
                      overwritten if it already exists.

    """
    first = True
    for vec in input_file_lyrs:
        vec_lyr_obj = open_gdal_vec_lyr(vec.vecfile, vec.veclyr)
        if first and overwrite:
            rsgislib.vectorutils.writeVecLyr2File(vec_lyr_obj, output_file, vec.outlyr, format,
                                                  options=['OVERWRITE=YES'], replace=True)
        else:
            rsgislib.vectorutils.writeVecLyr2File(vec_lyr_obj, output_file, vec.outlyr, format)
        vec_lyr_obj = None
        first = False


def convert_polygon2polyline(vec_poly_file, vec_poly_lyr, vec_line_file, vec_line_lyr=None,
                             out_format="ESRI Shapefile", force=False):
    """
    A function to convert a polygon vector file to a polyline file.

    :param vec_poly_file: Input polygon vector file
    :param vec_poly_lyr: The name of the vector layer
    :param vec_line_file: The output vector file
    :param vec_line_lyr: The output vector layer name
    :param out_format: The output vector file format (default: ESRI Shapefile).
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import os
    import tqdm

    if os.path.exists(vec_line_file):
        if force:
            delete_vector_file(vec_line_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vec_line_file))

    if vec_line_lyr is None:
        vec_line_lyr = os.path.splitext(os.path.basename(vec_line_file))[0]

    gdal.UseExceptions()
    vec_poly_ds_obj = gdal.OpenEx(vec_poly_file, gdal.OF_VECTOR)
    vec_poly_lyr_obj = vec_poly_ds_obj.GetLayer(vec_poly_lyr)
    vec_poly_spat_ref = vec_poly_lyr_obj.GetSpatialRef()

    out_vec_drv = gdal.GetDriverByName(out_format)
    if out_vec_drv == None:
        raise Exception("Driver ('{}') has not be recognised.".format(out_format))

    out_ds_obj = out_vec_drv.Create(vec_line_file, 0, 0, 0, gdal.GDT_Unknown)
    out_lyr_obj = out_ds_obj.CreateLayer(vec_line_lyr, vec_poly_spat_ref, geom_type=ogr.wkbLineString)
    feat_defn = out_lyr_obj.GetLayerDefn()

    n_feats = vec_poly_lyr_obj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    open_transaction = False
    counter = 0
    in_feature = vec_poly_lyr_obj.GetNextFeature()
    while in_feature:
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        geom = in_feature.GetGeometryRef()
        if geom is not None:
            ring = geom.GetGeometryRef(0)
            out_feat = ogr.Feature(feat_defn)
            out_feat.SetGeometry(ring)
            out_lyr_obj.CreateFeature(out_feat)
            out_feat = None

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = vec_poly_lyr_obj.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None


def get_pt_on_line(pt1, pt2, dist):
    """
    A function that calculates a point on the vector defined by pt1 and pt2.

    :param pt1: An ogr point geometry which has functions GetX() and GetY().
    :param pt2: An ogr point geometry which has functions GetX() and GetY().
    :param dist: The distance from pt1 the new point is to be created.
    :return: The created point; returned as a set of floats: (x, y)

    """
    import math
    out_pt_x = 0.0
    out_pt_y = 0.0
    if dist == 0:
        out_pt_x = pt1.GetX()
        out_pt_y = pt1.GetY()
    else:
        dx = pt2.GetX() - pt1.GetX()
        dy = pt2.GetY() - pt1.GetY()
        theta = math.atan(dy / dx)
        y1 = dist * math.sin(theta)
        x1 = dist * math.cos(theta)

        if (dx >= 0) and (dy > 0):
            out_pt_x = pt1.GetX() + x1
            out_pt_y = pt1.GetY() + y1
        elif (dx >= 0) and (dy <= 0):
            out_pt_x = pt1.GetX() + x1
            out_pt_y = pt1.GetY() + y1
        elif (dx < 0) & (dy > 0):
            out_pt_x = pt1.GetX() - x1
            out_pt_y = pt1.GetY() - y1
        elif (dx < 0) & (dy <= 0):
            out_pt_x = pt1.GetX() - x1
            out_pt_y = pt1.GetY() - y1
    return out_pt_x, out_pt_y


def find_pt_to_side(pt_start, pt, pt_end, line_len, left_hand=False):
    """
    A function to calculate a point location at a right-angle to the vector defined
    by the points pt_start and pt_end at the location pt.

    :param pt_start: An ogr point geometry which has functions GetX(), GetY() and Distance().
    :param pt: An ogr point geometry which has functions GetX(), GetY() and Distance().
    :param pt_end: An ogr point geometry which has functions GetX(), GetY() and Distance().
    :param line_len: The distance from the pt_start and pt_end vector to the new point.
    :param left_hand: Specify which side the point is projected from the pt_start and pt_end vector.
                      Default: False - project right-hand side of vector, True - project left-hand side of vector
    :return: The created point; returned as a set of floats: (x, y)

    """
    if left_hand:
        tmp_pt = pt_end
        pt_end = pt_start
        pt_start = tmp_pt
    import math
    dx = pt_end.GetX() - pt_start.GetX()
    dy = pt_end.GetY() - pt_start.GetY()
    beta = math.atan(dy / dx)

    distanceP1P2 = pt_start.Distance(pt)
    distanceP1P3 = math.sqrt((line_len * line_len) + (distanceP1P2 * distanceP1P2))
    theta = math.atan(line_len / distanceP1P2)
    alpha = math.pi - (theta - beta)

    localX = distanceP1P3 * math.cos(alpha)
    localY = distanceP1P3 * math.sin(alpha)

    if (dx >= 0) and (dy > 0):
        out_pt_x = pt_start.GetX() - localX
        out_pt_y = pt_start.GetY() - localY
    elif (dx >= 0) and (dy <= 0):
        out_pt_x = pt_start.GetX() - localX
        out_pt_y = pt_start.GetY() - localY
    elif (dx < 0) and (dy > 0):
        out_pt_x = pt_start.GetX() + localX
        out_pt_y = pt_start.GetY() + localY
    elif (dx < 0) and (dy <= 0):
        out_pt_x = pt_start.GetX() + localX
        out_pt_y = pt_start.GetY() + localY
    else:
        raise Exception("Could not resolve find_pt_to_side...")
    return out_pt_x, out_pt_y


def create_orthg_lines(vec_file, vec_lyr, out_vec_file, out_vec_lyr=None, pt_step=1000, line_len=10000, left_hand=False,
                       out_format="GEOJSON", force=False):
    """
    A function to create a set of lines which are orthogonal to the lines of the input
    vector file.
    
    :param vec_file: The inputted vector file path - this should be a polyline vector file
    :param vec_lyr: The name of the vector layer
    :param out_vec_file: The output vector file path - this will be a polyline vector file
    :param out_vec_lyr: The name of the output vector layer (if None then created as the same as the file name)
    :param pt_step: The steps (in the unit of the coordinate system) along lines in the layer at which lines
                    are created.
    :param line_len: The length of the lines created.
    :param left_hand: Specify which side the point is projected from the line (i.e., left or right side)
                      Default: False - project right-hand side of vector, True - project left-hand side of vector
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    gdal.UseExceptions()
    vec_ds_obj = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    vec_lyr_obj = vec_ds_obj.GetLayer(vec_lyr)
    vec_spat_ref = vec_lyr_obj.GetSpatialRef()

    out_vec_drv = gdal.GetDriverByName(out_format)
    if out_vec_drv == None:
        raise Exception("Driver ('{}') has not be recognised.".format(out_format))

    out_ds_obj = out_vec_drv.Create(out_vec_file, 0, 0, 0, gdal.GDT_Unknown)
    out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, vec_spat_ref, geom_type=ogr.wkbLineString)
    uid_field = ogr.FieldDefn('uid', ogr.OFTInteger)
    out_lyr_obj.CreateField(uid_field)
    start_x_field = ogr.FieldDefn('start_x', ogr.OFTReal)
    out_lyr_obj.CreateField(start_x_field)
    start_y_field = ogr.FieldDefn('start_y', ogr.OFTReal)
    out_lyr_obj.CreateField(start_y_field)
    end_x_field = ogr.FieldDefn('end_x', ogr.OFTReal)
    out_lyr_obj.CreateField(end_x_field)
    end_y_field = ogr.FieldDefn('end_y', ogr.OFTReal)
    out_lyr_obj.CreateField(end_y_field)
    feat_defn = out_lyr_obj.GetLayerDefn()

    n_feats = vec_lyr_obj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    open_transaction = False
    counter = 0
    line_uid = 1
    p_pt = ogr.Geometry(ogr.wkbPoint)
    p_pt.AddPoint(0.0, 0.0)
    c_pt = ogr.Geometry(ogr.wkbPoint)
    c_pt.AddPoint(0.0, 0.0)
    c_dist = 0.0
    first_pt = True
    in_feature = vec_lyr_obj.GetNextFeature()
    while in_feature:
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        geom = in_feature.GetGeometryRef()
        if geom is not None:
            pts = geom.GetPoints()
            first_pt = True
            c_dist = 0.0
            for pt in pts:
                if first_pt:
                    p_pt.SetPoint(0, pt[0], pt[1])
                    c_pt.SetPoint(0, pt[0], pt[1])
                    first_pt = False
                else:
                    p_pt.SetPoint(0, c_pt.GetX(), c_pt.GetY())
                    c_pt.SetPoint(0, pt[0], pt[1])
                    n_step = 0
                    step_c_dist = c_dist
                    while True:
                        if ((p_pt.Distance(c_pt) + step_c_dist) - (pt_step * n_step)) > pt_step:
                            pt_at_dist = ((pt_step * n_step) + pt_step) - step_c_dist
                            ptx, pty = get_pt_on_line(p_pt, c_pt, pt_at_dist)
                            base_pt = ogr.Geometry(ogr.wkbPoint)
                            base_pt.AddPoint(ptx, pty)
                            ptx_end, pty_end = find_pt_to_side(p_pt, base_pt, c_pt, line_len, left_hand)
                            out_line = ogr.Geometry(ogr.wkbLineString)
                            out_line.AddPoint(ptx, pty)
                            out_line.AddPoint(ptx_end, pty_end)
                            out_feat = ogr.Feature(feat_defn)
                            out_feat.SetGeometry(out_line)
                            out_feat.SetField('uid', line_uid)
                            out_feat.SetField('start_x', ptx)
                            out_feat.SetField('start_y', pty)
                            out_feat.SetField('end_x', ptx_end)
                            out_feat.SetField('end_y', pty_end)
                            out_lyr_obj.CreateFeature(out_feat)
                            out_feat = None
                            line_uid = line_uid + 1
                            n_step = n_step + 1
                        else:
                            if n_step == 0:
                                c_dist = c_dist + p_pt.Distance(c_pt)
                            else:
                                c_dist = (p_pt.Distance(c_pt) + step_c_dist) - (pt_step * n_step)
                            break

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None
    vec_ds_obj = None


def closest_line_intersection(vec_line_file, vec_line_lyr, vec_objs_file, vec_objs_lyr, out_vec_file, out_vec_lyr=None,
                              start_x_field='start_x', start_y_field='start_y', uid_field='uid', out_format="GEOJSON",
                              force=False):
    """
    A function which intersects each line within the input vector layer (vec_objs_file, vec_objs_lyr)
    creating a new line between the start point of the input layer (defined in the vector attribute table:
    start_x_field, start_y_field) and the intersection point which is closest to the start point.

    :param vec_line_file: Input lines vector file path.
    :param vec_line_lyr: Input lines vector layer name.
    :param vec_objs_file: The vector file for the objects (expecting polygons) to be intersected with.
    :param vec_objs_lyr: The vector layer for the objects (expecting polygons) to be intersected with.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name
    :param start_x_field: The field name for the start point X coordinate for the input lines.
    :param start_y_field: The field name for the start point Y coordinate for the input lines.
    :param uid_field: The field name for the Unique ID (UID) of the input lines.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import rsgislib
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    gdal.UseExceptions()
    rsgis_utils = rsgislib.RSGISPyUtils()
    vec_bbox = rsgis_utils.getVecLayerExtent(vec_line_file, vec_line_lyr)

    ds_line_vec = gdal.OpenEx(vec_line_file, gdal.OF_READONLY)
    if ds_line_vec is None:
        raise Exception("Could not open '{}'".format(vec_line_file))

    lyr_line_vec = ds_line_vec.GetLayerByName(vec_line_lyr)
    if lyr_line_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_line_lyr))

    x_col_exists = False
    y_col_exists = False
    uid_col_exists = False
    lyr_line_defn = lyr_line_vec.GetLayerDefn()
    for i in range(lyr_line_defn.GetFieldCount()):
        if lyr_line_defn.GetFieldDefn(i).GetName() == start_x_field:
            x_col_exists = True
        if lyr_line_defn.GetFieldDefn(i).GetName() == start_y_field:
            y_col_exists = True
        if lyr_line_defn.GetFieldDefn(i).GetName() == uid_field:
            uid_col_exists = True
        if x_col_exists and y_col_exists and uid_col_exists:
            break

    if (not x_col_exists) or (not y_col_exists) or (not uid_col_exists):
        ds_line_vec = None
        raise Exception("The start x and y columns and/or UID column are not present within the input file.")

    ds_objs_vec = gdal.OpenEx(vec_objs_file, gdal.OF_READONLY)
    if ds_objs_vec is None:
        raise Exception("Could not open '{}'".format(vec_objs_file))

    lyr_objs_vec = ds_objs_vec.GetLayerByName(vec_objs_lyr)
    if lyr_objs_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_objs_lyr))

    ds_objs_sub_vec, lyr_objs_sub_vec = subsetEnvsVecLyrObj(lyr_objs_vec, vec_bbox)

    spat_ref = lyr_objs_vec.GetSpatialRef()

    out_driver = ogr.GetDriverByName(out_format)
    out_ds_obj = out_driver.CreateDataSource(out_vec_file)
    out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, spat_ref, geom_type=ogr.wkbLineString)
    uid_field_obj = ogr.FieldDefn('uid', ogr.OFTInteger)
    out_lyr_obj.CreateField(uid_field_obj)
    length_field = ogr.FieldDefn('len', ogr.OFTReal)
    out_lyr_obj.CreateField(length_field)
    feat_defn = out_lyr_obj.GetLayerDefn()

    geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
    n_obj_feats = lyr_objs_sub_vec.GetFeatureCount(True)
    geom_pbar = tqdm.tqdm(total=n_obj_feats, leave=True)
    in_obj_feat = lyr_objs_sub_vec.GetNextFeature()
    while in_obj_feat:
        geom = in_obj_feat.GetGeometryRef()
        if geom is not None:
            boundary = geom.Boundary()
            geom_collect.AddGeometry(boundary)
        in_obj_feat = lyr_objs_sub_vec.GetNextFeature()
        geom_pbar.update(1)
    geom_pbar.close()

    n_feats = lyr_line_vec.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats, leave=True)
    open_transaction = False
    counter = 0
    in_feature = lyr_line_vec.GetNextFeature()
    while in_feature:
        pbar.update(1)
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        line_geom = in_feature.GetGeometryRef()
        if line_geom is not None:
            uid_str = in_feature.GetField(uid_field)
            start_pt_x = in_feature.GetField(start_x_field)
            start_pt_y = in_feature.GetField(start_y_field)
            start_pt = ogr.Geometry(ogr.wkbPoint)
            start_pt.AddPoint(start_pt_x, start_pt_y)

            inter_geom = geom_collect.Intersection(line_geom)

            if (inter_geom is not None) and (inter_geom.GetGeometryCount() > 0):
                min_dist_pt_x = 0.0
                min_dist_pt_y = 0.0
                min_dist = 0.0
                c_pt = ogr.Geometry(ogr.wkbPoint)
                c_pt.AddPoint(0.0, 0.0)
                first_dist = True
                for i in range(inter_geom.GetGeometryCount()):
                    c_geom = inter_geom.GetGeometryRef(i)
                    pts = c_geom.GetPoints()
                    for pt in pts:
                        c_pt.SetPoint(0, pt[0], pt[1])
                        if first_dist:
                            min_dist = start_pt.Distance(c_pt)
                            min_dist_pt_x = pt[0]
                            min_dist_pt_y = pt[1]
                            first_dist = False
                        else:
                            pt_dist = start_pt.Distance(c_pt)
                            if pt_dist < min_dist:
                                min_dist = pt_dist
                                min_dist_pt_x = pt[0]
                                min_dist_pt_y = pt[1]
                out_line = ogr.Geometry(ogr.wkbLineString)
                out_line.AddPoint(start_pt_x, start_pt_y)
                out_line.AddPoint(min_dist_pt_x, min_dist_pt_y)
                out_feat = ogr.Feature(feat_defn)
                out_feat.SetGeometry(out_line)
                out_feat.SetField('uid', uid_str)
                out_feat.SetField('len', min_dist)
                out_lyr_obj.CreateFeature(out_feat)
                out_feat = None

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = lyr_line_vec.GetNextFeature()
        counter = counter + 1

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None
    ds_line_vec = None
    ds_objs_vec = None
    ds_objs_sub_vec = None


def line_intersection_range(vec_line_file, vec_line_lyr, vec_objs_file, vec_objs_lyr, out_vec_file, out_vec_lyr=None,
                            start_x_field='start_x', start_y_field='start_y', uid_field='uid', out_format="GEOJSON",
                            force=False):
    """
    A function which intersects each line within the input vector layer (vec_objs_file, vec_objs_lyr)
    creating a new line between the closest intersection to the start point of the input layer
    (defined in the vector attribute table: start_x_field, start_y_field) and the intersection point
    which is furthest to the start point.

    :param vec_line_file: Input lines vector file path.
    :param vec_line_lyr: Input lines vector layer name.
    :param vec_objs_file: The vector file for the objects (expecting polygons) to be intersected with.
    :param vec_objs_lyr: The vector layer for the objects (expecting polygons) to be intersected with.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name
    :param start_x_field: The field name for the start point X coordinate for the input lines.
    :param start_y_field: The field name for the start point Y coordinate for the input lines.
    :param uid_field: The field name for the Unique ID (UID) of the input lines.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import rsgislib
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    gdal.UseExceptions()
    rsgis_utils = rsgislib.RSGISPyUtils()
    vec_bbox = rsgis_utils.getVecLayerExtent(vec_line_file, vec_line_lyr)

    ds_line_vec = gdal.OpenEx(vec_line_file, gdal.OF_READONLY)
    if ds_line_vec is None:
        raise Exception("Could not open '{}'".format(vec_line_file))

    lyr_line_vec = ds_line_vec.GetLayerByName(vec_line_lyr)
    if lyr_line_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_line_lyr))

    x_col_exists = False
    y_col_exists = False
    uid_col_exists = False
    lyr_line_defn = lyr_line_vec.GetLayerDefn()
    for i in range(lyr_line_defn.GetFieldCount()):
        if lyr_line_defn.GetFieldDefn(i).GetName() == start_x_field:
            x_col_exists = True
        if lyr_line_defn.GetFieldDefn(i).GetName() == start_y_field:
            y_col_exists = True
        if lyr_line_defn.GetFieldDefn(i).GetName() == uid_field:
            uid_col_exists = True
        if x_col_exists and y_col_exists and uid_col_exists:
            break

    if (not x_col_exists) or (not y_col_exists) or (not uid_col_exists):
        ds_line_vec = None
        raise Exception("The start x and y columns and/or UID column are not present within the input file.")

    ds_objs_vec = gdal.OpenEx(vec_objs_file, gdal.OF_READONLY)
    if ds_objs_vec is None:
        raise Exception("Could not open '{}'".format(vec_objs_file))

    lyr_objs_vec = ds_objs_vec.GetLayerByName(vec_objs_lyr)
    if lyr_objs_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_objs_lyr))

    ds_objs_sub_vec, lyr_objs_sub_vec = subsetEnvsVecLyrObj(lyr_objs_vec, vec_bbox)

    spat_ref = lyr_objs_vec.GetSpatialRef()

    out_driver = ogr.GetDriverByName(out_format)
    out_ds_obj = out_driver.CreateDataSource(out_vec_file)
    out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, spat_ref, geom_type=ogr.wkbLineString)
    uid_field_obj = ogr.FieldDefn('uid', ogr.OFTInteger)
    out_lyr_obj.CreateField(uid_field_obj)
    length_field = ogr.FieldDefn('len', ogr.OFTReal)
    out_lyr_obj.CreateField(length_field)
    feat_defn = out_lyr_obj.GetLayerDefn()

    geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
    n_obj_feats = lyr_objs_sub_vec.GetFeatureCount(True)
    geom_pbar = tqdm.tqdm(total=n_obj_feats, leave=True)
    in_obj_feat = lyr_objs_sub_vec.GetNextFeature()
    while in_obj_feat:
        geom = in_obj_feat.GetGeometryRef()
        if geom is not None:
            boundary = geom.Boundary()
            geom_collect.AddGeometry(boundary)
        in_obj_feat = lyr_objs_sub_vec.GetNextFeature()
        geom_pbar.update(1)
    geom_pbar.close()

    n_feats = lyr_line_vec.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats, leave=True)
    open_transaction = False
    counter = 0
    in_feature = lyr_line_vec.GetNextFeature()
    while in_feature:
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        line_geom = in_feature.GetGeometryRef()
        if line_geom is not None:
            uid_str = in_feature.GetField(uid_field)
            start_pt_x = in_feature.GetField(start_x_field)
            start_pt_y = in_feature.GetField(start_y_field)
            start_pt = ogr.Geometry(ogr.wkbPoint)
            start_pt.AddPoint(start_pt_x, start_pt_y)

            inter_geom = geom_collect.Intersection(line_geom)

            if (inter_geom is not None) and (inter_geom.GetGeometryCount() > 0):
                min_dist_pt_x = 0.0
                min_dist_pt_y = 0.0
                min_dist = 0.0
                max_dist_pt_x = 0.0
                max_dist_pt_y = 0.0
                max_dist = 0.0
                c_pt = ogr.Geometry(ogr.wkbPoint)
                c_pt.AddPoint(0.0, 0.0)
                first_dist = True
                for i in range(inter_geom.GetGeometryCount()):
                    c_geom = inter_geom.GetGeometryRef(i)
                    pts = c_geom.GetPoints()
                    for pt in pts:
                        c_pt.SetPoint(0, pt[0], pt[1])
                        if first_dist:
                            min_dist = start_pt.Distance(c_pt)
                            min_dist_pt_x = pt[0]
                            min_dist_pt_y = pt[1]
                            max_dist = min_dist
                            max_dist_pt_x = min_dist_pt_x
                            max_dist_pt_y = min_dist_pt_y
                            first_dist = False
                        else:
                            pt_dist = start_pt.Distance(c_pt)
                            if pt_dist < min_dist:
                                min_dist = pt_dist
                                min_dist_pt_x = pt[0]
                                min_dist_pt_y = pt[1]
                            if pt_dist > max_dist:
                                max_dist = pt_dist
                                max_dist_pt_x = pt[0]
                                max_dist_pt_y = pt[1]
                out_line = ogr.Geometry(ogr.wkbLineString)
                out_line.AddPoint(min_dist_pt_x, min_dist_pt_y)
                out_line.AddPoint(max_dist_pt_x, max_dist_pt_y)
                out_feat = ogr.Feature(feat_defn)
                out_feat.SetGeometry(out_line)
                out_feat.SetField('uid', uid_str)
                start_pt.SetPoint(0, min_dist_pt_x, min_dist_pt_y)
                c_pt.SetPoint(0, max_dist_pt_x, max_dist_pt_y)
                dist = start_pt.Distance(c_pt)
                out_feat.SetField('len', dist)
                out_lyr_obj.CreateFeature(out_feat)
                out_feat = None

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = lyr_line_vec.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None
    ds_line_vec = None
    ds_objs_vec = None
    ds_objs_sub_vec = None



def scnd_line_intersection_range(vec_line_file, vec_line_lyr, vec_objs_file, vec_objs_lyr, out_vec_file,
                                 out_vec_lyr=None, start_x_field='start_x', start_y_field='start_y', uid_field='uid',
                                 out_format="GEOJSON", force=False):
    """
    A function which intersects a line with a set of polygons outputting the lines cut to their second point
    of intersection. Assume, first point of intersection would be entering the polygon and the second point
    of intersection would be leaving the polygon.

    :param vec_line_file: Input lines vector file path.
    :param vec_line_lyr: Input lines vector layer name.
    :param vec_objs_file: The vector file for the objects (expecting polygons) to be intersected with.
    :param vec_objs_lyr: The vector layer for the objects (expecting polygons) to be intersected with.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name
    :param start_x_field: The field name for the start point X coordinate for the input lines.
    :param start_y_field: The field name for the start point Y coordinate for the input lines.
    :param uid_field: The field name for the Unique ID (UID) of the input lines.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import rsgislib
    import rsgislib.vectorutils
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    gdal.UseExceptions()
    rsgis_utils = rsgislib.RSGISPyUtils()
    vec_bbox = rsgis_utils.getVecLayerExtent(vec_line_file, vec_line_lyr)

    ds_line_vec = gdal.OpenEx(vec_line_file, gdal.OF_READONLY)
    if ds_line_vec is None:
        raise Exception("Could not open '{}'".format(vec_line_file))

    lyr_line_vec = ds_line_vec.GetLayerByName(vec_line_lyr)
    if lyr_line_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_line_lyr))

    x_col_exists = False
    y_col_exists = False
    uid_col_exists = False
    lyr_line_defn = lyr_line_vec.GetLayerDefn()
    for i in range(lyr_line_defn.GetFieldCount()):
        if lyr_line_defn.GetFieldDefn(i).GetName() == start_x_field:
            x_col_exists = True
        if lyr_line_defn.GetFieldDefn(i).GetName() == start_y_field:
            y_col_exists = True
        if lyr_line_defn.GetFieldDefn(i).GetName() == uid_field:
            uid_col_exists = True
        if x_col_exists and y_col_exists and uid_col_exists:
            break

    if (not x_col_exists) or (not y_col_exists) or (not uid_col_exists):
        ds_line_vec = None
        raise Exception("The start x and y columns and/or UID column are not present within the input file.")

    ds_objs_vec = gdal.OpenEx(vec_objs_file, gdal.OF_READONLY)
    if ds_objs_vec is None:
        raise Exception("Could not open '{}'".format(vec_objs_file))

    lyr_objs_vec = ds_objs_vec.GetLayerByName(vec_objs_lyr)
    if lyr_objs_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_objs_lyr))

    ds_objs_sub_vec, lyr_objs_sub_vec = rsgislib.vectorutils.subsetEnvsVecLyrObj(lyr_objs_vec, vec_bbox)

    spat_ref = lyr_objs_vec.GetSpatialRef()

    out_driver = ogr.GetDriverByName(out_format)
    out_ds_obj = out_driver.CreateDataSource(out_vec_file)
    out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, spat_ref, geom_type=ogr.wkbLineString)
    uid_field_out_obj = ogr.FieldDefn('uid', ogr.OFTInteger)
    out_lyr_obj.CreateField(uid_field_out_obj)
    start_x_out_field = ogr.FieldDefn('start_x', ogr.OFTReal)
    out_lyr_obj.CreateField(start_x_out_field)
    start_y_out_field = ogr.FieldDefn('start_y', ogr.OFTReal)
    out_lyr_obj.CreateField(start_y_out_field)
    end_x_out_field = ogr.FieldDefn('end_x', ogr.OFTReal)
    out_lyr_obj.CreateField(end_x_out_field)
    end_y_out_field = ogr.FieldDefn('end_y', ogr.OFTReal)
    out_lyr_obj.CreateField(end_y_out_field)
    length_field = ogr.FieldDefn('len', ogr.OFTReal)
    out_lyr_obj.CreateField(length_field)
    feat_defn = out_lyr_obj.GetLayerDefn()

    geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
    n_obj_feats = lyr_objs_sub_vec.GetFeatureCount(True)
    geom_pbar = tqdm.tqdm(total=n_obj_feats, leave=True)
    in_obj_feat = lyr_objs_sub_vec.GetNextFeature()
    while in_obj_feat:
        geom = in_obj_feat.GetGeometryRef()
        if geom is not None:
            boundary = geom.Boundary()
            geom_collect.AddGeometry(boundary)
        in_obj_feat = lyr_objs_sub_vec.GetNextFeature()
        geom_pbar.update(1)
    geom_pbar.close()

    n_feats = lyr_line_vec.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats, leave=True)
    open_transaction = False
    counter = 0
    in_feature = lyr_line_vec.GetNextFeature()
    while in_feature:
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        line_geom = in_feature.GetGeometryRef()
        if line_geom is not None:
            uid_str = in_feature.GetField(uid_field)
            start_pt_x = in_feature.GetField(start_x_field)
            start_pt_y = in_feature.GetField(start_y_field)
            start_pt = ogr.Geometry(ogr.wkbPoint)
            start_pt.AddPoint(start_pt_x, start_pt_y)

            inter_geom = geom_collect.Intersection(line_geom)

            if (inter_geom is not None) and (inter_geom.GetGeometryCount() > 0):
                min_dist_pt_x = 0.0
                min_dist_pt_y = 0.0
                min_dist = 0.0
                sec_dist_pt_x = 0.0
                sec_dist_pt_y = 0.0
                sec_dist = 0.0
                c_pt = ogr.Geometry(ogr.wkbPoint)
                c_pt.AddPoint(0.0, 0.0)
                first_dist = True
                first_sec = True
                for i in range(inter_geom.GetGeometryCount()):
                    c_geom = inter_geom.GetGeometryRef(i)
                    pts = c_geom.GetPoints()
                    for pt in pts:
                        c_pt.SetPoint(0, pt[0], pt[1])
                        if first_dist:
                            min_dist = start_pt.Distance(c_pt)
                            min_dist_pt_x = pt[0]
                            min_dist_pt_y = pt[1]
                            first_dist = False
                        elif first_sec:
                            pt_dist = start_pt.Distance(c_pt)
                            if pt_dist < min_dist:
                                sec_dist = min_dist
                                sec_dist_pt_x = min_dist_pt_x
                                sec_dist_pt_y = min_dist_pt_y
                                min_dist = pt_dist
                                min_dist_pt_x = pt[0]
                                min_dist_pt_y = pt[1]
                            else:
                                sec_dist = pt_dist
                                sec_dist_pt_x = pt[0]
                                sec_dist_pt_y = pt[1]
                            first_sec = False
                        else:
                            pt_dist = start_pt.Distance(c_pt)
                            if pt_dist < min_dist:
                                sec_dist = min_dist
                                sec_dist_pt_x = min_dist_pt_x
                                sec_dist_pt_y = min_dist_pt_y
                                min_dist = pt_dist
                                min_dist_pt_x = pt[0]
                                min_dist_pt_y = pt[1]
                            elif pt_dist < sec_dist:
                                sec_dist = pt_dist
                                sec_dist_pt_x = pt[0]
                                sec_dist_pt_y = pt[1]

                out_line = ogr.Geometry(ogr.wkbLineString)
                out_line.AddPoint(start_pt_x, start_pt_y)
                out_line.AddPoint(sec_dist_pt_x, sec_dist_pt_y)
                out_feat = ogr.Feature(feat_defn)
                out_feat.SetGeometry(out_line)
                out_feat.SetField('uid', uid_str)
                c_pt.SetPoint(0, sec_dist_pt_x, sec_dist_pt_y)
                dist = start_pt.Distance(c_pt)
                out_feat.SetField('len', dist)
                out_feat.SetField('start_x', start_pt_x)
                out_feat.SetField('start_y', start_pt_y)
                out_feat.SetField('end_x', sec_dist_pt_x)
                out_feat.SetField('end_y', sec_dist_pt_y)
                out_lyr_obj.CreateFeature(out_feat)
                out_feat = None

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = lyr_line_vec.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None
    ds_line_vec = None
    ds_objs_vec = None
    ds_objs_sub_vec = None


def vector_translate(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr=None, out_vec_drv='GPKG',
                     drv_create_opts=[], lyr_create_opts=[], access_mode='overwrite', src_srs=None,
                     dst_srs=None, force=False):
    """
    A function which translates a vector file to another format, similar to ogr2ogr. If you wish
    to reproject the input file then provide a destination srs (e.g., "EPSG:27700", or wkt string,
    or proj4 string).

    :param in_vec_file: the input vector file.
    :param in_vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same as the input).
    :param out_vec_drv: the output vector file format (e.g., GPKG, GEOJSON, ESRI Shapefile, etc.)
    :param drv_create_opts: a list of options for the creation of the output file.
    :param lyr_create_opts: a list of options for the creation of the output layer.
    :param access_mode: by default the function overwrites the output file but other
                        options are: ['update', 'append', 'overwrite']
    :param src_srs: provide a source spatial reference for the input vector file. Default=None.
                    can be used to provide a projection where none has been specified or the
                    information has gone missing. Can be used without performing a reprojection.
    :param dst_srs: provide a spatial reference for the output vector file to be reprojected to. (Default=None)
                    If specified then the file will be reprojected.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    gdal.UseExceptions()

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    if out_vec_lyr is None:
        out_vec_lyr = in_vec_lyr

    reproject_lyr = False
    if dst_srs is not None:
        reproject_lyr = True

    if src_srs is not None:
        if dst_srs is None:
            dst_srs = src_srs

    opts = gdal.VectorTranslateOptions(options=drv_create_opts,
                                       format=out_vec_drv,
                                       accessMode=access_mode,
                                       srcSRS=src_srs,
                                       dstSRS=dst_srs,
                                       reproject=reproject_lyr,
                                       layerCreationOptions=lyr_create_opts,
                                       layers=in_vec_lyr,
                                       layerName=out_vec_lyr,
                                       callback=callback)

    gdal.VectorTranslate(out_vec_file, in_vec_file, options=opts)

def get_geom_pts(geom, pts_lst=None):
    """
    Recursive function which extracts all the points within the an OGR geometry.

    :param geom: The geometry from with the points are extracted.
    :param pts_lst: The list for the points, if None a list will be created.
    :return: a list of points.

    """
    from osgeo import ogr
    if pts_lst is None:
        pts_lst = list()

    if geom is not None:
        if geom.GetGeometryType() == ogr.wkbPoint:
            pts_lst.append(geom)
        else:
            n_geoms = geom.GetGeometryCount()
            for n in range(0, n_geoms):
                c_geom = geom.GetGeometryRef(n)
                n_pts = c_geom.GetPointCount()
                if n_pts == 0:
                    get_geom_pts(c_geom, pts_lst)
                else:
                    for i in range(0, n_pts):
                        pt = c_geom.GetPoint(i)
                        pts_lst.append(pt)
    return pts_lst


def get_vec_lyr_as_pts(in_vec_file, in_vec_lyr):
    """
    Get a list of points from the vectors within an input file.

    :param in_vec_file: Input vector file
    :param in_vec_lyr: Input vector layer name
    :return: returns a list of points.

    """
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
            get_geom_pts(geom, pts_lst)
        in_feature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1
        pbar.update(1)
    pbar.close()
    return pts_lst


def reproj_wgs84_vec_to_utm(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr=None, use_hemi=True,
                            out_vec_drv='GPKG', drv_create_opts=[], lyr_create_opts=[],
                            access_mode='overwrite', force=False):
    """
    A function which reprojects an input file projected in WGS84 (EPSG:4326) to UTM, where the UTM zone is
    automatically identified using the mean x and y.

    :param in_vec_file: the input vector file.
    :param in_vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same as the input).
    :param use_hemi: True differentiate between Southern and Northern hemisphere. False use Northern hemisphere.
    :param out_vec_drv: the output vector file format (e.g., GPKG, GEOJSON, ESRI Shapefile, etc.)
    :param drv_create_opts: a list of options for the creation of the output file.
    :param lyr_create_opts: a list of options for the creation of the output layer.
    :param access_mode: by default the function overwrites the output file but other
                        options are: ['update', 'append', 'overwrite']
    :param force: remove output file if it exists.

    """
    import rsgislib.tools.utm
    from rsgislib.vectorutils import vector_translate
    from osgeo import gdal
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

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
            get_geom_pts(geom, pts_lst)
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
    hemi = 'N'
    if use_hemi and (lat < 0):
        hemi = 'S'
    print("UTM Zone: {}{}".format(utm_zone, hemi))

    out_epsg = rsgislib.tools.utm.epsg_for_UTM(utm_zone, hemi)
    print("EPSG: {}".format(out_epsg))

    dst_srs_str = "EPSG:{}".format(out_epsg)
    vector_translate(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr, out_vec_drv,
                     drv_create_opts, lyr_create_opts, access_mode, src_srs='EPSG:4326',
                     dst_srs=dst_srs_str)


def create_alpha_shape(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr, out_vec_drv='GEOJSON', alpha_val=None,
                       alpha_vals=None, max_iter=10000, force=False):
    """
    Function which calculate an alpha shape for a set of vector features (which are converted to points).

    For this function to work you need the alphashapes module installed:
    https://alphashape.readthedocs.io
    https://github.com/bellockk/alphashape

    :param in_vec_file: the input vector file.
    :param in_vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same as the input).
    :param out_vec_drv: the output vector file format (e.g., GPKG, GEOJSON, ESRI Shapefile, etc.)
    :param alpha_val: The alpha value to create the the alpha shape polygon. If None then a value will be
                      automatically calculate but warning this can a significant amount of time (i.e., hours!!)
    :param alpha_vals: Alternatively, a list of alpha values can be provided (e.g., [75, 50, 25, 5, 2]) where
                       first to produce a valid result will be outputted. i.e., the order you provide the alpha
                       values will be the order they are tested. If None then the alpha_val parameter will be used.
    :param max_iter: The maximum number of iterations for automatically selecting the alpha value. Note if the number
                     iteration is not sufficient to find an optimum value then no value is returned.
    :param force: remove output file if it exists.
    :return: (vec_output, alpha_val); vec_output is a boolean True an output produced; False no output,
             alpha_val - the alpha value used for the analysis. If a single value was inputted then the
             same value will be outputted.

    """
    import alphashape
    from osgeo import ogr
    from osgeo import gdal
    import os

    gdal.UseExceptions()

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    def _rescale_polygon(in_poly, min_x, min_y, ran_x, ran_y):
        ext_ring = ogr.Geometry(ogr.wkbLinearRing)
        for pt in in_poly.exterior.coords:
            ext_ring.AddPoint(((pt[0] * ran_x) + min_x), ((pt[1] * ran_y) + min_y))
        n_poly = ogr.Geometry(ogr.wkbPolygon)
        n_poly.AddGeometry(ext_ring)

        for int_ring in in_poly.interiors:
            n_int_ring = ogr.Geometry(ogr.wkbLinearRing)
            for pt in int_ring.coords:
                n_int_ring.AddPoint(((pt[0] * ran_x) + min_x), ((pt[1] * ran_y) + min_y))
            n_poly.AddGeometry(n_int_ring)
        return n_poly

    pts = get_vec_lyr_as_pts(in_vec_file, in_vec_lyr)
    min_x = 0.0
    min_y = 0.0
    max_x = 0.0
    max_y = 0.0
    first = True
    for pt in pts:
        if first:
            min_x = pt[0]
            min_y = pt[1]
            max_x = pt[0]
            max_y = pt[1]
            first = False
        else:
            if pt[0] < min_x:
                min_x = pt[0]
            if pt[1] < min_y:
                min_y = pt[1]
            if pt[0] > max_x:
                max_x = pt[0]
            if pt[1] > max_y:
                max_y = pt[1]

    print("Min: {}, {}".format(min_x, min_y))
    print("Max: {}, {}".format(max_x, max_y))

    ran_x = max_x - min_x
    ran_y = max_y - min_y
    print("Range: {}, {}".format(ran_x, ran_y))

    norm_pts = list()
    for pt in pts:
        norm_x = (pt[0] - min_x) / ran_x
        norm_y = (pt[1] - min_y) / ran_y
        norm_pts.append((norm_x, norm_y))

    if alpha_vals is not None:
        for alpha_test_val in alpha_vals:
            print("Trying Alpha: {}".format(alpha_test_val))
            alpha_shape = alphashape.alphashape(norm_pts, alpha=alpha_test_val)
            if alpha_shape.geom_type == 'MultiPolygon' or alpha_shape.geom_type == 'Polygon':
                alpha_val = alpha_test_val
                break
        print("Final Alpha: {}".format(alpha_val))
    else:
        if alpha_val is None:
            alpha_val = alphashape.optimizealpha(norm_pts, max_iterations=max_iter)

        print("Alpha: {}".format(alpha_val))
        alpha_shape = alphashape.alphashape(norm_pts, alpha=alpha_val)

    ogr_geom_type = ogr.wkbPolygon
    if alpha_shape.geom_type == 'MultiPolygon':
        ogr_geom_type = ogr.wkbMultiPolygon
        out_alpha_shape = ogr.Geometry(ogr.wkbMultiPolygon)
        for poly in alpha_shape:
            out_alpha_shape.AddGeometry(_rescale_polygon(poly, min_x, min_y, ran_x, ran_y))
    elif alpha_shape.geom_type == 'Polygon':
        ogr_geom_type = ogr.wkbPolygon
        out_alpha_shape = _rescale_polygon(alpha_shape, min_x, min_y, ran_x, ran_y)
    else:
        out_alpha_shape = None
        print("No output, did not create an output polygon or multipolygon...")

    vec_output = False
    if out_alpha_shape is not None:
        vecDS = gdal.OpenEx(in_vec_file, gdal.OF_VECTOR)
        if vecDS is None:
            raise Exception("Could not open '{}'".format(in_vec_file))
        veclyr = vecDS.GetLayerByName(in_vec_lyr)
        if veclyr is None:
            raise Exception("Could not open layer '{}'".format(in_vec_lyr))
        lyr_spat_ref = veclyr.GetSpatialRef()
        vecDS = None

        out_driver = ogr.GetDriverByName(out_vec_drv)
        result_ds = out_driver.CreateDataSource(out_vec_file)
        if result_ds is None:
            raise Exception("Could not open '{}'".format(out_vec_file))

        result_lyr = result_ds.CreateLayer(out_vec_lyr, lyr_spat_ref, geom_type=ogr_geom_type)
        if result_lyr is None:
            raise Exception("Could not open layer '{}'".format(out_vec_lyr))

        # Get the output Layer's Feature Definition
        featureDefn = result_lyr.GetLayerDefn()
        outFeature = ogr.Feature(featureDefn)
        outFeature.SetGeometry(out_alpha_shape)
        result_lyr.CreateFeature(outFeature)
        outFeature = None
        result_ds = None
        vec_output = True

    return vec_output, alpha_val


def convert_multi_geoms_to_single(vecfile, veclyrname, outVecDrvr, vecoutfile, vecoutlyrname, force=False):
    """
    A convert any multiple geometries into single geometries.

    :param vecfile: input vector file
    :param veclyrname: input vector layer within the input file.
    :param outVecDrvr: the format driver for the output vector file (e.g., GPKG, ESRI Shapefile).
    :param vecoutfile: output file path for the vector.
    :param vecoutlyrname: output vector layer name.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import tqdm
    gdal.UseExceptions()

    if os.path.exists(vecoutfile):
        if force:
            delete_vector_file(vecoutfile)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vecoutfile))

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile))

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))
    lyr_spat_ref = veclyr.GetSpatialRef()
    geom_type = veclyr.GetGeomType()
    if geom_type == ogr.wkbMultiPoint:
        geom_type = ogr.wkbPoint
        print("Changing to Point Type from Multi-Point")
    elif geom_type == ogr.wkbMultiLineString:
        geom_type = ogr.wkbLineString
        print("Changing to Line Type from Multi-Line")
    elif geom_type == ogr.wkbMultiPolygon:
        geom_type = ogr.wkbPolygon
        print("Changing to Polygon Type from Multi-Polygon")

    out_driver = ogr.GetDriverByName(outVecDrvr)
    result_ds = out_driver.CreateDataSource(vecoutfile)
    if result_ds is None:
        raise Exception("Could not open '{}'".format(vecoutfile))

    result_lyr = result_ds.CreateLayer(vecoutlyrname, lyr_spat_ref, geom_type=geom_type)
    if result_lyr is None:
        raise Exception("Could not open layer '{}'".format(vecoutlyrname))

    featDefn = result_lyr.GetLayerDefn()

    openTransaction = False
    veclyr.ResetReading()
    n_feats = veclyr.GetFeatureCount(True)
    counter = 0
    pbar = tqdm.tqdm(total=n_feats)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if not openTransaction:
            result_lyr.StartTransaction()
            openTransaction = True

        geom_ref = feat.GetGeometryRef()

        if geom_ref.GetGeometryName().lower() == 'multipolygon':
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                outFeat = ogr.Feature(featDefn)
                outFeat.SetGeometry(g)
                result_lyr.CreateFeature(outFeat)
        elif geom_ref.GetGeometryName().lower() == 'multilinestring':
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                outFeat = ogr.Feature(featDefn)
                outFeat.SetGeometry(g)
                result_lyr.CreateFeature(outFeat)
        elif geom_ref.GetGeometryName().lower() == 'multipoint':
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                outFeat = ogr.Feature(featDefn)
                outFeat.SetGeometry(g)
                result_lyr.CreateFeature(outFeat)
        elif geom_ref.GetGeometryName().lower() == 'geometrycollection':
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                outFeat = ogr.Feature(featDefn)
                outFeat.SetGeometry(g)
                result_lyr.CreateFeature(outFeat)
        else:
            outFeat = ogr.Feature(featDefn)
            outFeat.SetGeometry(geom_ref)
            result_lyr.CreateFeature(outFeat)

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        feat = veclyr.GetNextFeature()
        counter += 1
        pbar.update(1)

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    pbar.close()

    vecDS = None
    result_ds = None


def simplify_geometries(vecfile, veclyrname, tolerance, outVecDrvr, vecoutfile, vecoutlyrname, force=False):
    """
Create a simplified version of the input

:param vecfile: input vector file
:param veclyrname: input vector layer within the input file.
:param tolerance: simplification tolerance
:param outVecDrvr: the format driver for the output vector file (e.g., GPKG, ESRI Shapefile).
:param vecoutfile: output file path for the vector.
:param vecoutlyrname: output vector layer name.
:param force: remove output file if it exists.

"""
    from osgeo import gdal
    from osgeo import ogr
    import tqdm
    gdal.UseExceptions()

    if os.path.exists(vecoutfile):
        if force:
            delete_vector_file(vecoutfile)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vecoutfile))

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile))

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))
    lyr_spat_ref = veclyr.GetSpatialRef()
    geom_type = veclyr.GetGeomType()

    out_driver = ogr.GetDriverByName(outVecDrvr)
    result_ds = out_driver.CreateDataSource(vecoutfile)
    if result_ds is None:
        raise Exception("Could not open '{}'".format(vecoutfile))

    result_lyr = result_ds.CreateLayer(vecoutlyrname, lyr_spat_ref, geom_type=geom_type)
    if result_lyr is None:
        raise Exception("Could not open layer '{}'".format(vecoutlyrname))

    featDefn = result_lyr.GetLayerDefn()

    openTransaction = False
    veclyr.ResetReading()
    n_feats = veclyr.GetFeatureCount(True)
    counter = 0
    pbar = tqdm.tqdm(total=n_feats)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if not openTransaction:
            result_lyr.StartTransaction()
            openTransaction = True

        geom = feat.GetGeometryRef().Simplify(tolerance)
        outFeat = ogr.Feature(featDefn)
        outFeat.SetGeometry(geom)
        result_lyr.CreateFeature(outFeat)

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        feat = veclyr.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    pbar.close()

    vecDS = None
    result_ds = None


def delete_polygon_holes(vecfile, veclyrname, outVecDrvr, vecoutfile, vecoutlyrname, area_thres=None, force=False):
    """
Delete holes from the input polygons in below the area threshold.

:param vecfile: input vector file
:param veclyrname: input vector layer within the input file.
:param outVecDrvr: the format driver for the output vector file (e.g., GPKG, ESRI Shapefile).
:param vecoutfile: output file path for the vector.
:param vecoutlyrname: output vector layer name.
:param area_thres: threshold below which holes are removed. If threshold is None then all holes are removed.
:param force: remove output file if it exists.

"""
    from osgeo import gdal
    from osgeo import ogr
    import tqdm
    gdal.UseExceptions()

    if os.path.exists(vecoutfile):
        if force:
            delete_vector_file(vecoutfile)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vecoutfile))

    def _remove_holes_polygon(polygon, area_thres=None):
        if polygon.GetGeometryName().lower() != 'polygon':
            raise Exception("Can only remove holes from polygon geometry.")
        if polygon.GetGeometryCount() == 1:
            return polygon

        if area_thres is None:
            outer_ring = polygon.GetGeometryRef(0)
            poly = ogr.Geometry(ogr.wkbPolygon)
            poly.AddGeometry(outer_ring)
            return poly
        else:
            outer_ring = polygon.GetGeometryRef(0)
            poly = ogr.Geometry(ogr.wkbPolygon)
            poly.AddGeometry(outer_ring)
            for i in range(polygon.GetGeometryCount()):
                if i > 0:
                    c_ring = polygon.GetGeometryRef(i)
                    tmp_poly = ogr.Geometry(ogr.wkbPolygon)
                    tmp_poly.AddGeometry(c_ring)
                    if tmp_poly.Area() > area_thres:
                        poly.AddGeometry(c_ring)
            return poly

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile))

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))
    lyr_spat_ref = veclyr.GetSpatialRef()
    geom_type = veclyr.GetGeomType()

    out_driver = ogr.GetDriverByName(outVecDrvr)
    result_ds = out_driver.CreateDataSource(vecoutfile)
    if result_ds is None:
        raise Exception("Could not open '{}'".format(vecoutfile))

    result_lyr = result_ds.CreateLayer(vecoutlyrname, lyr_spat_ref, geom_type=geom_type)
    if result_lyr is None:
        raise Exception("Could not open layer '{}'".format(vecoutlyrname))

    featDefn = result_lyr.GetLayerDefn()

    openTransaction = False
    veclyr.ResetReading()
    n_feats = veclyr.GetFeatureCount(True)
    counter = 0
    pbar = tqdm.tqdm(total=n_feats)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if not openTransaction:
            result_lyr.StartTransaction()
            openTransaction = True

        geom_ref = feat.GetGeometryRef()
        if geom_ref.GetGeometryName().lower() == 'multipolygon':
            out_geom = ogr.Geometry(ogr.wkbMultiPolygon)
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                out_geom.AddGeometry(_remove_holes_polygon(g, area_thres))
        elif geom_ref.GetGeometryName().lower() == 'polygon':
            out_geom = _remove_holes_polygon(geom_ref, area_thres)

        if out_geom is not None:
            outFeat = ogr.Feature(featDefn)
            outFeat.SetGeometry(out_geom)
            result_lyr.CreateFeature(outFeat)

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        feat = veclyr.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    pbar.close()

    vecDS = None
    result_ds = None


def get_poly_hole_area(vecfile, veclyrname):
    """
Get an array of the areas of the polygon holes.

:param vecfile: input vector file
:param veclyrname: input vector layer within the input file.
:returns: A list of areas.

"""
    from osgeo import gdal
    from osgeo import ogr
    import tqdm
    gdal.UseExceptions()

    def _calc_hole_area(polygon):
        if polygon.GetGeometryName().lower() != 'polygon':
            raise Exception("Can only remove holes from polygon geometry.")
        if polygon.GetGeometryCount() == 1:
            return []
        else:
            areas = []
            for i in range(polygon.GetGeometryCount()):
                if i > 0:
                    c_ring = polygon.GetGeometryRef(i)
                    tmp_poly = ogr.Geometry(ogr.wkbPolygon)
                    tmp_poly.AddGeometry(c_ring)
                    areas.append(tmp_poly.Area())
            return areas

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile))

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))

    veclyr.ResetReading()
    n_feats = veclyr.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    hole_areas = []
    while feat is not None:
        geom_ref = feat.GetGeometryRef()
        if geom_ref.GetGeometryName().lower() == 'multipolygon':
            out_geom = ogr.Geometry(ogr.wkbMultiPolygon)
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                areas = _calc_hole_area(g)
                if len(areas) > 0:
                    hole_areas += areas
        elif geom_ref.GetGeometryName().lower() == 'polygon':
            areas = _calc_hole_area(geom_ref)
            if len(areas) > 0:
                hole_areas += areas

        feat = veclyr.GetNextFeature()
        pbar.update(1)

    pbar.close()
    vecDS = None
    return hole_areas


def remove_polygon_area(vecfile, veclyrname, outVecDrvr, vecoutfile, vecoutlyrname, area_thres, force=False):
    """
Delete polygons with an area below a defined threshold.

:param vecfile: input vector file
:param veclyrname: input vector layer within the input file.
:param outVecDrvr: the format driver for the output vector file (e.g., GPKG, ESRI Shapefile).
:param vecoutfile: output file path for the vector.
:param vecoutlyrname: output vector layer name.
:param area_thres: threshold below which polygons are removed.
:param force: remove output file if it exists.

"""
    from osgeo import gdal
    from osgeo import ogr
    import tqdm
    gdal.UseExceptions()

    if os.path.exists(vecoutfile):
        if force:
            delete_vector_file(vecoutfile)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(vecoutfile))

    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR)
    if vecDS is None:
        raise Exception("Could not open '{}'".format(vecfile))

    veclyr = vecDS.GetLayerByName(veclyrname)
    if veclyr is None:
        raise Exception("Could not open layer '{}'".format(veclyrname))
    lyr_spat_ref = veclyr.GetSpatialRef()
    geom_type = veclyr.GetGeomType()

    out_driver = ogr.GetDriverByName(outVecDrvr)
    result_ds = out_driver.CreateDataSource(vecoutfile)
    if result_ds is None:
        raise Exception("Could not open '{}'".format(vecoutfile))

    result_lyr = result_ds.CreateLayer(vecoutlyrname, lyr_spat_ref, geom_type=geom_type)
    if result_lyr is None:
        raise Exception("Could not open layer '{}'".format(vecoutlyrname))

    featDefn = result_lyr.GetLayerDefn()

    openTransaction = False
    veclyr.ResetReading()
    n_feats = veclyr.GetFeatureCount(True)
    counter = 0
    pbar = tqdm.tqdm(total=n_feats)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if not openTransaction:
            result_lyr.StartTransaction()
            openTransaction = True

        geom_ref = feat.GetGeometryRef()
        if geom_ref.GetGeometryName().lower() == 'multipolygon':
            out_geom = ogr.Geometry(ogr.wkbMultiPolygon)
            n_geoms = 0
            for i in range(0, geom_ref.GetGeometryCount()):
                g = geom_ref.GetGeometryRef(i)
                if g.Area() > area_thres:
                    out_geom.AddGeometry(g)
                    n_geoms += 1
            if n_geoms > 0:
                outFeat = ogr.Feature(featDefn)
                outFeat.SetGeometry(out_geom)
                result_lyr.CreateFeature(outFeat)
        elif geom_ref.GetGeometryName().lower() == 'polygon':
            if geom_ref.Area() > area_thres:
                outFeat = ogr.Feature(featDefn)
                outFeat.SetGeometry(geom_ref)
                result_lyr.CreateFeature(outFeat)

        if ((counter % 20000) == 0) and openTransaction:
            result_lyr.CommitTransaction()
            openTransaction = False

        feat = veclyr.GetNextFeature()
        counter = counter + 1
        pbar.update(1)

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    pbar.close()

    vecDS = None
    result_ds = None


def vec_lyr_intersection(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None,
                         out_format="GEOJSON", force=False):
    """
    A function which performs an intersection between the vector layer and the overlain vector.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import rsgislib
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    gdal.UseExceptions()
    rsgis_utils = rsgislib.RSGISPyUtils()
    vec_bbox = rsgis_utils.getVecLayerExtent(vec_file, vec_lyr)

    ds_in_vec = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if ds_in_vec is None:
        raise Exception("Could not open '{}'".format(vec_file))

    lyr_in_vec = ds_in_vec.GetLayerByName(vec_lyr)
    if lyr_in_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))
    spat_ref = lyr_in_vec.GetSpatialRef()
    geom_type = lyr_in_vec.GetGeomType()

    ds_over_vec = gdal.OpenEx(vec_over_file, gdal.OF_READONLY)
    if ds_over_vec is None:
        raise Exception("Could not open '{}'".format(vec_over_file))

    lyr_over_vec = ds_over_vec.GetLayerByName(vec_over_lyr)
    if lyr_over_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_over_lyr))

    out_driver = ogr.GetDriverByName(out_format)
    out_ds_obj = out_driver.CreateDataSource(out_vec_file)
    out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, spat_ref, geom_type=geom_type)
    feat_defn = out_lyr_obj.GetLayerDefn()

    geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
    n_obj_feats = lyr_over_vec.GetFeatureCount(True)
    geom_pbar = tqdm.tqdm(total=n_obj_feats, leave=True)
    lyr_over_vec.ResetReading()
    in_obj_feat = lyr_over_vec.GetNextFeature()
    while in_obj_feat:
        geom = in_obj_feat.GetGeometryRef()
        if geom is not None:
            geom_collect.AddGeometry(geom)
        in_obj_feat = lyr_over_vec.GetNextFeature()
        geom_pbar.update(1)
    geom_pbar.close()

    n_feats = lyr_in_vec.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats, leave=True)
    open_transaction = False
    counter = 0
    lyr_in_vec.ResetReading()
    in_feature = lyr_in_vec.GetNextFeature()
    while in_feature:
        pbar.update(1)
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        in_geom = in_feature.GetGeometryRef()
        if in_geom is not None:
            op_out_geom = geom_collect.Intersection(in_geom)

            if (op_out_geom is not None) and (op_out_geom.GetGeometryCount() > 0):
                for i in range(op_out_geom.GetGeometryCount()):
                    c_geom = op_out_geom.GetGeometryRef(i)

                    if ((c_geom.GetGeometryName().upper() == "POLYGON") or (
                            c_geom.GetGeometryName().upper() == "MULTIPOLYGON")) and (
                            (geom_type == ogr.wkbMultiPolygon) or (geom_type == ogr.wkbPolygon)):
                        out_feat = ogr.Feature(feat_defn)
                        out_feat.SetGeometry(c_geom)
                        out_lyr_obj.CreateFeature(out_feat)
                        out_feat = None
                    elif ((c_geom.GetGeometryName().upper() == "LINESTRING") or (
                            c_geom.GetGeometryName().upper() == "MULTILINESTRING")) and (
                            (geom_type == ogr.wkbMultiLineString) or (geom_type == ogr.wkbLineString)):
                        out_feat = ogr.Feature(feat_defn)
                        out_feat.SetGeometry(c_geom)
                        out_lyr_obj.CreateFeature(out_feat)
                        out_feat = None
                    elif ((c_geom.GetGeometryName().upper() == "POINT") or (
                            c_geom.GetGeometryName().upper() == "MULTIPOINT")) and (
                            (geom_type == ogr.wkbMultiPoint) or (geom_type == ogr.wkbPoint)):
                        out_feat = ogr.Feature(feat_defn)
                        out_feat.SetGeometry(c_geom)
                        out_lyr_obj.CreateFeature(out_feat)
                        out_feat = None

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = lyr_in_vec.GetNextFeature()
        counter = counter + 1

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None
    ds_in_vec = None
    ds_over_vec = None


def vec_lyr_difference(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None,
                       out_format="GEOJSON", symmetric=False, force=False):
    """
    A function which performs an difference between the vector layer and the overlain vector.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param symmetric: If True then the symmetric difference will be taken.
    :param force: remove output file if it exists.

    """
    from osgeo import gdal
    from osgeo import ogr
    import rsgislib
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    gdal.UseExceptions()
    rsgis_utils = rsgislib.RSGISPyUtils()
    vec_bbox = rsgis_utils.getVecLayerExtent(vec_file, vec_lyr)

    ds_in_vec = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if ds_in_vec is None:
        raise Exception("Could not open '{}'".format(vec_file))

    lyr_in_vec = ds_in_vec.GetLayerByName(vec_lyr)
    if lyr_in_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))
    spat_ref = lyr_in_vec.GetSpatialRef()
    geom_type = lyr_in_vec.GetGeomType()

    ds_over_vec = gdal.OpenEx(vec_over_file, gdal.OF_READONLY)
    if ds_over_vec is None:
        raise Exception("Could not open '{}'".format(vec_over_file))

    lyr_over_vec = ds_over_vec.GetLayerByName(vec_over_lyr)
    if lyr_over_vec is None:
        raise Exception("Could not find layer '{}'".format(vec_over_lyr))

    out_driver = ogr.GetDriverByName(out_format)
    out_ds_obj = out_driver.CreateDataSource(out_vec_file)
    out_lyr_obj = out_ds_obj.CreateLayer(out_vec_lyr, spat_ref, geom_type=geom_type)
    feat_defn = out_lyr_obj.GetLayerDefn()

    geoms_lst = list()
    n_obj_feats = lyr_over_vec.GetFeatureCount(True)
    geom_pbar = tqdm.tqdm(total=n_obj_feats, leave=True)
    lyr_over_vec.ResetReading()
    in_obj_feat = lyr_over_vec.GetNextFeature()
    while in_obj_feat:
        geom = in_obj_feat.GetGeometryRef()
        if geom is not None:
            geoms_lst.append(geom.Clone())
        in_obj_feat = lyr_over_vec.GetNextFeature()
        geom_pbar.update(1)
    geom_pbar.close()

    n_feats = lyr_in_vec.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats, leave=True)
    open_transaction = False
    counter = 0
    lyr_in_vec.ResetReading()
    in_feature = lyr_in_vec.GetNextFeature()
    while in_feature:
        pbar.update(1)
        if not open_transaction:
            out_lyr_obj.StartTransaction()
            open_transaction = True

        in_geom = in_feature.GetGeometryRef()
        if in_geom is not None:
            for over_geom in geoms_lst:
                if in_geom.Intersect(over_geom):
                    if symmetric:
                        op_out_geom = in_geom.SymmetricDifference(over_geom)
                    else:
                        op_out_geom = in_geom.Difference(over_geom)
                    if (op_out_geom is not None) and (op_out_geom.GetGeometryCount() > 0):
                        for i in range(op_out_geom.GetGeometryCount()):
                            c_geom = op_out_geom.GetGeometryRef(i)
                            if ((c_geom.GetGeometryName().upper() == "POLYGON") or (
                                    c_geom.GetGeometryName().upper() == "MULTIPOLYGON")) and (
                                    (geom_type == ogr.wkbMultiPolygon) or (geom_type == ogr.wkbPolygon)):
                                out_feat = ogr.Feature(feat_defn)
                                out_feat.SetGeometry(c_geom)
                                out_lyr_obj.CreateFeature(out_feat)
                                out_feat = None
                            elif ((c_geom.GetGeometryName().upper() == "LINESTRING") or (
                                    c_geom.GetGeometryName().upper() == "MULTILINESTRING")) and (
                                    (geom_type == ogr.wkbMultiLineString) or (geom_type == ogr.wkbLineString)):
                                out_feat = ogr.Feature(feat_defn)
                                out_feat.SetGeometry(c_geom)
                                out_lyr_obj.CreateFeature(out_feat)
                                out_feat = None
                            elif ((c_geom.GetGeometryName().upper() == "POINT") or (
                                    c_geom.GetGeometryName().upper() == "MULTIPOINT")) and (
                                    (geom_type == ogr.wkbMultiPoint) or (geom_type == ogr.wkbPoint)):
                                out_feat = ogr.Feature(feat_defn)
                                out_feat.SetGeometry(c_geom)
                                out_lyr_obj.CreateFeature(out_feat)
                                out_feat = None

        if ((counter % 20000) == 0) and open_transaction:
            out_lyr_obj.CommitTransaction()
            open_transaction = False

        in_feature = lyr_in_vec.GetNextFeature()
        counter = counter + 1

    if open_transaction:
        out_lyr_obj.CommitTransaction()
        open_transaction = False
    pbar.close()
    out_lyr_obj.SyncToDisk()
    out_ds_obj = None
    ds_in_vec = None
    ds_over_vec = None


def spatial_select(vec_file, vec_lyr, vec_roi_file, vec_roi_lyr, out_vec_file, out_vec_lyr, out_format="GPKG"):
    """
    A function to perform a spatial selection within the input vector using a ROI vector layer.
    This function uses geopandas so ensure that is installed.

    :param vec_file: Input vector file from which features are selected.
    :param vec_lyr: Input vector file layer from which features are selected.
    :param vec_roi_file: The ROI vector file used to select features within the input file.
    :param vec_roi_lyr: The ROI vector layer used to select features within the input file.
    :param out_vec_file: The output vector file with the selected features.
    :param out_vec_lyr: The output layer file with the selected features.
    :param out_format: the output vector format

    """
    import geopandas
    import numpy
    import tqdm
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    roi_gpdf = geopandas.read_file(vec_roi_file, layer=vec_roi_lyr)
    base_gpdf['msk_rsgis_sel'] = numpy.zeros((base_gpdf.shape[0]), dtype=bool)
    geoms = list()
    for i in tqdm.tqdm(range(roi_gpdf.shape[0])):
        inter = base_gpdf['geometry'].intersects(roi_gpdf.iloc[i]['geometry'])
        base_gpdf.loc[inter, 'msk_rsgis_sel'] = True
    base_gpdf = base_gpdf[base_gpdf['msk_rsgis_sel']]
    base_gpdf = base_gpdf.drop(['msk_rsgis_sel'], axis=1)
    if base_gpdf.shape[0] > 0:
        if out_format == 'GPKG':
            base_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            base_gpdf.to_file(out_vec_file, driver=out_format)
    else:
        raise Exception("No output file as no features intersect.")


def split_by_attribute(vec_file, vec_lyr, split_col_name, multi_layers=True, out_vec_file=None, out_file_path=None,
                       out_file_ext=None, out_format="GPKG", dissolve=False, chk_lyr_names=True):
    """
    A function which splits a vector layer by an attribute value into either different layers or different output
    files.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param split_col_name: The column name by which the vector layer will be split.
    :param multi_layers: Boolean (default True). If True then a mulitple layer output file will be created (e.g., GPKG).
                         If False then individual files will be outputted.
    :param out_vec_file: Output vector file - only used if multi_layers = True
    :param out_file_path: Output file path (directory) if multi_layers = False.
    :param out_file_ext: Output file extension is multi_layers = False
    :param out_format: The output format (e.g., GPKG, ESRI Shapefile).
    :param dissolve: Boolean (Default=False) if True then a dissolve on the specified variable will be run
                     as layers are separated.
    :param chk_lyr_names: If True (default) layer names (from split_col_name) will be checked, which means
                          punctuation removed and all characters being ascii characters.

    """
    import geopandas
    import tqdm
    import os
    if multi_layers:
        if out_vec_file is None:
            raise Exception("If a multiple layer output is specified then an output file needs to be specified "
                            "to which the layer need to be added.")
    if not multi_layers:
        if (out_file_path is None) or (out_file_ext is None):
            raise Exception("If a single layer output is specified then an output file path "
                            "and file extention needs to be specified.")

    rsgis_utils = rsgislib.RSGISPyUtils()

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
            # Test resolve if an error thrown then it it probably a topological error which
            # can sometimes be solved using a 0 buffer, so try that to see if it works.
            try:
                c_gpdf.dissolve(by=split_col_name)
            except:
                c_gpdf['geometry'] = c_gpdf.buffer(0)
                c_gpdf = c_gpdf.dissolve(by=split_col_name)
        # Write output to disk.
        if multi_layers and (out_format == 'GPKG'):
            if chk_lyr_names:
                val = rsgis_utils.check_str(val, rm_non_ascii=True, rm_dashs=True, rm_spaces=False, rm_punc=True)
            c_gpdf.to_file(out_vec_file, layer=val, driver='GPKG')
        else:
            if chk_lyr_names:
                val = rsgis_utils.check_str(val, rm_non_ascii=True, rm_dashs=True, rm_spaces=False, rm_punc=True)
            out_vec_file = os.path.join(out_file_path, "vec_{}.{}".format(val, out_file_ext))
            out_vec_lyr = "vec_{}".format(val)
            if out_format == 'GPKG':
                c_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
            else:
                c_gpdf.to_file(out_vec_file, driver=out_format)


def subset_by_attribute(vec_file, vec_lyr, sub_col, sub_vals, out_vec_file, out_vec_lyr, out_format="GPKG",
                        match_type='equals'):
    """
    A function which subsets an input vector layer based on a list of values.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer
    :param sub_col: The column used to subset the layer.
    :param sub_vals: A list of values used to subset the layer. If using contains or start then regular expressions
                     supported by the re library can be provided.
    :param out_vec_file: The output vector file
    :param out_vec_lyr: The output vector layer
    :param out_format: The output vector format.
    :param match_type: The type of match for the subset. Options: equals (default) - the same value.
                       contains - string is anywhere within attribute value.
                       start - string matches the start of the attribute value.

    """
    import geopandas
    import pandas

    match_type = match_type.lower()
    if match_type not in ['equals', 'contains', 'start']:
        raise Exception("The match_type must be either 'equals', 'contains' or 'start'")

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    first = True
    for val in sub_vals:
        print(val)
        if match_type == 'equals':
            tmp_gpdf = base_gpdf[base_gpdf[sub_col] == val]
        elif match_type == 'contains':
            tmp_gpdf = base_gpdf[base_gpdf[sub_col].str.contains(val, na=False)]
        elif match_type == 'start':
            tmp_gpdf = base_gpdf[base_gpdf[sub_col].str.match(val, na=False)]
        else:
            raise Exception("The match_type must be either 'equals', 'contains' or 'start'")

        if first:
            out_gpdf = tmp_gpdf.copy(deep=True)
            first = False
        else:
            out_gpdf = pandas.concat([out_gpdf, tmp_gpdf])

    if out_gpdf.shape[0] > 0:
        if out_format == 'GPKG':
            out_gpdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            out_gpdf.to_file(out_vec_file, driver=out_format)
    else:
        raise Exception("No output file as no features selected.")


def get_unq_col_values(vec_file, vec_lyr, col_name):
    """
    A function which splits a vector layer by an attribute value into either different layers or different output
    files.

    :param vec_file: Input vector file
    :param vec_lyr: Input vector layer
    :param col_name: The column name for which a list of unique values will be returned.

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    unq_vals = base_gpdf[col_name].unique()
    base_gpdf = None
    return unq_vals


def vec_intersects_vec(vec_base_file, vec_base_lyr, vec_comp_file, vec_comp_lyr):
    """
    Function to test whether the comparison vector layer intersects with the
    base vector layer.

    Note. This function iterates through the geometries of both files performing
    a comparison and therefore can be very slow to execute for large vector files.

    :param vec_base_file: vector layer file used as the base layer
    :param vec_base_lyr: vector layer used as the base layer
    :param vec_comp_file: vector layer file used as the comparison layer
    :param vec_comp_lyr: vector layer used as the comparison layer
    :return: boolean

    """
    import osgeo.gdal as gdal
    import tqdm

    gdal.UseExceptions()

    dsVecBaseObj = gdal.OpenEx(vec_base_file, gdal.OF_READONLY)
    if dsVecBaseObj is None:
        raise Exception("Could not open '{}'".format(vec_base_file))

    lyrVecBaseObj = dsVecBaseObj.GetLayerByName(vec_base_lyr)
    if lyrVecBaseObj is None:
        raise Exception("Could not find layer '{}'".format(vec_base_lyr))

    dsVecCompObj = gdal.OpenEx(vec_comp_file, gdal.OF_READONLY)
    if dsVecCompObj is None:
        raise Exception("Could not open '{}'".format(vec_comp_file))

    lyrVecCompObj = dsVecCompObj.GetLayerByName(vec_comp_lyr)
    if lyrVecCompObj is None:
        raise Exception("Could not find layer '{}'".format(vec_comp_lyr))

    n_feats = lyrVecBaseObj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    does_intersect = False
    lyrVecBaseObj.ResetReading()
    base_feat = lyrVecBaseObj.GetNextFeature()
    while base_feat is not None:
        base_geom = base_feat.GetGeometryRef()
        if base_geom is not None:
            lyrVecCompObj.ResetReading()
            comp_feat = lyrVecCompObj.GetNextFeature()
            while comp_feat is not None:
                comp_geom = comp_feat.GetGeometryRef()
                if comp_geom is not None:
                    if base_geom.Intersects(comp_geom):
                        does_intersect = True
                        break
                comp_feat = lyrVecCompObj.GetNextFeature()
        if does_intersect:
            break
        pbar.update(1)
        base_feat = lyrVecBaseObj.GetNextFeature()

    dsVecBaseObj = None
    dsVecCompObj = None

    return does_intersect


def vec_overlaps_vec(vec_base_file, vec_base_lyr, vec_comp_file, vec_comp_lyr):
    """
    Function to test whether the comparison vector layer overlaps with the
    base vector layer.

    Note. This function iterates through the geometries of both files performing
    a comparison and therefore can be very slow to execute for large vector files.

    :param vec_base_file: vector layer file used as the base layer
    :param vec_base_lyr: vector layer used as the base layer
    :param vec_comp_file: vector layer file used as the comparison layer
    :param vec_comp_lyr: vector layer used as the comparison layer
    :return: boolean

    """
    import osgeo.gdal as gdal
    import tqdm

    gdal.UseExceptions()

    dsVecBaseObj = gdal.OpenEx(vec_base_file, gdal.OF_READONLY)
    if dsVecBaseObj is None:
        raise Exception("Could not open '{}'".format(vec_base_file))

    lyrVecBaseObj = dsVecBaseObj.GetLayerByName(vec_base_lyr)
    if lyrVecBaseObj is None:
        raise Exception("Could not find layer '{}'".format(vec_base_lyr))

    dsVecCompObj = gdal.OpenEx(vec_comp_file, gdal.OF_READONLY)
    if dsVecCompObj is None:
        raise Exception("Could not open '{}'".format(vec_comp_file))

    lyrVecCompObj = dsVecCompObj.GetLayerByName(vec_comp_lyr)
    if lyrVecCompObj is None:
        raise Exception("Could not find layer '{}'".format(vec_comp_lyr))

    n_feats = lyrVecBaseObj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    does_overlap = False
    lyrVecBaseObj.ResetReading()
    base_feat = lyrVecBaseObj.GetNextFeature()
    while base_feat is not None:
        base_geom = base_feat.GetGeometryRef()
        if base_geom is not None:
            lyrVecCompObj.ResetReading()
            comp_feat = lyrVecCompObj.GetNextFeature()
            while comp_feat is not None:
                comp_geom = comp_feat.GetGeometryRef()
                if comp_geom is not None:
                    if comp_geom.Overlaps(base_geom):
                        does_overlap = True
                        break
                comp_feat = lyrVecCompObj.GetNextFeature()
        if does_overlap:
            break
        pbar.update(1)
        base_feat = lyrVecBaseObj.GetNextFeature()

    dsVecBaseObj = None
    dsVecCompObj = None

    return does_overlap


def vec_within_vec(vec_base_file, vec_base_lyr, vec_comp_file, vec_comp_lyr):
    """
    Function to test whether the comparison vector layer within with the
    base vector layer.

    Note. This function iterates through the geometries of both files performing
    a comparison and therefore can be very slow to execute for large vector files.

    :param vec_base_file: vector layer file used as the base layer
    :param vec_base_lyr: vector layer used as the base layer
    :param vec_comp_file: vector layer file used as the comparison layer
    :param vec_comp_lyr: vector layer used as the comparison layer
    :return: boolean

    """
    import osgeo.gdal as gdal
    import tqdm

    gdal.UseExceptions()

    dsVecBaseObj = gdal.OpenEx(vec_base_file, gdal.OF_READONLY)
    if dsVecBaseObj is None:
        raise Exception("Could not open '{}'".format(vec_base_file))

    lyrVecBaseObj = dsVecBaseObj.GetLayerByName(vec_base_lyr)
    if lyrVecBaseObj is None:
        raise Exception("Could not find layer '{}'".format(vec_base_lyr))

    dsVecCompObj = gdal.OpenEx(vec_comp_file, gdal.OF_READONLY)
    if dsVecCompObj is None:
        raise Exception("Could not open '{}'".format(vec_comp_file))

    lyrVecCompObj = dsVecCompObj.GetLayerByName(vec_comp_lyr)
    if lyrVecCompObj is None:
        raise Exception("Could not find layer '{}'".format(vec_comp_lyr))

    n_feats = lyrVecCompObj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    is_within = True

    lyrVecCompObj.ResetReading()
    comp_feat = lyrVecCompObj.GetNextFeature()
    while comp_feat is not None:
        comp_geom = comp_feat.GetGeometryRef()
        comp_feat_within = False
        if comp_geom is not None:
            lyrVecBaseObj.ResetReading()
            base_feat = lyrVecBaseObj.GetNextFeature()
            while base_feat is not None:
                base_geom = base_feat.GetGeometryRef()
                if base_geom is not None:
                    if comp_geom.Within(base_geom):
                        comp_feat_within = True
                        break
                base_feat = lyrVecBaseObj.GetNextFeature()

        if not comp_feat_within:
            is_within = False
            break
        pbar.update(1)
        comp_feat = lyrVecCompObj.GetNextFeature()

    dsVecBaseObj = None
    dsVecCompObj = None

    return is_within


def vec_contains_vec(vec_base_file, vec_base_lyr, vec_comp_file, vec_comp_lyr):
    """
    Function to test whether the base vector layer contains with the
    comparison vector layer.

    Note. This function iterates through the geometries of both files performing
    a comparison and therefore can be very slow to execute for large vector files.

    :param vec_base_file: vector layer file used as the base layer
    :param vec_base_lyr: vector layer used as the base layer
    :param vec_comp_file: vector layer file used as the comparison layer
    :param vec_comp_lyr: vector layer used as the comparison layer
    :return: boolean

    """
    import osgeo.gdal as gdal
    import tqdm

    gdal.UseExceptions()

    dsVecBaseObj = gdal.OpenEx(vec_base_file, gdal.OF_READONLY)
    if dsVecBaseObj is None:
        raise Exception("Could not open '{}'".format(vec_base_file))

    lyrVecBaseObj = dsVecBaseObj.GetLayerByName(vec_base_lyr)
    if lyrVecBaseObj is None:
        raise Exception("Could not find layer '{}'".format(vec_base_lyr))

    dsVecCompObj = gdal.OpenEx(vec_comp_file, gdal.OF_READONLY)
    if dsVecCompObj is None:
        raise Exception("Could not open '{}'".format(vec_comp_file))

    lyrVecCompObj = dsVecCompObj.GetLayerByName(vec_comp_lyr)
    if lyrVecCompObj is None:
        raise Exception("Could not find layer '{}'".format(vec_comp_lyr))

    n_feats = lyrVecCompObj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    does_contain = True

    lyrVecCompObj.ResetReading()
    comp_feat = lyrVecCompObj.GetNextFeature()
    while comp_feat is not None:
        comp_geom = comp_feat.GetGeometryRef()
        comp_feat_contained = False
        if comp_geom is not None:
            lyrVecBaseObj.ResetReading()
            base_feat = lyrVecBaseObj.GetNextFeature()
            while base_feat is not None:
                base_geom = base_feat.GetGeometryRef()
                if base_geom is not None:
                    if base_geom.Contains(comp_geom):
                        comp_feat_contained = True
                        break
                base_feat = lyrVecBaseObj.GetNextFeature()

        if not comp_feat_contained:
            does_contain = False
            break
        pbar.update(1)
        comp_feat = lyrVecCompObj.GetNextFeature()

    dsVecBaseObj = None
    dsVecCompObj = None

    return does_contain


def vec_touches_vec(vec_base_file, vec_base_lyr, vec_comp_file, vec_comp_lyr):
    """
    Function to test whether the comparison vector layer touches the
    base vector layer.

    Note. This function iterates through the geometries of both files performing
    a comparison and therefore can be very slow to execute for large vector files.

    :param vec_base_file: vector layer file used as the base layer
    :param vec_base_lyr: vector layer used as the base layer
    :param vec_comp_file: vector layer file used as the comparison layer
    :param vec_comp_lyr: vector layer used as the comparison layer
    :return: boolean

    """
    import osgeo.gdal as gdal
    import tqdm

    gdal.UseExceptions()

    dsVecBaseObj = gdal.OpenEx(vec_base_file, gdal.OF_READONLY)
    if dsVecBaseObj is None:
        raise Exception("Could not open '{}'".format(vec_base_file))

    lyrVecBaseObj = dsVecBaseObj.GetLayerByName(vec_base_lyr)
    if lyrVecBaseObj is None:
        raise Exception("Could not find layer '{}'".format(vec_base_lyr))

    dsVecCompObj = gdal.OpenEx(vec_comp_file, gdal.OF_READONLY)
    if dsVecCompObj is None:
        raise Exception("Could not open '{}'".format(vec_comp_file))

    lyrVecCompObj = dsVecCompObj.GetLayerByName(vec_comp_lyr)
    if lyrVecCompObj is None:
        raise Exception("Could not find layer '{}'".format(vec_comp_lyr))

    n_feats = lyrVecBaseObj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    does_touch = False

    lyrVecBaseObj.ResetReading()
    base_feat = lyrVecBaseObj.GetNextFeature()
    while base_feat is not None:
        base_geom = base_feat.GetGeometryRef()
        if base_geom is not None:
            lyrVecCompObj.ResetReading()
            comp_feat = lyrVecCompObj.GetNextFeature()
            while comp_feat is not None:
                comp_geom = comp_feat.GetGeometryRef()
                if comp_geom is not None:
                    if comp_geom.Touches(base_geom):
                        does_touch = True
                        break
                comp_feat = lyrVecCompObj.GetNextFeature()
        if does_touch:
            break
        pbar.update(1)
        base_feat = lyrVecBaseObj.GetNextFeature()

    dsVecBaseObj = None
    dsVecCompObj = None

    return does_touch


def vec_crosses_vec(vec_base_file, vec_base_lyr, vec_comp_file, vec_comp_lyr):
    """
    Function to test whether the comparison vector layer touches the
    base vector layer.

    Note. This function iterates through the geometries of both files performing
    a comparison and therefore can be very slow to execute for large vector files.

    :param vec_base_file: vector layer file used as the base layer
    :param vec_base_lyr: vector layer used as the base layer
    :param vec_comp_file: vector layer file used as the comparison layer
    :param vec_comp_lyr: vector layer used as the comparison layer
    :return: boolean

    """
    import osgeo.gdal as gdal
    import tqdm

    gdal.UseExceptions()

    dsVecBaseObj = gdal.OpenEx(vec_base_file, gdal.OF_READONLY)
    if dsVecBaseObj is None:
        raise Exception("Could not open '{}'".format(vec_base_file))

    lyrVecBaseObj = dsVecBaseObj.GetLayerByName(vec_base_lyr)
    if lyrVecBaseObj is None:
        raise Exception("Could not find layer '{}'".format(vec_base_lyr))

    dsVecCompObj = gdal.OpenEx(vec_comp_file, gdal.OF_READONLY)
    if dsVecCompObj is None:
        raise Exception("Could not open '{}'".format(vec_comp_file))

    lyrVecCompObj = dsVecCompObj.GetLayerByName(vec_comp_lyr)
    if lyrVecCompObj is None:
        raise Exception("Could not find layer '{}'".format(vec_comp_lyr))

    n_feats = lyrVecBaseObj.GetFeatureCount(True)
    pbar = tqdm.tqdm(total=n_feats)
    does_cross = False

    lyrVecBaseObj.ResetReading()
    base_feat = lyrVecBaseObj.GetNextFeature()
    while base_feat is not None:
        base_geom = base_feat.GetGeometryRef()
        if base_geom is not None:
            lyrVecCompObj.ResetReading()
            comp_feat = lyrVecCompObj.GetNextFeature()
            while comp_feat is not None:
                comp_geom = comp_feat.GetGeometryRef()
                if comp_geom is not None:
                    if comp_geom.Crosses(base_geom):
                        does_cross = True
                        break
                comp_feat = lyrVecCompObj.GetNextFeature()
        if does_cross:
            break
        pbar.update(1)
        base_feat = lyrVecBaseObj.GetNextFeature()

    dsVecBaseObj = None
    dsVecCompObj = None

    return does_cross


def merge_vector_files(input_files, output_file, output_lyr=None, out_format='GPKG', out_epsg=None):
    """
    A function which merges the input files into a single output file using geopandas. If the input files
    have multiple layers they are all merged into the output file.

    :param input_files: list of input files
    :param output_file: output vector file.
    :param output_lyr: output vector layer.
    :param out_format: output file format.
    :param out_epsg: if input layers are different projections then option can be used to define the output
                     projection.

    """
    import tqdm
    import geopandas
    first = True
    for vec_file in tqdm.tqdm(input_files):
        lyrs = rsgislib.vectorutils.getVecLyrsLst(vec_file)
        for lyr in lyrs:
            if first:
                data_gdf = geopandas.read_file(vec_file, layer=lyr)
                if out_epsg is not None:
                    data_gdf = data_gdf.to_crs(epsg=out_epsg)
                first = False
            else:
                tmp_data_gdf = geopandas.read_file(vec_file, layer=lyr)
                if out_epsg is not None:
                    tmp_data_gdf = tmp_data_gdf.to_crs(epsg=out_epsg)

                data_gdf = data_gdf.append(tmp_data_gdf)

    if not first:
        if out_format == "GPKG":
            if output_lyr is None:
                raise Exception("If output format is GPKG then an output layer is required.")
            data_gdf.to_file(output_file, layer=output_lyr, driver=out_format)
        else:
            data_gdf.to_file(output_file, driver=out_format)


def merge_vector_layers(inputs, output_file, output_lyr=None, out_format='GPKG', out_epsg=None):
    """
    A function which merges the input vector layers into a single output file using geopandas.

    :param inputs: list of dicts with keys [{'file': '/file/path/to/file.gpkg', 'layer': 'layer_name'}]
                        providing the file paths and layer names.
    :param output_file: output vector file.
    :param output_lyr: output vector layer.
    :param out_format: output file format.
    :param out_epsg: if input layers are different projections then option can be used to define the output
                     projection.

    """
    import tqdm
    import geopandas
    first = True
    for vec_info in tqdm.tqdm(inputs):
        if ('file' in vec_info) and ('layer' in vec_info):
            if first:
                data_gdf = geopandas.read_file(vec_info['file'], layer=vec_info['layer'])
                if out_epsg is not None:
                    data_gdf = data_gdf.to_crs(epsg=out_epsg)
                first = False
            else:
                tmp_data_gdf = geopandas.read_file(vec_info['file'], layer=vec_info['layer'])
                if out_epsg is not None:
                    tmp_data_gdf = tmp_data_gdf.to_crs(epsg=out_epsg)

                data_gdf = data_gdf.append(tmp_data_gdf)
        else:
            raise Exception("The inputs should be a list of dicts with keys 'file' and 'layer'.")

    if not first:
        if out_format == "GPKG":
            if output_lyr is None:
                raise Exception("If output format is GPKG then an output layer is required.")
            data_gdf.to_file(output_file, layer=output_lyr, driver=out_format)
        else:
            data_gdf.to_file(output_file, driver=out_format)


def explode_vec_lyr(vec_file, vec_lyr, vec_out_file, vec_out_lyr, out_format='GPKG'):
    """
    A function to explode a vector layer separating any multiple geometries (e.g., multipolygons)
    to single geometries.

    Note. this function uses geopandas and therefore the vector layer is loaded into memory.

    :param vec_file: vector layer file
    :param vec_lyr: vector layer name
    :param vec_out_file: output vector layer file
    :param vec_out_lyr: output vector layer name (Can be None if output format is not GPKG).
    :param out_format: The output format for the vector file.

    """
    import geopandas
    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    data_explode_gdf = data_gdf.explode()

    if len(data_explode_gdf) > 0:
        if out_format == "GPKG":
            data_explode_gdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
        else:
            data_explode_gdf.to_file(vec_out_file, driver=out_format)


def explode_vec_files(input_vecs, output_dir, out_format='GPKG', out_file_ext='gpkg'):
    """
    A function which explodes the multiple geometries within a list of input layers.
    The output directory must be different to the directory the input files as the
    output file name will be the same as the input name.

    Note. this function uses the explode_vec_lyr function which uses geopandas and therefore
    the vector layer is loaded into memory.

    :param input_vecs: A list of input files.
    :param output_dir: The directory where the output files will be placed.
    :param out_format: The vector format for the outputs.
    :param out_file_ext: the file extension for the output files. There should not be a dot within the extension.
                         e.g., gpkg, shp, geojson.

    """
    import tqdm
    rsgis_utils = rsgislib.RSGISPyUtils()
    for vec_file in tqdm.tqdm(input_vecs):
        lyrs = getVecLyrsLst(vec_file)
        basename = rsgis_utils.get_file_basename(vec_file)
        out_vec_file = os.path.join(output_dir, "{}.{}".format(basename, out_file_ext))
        for lyr in lyrs:
            explode_vec_lyr(vec_file, lyr, out_vec_file, lyr, out_format)


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


def geopd_check_polys_wgs84bounds_geometry(data_gdf, width_thres=350):
    """
    A function which checks a polygons within the geometry of a geopanadas dataframe
    for specific case where they on the east/west edge (i.e., 180 / -180) and are therefore
    being wrapped around the world. For example, this function would change a longitude
    -179.91 to 180.01. The geopandas dataframe will be edit in place.

    This function will import the shapely library.

    :param data_gdf: geopandas dataframe.
    :param width_thres: The threshold (default 350 degrees) for the width of a polygon for which
                        the polygons will be checked, looping through all the coordinates
    :return: geopandas dataframe

    """
    from shapely.geometry import Polygon, LinearRing
    import geopandas

    out_gdf = geopandas.GeoDataFrame()
    out_gdf['geometry'] = None
    i_geom = 0

    for index, row in data_gdf.iterrows():
        n_east = 0
        n_west = 0
        row_bbox = row['geometry'].bounds
        row_width = row_bbox[2] - row_bbox[0]
        if row_width > width_thres:
            if row['geometry'].geom_type == 'Polygon':
                for coord in row['geometry'].exterior.coords:
                    if coord[0] < 0:
                        n_west += 1
                    else:
                        n_east += 1
                east_focus = True
                if n_west > n_east:
                    east_focus = False

                out_coords = []
                for coord in row['geometry'].exterior.coords:
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
                for hole in row['geometry'].interiors:
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
                out_gdf.loc[i_geom, 'geometry'] = Polygon(out_coords, holes=out_holes)
                i_geom += 1
            elif row['geometry'].geom_type == 'MultiPolygon':
                for poly in row['geometry']:
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
                    out_gdf.loc[i_geom, 'geometry'] = Polygon(out_coords, holes=out_holes)
                    i_geom += 1
        else:
            out_gdf.loc[i_geom, 'geometry'] = row['geometry']
            i_geom += 1

    return out_gdf


def merge_utm_vecs_wgs84(input_files, output_file, output_lyr=None, out_format='GPKG',
                         n_hemi_utm_file=None, s_hemi_utm_file=None, width_thres=350):
    """
    A function which merges input files in UTM projections to the WGS84 projection cutting
    polygons which wrap from one side of the world to other (i.e., 180/-180 boundary).

    :param input_files: list of input files
    :param output_file: output vector file.
    :param output_lyr: output vector layer - only used if output format is GPKG
    :param out_format: output file format.
    :param n_utm_zones_vec: GPKG file with layer per zone (layer names: 01, 02, ... 59, 60) each projected in
                            the northern hemisphere UTM projections.
    :param s_utm_zone_vec: GPKG file with layer per zone (layer names: 01, 02, ... 59, 60) each projected in
                            the southern hemisphere UTM projections.
    :param width_thres: The threshold (default 350 degrees) for the width of a polygon for which
                        the polygons will be checked, looping through all the coordinates

    """
    import geopandas
    import pandas
    import rsgislib.tools.utm
    import tqdm
    
    if n_hemi_utm_file is None:
        install_prefix = __file__[:__file__.find('lib')]
        n_hemi_utm_file = os.path.join(install_prefix, "share", "rsgislib", "utm_zone_boundaries_lyrs_north.gpkg")
        if n_hemi_utm_file is None:
            raise Exception("An input is needed for n_hemi_utm_file. The RSGISLib installed version was not be found.")
    if s_hemi_utm_file is None:
        install_prefix = __file__[:__file__.find('lib')]
        s_hemi_utm_file = os.path.join(install_prefix, "share", "rsgislib", "utm_zone_boundaries_lyrs_south.gpkg")
        if s_hemi_utm_file is None:
            raise Exception("An input is needed for s_hemi_utm_file. The RSGISLib installed version was not be found.")
    
    rsgis_utils = rsgislib.RSGISPyUtils()
    first = True
    for file in tqdm.tqdm(input_files):
        lyrs = getVecLyrsLst(file)
        for lyr in lyrs:
            bbox = rsgis_utils.getVecLayerExtent(file, layerName=lyr)
            bbox_area = rsgis_utils.calc_bbox_area(bbox)
            if bbox_area > 0:
                vec_epsg = rsgis_utils.getProjEPSGFromVec(file, vecLyr=lyr)
                zone, hemi = rsgislib.tools.utm.utm_from_epsg(int(vec_epsg))
                zone_str = rsgis_utils.zero_pad_num_str(zone, str_len=2, round_num=False, round_n_digts=0, integerise=True)

                if hemi.upper() == 'S':
                    utm_zones_file = s_hemi_utm_file
                else:
                    utm_zones_file = n_hemi_utm_file

                contained = vec_within_vec(utm_zones_file, zone_str, file, lyr)
                if not contained:
                    data_gdf = geopandas.read_file(file, layer=lyr)
                    utm_gdf = geopandas.read_file(utm_zones_file, layer=zone_str)

                    data_inter_gdf = geopandas.overlay(data_gdf, utm_gdf, how='intersection')
                    data_diff_gdf = geopandas.overlay(data_gdf, utm_gdf, how='difference')
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
                    widths = data_gdf_bounds['maxx'] - data_gdf_bounds['minx']
                    if widths.max() > width_thres:
                        data_gdf = geopd_check_polys_wgs84bounds_geometry(data_gdf, width_thres)
                    if first:
                        out_gdf = data_gdf
                        first = False
                    else:
                        out_gdf = out_gdf.append(data_gdf)

    if not first:
        if out_format == "GPKG":
            if output_lyr is None:
                raise Exception("If output format is GPKG then an output layer is required.")
            out_gdf.to_file(output_file, layer=output_lyr, driver=out_format)
        else:
            out_gdf.to_file(output_file, driver=out_format)


def clip_vec_lyr(vec_file, vec_lyr, roi_file, roi_lyr, vec_out_file, vec_out_lyr, out_format='GPKG'):
    """
    A function which clips a vector layer using an input region of interest (ROI) polygon layer.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param roi_file: Input vector file defining the ROI polygon(s)
    :param roi_lyr: Input vector layer within the roi input file.
    :param vec_out_file: Output vector file
    :param vec_out_lyr: Output vector layer name.
    :param out_format: Output file format (default GPKG).

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)
    roi_gpdf = geopandas.read_file(roi_file, layer=roi_lyr)

    cliped_gpdf = geopandas.clip(base_gpdf, roi_gpdf, keep_geom_type=True)

    if out_format == 'GPKG':
        cliped_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        cliped_gpdf.to_file(vec_out_file, driver=out_format)


def shiftxy_vec_lyr(vec_file, vec_lyr, x_shift, y_shift, vec_out_file, vec_out_lyr, out_format='GPKG'):
    """
    A function which shifts (translates) a vector layer in the x and y axis'.

    :param vec_file: Input vector file.
    :param vec_lyr: Input vector layer within the input file.
    :param x_shift: The shift in the x axis. In the units of the coordinate system the file is projected in.
    :param y_shift: The shift in the y axis. In the units of the coordinate system the file is projected in.
    :param vec_out_file: Output vector file
    :param vec_out_lyr: Output vector layer name.
    :param out_format: Output file format (default GPKG).

    """
    import geopandas

    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    shifted_gseries = base_gpdf.translate(xoff=x_shift, yoff=y_shift)
    shifted_gpdf = geopandas.GeoDataFrame(geometry=shifted_gseries)

    col_names = base_gpdf.columns
    for col_name in col_names:
        if col_name != 'geometry':
            shifted_gpdf[col_name] = base_gpdf[col_name]

    if out_format == 'GPKG':
        shifted_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        shifted_gpdf.to_file(vec_out_file, driver=out_format)


def split_feats_to_mlyrs(in_vec_file, in_vec_lyr, out_vec_file, out_format='GPKG'):
    """
    A function which splits an existing vector layer into multiple layers

    :param in_vec_file: input vector file
    :param in_vec_lyr: input vector layer
    :param out_vec_file: output file, note the format must be one which
                         supports multiple layers (e.g., GPKG).
    :param out_format: The output format of the output file.

    """
    import geopandas
    import tqdm
    base_gpdf = geopandas.read_file(in_vec_file, layer=in_vec_lyr)

    for i in tqdm.tqdm(range(base_gpdf.shape[0])):
        tmp_gp_series = base_gpdf.loc[i]
        tmp_gpdf = geopandas.GeoDataFrame([tmp_gp_series])
        vec_out_lyr = "veclyr_{}".format(i)
        tmp_gpdf.to_file(out_vec_file, layer=vec_out_lyr, driver=out_format)
        tmp_gpdf = None
    base_gpdf = None


def addGeomBBOXCols(vec_file, vec_lyr, vec_out_file, vec_out_lyr, out_format='GPKG',
                    min_x_col='MinX', max_x_col='MaxX', min_y_col='MinY', max_y_col='MaxY'):
    """
    A function which adds columns to the vector layer with the bbox of each geometry.

    :param vec_file: input vector file
    :param vec_lyr: input vector layer name
    :param vec_out_file: output vector file
    :param vec_out_lyr: output vector layer name
    :param out_format: The output format of the output file. (Default: GPKG)
    :param min_x_col: Name of the MinX column (Default: MinX)
    :param max_x_col: Name of the MaxX column (Default: MaxX)
    :param min_y_col: Name of the MinY column (Default: MinY)
    :param max_y_col: Name of the MaxY column (Default: MaxY)

    """
    import geopandas
    # Read input vector file.
    base_gpdf = geopandas.read_file(vec_file, layer=vec_lyr)

    # Get Geometry bounds
    geom_bounds = base_gpdf['geometry'].bounds

    # Add columns to the geodataframe
    base_gpdf[min_x_col] = geom_bounds['minx']
    base_gpdf[max_x_col] = geom_bounds['maxx']
    base_gpdf[min_y_col] = geom_bounds['miny']
    base_gpdf[max_y_col] = geom_bounds['maxy']

    # Output the file.
    if out_format == 'GPKG':
        base_gpdf.to_file(vec_out_file, layer=vec_out_lyr, driver=out_format)
    else:
        base_gpdf.to_file(vec_out_file, driver=out_format)


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
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()

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
            x_col_val_str = rsgis_utils.zero_pad_num_str(x_col_val, str_len=zero_x_pad, round_num=False,
                                                         round_n_digts=round_n_digts, integerise=int_coords)
        else:
            x_col_val = int(x_col_val)
            x_col_val_str = '{}'.format(x_col_val)

        if zero_y_pad > 0:
            y_col_val_str = rsgis_utils.zero_pad_num_str(y_col_val, str_len=zero_y_pad, round_num=False,
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


def vec_lyr_intersection_gp(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None, out_format="GPKG", force=False):
    """
    A function which performs an intersection between the vector layer and the overlain vector using Geopandas.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.
    """
    import os
    import geopandas

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    over_data_gdf = geopandas.read_file(vec_over_file, layer=vec_over_lyr)
    # Perform Intersection
    data_inter_gdf = geopandas.overlay(data_gdf, over_data_gdf, how='intersection')

    if out_format == "GPKG":
        if out_vec_lyr is None:
            raise Exception("If output format is GPKG then an output layer is required.")
        data_inter_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_inter_gdf.to_file(out_vec_file, driver=out_format)


def vec_lyr_difference_gp(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None, out_format="GPKG", force=False):
    """
    A function which performs a difference between the vector layer and the overlain vector using Geopandas.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.
    """
    import os
    import geopandas

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    over_data_gdf = geopandas.read_file(vec_over_file, layer=vec_over_lyr)
    # Perform Difference
    data_inter_gdf = geopandas.overlay(data_gdf, over_data_gdf, how='difference')

    if out_format == "GPKG":
        if out_vec_lyr is None:
            raise Exception("If output format is GPKG then an output layer is required.")
        data_inter_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_inter_gdf.to_file(out_vec_file, driver=out_format)


def vec_lyr_sym_difference_gp(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None, out_format="GPKG", force=False):
    """
    A function which performs a symmetric difference between the vector layer and the overlain vector using Geopandas.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.
    """
    import os
    import geopandas

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    over_data_gdf = geopandas.read_file(vec_over_file, layer=vec_over_lyr)
    # Perform symmetric difference
    data_inter_gdf = geopandas.overlay(data_gdf, over_data_gdf, how='symmetric_difference')

    if out_format == "GPKG":
        if out_vec_lyr is None:
            raise Exception("If output format is GPKG then an output layer is required.")
        data_inter_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_inter_gdf.to_file(out_vec_file, driver=out_format)


def vec_lyr_identity_gp(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None, out_format="GPKG", force=False):
    """
    A function which performs a identity between the vector layer and the overlain vector using Geopandas.

    The result consists of the surface of vec_file, but with the geometries obtained from overlaying vec_file with vec_over_file.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.
    """
    import os
    import geopandas

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    over_data_gdf = geopandas.read_file(vec_over_file, layer=vec_over_lyr)
    # Perform identity
    data_inter_gdf = geopandas.overlay(data_gdf, over_data_gdf, how='identity')

    if out_format == "GPKG":
        if out_vec_lyr is None:
            raise Exception("If output format is GPKG then an output layer is required.")
        data_inter_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_inter_gdf.to_file(out_vec_file, driver=out_format)


def vec_lyr_union_gp(vec_file, vec_lyr, vec_over_file, vec_over_lyr, out_vec_file, out_vec_lyr=None, out_format="GPKG", force=False):
    """
    A function which performs a union between the vector layer and the overlain vector using Geopandas.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param vec_over_file: The vector file overlained on the input vector file.
    :param vec_over_lyr: The vector layer overlained on the input vector file.
    :param out_vec_file: The output vector file path.
    :param out_vec_lyr: The output vector layer name.
    :param out_format: The output file format of the vector file.
    :param force: remove output file if it exists.
    """
    import os
    import geopandas

    if os.path.exists(out_vec_file):
        if force:
            delete_vector_file(out_vec_file)
        else:
            raise Exception("The output vector file ({}) already exists, remove it and re-run.".format(out_vec_file))

    if out_vec_lyr is None:
        out_vec_lyr = os.path.splitext(os.path.basename(out_vec_file))[0]

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    over_data_gdf = geopandas.read_file(vec_over_file, layer=vec_over_lyr)
    # Perform union
    data_inter_gdf = geopandas.overlay(data_gdf, over_data_gdf, how='union')

    if out_format == "GPKG":
        if out_vec_lyr is None:
            raise Exception("If output format is GPKG then an output layer is required.")
        data_inter_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_inter_gdf.to_file(out_vec_file, driver=out_format)

