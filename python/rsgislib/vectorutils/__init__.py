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

def rasterise2Image(inputVec, inputImage, outImage, gdalFormat="KEA", shpAtt=None):
    """ A utillity to rasterise a shapefile into an image covering the same region and at the same resolution as the input image. 

Where:

* inputVec is a string specifying the input vector (shapefile) file
* inputImage is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation
* outImage is a string specifying the output image for the rasterised shapefile
* gdalFormat is the output image format (Default: KEA).
* shpAtt is a string specifying the attribute to be rasterised, value of None creates a 
              binary mask and \"FID\" creates a temp shapefile with a "FID" column and rasterises that column.

Example::

    from rsgislib import vectorutils
    
    inputVector = 'crowns.shp'
    inputImage = 'injune_p142_casi_sub_utm.kea'
    outputImage = 'psu142_crowns.kea'  
    vectorutils.rasterise2Image(inputVector, inputImage, outputImage, 'KEA', 'FID')


    """
    try:
        # Check gdal is available
        if not haveGDALPy:
            raise Exception("The GDAL python bindings required for this function could not be imported\n\t" + gdalErr)
        # Check ogr is available
        if not haveOGRPy:
            raise Exception("The OGR python bindings required for this function could not be imported\n\t" + ogrErr)
        
        print("Creating output image")
        imageutils.createCopyImage(inputImage, outImage, 1, 0, gdalFormat, rsgislib.TYPE_32UINT)
        
        if shpAtt == "FID":   
            tmpVector = os.path.splitext(inputVec)[0] + "_tmpFIDFile.shp"
            print("Added FID Column...")
            addFIDColumn(inputVec, tmpVector, True)
        else:
            tmpVector = inputVec
        
        print("Running Rasterise now...")
        outRasterDS = gdal.Open(outImage, gdal.GA_Update)
        
        inVectorDS = ogr.Open(tmpVector)
        inVectorLayer = inVectorDS.GetLayer(0)
        
        # Run the algorithm.
        err = 0
        if shpAtt == None:
            err = gdal.RasterizeLayer(outRasterDS, [1], inVectorLayer, burn_values=[1])
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
        rastergis.populateStats(clumps=outImage, addclrtab=True, calcpyramids=True, ignorezero=True)
        print("Completed")
    except Exception as e:
        raise e


def copyShapefile2RAT(inputVec, inputImage, outputImage):
    """ A utillity to create raster copy of a shapefile. The output image is a KEA file and the attribute table has the attributes from the shapefile. 
    
Where:

* inputVec is a string specifying the input vector (shapefile) file
* inputImage is a string specifying the input image defining the grid, pixel resolution and area for the rasterisation
* outputImage is a string specifying the output KEA image for the rasterised shapefile

Example::

    from rsgislib import vectorutils
     
    inputVector = 'crowns.shp'
    inputImage = 'injune_p142_casi_sub_utm.kea'
    outputImage = 'psu142_crowns.kea'
        
    vectorutils.copyShapefile2RAT(inputVector, inputImage, outputImage)

"""
    try:
        rasterise2Image(inputVec, inputImage, outputImage, "KEA")
        rsgislib.rastergis.importVecAtts(outputImage, inputVec, None)
    except Exception as e:
        raise e


