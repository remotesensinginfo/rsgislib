"""
The vector utils module performs geometry / attribute table operations on vectors.
"""

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


def rasterise2Image(inputVec, inputImage, outImage, gdalformat="KEA", burnVal=1, shpAtt=None, shpExt=False):
    """ 
*** Deprecated *** A utillity to rasterise a shapefile into an image covering the same region and at the same resolution as the input image. 

Where:

* inputVec is a string specifying the input vector (shapefile) file
* inputImage is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation (if None and shpExt is False them assumes output image already exists and just uses it as is burning vector into it)
* outImage is a string specifying the output image for the rasterised shapefile
* gdalformat is the output image format (Default: KEA).
* burnVal is the value for the output image pixels if no attribute is provided.
* shpAtt is a string specifying the attribute to be rasterised, value of None creates a binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.
* shpExt is a boolean specifying that the output image should be cut to the same extent as the input shapefile (Default is False and therefore output image will be the same as the input).

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

* inputVec is a string specifying the input vector file
* inputVecLyr is a string specifying the input vector layer name.
* inputImage is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation (if None and vecExt is False them assumes output image already exists and just uses it as is burning vector into it)
* outImage is a string specifying the output image for the rasterised shapefile
* gdalformat is the output image format (Default: KEA).
* burnVal is the value for the output image pixels if no attribute is provided.
* datatype of the output file, default is rsgislib.TYPE_8UINT
* vecAtt is a string specifying the attribute to be rasterised, value of None creates a binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.
* vecExt is a boolean specifying that the output image should be cut to the same extent as the input shapefile (Default is False and therefore output image will be the same as the input).
* thematic is a boolean (default True) specifying that the output image is an thematic dataset so a colour table will be populated.
* nodata is a float specifying the no data value associated with a continous output image.

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
A utillity to rasterise a vector layer to an image covering the same region. 

Where:

* vec_lyr_obj is a OGR Vector Layer Object
* outImage is a string specifying the output image, this image must already exist and intersect within the input vector layer.
* burnVal is the value for the output image pixels if no attribute is provided.
* vecAtt is a string specifying the attribute to be rasterised, value of None creates a binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.
* calcstats is a boolean specifying whether image stats and pyramids should be calculated.
* thematic is a boolean (default True) specifying that the output image is an thematic dataset so a colour table will be populated.
* nodata is a float specifying the no data value associated with a continous output image.

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

* inputVec is a string specifying the input vector (shapefile) file
* inputImage is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation
* outputImage is a string specifying the output KEA image for the rasterised shapefile

Example::

    from rsgislib import vectorutils
     
    inputVector = 'crowns.shp'
    inputImage = 'injune_p142_casi_sub_utm.kea'
    outputImage = 'psu142_crowns.kea'
        
    vectorutils.copyShapefile2RAT(inputVector, 'crowns', inputImage, outputImage)

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

* inputImg is a string specifying the input image file to be polygonised
* outShp is a string specifying the output shapefile path. If it exists it will be deleted and overwritten.
* imgBandNo is an int specifying the image band to be polygonised. (default = 1)
* maskImg is an optional string mask file specifying a no data mask (default = None)
* imgMaskBandNo is an int specifying the image band to be used the mask (default = 1)

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
    
    print("Polygonising...")
    gdal.Polygonize(imgBand, imgMaskBand, outLayer, dstFieldIdx, [], callback=gdal.TermProgress )
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

* outvec is a string specifying the output vector file path. If it exists it will be deleted and overwritten.
* outlyr is a string with the name of the vector layer.
* vecdrv is a string with the driver
* inputImg is a string specifying the input image file to be polygonised
* imgBandNo is an int specifying the image band to be polygonised. (default = 1)
* maskImg is an optional string mask file specifying a no data mask (default = None)
* imgMaskBandNo is an int specifying the image band to be used the mask (default = 1)
* replace_file is a boolean specifying whether the vector file should be replaced (i.e., overwritten). Default=True.
* replace_lyr is a boolean specifying whether the vector layer should be replaced (i.e., overwritten). Default=True.
* pxl_val_fieldname is a string to specify the name of the output column representing the pixel value within the
                    input image.

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

    print("Polygonising...")
    gdal.Polygonize(imgBand, imgMaskBand, outLayer, dstFieldIdx, [], callback=gdal.TermProgress)
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

* vectorFile - The file / path to the vector data 'file'.
* vectorLayer - The layer to which the data is to be added.
* colName - Name of the output column
* colDataType - ogr data type (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
* colData - A list of the same length as the number of features in vector file.

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

* lyr - GDAL/OGR vector layer object
* colName - Name of the output column
* colDataType - ogr data type (e.g., ogr.OFTString, ogr.OFTInteger, ogr.OFTReal)
* colData - A list of the same length as the number of features in vector file.

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

* vectorFile - The file / path to the vector data 'file'.
* vectorLayer - The layer to which the data is to be read from.
* colName - Name of the input column

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

* vectorFile - The file / path to the vector data 'file'.
* vectorLayer - The layer to which the data is to be read from.
* attNames - List of input attribute column names to be read in.

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

* vecFile - vector file.
* vecLyr - layer within the vector file.
* xminCol - column name.
* xmaxCol - column name.
* yminCol - column name.
* ymaxCol - column name.

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

* inputImg - the input image file for which the footprint will be extracted.
* outVec - output shapefile path and name.
* tmpDIR - temp directory which will be used during processing. It will be created and deleted once processing complete.
* rePrjTo - optional command 

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

* inVec - is a string with the input vector file name and path.
* layerName - is the layer for which extent is to be calculated (Default: None). if None assume there is only one layer and that will be read.
* computeCount - is a boolean which specifies whether the layer extent 
                 should be calculated (rather than estimated from header)
                 even if that operation is computationally expensive.

return: nfeats

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

* inFileList - is a list of input files.
* outVecFile - is the output shapefile

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

* inFileList - is a list of input files.
* outDBFile - is the output SQLite database (\*.sqlite)
* lyrName - is the layer name in the output database (i.e., you can merge layers into single layer or write a number of layers to the same database).
* exists - boolean which specifies whether the database file exists or not.

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

* inFileList - is a list of input files.
* outFile - is the output GPKG database (\*.gpkg)
* lyrName - is the layer name in the output database (i.e., you can merge layers into single layer or write a number of layers to the same database).
* exists - boolean which specifies whether the database file exists or not.

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
Function which will merge a list of vector files into an single output GeoPackage (GPKG) file using ogr2ogr.

Where:

* inputFile - is a vector file which contains multiple layers which are to be merged
* outFile - is the output GPKG database (\*.gpkg)
* lyrName - is the layer name in the output database (i.e., you can merge layers into single layer or write a number of layers to the same database).
* exists - boolean which specifies whether the database file exists or not.

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

* inFileList - is a list of input files.
* outDBFile - is the output SQLite database (\*.sqlite)

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


def mergeVectors2GPKGIndLyrs(inFileList, outFile):
    """
Function which will merge a list of vector files into an single output GPKG file where each input 
file forms a new layer using the existing layer name. This function wraps the ogr2ogr command.

Where:

* inFileList - is a list of input files.
* outFile - is the output GPKG database (\*.gpkg)

"""
    for inFile in inFileList:
        inlyrs = getVecLyrsLst(inFile)
        print("Processing File: {0} has {1} layers to copy.".format(inFile, len(inlyrs)))
        for lyr in inlyrs:
            nFeat = getVecFeatCount(inFile, lyr)
            print("Processing Layer: {0} has {1} features to copy.".format(lyr, nFeat))
            if nFeat > 0:
                cmd = 'ogr2ogr -overwrite -f "GPKG" -lco SPATIAL_INDEX=YES -nln {0} "{1}" "{2}" {0}'.format(lyr, outFile, inFile)
                try:
                    subprocess.check_call(cmd, shell=True)
                except OSError as e:
                    raise Exception('Error running ogr2ogr: ' + cmd)


def createPolySHP4LstBBOXs(csvFile, outSHP, epsgCode, minXCol=0, maxXCol=1, minYCol=2, maxYCol=3, ignoreRows=0, force=False):
    """
This function takes a CSV file of bounding boxes (1 per line) and creates a polygon shapefile.
    
* csvFile - input CSV file.
* outSHP - output ESRI shapefile
* epsgCode - EPSG code specifying the projection of the data (4326 is WSG84 Lat/Long).
* minXCol - The index (starting at 0) for the column within the CSV file for the minimum X coordinate.
* maxXCol - The index (starting at 0) for the column within the CSV file for the maximum X coordinate.
* minYCol - The index (starting at 0) for the column within the CSV file for the minimum Y coordinate.
* maxYCol - The index (starting at 0) for the column within the CSV file for the maximum Y coordinate.
* ignoreRows - The number of rows to ignore from the start of the CSV file (i.e., column headings)
* force - If the output file already exists delete it before proceeding.

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

* vecFile - file name and path to input vector layer.

returns: list of layer names (can be used with gdal.Dataset.GetLayerByName()).

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

* inVec - is a string with the input vector file name and path.
* layerName - is the layer for which extent is to be calculated (Default: None)
*             if None assume there is only one layer and that will be read.
* computeIfExp - is a boolean which specifies whether the layer extent 
                 should be calculated (rather than estimated from header)
                 even if that operation is computationally expensive.

return: boundary box is returned (MinX, MaxX, MinY, MaxY)

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
* inVec - is a string with the input vector file name and path.

return: WKT representation of projection

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

* vecFile - input vector file.
* vecLyr - input layer name.
* nfeats - number of features within each output file.
* outVecDrvr - output file driver.
* outdir - output directory for the created output files.
* outvecbase - output layer name will be the same as the base file name.
* outvecend - file ending (e.g., .shp).

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


def reProjVectorLayer(inputVec, outputVec, outProjWKT, outDriverName='ESRI Shapefile', outLyrName=None, inLyrName=None, inProjWKT=None, force=False):
    """
A function which reprojects a vector layer. 
    
* inputVec is a string with name and path to input vector file.
* outputVec is a string with name and path to output vector file.
* outProjWKT is a string with the WKT string for the output vector file.
* outDriverName is the output vector file format. Default is ESRI Shapefile.
* outLyrName is a string for the output layer name. If None then ignored and 
             assume there is just a single layer in the vector and layer name
             is the same as the file name.
* inLyrName is a string for the input layer name. If None then ignored and 
            assume there is just a single layer in the vector.
* inProjWKT is a string with the WKT string for the input shapefile 
            (Optional; taken from input file if not specified).

"""
    ## This code has been editted from https://pcjericks.github.io/py-gdalogr-cookbook/projection.html#reproject-a-layer
    ## Updated for GDAL 2.0
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


def getAttLstSelectFeats(vecFile, vecLyr, attNames, selVecFile, selVecLyr):
    """
Function to get a list of attribute values from features which intersect
with the select layer.
* vecFile - vector layer from which the attribute data comes from.
* vecLyr - the layer name from which the attribute data comes from.
* attNames - a list of attribute names to be outputted.
* selVecFile - the vector file which will be intersected within the vector file.
* selVecLyr - the layer name which will be intersected within the vector file.

returns list of dictionaries with the output values.

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
* vecLyrObj - the OGR layer object from which the attribute data comes from.
* attNames - a list of attribute names to be outputted.
* selVecLyrObj - the OGR layer object which will be intersected within the vector file.

returns list of dictionaries with the output values.

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
* vec_file - the OGR file from which the attribute data comes from.
* vec_lyr - the layer name within the file from which the attribute data comes from.
* attNames - a list of attribute names to be outputted.
* bbox - the bounding box for the region of interest (xMin, xMax, yMin, yMax).
* bbox_epsg - the projection of the BBOX (if None then ignore).

returns list of dictionaries with the output values.

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
* vecLyrObj - the OGR layer object from which the attribute data comes from.
* attNames - a list of attribute names to be outputted.
* bbox - the bounding box for the region of interest (xMin, xMax, yMin, yMax).
* bbox_epsg - the projection of the BBOX (if None then ignore).

returns list of dictionaries with the output values.

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

* vecFile - vector layer from which the attribute data comes from.
* vecLyr - the layer name from which the attribute data comes from.
* roiVecFile - the vector file which will be intersected within the vector file.
* roiVecLyr - the layer name which will be intersected within the vector file.
* out_vec_file - the vector file which will be outputted.
* out_vec_lyr - the layer name which will be outputted.
* out_vec_format - output vector format (default GPKG)

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
* vecFile - vector layer from which the attribute data comes from.
* vecLyr - the layer name from which the attribute data comes from.
* selVecFile - the vector file which will be intersected within the vector file.
* selVecLyr - the layer name which will be intersected within the vector file.
* outputVec - output vector file/path
* outVecLyrName - output vector layer
* outVecDrvr - the output vector layer type.
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

* bbox - a bounding box (xMin, xMax, yMin, yMax)
* x_size - Output grid size in X axis. If out_epsg_code or utm_grid defined then the grid size
           needs to be in the output unit.
* y_size - Output grid size in Y axis. If out_epsg_code or utm_grid defined then the grid size
           needs to be in the output unit.
* in_epsg_code - EPDSG code for the projection of the bbox
* out_vec - output vector file.
* out_vec_lyr - output vector layer name.
* vec_drv - output vector file format (see OGR codes). Default is GPKG.
* out_epsg_code - if provided the output grid is reprojected to the projection defined by this EPSG code.
                  (note. the grid size needs to the in the unit of this projection). Default is None.
* utm_grid - provide the output grid in UTM projection where grid might go across multiple UTM zones.
             Default is False. grid size unit should be metres.
* utm_hemi - if outputting a UTM projected grid then decided whether to use hemispheres or otherwise. If False
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
    
* vectorFile - output vector file/path
* vectorLyr - output vector layer
* vecDriver - the output vector layer type.
* epsgCode - EPSG code specifying the projection of the data (e.g., 4326 is WSG84 Lat/Long).
* bboxs - is a list of bounding boxes ([xMin, xMax, yMin, yMax]) to be saved to the output vector.
* atts - is a dict of lists of attributes with the same length as the bboxs list. The dict should be named
         the same as the attTypes['names'] list.
* attTypes - is a dict with a list of attribute names (attTypes['names']) and types (attTypes['types']).
             The list must be the same length as one another and the number of atts. Example type: ogr.OFTString
* overwrite - overwrite the vector file specified if it exists. Use False for GPKG where you want to add multiple layers.

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

* outputVec - outout file
* epsgCode - EPSG code of the output projection 
* grid_x - the size in the x axis of the grid cells.
* grid_y - the size in the y axis of the grid cells.
* bbox - the area for which cells will be defined (MinX, MaxX, MinY, MaxY).
* vecDriver - the output vector layer type.
* vecLyrName - output vector layer

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


def writePts2Vec(vectorFile, vectorLyr, vecDriver, epsgCode, ptsX, ptsY, atts=None, attTypes=None):
    """
This function creates a set of polygons for a set of bounding boxes. 

When creating an attribute the available data types are ogr.OFTString, ogr.OFTInteger, ogr.OFTReal
    
* vectorFile - output vector file/path
* vectorLyr - output vector layer
* vecDriver - the output vector layer type.
* epsgCode - EPSG code specifying the projection of the data (e.g., 4326 is WSG84 Lat/Long).
* ptsX - is a list of x coordinates.
* ptsY - is a list of y coordinates.
* atts - is a dict of lists of attributes with the same length as the bboxs list.
         The dict should be named the same as the attTypes['names'] list.
* attTypes - is a dict with a list of attribute names (attTypes['names']) and types (attTypes['types']).
             The list must be the same length as one another and the number of atts. Example type: ogr.OFTString

"""
    try:
        if len(ptsX) != len(ptsY):
            raise Exception("The X and Y coordinates lists are not the same length.")
        nPts = len(ptsX)
        
        gdal.UseExceptions()
        # Create the output Driver
        outDriver = ogr.GetDriverByName(vecDriver)
        # create the spatial reference, WGS84
        srs = osr.SpatialReference()
        srs.ImportFromEPSG(int(epsgCode))
        # Create the output Shapefile
        outDataSource = outDriver.CreateDataSource(vectorFile)
        outLayer = outDataSource.CreateLayer(vectorLyr, srs, geom_type=ogr.wkbPoint )
        
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
                field_defn = ogr.FieldDefn( attTypes['names'][i], attTypes['types'][i] )
                if outLayer.CreateField ( field_defn ) != 0:
                    raise Exception("Creating '" + attTypes['names'][i] + "' field failed.\n")
            addAtts = True
        elif not ((atts is None) and (attTypes is None)): 
            raise Exception('If atts or attTypes is not None then the other should also not be none and equalivent in length.')
            
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
        outDataSource = None
    except Exception as e:
        raise e


def bboxIntersectsVecLyr(vectorFile, vectorLyr, bbox):
    """
A function which tests where a feature within an inputted vector layer intersects
with a bounding box. 

* vectorFile - vector file/path
* vectorLyr - vector layer name
* bbox - the bounding box (xMin, xMax, yMin, yMax). Same projection as vector layer.

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


def createImgExtentLUT(imgList, vectorFile, vectorLyr, vecDriver, ignore_none_imgs=False, out_proj_wgs84=False,
                       overwrite_lut_file=False):
    """
Create a vector layer look up table (LUT) for a directory of images.

* imgList - list of input images for the LUT. All input images should be the same projection/coordinate system.
* vectorFile - output vector file/path
* vectorLyr - output vector layer
* vecDriver - the output vector layer type (e.g., GPKG).
* ignore_none_imgs - if a NULL epsg is returned from an image then ignore and don't include in LUT else throw exception.
* out_proj_wgs84 - if True then the image bounding boxes will be re-projected to EPSG:4326.
* overwrite_lut_file if True then output file will be overwritten. If false then not, e.g., can add extra layer to GPKG

Example::

    import glob
    import rsgislib.vectorutils
    imgList = glob.glob('/Users/pete/Temp/GabonLandsat/Hansen*.kea')
    rsgislib.vectorutils.createImgExtentLUT(imgList, './ImgExtents.gpkg', 'HansenImgExtents', 'GPKG')

"""
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
    for img in imgList:
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

* vecfile - input vector file
* veclyrname - input vector layer within the input file.
* outVecDrvr - the format driver for the output vector file (e.g., GPKG, ESRI Shapefile).
* vecoutfile - output file path for the vector.
* vecoutlyrname - output vector layer name.

"""
    gdal.UseExceptions()
    
    vecDS = gdal.OpenEx(vecfile, gdal.OF_VECTOR )
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
    nFeats = veclyr.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    veclyr.ResetReading()
    feat = veclyr.GetNextFeature()
    while feat is not None:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
        
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

    if openTransaction:
        result_lyr.CommitTransaction()
        openTransaction = False
    result_lyr.SyncToDisk()
    print(" Completed")

    vecDS = None
    result_ds = None


def lstveclyrcols(vecfile, veclyr):
    """
A function which returns a list of columns from the input vector layer.

* vecfile - input vector file.
* veclyr - input vector layer

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

* vecFile - vector file.
* vecLyr - layer within the vector file.

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


def subsetEnvsVecLyrObj(lyrVecObj, bbox):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

* lyrVecObj - OGR Layer Object.
* bbox - region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].

returns OGR Layer and Dataset objects.

"""
    gdal.UseExceptions()
    if lyrVecObj is None:
        raise Exception("Vector layer object which was provided was None.")
        
    lyr_spatial_ref = lyrVecObj.GetSpatialRef()
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


def readVecLyr2Mem(vecfile, veclyrname):
    """
A function which reads a vector layer to an OGR in memory layer.

* vecfile - input vector file
* veclyrname - input vector layer within the input file.

returns layer and datasets

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


def getMemVecLyrSubset(vecFile, vecLyr, bbox):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

* vecFile - vector layer from which the attribute data comes from.
* vecLyr - the layer name from which the attribute data comes from.
* bbox - region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].

returns OGR Layer and Dataset objects.

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

* veclyr - OGR vector layer object
* vecfile - output vector file
* veclyrname - output vector layer within the input file.
* vecDriver - the OGR driver for the output file.
* options - provide a list of driver specific options; see https://www.gdal.org/ogr_formats.html
* replace - if true the output file is replaced (i.e., overwritten to anything in an existing file will be lost).

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
