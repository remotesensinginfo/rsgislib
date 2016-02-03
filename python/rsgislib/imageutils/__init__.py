"""
The imageutils module contains general utilities for applying to images.
"""

# import the C++ extension into this level
from ._imageutils import *
import rsgislib 

haveGDALPy = True
try:
    import osgeo.gdal as gdal
except ImportError as gdalErr:
    haveGDALPy = False

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


def getRSGISLibDataType(inImgFile):
    """
    Returns the rsgislib datatype ENUM for a raster file
    :param inImgFile: The file to get the datatype for
    :return: The rsgislib datatype enum, e.g., rsgislib.TYPE_8INT
    """
    raster = gdal.Open(inImgFile, gdal.GA_ReadOnly)
    band = raster.GetRasterBand(1)
    gdal_dtype = gdal.GetDataTypeName(band.DataType)
    raster = None
    rsgis_utils = rsgislib.RSGISPyUtils()
    return rsgis_utils.getRSGISLibDataType(gdal_dtype)

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
    import rsgislib
    
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
 
    gdal.ReprojectImage(inFile, outFile, None, None, interpolationMethod)
    
    inFile = None
    outFile = None
    
    
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

