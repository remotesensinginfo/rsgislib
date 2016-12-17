"""
The imageutils module contains general utilities for applying to images.
"""

# import the C++ extension into this level
from ._imageutils import *
import rsgislib 

import os.path
import math

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

haveOSRPy = True
try:
    import osgeo.osr as osr
except ImportError as osrErr:
    haveOSRPy = False

# define our own classes
class ImageBandInfo(object):
    """
    Create a list of these objects to pass to the extractZoneImageBandValues2HDF function
    * fileName - is the input image file name and path.
    * bands - is a list of image bands within the fileName to be used for processing (band numbers start at 1).
    """
    def __init__(self, fileName=None, name=None, bands=None):
        self.fileName = fileName
        self.name = name
        self.bands = bands


def setBandNames(inputImage, bandNames):
    """A utility function to set band names.
Where:

* inImage is the input image
* bandNames is a list of band names

Example::

    from rsgislib import imageutils

    inputImage = 'injune_p142_casi_sub_utm.kea'
    bandNames = ['446nm','530nm','549nm','569nm','598nm','633nm','680nm','696nm','714nm','732nm','741nm','752nm','800nm','838nm']
    
    imageutils.setBandNames(inputImage, bandNames)
    

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported" + gdalErr)

    print('Set band names.')
 
    dataset = gdal.Open(inputImage, gdal.GA_Update)
    
    for i in range(len(bandNames)):
        band = i+1
        bandName = bandNames[i]

        imgBand = dataset.GetRasterBand(band)
        # Check the image band is available
        if not imgBand is None:
            print('Setting Band {0} to "{1}"'.format(band, bandName))
            imgBand.SetDescription(bandName)
        else:
            raise Exception("Could not open the image band: ", band)

def getBandNames(inputImage):
    """A utility function to get band names.
Where:

* inImage is the input image

Return: 
    list of band names

Example::

    from rsgislib import imageutils

    inputImage = 'injune_p142_casi_sub_utm.kea'
    
    bandNames = imageutils.getBandNames(inputImage)
    

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported" + gdalErr)
 
    dataset = gdal.Open(inputImage, gdal.GA_Update)
    bandNames = list()
    
    for i in range(dataset.RasterCount):
        imgBand = dataset.GetRasterBand(i+1)
        # Check the image band is available
        if not imgBand is None:
            bandNames.append(imgBand.GetDescription())
        else:
            raise Exception("Could not open the image band: ", band)
    return bandNames

def getRSGISLibDataType(inImg):
    """
Returns the rsgislib datatype ENUM for a raster file

* inImg -- The file to get the datatype for

return::

    The rsgislib datatype enum, e.g., rsgislib.TYPE_8INT

"""
    from osgeo import gdal
    raster = gdal.Open(inImg, gdal.GA_ReadOnly)
    band = raster.GetRasterBand(1)
    gdal_dtype = gdal.GetDataTypeName(band.DataType)
    raster = None
    rsgis_utils = rsgislib.RSGISPyUtils()

    return rsgis_utils.getRSGISLibDataType(gdal_dtype)
    
def getGDALDataType(inImg):
    """
Returns the rsgislib datatype ENUM for a raster file

*  inImg -- The file to get the datatype for

return::

    The rsgislib datatype enum, e.g., rsgislib.TYPE_8INT
"""
    from osgeo import gdal
    raster = gdal.Open(inImg, gdal.GA_ReadOnly)
    band = raster.GetRasterBand(1)
    gdal_dtype = gdal.GetDataTypeName(band.DataType)
    raster = None
    return gdal_dtype

def resampleImage2Match(inRefImg, inProcessImg, outImg, format, interpMethod, datatype=None):
    """A utility function to resample an existing image to the projection
and/or pixel size of another image.
Where:

* inRefImg is the input reference image to which the processing image is to resampled to.
* inProcessImg is the image which is to be resampled.
* outImg is the output image file.
* format is the gdal format for the output image.
* interpMethod is the interpolation method used to resample the image [bilinear, lanczos, cubicspline, nearestneighbour, cubic, average, mode]
* datatype is the rsgislib datatype of the output image (if none then it will be the same as the input file).

"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported" + gdalErr)
    
    dataset = gdal.Open(inProcessImg, gdal.GA_ReadOnly)
    numBands = dataset.RasterCount
    gdalDType = dataset.GetRasterBand(1).DataType
    dataset = None
    if datatype == None:
        if gdalDType == gdal.GDT_Byte:
            datatype = rsgislib.TYPE_8UINT
        elif gdalDType == gdal.GDT_Int16:
            datatype = rsgislib.TYPE_16INT
        elif gdalDType == gdal.GDT_Int32:
            datatype = rsgislib.TYPE_32INT
        elif gdalDType == gdal.GDT_UInt16:
            datatype = rsgislib.TYPE_16UINT
        elif gdalDType == gdal.GDT_UInt32:
            datatype = rsgislib.TYPE_32UINT         
        elif gdalDType == gdal.GDT_Float32:
            datatype = rsgislib.TYPE_32FLOAT
        elif gdalDType == gdal.GDT_Float64:
            datatype = rsgislib.TYPE_64FLOAT
        else:
            raise Exception("Data type of the input file was not recognised or known.")
    interpolationMethod = gdal.GRA_NearestNeighbour
    if interpMethod == 'bilinear':
        interpolationMethod = gdal.GRA_Bilinear 
    elif interpMethod == 'lanczos':
        interpolationMethod = gdal.GRA_Lanczos 
    elif interpMethod == 'cubicspline':
        interpolationMethod = gdal.GRA_CubicSpline 
    elif interpMethod == 'nearestneighbour':
        interpolationMethod = gdal.GRA_NearestNeighbour 
    elif interpMethod == 'cubic':
        interpolationMethod = gdal.GRA_Cubic
    elif interpMethod == 'average':
        interpolationMethod = gdal.GRA_Average
    elif interpMethod == 'mode':
        interpolationMethod = gdal.GRA_Mode
    else:
        raise Exception("Interpolation method was not recognised or known.")
    
    rsgislib.imageutils.createCopyImage(inRefImg, outImg, numBands, 0, format, datatype)

    inFile = gdal.Open(inProcessImg, gdal.GA_ReadOnly)
    outFile = gdal.Open(outImg, gdal.GA_Update)
 
    gdal.ReprojectImage(inFile, outFile, None, None, interpolationMethod, 0.0, 0.0, gdal.TermProgress)
    
    inFile = None
    outFile = None

def reprojectImage(inputImage, outputImage, outWKT, outFormat='KEA', interp='cubic', inWKT=None, noData=0.0, outPxlRes='image', snap2Grid=True):
    """
    This function provides a tool which uses the gdalwarp function to reproject an input 
    image.
    * inputImage - the input image name and path
    * outputImage - the output image name and path
    * outWKT - a WKT file representing the output projection
    * outFormat - the output image file format (Default is KEA)
    * interp - interpolation algorithm. Options are: near, bilinear, cubic, cubicspline, lanczos, average, mode. (Default is cubic)
    * inWKT - if input image is not well defined this is the input image projection as a WKT file (Default is None, i.e., ignored)
    * noData - float representing the not data value (Default is 0.0)
    * outPxlRes three inputs can be provided 
                1) 'image' where the output resolution will match the input (Default is image)
                2) 'auto' where an output resolution maintaining the image size of the input image will be used
                3) provide a floating point value for the image resolution (note. pixels will be sqaure) 
    * snap2Grid is a boolean specifying whether the TL pixel should be snapped to a multiple of the pixel resolution (Default is True).
    """
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported" + gdalErr)
    if not haveOGRPy:
        raise Exception("The OGR python bindings required for this function could not be imported" + ogrErr)
    if not haveOSRPy:
        raise Exception("The OSR python bindings required for this function could not be imported" + osrErr)
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    eResampleAlg = gdal.GRA_CubicSpline
    if interp == 'near':
        eResampleAlg = gdal.GRA_NearestNeighbour
    elif interp == 'bilinear':
        eResampleAlg = gdal.GRA_Bilinear
    elif interp == 'cubic':
        eResampleAlg = gdal.GRA_Cubic
    elif interp == 'cubicspline':
        eResampleAlg = gdal.GRA_CubicSpline
    elif interp == 'lanczos':
        eResampleAlg = gdal.GRA_Lanczos
    elif interp == 'average':
        eResampleAlg = gdal.GRA_Average
    elif interp == 'mode':
        eResampleAlg = gdal.GRA_Mode
    else:
        raise Exception('The interpolation algorithm was not recogonised: \'' + interp + '\'')
    
    if not os.path.exists(inputImage):
        raise Exception('The input image file does not exist: \'' + inputImage + '\'')
    
    inImgDS = gdal.Open(inputImage, gdal.GA_ReadOnly)
    if inImgDS is None:
        raise Exception('Could not open the Input Image: \'' + inputImage + '\'')    
    
    inImgProj = osr.SpatialReference()
    if not inWKT is None:
        if not os.path.exists(inWKT):
            raise Exception('The input WKT file does not exist: \'' + inWKT + '\'')
        inWKTStr = rsgisUtils.readTextFileNoNewLines(inWKT)
        inImgProj.ImportFromWkt(inWKTStr)
    else:
        inImgProj.ImportFromWkt(inImgDS.GetProjectionRef())
        
    if not os.path.exists(outWKT):
        raise Exception('The output WKT file does not exist: \'' + outWKT + '\'')
    outImgProj = osr.SpatialReference()
    outWKTStr = rsgisUtils.readTextFileNoNewLines(outWKT)
    outImgProj.ImportFromWkt(outWKTStr)
    
    geoTransform = inImgDS.GetGeoTransform()
    if geoTransform is None:
        raise Exception('Could read the geotransform from the Input Image: \'' + inputImage + '\'')
    
    xPxlRes = geoTransform[1]
    yPxlRes = geoTransform[5]
    
    inRes = xPxlRes
    if math.fabs(yPxlRes) < math.fabs(xPxlRes):
        inRes = math.fabs(yPxlRes)
    
    xSize = inImgDS.RasterXSize
    ySize = inImgDS.RasterYSize
    
    tlXIn = geoTransform[0]
    tlYIn = geoTransform[3]
    
    brXIn = tlXIn + (xSize * math.fabs(xPxlRes))
    brYIn = tlYIn - (ySize * math.fabs(yPxlRes))
    
    trXIn = brXIn
    trYIn = tlYIn
    
    blXIn = tlXIn
    blYIn = trYIn
    
    #print("TL (IN): [" + str(tlXIn) + " , " + str(tlYIn) + "]")
    #print("BR (IN): [" + str(brYIn) + " , " + str(brYIn) + "]")
    
    numBands = inImgDS.RasterCount
    
    inImgBand = inImgDS.GetRasterBand( 1 );
    gdalDataType = gdal.GetDataTypeName(inImgBand.DataType)
    rsgisDataType = rsgisUtils.getRSGISLibDataType(gdalDataType)
    
    tlPtWKT = 'POINT(%s %s)' % (tlXIn, tlYIn)
    tlPt = ogr.CreateGeometryFromWkt(tlPtWKT)
    tlPt.AssignSpatialReference(inImgProj)
    tlPt.TransformTo(outImgProj)
    tlXOut = tlPt.GetX()
    tlYOut = tlPt.GetY()
    
    brPtWKT = 'POINT(%s %s)' % (brXIn, brYIn)
    brPt = ogr.CreateGeometryFromWkt(brPtWKT)
    brPt.AssignSpatialReference(inImgProj)
    brPt.TransformTo(outImgProj)
    brXOut = brPt.GetX()
    brYOut = brPt.GetY()
    
    trPtWKT = 'POINT(%s %s)' % (trXIn, trYIn)
    trPt = ogr.CreateGeometryFromWkt(trPtWKT)
    trPt.AssignSpatialReference(inImgProj)
    trPt.TransformTo(outImgProj)
    trXOut = trPt.GetX()
    trYOut = trPt.GetY()
    
    blPtWKT = 'POINT(%s %s)' % (blXIn, blYIn)
    btPt = ogr.CreateGeometryFromWkt(blPtWKT)
    btPt.AssignSpatialReference(inImgProj)
    btPt.TransformTo(outImgProj)
    blXOut = btPt.GetX()
    blYOut = btPt.GetY()
    
    xValsOut = [tlXOut, brXOut, trXOut, blXOut]
    yValsOut = [tlYOut, brYOut, trYOut, blYOut]
    
    xMax = max(xValsOut)
    xMin = min(xValsOut)
    
    yMax = max(yValsOut)
    yMin = min(yValsOut)
        
    outPxlRes = outPxlRes.strip()
    outRes = 0.0
    if outPxlRes.isnumeric():
        outRes = math.fabs(float(outPxlRes))
    elif outPxlRes == 'image':
        outRes = inRes
    elif outPxlRes == 'auto':
        xOutRes = (brXOut - tlXOut) / xSize
        yOutRes = (tlYOut - brYOut) / ySize
        outRes = xOutRes
        if yOutRes < xOutRes:
            outRes = yOutRes
    else: 
        raise Exception('Was not able to defined the output resolution. Check Input: \'' + outPxlRes + '\'')
    #print("Output Image has resolution of: " + str(outRes))
    
    outTLX = xMin
    outTLY = yMax
    outWidth = int(round((xMax - xMin) / outRes)) + 1
    outHeight = int(round((yMax - yMin) / outRes)) + 1
    
    #print("TL (OUT): [" + str(tlXOut) + " , " + str(tlYOut) + "]")
    #print("BR (OUT): [" + str(brXOut) + " , " + str(brYOut) + "]")
    
    if snap2Grid:
    
        xLeft = outTLX % outRes
        yLeft = outTLY % outRes
        
        outTLX = (outTLX-xLeft) - (5 * outRes)
        outTLY = ((outTLY-yLeft) + outRes) + (5 * outRes)
    
        #print("TL Leftover: [" + str(xLeft) + " x " + str(yLeft) + "]")
        #print("TL (OUT DEF): [" + str(outTLX) + " , " + str(outTLY) + "]")
        
        outWidth = int(round((xMax - xMin) / outRes)) + 10
        outHeight = int(round((yMax - yMin) / outRes)) + 10
        
    #print("Input Image Size: [" + str(xSize) + " x " + str(ySize) + "]")
    #print("Output Image Size: [" + str(outWidth) + " x " + str(outHeight) + "]")
    
    #print("TL (OUT DEF): [" + str(outTLX) + " , " + str(outTLY) + "]")
    
    print('Creating blank image')
    rsgislib.imageutils.createBlankImage(outputImage, numBands, outWidth, outHeight, outTLX, outTLY, outRes, noData, "", outWKTStr, outFormat, rsgisDataType)

    outImgDS = gdal.Open(outputImage, gdal.GA_Update)
    
    for i in range(numBands):
        outImgDS.GetRasterBand(i+1).SetNoDataValue(noData)
    
    print("Performing the reprojection")
    gdal.ReprojectImage(inImgDS, outImgDS, None, None, eResampleAlg, 0.0, 0.0, gdal.TermProgress )  
    
    inImgDS = None
    outImgDS = None    



def subsetImgs2CommonExtent(inImagesDict, outShpEnv, imgFormat):
    """A command to subset a set of images to the same overlapped extent.

Where:

* inImagesDict is a list of dictionaries containing values for IN (input image) OUT (output image) and TYPE (data type for output)
* outShpEnv is a file path for the output shapefile representing the overlap extent.
* imgFormat is the gdal format of the output images.

Example::
    
    from rsgislib import imageutils
    
    inImagesDict = []
    inImagesDict.append({'IN': './Images/Lifeformclip.tif', 'OUT':'./Subsets/Lifeformclip_sub.kea', 'TYPE':rsgislib.TYPE_32INT})
    inImagesDict.append({'IN': './Images/chmclip.tif', 'OUT':'./Subsets/chmclip_sub.kea', 'TYPE':rsgislib.TYPE_32FLOAT})
    inImagesDict.append({'IN': './Images/peakBGclip.tif', 'OUT':'./Subsets/peakBGclip_sub.kea', 'TYPE':rsgislib.TYPE_32FLOAT})
    
    outputVector = 'imgSubExtent.shp'
    imageutils.subsetImgs2CommonExtent(inImagesDict, outputVector, 'KEA')
    
"""
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported: " + gdalErr)
    import rsgislib
    import rsgislib.vectorutils
    
    inImages = []
    for inImgDict in inImagesDict:
        inImages.append(inImgDict['IN'])
    
    rsgislib.vectorutils.findCommonImgExtent(inImages, outShpEnv, True)
    
    for inImgDict in inImagesDict:
        rsgislib.imageutils.subset(inImgDict['IN'], outShpEnv, inImgDict['OUT'], imgFormat, inImgDict['TYPE'])
    
    
def buildImgSubDict(globFindImgsStr, outDir, suffix, ext):
    """
    Automate building the dictionary of image to be used within the 
subsetImgs2CommonExtent(inImagesDict, outShpEnv, imgFormat) function.

Where:

* globFindImgsStr is a string to be passed to the glob module to find the input image files.
* outDir is the output directory path for the images.
* suffix is a suffix to be appended on to the end of the file name (can be a blank string, i.e., '')
* ext is a string with the output file extension

Example::
    
    from rsgislib import imageutils
    
    inImagesDict = imageutils.buildImgSubDict("./Images/*.tif", "./Subsets/", "_sub", ".kea")
    print(inImagesDict)
    
    outputVector = 'imgSubExtent.shp'
    imageutils.subsetImgs2CommonExtent(inImagesDict, outputVector, 'KEA')

"""
    # Check is glob is available.
    try:
        import glob
    except ImportError as globErr:
        raise Exception("The glob module could not be imported: " + globErr)
    # Check gdal is available
    if not haveGDALPy:
        raise Exception("The GDAL python bindings required for this function could not be imported: " + gdalErr)
    import rsgislib
    import os.path
        
    inImagesDict = []
    
    inputImages = glob.glob(globFindImgsStr)
    #print(inputImages)
    if len(inputImages) == 0:
        raise Exception("No images were found using \'" + globFindImgsStr + "\'")
    
    for image in inputImages:
        #print(image)
        dataset = gdal.Open(image, gdal.GA_ReadOnly)
        gdalDType = dataset.GetRasterBand(1).DataType
        dataset = None
        datatype = rsgislib.TYPE_32FLOAT
        if gdalDType == gdal.GDT_Byte:
            datatype = rsgislib.TYPE_8UINT
        elif gdalDType == gdal.GDT_Int16:
            datatype = rsgislib.TYPE_16INT
        elif gdalDType == gdal.GDT_Int32:
            datatype = rsgislib.TYPE_32INT
        elif gdalDType == gdal.GDT_UInt16:
            datatype = rsgislib.TYPE_16UINT
        elif gdalDType == gdal.GDT_UInt32:
            datatype = rsgislib.TYPE_32UINT         
        elif gdalDType == gdal.GDT_Float32:
            datatype = rsgislib.TYPE_32FLOAT
        elif gdalDType == gdal.GDT_Float64:
            datatype = rsgislib.TYPE_64FLOAT
        else:
            raise Exception("Data type of the input file was not recognised or known.")
            
        imgBase = os.path.splitext(os.path.basename(image))[0]
        #print(imgBase)
        outImg = os.path.join(outDir, (imgBase+suffix+ext))
        #print(outImg)
        #print('\n')
        inImagesDict.append({'IN':image, 'OUT':outImg, 'TYPE':datatype})

    return inImagesDict

