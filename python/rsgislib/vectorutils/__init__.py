"""
The vector utils module performs geometry / attribute table operations on vectors.
"""

# import the C++ extension into this level
from ._vectorutils import *

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False
    
haveOGRPy = True
try:
    import osgeo.ogr as ogr
except ImportError as ogrErr:
    haveOGRPy = False

import os.path

# Import the RSGISLib module
import rsgislib

# Import the RSGISLib Image Utils module
from rsgislib import imageutils

# Import the RSGISLib RasterGIS module
from rsgislib import rastergis

def rasterise2Image(inputVec, inputImage, outImage, gdalFormat):
    try:
        # Check gdal is available
        if not haveGDALPy:
            raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
        # Check ogr is available
        if not haveOGRPy:
            raise Exception("The OGR python bindings required for this function could not be imported\n\t" + ogrErr)
        
        print("Creating output image")
        imageutils.createCopyImage(inputImage, outImage, 1, 0, gdalFormat, rsgislib.TYPE_32UINT)
                
        tmpVector = os.path.splitext(inputVec)[0] + "_tmpFIDFile.shp"
        print("Added FID Column...")
        addFIDColumn(inputVec, tmpVector, True)
        
        
        print("Running Rasterise now...")
        outRasterDS = gdal.Open(outImage, gdal.GA_Update)
        
        inVectorDS = ogr.Open(tmpVector)
        inVectorLayer = inVectorDS.GetLayer(0)
        
        # Run the algorithm.
        err = gdal.RasterizeLayer(outRasterDS, [1], inVectorLayer, options=["ATTRIBUTE=FID"])
        if err != 0:
            raise Exception("Rasterisation Error: " + str(err))
        
        outRasterDS = None
        inVectorDS = None
        
        driver = ogr.GetDriverByName("ESRI Shapefile")
        if os.path.exists(tmpVector):
            driver.DeleteDataSource(tmpVector)
        
        print("Adding Colour Table")
        rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
        print("Completed")
    except Exception as e:
        raise e


def copyShapefile2RAT(inputVec, inputImage, outputImage):
    try:
        rasterise2Image(inputVec, inputImage, outputImage, "KEA")
        rsgislib.rastergis.importVecAtts(outputImage, inputVec, None)
    except Exception as e:
        raise e


