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

"""
import os.path
import os
import time
import datetime

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

def getRSGISLibVersion():
    """ Calls rsgis-config to get the version number. """

    # Try calling rsgis-config to get minor version number
    try:
        import subprocess
        out = subprocess.Popen('rsgis-config --version',shell=True,stdin=subprocess.PIPE, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
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
        """ Get GDAL format, based on filename """
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
        Returns the rsgislib datatype ENUM (e.g., rsgislib.TYPE_8INT) for the inputted raster file
        """
        import osgeo.gdal as gdal
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
        Returns the rsgislib datatype ENUM for a raster file
        :param in_file: The file to get the datatype for
        :return: The rsgislib datatype enum, e.g., rsgislib.TYPE_8INT
        """
        import osgeo.gdal as gdal
        raster = gdal.Open(inImg, gdal.GA_ReadOnly)
        if raster == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\''))
        band = raster.GetRasterBand(1)
        if band == None:
            raise RSGISPyException('Could not open raster band 1 in image: \'' + inImg+ '\'')
        gdal_dtype = gdal.GetDataTypeName(band.DataType)
        raster = None
        return gdal_dtype
    
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
        So becareful you don't delete things within meaning it.
        """
        for root, dirs, files in os.walk(dirPath, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
        os.rmdir(dirPath)
        print("Deleted " + dirPath)

    def getRSGISLibDataType(self, gdaltype):
        """ Convert from GDAL data type string to 
            RSGISLib data type int.
        """
        gdaltype = gdaltype.lower()
        if gdaltype == 'byte' or gdaltype == 'int8':
            return TYPE_8INT
        elif gdaltype == 'int16':
            return TYPE_16INT
        elif gdaltype == 'int32':
            return TYPE_32INT
        elif gdaltype == 'int64':
            return TYPE_64INT
        elif gdaltype == 'uint8':
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
    
    def getImageRes(self, inImg):
        """
        A function to retrieve the image resolution.
        return xRes, yRes
        """
        import osgeo.gdal as gdal
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
    
    def getImageSize(self, inImg):
        """
        A function to retrieve the image size in pixels.
        return xSize, ySize
        """
        import osgeo.gdal as gdal
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        xSize = rasterDS.RasterXSize
        ySize = rasterDS.RasterYSize
        rasterDS = None
        return xSize, ySize
    
    def getImageBandCount(self, inImg):
        """
        A function to retrieve the number of image bands in an image file.
        return nBands
        """
        import osgeo.gdal as gdal
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        nBands = rasterDS.RasterCount
        rasterDS = None
        return nBands
        
    def getImageNoDataValue(self, inImg, band=1):
        """
        A function to retrieve the no data value for the image (from band; default 1).
        """
        import osgeo.gdal as gdal
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        
        noDataVal = rasterDS.GetRasterBand(band).GetNoDataValue()
        rasterDS = None
        return noDataVal
    
    def getWKTProjFromImage(self, inImg):
        """
        A function which returns the WKT string representing the projection of the input image.
        """
        import osgeo.gdal as gdal
        rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
        if rasterDS == None:
            raise RSGISPyException('Could not open raster image: \'' + inImg+ '\'')
        projStr = rasterDS.GetProjection()
        rasterDS = None
        return projStr
    
    def uidGenerator(self, size=6):
        """
        A function which will generate a 'random' string of the specified length based on the UUID
        """
        import uuid
        randomStr = str(uuid.uuid4())
        randomStr = randomStr.replace("-","")
        return randomStr[0:size]
    
    def isNumber(self, strVal):
        """
        A function which tests whether the input string contains a number of not.
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
        """
        import multiprocessing
        return multiprocessing.cpu_count()
        
    def readTextFileNoNewLines(self, file):
        """
        Read a text file into a single string
        removing new lines.
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


class RSGISTime (object):
    """ Class to calculate run time for a function, format and print out (similar to for XML interface).

        Need to call start before running function and end immediately after.
        Example::

            t = RSGISTime()
            t.start()
            rsgislib.segmentation.clump(kMeansFileZonesNoSgls, initClumpsFile, gdalFormat, False, 0) 
            t.end()
        
        Note, this is only designed to provide some general feedback, for benchmarking the timeit module
        is better suited."""

    def __init__(self):
        self.startTime = time.time()
        self.endTime = time.time()

    def start(self, printStartTime=False):
        """ Start timer, optionally printing start time"""
        self.startTime = time.time()
        if printStartTime:
            print(time.strftime('Start Time: %H:%M:%S, %a %b %m %Y.'))

    def end(self,reportDiff = True, preceedStr="", postStr=""):
        """ 
        End timer and optionally print difference.
        If preceedStr or postStr have a value then they will be used instead
        of the generic wording around the time. 
        
        preceedStr + time + postStr
        """
        self.endTime = time.time()
        if reportDiff:
            self.calcDiff(preceedStr, postStr)

    def calcDiff(self, preceedStr="", postStr=""):
        """ Calculate time difference, format and print. """
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
        

