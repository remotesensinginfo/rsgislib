#!/usr/bin/env python
"""
The imageutils module contains general utilities for applying to images.
"""
# Maintain python 2 backwards compatibility
from __future__ import print_function
# import the C++ extension into this level
from ._imageutils import *
import rsgislib 

import os
import os.path
import math
import shutil

import numpy

import osgeo.gdal as gdal
import osgeo.osr as osr

from rios import applier


class ImageBandInfo(object):
    """
Create a list of these objects to pass to the extractZoneImageBandValues2HDF function

:param fileName: is the input image file name and path.
:param name: is a name associated with this layer - doesn't really matter what you use but needs to be unique; this is used as a dict key in some functions.
:param bands: is a list of image bands within the fileName to be used for processing (band numbers start at 1).

"""
    def __init__(self, fileName=None, name=None, bands=None):
        """
        :param fileName: is the input image file name and path.
        :param name: is a name associated with this layer - doesn't really matter what you use but needs to be unique; this is used as a dict key in some functions.
        :param bands: is a list of image bands within the fileName to be used for processing (band numbers start at 1).
        """
        self.fileName = fileName
        self.name = name
        self.bands = bands


class OutImageInfo(object):
    """
A class which is used to define the information to create a new output image.
This class is used within the StdImgBlockIter class.

:param file_name: is the output image file name and path.
:param name: is a name associated with this layer - doesn't really matter what you use but needs to be unique; this is used as a dict key in some functions.
:param nbands: is an int with the number of output image bands.
:param no_data_val: is a no data value for the output image
:param gdal_format: is the output image file format
:param datatype: is the output datatype rsgislib.TYPE_*

"""
    def __init__(self, file_name=None, name=None, nbands=None, no_data_val=None, gdal_format=None, datatype=None):
        """
        :param file_name: is the input image file name and path.
        :param name: is a name associated with this layer - doesn't really matter what you use but needs to be unique; this is used as a dict key in some functions.
        :param nbands: is an int with the number of output image bands.
        :param no_data_val: is a no data value for the output image
        :param gdal_format: is the output image file format
        :param datatype: is the output datatype rsgislib.TYPE_*
        """
        self.file_name = file_name
        self.name = name
        self.nbands = nbands
        self.no_data_val = no_data_val
        self.gdal_format = gdal_format
        self.datatype = datatype


class SharpBandInfo(object):
    """
Create a list of these objects to pass to the sharpenLowResBands function.

:param band: is the band number (band numbering starts at 1).
:param status: needs to be either rsgislib.SHARP_RES_IGNORE, rsgislib.SHARP_RES_LOW or rsgislib.SHARP_RES_HIGH
               lowres bands will be sharpened using the highres bands and ignored bands
               will just be copied into the output image.
:param name: is a name associated with this image band - doesn't really matter what you put in here.

"""
    def __init__(self, band=None, status=None, name=None):
        """
        :param band: is the band number (band numbering starts at 1).
        :param status: needs to be either 'ignore', 'lowres' or 'highres' - lowres bands will be sharpened using the highres bands and ignored bands will just be copied into the output image.
        :param name: is a name associated with this image band - doesn't really matter what you put in here.

        """
        self.band = band
        self.status = status
        self.name = name

# Define Class for time series fill
class RSGISTimeseriesFillInfo(object):
    """
Create a list of these objects to pass to the fillTimeSeriesGaps function

:param year: year the composite represents.
:param day: the (nominal) day within the year the composite represents (a value of zero and day will not be used)
:param compImg: The input compsite image which has been generated.
:param imgRef:  The reference layer (e.g., from createMaxNDVIComposite or createMaxNDVINDWICompositeLandsat) with zero for no data regions
:param outRef: A boolean variable specify which layer a fill reference layer is to be produced.

"""
    def __init__(self, year=1900, day=0, compImg=None, imgRef=None, outRef=False):
        """
        :param year: year the composite represents.
        :param day: the (nominal) day within the year the composite represents (a value of zero and day will not be used)
        :param compImg: The input compsite image which has been generated.
        :param imgRef:  The reference layer (e.g., from createMaxNDVIComposite or createMaxNDVINDWICompositeLandsat) with zero for no data regions
        :param outRef: A boolean variable specify which layer a fill reference layer is to be produced.

        """
        self.year = year
        self.day = day
        self.compImg = compImg
        self.imgRef = imgRef
        self.outRef = outRef
    
    def __repr__(self):
        return repr((self.year, self.day, self.compImg, self.imgRef, self.outRef))


def setBandNames(inputImage, bandNames, feedback=False):
    """A utility function to set band names.
Where:

:param inImage: is the input image
:param bandNames: is a list of band names
:param feedback: is a boolean specifying whether feedback will be printed to the console (True= Printed / False (default) Not Printed)

Example::

    from rsgislib import imageutils

    inputImage = 'injune_p142_casi_sub_utm.kea'
    bandNames = ['446nm','530nm','549nm','569nm','598nm','633nm','680nm','696nm','714nm','732nm','741nm','752nm','800nm','838nm']
    
    imageutils.setBandNames(inputImage, bandNames)
    
"""
    dataset = gdal.Open(inputImage, gdal.GA_Update)
    
    for i in range(len(bandNames)):
        band = i+1
        bandName = bandNames[i]

        imgBand = dataset.GetRasterBand(band)
        # Check the image band is available
        if not imgBand is None:
            if feedback:
                print('Setting Band {0} to "{1}"'.format(band, bandName))
            imgBand.SetDescription(bandName)
        else:
            raise Exception("Could not open the image band: ", band)

def getBandNames(inputImage):
    """
A utility function to get band names.

Where:

:param inImage: is the input image

:return: list of band names

Example::

    from rsgislib import imageutils

    inputImage = 'injune_p142_casi_sub_utm.kea'
    bandNames = imageutils.getBandNames(inputImage)

"""
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

:param inImg: The file to get the datatype for

:return: The rsgislib datatype enum, e.g., rsgislib.TYPE_8INT

"""
    raster = gdal.Open(inImg, gdal.GA_ReadOnly)
    if raster == None:
        raise Exception("Could not open the inImg.")
    band = raster.GetRasterBand(1)
    gdal_dtype = gdal.GetDataTypeName(band.DataType)
    raster = None
    rsgis_utils = rsgislib.RSGISPyUtils()

    return rsgis_utils.getRSGISLibDataType(gdal_dtype)


def getGDALDataType(inImg):
    """
Returns the rsgislib datatype ENUM for a raster file

:param inImg: The file to get the datatype for

:return: The rsgislib datatype enum, e.g., rsgislib.TYPE_8INT

"""
    raster = gdal.Open(inImg, gdal.GA_ReadOnly)
    if raster == None:
        raise Exception("Could not open the inImg.")
    band = raster.GetRasterBand(1)
    gdal_dtype = gdal.GetDataTypeName(band.DataType)
    raster = None
    return gdal_dtype


def setImgThematic(imageFile):
    """
Set all image bands to be thematic. 

:param imageFile: The file for which the bands are to be set as thematic

"""
    ds = gdal.Open(imageFile, gdal.GA_Update)
    if ds == None:
        raise Exception("Could not open the imageFile.")
    for bandnum in range(ds.RasterCount):
        band = ds.GetRasterBand(bandnum + 1)
        band.SetMetadataItem('LAYER_TYPE', 'thematic')
    ds = None


def setImgNotThematic(imageFile):
    """
Set all image bands to be not thematic (athematic).

:param imageFile: The file for which the bands are to be set as not thematic (athematic)

"""
    ds = gdal.Open(imageFile, gdal.GA_Update)
    if ds == None:
        raise Exception("Could not open the imageFile.")
    for bandnum in range(ds.RasterCount):
        band = ds.GetRasterBand(bandnum + 1)
        band.SetMetadataItem('LAYER_TYPE', 'athematic')
    ds = None

def hasGCPs(inImg):
    """
Test whether the input image has GCPs - returns boolean

:param inImg: input image file

:return: boolean True - has GCPs; False - does not have GCPs

"""
    raster = gdal.Open(inImg, gdal.GA_ReadOnly)
    if raster == None:
        raise Exception("Could not open the inImg.")
    numGCPs = raster.GetGCPCount()
    hasGCPs = False
    if numGCPs > 0:
        hasGCPs = True
    raster = None
    return hasGCPs

def copyGCPs(srcImg, destImg):
    """
Copy the GCPs from the srcImg to the destImg

:param srcImg: Raster layer with GCPs
:param destImg: Raster layer to which GCPs will be added
    
"""
    srcDS = gdal.Open(srcImg, gdal.GA_ReadOnly)
    if srcDS == None:
        raise Exception("Could not open the srcImg.")
    destDS = gdal.Open(destImg, gdal.GA_Update)
    if destDS == None:
        raise Exception("Could not open the destImg.")
        srcDS = None

    numGCPs = srcDS.GetGCPCount()
    if numGCPs > 0:
        gcpProj = srcDS.GetGCPProjection()
        gcpList = srcDS.GetGCPs()
        destDS.SetGCPs(gcpList, gcpProj)

    srcDS = None
    destDS = None


def getWKTProjFromImage(inImg):
    """
A function which returns the WKT string representing the projection of the input image.

:param inImg: input image from which WKT string will be read.

"""
    rasterDS = gdal.Open(inImg, gdal.GA_ReadOnly)
    if rasterDS == None:
        raise Exception('Could not open raster image: \'' + inImg+ '\'')
    projStr = rasterDS.GetProjection()
    rasterDS = None
    return projStr


def createBlankImagePy(output_img, n_bands, width, height, tlX, tlY, out_img_res_x, out_img_res_y, wkt_string,
                       gdal_format, data_type, options=[], no_data_val=0):
    """
    Create a blank output image file - this is a pure python implementation of rsgislib.imageutils.createBlankImage

    :param output_img: the output file and path.
    :param n_bands: the number of output image bands.
    :param width: the number of x pixels.
    :param height: the number of Y pixels.
    :param tlX: the top-left corner x coordinate
    :param tlY: the top-left corner y coordinate
    :param out_img_res_x: the output image resolution in the x axis
    :param out_img_res_y: the output image resolution in the y axis
    :param wkt_string: a WKT string with the output image projection
    :param gdal_format: the output image file format.
    :param data_type: the output image data type - needs to be a rsgislib datatype (e.g., )
    :param options: image creation options e.g., ["TILED=YES", "INTERLEAVE=PIXEL", "COMPRESS=LZW", "BIGTIFF=YES"]
    :param no_data_val: the output image no data value.

    """
    rsgis_utils = rsgislib.RSGISPyUtils()
    gdal_data_type = rsgis_utils.getGDALDataType(data_type)
    gdal_driver = gdal.GetDriverByName(gdal_format)
    out_img_ds_obj = gdal_driver.Create(output_img, width, height, n_bands, gdal_data_type, options=options)
    out_img_ds_obj.SetGeoTransform((tlX, out_img_res_x, 0, tlY, 0, out_img_res_y))
    out_img_ds_obj.SetProjection(wkt_string)

    raster = numpy.zeros((height, width), dtype=rsgis_utils.getNumpyDataType(data_type))
    raster[...] = no_data_val
    for band in range(n_bands):
        band_obj = out_img_ds_obj.GetRasterBand(band + 1)
        band_obj.SetNoDataValue(no_data_val)
        band_obj.WriteArray(raster)
    out_img_ds_obj = None


def createBlankImgFromRefVector(inVecFile, inVecLyr, outputImg, outImgRes, outImgNBands, gdalformat, datatype):
    """
A function to create a new image file based on a vector layer to define the extent and projection
of the output image. 

:param inVecFile: input vector file.
:param inVecLyr: name of the vector layer, if None then assume the layer name will be the same as the file
                 name of the input vector file.
:param outputImg: output image file.
:param outImgRes: output image resolution, square pixels so a single value.
:param outImgNBands: the number of image bands in the output image
:param gdalformat: output image file format.
:param datatype: is a rsgislib.TYPE_* value providing the data type of the output image

"""

    rsgisUtils = rsgislib.RSGISPyUtils()

    baseExtent = rsgisUtils.getVecLayerExtent(inVecFile, inVecLyr)
    xMin, xMax, yMin, yMax = rsgisUtils.findExtentOnGrid(baseExtent, outImgRes, fullContain=True)

    tlX = xMin
    tlY = yMax
    
    widthCoord = xMax - xMin
    heightCoord = yMax - yMin
    
    width = int(math.ceil(widthCoord/outImgRes))
    height = int(math.ceil(heightCoord/outImgRes))
    
    wktString = rsgisUtils.getProjWKTFromVec(inVecFile)

    rsgislib.imageutils.createBlankImage(outputImg, outImgNBands, width, height, tlX, tlY, outImgRes, 0.0, '', wktString, gdalformat, datatype)
    

def createCopyImageVecExtentSnap2Grid(inVecFile, inVecLyr, outputImg, outImgRes, outImgNBands, gdalformat, datatype, bufnpxl=0):
    """
A function to create a new image file based on a vector layer to define the extent and projection
of the output image. The image file extent is snapped on to the grid defined by the vector layer.

:param inVecFile: input vector file.
:param inVecLyr: name of the vector layer, if None then assume the layer name will be the same as the file
                 name of the input vector file.
:param outputImg: output image file.
:param outImgRes: output image resolution, square pixels so a single value.
:param outImgNBands: the number of image bands in the output image
:param gdalformat: output image file format.
:param datatype: is a rsgislib.TYPE_* value providing the data type of the output image
:param bufnpxl: is an integer specifying the number of pixels to buffer the vector file extent by.

"""
    rsgisUtils = rsgislib.RSGISPyUtils()
    
    vec_bbox = rsgisUtils.getVecLayerExtent(inVecFile, layerName=inVecLyr, computeIfExp=True)
    xMin = vec_bbox[0] - (outImgRes * bufnpxl)
    xMax = vec_bbox[1] + (outImgRes * bufnpxl)
    yMin = vec_bbox[2] - (outImgRes * bufnpxl)
    yMax = vec_bbox[3] + (outImgRes * bufnpxl)
    xMin, xMax, yMin, yMax = rsgisUtils.findExtentOnWholeNumGrid([xMin, xMax, yMin, yMax], outImgRes, True) 
    
    tlX = xMin
    tlY = yMax
    
    widthCoord = xMax - xMin
    heightCoord = yMax - yMin
    
    width = int(math.ceil(widthCoord/outImgRes))
    height = int(math.ceil(heightCoord/outImgRes))
    
    wktString = rsgisUtils.getProjWKTFromVec(inVecFile)
    
    rsgislib.imageutils.createBlankImage(outputImg, outImgNBands, width, height, tlX, tlY, outImgRes, 0.0, '', wktString, gdalformat, datatype)
    

def createBlankImgFromBBOX(bbox, wktstr, outputImg, outImgRes, outImgPxlVal, outImgNBands, gdalformat, datatype, snap2grid=False):
    """
A function to create a new image file based on a bbox to define the extent. 

:param bbox: bounding box defining the extent of the output image (xMin, xMax, yMin, yMax)
:param wktstr: the WKT string defining the bbox and output image projection.
:param outputImg: output image file.
:param outImgRes: output image resolution, square pixels so a single value.
:param outImgPxlVal: output image pixel value.
:param outImgNBands: the number of image bands in the output image
:param gdalformat: output image file format.
:param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.
:param snap2grid: optional variable to snap the image to a grid of whole numbers with respect to the image pixel resolution.

"""    
    if snap2grid:
        rsgisUtils = rsgislib.RSGISPyUtils()
        bbox = rsgisUtils.findExtentOnGrid(bbox, outImgRes, fullContain=True)

    xMin = bbox[0]
    xMax = bbox[1]
    yMin = bbox[2]
    yMax = bbox[3]

    tlX = xMin
    tlY = yMax
    
    widthCoord = xMax - xMin
    heightCoord = yMax - yMin
    
    width = int(math.ceil(widthCoord/outImgRes))
    height = int(math.ceil(heightCoord/outImgRes))
    
    rsgislib.imageutils.createBlankImage(outputImg, outImgNBands, width, height, tlX, tlY, outImgRes, outImgPxlVal, '', wktstr, gdalformat, datatype)

   
def createImageForEachVecFeat(vectorFile, vectorLyr, fileNameCol, outImgPath, outImgExt, outImgPxlVal, outImgNBands, outImgRes, gdalformat, datatype, snap2grid=False):
    """
A function to create a set of image files representing the extent of each feature in the 
inputted vector file.

:param vectorFile: the input vector file.
:param vectorLyr: the input vector layer
:param fileNameCol: the name of the column in the vector layer which will be used as the file names.
:param outImgPath: output file path (directory) where the images will be saved.
:param outImgExt: the file extension to be added on to the output file names.
:param outImgPxlVal: output image pixel value
:param outImgNBands: the number of image bands in the output image
:param outImgRes: output image resolution, square pixels so a single value
:param gdalformat: output image file format.
:param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.
:param snap2grid: optional variable to snap the image to a grid of whole numbers with respect to the image pixel resolution.

"""
    
    dsVecFile = gdal.OpenEx(vectorFile, gdal.OF_VECTOR )
    if dsVecFile is None:
        raise Exception("Could not open '" + vectorFile + "'")
        
    lyrVecObj = dsVecFile.GetLayerByName( vectorLyr )
    if lyrVecObj is None:
        raise Exception("Could not find layer '" + vectorLyr + "'")
        
    lyrSpatRef = lyrVecObj.GetSpatialRef()
    if lyrSpatRef is not None:
        wktstr = lyrSpatRef.ExportToWkt()
    else:
        wktstr = ''
        
    colExists = False
    feat_idx = 0
    lyrDefn = lyrVecObj.GetLayerDefn()
    for i in range( lyrDefn.GetFieldCount() ):
        if lyrDefn.GetFieldDefn(i).GetName().lower() == fileNameCol.lower():
            feat_idx = i
            colExists = True
            break
    
    if not colExists:
        dsVecFile = None
        raise Exception("The specified column does not exist in the input layer; check case as some drivers are case sensitive.")
    
    lyrVecObj.ResetReading()
    for feat in lyrVecObj:
        geom = feat.GetGeometryRef()
        if geom is not None:
            env = geom.GetEnvelope()
            tilebasename = feat.GetFieldAsString(feat_idx)
            outputImg = os.path.join(outImgPath, "{0}{1}".format(tilebasename, outImgExt))
            print(outputImg)
            createBlankImgFromBBOX(env, wktstr, outputImg, outImgRes, outImgPxlVal, outImgNBands, gdalformat, datatype, snap2grid)


def resampleImage2Match(inRefImg, inProcessImg, outImg, gdalformat, interpMethod, datatype=None, noDataVal=None, multicore=False):
    """
A utility function to resample an existing image to the projection and/or pixel size of another image.

Where:

:param inRefImg: is the input reference image to which the processing image is to resampled to.
:param inProcessImg: is the image which is to be resampled.
:param outImg: is the output image file.
:param gdalformat: is the gdal format for the output image.
:param interpMethod: is the interpolation method used to resample the image [bilinear, lanczos, cubicspline, nearestneighbour, cubic, average, mode]
:param datatype: is the rsgislib datatype of the output image (if none then it will be the same as the input file).
:param multicore: use multiple processing cores (Default = False)

""" 
    rsgisUtils = rsgislib.RSGISPyUtils()
    numBands = rsgisUtils.getImageBandCount(inProcessImg)
    if noDataVal == None:
        noDataVal = rsgisUtils.getImageNoDataValue(inProcessImg)
    
    if datatype == None:
        datatype = rsgisUtils.getRSGISLibDataTypeFromImg(inProcessImg)
        
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
    
    backVal = 0.0
    haveNoData = False
    if noDataVal != None:
        backVal = float(noDataVal)
        haveNoData = True
    
    rsgislib.imageutils.createCopyImage(inRefImg, outImg, numBands, backVal, gdalformat, datatype)

    inFile = gdal.Open(inProcessImg, gdal.GA_ReadOnly)
    outFile = gdal.Open(outImg, gdal.GA_Update)

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    wrpOpts = []
    if multicore:
        if haveNoData:
            wrpOpts = gdal.WarpOptions(resampleAlg=interpolationMethod, srcNodata=noDataVal, dstNodata=noDataVal, multithread=True, callback=callback )
        else:
            wrpOpts = gdal.WarpOptions(resampleAlg=interpolationMethod, multithread=True, callback=callback )
    else:
        if haveNoData:
            wrpOpts = gdal.WarpOptions(resampleAlg=interpolationMethod, srcNodata=noDataVal, dstNodata=noDataVal, multithread=False, callback=callback )
        else:
            wrpOpts = gdal.WarpOptions(resampleAlg=interpolationMethod, multithread=False, callback=callback )
    
    gdal.Warp(outFile, inFile, options=wrpOpts)
    
    inFile = None
    outFile = None


def reprojectImage(inputImage, outputImage, outWKT, gdalformat='KEA', interp='cubic', inWKT=None, noData=0.0, outPxlRes='image', snap2Grid=True, multicore=False, gdal_options=[]):
    """
This function provides a tool which uses the gdalwarp function to reproject an input image. When you want an simpler
interface use the rsgislib.imageutils.gdal_warp function. This handles more automatically.

Where:

:param inputImage: the input image name and path
:param outputImage: the output image name and path
:param outWKT: a WKT file representing the output projection
:param gdalformat: the output image file format (Default is KEA)
:param interp: interpolation algorithm. Options are: near, bilinear, cubic, cubicspline, lanczos, average,
               mode. (Default is cubic)
:param inWKT: if input image is not well defined this is the input image projection as a WKT file (Default
              is None, i.e., ignored)
:param noData: float representing the not data value (Default is 0.0)
:param outPxlRes: three inputs can be provided. 1) 'image' where the output resolution will match the input
                  (Default is image). 2) 'auto' where an output resolution maintaining the image size of the
                  input image will be used. You may consider using rsgislib.imageutils.gdal_warp instead of
                  this option. 3) provide a floating point value for the image resolution (note. pixels will
                  be sqaure)
:param snap2Grid: is a boolean specifying whether the TL pixel should be snapped to a multiple of the pixel
                  resolution (Default is True).
:param nCores: the number of processing cores available for processing (-1 is all cores: Default=-1)
:param gdal_options: GDAL file creation options e.g., ["TILED=YES", "COMPRESS=LZW", "BIGTIFF=YES"]

    """    
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
    
    numBands = inImgDS.RasterCount
    
    inImgBand = inImgDS.GetRasterBand( 1 )
    gdalDataType = gdal.GetDataTypeName(inImgBand.DataType)
    rsgisDataType = rsgisUtils.getRSGISLibDataType(gdalDataType)

    tlXOut, tlYOut = rsgisUtils.reprojPoint(inImgProj, outImgProj, tlXIn, tlYIn)
    brXOut, brYOut = rsgisUtils.reprojPoint(inImgProj, outImgProj, brXIn, brYIn)
    trXOut, trYOut = rsgisUtils.reprojPoint(inImgProj, outImgProj, trXIn, trYIn)
    blXOut, blYOut = rsgisUtils.reprojPoint(inImgProj, outImgProj, blXIn, blYIn)

    xValsOut = [tlXOut, brXOut, trXOut, blXOut]
    yValsOut = [tlYOut, brYOut, trYOut, blYOut]
    
    xMax = max(xValsOut)
    xMin = min(xValsOut)
    
    yMax = max(yValsOut)
    yMin = min(yValsOut)
    
    outPxlRes = str(outPxlRes).strip()
    outRes = 0.0
    if rsgisUtils.isNumber(outPxlRes):
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

    outTLX = xMin
    outTLY = yMax
    outWidth = int(round((xMax - xMin) / outRes)) + 1
    outHeight = int(round((yMax - yMin) / outRes)) + 1
    
    if snap2Grid:
    
        xLeft = outTLX % outRes
        yLeft = outTLY % outRes
        
        outTLX = (outTLX-xLeft) - (5 * outRes)
        outTLY = ((outTLY-yLeft) + outRes) + (5 * outRes)
        
        outWidth = int(round((xMax - xMin) / outRes)) + 10
        outHeight = int(round((yMax - yMin) / outRes)) + 10
    
    print('Creating blank image')
    rsgislib.imageutils.createBlankImagePy(outputImage, numBands, outWidth, outHeight, outTLX, outTLY, outRes,
                       (outRes * (-1)), outWKTStr, gdalformat, rsgisDataType, options=gdal_options, no_data_val=noData)

    outImgDS = gdal.Open(outputImage, gdal.GA_Update)
    
    for i in range(numBands):
        outImgDS.GetRasterBand(i+1).SetNoDataValue(noData)

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    print("Performing the reprojection")
    wrpOpts = []
    if multicore:
        wrpOpts = gdal.WarpOptions(resampleAlg=eResampleAlg, srcNodata=noData, dstNodata=noData, multithread=True, callback=callback)
    else:
        wrpOpts = gdal.WarpOptions(resampleAlg=eResampleAlg, srcNodata=noData, dstNodata=noData, multithread=False, callback=callback)

    gdal.Warp(outImgDS, inImgDS, options=wrpOpts)

    inImgDS = None
    outImgDS = None


def gdal_warp(input_img, output_img, out_epsg, interp='near', gdalformat='KEA', use_multi_threaded=True, options=[]):
    """
    A function which runs GDAL Warp function to tranform an image from one projection to another. Use this function
    when you want GDAL to do procesing of pixel size and image size automatically. rsgislib.imageutils.reprojectImage
    should be used when you want to put the output image on a particular grid etc.

    :param input_img: input image file
    :param output_img: output image file
    :param out_epsg: the EPSG for the output image file.
    :param interp: interpolation algorithm. Options are: near, bilinear, cubic, cubicspline, lanczos, average, mode. (Default is near)
    :param gdalformat: output image file format
    :param use_multi_threaded: Use multiple cores for processing (Default: True).
    :param options: GDAL file creation options e.g., ["TILED=YES", "COMPRESS=LZW", "BIGTIFF=YES"]

    """
    from osgeo import gdal
    gdal.UseExceptions()
    rsgisUtils = rsgislib.RSGISPyUtils()
    in_no_data_val = rsgisUtils.getImageNoDataValue(input_img)
    in_epsg = rsgisUtils.getEPSGCode(input_img)
    img_data_type = rsgisUtils.getGDALDataTypeFromImg(input_img)

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

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    warp_opts = gdal.WarpOptions(format=gdalformat, srcSRS="EPSG:{}".format(in_epsg), dstSRS="EPSG:{}".format(out_epsg),
                                 resampleAlg=eResampleAlg, srcNodata=in_no_data_val, dstNodata=in_no_data_val,
                                 callback=callback, creationOptions=options, outputType=img_data_type,
                                 workingType=gdal.GDT_Float32, multithread=use_multi_threaded)
    gdal.Warp(output_img, input_img, options=warp_opts)

def subsetPxlBBox(inputimage, outputimage, gdalformat, datatype, xMinPxl, xMaxPxl, yMinPxl, yMaxPxl):
    """
Function to subset an input image using a defined pixel bbox.

:param inputimage: input image to be subset.
:param outputimage: output image file.
:param gdalformat: output image file format
:param datatype: datatype is a rsgislib.TYPE_* value providing the data type of the output image.
:param xMinPxl: min x in pixels
:param xMaxPxl: max x in pixels
:param yMinPxl: min y in pixels
:param yMaxPxl: max y in pixels

"""
    rsgis_utils = rsgislib.RSGISPyUtils()
    bbox = rsgis_utils.getImageBBOX(inputimage)
    xRes, yRes = rsgis_utils.getImageRes(inputimage)
    xSize, ySize = rsgis_utils.getImageSize(inputimage)
    
    if (xMaxPxl > xSize) or (yMaxPxl > ySize):
        raise Exception("The pixel extent defined is bigger than the input image.")
    
    xMin = bbox[0] + (xMinPxl * xRes)
    xMax = bbox[0] + (xMaxPxl * xRes)
    yMin = bbox[2] + (yMinPxl * yRes)
    yMax = bbox[2] + (yMaxPxl * yRes)
    
    rsgislib.imageutils.subsetbbox(inputimage, outputimage, gdalformat, datatype, xMin, xMax, yMin, yMax)

def createTilesMultiCore(inputimage, baseimage, width, height, gdalformat, datatype, ext, ncores=1):
    """
Function to generate a set of tiles for the input image.

:param inputimage: input image to be subset.
:param baseimage: output image files base path.
:param width: width in pixels of the tiles.
:param height: height in pixels of the tiles.
:param gdalformat: output image file format
:param datatype: datatype is a rsgislib.TYPE_* value providing the data type of the output image.
:param ext: output file extension to be added to the baseimage path (e.g., kea)
:param ncores: number of cores to be used; uses python multiprocessing module.

"""
    import multiprocessing
    rsgis_utils = rsgislib.RSGISPyUtils()
    xSize, ySize = rsgis_utils.getImageSize(inputimage)
    
    n_full_xtiles = math.floor(xSize/width)
    x_remain_width = xSize - (n_full_xtiles * width)
    n_full_ytiles = math.floor(ySize/height)
    y_remain_height = ySize - (n_full_ytiles * height)
    
    tiles = []
    
    for ytile in range(n_full_ytiles):
        y_pxl_min = ytile * height
        y_pxl_max = y_pxl_min + height
    
        for xtile in range(n_full_xtiles):
            x_pxl_min = xtile * width
            x_pxl_max = x_pxl_min + width
            tiles.append({'tile':'x{0}y{1}'.format(xtile+1, ytile+1), 'bbox':[x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max]})

        if x_remain_width > 0:
            x_pxl_min = n_full_xtiles * width
            x_pxl_max = x_pxl_min + x_remain_width
            tiles.append({'tile':'x{0}y{1}'.format(n_full_xtiles+1, ytile+1), 'bbox':[x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max]})
    
    if y_remain_height > 0:
        y_pxl_min = n_full_ytiles * height
        y_pxl_max = y_pxl_min + y_remain_height
        
        for xtile in range(n_full_xtiles):
            x_pxl_min = xtile * width
            x_pxl_max = x_pxl_min + width
            tiles.append({'tile':'x{0}y{1}'.format(xtile+1, n_full_ytiles+1), 'bbox':[x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max]})

        if x_remain_width > 0:
            x_pxl_min = n_full_xtiles * width
            x_pxl_max = x_pxl_min + x_remain_width
            tiles.append({'tile':'x{0}y{1}'.format(n_full_xtiles+1, n_full_ytiles+1), 'bbox':[x_pxl_min, x_pxl_max, y_pxl_min, y_pxl_max]})
    
    for tile in tiles:
        tile['inputimage'] = inputimage
        tile['outfile'] = "{0}_{1}.{2}".format(baseimage, tile['tile'], ext)
        tile['gdalformat'] = gdalformat
        tile['datatype'] = datatype

    def _runSubset(tileinfo):
        """ Internal function for createTilesMultiCore for multiprocessing Pool. """
        subsetPxlBBox(tileinfo['inputimage'], tileinfo['outfile'], tileinfo['gdalformat'], tileinfo['datatype'],
                      tileinfo['bbox'][0], tileinfo['bbox'][1], tileinfo['bbox'][2], tileinfo['bbox'][3])

    poolobj = multiprocessing.Pool(ncores)
    poolobj.map(_runSubset, tiles)


def subsetImgs2CommonExtent(inImagesDict, outShpEnv, gdalformat):
    """
A command to subset a set of images to the same overlapped extent.

Where:

:param inImagesDict: is a list of dictionaries containing values for IN (input image) OUT (output image) and TYPE (data type for output)
:param outShpEnv: is a file path for the output shapefile representing the overlap extent.
:param gdalformat: is the gdal format of the output images.

Example::
    
    from rsgislib import imageutils
    
    inImagesDict = []
    inImagesDict.append({'IN': './Images/Lifeformclip.tif', 'OUT':'./Subsets/Lifeformclip_sub.kea', 'TYPE':rsgislib.TYPE_32INT})
    inImagesDict.append({'IN': './Images/chmclip.tif', 'OUT':'./Subsets/chmclip_sub.kea', 'TYPE':rsgislib.TYPE_32FLOAT})
    inImagesDict.append({'IN': './Images/peakBGclip.tif', 'OUT':'./Subsets/peakBGclip_sub.kea', 'TYPE':rsgislib.TYPE_32FLOAT})
    
    outputVector = 'imgSubExtent.shp'
    imageutils.subsetImgs2CommonExtent(inImagesDict, outputVector, 'KEA')
    
"""
    import rsgislib.vectorutils
    
    inImages = []
    for inImgDict in inImagesDict:
        inImages.append(inImgDict['IN'])
    
    rsgislib.vectorutils.findCommonImgExtent(inImages, outShpEnv, True)
    
    for inImgDict in inImagesDict:
        rsgislib.imageutils.subset(inImgDict['IN'], outShpEnv, inImgDict['OUT'], gdalformat, inImgDict['TYPE'])


def buildImgSubDict(globFindImgsStr, outDir, suffix, ext):
    """
Automate building the dictionary of image to be used within the 
subsetImgs2CommonExtent(inImagesDict, outShpEnv, imgFormat) function.

Where:

:param globFindImgsStr: is a string to be passed to the glob module to find the input image files.
:param outDir: is the output directory path for the images.
:param suffix: is a suffix to be appended on to the end of the file name (can be a blank string, i.e., '')
:param ext: is a string with the output file extension

Example::
    
    from rsgislib import imageutils
    
    inImagesDict = imageutils.buildImgSubDict("./Images/*.tif", "./Subsets/", "_sub", ".kea")
    print(inImagesDict)
    
    outputVector = 'imgSubExtent.shp'
    imageutils.subsetImgs2CommonExtent(inImagesDict, outputVector, 'KEA')

"""
    import glob
    import os.path
        
    inImagesDict = []
    
    inputImages = glob.glob(globFindImgsStr)
    if len(inputImages) == 0:
        raise Exception("No images were found using \'" + globFindImgsStr + "\'")
    
    for image in inputImages:
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
        outImg = os.path.join(outDir, (imgBase+suffix+ext))
        inImagesDict.append({'IN':image, 'OUT':outImg, 'TYPE':datatype})

    return inImagesDict


def calcPixelLocations(inputImg, outputImg, gdalformat):
    """
Function which produces a 2 band output image with the X and Y locations of the image pixels.

Where:

:param inputImg: the input reference image
:param outputImg: the output image file name and path (will be same dimensions as the input)
:param gdalformat: the GDAL image file format of the output image file.

"""
    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image1 = inputImg
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    
    def _getXYPxlLocs(info, inputs, outputs, otherargs):
        """
        This is an internal rios function 
        """
        xBlock, yBlock = info.getBlockCoordArrays()
        outputs.outimage = numpy.stack((xBlock,yBlock))

    applier.apply(_getXYPxlLocs, infiles, outfiles, otherargs, controls=aControls)

def mergeExtractedHDF5Data(h5Files, outH5File, datatype=None):
    """
A function to merge a list of HDF files (e.g., from rsgislib.imageutils.extractZoneImageBandValues2HDF)
with the same number of variables (i.e., columns) into a single file. For example, if class training
regions have been sourced from multiple images. 

:param h5Files: a list of input files.
:param outH5File: the output file.
:param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

Example::

    inTrainSamples = ['MSS_CloudTrain1.h5', 'MSS_CloudTrain2.h5', 'MSS_CloudTrain3.h5']
    cloudTrainSamples = 'LandsatMSS_CloudTrainingSamples.h5'
    rsgislib.imageutils.mergeExtractedHDF5Data(inTrainSamples, cloudTrainSamples)

"""
    import h5py
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    first = True
    numVars = 0
    numVals = 0
    for h5File in h5Files:
        fH5 = h5py.File(h5File, 'r')
        dataShp = fH5['DATA/DATA'].shape
        if first:
            numVars = dataShp[1]
            first = False
        elif numVars is not dataShp[1]:
            raise rsgislib.RSGISPyException("The number of variables within the inputted HDF5 files was not the same.")
        numVals += dataShp[0]
        fH5.close()
    
    dataArr = numpy.zeros([numVals, numVars], dtype=float)
    
    rowInit = 0
    for h5File in h5Files:
        fH5 = h5py.File(h5File, 'r')
        numRows = fH5['DATA/DATA'].shape[0]
        dataArr[rowInit:(rowInit+numRows)] = fH5['DATA/DATA']
        rowInit += numRows
        fH5.close()

    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    fH5Out = h5py.File(outH5File, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=dataArr, chunks=(1000, numVars), compression="gzip",
                           shuffle=True, dtype=h5_dtype)
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'Merged'.encode()
    fH5Out.close()


def doImagesOverlap(image1, image2, overThres=0.0):
    """
Function to test whether two images overlap with one another.
If the images have a difference projection/coordinate system then corners 

:param image1: path to first image
:param image2: path to second image
:param overThres: the amount of overlap required to return true (e.g., at least 1 pixel)

:return: Boolean specifying whether they overlap or not.

Example::

    import rsgislib.imageutils
    img = "/Users/pete/Temp/LandsatStatsImgs/MSS/ClearSkyMsks/LS1MSS_19720823_lat52lon114_r24p218_osgb_clearsky.tif"
    tile = "/Users/pete/Temp/LandsatStatsImgs/MSS/RefImages/LandsatWalesRegion_60m_tile8.kea"
    
    overlap = rsgislib.imageutils.doImagesOverlap(tile, img)
    print("Images Overlap: " + str(overlap))

"""
    overlap = True
    
    projSame = False
    rsgisUtils = rsgislib.RSGISPyUtils()
    if rsgisUtils.doGDALLayersHaveSameProj(image1, image2):
        projSame = True
    
    img1DS = gdal.Open(image1, gdal.GA_ReadOnly)
    if img1DS is None:
        raise rsgislib.RSGISPyException('Could not open image: ' + image1)
        
    img2DS = gdal.Open(image2, gdal.GA_ReadOnly)
    if img2DS is None:
        raise rsgislib.RSGISPyException('Could not open image: ' + image2)

    img1GeoTransform = img1DS.GetGeoTransform()
    if img1GeoTransform is None:
        img1DS = None
        img2DS = None
        raise rsgislib.RSGISPyException('Could not get geotransform: ' + image1)
        
    img2GeoTransform = img2DS.GetGeoTransform()
    if img2GeoTransform is None:
        img1DS = None
        img2DS = None
        raise rsgislib.RSGISPyException('Could not get geotransform: ' + image2)
    
    img1TLX = img1GeoTransform[0]
    img1TLY = img1GeoTransform[3]
    
    img1BRX = img1GeoTransform[0] + (img1DS.RasterXSize * img1GeoTransform[1])
    img1BRY = img1GeoTransform[3] + (img1DS.RasterYSize * img1GeoTransform[5])
    
    img2TLX_orig = img2GeoTransform[0]
    img2TLY_orig = img2GeoTransform[3]
    
    img2BRX_orig = img2GeoTransform[0] + (img2DS.RasterXSize * img2GeoTransform[1])
    img2BRY_orig = img2GeoTransform[3] + (img2DS.RasterYSize * img2GeoTransform[5])
    
    img1EPSG = rsgisUtils.getEPSGCode(image1)
    img2EPSG = rsgisUtils.getEPSGCode(image2)
    
    if projSame:
        img2TLX = img2GeoTransform[0]
        img2TLY = img2GeoTransform[3]
        
        img2BRX = img2GeoTransform[0] + (img2DS.RasterXSize * img2GeoTransform[1])
        img2BRY = img2GeoTransform[3] + (img2DS.RasterYSize * img2GeoTransform[5])
    else:
        inProj = osr.SpatialReference()
        
        if img2EPSG is None:
            wktImg2 = rsgisUtils.getWKTProjFromImage(image2)
            if (wktImg2 is None) or (wktImg2 == ""):
                raise rsgislib.RSGISPyException('Could not retrieve EPSG or WKT for image: ' + image2)
            inProj.ImportFromWkt(wktImg2)
        else:
            inProj.ImportFromEPSG(int(img2EPSG))
        
        outProj = osr.SpatialReference()
        if img1EPSG is None:
            wktImg1 = rsgisUtils.getWKTProjFromImage(image1)
            if (wktImg1 is None) or (wktImg1 == ""):
                raise rsgislib.RSGISPyException('Could not retrieve EPSG or WKT for image: ' + image1)
            outProj.ImportFromWkt(wktImg1)
        else:
            outProj.ImportFromEPSG(int(img1EPSG))
        
        if img1EPSG is None:
            img1EPSG = 0

        img2TLX, img2TLY = rsgisUtils.reprojPoint(inProj, outProj, img2TLX_orig, img2TLY_orig)
        img2BRX, img2BRY = rsgisUtils.reprojPoint(inProj, outProj, img2BRX_orig, img2BRY_orig)
    
    xMin = img1TLX
    xMax = img1BRX
    yMin = img1BRY
    yMax = img1TLY
    
    if img2TLX > xMin:
        xMin = img2TLX
    if img2BRX < xMax:
        xMax = img2BRX
    if img2BRY > yMin:
        yMin = img2BRY
    if img2TLY < yMax:
        yMax = img2TLY
        
    if xMax - xMin <= overThres:
        overlap = False
    elif yMax - yMin <= overThres:
        overlap = False

    return overlap


def generateRandomPxlValsImg(inputImg, outputImg, gdalformat, lowVal, upVal):
    """
Function which produces a 1 band image with random values between lowVal and upVal.

Where:

:param inputImg: the input reference image
:param outputImg: the output image file name and path (will be same dimensions as the input)
:param gdalformat: the GDAL image file format of the output image file.
:param lowVal: lower value
:param upVal: upper value

"""
    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.inImg = inputImg
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    otherargs.lowVal = lowVal
    otherargs.upVal = upVal
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    
    def _popPxlsRanVals(info, inputs, outputs, otherargs):
        """
        This is an internal rios function for generateRandomPxlValsImg()
        """
        outputs.outimage = numpy.random.random_integers(otherargs.lowVal, high=otherargs.upVal, size=inputs.inImg.shape)
        outputs.outimage = outputs.outimage.astype(numpy.int32, copy=False)
    
    applier.apply(_popPxlsRanVals, infiles, outfiles, otherargs, controls=aControls)


def extractImgPxlSample(inputImg, pxlNSample, noData=None):
    """
A function which extracts a sample of pixels from the 
input image file to a number array.

:param inputImg: the image from which the random sample will be taken.
:param pxlNSample: the sample to be taken (e.g., a value of 100 will sample every 100th,
                   valid (if noData specified), pixel)
:param noData: provide a no data value which is to be ignored during processing. If None then ignored (Default: None)

:return: outputs a numpy array (n sampled values, n bands)

""" 
    # Import the RIOS image reader
    from rios.imagereader import ImageReader
    import tqdm

    first = True
    reader = ImageReader(inputImg, windowxsize=200, windowysize=200)
    for (info, block) in tqdm.tqdm(reader):
        blkShape = block.shape
        blkBands = block.reshape((blkShape[0], (blkShape[1]*blkShape[2])))
        
        blkBandsTrans = numpy.transpose(blkBands)
        
        if noData is not None:
            blkBandsTrans = blkBandsTrans[(blkBandsTrans!=noData).all(axis=1)]
        
        if blkBandsTrans.shape[0] > 0:
            nSamp = int((blkBandsTrans.shape[0])/pxlNSample)
            nSampRange = numpy.arange(0, nSamp, 1)*pxlNSample
            blkBandsTransSamp = blkBandsTrans[nSampRange]
            
            if first:
                outArr = blkBandsTransSamp
                first = False
            else:
                outArr = numpy.concatenate((outArr, blkBandsTransSamp), axis=0)
    return outArr


def extractImgPxlValsInMsk(img, img_bands, img_mask, img_mask_val, no_data=None):
    """
A function which extracts the image values within a mask for the specified image bands.

:param img: the image from which the random sample will be taken.
:param img_bands: the image bands the values are to be read from.
:param img_mask: the image mask specifying the regions of interest.
:param img_mask_val: the pixel value within the mask defining the region of interest.

:return: outputs a numpy array (n values, n bands)

"""
    # Import the RIOS image reader
    from rios.imagereader import ImageReader
    import tqdm

    outArr = None
    first = True
    reader = ImageReader([img, img_mask], windowxsize=200, windowysize=200)
    for (info, block) in tqdm.tqdm(reader):
        blk_img = block[0]
        blk_msk = block[1].flatten()
        blk_img_shape = blk_img.shape

        blk_bands = blk_img.reshape((blk_img_shape[0], (blk_img_shape[1] * blk_img_shape[2])))
        band_lst = []
        for band in img_bands:
            if (band > 0) and (band <= blk_bands.shape[0]):
                band_lst.append(blk_bands[band - 1])
            else:
                raise Exception("Band ({}) specified is not within the image".format(band))
        blk_bands_sel = numpy.stack(band_lst, axis=0)
        blk_bands_trans = numpy.transpose(blk_bands_sel)

        if no_data is not None:
            blk_msk = blk_msk[(blk_bands_trans != no_data).all(axis=1)]
            blk_bands_trans = blk_bands_trans[(blk_bands_trans != no_data).all(axis=1)]

        if blk_bands_trans.shape[0] > 0:
            blk_bands_trans = blk_bands_trans[blk_msk == img_mask_val]
            if first:
                out_arr = blk_bands_trans
                first = False
            else:
                out_arr = numpy.concatenate((out_arr, blk_bands_trans), axis=0)
    return out_arr


def extractChipZoneImageBandValues2HDF(inputImageInfo, imageMask, maskValue, chipSize, outputHDF,
                                       rotateChips=None, datatype=None):
    """
    A function which extracts a chip/window of image pixel values. The expectation is that
    this is used to train a classifer (see deep learning functions in classification) but it
    could be used to extract image 'chips' for other purposes.

    :param inputImageInfo: is a list of rsgislib.imageutils.ImageBandInfo objects specifying the input images and bands
    :param imageMask: is a single band input image to specify the regions of interest
    :param maskValue: is the pixel value within the imageMask to specify the region of interest
    :param chipSize: is the chip size .
    :param outputHDF: is the output HDF5 file. If it all ready exists then it is overwritten.
    :param rotateChips: specify whether you wish to have the image chips rotated during extraction to
                        increase the number of samples. Default is None and will therefore be ignored.
                        Otherwise, provide a list of rotation angles in degrees (e.g., [30, 60, 90, 120, 180])
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    # Import the RIOS image reader
    from rios.imagereader import ImageReader
    import h5py
    import tqdm
    import numpy
    import math
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    chip_size_odd = False
    if (chipSize % 2) != 0:
        chip_size_odd = True

    chipHSize = math.floor(chipSize / 2)

    rotate = False
    n_rotations = 0
    if rotateChips is not None:
        import scipy.ndimage
        img_win_h_size = math.floor(math.sqrt((chipHSize * chipHSize) + (chipHSize * chipHSize)))
        img_win_size = (img_win_h_size * 2)
        rotate = True
        n_rotations = len(rotateChips)
        minSub = img_win_h_size - chipHSize
        maxSub = img_win_size - minSub
    else:
        img_win_h_size = chipHSize
        img_win_size = chipSize

    ######################################################################
    # Count the number of features to extract so arrays can be initialised
    # at the correct size.
    ######################################################################
    nFeats = 0
    reader = ImageReader(imageMask, windowxsize=200, windowysize=200)
    for (info, block) in tqdm.tqdm(reader):
        nFeats = nFeats + numpy.sum(block[0] == maskValue)
    ######################################################################
    if rotate:
        nFeats = nFeats * (n_rotations + 1)
        print("There are {} pixel samples (inc. rotations) in the mask.".format(nFeats))
    else:
        print("There are {} pixel samples in the mask.".format(nFeats))

    ######################################################################
    # Initialise the numpy array for the feature data
    ######################################################################
    nBands = 0
    for inImgInfo in inputImageInfo:
        for band in inImgInfo.bands:
            nBands = nBands + 1
    featArr = numpy.zeros([nFeats, chipSize, chipSize, nBands], dtype=numpy.float32)
    sgl_feat_arr = numpy.zeros([nBands, chipSize, chipSize], dtype=numpy.float32)
    ######################################################################

    ######################################################################
    # Populate the feature arrays with the input data
    ######################################################################
    inImgs = list()
    inImgBands = list()

    inImgs.append(imageMask)
    inImgBands.append([1])

    for inImgInfo in inputImageInfo:
        inImgs.append(inImgInfo.fileName)
        inImgBands.append(inImgInfo.bands)
    nImgs = len(inputImageInfo)

    scnOverlap = img_win_h_size

    reader = ImageReader(inImgs, windowxsize=200, windowysize=200, overlap=scnOverlap, layerselection=inImgBands)
    iFeat = 0
    for (info, block) in tqdm.tqdm(reader):
        classMskArr = block[0]
        blkShape = classMskArr.shape

        if rotate:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - chipHSize
                yMax = y + chipHSize
                if chip_size_odd:
                    yMax += 1
                yMinExt = y - scnOverlap
                yMaxExt = y + scnOverlap
                if chip_size_odd:
                    yMax += 1
                for x in xRange:
                    xMin = x - chipHSize
                    xMax = x + chipHSize
                    if chip_size_odd:
                        xMax += 1
                    xMinExt = x - scnOverlap
                    xMaxExt = x + scnOverlap
                    if chip_size_odd:
                        xMaxExt += 1
                    if classMskArr[0][y][x] == maskValue:
                        # Rotation 0...
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 1][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(sgl_feat_arr[iBand], imgBlk[iBand], casting='safe')
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting='safe')
                        iFeat = iFeat + 1
                        # Iterate through rotation angles
                        for rotate_angle in rotateChips:
                            # Perform Rotate
                            sgl_feat_arr[...] = 0.0
                            for nImg in range(nImgs):
                                imgBlk = block[nImg + 1][..., yMinExt:yMaxExt, xMinExt:xMaxExt]
                                # Perform Rotate
                                imgBlkRot = scipy.ndimage.rotate(imgBlk, rotate_angle, axes=[1, 2], reshape=False,
                                                                 output=numpy.float32, mode='nearest')
                                for iBand in range(imgBlk.shape[0]):
                                    numpy.copyto(sgl_feat_arr[iBand], imgBlkRot[iBand, minSub:maxSub, minSub:maxSub],
                                                 casting='safe')
                            numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting='safe')
                            iFeat = iFeat + 1
        else:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - scnOverlap
                yMax = y + scnOverlap
                if chip_size_odd:
                    yMax += 1
                for x in xRange:
                    xMin = x - scnOverlap
                    xMax = x + scnOverlap
                    if chip_size_odd:
                        xMax += 1
                    if classMskArr[0][y][x] == maskValue:
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 1][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(sgl_feat_arr[iBand], imgBlk[iBand], casting='safe')
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting='safe')
                        iFeat = iFeat + 1
    ######################################################################

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    fH5Out = h5py.File(outputHDF, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    # Chunk size needs to be less than number of data points
    if nFeats < 250:
        chunkFeatures = nFeats
    else:
        chunkFeatures = 250
    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)
    dataGrp.create_dataset('DATA', data=featArr, chunks=(chunkFeatures, chipSize, chipSize, nBands),
                           compression="gzip", shuffle=True, dtype=h5_dtype)
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'IMAGE TILES'.encode()
    fH5Out.close()
    ######################################################################


def splitSampleChipHDF5File(input_h5_file, sample_h5_file, remain_h5_file, sample_size, rnd_seed, datatype=None):
    """
    A function to split the HDF5 outputs from the rsgislib.imageutils.extractChipZoneImageBandValues2HDF
    function into two sets by taking a random set with the defined sample size from the input file,
    saving the sample and the remainder to output HDF5 files.

    :param input_h5_file: The input HDF5 file to the split.
    :param sample_h5_file: The output HDF5 file with the sample outputted.
    :param remain_h5_file: The output HDF5 file with the remainder outputted.
    :param sample_size: An integer specifying the size of the sample to be taken.
    :param rnd_seed: An integer specifying the seed for the random number generator,
                     allowing the same 'random' sample to be taken.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import numpy
    import h5py
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, 'r')
    n_rows = f['DATA/DATA'].shape[0]
    chip_size = f['DATA/DATA'].shape[1]
    n_bands = f['DATA/DATA'].shape[3]
    f.close()

    if sample_size > n_rows:
        raise Exception("The requested sample is larger than the number samples in the input file.")

    rnd_obj = numpy.random.RandomState(rnd_seed)
    # Find sufficient unique sample indexes.
    smp_idxs = numpy.zeros(sample_size, dtype=int)
    while numpy.unique(smp_idxs).shape[0] != sample_size:
        tmp_idxs = rnd_obj.randint(0, n_rows, sample_size)
        tmp_uniq_idxs = numpy.unique(tmp_idxs)
        c_idx = 0
        if numpy.sum(smp_idxs) > 0:
            c_smp = smp_idxs[smp_idxs > 0]
            tmp_idxs = numpy.concatenate((c_smp, tmp_uniq_idxs))
            tmp_uniq_idxs = numpy.unique(tmp_idxs)

        max_idx = (sample_size - 1)
        if tmp_uniq_idxs.shape[0] < max_idx:
            max_idx = tmp_uniq_idxs.shape[0]
        smp_idxs[0:max_idx] = tmp_uniq_idxs[0:max_idx]
    smp_idxs = numpy.sort(smp_idxs)

    # Get the remaining indexes
    remain_idxs = numpy.arange(0, n_rows)
    remain_idxs = remain_idxs[numpy.isin(remain_idxs, smp_idxs, assume_unique=True, invert=True)]

    # Read the input HDF5 file.
    f = h5py.File(input_h5_file, 'r')
    in_samples = f['DATA/DATA']
    out_samples = in_samples[smp_idxs]
    remain_samples = in_samples[remain_idxs]
    f.close()

    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    # Create an output HDF5 file and populate with sample data.
    if sample_size < 250:
	    sample_chunks = sample_size
    else:
	    sample_chunks = 250
    fSampleH5Out = h5py.File(sample_h5_file, 'w')
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    dataSampleGrp.create_dataset('DATA', data=out_samples, chunks=(sample_chunks, chip_size, chip_size, n_bands),
                                 compression="gzip", shuffle=True, dtype=h5_dtype)
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = 'IMAGE TILES'.encode()
    fSampleH5Out.close()

    # Create an output HDF5 file and populate with remain data.
    if (n_rows - sample_size) < 250:
	    sample_chunks = (n_rows - sample_size)
    else:
	    sample_chunks = 250
    fSampleH5Out = h5py.File(remain_h5_file, 'w')
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    dataSampleGrp.create_dataset('DATA', data=remain_samples, chunks=(sample_chunks, chip_size, chip_size, n_bands),
                                 compression="gzip", shuffle=True, dtype=h5_dtype)
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = 'IMAGE TILES'.encode()
    fSampleH5Out.close()


def mergeExtractedHDF5ChipData(h5Files, outH5File, datatype=None):
    """
A function to merge a list of HDF files
(e.g., from rsgislib.imageutils.extractChipZoneImageBandValues2HDF)
with the same number of variables (i.e., image bands) and chip size into
a single file. For example, if class training regions have been sourced
from multiple images.

:param h5Files: a list of input files.
:param outH5File: the output file.
:param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

Example::

    inTrainSamples = ['MSS_CloudTrain1.h5', 'MSS_CloudTrain2.h5', 'MSS_CloudTrain3.h5']
    cloudTrainSamples = 'LandsatMSS_CloudTrainingSamples.h5'
    rsgislib.imageutils.mergeExtractedHDF5ChipData(inTrainSamples, cloudTrainSamples)

"""
    import h5py
    import numpy
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    first = True
    n_feats = 0
    chip_size = 0
    n_bands = 0
    for h5File in h5Files:
        fH5 = h5py.File(h5File, 'r')
        data_shp = fH5['DATA/DATA'].shape
        if first:
            n_bands = data_shp[3]
            chip_size = data_shp[1]
            first = False
        else:
            if n_bands != data_shp[3]:
                raise rsgislib.RSGISPyException(
                    "The number of bands (variables) within the inputted HDF5 files was not the same.")
            if chip_size != data_shp[1]:
                raise rsgislib.RSGISPyException("The chip size within the inputted HDF5 files was not the same.")
        n_feats += data_shp[0]
        fH5.close()

    feat_arr = numpy.zeros([n_feats, chip_size, chip_size, n_bands], dtype=numpy.float32)

    row_init = 0
    for h5File in h5Files:
        fH5 = h5py.File(h5File, 'r')
        n_rows = fH5['DATA/DATA'].shape[0]
        feat_arr[row_init:(row_init + n_rows)] = fH5['DATA/DATA']
        row_init += n_rows
        fH5.close()

    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    fH5Out = h5py.File(outH5File, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=feat_arr, chunks=(250, chip_size, chip_size, n_bands),
                           compression="gzip", shuffle=True, dtype=h5_dtype)
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'Merged'.encode()
    fH5Out.close()


def extractRefChipZoneImageBandValues2HDF(inputImageInfo, refImg, refImgBand, imageMask, maskValue, chipSize, outputHDF,
                                          rotateChips=None, datatype=None):
    """
A function which extracts a chip/window of image pixel values. The expectation is that
this is used to train a classifer (see deep learning functions in classification) but it
could be used to extract image 'chips' for other purposes.

:param inputImageInfo: is a list of rsgislib.imageutils.ImageBandInfo objects specifying the input images and bands
:param refImg: is an image file (same pixel size and projection as the other input images)
               which is used as the class training
:param refImgBand: is the image band in the reference image to be used (only a single reference band can be used).
:param imageMask: is a single band input image to specify the regions of interest
:param maskValue: is the pixel value within the imageMask to specify the region of interest
:param chipSize: is the chip size .
:param outputHDF: is the output HDF5 file. If it all ready exists then it is overwritten.
:param rotateChips: specify whether you wish to have the image chips rotated during extraction to
                    increase the number of samples. Default is None and will therefore be ignored.
                    Otherwise, provide a list of rotation angles in degrees (e.g., [30, 60, 90, 120, 180])
:param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

"""
    # Import the RIOS image reader
    from rios.imagereader import ImageReader
    import h5py
    import tqdm
    import numpy
    import math
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    if (chipSize % 2) != 0:
        raise Exception("The chip size must be an even number.")

    chipHSize = math.floor(chipSize / 2)

    rotate = False
    n_rotations = 0
    if rotateChips is not None:
        import scipy.ndimage
        img_win_h_size = math.floor(math.sqrt((chipHSize * chipHSize) + (chipHSize * chipHSize)))
        img_win_size = (img_win_h_size * 2)
        rotate = True
        n_rotations = len(rotateChips)
        minSub = img_win_h_size - chipHSize
        maxSub = img_win_size - minSub
    else:
        img_win_h_size = chipHSize
        img_win_size = chipSize

    ######################################################################
    # Count the number of features to extract so arrays can be initialised
    # at the correct size.
    ######################################################################
    nFeats = 0
    reader = ImageReader(imageMask, windowxsize=200, windowysize=200)
    for (info, block) in tqdm.tqdm(reader):
        nFeats = nFeats + numpy.sum(block[0] == maskValue)
    ######################################################################
    if rotate:
        nFeats = nFeats * (n_rotations + 1)
        print("There are {} pixel samples (inc. rotations) in the mask.".format(nFeats))
    else:
        print("There are {} pixel samples in the mask.".format(nFeats))

    ######################################################################
    # Initialise the numpy array for the feature data
    ######################################################################
    nBands = 0
    for inImgInfo in inputImageInfo:
        for band in inImgInfo.bands:
            nBands = nBands + 1
    featArr = numpy.zeros([nFeats, chipSize, chipSize, nBands],
                          dtype=numpy.float32)  # [nFeats, chipSize, chipSize, nBands]
    sgl_feat_arr = numpy.zeros([nBands, chipSize, chipSize], dtype=numpy.float32)  # [chipSize, chipSize, nBands]
    featRefArr = numpy.zeros([nFeats, chipSize, chipSize], dtype=numpy.uint16)
    ######################################################################

    ######################################################################
    # Populate the feature arrays with the input data
    ######################################################################
    inImgs = list()
    inImgBands = list()

    inImgs.append(imageMask)
    inImgBands.append([1])
    inImgs.append(refImg)
    inImgBands.append([refImgBand])

    for inImgInfo in inputImageInfo:
        inImgs.append(inImgInfo.fileName)
        inImgBands.append(inImgInfo.bands)
    nImgs = len(inputImageInfo)

    scnOverlap = img_win_h_size

    reader = ImageReader(inImgs, windowxsize=200, windowysize=200, overlap=scnOverlap, layerselection=inImgBands)
    iFeat = 0
    for (info, block) in tqdm.tqdm(reader):
        classMskArr = block[0]
        blkShape = classMskArr.shape

        if rotate:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - chipHSize
                yMax = y + chipHSize
                yMinExt = y - scnOverlap
                yMaxExt = y + scnOverlap
                for x in xRange:
                    xMin = x - chipHSize
                    xMax = x + chipHSize
                    xMinExt = x - scnOverlap
                    xMaxExt = x + scnOverlap
                    if classMskArr[0][y][x] == maskValue:
                        # Rotation 0...
                        refImgBlk = block[1][0, yMin:yMax, xMin:xMax]
                        numpy.copyto(featRefArr[iFeat], refImgBlk, casting='safe')
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 2][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(sgl_feat_arr[iBand], imgBlk[iBand], casting='safe')
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting='safe')
                        iFeat = iFeat + 1
                        # Iterate through rotation angles
                        for rotate_angle in rotateChips:
                            refImgBlk = block[1][0, yMinExt:yMaxExt, xMinExt:xMaxExt]
                            # Perform Rotate
                            refImgBlkRot = scipy.ndimage.rotate(refImgBlk, rotate_angle, reshape=False,
                                                                output=numpy.uint16, order=0, mode='nearest')
                            numpy.copyto(featRefArr[iFeat], refImgBlkRot[minSub:maxSub, minSub:maxSub], casting='safe')
                            sgl_feat_arr[...] = 0.0
                            for nImg in range(nImgs):
                                imgBlk = block[nImg + 2][..., yMinExt:yMaxExt, xMinExt:xMaxExt]
                                # Perform Rotate
                                imgBlkRot = scipy.ndimage.rotate(imgBlk, rotate_angle, axes=[1, 2], reshape=False,
                                                                 output=numpy.float32, mode='nearest')
                                for iBand in range(imgBlk.shape[0]):
                                    numpy.copyto(sgl_feat_arr[iBand], imgBlkRot[iBand, minSub:maxSub, minSub:maxSub],
                                                 casting='safe')
                            numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting='safe')
                            iFeat = iFeat + 1
        else:
            xSize = blkShape[2] - (scnOverlap * 2)
            ySize = blkShape[1] - (scnOverlap * 2)
            xRange = numpy.arange(scnOverlap, scnOverlap + xSize, 1)
            yRange = numpy.arange(scnOverlap, scnOverlap + ySize, 1)

            for y in yRange:
                yMin = y - scnOverlap
                yMax = y + scnOverlap
                for x in xRange:
                    xMin = x - scnOverlap
                    xMax = x + scnOverlap
                    if classMskArr[0][y][x] == maskValue:
                        refImgBlk = block[1][0, yMin:yMax, xMin:xMax]
                        numpy.copyto(featRefArr[iFeat], refImgBlk, casting='safe')
                        sgl_feat_arr[...] = 0.0
                        for nImg in range(nImgs):
                            imgBlk = block[nImg + 2][..., yMin:yMax, xMin:xMax]
                            for iBand in range(imgBlk.shape[0]):
                                numpy.copyto(sgl_feat_arr[iBand], imgBlk[iBand], casting='safe')
                        numpy.copyto(featArr[iFeat], sgl_feat_arr.T, casting='safe')
                        iFeat = iFeat + 1
    ######################################################################

    ######################################################################
    # Create the output HDF5 file and populate with data.
    ######################################################################
    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    fH5Out = h5py.File(outputHDF, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    # Chunk size needs to be less than number of data points
    if nFeats < 250:
        chunkFeatures = nFeats
    else:
        chunkFeatures = 250
    dataGrp.create_dataset('DATA', data=featArr, chunks=(chunkFeatures, chipSize, chipSize, nBands),
                           compression="gzip", shuffle=True, dtype=h5_dtype)
    dataGrp.create_dataset('REF', data=featRefArr, chunks=(chunkFeatures, chipSize, chipSize),
                           compression="gzip", shuffle=True, dtype='H')
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'IMAGE REF TILES'.encode()
    fH5Out.close()
    ######################################################################


def splitSampleRefChipHDF5File(input_h5_file, sample_h5_file, remain_h5_file, sample_size, rnd_seed, datatype=None):
    """
    A function to split the HDF5 outputs from the rsgislib.imageutils.extractChipZoneImageBandValues2HDF
    function into two sets by taking a random set with the defined sample size from the input file,
    saving the sample and the remainder to output HDF5 files.

    :param input_h5_file: The input HDF5 file to the split.
    :param sample_h5_file: The output HDF5 file with the sample outputted.
    :param remain_h5_file: The output HDF5 file with the remainder outputted.
    :param sample_size: An integer specifying the size of the sample to be taken.
    :param rnd_seed: An integer specifying the seed for the random number generator,
                     allowing the same 'random' sample to be taken.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

    """
    import numpy
    import h5py
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    f = h5py.File(input_h5_file, 'r')
    n_rows = f['DATA/REF'].shape[0]
    chip_size = f['DATA/REF'].shape[1]
    n_bands = f['DATA/REF'].shape[3]
    f.close()

    if sample_size > n_rows:
        raise Exception("The requested sample is larger than the number samples in the input file.")

    rnd_obj = numpy.random.RandomState(rnd_seed)
    # Find sufficient unique sample indexes.
    smp_idxs = numpy.zeros(sample_size, dtype=int)
    while numpy.unique(smp_idxs).shape[0] != sample_size:
        tmp_idxs = rnd_obj.randint(0, n_rows, sample_size)
        tmp_uniq_idxs = numpy.unique(tmp_idxs)
        c_idx = 0
        if numpy.sum(smp_idxs) > 0:
            c_smp = smp_idxs[smp_idxs > 0]
            tmp_idxs = numpy.concatenate((c_smp, tmp_uniq_idxs))
            tmp_uniq_idxs = numpy.unique(tmp_idxs)

        max_idx = (sample_size - 1)
        if tmp_uniq_idxs.shape[0] < max_idx:
            max_idx = tmp_uniq_idxs.shape[0]
        smp_idxs[0:max_idx] = tmp_uniq_idxs[0:max_idx]
    smp_idxs = numpy.sort(smp_idxs)

    # Get the remaining indexes
    remain_idxs = numpy.arange(0, n_rows)
    remain_idxs = remain_idxs[numpy.isin(remain_idxs, smp_idxs, assume_unique=True, invert=True)]

    # Read the input HDF5 file.
    f = h5py.File(input_h5_file, 'r')
    in_data_samples = f['DATA/DATA']
    out_data_samples = in_data_samples[smp_idxs]
    remain_data_samples = in_data_samples[remain_idxs]
    in_ref_samples = f['DATA/REF']
    out_ref_samples = in_ref_samples[smp_idxs]
    remain_ref_samples = in_ref_samples[remain_idxs]
    f.close()

    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    # Create an output HDF5 file and populate with sample data.
    fSampleH5Out = h5py.File(sample_h5_file, 'w')
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    dataSampleGrp.create_dataset('DATA', data=out_data_samples, chunks=(250, chip_size, chip_size, n_bands),
                                 compression="gzip", shuffle=True, dtype=h5_dtype)
    dataSampleGrp.create_dataset('REF', data=out_ref_samples, chunks=(250, chip_size, chip_size),
                                 compression="gzip", shuffle=True, dtype='H')
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = 'IMAGE REF TILES'.encode()
    fSampleH5Out.close()

    # Create an output HDF5 file and populate with remain data.
    fSampleH5Out = h5py.File(remain_h5_file, 'w')
    dataSampleGrp = fSampleH5Out.create_group("DATA")
    metaSampleGrp = fSampleH5Out.create_group("META-DATA")
    dataSampleGrp.create_dataset('DATA', data=remain_data_samples, chunks=(250, chip_size, chip_size, n_bands),
                                 compression="gzip", shuffle=True, dtype=h5_dtype)
    dataSampleGrp.create_dataset('REF', data=remain_ref_samples, chunks=(250, chip_size, chip_size),
                                 compression="gzip", shuffle=True, dtype='H')
    describSampleDS = metaSampleGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describSampleDS[0] = 'IMAGE REF TILES'.encode()
    fSampleH5Out.close()


def mergeExtractedHDF5ChipRefData(h5Files, outH5File, datatype=None):
    """
A function to merge a list of HDF files
(e.g., from rsgislib.imageutils.extractRefChipZoneImageBandValues2HDF)
with the same number of variables (i.e., image bands) and chip size into
a single file. For example, if class training regions have been sourced
from multiple images.

:param h5Files: a list of input files.
:param outH5File: the output file.
:param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.

Example::

    inTrainSamples = ['MSS_CloudTrain1.h5', 'MSS_CloudTrain2.h5', 'MSS_CloudTrain3.h5']
    cloudTrainSamples = 'LandsatMSS_CloudTrainingSamples.h5'
    rsgislib.imageutils.mergeExtractedHDF5ChipRefData(inTrainSamples, cloudTrainSamples)

"""
    import h5py
    import numpy
    import rsgislib

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT

    first = True
    n_feats = 0
    chip_size = 0
    n_bands = 0
    for h5File in h5Files:
        fH5 = h5py.File(h5File, 'r')
        data_shp = fH5['DATA/DATA'].shape
        if first:
            n_bands = data_shp[3]
            chip_size = data_shp[1]
            first = False
        else:
            if n_bands != data_shp[3]:
                raise rsgislib.RSGISPyException(
                    "The number of bands (variables) within the inputted HDF5 files was not the same.")
            if chip_size != data_shp[1]:
                raise rsgislib.RSGISPyException("The chip size within the inputted HDF5 files was not the same.")
        n_feats += data_shp[0]
        fH5.close()

    feat_arr = numpy.zeros([n_feats, chip_size, chip_size, n_bands], dtype=numpy.float32)
    feat_ref_arr = numpy.zeros([n_feats, chip_size, chip_size], dtype=numpy.uint16)

    row_init = 0
    for h5File in h5Files:
        fH5 = h5py.File(h5File, 'r')
        n_rows = fH5['DATA/DATA'].shape[0]
        feat_arr[row_init:(row_init + n_rows)] = fH5['DATA/DATA']
        feat_ref_arr[row_init:(row_init + n_rows)] = fH5['DATA/REF']
        row_init += n_rows
        fH5.close()

    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    fH5Out = h5py.File(outH5File, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=feat_arr, chunks=(250, chip_size, chip_size, n_bands),
                           compression="gzip", shuffle=True, dtype=h5_dtype)
    dataGrp.create_dataset('REF', data=feat_ref_arr, chunks=(250, chip_size, chip_size),
                           compression="gzip", shuffle=True, dtype='H')
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'Merged'.encode()
    fH5Out.close()


def msk_h5_smpls_to_finite_values(input_h5, output_h5, datatype=None, lower_limit=None, upper_limit=None):
    """
    A function to remove values from a HDF5 sample file which are not finite. Upper and lower values
    can also be specified.

    :param input_h5: Input HDF5 file.
    :param output_h5: Output HDF5 file.
    :param datatype: is the data type used for the output HDF5 file (e.g., rsgislib.TYPE_32FLOAT). If None (default)
                     then the output data type will be float32.
    :param lower_limit: Optional lower value threshold (if None then not used).
    :param upper_limit: Optional upper value threshold (if None then not used).

    """
    import h5py
    import numpy

    rsgis_utils = rsgislib.RSGISPyUtils()
    if datatype is None:
        datatype = rsgislib.TYPE_32FLOAT
    h5_dtype = rsgis_utils.getNumpyCharCodesDataType(datatype)

    fH5 = h5py.File(input_h5, 'r')
    data_shp = fH5['DATA/DATA'].shape
    num_vars = data_shp[1]
    data = numpy.array(fH5['DATA/DATA'])
    data = data[numpy.isfinite(data).all(axis=1)]
    if lower_limit is not None:
        data = data[numpy.any(data > lower_limit, axis=1)]
    if upper_limit is not None:
        data = data[numpy.any(data < upper_limit, axis=1)]

    n_samples = data.shape[0]
    chunk_size = 1000
    if n_samples < 1000:
        chunk_size = n_samples

    fH5Out = h5py.File(output_h5, 'w')
    dataGrp = fH5Out.create_group("DATA")
    metaGrp = fH5Out.create_group("META-DATA")
    dataGrp.create_dataset('DATA', data=data, chunks=(chunk_size, num_vars), compression="gzip",
                           shuffle=True, dtype=h5_dtype)
    describDS = metaGrp.create_dataset("DESCRIPTION", (1,), dtype="S10")
    describDS[0] = 'finite values'.encode()
    fH5Out.close()


def getUniqueValues(img, img_band=1):
    """
Find the unique image values within an image band.
Note, the whole image band gets read into memory.

:param img: input image file path
:param img_band: image band to be processed (starts at 1)

:return: array of unique values.

"""
    imgDS = gdal.Open(img)
    if imgDS is None:
        raise Exception("Could not open output image")
    imgBand = imgDS.GetRasterBand(img_band)
    if imgBand is None:
        raise Exception("Could not open output image band ({})".format(img_band))
    valsArr = imgBand.ReadAsArray()
    imgDS = None
    
    uniq_vals = numpy.unique(valsArr)

    return uniq_vals


def combineBinaryMasks(msk_imgs_dict, out_img, output_lut, gdalformat='KEA'):
    """
A function which combines up to 8 binary image masks to create a single 
output image with a unique value for each combination of intersecting 
masks. A JSON LUT is also generated to identify the image values to a
'class'.

:param msk_imgs_dict: dict of input images.
:param out_img: output image file.
:param output_lut: output file path to JSON LUT file identifying the image values.
:param gdalformat: output GDAL format (e.g., KEA)

""" 
    import json
    rsgis_utils = rsgislib.RSGISPyUtils()

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    in_vals_dict = dict()
    msk_imgs = list()
    for key in msk_imgs_dict.keys():
        msk_imgs.append(msk_imgs_dict[key])
        in_vals_dict[key] = [0,1]
    
    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.msk_imgs = msk_imgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = out_img
    otherargs = applier.OtherInputs()
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False
    
    def _combineMsks(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.msk_imgs[0], dtype=numpy.uint8)        
        out_bit_arr = numpy.unpackbits(out_arr, axis=2)
        img_n = 0
        for img in inputs.msk_imgs:
            for x in range(img.shape[1]):
                for y in range(img.shape[2]):
                    if img[0,x,y] > 1:
                        out_bit_arr[0,x,(8*y)+img_n] = 1
            img_n = img_n + 1
        
        out_arr = numpy.packbits(out_bit_arr, axis=2)
        
        outputs.outimage = out_arr
    applier.apply(_combineMsks, infiles, outfiles, otherargs, controls=aControls)
    
    # find the unique output image files.
    uniq_vals = getUniqueValues(out_img, img_band=1)
    
    # find the powerset of the inputs
    possible_outputs = rsgis_utils.createVarList(in_vals_dict, val_dict=None)
    
    out_poss_lut = dict()
    for poss in possible_outputs:
        val = numpy.zeros(1, dtype=numpy.uint8)
        val_bit_arr = numpy.unpackbits(val, axis=0)
        i = 0
        for key in msk_imgs_dict.keys():
            val_bit_arr[i] = poss[key]
            i = i + 1
        out_arr = numpy.packbits(val_bit_arr)
        if out_arr[0] in uniq_vals:
            out_poss_lut[str(out_arr[0])] = poss
        
    with open(output_lut, 'w') as outJSONfile:
        json.dump(out_poss_lut, outJSONfile, sort_keys=True,indent=4, separators=(',', ': '), ensure_ascii=False)


def gdal_translate(input_img, output_img, gdal_format='KEA', options=''):
    """
    Using GDAL translate to convert input image to a different format, if GTIFF selected
    and no options are provided then a cloud optimised GeoTIFF will be outputted.

    :param input_img: Input image which is GDAL readable.
    :param output_img: The output image file.
    :param gdal_format: The output image file format
    :param options: options for the output driver (e.g., "-co TILED=YES -co COMPRESS=LZW -co BIGTIFF=YES")
    """
    if (gdal_format == 'GTIFF') and (options != ''):
        options = "-co TILED=YES -co INTERLEAVE=PIXEL -co BLOCKXSIZE=256 -co BLOCKYSIZE=256 -co COMPRESS=LZW -co BIGTIFF=YES -co COPY_SRC_OVERVIEWS=YES"

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    trans_opt = gdal.TranslateOptions(format=gdal_format, options=options, callback=callback)
    gdal.Translate(output_img, input_img, options=trans_opt)


def gdal_stack_images_vrt(input_imgs, output_vrt_file):
    """
    A function which creates a GDAL VRT file from a set of input images by stacking the input images
    in a multi-band output file.

    :param input_imgs: A list of input images
    :param output_vrt_file: The output file location for the VRT.
    """
    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    build_vrt_opt = gdal.BuildVRTOptions(separate=True, callback=callback)
    gdal.BuildVRT(output_vrt_file, input_imgs, options=build_vrt_opt)


def gdal_mosaic_images_vrt(input_imgs, output_vrt_file, vrt_extent=None):
    """
    A function which creates a GDAL VRT file from a set of input images by mosaicking
    the input images.

    :param input_imgs: A list of input images
    :param output_vrt_file: The output file location for the VRT.
    :param vrt_extent: An optional (If None then ignored) extent (minX, minY, maxX, maxY)
                       for the VRT image.
    """
    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
    if vrt_extent is not None:
        build_vrt_opt = gdal.BuildVRTOptions(outputBounds=vrt_extent, callback=callback)
    else:
        build_vrt_opt = gdal.BuildVRTOptions(callback=callback)
    gdal.BuildVRT(output_vrt_file, input_imgs, options=build_vrt_opt)


def subset_to_vec(in_img, out_img, gdalformat, roi_vec_file, roi_vec_lyr, datatype=None, vec_epsg=None):
    """
    A function which subsets an input image using the extent of a vector layer where the
    input vector can be a different projection to the input image. Reprojection will be handled.

    :param in_img: Input Image file.
    :param out_img: Output Image file.
    :param gdalformat: Output image file format.
    :param roi_vec_file: The input vector file.
    :param roi_vec_lyr: The name of the input layer.
    :param datatype: Output image data type. If None then the datatype of the input image will be used.
    :param vec_epsg: If projection is poorly defined by the vector layer then it can be specified.
    """
    rsgis_utils = rsgislib.RSGISPyUtils()
    if vec_epsg is None:
        vec_epsg = rsgis_utils.getProjEPSGFromVec(roi_vec_file, roi_vec_lyr)
    img_epsg = rsgis_utils.getEPSGCode(in_img)
    if img_epsg == vec_epsg:

        projs_match = True
    else:
        img_bbox = rsgis_utils.getImageBBOXInProj(in_img, vec_epsg)
        projs_match = False
    img_bbox = rsgis_utils.getImageBBOX(in_img)
    vec_bbox = rsgis_utils.getVecLayerExtent(roi_vec_file, roi_vec_lyr, computeIfExp=True)
    if img_epsg != vec_epsg:
        vec_bbox = rsgis_utils.reprojBBOX_epsg(vec_bbox, vec_epsg, img_epsg)

    if rsgis_utils.do_bboxes_intersect(img_bbox, vec_bbox):
        common_bbox = rsgis_utils.bbox_intersection(img_bbox, vec_bbox)
        if datatype == None:
            datatype = rsgis_utils.getRSGISLibDataTypeFromImg(in_img)
        rsgislib.imageutils.subsetbbox(in_img, out_img, gdalformat, datatype, common_bbox[0], common_bbox[1],
                                       common_bbox[2], common_bbox[3])
    else:
        raise Exception("The image and vector do not intersect and therefore the image cannot be subset.")


def mask_img_with_vec(input_img, output_img, gdalformat, roi_vec_file, roi_vec_lyr, tmp_dir, outvalue=0, datatype=None,
                      vec_epsg=None):
    """
    This function masks the input image using a polygon vector file.

    :param input_img: Input Image file.
    :param output_img: Output Image file.
    :param gdalformat: Output image file format.
    :param roi_vec_file: The input vector file.
    :param roi_vec_lyr: The name of the input layer.
    :param tmp_dir: a temporary directory for files generated during processing.
    :param outvalue: The output value in the regions masked.
    :param datatype: Output image data type. If None then the datatype of the input image will be used.
    :param vec_epsg: If projection is poorly defined by the vector layer then it can be specified.

    """
    import rsgislib.vectorutils
    rsgis_utils = rsgislib.RSGISPyUtils()

    # Does the input image BBOX intersect the BBOX of the ROI vector?
    if vec_epsg is None:
        vec_epsg = rsgis_utils.getProjEPSGFromVec(roi_vec_file, roi_vec_lyr)
    img_epsg = rsgis_utils.getEPSGCode(input_img)
    if img_epsg == vec_epsg:
        img_bbox = rsgis_utils.getImageBBOX(input_img)
        projs_match = True
    else:
        img_bbox = rsgis_utils.getImageBBOXInProj(input_img, vec_epsg)
        projs_match = False
    vec_bbox = rsgis_utils.getVecLayerExtent(roi_vec_file, roi_vec_lyr, computeIfExp=True)

    if rsgis_utils.do_bboxes_intersect(img_bbox, vec_bbox):
        uid_str = rsgis_utils.uidGenerator()
        base_vmsk_img = rsgis_utils.get_file_basename(input_img)

        tmp_file_dir = os.path.join(tmp_dir, "{}_{}".format(base_vmsk_img, uid_str))
        if not os.path.exists(tmp_file_dir):
            os.mkdir(tmp_file_dir)

        # Rasterise the vector layer to the input image extent.
        mem_ds, mem_lyr = rsgislib.vectorutils.getMemVecLyrSubset(roi_vec_file, roi_vec_lyr, img_bbox)

        if not projs_match:
            mem_result_ds, mem_result_lyr = rsgislib.vectorutils.reproj_vec_lyr(mem_lyr, 'mem_vec', img_epsg,
                                                                                out_vec_drv='MEMORY', out_lyr_name=None,
                                                                                in_epsg=None, print_feedback=True)
            mem_ds = None
        else:
            mem_result_ds = mem_ds
            mem_result_lyr = mem_lyr

        roi_img = os.path.join(tmp_file_dir, "{}_roiimg.kea".format(base_vmsk_img))
        rsgislib.imageutils.createCopyImage(input_img, roi_img, 1, 0, 'KEA', rsgislib.TYPE_8UINT)
        rsgislib.vectorutils.rasteriseVecLyrObj(mem_result_lyr, roi_img, burnVal=1, vecAtt=None, calcstats=True,
                                                thematic=True, nodata=0)
        mem_result_ds = None

        if datatype == None:
            datatype = rsgis_utils.getRSGISLibDataTypeFromImg(input_img)
        rsgislib.imageutils.maskImage(input_img, roi_img, output_img, gdalformat, datatype, outvalue, 0)
        shutil.rmtree(tmp_file_dir)
    else:
        raise Exception("The vector file and image file do not intersect.")


def create_valid_mask(imgBandInfo, out_msk_file, gdalformat, tmpdir):
    """
    A function to create a single valid mask from the intersection of the valid masks for all the input
    images.

    :param imgBandInfo: A list of rsgislib.imageutils.ImageBandInfo objects to define the images and and bands of interest.
    :param out_msk_file: A output image file and path
    :param gdalformat: The output file format.
    :param tmpdir: A directory for temporary outputs created during the processing.

    """
    rsgis_utils = rsgislib.RSGISPyUtils()
    if len(imgBandInfo) == 1:
        no_data_val = rsgis_utils.getImageNoDataValue(imgBandInfo[0].fileName)
        rsgislib.imageutils.genValidMask(imgBandInfo[0].fileName, out_msk_file, gdalformat, no_data_val)
    else:
        uid_str = rsgis_utils.uidGenerator()
        tmp_lcl_dir = os.path.join(tmpdir, "create_valid_mask_{}".format(uid_str))
        if not os.path.exists(tmp_lcl_dir):
            os.makedirs(tmp_lcl_dir)

        validMasks = []
        for imgInfo in imgBandInfo:
            tmpBaseName = rsgis_utils.get_file_basename(imgInfo.fileName)
            vdmskFile = os.path.join(tmp_lcl_dir, '{}_vmsk.kea'.format(tmpBaseName))
            no_data_val = rsgis_utils.getImageNoDataValue(imgInfo.fileName)
            rsgislib.imageutils.genValidMask(imgInfo.fileName, vdmskFile, gdalformat='KEA', nodata=no_data_val)
            validMasks.append(vdmskFile)

        rsgislib.imageutils.genValidMask(validMasks, out_msk_file, gdalformat, nodata=0.0)
        shutil.rmtree(tmp_lcl_dir)


def get_image_pxl_values(image, band, x_coords, y_coords):
    """
    Function which gets pixel values from a image for specified
    image pixels. The coordinate space is image pixels, i.e.,
    (0 - xSize) and (0 - ySize).

    :param image: The input image name and path
    :param band: The band within the input image.
    :param x_coords: A numpy array of image X coordinates (in the image pixel coordinates)
    :param y_coords: A numpy array of image Y coordinates (in the image pixel coordinates)
    :return: An array of image pixel values.
    
    """
    from osgeo import gdal
    import tqdm
    import numpy

    if x_coords.shape[0] != y_coords.shape[0]:
        raise Exception("The X and Y image coordinates are not the same.")

    image_ds = gdal.Open(image, gdal.GA_Update)
    if image_ds is None:
        raise Exception("Could not open the input image file: '{}'".format(image))
    image_band = image_ds.GetRasterBand(band)
    if image_band is None:
        raise Exception("The image band wasn't opened")

    out_pxl_vals = numpy.zeros(x_coords.shape[0], dtype=float)

    img_data = image_band.ReadAsArray()
    for i in tqdm.tqdm(range(x_coords.shape[0])):
        out_pxl_vals[i] = img_data[y_coords[i], x_coords[i]]
    image_ds = None
    return out_pxl_vals


def set_image_pxl_values(image, band, x_coords, y_coords, pxl_value=1):
    """
    A function which sets defined image pixels to a value.
    The coordinate space is image pixels, i.e.,
    (0 - xSize) and (0 - ySize).

    :param image: The input image name and path
    :param band: The band within the input image.
    :param x_coords: A numpy array of image X coordinates (in the image pixel coordinates)
    :param y_coords: A numpy array of image Y coordinates (in the image pixel coordinates)
    :param pxl_value: The value to set the image pixel to (specified by the x/y coordinates)

    """
    from osgeo import gdal
    import tqdm

    if x_coords.shape[0] != y_coords.shape[0]:
        raise Exception("The X and Y image coordinates are not the same.")

    image_ds = gdal.Open(image, gdal.GA_Update)
    if image_ds is None:
        raise Exception("Could not open the input image file: '{}'".format(image))
    image_band = image_ds.GetRasterBand(band)
    if image_band is None:
        raise Exception("The image band wasn't opened")

    img_data = image_band.ReadAsArray()
    for i in tqdm.tqdm(range(x_coords.shape[0])):
        img_data[y_coords[i], x_coords[i]] = pxl_value
    image_band.WriteArray(img_data)
    image_ds = None


def assign_random_pxls(input_img, output_img, n_pts, img_band=1, gdalformat='KEA', edge_pxl=0, use_no_data=True,
                       seed=None):
    """
    A function which can generate a set of random pixels. Can honor the image no data value
    and use an edge buffer so pixels are not identified near the image edge.

    :param input_img: The input image providing the reference area and no data value.
    :param output_img: The output image with the random pixels.
    :param n_pts: The number of pixels to be sampled.
    :param img_band: The image band from the input image used for the no data value.
    :param gdalformat: The file format of the output image.
    :param edge_pxl: The edge pixel buffer, in pixels. This is a buffer around the edge of
                     the image within which pixels will not be identified. (Default: 0)
    :param use_no_data: A boolean specifying whether the image no data value should be used. (Default: True)
    :param seed: A random seed for generating the pixel locations. If None then a different
                 seed is used each time the system is executed. (Default None)

    Example::

        input_img = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_clouds_up.kea'
        output_img = 'LS5TM_20000108_latn531lonw37_r23p204_osgb_samples.kea'
        n_pts = 5000

        assign_random_pxls(input_img, output_img, n_pts, img_band=1, gdalformat='KEA')
        # Calculate the image stats and pyramids for display
        import rsgislib.rastergis
        rsgislib.rastergis.populateStats(output_img, True, True, True)

    """
    import numpy
    import numpy.random

    if seed is not None:
        numpy.random.seed(seed)

    if edge_pxl < 0:
        raise Exception("edge_pxl value must be greater than 0.")

    rsgis_utils = rsgislib.RSGISPyUtils()
    xSize, ySize = rsgis_utils.getImageSize(input_img)

    x_min = edge_pxl
    x_max = xSize - edge_pxl

    y_min = edge_pxl
    y_max = ySize - edge_pxl

    if use_no_data:
        no_data_val = rsgis_utils.getImageNoDataValue(input_img, img_band)

        out_x_coords = numpy.zeros(n_pts, dtype=numpy.uint16)
        out_y_coords = numpy.zeros(n_pts, dtype=numpy.uint16)

        out_n_pts = 0
        pts_size = n_pts
        while out_n_pts < n_pts:
            x_coords = numpy.random.randint(x_min, high=x_max, size=pts_size, dtype=numpy.uint16)
            y_coords = numpy.random.randint(y_min, high=y_max, size=pts_size, dtype=numpy.uint16)
            pxl_vals = get_image_pxl_values(input_img, img_band, x_coords, y_coords)

            for i in range(pts_size):
                if pxl_vals[i] != no_data_val:
                    out_x_coords[out_n_pts] = x_coords[i]
                    out_y_coords[out_n_pts] = y_coords[i]
                    out_n_pts += 1
            pts_size = n_pts - out_n_pts
    else:
        out_x_coords = numpy.random.randint(x_min, high=x_max, size=n_pts, dtype=numpy.uint16)
        out_y_coords = numpy.random.randint(y_min, high=y_max, size=n_pts, dtype=numpy.uint16)

    rsgislib.imageutils.createCopyImage(input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_8UINT)
    set_image_pxl_values(output_img, 1, out_x_coords, out_y_coords, 1)
