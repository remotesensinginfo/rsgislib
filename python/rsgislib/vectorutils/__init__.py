#!/usr/bin/env python
"""
The vector utils module performs geometry / attribute table operations on vectors.
"""

# import the C++ extension into this level
from ._vectorutils import *

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

    :param vec_file: input vector file.
    :param vec_lyr: input vector layer name
    :param outlyr: output vector layer name

    """

    def __init__(self, vec_file=None, vec_lyr=None, outlyr=None):
        """
        :param vec_file: input vector file.
        :param vec_lyr: input vector layer name
        :param outlyr: output vector layer name

        """
        self.vec_file = vec_file
        self.vec_lyr = vec_lyr
        self.outlyr = outlyr

def deleteVectorFile(vec_file: str, feedback:bool =True):
    """
    Function to delete an existing vector file.

    :param vec_file: vector file path
    :param feedback: Boolean specifying whether the function should print feedback to the console as files are delted.

    """
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

def getProjWKTFromVec(vec_file, vec_lyr=None):
    """
    A function which gets the WKT projection from the inputted vector file.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is a string with the input vector layer name, if None then first layer read. (default: None)

    :return: WKT representation of projection

    """
    dataset = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if dataset is None:
        raise Exception("Could not open file: {}".format(vec_file))
    if vec_lyr is None:
        layer = dataset.GetLayer()
    else:
        layer = dataset.GetLayer(vec_lyr)
    if layer is None:
        raise Exception("Could not open layer within file: {}".format(vec_file))
    spatialRef = layer.GetSpatialRef()
    return spatialRef.ExportToWkt()


def getProjEPSGFromVec(vec_file, vec_lyr=None):
    """
    A function which gets the EPSG projection from the inputted vector file.

    :param vec_file: is a string with the input vector file name and path.
    :param vec_lyr: is a string with the input vector layer name, if None then first layer read. (default: None)

    :return: EPSG representation of projection

    """
    dataset = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
    if dataset is None:
        raise Exception("Could not open file: {}".format(vec_file))
    if vec_lyr is None:
        layer = dataset.GetLayer()
    else:
        layer = dataset.GetLayer(vec_lyr)
    if layer is None:
        raise Exception("Could not open layer within file: {}".format(vec_file))
    spatialRef = layer.GetSpatialRef()
    spatialRef.AutoIdentifyEPSG()
    return spatialRef.GetAuthorityCode(None)









def getVecFeatCount(vec_file, vec_lyr=None, computeCount=True):
    """
Get a count of the number of features in the vector layers.

:param vec_file: is a string with the input vector file name and path.
:param vec_lyr: is the layer for which extent is to be calculated (Default: None). if None assume there is only one layer and that will be read.
:param computeCount: is a boolean which specifies whether the layer extent
                     should be calculated (rather than estimated from header)
                     even if that operation is computationally expensive.

:return: nfeats

"""
    gdal.UseExceptions()
    inDataSource = gdal.OpenEx(vec_file, gdal.OF_VECTOR )
    if vec_lyr is not None:
        inLayer = inDataSource.GetLayer(vec_lyr)
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
    import rsgislib.tools.utils
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
                    out_lyr = '{}_{}'.format(lyr, rsgislib.tools.utils.uid_generator())
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


def getVecLyrsLst(vec_file):
    """
A function which returns a list of available layers within the inputted vector file.

:param vec_file: file name and path to input vector layer.

:return: list of layer names (can be used with gdal.Dataset.GetLayerByName()).

"""
    gdalDataset = gdal.OpenEx(vec_file, gdal.OF_VECTOR )
    layerList = []
    for lyr_idx in range(gdalDataset.GetLayerCount()):
        lyr = gdalDataset.GetLayerByIndex(lyr_idx)
        tLyrName = lyr.GetName()
        if not tLyrName in layerList:
            layerList.append(tLyrName)
    gdalDataset = None
    return layerList


def getVecLayerExtent(vec_file, vec_lyr=None, computeIfExp=True):
    """
Get the extent of the vector layer.

:param vec_file: is a string with the input vector file name and path.
:param vec_lyr: is the layer for which extent is to be calculated (Default: None)
                  if None assume there is only one layer and that will be read.
:param computeIfExp: is a boolean which specifies whether the layer extent
                     should be calculated (rather than estimated from header)
                     even if that operation is computationally expensive.

:return: boundary box is returned (MinX, MaxX, MinY, MaxY)

"""
    gdal.UseExceptions()
    # Get a Layer's Extent
    inDataSource = gdal.OpenEx(vec_file, gdal.OF_VECTOR )
    if vec_lyr is not None:
        inLayer = inDataSource.GetLayer(vec_lyr)
    else:
        inLayer = inDataSource.GetLayer()
    extent = inLayer.GetExtent(computeIfExp)
    return extent



def splitVecLyr(vec_file, vec_lyr, nfeats, out_format, outdir, outvecbase, outvecend):
    """
A function which splits the input vector layer into a number of output layers.

:param vec_file: input vector file.
:param vec_lyr: input layer name.
:param nfeats: number of features within each output file.
:param out_format: output file driver.
:param outdir: output directory for the created output files.
:param outvecbase: output layer name will be the same as the base file name.
:param outvecend: file ending (e.g., .shp).

"""
    gdal.UseExceptions()
    datasrc = gdal.OpenEx(vec_file, gdal.OF_VECTOR )
    srcLyr = datasrc.GetLayer(vec_lyr)
    nInFeats = srcLyr.GetFeatureCount(True)
    print(nInFeats)
    
    nOutFiles = math.floor(nInFeats/nfeats)
    remainFeats = nInFeats - (nOutFiles*nfeats)
    print(nOutFiles)
    print(remainFeats)
    
    out_driver = ogr.GetDriverByName(out_format)
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


def reProjVectorLayer(in_vec_file, out_vec_file, outProjWKT, outDriverName='ESRI Shapefile', outLyrName=None,
                      inLyrName=None, inProjWKT=None, del_exist_vec=False):
    """
A function which reprojects a vector layer. You might also consider using rsgislib.vectorutils.vector_translate,
particularly if you are reprojecting the data and changing between coordinate units (e.g., degrees to meters)

:param in_vec_file: is a string with name and path to input vector file.
:param out_vec_file: is a string with name and path to output vector file.
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
    inDataSet = gdal.OpenEx(in_vec_file, gdal.OF_VECTOR )
    if inDataSet is None:
        raise Exception("Failed to open input vector file: {}".format(in_vec_file))
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
    if os.path.exists(out_vec_file):
        if (outDriverName == 'ESRI Shapefile'):
            if del_exist_vec:
                driver.DeleteDataSource(out_vec_file)
            else:
                raise Exception('Output shapefile already exists - stopping.')
            outDataSet = driver.Create(out_vec_file, 0, 0, 0, gdal.GDT_Unknown )
        else:
            outDataSet = gdal.OpenEx(out_vec_file, gdal.OF_UPDATE )
    else:
        outDataSet = driver.Create(out_vec_file, 0, 0, 0, gdal.GDT_Unknown )
    
    if outLyrName is None:
        outLyrName = os.path.splitext(os.path.basename(out_vec_file))[0]
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


def reproj_vec_lyr(in_vec_lyr, out_vec_file, out_epsg, out_format='MEMORY', out_lyr_name=None, in_epsg=None,
                      print_feedback=True):
    """
A function which reprojects a vector layer. You might also consider using rsgislib.vectorutils.vector_translate,
particularly if you are reprojecting the data and changing between coordinate units (e.g., degrees to meters)

:param in_vec_lyr: is a GDAL vector layer object.
:param out_vec_file: is a string with name and path to output vector file - is created.
:param out_epsg: is an int with the EPSG code to which the input vector layer is to be reprojected to.
:param out_format: is the output vector file format. Default is MEMORY - i.e., nothing written to disk.
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
    driver = ogr.GetDriverByName(out_format)
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


def getAttLstSelectFeats(vec_file, vec_lyr, attNames, selVecFile, selVecLyr):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vec_file: vector layer from which the attribute data comes from.
:param vec_lyr: the layer name from which the attribute data comes from.
:param attNames: a list of attribute names to be outputted.
:param selVecFile: the vector file which will be intersected within the vector file.
:param selVecLyr: the layer name which will be intersected within the vector file.
:return: list of dictionaries with the output values.

"""
    gdal.UseExceptions()
    att_vals = []
    try:
        dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '" + vec_file + "'")
        
        vec_lyr_obj = dsVecFile.GetLayerByName( vec_lyr )
        if vec_lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))
            
        dsSelVecFile = gdal.OpenEx(selVecFile, gdal.OF_READONLY )
        if dsSelVecFile is None:
            raise Exception("Could not open '" + selVecFile + "'")
        
        lyrSelVecObj = dsSelVecFile.GetLayerByName( selVecLyr )
        if lyrSelVecObj is None:
            raise Exception("Could not find layer '" + selVecLyr + "'")
        
        lyrDefn = vec_lyr_obj.GetLayerDefn()
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
        mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", geom_type=vec_lyr_obj.GetGeomType())
        
        for attName in attNames:
            mem_result_lyr.CreateField(ogr.FieldDefn(attName, feat_types[attName]))
        
        vec_lyr_obj.Intersection(mem_sel_lyr, mem_result_lyr)
        
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
        raise Exception("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    outvals = getAttLstSelectBBoxFeatsLyrObjs(vec_lyr_obj, attNames, bbox, bbox_epsg)
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


def selectIntersectFeats(vec_file, vec_lyr, roiVecFile, roiVecLyr, out_vec_file, out_vec_lyr, out_vec_format='GPKG'):
    """
Function to select the features which intersect with region of interest (ROI) features which will be outputted
into a new vector layer.

:param vec_file: vector layer from which the attribute data comes from.
:param vec_lyr: the layer name from which the attribute data comes from.
:param roiVecFile: the vector file which will be intersected within the vector file.
:param roiVecLyr: the layer name which will be intersected within the vector file.
:param out_vec_file: the vector file which will be outputted.
:param out_vec_lyr: the layer name which will be outputted.
:param out_vec_format: output vector format (default GPKG)

"""
    gdal.UseExceptions()
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY)
    if dsVecFile is None:
        raise Exception("Could not open '{}'".format(vec_file))

    vec_lyr_obj = dsVecFile.GetLayerByName(vec_lyr)
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))

    in_vec_lyr_spat_ref = vec_lyr_obj.GetSpatialRef()

    dsROIVecFile = gdal.OpenEx(roiVecFile, gdal.OF_READONLY)
    if dsROIVecFile is None:
        raise Exception("Could not open '" + roiVecFile + "'")

    lyrROIVecObj = dsROIVecFile.GetLayerByName(roiVecLyr)
    if lyrROIVecObj is None:
        raise Exception("Could not find layer '" + roiVecLyr + "'")

    lyrDefn = vec_lyr_obj.GetLayerDefn()

    mem_driver = ogr.GetDriverByName('MEMORY')
    mem_roi_ds = mem_driver.CreateDataSource('MemSelData')
    mem_roi_lyr = mem_roi_ds.CopyLayer(lyrROIVecObj, roiVecLyr, ['OVERWRITE=YES'])

    out_driver = ogr.GetDriverByName(out_vec_format)
    result_ds = out_driver.CreateDataSource(out_vec_file)
    result_lyr = result_ds.CreateLayer(out_vec_lyr, in_vec_lyr_spat_ref, geom_type=vec_lyr_obj.GetGeomType())

    vec_lyr_obj.Intersection(mem_roi_lyr, result_lyr)

    dsVecFile = None
    dsROIVecFile = None
    mem_roi_ds = None
    result_ds = None


def exportSpatialSelectFeats(vec_file, vec_lyr, selVecFile, selVecLyr, out_vec_file, out_vec_lyr, out_format):
    """
Function to get a list of attribute values from features which intersect
with the select layer.

:param vec_file: vector layer from which the attribute data comes from.
:param vec_lyr: the layer name from which the attribute data comes from.
:param selVecFile: the vector file which will be intersected within the vector file.
:param selVecLyr: the layer name which will be intersected within the vector file.
:param out_vec_file: output vector file/path
:param out_vec_lyr: output vector layer
:param out_format: the output vector layer type.

"""
    gdal.UseExceptions()
    att_vals = []
    try:
        dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '{}'".format(vec_file))
        
        vec_lyr_obj = dsVecFile.GetLayerByName( vec_lyr )
        if vec_lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))
        
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
        
        out_driver = ogr.GetDriverByName(out_format)
        result_ds = out_driver.CreateDataSource(out_vec_file)
        result_lyr = result_ds.CreateLayer(out_vec_lyr, lyr_spatial_ref, geom_type=vec_lyr_obj.GetGeomType())
        
        srcLayerDefn = vec_lyr_obj.GetLayerDefn()
        for i in range(srcLayerDefn.GetFieldCount()):
            fieldDefn = srcLayerDefn.GetFieldDefn(i)
            result_lyr.CreateField(fieldDefn)
        rsltLayerDefn = result_lyr.GetLayerDefn()   
        
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

                utm_bbox = rsgislib.tools.geometrytools.reprojBBOX(zone_roi[1], in_proj_obj, out_proj_obj)
                bboxs = rsgislib.tools.geometrytools.getBBoxGrid(utm_bbox, x_size, y_size)

                utm_out_vec_lyr = out_vec_lyr + '_utm{0}{1}'.format(zone_roi[0], utm_top_hemi.lower())
                createPolyVecBBOXs(out_vec, utm_out_vec_lyr, vec_drv, utm_proj_epsg, bboxs, overwrite=first)
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
            createPolyVecBBOXs(out_vec, out_vec_lyr, vec_drv, in_epsg_code, bboxs)
        else:
            createPolyVecBBOXs(out_vec, out_vec_lyr, vec_drv, out_epsg_code, bboxs)


def createPolyVecBBOXs(vec_file, vectorLyr, out_format, epsgCode, bboxs, atts=None, attTypes=None, overwrite=True):
    """
This function creates a set of polygons for a set of bounding boxes.
When creating an attribute the available data types are ogr.OFTString, ogr.OFTInteger, ogr.OFTReal

:param vec_file: output vector file/path
:param vectorLyr: output vector layer
:param out_format: the output vector layer type.
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


def createVectorGrid(out_vec_file, out_format, out_vec_lyr, epsgCode, grid_x, grid_y, bbox):
    """
A function which creates a regular grid across a defined area.

:param out_vec_file: outout file
:param epsgCode: EPSG code of the output projection
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
    
    createPolyVecBBOXs(out_vec_file, out_vec_lyr, out_format, epsgCode, bboxs)


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
            vecDS = gdal.OpenEx(vec_file, gdal.GA_Update )
        else:
            if os.path.exists(vec_file):
                deleteVectorFile(vec_file)
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


def bboxIntersectsVecLyr(vec_file, vectorLyr, bbox):
    """
A function which tests whether a feature within an inputted vector layer intersects
with a bounding box. 

:param vec_file: vector file/path
:param vectorLyr: vector layer name
:param bbox: the bounding box (xMin, xMax, yMin, yMax). Same projection as vector layer.
:returns: boolean (True = Intersection)

"""
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY )
    if dsVecFile is None:
        raise Exception("Could not open '{}'".format(vec_file))
    
    vec_lyr_obj = dsVecFile.GetLayerByName( vectorLyr )
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '" + vectorLyr + "'")
    
    # Get a geometry collection object for shapefile.
    geom_collect = ogr.Geometry(ogr.wkbGeometryCollection)
    for feat in vec_lyr_obj:
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





def createImgExtentLUT(imgList, vec_file, vectorLyr, out_format, ignore_none_imgs=False, out_proj_wgs84=False,
                       overwrite_lut_file=False):
    """
Create a vector layer look up table (LUT) for a directory of images.

:param imgList: list of input images for the LUT. All input images should be the same projection/coordinate system.
:param vec_file: output vector file/path
:param vectorLyr: output vector layer
:param out_format: the output vector layer type (e.g., GPKG).
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
        epsgCodeTmp = rsgislib.imageutils.getEPSGProjFromImage(img)
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
                img_bbox = rsgislib.imageutils.getImageBBOXInProj(img, 4326)
            else:
                img_bbox = rsgislib.imageutils.getImageBBOX(img)

            bboxs.append(img_bbox)
            baseName = os.path.basename(img)
            filePath = os.path.dirname(img)
            atts['filename'].append(baseName)
            atts['path'].append(filePath)
    # Create vector layer
    createPolyVecBBOXs(vec_file, vectorLyr, out_format, epsgCode, bboxs, atts, attTypes, overwrite=overwrite_lut_file)



def getVecLyrCols(vec_file, vec_lyr):
    """
A function which returns a list of columns from the input vector layer.

:param vec_file: input vector file.
:param vec_lyr: input vector layer
:returns: list of column names

"""
    gdal.UseExceptions()
    atts = []
    
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY )
    if dsVecFile is None:
        raise Exception("Could not open '{}'".format(vec_file))
    
    vec_lyr_obj = dsVecFile.GetLayerByName( vec_lyr )
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))
    
    lyrDefn = vec_lyr_obj.GetLayerDefn()
    for i in range(lyrDefn.GetFieldCount()):
        atts.append(lyrDefn.GetFieldDefn(i).GetName())
    return atts


def getFeatEnvs(vec_file, vec_lyr):
    """
A function which returns a list of bounding boxes for each feature
within the vector layer.

:param vec_file: vector file.
:param vec_lyr: layer within the vector file.
:returns: list of BBOXs

"""
    dsVecFile = gdal.OpenEx(vec_file, gdal.OF_VECTOR )
    if dsVecFile is None:
        raise Exception("Could not open '{}'".format(vec_file))
        
    vec_lyr_obj = dsVecFile.GetLayerByName( vec_lyr )
    if vec_lyr_obj is None:
        raise Exception("Could not find layer '{}'".format(vec_lyr))
    
    openTransaction = False
    nFeats = vec_lyr_obj.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    print("Started .0.", end='', flush=True)
    outenvs = []
    # loop through the input features
    inFeature = vec_lyr_obj.GetNextFeature()
    while inFeature:
        if (nFeats>10) and (counter == feedback_next):
            print(".{}.".format(feedback), end='', flush=True)
            feedback_next = feedback_next + step
            feedback = feedback + 10
            
        # get the input geometry
        geom = inFeature.GetGeometryRef()
        if geom is not None:
            outenvs.append(geom.GetEnvelope())
        
        inFeature = vec_lyr_obj.GetNextFeature()
        counter = counter + 1
    print(" Completed")
    dsVecFile = None
    return outenvs


def subsetEnvsVecLyrObj(vec_lyr_obj, bbox, epsg=None):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

:param vec_lyr_obj: OGR Layer Object.
:param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].
:param epsg: provide an EPSG code for the layer if not well defined by the input layer.

:return: OGR Layer and Dataset objects.

"""
    gdal.UseExceptions()
    if vec_lyr_obj is None:
        raise Exception("Vector layer object which was provided was None.")

    if epsg is not None:
        lyr_spatial_ref = osr.SpatialReference()
        lyr_spatial_ref.ImportFromEPSG(epsg)
    else:
        lyr_spatial_ref = vec_lyr_obj.GetSpatialRef()
    if lyr_spatial_ref is None:
        raise Exception("The spatial reference for the layer is None - please provide EPSG code.")
    lyrDefn = vec_lyr_obj.GetLayerDefn()
    
    # Copy the Layer to a new in memory OGR Layer.
    mem_driver = ogr.GetDriverByName('MEMORY')
    mem_result_ds = mem_driver.CreateDataSource('MemResultData')    
    mem_result_lyr = mem_result_ds.CreateLayer("MemResultLyr", lyr_spatial_ref, geom_type=vec_lyr_obj.GetGeomType())
    for i in range(lyrDefn.GetFieldCount()):
        fieldDefn = lyrDefn.GetFieldDefn(i)
        mem_result_lyr.CreateField(fieldDefn)
        
    openTransaction = False
    trans_step = 20000
    next_trans = trans_step
    nFeats = vec_lyr_obj.GetFeatureCount(True)
    step = math.floor(nFeats/10)
    feedback = 10
    feedback_next = step
    counter = 0
    vec_lyr_obj.ResetReading()
    print("Started .0.", end='', flush=True)
    outenvs = []
    # loop through the input features
    inFeature = vec_lyr_obj.GetNextFeature()
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
        
        inFeature = vec_lyr_obj.GetNextFeature()
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


def readVecLyr2Mem(vec_file, vec_lyr):
    """
A function which reads a vector layer to an OGR in memory layer.

:param vec_file: input vector file
:param vec_lyr: input vector layer within the input file.

:return: ogr_dataset, ogr_layer

"""
    gdal.UseExceptions()
    try:
        vecDS = gdal.OpenEx(vec_file, gdal.OF_READONLY )
        if vecDS is None:
            raise Exception("Could not open '{}'".format(vec_file))
        
        vec_lyr_obj = vecDS.GetLayerByName( vec_lyr )
        if vec_lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))
            
        mem_driver = ogr.GetDriverByName('MEMORY')
        
        mem_ds = mem_driver.CreateDataSource('MemSelData')
        mem_lyr = mem_ds.CopyLayer(vec_lyr_obj, vec_lyr, ['OVERWRITE=YES'])
            
    except Exception as e:
        print("Error Vector File: {}".format(vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(vec_lyr), file=sys.stderr)
        raise e
    return mem_ds, mem_lyr


def openGDALVecLyr(vec_file, vec_lyr=None):
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


def getMemVecLyrSubset(vec_file, vec_lyr, bbox):
    """
Function to get an ogr vector layer for the defined bounding box. The returned
layer is returned as an in memory ogr Layer object.

:param vec_file: vector layer from which the attribute data comes from.
:param vec_lyr: the layer name from which the attribute data comes from.
:param bbox: region of interest (bounding box). Define as [xMin, xMax, yMin, yMax].

:return: OGR Layer and Dataset objects.

"""
    gdal.UseExceptions()
    try:
        dsVecFile = gdal.OpenEx(vec_file, gdal.OF_READONLY )
        if dsVecFile is None:
            raise Exception("Could not open '{}'".format(vec_file))
        
        vec_lyr_obj = dsVecFile.GetLayerByName( vec_lyr )
        if vec_lyr_obj is None:
            raise Exception("Could not find layer '{}'".format(vec_lyr))
            
        mem_result_ds, mem_result_lyr = subsetEnvsVecLyrObj(vec_lyr_obj, bbox)
        
    except Exception as e:
        print("Error: Layer: {} File: {}".format(vec_lyr, vec_file))
        raise e
    return mem_result_ds, mem_result_lyr


def writeVecLyr2File(vec_lyr_obj, out_vec_file, out_vec_lyr, out_format, options=[], replace=False):
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
    gdal.UseExceptions()
    try:
        if os.path.exists(out_vec_file) and replace:
            deleteVectorFile(out_vec_file)

        if os.path.exists(out_vec_file) and (not replace):
            vecDS = gdal.OpenEx(out_vec_file, gdal.GA_Update )
        else:
            outdriver = ogr.GetDriverByName(out_format)
            vecDS = outdriver.CreateDataSource(out_vec_file)
        
        if vecDS is None:
            raise Exception("Could not open or create '{}'".format(out_vec_file))

        vecDS_lyr = vecDS.CopyLayer(vec_lyr_obj, out_vec_lyr, options)
        vecDS = None
            
    except Exception as e:
        print("Error Vector File: {}".format(out_vec_file), file=sys.stderr)
        print("Error Vector Layer: {}".format(out_vec_lyr), file=sys.stderr)
        raise e

def createCopyVectorLyr(vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_format, options=[], replace=False, in_memory=False):
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
        vec_obj_ds, vec_lyr_obj = readVecLyr2Mem(vec_file, vec_lyr)
    else:
        vec_obj_ds, vec_lyr_obj = openGDALVecLyr(vec_file, vec_lyr)

    writeVecLyr2File(vec_lyr_obj, out_vec_file, out_vec_lyr, out_format, options, replace)

    vec_obj_ds = None


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
        raise Exception("Could not find layer '{}'".format(vec_file))

    rat_cols_all = rsgislib.rastergis.getRATColumnsInfo(clumps_img)

    cols_exist = []
    for ratcol in ratcols:
        if ratcol not in rat_cols_all:
            raise Exception("Column '{}' is not within the clumps image: {}".format(ratcol, clumps_img))
        cols_exist.append(False)

    if outcoltypes is None:
        outcoltypes = []
        for att_column in ratcols:
            rat_type = rat_cols_all[att_column]['type']
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
        for att_column in outcolnames:
            if lyrDefn.GetFieldDefn(i).GetName().lower() == att_column.lower():
                cols_exist[col_n] = True
                break
            col_n = col_n + 1

    col_n = 0
    for att_column in outcolnames:
        if not cols_exist[col_n]:
            field_defn = ogr.FieldDefn(att_column, outcoltypes[col_n])
            if vec_lyr_obj.CreateField(field_defn) != 0:
                raise Exception(
                    "Creating '{}' field failed; becareful with case, some drivers are case insensitive but column might not be found.".format(
                        att_column))

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
                           out_format=None, join_how="inner", join_op="within"):
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
    :param out_format: The output vector file format, if none then shapefile outputted (Default None)
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

    if out_format is None:
        join_gpg_df.to_file(output_vec_file)
    else:
        if output_lyr is None:
            join_gpg_df.to_file(output_vec_file, driver=out_format)
        else:
            join_gpg_df.to_file(output_vec_file, layer=output_lyr, driver=out_format)


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
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.tools.geometrytools

    # Does the input image BBOX intersect the BBOX of the ROI vector?
    if vec_epsg is None:
        vec_epsg = getProjEPSGFromVec(roi_vec_file, roi_vec_lyr)
    img_epsg = rsgislib.imageutils.getEPSGProjFromImage(input_vmsk_img)
    if img_epsg == vec_epsg:
        img_bbox = rsgislib.imageutils.getImageBBOX(input_vmsk_img)
        projs_match = True
    else:
        img_bbox = rsgislib.imageutils.getImageBBOXInProj(input_vmsk_img, vec_epsg)
        projs_match = False
    vec_bbox = getVecLayerExtent(roi_vec_file, roi_vec_lyr, computeIfExp=True)

    img_intersect = False
    if rsgislib.tools.geometrytools.do_bboxes_intersect(img_bbox, vec_bbox):
        uid_str = rsgislib.tools.utils.uid_generator()
        base_vmsk_img = rsgislib.tools.filetools.get_file_basename(input_vmsk_img)

        tmp_file_dir = os.path.join(tmp_dir, "{}_{}".format(base_vmsk_img, uid_str))
        if not os.path.exists(tmp_file_dir):
            os.mkdir(tmp_file_dir)

        # Rasterise the vector layer to the input image extent.
        mem_ds, mem_lyr = getMemVecLyrSubset(roi_vec_file, roi_vec_lyr, img_bbox)

        if not projs_match:
            mem_result_ds, mem_result_lyr = reproj_vec_lyr(mem_lyr, 'mem_vec', img_epsg,
                                                           out_format='MEMORY', out_lyr_name=None,
                                                           in_epsg=None, print_feedback=False)
            mem_ds = None
        else:
            mem_result_ds = mem_ds
            mem_result_lyr = mem_lyr

        roi_img = os.path.join(tmp_file_dir, "{}_roiimg.kea".format(base_vmsk_img))
        rsgislib.imageutils.create_copy_img(input_vmsk_img, roi_img, 1, 0, 'KEA', rsgislib.TYPE_8UINT)
        rasteriseVecLyrObj(mem_result_lyr, roi_img, burnVal=1, vecAtt=None, calcstats=True, thematic=True, nodata=0)
        mem_result_ds = None

        bandDefns = []
        bandDefns.append(rsgislib.imagecalc.BandDefn('vmsk', input_vmsk_img, 1))
        bandDefns.append(rsgislib.imagecalc.BandDefn('roi', roi_img, 1))
        intersect_img = os.path.join(tmp_file_dir, "{}_intersectimg.kea".format(base_vmsk_img))
        rsgislib.imagecalc.band_math(intersect_img, "(vmsk==1) && (roi==1)?1:0", 'KEA', rsgislib.TYPE_8UINT, bandDefns)
        rsgislib.rastergis.pop_rat_img_stats(intersect_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True)
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
        vec_lyr_obj = openGDALVecLyr(vec.vec_file, vec.vec_lyr)
        if first and overwrite:
            rsgislib.vectorutils.writeVecLyr2File(vec_lyr_obj, output_file, vec.outlyr, format,
                                                  options=['OVERWRITE=YES'], replace=True)
        else:
            rsgislib.vectorutils.writeVecLyr2File(vec_lyr_obj, output_file, vec.outlyr, format)
        vec_lyr_obj = None
        first = False




def vectorTranslate(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr=None, out_format='GPKG',
                     drv_create_opts=[], lyr_create_opts=[], access_mode=None, src_srs=None,
                     dst_srs=None, del_exist_vec=False):
    """
    A function which translates a vector file to another format, similar to ogr2ogr. If you wish
    to reproject the input file then provide a destination srs (e.g., "EPSG:27700", or wkt string,
    or proj4 string).

    :param in_vec_file: the input vector file.
    :param in_vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same as the input).
    :param out_format: the output vector file format (e.g., GPKG, GEOJSON, ESRI Shapefile, etc.)
    :param drv_create_opts: a list of options for the creation of the output file.
    :param lyr_create_opts: a list of options for the creation of the output layer.
    :param access_mode: default is None for creatoion but other but other options are:
                        [None (creation), 'update', 'append', 'overwrite']
    :param src_srs: provide a source spatial reference for the input vector file. Default=None.
                    can be used to provide a projection where none has been specified or the
                    information has gone missing. Can be used without performing a reprojection.
    :param dst_srs: provide a spatial reference for the output vector file to be reprojected to. (Default=None)
                    If specified then the file will be reprojected.
    :param del_exist_vec: remove output file if it exists.

    """
    from osgeo import gdal
    gdal.UseExceptions()

    if access_mode is not None:
        if access_mode not in ['update', 'append', 'overwrite']:
            raise Exception("access_mode must be one of: [None (creation), "
                            "'update', 'append', 'overwrite']")

    if os.path.exists(out_vec_file):
        if del_exist_vec:
            deleteVectorFile(out_vec_file)
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
                                       format=out_format,
                                       accessMode=access_mode,
                                       srcSRS=src_srs,
                                       dstSRS=dst_srs,
                                       reproject=reproject_lyr,
                                       layerCreationOptions=lyr_create_opts,
                                       layers=in_vec_lyr,
                                       layerName=out_vec_lyr,
                                       callback=callback)

    gdal.VectorTranslate(out_vec_file, in_vec_file, options=opts)



def reproj_wgs84_vec_to_utm(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr=None, use_hemi=True,
                            out_format='GPKG', drv_create_opts=[], lyr_create_opts=[],
                            access_mode='overwrite', del_exist_vec=False):
    """
    A function which reprojects an input file projected in WGS84 (EPSG:4326) to UTM, where the UTM zone is
    automatically identified using the mean x and y.

    :param in_vec_file: the input vector file.
    :param in_vec_lyr: the input vector layer name
    :param out_vec_file: the output vector file.
    :param out_vec_lyr: the name of the output vector layer (if None then the same as the input).
    :param use_hemi: True differentiate between Southern and Northern hemisphere. False use Northern hemisphere.
    :param out_format: the output vector file format (e.g., GPKG, GEOJSON, ESRI Shapefile, etc.)
    :param drv_create_opts: a list of options for the creation of the output file.
    :param lyr_create_opts: a list of options for the creation of the output layer.
    :param access_mode: by default the function overwrites the output file but other
                        options are: ['update', 'append', 'overwrite']
    :param del_exist_vec: remove output file if it exists.

    """
    import rsgislib.tools.utm
    from rsgislib.vectorutils import vector_translate
    from osgeo import gdal
    import os
    import tqdm

    if os.path.exists(out_vec_file):
        if del_exist_vec:
            deleteVectorFile(out_vec_file)
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
    vector_translate(in_vec_file, in_vec_lyr, out_vec_file, out_vec_lyr, out_format,
                     drv_create_opts, lyr_create_opts, access_mode, src_srs='EPSG:4326',
                     dst_srs=dst_srs_str)



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
    import rsgislib.tools.utils
    if multi_layers:
        if out_vec_file is None:
            raise Exception("If a multiple layer output is specified then an output file needs to be specified "
                            "to which the layer need to be added.")
    if not multi_layers:
        if (out_file_path is None) or (out_file_ext is None):
            raise Exception("If a single layer output is specified then an output file path "
                            "and file extention needs to be specified.")

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
                val = rsgislib.tools.utils.check_str(val, rm_non_ascii=True, rm_dashs=True, rm_spaces=False, rm_punc=True)
            c_gpdf.to_file(out_vec_file, layer=val, driver='GPKG')
        else:
            if chk_lyr_names:
                val = rsgislib.tools.utils.check_str(val, rm_non_ascii=True, rm_dashs=True, rm_spaces=False, rm_punc=True)
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
    import rsgislib.tools.filetools
    for vec_file in tqdm.tqdm(input_vecs):
        lyrs = getVecLyrsLst(vec_file)
        basename = rsgislib.tools.filetools.get_file_basename(vec_file)
        out_vec_file = os.path.join(output_dir, "{}.{}".format(basename, out_file_ext))
        for lyr in lyrs:
            explode_vec_lyr(vec_file, lyr, out_vec_file, lyr, out_format)



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
    import rsgislib.tools.utils
    import rsgislib.tools.geometrytools
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
    
    first = True
    for file in tqdm.tqdm(input_files):
        lyrs = getVecLyrsLst(file)
        for lyr in lyrs:
            bbox = getVecLayerExtent(file, vec_lyr=lyr)
            bbox_area = rsgislib.tools.geometrytools.calc_bbox_area(bbox)
            if bbox_area > 0:
                vec_epsg = getProjEPSGFromVec(file, vec_lyr=lyr)
                zone, hemi = rsgislib.tools.utm.utm_from_epsg(int(vec_epsg))
                zone_str = rsgislib.tools.utils.zero_pad_num_str(zone, str_len=2, round_num=False, round_n_digts=0, integerise=True)

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





