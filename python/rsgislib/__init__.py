"""
This namespace contains rsgislib Python bindings 

Please be aware that the following variables have been 
defined to match enums within RSGISLib.

Data Types for images:

    * TYPE_UNDEFINED = 0
    * TYPE_8INT = 1
    * TYPE_16INT = 2
    * TYPE_32INT = 3
    * TYPE_64INT = 4
    * TYPE_8UINT = 5
    * TYPE_16UINT = 6
    * TYPE_32UINT = 7
    * TYPE_64UINT = 8
    * TYPE_32FLOAT = 9
    * TYPE_64FLOAT = 10

Methods for the Maximum Likelihood Classifier:

    * METHOD_SAMPLES = 0        # as calculated by ML
    * METHOD_AREA = 1           # priors set by the relative area
    * METHOD_EQUAL = 2          # priors all equal
    * METHOD_USERDEFINED = 3    # priors passed in to function
    * METHOD_WEIGHTED = 4       # priors by area but with a weight applied

Shape indexes used with RasterGIS:

    * SHAPE_SHAPENA = 0
    * SHAPE_SHAPEAREA = 1
    * SHAPE_ASYMMETRY = 2
    * SHAPE_BORDERINDEX = 3
    * SHAPE_BORDERLENGTH = 4
    * SHAPE_COMPACTNESS = 5
    * SHAPE_DENSITY = 6
    * SHAPE_ELLIPTICFIT = 7
    * SHAPE_LENGTH = 8
    * SHAPE_LENGTHWIDTH = 9
    * SHAPE_WIDTH = 10
    * SHAPE_MAINDIRECTION = 11
    * SHAPE_RADIUSLARGESTENCLOSEDELLIPSE = 12
    * SHAPE_RADIUSSMALLESTENCLOSEDELLIPSE = 13
    * SHAPE_RECTANGULARFIT = 14
    * SHAPE_ROUNDNESS = 15
    * SHAPE_SHAPEINDEX = 16

Methods of initialising KMEANS:

    * INITCLUSTER_RANDOM = 0
    * INITCLUSTER_DIAGONAL_FULL = 1
    * INITCLUSTER_DIAGONAL_STDDEV = 2
    * INITCLUSTER_DIAGONAL_FULL_ATTACH = 3
    * INITCLUSTER_DIAGONAL_STDDEV_ATTACH = 4
    * INITCLUSTER_KPP = 5
    
    
Methods of calculating distance:

    * DIST_UNDEFINED = 0
    * DIST_EUCLIDEAN = 1
    * DIST_MAHALANOBIS = 2
    * DIST_MANHATTEN = 3
    * DIST_MINKOWSKI = 4
    * DIST_CHEBYSHEV = 5
    * DIST_MUTUALINFO = 6
    
Methods of summerising data:

    * SUMTYPE_UNDEFINED = 0
    * SUMTYPE_MODE = 1
    * SUMTYPE_MEAN = 2
    * SUMTYPE_MEDIAN = 3
    * SUMTYPE_MIN = 4
    * SUMTYPE_MAX = 5
    * SUMTYPE_STDDEV = 6
    * SUMTYPE_COUNT = 7
    * SUMTYPE_RANGE = 8
    * SUMTYPE_SUM = 9
    
Constants specifying how bands should be treated when sharpening (see rsgislib.imageutils)
    * SHARP_RES_IGNORE = 0
    * SHARP_RES_LOW = 1
    * SHARP_RES_HIGH = 2

"""
import os.path
import os
import time
import datetime
import math

import osgeo.osr as osr
import osgeo.ogr as ogr
import osgeo.gdal as gdal

TYPE_UNDEFINED = 0
TYPE_8INT = 1
TYPE_16INT = 2
TYPE_32INT = 3
TYPE_64INT = 4
TYPE_8UINT = 5
TYPE_16UINT = 6
TYPE_32UINT = 7
TYPE_64UINT = 8
TYPE_32FLOAT = 9
TYPE_64FLOAT = 10

DIST_UNDEFINED = 0
DIST_EUCLIDEAN = 1
DIST_MAHALANOBIS = 2
DIST_MANHATTEN = 3
DIST_MINKOWSKI = 4
DIST_CHEBYSHEV = 5
DIST_MUTUALINFO = 6

SUMTYPE_UNDEFINED = 0
SUMTYPE_MODE = 1
SUMTYPE_MEAN = 2
SUMTYPE_MEDIAN = 3
SUMTYPE_MIN = 4
SUMTYPE_MAX = 5
SUMTYPE_STDDEV = 6
SUMTYPE_COUNT = 7
SUMTYPE_RANGE = 8
SUMTYPE_SUM = 9

METHOD_SAMPLES = 0      # as calculated by ML
METHOD_AREA = 1         # priors set by the relative area
METHOD_EQUAL = 2        # priors all equal
METHOD_USERDEFINED = 3  # priors passed in to function
METHOD_WEIGHTED = 4     # priors by area but with a weight applied

SHAPE_SHAPENA = 0
SHAPE_SHAPEAREA = 1
SHAPE_ASYMMETRY = 2
SHAPE_BORDERINDEX = 3
SHAPE_BORDERLENGTH = 4
SHAPE_COMPACTNESS = 5
SHAPE_DENSITY = 6
SHAPE_ELLIPTICFIT = 7
SHAPE_LENGTH = 8
SHAPE_LENGTHWIDTH = 9
SHAPE_WIDTH = 10
SHAPE_MAINDIRECTION = 11
SHAPE_RADIUSLARGESTENCLOSEDELLIPSE = 12
SHAPE_RADIUSSMALLESTENCLOSEDELLIPSE = 13
SHAPE_RECTANGULARFIT = 14
SHAPE_ROUNDNESS = 15
SHAPE_SHAPEINDEX = 16

INITCLUSTER_RANDOM = 0
INITCLUSTER_DIAGONAL_FULL = 1
INITCLUSTER_DIAGONAL_STDDEV = 2
INITCLUSTER_DIAGONAL_FULL_ATTACH = 3
INITCLUSTER_DIAGONAL_STDDEV_ATTACH = 4
INITCLUSTER_KPP = 5

SHARP_RES_IGNORE = 0
SHARP_RES_LOW = 1
SHARP_RES_HIGH = 2

def getRSGISLibVersion():
    """ Calls rsgis-config to get the version number. """

    # Try calling rsgis-config to get minor version number
    try:
        import subprocess
        out = subprocess.Popen('rsgis-config --version',shell=True,stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        (stdout, stderr) = out.communicate()
        versionStr = stdout.decode()
        versionStr = versionStr.split('\n')[0]
    except Exception:
        versionStr = 'NA'
    return(versionStr)

__version__ = getRSGISLibVersion()

class RSGISPyException(Exception):
    """
    A class representing the RSGIS exception.
    """
    
    def __init__(self, value):
        """
        Init for the RSGISPyException class

        """
        self.value = value
        
    def __str__(self):
        """
        Return a string representation of the exception

        """
        return repr(self.value)


class RSGISPyUtils (object):
    """
    A class with useful utilities within RSGISLib.
    """
    
    def getFileExtension(self, gdalformat):
        """
        A function to get the extension for a given file format 
        (NOTE, currently only KEA, GTIFF, HFA, PCI and ENVI are supported).

        :return: string

        """
        ext = ".NA"
        if gdalformat.lower() == "kea":
            ext = ".kea"
        elif gdalformat.lower() == "gtiff":
            ext = ".tif"
        elif gdalformat.lower() == "hfa":
            ext = ".img"
        elif gdalformat.lower() == "envi":
            ext = ".env"
        elif gdalformat.lower() == "pcidsk":
            ext = ".pix"
        else:
            raise RSGISPyException("The extension for the gdalformat specified is unknown.")
        return ext
    
    def getGDALFormatFromExt(self, fileName):
        """
        Get GDAL format, based on filename

        :return: string

        """
        gdalStr = ''
        extension = os.path.splitext(fileName)[-1] 
        if extension == '.env':
            gdalStr = 'ENVI'
        elif extension == '.kea':
            gdalStr = 'KEA'
        elif extension == '.tif' or extension == '.tiff':
            gdalStr = 'GTiff'
        elif extension == '.img':
            gdalStr = 'HFA'
        elif extension == '.pix':
            gdalStr = 'PCIDSK'
        else:
            raise RSGISPyException('Type not recognised')
        
        return gdalStr
    
    def getRSGISLibDataTypeFromImg(self, inImg):
        """
        Returns the rsgislib datatype ENUM (e.g., rsgislib.TYPE_8INT) 
        for the inputted raster file

        :return: int

        """
        raster = gdal.Open(inImg, gdal.GA_ReadOnly)
        if raster == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        band = raster.GetRasterBand(1)
        if band == None:
            raise RSGISPyException('Could not open raster band 1 in image: \'' + inImg+ '\'')
        gdal_dtype = gdal.GetDataTypeName(band.DataType)
        raster = None
        return self.getRSGISLibDataType(gdal_dtype)
        
    def getGDALDataTypeFromImg(self, inImg):
        """
        Returns the GDAL datatype ENUM (e.g., GDT_Float32) for the inputted raster file.

        :return: ints

        """
        raster = gdal.Open(inImg, gdal.GA_ReadOnly)
        if raster == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        band = raster.GetRasterBand(1)
        if band == None:
            raise RSGISPyException('Could not open raster band 1 in image: \'' + inImg+ '\'')
        gdal_dtype = band.DataType
        raster = None
        return gdal_dtype
        
    def getGDALDataTypeNameFromImg(self, inImg):
        """
        Returns the GDAL datatype ENUM (e.g., GDT_Float32) for the inputted raster file.

        :return: int

        """
        raster = gdal.Open(inImg, gdal.GA_ReadOnly)
        if raster == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        band = raster.GetRasterBand(1)
        if band == None:
            raise RSGISPyException('Could not open raster band 1 in image: \'' + inImg+ '\'')
        dtypeName = gdal.GetDataTypeName(band.DataType)
        raster = None
        return dtypeName
    
    def deleteFileWithBasename(self, filePath):
        """
        Function to delete all the files which have a path
        and base name defined in the filePath attribute.

        """
        import glob
        baseName = os.path.splitext(filePath)[0]
        fileList = glob.glob(baseName+str('.*'))
        for file in fileList:
            print("Deleting file: " + str(file))
            os.remove(file)
                
    def deleteDIR(self, dirPath):
        """
        A function which will delete a directory, if files and other directories
        are within the path specified they will be recursively deleted as well.
        So be careful you don't delete things within meaning it.

        """
        for root, dirs, files in os.walk(dirPath, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
        os.rmdir(dirPath)
        print("Deleted " + dirPath)
        
    def renameGDALLayer(self, cFileName, oFileName):
        """
        Rename all the files associated with a GDAL layer.

        """
        layerDS = gdal.Open(cFileName, gdal.GA_ReadOnly)
        gdalDriver = layerDS.GetDriver()
        layerDS = None
        gdalDriver.Rename(oFileName, cFileName)

    def getRSGISLibDataType(self, gdaltype):
        """
        Convert from GDAL data type string to RSGISLib data type int.

        :return: int

        """
        gdaltype = gdaltype.lower()
        if gdaltype == 'int8':
            return TYPE_8INT
        elif gdaltype == 'int16':
            return TYPE_16INT
        elif gdaltype == 'int32':
            return TYPE_32INT
        elif gdaltype == 'int64':
            return TYPE_64INT
        elif gdaltype == 'byte' or gdaltype == 'uint8':
            return TYPE_8UINT
        elif gdaltype == 'uint16':
            return TYPE_16UINT
        elif gdaltype == 'uint32':
            return TYPE_32UINT
        elif gdaltype == 'uint64':
            return TYPE_64UINT
        elif gdaltype == 'float32':
            return TYPE_32FLOAT
        elif gdaltype == 'float64':
            return TYPE_64FLOAT
        else:
            raise RSGISPyException("The data type '" + str(gdaltype) + "' is unknown / not supported.")

    def getNumpyDataType(self, rsgislib_datatype):
        """
        Convert from RSGISLib data type to numpy datatype

        :param rsgis_datatype:
        :return:
        """
        import numpy
        numpyDT = numpy.float32
        if rsgislib_datatype == TYPE_8INT:
            numpyDT = numpy.int8
        elif rsgislib_datatype == TYPE_16INT:
            numpyDT = numpy.int16
        elif rsgislib_datatype == TYPE_32INT:
            numpyDT = numpy.int32
        elif rsgislib_datatype == TYPE_64INT:
            numpyDT = numpy.int64
        elif rsgislib_datatype == TYPE_8UINT:
            numpyDT = numpy.uint8
        elif rsgislib_datatype == TYPE_16UINT:
            numpyDT = numpy.uint16
        elif rsgislib_datatype == TYPE_32UINT:
            numpyDT = numpy.uint32
        elif rsgislib_datatype == TYPE_64UINT:
            numpyDT = numpy.uint64
        elif rsgislib_datatype == TYPE_32FLOAT:
            numpyDT = numpy.float32
        elif rsgislib_datatype == TYPE_64FLOAT:
            numpyDT = numpy.float64
        else:
            raise Exception('Datatype was not recognised.')

        return numpyDT
    
    def getImageRes(self, inImg):
        """
        A function to retrieve the image resolution.

        :return: xRes, yRes

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        geotransform = rasterDS.GetGeoTransform()
        xRes = geotransform[1]
        yRes = geotransform[5]
        if yRes < 0:
            yRes = yRes * -1
        rasterDS = None
        return xRes, yRes
    
    def doImageResMatch(self, img1, img2):
        """
        A function to test whether two images have the same
        image pixel resolution.

        :return: boolean

        """
        img1XRes, img1YRes = self.getImageRes(img1)
        img2XRes, img2YRes = self.getImageRes(img2)

        return ((img1XRes == img2XRes) and (img1YRes == img2YRes))
    
    def getImageSize(self, inImg):
        """
        A function to retrieve the image size in pixels.

        :return: xSize, ySize

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        xSize = rasterDS.RasterXSize
        ySize = rasterDS.RasterYSize
        rasterDS = None
        return xSize, ySize
        
    def getImageBBOX(self, inImg):
        """
        A function to retrieve the bounding box in the spatial 
        coordinates of the image.

        :return: (MinX, MaxX, MinY, MaxY)

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        xSize = rasterDS.RasterXSize
        ySize = rasterDS.RasterYSize
        
        geotransform = rasterDS.GetGeoTransform()
        tlX = geotransform[0]
        tlY = geotransform[3]
        xRes = geotransform[1]
        yRes = geotransform[5]
        if yRes < 0:
            yRes = yRes * -1
        rasterDS = None
        
        brX = tlX + (xRes * xSize)
        brY = tlY - (yRes * ySize)
        
        return [tlX, brX, brY, tlY]
    
    def getImageBBOXInProj(self, inImg, outEPSG):
        """
        A function to retrieve the bounding box in the spatial 
        coordinates of the image.

        :return: (MinX, MaxX, MinY, MaxY)

        """
        inProjWKT = self.getWKTProjFromImage(inImg)
        inSpatRef = osr.SpatialReference()
        inSpatRef.ImportFromWkt(inProjWKT)
        
        outSpatRef = osr.SpatialReference()
        outSpatRef.ImportFromEPSG(int(outEPSG))

        img_bbox = self.getImageBBOX(inImg)
        reproj_img_bbox = self.reprojBBOX(img_bbox, inSpatRef, outSpatRef)
        return reproj_img_bbox
        
    def reprojBBOX(self, bbox, inProjObj, outProjObj):
        """
        A function to reproject a bounding box.
        * bbox - input bounding box (MinX, MaxX, MinY, MaxY)
        * inProjObj - an osr.SpatialReference() object representing input projection.
        * outProjObj - an osr.SpatialReference() object representing output projection.

        :return: (MinX, MaxX, MinY, MaxY)

        """
        tlX = bbox[0]
        tlY = bbox[3]
        trX = bbox[1]
        trY = bbox[3]
        brX = bbox[1]
        brY = bbox[2]
        blX = bbox[0]
        blY = bbox[2]

        out_tlX, out_tlY = self.reprojPoint(inProjObj, outProjObj, tlX, tlY)
        out_trX, out_trY = self.reprojPoint(inProjObj, outProjObj, trX, trY)
        out_brX, out_brY = self.reprojPoint(inProjObj, outProjObj, brX, brY)
        out_blX, out_blY = self.reprojPoint(inProjObj, outProjObj, blX, blY)

        minX = out_tlX
        if out_blX < minX:
            minX = out_blX

        maxX = out_brX
        if out_trX > maxX:
            maxX = out_trX

        minY = out_brY
        if out_blY < minY:
            minY = out_blY

        maxY = out_tlY
        if out_trY > maxY:
            maxY = out_trY

        return [minX, maxX, minY, maxY]
        
    def getVecLayerExtent(self, inVec, layerName=None, computeIfExp=True):
        """
        Get the extent of the vector layer.
        
        * inVec - is a string with the input vector file name and path.
        * layerName - is the layer for which extent is to be calculated (Default: None)
        *             if None assume there is only one layer and that will be read.
        * computeIfExp - is a boolean which specifies whether the layer extent 
                         should be calculated (rather than estimated from header)
                         even if that operation is computationally expensive.
        
        :return: boundary box is returned (MinX, MaxX, MinY, MaxY)
        
        """
        inDataSource = gdal.OpenEx(inVec, gdal.OF_VECTOR )
        if layerName is not None:
            inLayer = inDataSource.GetLayer(layerName)
        else:
            inLayer = inDataSource.GetLayer()
        extent = inLayer.GetExtent(computeIfExp)
        return extent
        
    def getVecFeatCount(self, inVec, layerName=None, computeCount=True):
        """
        Get a count of the number of features in the vector layers.
        
        * inVec - is a string with the input vector file name and path.
        * layerName - is the layer for which extent is to be calculated (Default: None)
        *             if None assume there is only one layer and that will be read.
        * computeCount - is a boolean which specifies whether the layer extent 
                         should be calculated (rather than estimated from header)
                         even if that operation is computationally expensive.
        
        :return: nfeats
        
        """
        inDataSource = gdal.OpenEx(inVec, gdal.OF_VECTOR )
        if layerName is not None:
            inLayer = inDataSource.GetLayer(layerName)
        else:
            inLayer = inDataSource.GetLayer()
        nFeats = inLayer.GetFeatureCount(computeCount)
        return nFeats
        
        
    def findCommonExtentOnGrid(self, baseExtent, baseGrid, otherExtent, fullContain=True):
        """
        A function which calculates the common extent between two extents but defines output on 
        grid with defined resolutions. Useful for finding common extent on a particular image grid.
        
        * baseExtent is a bbox (xMin, xMax, yMin, yMax) providing the base for the grid on which output will be defined.
        * baseGrid the size of the (square) grid on which output will be defined.
        * otherExtent is a bbox (xMin, xMax, yMin, yMax) to be intersected with the baseExtent.
        * fullContain is a boolean. True: moving output onto grid will increase size of bbox (i.e., intersection fully contained)
                                    False: move output onto grid will decrease size of bbox (i.e., bbox fully contained within intesection)
        
        :return: bbox (xMin, xMax, yMin, yMax)

        """
        xMinOverlap = baseExtent[0]
        xMaxOverlap = baseExtent[1]
        yMinOverlap = baseExtent[2]
        yMaxOverlap = baseExtent[3]
        
        if otherExtent[0] > xMinOverlap:
            if fullContain:
                diff = math.floor((otherExtent[0] - xMinOverlap)/baseGrid)*baseGrid
            else:   
                diff = math.ceil((otherExtent[0] - xMinOverlap)/baseGrid)*baseGrid
            xMinOverlap = xMinOverlap + diff
        
        if otherExtent[1] < xMaxOverlap:
            if fullContain:
                diff = math.floor((xMaxOverlap - otherExtent[1])/baseGrid)*baseGrid
            else:
                diff = math.ceil((xMaxOverlap - otherExtent[1])/baseGrid)*baseGrid
            xMaxOverlap = xMaxOverlap - diff
        
        if otherExtent[2] > yMinOverlap:
            if fullContain:
                diff = math.floor(abs(otherExtent[2] - yMinOverlap)/baseGrid)*baseGrid
            else:
                diff = math.ceil(abs(otherExtent[2] - yMinOverlap)/baseGrid)*baseGrid
            yMinOverlap = yMinOverlap + diff
        
        if otherExtent[3] < yMaxOverlap:
            if fullContain:
                diff = math.floor(abs(yMaxOverlap - otherExtent[3])/baseGrid)*baseGrid
            else:
                diff = math.ceil(abs(yMaxOverlap - otherExtent[3])/baseGrid)*baseGrid
            yMaxOverlap = yMaxOverlap - diff
    
        return [xMinOverlap, xMaxOverlap, yMinOverlap, yMaxOverlap]
    
    def findExtentOnGrid(self, baseExtent, baseGrid, fullContain=True):
        """
        A function which calculates the extent but defined on a grid with defined resolution. 
        Useful for finding extent on a particular image grid.
        
        * baseExtent is a bbox (xMin, xMax, yMin, yMax) providing the base for the grid on which output will be defined.
        * baseGrid the size of the (square) grid on which output will be defined.
        * fullContain is a boolean. True: moving output onto grid will increase size of bbox (i.e., intersection fully contained)
                                    False: move output onto grid will decrease size of bbox (i.e., bbox fully contained within intesection)
        
        :return: bbox (xMin, xMax, yMin, yMax)

        """
        xMin = baseExtent[0]
        xMax = baseExtent[1]
        yMin = baseExtent[2]
        yMax = baseExtent[3]
        
        diffX = xMax - xMin
        diffY = abs(yMax - yMin)
        
        nPxlX = 0.0
        nPxlY = 0.0
        if fullContain:
            nPxlX = math.ceil(diffX/baseGrid)
            nPxlY = math.ceil(diffY/baseGrid)
        else:
            nPxlX = math.floor(diffX/baseGrid)
            nPxlY = math.floor(diffY/baseGrid)
        
        xMaxOut = xMin + (nPxlX * baseGrid)
        yMinOut = yMax - (nPxlY * baseGrid)
    
        return [xMin, xMaxOut, yMinOut, yMax]

    def findExtentOnWholeNumGrid(self, baseExtent, baseGrid, fullContain=True, round_vals=None):
        """
        A function which calculates the extent but defined on a grid with defined resolution.
        Useful for finding extent on a particular image grid.

        * baseExtent is a bbox (xMin, xMax, yMin, yMax) providing the base for the grid on which output will be defined.
        * baseGrid the size of the (square) grid on which output will be defined.
        * fullContain is a boolean. True: moving output onto grid will increase size of bbox (i.e., intersection fully contained)
                                    False: move output onto grid will decrease size of bbox (i.e., bbox fully contained within intesection)
        * round_vals specify whether outputted values should be rounded. None for no rounding (default) or integer for number of
                     significant figures to round to.

        :return: bbox (xMin, xMax, yMin, yMax)

        """
        xMin = baseExtent[0]
        xMax = baseExtent[1]
        yMin = baseExtent[2]
        yMax = baseExtent[3]

        nPxlXMin = math.floor(xMin / baseGrid)
        nPxlYMin = math.floor(yMin / baseGrid)

        xMinOut = nPxlXMin * baseGrid
        yMinOut = nPxlYMin * baseGrid

        diffX = xMax - xMinOut
        diffY = abs(yMax - yMinOut)

        nPxlX = 0.0
        nPxlY = 0.0
        if fullContain:
            nPxlX = math.ceil(diffX / baseGrid)
            nPxlY = math.ceil(diffY / baseGrid)
        else:
            nPxlX = math.floor(diffX / baseGrid)
            nPxlY = math.floor(diffY / baseGrid)

        xMaxOut = xMinOut + (nPxlX * baseGrid)
        yMaxOut = yMinOut + (nPxlY * baseGrid)

        if round_vals is None:
            out_bbox = [xMinOut, xMaxOut, yMinOut, yMaxOut]
        else:
            out_bbox = [round(xMinOut, round_vals), round(xMaxOut, round_vals), round(yMinOut, round_vals),
                        round(yMaxOut, round_vals)]
        return out_bbox

    def getBBoxGrid(self, bbox, x_size, y_size):
        """
        Create a grid with size x_size, y_size for the area represented by bbox.

        * bbox - a bounding box within which the grid will be created (xMin, xMax, yMin, yMax)
        * x_size - Output grid size in X axis (same unit as bbox).
        * y_size - Output grid size in Y axis (same unit as bbox).

        :return: list of bounding boxes (xMin, xMax, yMin, yMax)

        """
        width = bbox[1] - bbox[0]
        height = bbox[3] - bbox[2]

        n_tiles_x = math.floor(width / x_size)
        n_tiles_y = math.floor(height / y_size)

        if (n_tiles_x > 10000) or (n_tiles_y > 10000):
            print("WARNING: did you mean to product so many tiles (X: {}, Y: {}) "
                  "might want to check your units".format(n_tiles_x, n_tiles_y))

        full_tile_width = n_tiles_x * x_size
        full_tile_height = n_tiles_y * y_size

        x_remain = width - full_tile_width
        if x_remain < 0.000001:
            x_remain = 0.0
        y_remain = height - full_tile_height
        if y_remain < 0.000001:
            y_remain = 0.0

        c_min_y = bbox[2]
        c_max_y = c_min_y + y_size

        bboxs = list()
        for ny in range(n_tiles_y):
            c_min_x = bbox[0]
            c_max_x = c_min_x + x_size
            for nx in range(n_tiles_x):
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
                c_min_x = c_max_x
                c_max_x = c_max_x + x_size
            if x_remain > 0:
                c_max_x = c_min_x + x_remain
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            c_min_y = c_max_y
            c_max_y = c_max_y + y_size
        if y_remain > 0:
            c_max_y = c_min_y + y_remain
            c_min_x = bbox[0]
            c_max_x = c_min_x + x_size
            for nx in range(n_tiles_x):
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
                c_min_x = c_max_x
                c_max_x = c_max_x + x_size
            if x_remain > 0:
                c_max_x = c_min_x + x_remain
                bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])

        return bboxs

    def reprojPoint(self, inProjOSRObj, outProjOSRObj, x, y):
        """
        Reproject a point from 'inProjOSRObj' to 'outProjOSRObj' where they are gdal
        osgeo.osr.SpatialReference objects. 
        
        :return: x, y. (note if returning long, lat you might need to invert)

        """
        wktPt = 'POINT(%s %s)' % (x, y)
        point = ogr.CreateGeometryFromWkt(wktPt)
        point.AssignSpatialReference(inProjOSRObj)
        point.TransformTo(outProjOSRObj)

        if int(gdal.VersionInfo()) >= 3000000:
            # With GDAL > 3.0 see RFC73.
            outProjOSRObj.SetAxisMappingStrategy(osr.OAMS_TRADITIONAL_GIS_ORDER)
            axis_idx = outProjOSRObj.GetDataAxisToSRSAxisMapping()
            # Get the X Value.
            if axis_idx[0] == 1:
                outX = point.GetX()
            else:
                outX = point.GetY()

            # Get the Y Value.
            if axis_idx[1] == 2:
                outY = point.GetY()
            else:
                outY = point.GetX()
        else:
            # With GDAL versions below 3.X.
            if outProjOSRObj.EPSGTreatsAsLatLong():
                outX = point.GetY()
                outY = point.GetX()
            else:
                outX = point.GetX()
                outY = point.GetY()
        return outX, outY

    def getImageBandStats(self, img, band, compute=True):
        """
        A function which calls the GDAL function on the band selected to calculate the pixel stats
        (min, max, mean, standard deviation). 
        
        * img - input image file path
        * band - specified image band for which stats are to be calculated (starts at 1). 
        * compute - whether the stats should be calculated (True; Default) or an approximation or pre-calculated stats are OK (False).
        
        :return: stats (min, max, mean, stddev)

        """
        img_ds = gdal.Open(img, gdal.GA_ReadOnly)
        if img_ds is None:
            raise Exception("Could not open image: '{}'".format(img))
        n_bands = img_ds.RasterCount
        
        if band > 0 and band <= n_bands:
            img_band = img_ds.GetRasterBand(band)
            if img_band is None:
                raise Exception("Could not open image band ('{0}') from : '{1}'".format(band, img))
            img_stats = img_band.ComputeStatistics((not compute))
        else:
            raise Exception("Band specified is not within the image: '{}'".format(img))
        return img_stats
    
    
    def getImageBandCount(self, inImg):
        """
        A function to retrieve the number of image bands in an image file.

        :return: nBands

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        nBands = rasterDS.RasterCount
        rasterDS = None
        return nBands
        
    def getImageNoDataValue(self, inImg, band=1):
        """
        A function to retrieve the no data value for the image 
        (from band; default 1).

        :return: number

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        noDataVal = rasterDS.GetRasterBand(band).GetNoDataValue()
        rasterDS = None
        return noDataVal

    def setImageNoDataValue(self, inImg, noDataValue, band=None):
        """
        A function to set the no data value for an image.
        If band is not specified sets value for all bands.

        """
        rasterDS = gdal.Open(inImg, gdal.GA_Update)
        if rasterDS is None:
            raise RSGISPyException('Could not open raster image: \'' + inImg + '\'')

        if band is not None:
            rasterDS.GetRasterBand(band).SetNoDataValue(noDataValue)
        else:
            for b in range(rasterDS.RasterCount):
                rasterDS.GetRasterBand(b+1).SetNoDataValue(noDataValue)

        rasterDS = None
    
    def getImgBandColourInterp(self, inImg, band):
        """
        A function to get the colour interpretation for a specific band.

        :return: is a GDALColorInterp value:
        
        * GCI_Undefined=0, 
        * GCI_GrayIndex=1, 
        * GCI_PaletteIndex=2, 
        * GCI_RedBand=3, 
        * GCI_GreenBand=4, 
        * GCI_BlueBand=5, 
        * GCI_AlphaBand=6, 
        * GCI_HueBand=7, 
        * GCI_SaturationBand=8, 
        * GCI_LightnessBand=9, 
        * GCI_CyanBand=10, 
        * GCI_MagentaBand=11, 
        * GCI_YellowBand=12, 
        * GCI_BlackBand=13, 
        * GCI_YCbCr_YBand=14, 
        * GCI_YCbCr_CbBand=15, 
        * GCI_YCbCr_CrBand=16, 
        * GCI_Max=16 
         
        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS is None:
            raise RSGISPyException('Could not open raster image: \'' + inImg + '\'')
        clrItrpVal = rasterDS.GetRasterBand(band).GetRasterColorInterpretation()
        rasterDS = None
        return clrItrpVal
        
    def setImgBandColourInterp(self, inImg, band, clrItrpVal):
        """
        A function to set the colour interpretation for a specific band.
        input is a GDALColorInterp value:
        
        * GCI_Undefined=0, 
        * GCI_GrayIndex=1, 
        * GCI_PaletteIndex=2, 
        * GCI_RedBand=3, 
        * GCI_GreenBand=4, 
        * GCI_BlueBand=5, 
        * GCI_AlphaBand=6, 
        * GCI_HueBand=7, 
        * GCI_SaturationBand=8, 
        * GCI_LightnessBand=9, 
        * GCI_CyanBand=10, 
        * GCI_MagentaBand=11, 
        * GCI_YellowBand=12, 
        * GCI_BlackBand=13, 
        * GCI_YCbCr_YBand=14, 
        * GCI_YCbCr_CbBand=15, 
        * GCI_YCbCr_CrBand=16, 
        * GCI_Max=16 
         
        """
        rasterDS = gdal.Open(inImg, gdal.GA_Update)
        if rasterDS is None:
            raise RSGISPyException('Could not open raster image: \'' + inImg + '\'')
        rasterDS.GetRasterBand(band).SetColorInterpretation(clrItrpVal)
        rasterDS = None
    
    def getWKTProjFromImage(self, inImg):
        """
        A function which returns the WKT string representing the projection 
        of the input image.

        :return: string

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        projStr = rasterDS.GetProjection()
        rasterDS = None
        return projStr
    
    def getImageFiles(self, inImg):
        """
        A function which returns a list of the files associated (e.g., header etc.) 
        with the input image file.

        :return: lists

        """
        imgDS = gdal.Open(inImg)
        fileList = imgDS.GetFileList()
        imgDS = None
        return fileList
    
    def getUTMZone(self, inImg):
        """
        A function which returns a string with the UTM (XXN | XXS) zone of the input image 
        but only if it is projected within the UTM projection/coordinate system.

        :return: string

        """
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        projStr = rasterDS.GetProjection()
        rasterDS = None
    
        spatRef = osr.SpatialReference()
        spatRef.ImportFromWkt(projStr)
        utmZone = None
        if spatRef.IsProjected():
            projName = spatRef.GetAttrValue('projcs')
            zone = spatRef.GetUTMZone()
            if zone != 0:
                if zone < 0:
                    utmZone = str(zone*(-1))
                    if len(utmZone) == 1:
                        utmZone = '0' + utmZone
                    utmZone = utmZone+'S'
                else:
                    utmZone = str(zone)
                    if len(utmZone) == 1:
                        utmZone = '0' + utmZone
                    utmZone = utmZone+'N'
        return utmZone
    
    def getEPSGCode(self, gdalLayer):
        """
        Using GDAL to return the EPSG code for the input layer.

        :return: EPSG code

        """
        epsgCode = None
        try:
            layerDS = gdal.Open(gdalLayer, gdal.GA_ReadOnly)
            if layerDS == None:
                raise RSGISPyException('Could not open raster image: \'' + gdalLayer+ '\'')
            projStr = layerDS.GetProjection()
            layerDS = None
            
            spatRef = osr.SpatialReference()
            spatRef.ImportFromWkt(projStr)            
            spatRef.AutoIdentifyEPSG()
            epsgCode = spatRef.GetAuthorityCode(None)
        except Exception:
            epsgCode = None
        return epsgCode
        
    def doGDALLayersHaveSameProj(self, layer1, layer2):
        """
        A function which tests whether two gdal compatiable layers are in the same
        projection/coordinate system. This is done using the GDAL SpatialReference
        function AutoIdentifyEPSG. If the identified EPSG codes are different then 
        False is returned otherwise True.

        :return: boolean

        """
        layer1EPSG = self.getEPSGCode(layer1)
        layer2EPSG = self.getEPSGCode(layer2)
        
        sameEPSG = False
        if layer1EPSG == layer2EPSG:
            sameEPSG = True
        
        return sameEPSG
        
    def getProjWKTFromVec(self, inVec, vecLyr=None):
        """
        A function which gets the WKT projection from the inputted vector file.
        
        * inVec - is a string with the input vector file name and path.
        * vecLyr - is a string with the input vector layer name, if None then first layer read. (default: None)
        
        :return: WKT representation of projection

        """
        dataset = gdal.OpenEx(inVec, gdal.OF_VECTOR )
        if dataset is None:
            raise Exception("Could not open file: {}".format(inVec))
        if vecLyr is None:
            layer = dataset.GetLayer()
        else:
            layer = dataset.GetLayer(vecLyr)
        if layer is None:
            raise Exception("Could not open layer within file: {}".format(inVec))
        spatialRef = layer.GetSpatialRef()
        return spatialRef.ExportToWkt()

    def getProjEPSGFromVec(self, inVec, vecLyr=None):
        """
        A function which gets the EPSG projection from the inputted vector file.

        * inVec - is a string with the input vector file name and path.
        * vecLyr - is a string with the input vector layer name, if None then first layer read. (default: None)

        :return: EPSG representation of projection

        """
        dataset = gdal.OpenEx(inVec, gdal.OF_VECTOR)
        if dataset is None:
            raise Exception("Could not open file: {}".format(inVec))
        if vecLyr is None:
            layer = dataset.GetLayer()
        else:
            layer = dataset.GetLayer(vecLyr)
        if layer is None:
            raise Exception("Could not open layer within file: {}".format(inVec))
        spatialRef = layer.GetSpatialRef()
        spatialRef.AutoIdentifyEPSG()
        return spatialRef.GetAuthorityCode(None)
        
    def getEPSGCodeFromWKT(self, wktString):
        """
        Using GDAL to return the EPSG code for inputted WKT string.

        :return: the EPSG code.

        """
        epsgCode = None
        try:        
            spatRef = osr.SpatialReference()
            spatRef.ImportFromWkt(wktString)            
            spatRef.AutoIdentifyEPSG()
            epsgCode = spatRef.GetAuthorityCode(None)
        except Exception:
            epsgCode = None
        return epsgCode
        
    def getWKTFromEPSGCode(self, epsgCode):
        """
        Using GDAL to return the WKT string for inputted EPSG Code.
        
        * epsgCode integer variable of the epsg code.

        :return: string with WKT representation of the projection.

        """
        wktString = None
        try:        
            spatRef = osr.SpatialReference()
            spatRef.ImportFromEPSG(epsgCode)            
            wktString = spatRef.ExportToWkt()
        except Exception:
            wktString = None
        return wktString
    
    def uidGenerator(self, size=6):
        """
        A function which will generate a 'random' string of the specified length based on the UUID

        :return: string of length size.

        """
        import uuid
        randomStr = str(uuid.uuid4())
        randomStr = randomStr.replace("-","")
        return randomStr[0:size]
    
    def isNumber(self, strVal):
        """
        A function which tests whether the input string contains a number of not.

        :return: boolean

        """
        try:
            float(strVal) # for int, long and float
        except ValueError:
            try:
                complex(strVal) # for complex
            except ValueError:
                return False
        return True
    
    def getEnvironmentVariable(self, var):
        """
        A function to get an environmental variable, if variable is not present returns None.

        :return: value of env var.

        """
        outVar = None
        try:
            outVar = os.environ[var]
        except Exception:
            outVar = None
        return outVar
    
    def numProcessCores(self):
        """
        A functions which returns the number of processing cores available on the machine

        :return: int

        """
        import multiprocessing
        return multiprocessing.cpu_count()
        
    def readTextFileNoNewLines(self, file):
        """
        Read a text file into a single string
        removing new lines.

        :return: string

        """
        txtStr = ""
        try:
            dataFile = open(file, 'r')
            for line in dataFile:
                txtStr += line.strip()
            dataFile.close()
        except Exception as e:
            raise e
        return txtStr

    def readTextFile2List(self, file):
        """
        Read a text file into a list where each line 
        is an element in the list.

        :return: list

        """
        outList = []
        try:
            dataFile = open(file, 'r')
            for line in dataFile:
                line = line.strip()
                if line != "":
                    outList.append(line)
            dataFile.close()
        except Exception as e:
            raise e
        return outList

    def writeList2File(self, dataList, outFile):
        """
        Write a list a text file, one line per item.

        """
        try:
            f = open(outFile, 'w')
            for item in dataList:
               f.write(str(item)+'\n')
            f.flush()
            f.close()
        except Exception as e:
            raise e
    
    def findFile(self, dirPath, fileSearch):
        """
        Search for a single file with a path using glob. Therefore, the file 
        path returned is a true path. Within the fileSearch provide the file
        name with '*' as wildcard(s).

        :return: string

        """
        import glob
        files = glob.glob(os.path.join(dirPath, fileSearch))
        if len(files) != 1:
            raise RSGISPyException('Could not find a single file ('+fileSearch+'); found ' + str(len(files)) + ' files.')
        return files[0]

    def findFileNone(self, dirPath, fileSearch):
        """
        Search for a single file with a path using glob. Therefore, the file
        path returned is a true path. Within the fileSearch provide the file
        name with '*' as wildcard(s). Returns None is not found.

        :return: string

        """
        import glob
        import os.path
        files = glob.glob(os.path.join(dirPath, fileSearch))
        if len(files) != 1:
            return None
        return files[0]

    def createVarList(self, in_vals_lsts, val_dict=None):
        """
        A function which will produce a list of dictionaries with all the combinations 
        of the input variables listed (i.e., the powerset). 
        
        :param in_vals_lsts - dictionary with each value having a list of values.
        :param val_dict - variable used in iterative nature of function which lists
                   the variable for which are still to be looped through. Would 
                   normally not be provided by the user as default is None. Be
                   careful if you set as otherwise.

        :returns: list of dictionaries with the same keys are the input but only a
                 single value will be associate with key rather than a list.
                   
        Example::
        
        seg_vars_ranges = dict()
        seg_vars_ranges['k'] = [5, 10, 20, 30, 40, 50, 60, 80, 100, 120]
        seg_vars_ranges['d'] = [10, 20, 50, 100, 200, 1000, 10000]
        seg_vars_ranges['minsize'] = [5, 10, 20, 50, 100, 200]
        
        seg_vars = rsgis_utils.createVarList(seg_vars_ranges)
        
        """
        out_vars = []
        if (in_vals_lsts is None) and (val_dict is not None):
            out_val_dict = dict()
            for key in val_dict.keys():
                out_val_dict[key] = val_dict[key]
            out_vars.append(out_val_dict)
        elif in_vals_lsts is not None:
            if len(in_vals_lsts.keys()) > 0:
                key = list(in_vals_lsts.keys())[0]
                vals_arr = in_vals_lsts[key]
                next_vals_lsts = dict()
                for ckey in in_vals_lsts.keys():
                    if ckey != key:
                        next_vals_lsts[ckey] = in_vals_lsts[ckey]
                        
                if len(next_vals_lsts.keys()) == 0:
                    next_vals_lsts = None
                
                if val_dict is None:
                    val_dict = dict()
                
                for val in vals_arr:
                    c_val_dict = dict()
                    for ckey in val_dict.keys():
                        c_val_dict[ckey] = val_dict[ckey]
                    c_val_dict[key] = val
                    c_out_vars = self.createVarList(next_vals_lsts, c_val_dict)
                    out_vars = out_vars+c_out_vars
        return out_vars

    def in_bounds(self, x, lower, upper, upper_strict=False):
        """
        Checks whether a value is within specified bounds.

        :param x: value or array of values to check.
        :param lower: lower bound
        :param upper: upper bound
        :param upper_strict: True is less than upper; False is less than equal to upper

        :return:

        """
        import numpy
        if upper_strict:
            return lower <= numpy.min(x) and numpy.max(x) < upper
        else:
            return lower <= numpy.min(x) and numpy.max(x) <= upper

    def mixed_signs(self, x):
        """
        Check whether a list of numbers has a mix of postive and negative values.

        :param x: list of values.

        :return: boolean

        """
        import numpy
        return numpy.min(x) < 0 and numpy.max(x) >= 0

    def negative(self, x):
        """
        Is the maximum number in the list negative.
        :param x: list of values

        :return: boolean

        """
        import numpy
        return numpy.max(x) < 0


class RSGISTime (object):
    """ Class to calculate run time for a function, format and print out (similar to for XML interface).

        Need to call start before running function and end immediately after.
        Example::

            t = RSGISTime()
            t.start()
            rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalformat, False, 0) 
            t.end()
        
        Note, this is only designed to provide some general feedback, for benchmarking the timeit module
        is better suited.

    """

    def __init__(self):
        self.startTime = time.time()
        self.endTime = time.time()

    def start(self, printStartTime=False):
        """
        Start timer, optionally printing start time

        :param printStartTime: A boolean specifiying whether the start time should be printed to console.

        """
        self.startTime = time.time()
        if printStartTime:
            print(time.strftime('Start Time: %H:%M:%S, %a %b %m %Y.'))

    def end(self, reportDiff=True, preceedStr="", postStr=""):
        """ 
        End timer and optionally print difference.
        If preceedStr or postStr have a value then they will be used instead
        of the generic wording around the time. 
        
        preceedStr + time + postStr

        :param reportDiff: A boolean specifiying whether time difference should be printed to console.
        :param preceedStr: A string which is printed ahead of time difference
        :param postStr: A string which is printed after the time difference

        """
        self.endTime = time.time()
        if reportDiff:
            self.calcDiff(preceedStr, postStr)

    def calcDiff(self, preceedStr="", postStr=""):
        """
        Calculate time difference, format and print.
        :param preceedStr: A string which is printed ahead of time difference
        :param postStr: A string which is printed after the time difference

        """
        timeDiff = self.endTime - self.startTime
        
        useCustomMss = False
        if (len(preceedStr) > 0) or (len(postStr) > 0):
            useCustomMss = True
        
        if timeDiff <= 1:
            if useCustomMss:
                outStr = preceedStr + str("in less than a second") + postStr
                print(outStr)
            else:
                print("Algorithm Completed in less than a second.")
        else:
            timeObj = datetime.datetime.utcfromtimestamp(timeDiff)
            timeDiffStr = timeObj.strftime('%H:%M:%S')
            if useCustomMss:
                print(preceedStr + timeDiffStr + postStr)
            else:
                print('Algorithm Completed in %s.'%(timeDiffStr))
        

