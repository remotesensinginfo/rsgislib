#!/usr/bin/env python
"""
The imagecalc module contains functions for performing a number of calculating on images.
"""
from __future__ import print_function

# import the C++ extension into this level
from ._imagecalc import *

import rsgislib
import numpy
import math

# define our own classes
class BandDefn(object):
    """
Create a list of these objects to pass to the bandMath function as the 'bands' parameter.
"""
    def __init__(self, bandName=None, fileName=None, bandIndex=None):
        self.bandName = bandName
        self.fileName = fileName
        self.bandIndex = bandIndex


class StatsSummary:
    """ 
This is passed to the imagePixelColumnSummary function 
"""
    def __init__(self, min=0.0, max=0.0, sum=0.0, median=0.0, stdDev=0.0, mean=0.0, mode=0.0,
                 calcMin=False, calcMax=False, calcSum=False, calcMean=False, calcStdDev=False, calcMedian=False, calcMode=False):
        self.min = min
        self.max = max
        self.sum = sum
        self.mean = mean
        self.stdDev = stdDev
        self.median = median
        self.mode = mode
        self.calcMin = calcMin
        self.calcMax = calcMax
        self.calcSum = calcSum
        self.calcMean = calcMean
        self.calcStdDev = calcStdDev
        self.calcMedian = calcMedian
        self.calcMode = calcMode



class ImageBandRescale(object):
    """
Data structure for rescaling information for rescaleImgPxlVals function.
:param band: specified image band (band numbering starts at 1).
:param inMin: the input image band minimum value for rescaling.
:param inMax: the input image band maximum value for rescaling.
:param inNoData: no data value for the input image band.
:param outMin: the output image band minimum value for rescaling.
:param outMax: the output image band maximum value for rescaling.
:param outNoData: no data value for the output image band.

"""
    def __init__(self, band=0, inMin=0.0, inMax=0.0, inNoData=0, outMin=0.0, outMax=0.0, outNoData=0.0):
        """
        :param band: specified image band (band numbering starts at 1).
        :param inMin: the input image band minimum value for rescaling.
        :param inMax: the input image band maximum value for rescaling.
        :param inNoData: no data value for the input image band.
        :param outMin: the output image band minimum value for rescaling.
        :param outMax: the output image band maximum value for rescaling.
        :param outNoData: no data value for the output image band.

        """
        self.band = band
        self.inMin = inMin
        self.inMax = inMax
        self.inNoData = inNoData
        self.outMin = outMin
        self.outMax = outMax
        self.outNoData = outNoData
        
    def __str__(self):
        strVal = 'Band ' + str(self.band) + " ["+str(self.inMin)+", "+str(self.inMax)+", "+str(self.inNoData)+"] ["+str(self.outMin)+", "+str(self.outMax)+", "+str(self.outNoData)+"]"
        return strVal
        
    def __repr__(self):
        strVal = 'Band ' + str(self.band) + " ["+str(self.inMin)+", "+str(self.inMax)+", "+str(self.inNoData)+"] ["+str(self.outMin)+", "+str(self.outMax)+", "+str(self.outNoData)+"]"
        return strVal


def calcDist2ImgVals(inputValsImg, outputDistImg, pxlVals, valsImgBand=1, gdalformat='KEA', maxDist=None, noDataVal=None, unitGEO=True):
    """ 
A function to calculate the distance to the nearest pixel value with one of the specified values.

Where:

:param inputValsImg: is a string specifying the input image file.
:param outputDistImg: is a string specfiying the output image file.
:param pxlVals: is a number of list of numbers specifying the features to which the distance from should be calculated.
:param valsImgBand: is an integer specifying the image band of the input image to be used (Default = 1).
:param gdalformat: is a string specifying the output image format (Default = KEA)
:param maxDist: is a number specifying the maximum distance to be calculated, if None not max value is used (Default = None).
:param noDataVal: is the no data value in the input image for which distance should not be calculated for (Default = None; None = no specified no data value).
:param unitGEO: is a boolean specifying the output distance units. True = Geographic units (e.g., metres), False is in Pixels (Default = True).

Example::

    import rsgislib.imagecalc
    cloudsImg = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'
    dist2Clouds = 'LS5TM_20110701_lat52lon421_r24p204_distclouds.kea'
    # Pixel value 1 == Clouds
    # Pixel value 2 == Cloud Shadows
    rsgislib.imagecalc.calcDist2ImgVals(cloudsImg, dist2Clouds, pxlVals=[1,2])
    
"""
    # Check gdal is available
    import osgeo.gdal as gdal
    import rsgislib.imageutils
    
    haveListVals = False
    if type(pxlVals) is list:
        haveListVals = True
    
    proxOptions = []
    
    if maxDist is not None:
        proxOptions.append('MAXDIST='+str(maxDist))
    if noDataVal is not None:
        proxOptions.append('NODATA='+str(noDataVal))
    if unitGEO:
        proxOptions.append('DISTUNITS=GEO')
    else:
        proxOptions.append('DISTUNITS=PIXEL')
    
    if haveListVals:
        strVals = ''
        first = True
        for val in pxlVals:
            if first:
                strVals = str(val)
                first = False
            else:
                strVals = strVals + "," + str(val)
        proxOptions.append('VALUES='+strVals)    
    else:
        proxOptions.append('VALUES='+str(pxlVals))

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress
        
    valsImgDS = gdal.Open(inputValsImg, gdal.GA_ReadOnly)
    valsImgBand = valsImgDS.GetRasterBand(valsImgBand)
    rsgislib.imageutils.createCopyImage(inputValsImg, outputDistImg, 1, 0.0, gdalformat, rsgislib.TYPE_32FLOAT)
    distImgDS = gdal.Open(outputDistImg, gdal.GA_Update)
    distImgBand = distImgDS.GetRasterBand(1)
    gdal.ComputeProximity(valsImgBand, distImgBand, proxOptions, callback=callback)
    distImgBand = None
    distImgDS = None
    classImgBand = None
    classImgDS = None


def calcDist2ImgValsTiled(inputValsImg, outputDistImg, pxlVals, valsImgBand=1, maxDist=1000, noDataVal=1000, gdalformat='KEA', unitGEO=True, tmpDIR='./tmp', tileSize=2000,  nCores=-1):
    """ 
A function to calculate the distance to the nearest pixel value with one of the specified values.

:param inputValsImg: is a string specifying the input image file.
:param outputDistImg: is a string specfiying the output image file.
:param pxlVals: is a number of list of numbers specifying the features to which the distance from should be calculated.
:param valsImgBand: is an integer specifying the image band of the input image to be used (Default = 1).
:param gdalformat: is a string specifying the output image format (Default = KEA)
:param maxDist: is a number specifying the maximum distance to be calculated, if None not max value is used (Default = None).
:param noDataVal: is the no data value in the input image for which distance should not be calculated for (Default = None; None = no specified no data value).
:param unitGEO: is a boolean specifying the output distance units. True = Geographic units (e.g., metres), False is in Pixels (Default = True).
:param tmpDIR: is a directory to be used for storing the image tiles and other temporary files - if not directory does not exist it will be created and deleted on completion (Default: ./tmp).
:param tileSize: is an int specifying in pixels the size of the image tiles used for processing (Default: 2000)
:param nCores: is the number of processing cores which are available to be used for this processing. If -1 all available cores will be used. (Default: -1)

Example::

    import rsgislib.imagecalc
    cloudsImg = 'LS5TM_20110701_lat52lon421_r24p204_clouds.kea'
    dist2Clouds = 'LS5TM_20110701_lat52lon421_r24p204_distclouds.kea'
    # Pixel value 1 == Clouds
    # Pixel value 2 == Cloud Shadows
    rsgislib.imagecalc.calcDist2ImgValsTiled(cloudsImg, dist2Clouds, pxlVals=[1,2])

"""
    
    # Check gdal is available
    import osgeo.gdal as gdal
    import os.path
    import math
    import glob
    import shutil    
    from multiprocessing import Pool
    import rsgislib.imageutils
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    
    haveListVals = False
    if type(pxlVals) is list:
        haveListVals = True
    
    tmpPresent = True
    if not os.path.exists(tmpDIR):
        print("WARNING: \'" + tmpDIR + "\' directory does not exist so creating it...")
        os.makedirs(tmpDIR)
        tmpPresent = False

    if nCores <= 0:
        nCores = rsgislib.tools.utils.numProcessCores()
    
    uid = rsgislib.tools.utils.uidGenerator()
    
    xRes, yRes = rsgislib.imageutils.getImageRes(inputValsImg)
    if unitGEO:
        xMaxDistPxl = math.ceil(maxDist/xRes)
        yMaxDistPxl = math.ceil(maxDist/yRes)
    else:
        xMaxDistPxl = maxDist
        yMaxDistPxl = maxDist
                
    print("Max Dist Pxls X = {}, Y = {}".format(xMaxDistPxl, yMaxDistPxl))
    
    tileOverlap = xMaxDistPxl
    if yMaxDistPxl > xMaxDistPxl:
        tileOverlap = yMaxDistPxl
    
    imgTilesDIR = os.path.join(tmpDIR, 'ImgTiles_'+uid)
    imgTilesDIRPresent = True
    if not os.path.exists(imgTilesDIR):
        os.makedirs(imgTilesDIR)
        imgTilesDIRPresent = False
        
    imgTileBase = os.path.join(imgTilesDIR, 'ImgTile')
    rsgislib.imageutils.createTiles(inputValsImg, imgTileBase, tileSize, tileSize, tileOverlap, 0, 'KEA', rsgislib.imageutils.getRSGISLibDataTypeFromImg(inputValsImg), 'kea')
    imgTileFiles = glob.glob(imgTileBase+'*.kea')
    
    distTilesDIR = os.path.join(tmpDIR, 'DistTiles_'+uid)
    distTilesDIRPresent = True
    if not os.path.exists(distTilesDIR):
        os.makedirs(distTilesDIR)
        distTilesDIRPresent = False    
    
    proxOptions = []
    
    if maxDist is not None:
        proxOptions.append('MAXDIST='+str(maxDist))
    if noDataVal is not None:
        proxOptions.append('NODATA='+str(noDataVal))
    if unitGEO:
        proxOptions.append('DISTUNITS=GEO')
    else:
        proxOptions.append('DISTUNITS=PIXEL')
    
    if haveListVals:
        strVals = ''
        first = True
        for val in pxlVals:
            if first:
                strVals = str(val)
                first = False
            else:
                strVals = strVals + "," + str(val)
        proxOptions.append('VALUES='+strVals)    
    else:
        proxOptions.append('VALUES='+str(pxlVals))

    distTiles = []
    distTileArgs = []
    for tileFile in imgTileFiles:
        baseTileName = os.path.basename(tileFile)
        distTileFile = os.path.join(distTilesDIR, baseTileName)
        tileArgs = [tileFile, distTileFile, proxOptions, noDataVal, 'KEA']
        distTiles.append(distTileFile)
        distTileArgs.append(tileArgs)

    def _computeProximityArrArgsFunc(argVals):
        """
    This function is used internally within calcDist2Classes for the multiprocessing Pool
    """
        import rsgislib.imageutils
        try:
            import tqdm
            pbar = tqdm.tqdm(total=100)
            callback = lambda *args, **kw: pbar.update()
        except:
            callback = gdal.TermProgress
        classImgDS = gdal.Open(argVals[0], gdal.GA_ReadOnly)
        classImgBand = classImgDS.GetRasterBand(1)
        rsgislib.imageutils.createCopyImage(argVals[0], argVals[1], 1, argVals[3], argVals[4], rsgislib.TYPE_32FLOAT)
        distImgDS = gdal.Open(argVals[1], gdal.GA_Update)
        distImgBand = distImgDS.GetRasterBand(1)
        gdal.ComputeProximity(classImgBand, distImgBand, argVals[2], callback=callback)
        distImgBand = None
        distImgDS = None
        classImgBand = None
        classImgDS = None
    
    with Pool(nCores) as p:
        p.map(_computeProximityArrArgsFunc, distTileArgs)
            
    # Mosaic Tiles
    rsgislib.imageutils.createImageMosaic(distTiles, outputDistImg, 0, 0, 1, 1, gdalformat, rsgislib.TYPE_32FLOAT)
    rsgislib.imageutils.popImageStats(outputDistImg, usenodataval=True, nodataval=0, calcpyramids=True)
    
    for imgFile in distTiles:
        rsgislib.tools.filetools.deleteFileWithBasename(imgFile)
    
    if not imgTilesDIRPresent:
        shutil.rmtree(imgTilesDIR, ignore_errors=True)
    else:
        for tileFile in imgTileFiles:
            rsgislib.tools.filetools.deleteFileWithBasename(tileFile)
    
    if not distTilesDIRPresent:
        shutil.rmtree(distTilesDIR, ignore_errors=True)
    
    if not tmpPresent:
        shutil.rmtree(tmpDIR, ignore_errors=True)


def countPxlsOfVal(input_img, vals, img_band=None):
    """
    Function which counts the number of pixels of a set of values returning a list in the same order as
    the list of values provided.

    :param input_img: the input image
    :param vals: is a list of pixel values to be counted
    :param img_band: specify the image band for which the analysis is to be undertaken.
                    If None (default) then all bands will be used.
    :returns list of pixel counts in same order as the vals input list

    """
    from rios.imagereader import ImageReader
    import rsgislib.imageutils
    if vals is None:
        raise Exception("Input vals list must not be None.")
    if len(vals) == 0:
        raise Exception('At least 1 value should be provided within the vals input variable.')
    n_bands = rsgislib.imageutils.getImageBandCount(input_img)
    if (img_band is not None) and ((img_band < 1) or (img_band > n_bands)):
        raise Exception("The specified input image band is not within the input image.")
    if img_band is not None:
        img_band_idx = img_band - 1

    numVals = len(vals)
    outVals = numpy.zeros(numVals, dtype=numpy.int64)

    reader = ImageReader(input_img)
    for (info, block) in reader:     
        for idx in range(numVals):
            if img_band is None:
                outVals[idx] = outVals[idx] + (block == vals[idx]).sum()
            else:
                outVals[idx] = outVals[idx] + (block[img_band_idx,] == vals[idx]).sum()

    return outVals


def getUniqueValues(input_img, img_band=1):
    """
Find the unique image values within an image band.
Note, the whole image band gets read into memory.

:param input_img: input image file path
:param img_band: image band to be processed (starts at 1)

:return: array of unique values.

"""
    import osgeo.gdal as gdal
    imgDS = gdal.Open(input_img)
    if imgDS is None:
        raise Exception("Could not open output image")
    imgBand = imgDS.GetRasterBand(img_band)
    if imgBand is None:
        raise Exception("Could not open output image band ({})".format(img_band))
    valsArr = imgBand.ReadAsArray()
    imgDS = None

    uniq_vals = numpy.unique(valsArr)
    return uniq_vals


def getPCAEigenVector(input_img, pxlNSample, noData=None, outMatrixFile=None):
    """
A function which takes a sample from an input image and uses it to 
generate eigenvector for a PCA. Note. this can be used as input to rsgislib.imagecalc.pca

:param input_img: the image from which the random sample will be taken.
:param pxlNSample: the sample to be taken (e.g., a value of 100 will sample every 100th pixel)
:param noData: provide a no data value which is to be ignored during processing. If None then ignored (Default: None)
:param outMatrixFile: path and name for the output rsgislib matrix file. If None file is not created (Default: None)

:returns: 1. array with the eigenvector, 2. array with the ratio of the explained variance

""" 
    from sklearn.decomposition import PCA
    import rsgislib.imageutils
    
    # Read input data from image file.
    X = rsgislib.imageutils.extractImgPxlSample(input_img, pxlNSample, noData)
    
    print(str(X.shape[0]) + ' values were extracted from the input image.')
    
    pca = PCA()
    pca.fit(X)
    
    if outMatrixFile is not None:
        f = open(outMatrixFile, 'w')
        f.write('m='+str(pca.components_.shape[0])+'\n')
        f.write('n='+str(pca.components_.shape[1])+'\n')
        first = True
        for val in pca.components_.flatten():
            if first:
                f.write(str(val))
                first = False
            else:
                f.write(','+str(val))
        f.write('\n\n')
        f.flush()
        f.close()
    
    pcaComp = 1
    print("Prop. of variance explained:")
    for val in pca.explained_variance_ratio_:
        print('\t PCA Component ' + str(pcaComp) + ' = ' + str(round(val, 4)))
        pcaComp = pcaComp + 1

    return pca.components_, pca.explained_variance_ratio_


def performImagePCA(input_img, outputImg, eigenVecFile, nComponents=None, pxlNSample=100, gdalformat='KEA', datatype=rsgislib.TYPE_32UINT, noData=None, calcStats=True):
    """
A function which performs a PCA on the input image.

:param input_img: the image from which the random sample will be taken.
:param outputImg: the output image transformed using the calculated PCA
:param eigenVecFile: path and name for the output rsgislib matrix file containing the eigenvector for the PCA.
:param nComponents: the number of PCA compoents outputted. A value of None is all components (Default: None)
:param pxlNSample: the sample to be taken (e.g., a value of 100 will sample every 100th pixel) (Default: 100)
:param gdalformat: the output gdal supported file format (Default KEA)
:param datatype: the output data type of the input image (Default: rsgislib.TYPE_32UINT)
:param noData: provide a no data value which is to be ignored during processing. If None then ignored (Default: None)
:param calcStats: Boolean specifying whether pyramids and statistics should be calculated for the output image. (Default: True)
:returns: an array with the ratio of the explained variance per band.
"""
    import rsgislib.imageutils
    eigenVec, varExplain = rsgislib.imagecalc.getPCAEigenVector(input_img, pxlNSample, noData, eigenVecFile)
    outNComp = varExplain.shape[0]
    if nComponents is not None:
        if nComponents > varExplain.shape[0]:
            raise Exception("You cannot output more components than the number of input image bands.")
        outNComp = nComponents
    
    rsgislib.imagecalc.pca(input_img, eigenVecFile, outputImg, outNComp, gdalformat, datatype)
    if calcStats:
        usenodataval=False
        nodataval=0
        if noData is not None:
            usenodataval=True
            nodataval=noData
        rsgislib.imageutils.popImageStats(outputImg, usenodataval, nodataval, True)

    return varExplain


def performImageMNF(input_img, outputImg, nComponents=None, pxlNSample=100, in_img_no_data=None, tmp_dir='./tmp',
                    gdalformat='KEA', datatype=rsgislib.TYPE_32FLOAT, calcStats=True):
    """
A function which takes a sample from an input image and uses it to
generate eigenvector for a MNF. Note. this can be used as input to rsgislib.imagecalc.pca

:param input_img: the image to which the MNF will be applied
:param outputImg: the output image file with the MNF result
:param nComponents: the number of components to be outputted
:param pxlNSample: the sample to be taken (e.g., a value of 100 will sample every 100th pixel) for the PCA
:param in_img_no_data: provide a no data value which is to be ignored during processing. If None then try to read from input image.
:param tmp_dir: a directory where temporary output files will be stored. If it doesn't exist it will be created.
:param gdalformat: output image file format
:param datatype: data type for the output image. Note, it is common to have negative values.
:param calcStats: whether image statistics and pyramids could be calculated.
:returns: array with the ratio of the explained variance

"""
    from sklearn.decomposition import PCA
    import rsgislib.tools.filetools
    import rsgislib.imageutils
    import os
    import shutil

    created_tmp_dir = False
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
        created_tmp_dir = True

    img_basename = rsgislib.tools.filetools.get_file_basename(input_img)

    if in_img_no_data is None:
        in_img_no_data = rsgislib.imageutils.getImageNoDataValue(input_img)
        if in_img_no_data is None:
            raise Exception("A no data value for the input image must be provided.")

    valid_msk_img = os.path.join(tmp_dir, "{}_vld_msk.kea".format(img_basename))
    rsgislib.imageutils.genValidMask(input_img, valid_msk_img, "KEA", in_img_no_data)

    whiten_img = os.path.join(tmp_dir, "{}_whiten.kea".format(img_basename))
    rsgislib.imageutils.whitenImage(input_img, valid_msk_img, 1, whiten_img, "KEA")

    # Read input data from image file.
    X = rsgislib.imageutils.extractImgPxlSample(whiten_img, pxlNSample, in_img_no_data)
    print('{} values were extracted from the input image.'.format(X.shape[0]))

    pca = PCA()
    pca.fit(X)

    eigenVecFile = os.path.join(tmp_dir, "{}_eigen_vec.txt".format(img_basename))
    f = open(eigenVecFile, 'w')
    f.write('m=' + str(pca.components_.shape[0]) + '\n')
    f.write('n=' + str(pca.components_.shape[1]) + '\n')
    first = True
    for val in pca.components_.flatten():
        if first:
            f.write(str(val))
            first = False
        else:
            f.write(',' + str(val))
    f.write('\n\n')
    f.flush()
    f.close()

    pcaComp = 1
    print("Prop. of variance explained:")
    for val in pca.explained_variance_ratio_:
        print('\t PCA Component ' + str(pcaComp) + ' = ' + str(round(val, 4)))
        pcaComp = pcaComp + 1
    varExplain = pca.explained_variance_ratio_

    outNComp = varExplain.shape[0]
    if nComponents is not None:
        if nComponents > varExplain.shape[0]:
            raise Exception("You cannot output more components than the number of input image bands.")
        outNComp = nComponents

    rsgislib.imagecalc.pca(whiten_img, eigenVecFile, outputImg, outNComp, gdalformat, datatype)
    if calcStats:
        rsgislib.imageutils.popImageStats(outputImg, True, in_img_no_data, True)

    if created_tmp_dir:
        shutil.rmtree(tmp_dir)
    else:
        os.remove(valid_msk_img)
        os.remove(whiten_img)
        os.remove(eigenVecFile)

    return varExplain


def rescaleImgPxlVals(input_img, outputImg, gdalformat, datatype, bandRescale, trim2Limits=True):
    """
Function which rescales an input image base on a list of rescaling parameters.

:param input_img: the input image
:param outputImg: the output image file name and path (will be same dimensions as the input)
:param gdalformat: the GDAL image file format of the output image file.
:param bandRescale: list of ImageBandRescale objects
:param trim2Limits: whether to trim the output to the output min/max values.

"""
    from rios import applier

    bandRescaleDict = dict()
    for rescaleObj in bandRescale:
        bandRescaleDict[rescaleObj.band-1] = rescaleObj

    numpyDT = rsgislib.getNumpyDataType(datatype)

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.image = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    otherargs.rescaleDict = bandRescaleDict
    otherargs.trim = trim2Limits
    otherargs.numpyDT = numpyDT
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    
    def _applyRescale(info, inputs, outputs, otherargs):
        """
        This is an internal rios function 
        """
        outputs.outimage = numpy.zeros_like(inputs.image, dtype=numpyDT)
        for idx in range(inputs.image.shape[0]):
            outputs.outimage[idx] = numpy.where(inputs.image[idx] == otherargs.rescaleDict[idx].inNoData, otherargs.rescaleDict[idx].outNoData, (((inputs.image[idx]-otherargs.rescaleDict[idx].inMin)/(inputs.image[idx]-otherargs.rescaleDict[idx].inMax - inputs.image[idx]-otherargs.rescaleDict[idx].inMin)) * (inputs.image[idx]-otherargs.rescaleDict[idx].outMax - inputs.image[idx]-otherargs.rescaleDict[idx].outMin)) + inputs.image[idx]-otherargs.rescaleDict[idx].outMin)
            if otherargs.trim:
                outputs.outimage[idx] = numpy.where((outputs.outimage[idx] != otherargs.rescaleDict[idx].outNoData) & (outputs.outimage[idx]<otherargs.rescaleDict[idx].outMin), otherargs.rescaleDict[idx].outMin, outputs.outimage[idx])
                outputs.outimage[idx] = numpy.where((outputs.outimage[idx] != otherargs.rescaleDict[idx].outNoData) & (outputs.outimage[idx]>otherargs.rescaleDict[idx].outMax), otherargs.rescaleDict[idx].outMax, outputs.outimage[idx])

    applier.apply(_applyRescale, infiles, outfiles, otherargs, controls=aControls)


def calcHistograms4MskVals(input_img, imgBand, imgMsk, mskBand, minVal, maxVal, binWidth, mskvals=None):
    """
A function which reads the image bands (values and mask) into memory and creates a 
histogram for each value within the mask value. Within the mask 0 is considered to be no data.

:param input_img: image values image file path.
:param imgBand: values image band
:param imgMsk: file path for image mask.
:param mskBand: mask image band
:param minVal: minimum value for the histogram bins
:param maxVal: maximum value for the histogram bins
:param binWidth: the width of the histograms bins.
:param mskvals: a list of values within the mask can be provided to just consider a limited number of mask values
                when calculating the histograms. If None (default) then calculated for all mask values.

:return: returns a dict of mask values with an array for the histogram.

"""
    minVal = float(minVal)
    maxVal = float(maxVal)
    nBins = math.ceil((maxVal - minVal)/binWidth)
    maxVal = float(minVal + (binWidth * nBins))
    
    imgValsDS = gdal.Open(input_img)
    imgValsBand = imgValsDS.GetRasterBand(imgBand)
    valsArr = imgValsBand.ReadAsArray()
    imgValsDS = None
    
    imgMskDS = gdal.Open(imgMsk)
    imgMskBand = imgMskDS.GetRasterBand(mskBand)
    mskArr = imgMskBand.ReadAsArray()
    imgMskDS = None
    
    if mskvals is None:
        uniq_vals = numpy.unique(mskArr)
    else:
        uniq_vals = mskvals
    
    hist_dict = dict()
    
    for msk_val in uniq_vals:
        if msk_val != 0:
            mskd_vals = valsArr[mskArr==msk_val]
            hist_arr, bin_edges = numpy.histogram(mskd_vals, bins=nBins, range=(minVal, maxVal))
            hist_dict[msk_val] = hist_arr
    
    valsArr = None
    mskArr = None
            
    return hist_dict


def calcWSG84PixelArea(img, out_img, scale=10000, gdalformat='KEA'):
    """
A function which calculates the area (in metres) of the pixel projected in WGS84.

:param img: input image, for which the per-pixel area will be calculated.
:param out_img: output image file.
:param scale: scale the output area to unit of interest. Scale=10000(Ha), Scale=1(sq m), Scale=1000000(sq km), Scale=4046.856(Acre), Scale=2590000(sq miles), Scale=0.0929022668(sq feet)

"""
    import rsgislib.tools
    from rios import applier

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    x_res, y_res = rsgislib.imagecalc.getImageRes(img)
    
    infiles = applier.FilenameAssociations()
    infiles.img = img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = out_img
    otherargs = applier.OtherInputs()
    otherargs.x_res = x_res
    otherargs.y_res = y_res
    otherargs.scale = float(scale)
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False
    
    def _calcPixelArea(info, inputs, outputs, otherargs):
        xBlock, yBlock = info.getBlockCoordArrays()
            
        x_res_arr = numpy.zeros_like(yBlock, dtype=float)
        x_res_arr[...] = otherargs.x_res
        y_res_arr = numpy.zeros_like(yBlock, dtype=float)
        y_res_arr[...] = otherargs.y_res
        x_res_arr_m, y_res_arr_m = rsgislib.tools.degrees_to_metres(yBlock, x_res_arr, y_res_arr)
        outputs.outimage = numpy.expand_dims((x_res_arr_m*y_res_arr_m)/otherargs.scale, axis=0)
    
    applier.apply(_calcPixelArea, infiles, outfiles, otherargs, controls=aControls)


def calcPPI(input_img, outputimg, gdalformat, niters=1000, lthres=0, uthres=0, img_gain=1, seed=None, calcstats=True):
    """
A function which calculate the pixel purity index (PPI). Using an appropriate number of iterations
this can take a little while to run. Note, the whole input image is read into memory.

It is recommended that you use the an MNF/PCA output and rescale that so all bands have the same range to improve
the PPI result.

Boardman J.W., Kruse F.A, and Green R.O., "Mapping Target Signatures via
    Partial Unmixing of AVIRIS Data," Pasadena, California, USA, 23 Jan 1995,
    URI: http://hdl.handle.net/2014/33635

:param input_img: image values image file path.
:param outputimg: output image
:param gdalformat: GDAL file format (e.g., KEA) of the output image.
:param niters: number of iterations
:param thres: a threshold in the image space (after again as been applied) to select more pixels around the extreme (e.g., 1% reflectance)
:param img_gain: the gain by which the image was multipled, reflectance images are typically multiplied by 1000 or 10000. The result should be an image with a range 0-1.
:param seed: seed for the random squence of numbers being generated. Using the same seed will result in the same seqence and therefore the same output.
:param calcstats: whether to calculate image statistics and pyramids on the output image.

"""
    # Check gdal is available
    import osgeo.gdal as gdal
    import rsgislib.imageutils
    import tqdm
    import numpy

    imgDS = gdal.Open(input_img)
    if imgDS is None:
        raise Exception("Could not open input image")
    n_bands = imgDS.RasterCount
    x_size = imgDS.RasterXSize
    y_size = imgDS.RasterYSize
    img_data = numpy.zeros((n_bands, (x_size * y_size)), dtype=numpy.float32)
    img_data_msk = numpy.ones((x_size * y_size), dtype=bool)
    img_data_mean = numpy.zeros(n_bands, dtype=numpy.float32)

    print("Importing Bands:")
    for n in tqdm.tqdm(range(n_bands)):
        imgBand = imgDS.GetRasterBand(n + 1)
        if imgBand is None:
            raise Exception("Could not open image band ({})".format(n + 1))
        no_data_val = imgBand.GetNoDataValue()
        band_arr = imgBand.ReadAsArray().flatten()
        band_arr = band_arr.astype(numpy.float32)
        img_data[n] = band_arr
        img_data_msk[band_arr == no_data_val] = False
        band_arr[band_arr == no_data_val] = numpy.nan
        if img_gain > 1:
            band_arr = band_arr / img_gain
            img_data[n] = img_data[n] / img_gain
        img_data_mean[n] = numpy.nanmean(band_arr)
        img_data[n] = img_data[n] - img_data_mean[n]
    imgDS = None
    band_arr = None

    print("Create empty output image file")
    rsgislib.imageutils.createCopyImage(input_img, outputimg, 1, 0, gdalformat, rsgislib.TYPE_16UINT)

    # Open output image
    outImgDS = gdal.Open(outputimg, gdal.GA_Update)
    if outImgDS is None:
        raise Exception("Could not open output image")
    outImgBand = outImgDS.GetRasterBand(1)
    if outImgBand is None:
        raise Exception("Could not open output image band (1)")
    out_img_data = outImgBand.ReadAsArray()

    # Mask the datasets to obtain just the valid pixel values (i.e., using the no data value)
    img_data = img_data.T
    out_img_data = out_img_data.flatten()
    pxl_idxs = numpy.arange(out_img_data.shape[0])
    pxl_idxs = pxl_idxs[img_data_msk]
    out_img_count = out_img_data[img_data_msk]
    img_data = img_data[img_data_msk]

    if seed is not None:
        numpy.random.seed(seed)

    print("Perform PPI iterations.")
    for i in tqdm.tqdm(range(niters)):
        r = numpy.random.rand(n_bands) - 0.5
        s = numpy.dot(img_data, r)

        imin = numpy.argmin(s)
        imax = numpy.argmax(s)
        if lthres == 0:
            # Only the two extreme pixels are incremented
            out_img_count[imin] += 1
        else:
            # All pixels within threshold distance from the two extremes
            out_img_count[s <= (s[imin] + lthres)] += 1

        if uthres == 0:
            # Only the two extreme pixels are incremented
            out_img_count[imax] += 1
        else:
            # All pixels within threshold distance from the two extremes
            out_img_count[s >= (s[imax] - uthres)] += 1
    s = None

    out_img_data[pxl_idxs] = out_img_count
    out_img_data = out_img_data.reshape((y_size, x_size))

    outImgBand.WriteArray(out_img_data)
    outImgDS = None

    if calcstats:
        print("Calculate Image stats and pyramids.")
        rsgislib.imageutils.popImageStats(outputimg, usenodataval=True, nodataval=0, calcpyramids=True)

def calcImgsPxlMode(inputImgs, outputImg, gdalformat, no_data_val=0):
    """
Function which calculates the mode of a group of images.

Warning, this function can be very slow. Use rsgislib.imagecalc.imagePixelColumnSummary

:param inputImgs: the list of images
:param outputImg: the output image file name and path (will be same dimensions as the input)
:param gdalformat: the GDAL image file format of the output image file.

"""
    import rsgislib.imageutils
    import scipy.stats
    from rios import applier

    datatype = rsgislib.imageutils.getRSGISLibDataTypeFromImg(inputImgs[0])
    numpyDT = rsgislib.getNumpyDataType(datatype)

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.images = inputImgs
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = outputImg
    otherargs = applier.OtherInputs()
    otherargs.no_data_val = no_data_val
    otherargs.numpyDT = numpyDT
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _applyCalcMode(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        image_data = numpy.concatenate(inputs.images, axis=0).astype(numpy.float32)
        image_data[image_data == otherargs.no_data_val] = numpy.nan
        mode_arr, count_arr = scipy.stats.mode(image_data, axis=0, nan_policy='omit')
        outputs.outimage = mode_arr.astype(otherargs.numpyDT)

    applier.apply(_applyCalcMode, infiles, outfiles, otherargs, controls=aControls)


def calcImgBasicStats4RefRegion(ref_img, stats_imgs, output_img, gdalformat='KEA'):
    """
A function which calculates the mean and standard deviation through a series of
input images. The region for processing is defined by the reference image and
images padded with no-data where no data is present.

The output image has twice the number of bands as the input image providing
a mean and standard deviation for each input band.

If the input images has 2 bands then the output bands will have the following
order:

1. band 1 mean
2. band 1 std dev
3. band 2 mean
4. band 2 std dev

:param ref_img: reference image which defines the output image
:param stats_imgs: a list of input images over which the stats will be calculated.
:param output_img: the output image path and file name
:param gdalformat: the output image file format. Default KEA.

"""
    import rsgislib.imageutils
    from rios import applier

    first = True
    n_bands = 0
    no_data_val = 0
    for img in stats_imgs:
        print(img)
        if first:
            n_bands = rsgislib.imageutils.getImageBandCount(img)
            no_data_val = rsgislib.imageutils.getImageNoDataValue(img)
            first = False
        else:
            if n_bands != rsgislib.imageutils.getImageBandCount(img):
                raise Exception("The number of bands must be the same in all input images.")
            if no_data_val != rsgislib.imageutils.getImageNoDataValue(img):
                raise Exception("The no data value should be the same in all input images.")

    # RIOS internal function to calculate  mean and standard deviation of the input images
    def _calcBasicStats(info, inputs, outputs, otherargs):
        n_imgs = len(inputs.imgs)
        blk_shp = inputs.imgs[0].shape
        if blk_shp[0] != otherargs.n_bands:
            raise Exception("Block shape and the number of input image bands do not align.")
        outputs.output_img = numpy.zeros((blk_shp[0] * 2, blk_shp[1], blk_shp[2]), dtype=float)

        band_arr = []
        for band in range(blk_shp[0]):
            band_arr.append(numpy.zeros((n_imgs, blk_shp[1], blk_shp[2]), dtype=float))

        img_idx = 0
        for img_blk in inputs.imgs:
            for band in range(blk_shp[0]):
                band_arr[band][img_idx] = img_blk[band]
            img_idx = img_idx + 1

        for band in range(blk_shp[0]):
            band_arr[band][band_arr[band] == otherargs.no_data_val] = numpy.nan

            outputs.output_img[band * 2] = numpy.nanmean(band_arr[band], axis=0)
            outputs.output_img[band * 2 + 1] = numpy.nanstd(band_arr[band], axis=0)

            outputs.output_img[band * 2][numpy.isnan(outputs.output_img[band * 2])] = otherargs.no_data_val
            outputs.output_img[band * 2 + 1][numpy.isnan(outputs.output_img[band * 2 + 1])] = 0.0

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.imgs = stats_imgs

    otherargs = applier.OtherInputs()
    otherargs.n_bands = n_bands
    otherargs.no_data_val = no_data_val

    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img

    aControls = applier.ApplierControls()
    aControls.referenceImage = ref_img
    aControls.footprint = applier.BOUNDS_FROM_REFERENCE
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False
    print("Calculating Stats Image.")
    applier.apply(_calcBasicStats, infiles, outfiles, otherargs, controls=aControls)
    print("Completed")

    rsgislib.imageutils.popImageStats(output_img, usenodataval=True, nodataval=no_data_val, calcpyramids=True)


def normaliseImageBand(input_img, band, output_img, gdal_format='KEA'):
    """
    Perform a simple normalisation a single image band (val - min)/range.

    :param input_img: The input image file.
    :param band: the image band (starts at 1) to be normalised
    :param output_img: the output image file (will just be a single band).
    :param gdal_format: The output image format.

    """
    import rsgislib
    import rsgislib.imageutils

    no_data_val = rsgislib.imageutils.getImageNoDataValue(input_img, band)
    use_no_data_val = True
    if no_data_val is None:
        use_no_data_val = False
        no_data_val = 0.0

    band_min, band_max = rsgislib.imagecalc.getImageBandMinMax(input_img, band, use_no_data_val, no_data_val)

    band_defns = [rsgislib.imagecalc.BandDefn('b1', input_img, band)]
    band_range = band_max - band_min
    exp = '(b1=={0})?0.0:(b1-{1})/{2}'.format(no_data_val, band_min, band_range)
    rsgislib.imagecalc.bandMath(output_img, exp, gdal_format, rsgislib.TYPE_32FLOAT, band_defns)
    rsgislib.imageutils.popImageStats(output_img, usenodataval=True, nodataval=0.0, calcpyramids=True)


def recodeIntRaster(input_img, output_img, recode_dict, keepvalsnotindict=True, gdalformat='KEA',
                    datatype=rsgislib.TYPE_32INT):
    """
    A function recodes an input image. Assuming image only has a single image band so
    it will be band 1 which is recoded. The recode is provided as a dict where the key
    is the value to be recoded and the value of the dict is the output value.

    :param input_img: Input image file.
    :param output_img: Output image file.
    :param recode_dict: dict for recode (key: int to be recoded, value: int to recode to)
    :param keepvalsnotindict: boolean whether pixels not being recoded should be copied to the output (True)
                              or whether only those pixels recoded should be outputted (False) (default: True)
    :param gdalformat: output file format (default: KEA)
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.

    """
    from rios import applier
    import numpy

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.inimage = input_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = output_img
    otherargs = applier.OtherInputs()
    otherargs.np_dtype = rsgislib.getNumpyDataType(datatype)
    otherargs.keepvalsnotindict = keepvalsnotindict
    otherargs.recode_dict = recode_dict
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _recode(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.inimage[0], dtype=otherargs.np_dtype)
        if otherargs.keepvalsnotindict:
            out_arr = inputs.inimage[0]

        for rc_val in recode_dict:
            out_arr[inputs.inimage[0] == rc_val] = recode_dict[rc_val]
        outputs.outimage = numpy.expand_dims(out_arr, axis=0)

    applier.apply(_recode, infiles, outfiles, otherargs, controls=aControls)


def calcFillRegionsKNN(ref_img, ref_no_data, fill_regions_img, fill_region_val, out_img, k=5,
                          summary=rsgislib.SUMTYPE_MODE, gdalformat='KEA', datatype=rsgislib.TYPE_32INT):
    """
    A function will fills regions (defined by having a value == fill_region_val) of the fill_regions_img
    image using a KNN approach based of pixels within the ref_img image. The KNN distance is the spatial
    distance based off the pixel locations.

    The ref_img and fill_regions_img must both be in the same projection.

    :param ref_img: The reference image (can be a different resolution) from which the KNN will be trained.
    :param ref_no_data: The no data value used within the reference image.
    :param fill_regions_img: A mask image defining the regions to be filled.
    :param fill_region_val: The pixel value specifying the pixels in the fill_regions_img image for the KNN value
                            will be calculated.
    :param out_img: The output image file name and path.
    :param k: The k parameter of the KNN.
    :param summary: Summary method (rsgislib.SUMTYPE_*) to be used (Default is Mode).
                    Options: Mode, Median, Sum, Mean, Range, Min, Max.
    :param gdalformat: output file format (default: KEA)
    :param datatype: is a rsgislib.TYPE_* value providing the data type of the output image.

    """
    import rsgislib.imageutils
    from rios import applier
    import numpy
    import scipy
    import scipy.stats
    import rtree

    if not rsgislib.imageutils.doGDALLayersHaveSameProj(ref_img, fill_regions_img):
        raise Exception("The reference image and fill regions image do not have the same projection.")

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    x_res, y_res = rsgislib.imageutils.getImageRes(ref_img)
    if x_res < 0:
        x_res *= -1
    if y_res < 0:
        y_res *= -1

    print("Building Spatial Index...")
    # Create the index.
    kd_idx = rtree.index.Index()

    # Populate Index
    infiles = applier.FilenameAssociations()
    infiles.ref_img = ref_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.kd_idx = kd_idx
    otherargs.ref_no_data = ref_no_data
    otherargs.w_box = x_res / 2
    otherargs.h_box = y_res / 2
    otherargs.n = 0
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _retrieve_idx_info(info, inputs, outputs, otherargs):
        xBlock, yBlock = info.getBlockCoordArrays()
        xBlock = xBlock.flatten()
        yBlock = yBlock.flatten()
        ref_data = inputs.ref_img[0].flatten()

        x_coords = xBlock[ref_data != otherargs.ref_no_data]
        y_coords = yBlock[ref_data != otherargs.ref_no_data]
        val_data = ref_data[ref_data != otherargs.ref_no_data]

        left_coords = x_coords - otherargs.w_box
        right_coords = x_coords + otherargs.w_box

        top_coords = y_coords + otherargs.h_box
        bot_coords = y_coords - otherargs.h_box

        coords = numpy.stack((left_coords, bot_coords, right_coords, top_coords), axis=1)
        for coord, cls in zip(coords, val_data):
            otherargs.kd_idx.insert(otherargs.n, coord, cls)
            otherargs.n += 1

    applier.apply(_retrieve_idx_info, infiles, outfiles, otherargs, controls=aControls)

    print("\nFilling Image Regions using KNN (k={})".format(k))
    x_res, y_res = rsgislib.imageutils.getImageRes(fill_regions_img)
    if x_res < 0:
        x_res *= -1
    if y_res < 0:
        y_res *= -1

    # Apply KNN fill.
    infiles = applier.FilenameAssociations()
    infiles.inimage = fill_regions_img
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = out_img
    otherargs = applier.OtherInputs()
    otherargs.kd_idx = kd_idx
    otherargs.k = k
    otherargs.fill_region_val = fill_region_val
    otherargs.np_dtype = rsgislib.getNumpyDataType(datatype)
    otherargs.w_box = x_res / 2
    otherargs.h_box = y_res / 2
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = False
    aControls.calcStats = False

    def _knn_fill_regions(info, inputs, outputs, otherargs):
        out_arr = numpy.zeros_like(inputs.inimage[0], dtype=otherargs.np_dtype)
        xBlock, yBlock = info.getBlockCoordArrays()
        data_shp = out_arr.shape
        for i in range(data_shp[0]):
            for j in range(data_shp[1]):
                if inputs.inimage[0, i, j] == otherargs.fill_region_val:
                    bbox = (
                    xBlock[i, j] - otherargs.w_box, yBlock[i, j] - otherargs.h_box, xBlock[i, j] + otherargs.w_box,
                    yBlock[i, j] + otherargs.h_box)

                    vals = list(otherargs.kd_idx.nearest(bbox, otherargs.k, objects='raw'))
                    int_vals = list()
                    for val in vals:
                        int_vals.append(int(val))
                    if summary == rsgislib.SUMTYPE_MODE:
                        out_arr[i, j] = scipy.stats.mode(int_vals).mode[0]
                    elif summary == rsgislib.SUMTYPE_MEDIAN:
                        out_arr[i, j] = scipy.median(int_vals)
                    elif summary == rsgislib.SUMTYPE_MAX:
                        out_arr[i, j] = scipy.amax(int_vals)
                    elif summary == rsgislib.SUMTYPE_MIN:
                        out_arr[i, j] = scipy.amin(int_vals)
                    elif summary == rsgislib.SUMTYPE_MEAN:
                        out_arr[i, j] = scipy.mean(int_vals)
                    elif summary == rsgislib.SUMTYPE_SUM:
                        out_arr[i, j] = scipy.sum(int_vals)
                    elif summary == rsgislib.SUMTYPE_RANGE:
                        out_arr[i, j] = scipy.amax(int_vals) - scipy.amin(int_vals)
                    else:
                        raise Exception("Summary method not recognised/available.")

        outputs.outimage = numpy.expand_dims(out_arr, axis=0)

    applier.apply(_knn_fill_regions, infiles, outfiles, otherargs, controls=aControls)
    print("Finished fill")


def areImgsEqual(in_ref_img, in_cmp_img, prop_eql=1.0, flt_dif=0.0001):
    """
    A function to check whether two images have equal pixel values within the spatial overlap.
    Note, if the two input images only have a partial overlap (i.e., one is a subset of the other)
    then they are still be equal if the overlapping region has matching pixel values.

    :param in_ref_img: The input reference image for the comparison.
    :param in_cmp_img: The input comparison image to be compared to the reference image.
    :param prop_eql: The proportion of pixels within the scene which need to be identified as identical to return True.
                     Range is 0 - 1. Default is 1.0 (i.e., 100 % of pixels have to be identical to return True).
    :param flt_dif: A threshold for comparing two floating point numbers as being identical - this avoids issues with
                    rounding and the number of decimal figures stored.
    :return: Boolean (match), float (proportion of pixels which matched)

    """
    import rsgislib.imageutils
    from rios import applier
    import numpy

    if rsgislib.imageutils.getImageBandCount(in_ref_img) != rsgislib.imageutils.getImageBandCount(in_cmp_img):
        raise Exception("The number of image bands is not the same between the two images.")

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    # Generated the combined mask.
    infiles = applier.FilenameAssociations()
    infiles.in_ref_img = in_ref_img
    infiles.in_cmp_img = in_cmp_img
    outfiles = applier.FilenameAssociations()
    otherargs = applier.OtherInputs()
    otherargs.flt_dif = flt_dif
    otherargs.n_pxls = 0.0
    otherargs.n_eq_pxls = 0.0
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar

    def _calcPropEqual(info, inputs, outputs, otherargs):
        ref_pxls = inputs.in_ref_img.flatten()
        cmp_pxls = inputs.in_cmp_img.flatten()

        cmp_pxls = cmp_pxls[~numpy.isnan(ref_pxls)]
        ref_pxls = ref_pxls[~numpy.isnan(ref_pxls)]

        ref_pxls = ref_pxls[~numpy.isnan(cmp_pxls)]
        cmp_pxls = cmp_pxls[~numpy.isnan(cmp_pxls)]

        pxl_diff = numpy.abs(ref_pxls - cmp_pxls)
        eql_pxls = numpy.where(pxl_diff < otherargs.flt_dif, 1, 0)

        otherargs.n_eq_pxls += eql_pxls.sum()
        otherargs.n_pxls += ref_pxls.shape[0]

    applier.apply(_calcPropEqual, infiles, outfiles, otherargs, controls=aControls)

    prop_pxls_eq = otherargs.n_eq_pxls / otherargs.n_pxls
    return (prop_pxls_eq >= prop_eql), prop_pxls_eq

