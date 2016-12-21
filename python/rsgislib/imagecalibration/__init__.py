"""
The image calibration module contains functions for calibrating optical data from DN to radience and top of atmosphere reflectance and, using coefficients from 6S, surface reflectance.

For obtaining the correct parameters for each function it is recomented that rather than running directly they are called through the Atmospheric and Radiometric Correction of Satellite Imagery (ARCSI) software. 

More details on ARCSI are avaialble from http://rsgislib.org/arcsi
"""

# import the C++ extension into this level
from ._imagecalibration import *


def performDOSCalc(inputFile, outputFile, gdalFormat='KEA', nonNegative=True, noDataVal=0, darkObjReflVal=0, darkObjPercentile=0.01, copyBandNames=True, calcStatsPyd=True):
    """
A command to perform a dark object subtraction (DOS) on an input image.

* inputFile - input image to which the DOS method is to be applied. Typically, this image with be in top of atmosphere reflectance (TOA)
* outputFile - the output image file
* gdalFormat - the output image file format (default = KEA)
* nonNegative - is a boolean specifying where negative output pixel values will be accepted (Dafualt is True; i.e., no negative values)
* noDataVal - is the no data value within the input image file. 
* darkObjReflVal - is an offset which is applied to all pixel values to make a minimum reflectance value (Default = 0)
* darkObjPercentile - is the percentile of the input image used to define the dark object threshold, range is 0 - 1 (Default is 0.01; i.e., 1%).
* copyBandNames - is a boolean specifying that the band names of the input image should be copied to the output image file (Default: True)
* calcStatsPyd - is a boolean specifying that the image stats and pyramids should be calculated on the output image (Default: True)

Example::

    import rsgislib.imagecalibration
    rsgislib.imagecalibration.performDOSCalc("LS5TM_20110701_lat52lon421_r24p204_rad_toa.kea", 'LS5TM_20110701_lat52lon421_r24p204_rad_toa_dos.kea")

"""
    import rsgislib
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import collections
    
    rsgisUtils = rsgislib.RSGISPyUtils()
    outDataType = rsgisUtils.getRSGISLibDataTypeFromImg(inputFile)
    
    percentList = rsgislib.imagecalc.bandPercentile(inputFile, darkObjPercentile, noDataVal)
        
    offsetsList = list()
    OffVal = collections.namedtuple('DOSOffset', ['offset'])
    for val in percentList:
        offsetsList.append(OffVal(offset=val))
    
    applySubtractSingleOffsets(inputFile, outputFile, gdalFormat, outDataType, nonNegative, True, noDataVal, darkObjReflVal, offsetsList)
    
    if copyBandNames:
        bandNames = rsgislib.imageutils.getBandNames(inputFile)
        rsgislib.imageutils.setBandNames(outputFile, bandNames)
    
    if calcStatsPyd:
        rsgislib.imageutils.popImageStats(outputFile, usenodataval=True, nodataval=noDataVal, calcpyramids=True)



def calcClearSkyRegions(cloudsImg, validAreaImg, outputClearSkyMask, outFormat, tmpPath='./tmpClearSky', deleteTmpFiles=True, initClearSkyRegionDist=5000, initClearSkyRegionMinSize=3000, finalClearSkyRegionDist=1000, morphSize=21):
    """
Given a cloud mask, identify the larger extent regions of useful clear-sky regions.

* cloudsImg - An image with the input mask of the cloud (pixel == 1) and shadow (pixel == 2) 
* validAreaImg - A mask of the image data area (1 = valid and 0 = not-valid; i.e., outside of the data area)
* outputClearSkyMask - The output mask of the clear sky areas
* outFormat - The output image format.
* tmpPath - The path for temporay images produced during the processing to be stored (Default: './tmpClearSky'; Note. all temp files are generated as KEA files).
* deleteTmpFiles - Boolean as to whether the intermediate files should be deleted following processing (Default: True - delete files).
* initClearSkyRegionDist - The distance in metres from a cloud/shadow object for the initial identification of clear sky regions (Default: 5000)
* initClearSkyRegionMinSize - The minimum size (in pixels) of the initial clear sky regions (Default: 3000 pixels)
* finalClearSkyRegionDist - The distance in metres from a cloud/shadow object for the final boundaries of the clear sky regions (Default: 1000)
* morphSize - the size of the circular morphological operator used to tidy up the result (Default: 21)

Example::

    import rsgislib.imagecalibration
    cloudsImg = "./Outputs/LS8_20160605_lat52lon261_r24p203_clouds.kea"
    validAreaImg = "./Outputs/LS8_20160605_lat52lon261_r24p203_valid.kea"
    outputMask = "./Outputs/LS8_20160605_lat52lon261_r24p203_openskyvalid.kea"
    tmpPath = "./temp"
    rsgislib.imagecalibration.calcClearSkyRegions(cloudsImg, validAreaImg, outputMask, 'KEA', tmpPath)

"""
    
    import rsgislib
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.segmentation
    import rsgislib.rastergis
    import rsgislib.vectorutils
    import rsgislib.imagemorphology
    import os.path
    import osgeo.gdal as gdal
    from rios import rat
    import numpy
    
    if morphSize % 2 == 0:
        raise rsgislib.RSGISPyException("The size of the morphology operator must be odd.")
    
    baseDataName = os.path.splitext(os.path.basename(cloudsImg))[0]  
    tmpCloudsImgDist2Clouds = os.path.join(tmpPath, baseDataName+"_dist2clouds.kea")
    tmpCloudsImgDist2CloudsNoData = os.path.join(tmpPath, baseDataName+"_dist2clouds_masked.kea")
    tmpInitClearSkyRegions = os.path.join(tmpPath, baseDataName+"initclearsky.kea")
    tmpInitClearSkyRegionsClumps = os.path.join(tmpPath, baseDataName+"initclearskyClumps.kea")
    tmpInitClearSkyRegionsRmSmall = os.path.join(tmpPath, baseDataName+"initclearskyClumpsRMSmall.kea")
    tmpInitClearSkyRegionsFinal = os.path.join(tmpPath, baseDataName+"initclearskyClumpsFinal.kea")
    tmpClearSkyRegionsFullExtent = os.path.join(tmpPath, baseDataName+"clearskyClumpsFullExtent.kea")
    tmpClearSkyRegionsFullExtentClumps = os.path.join(tmpPath, baseDataName+"clearskyClumpsFullExtentClumps.kea")
    tmpClearSkyRegionsFullExtentSelectClumps = os.path.join(tmpPath, baseDataName+"clearskyClumpsFullExtentSelectClumps.kea")
    tmpClearSkyRegionsFullExtentSelectClumpsOpen = os.path.join(tmpPath, baseDataName+"clearskyClumpsFullExtentSelectClumpsOpen.kea")
    tmpClearSkyRegionsFullExtentSelectClumpsOpenClump = os.path.join(tmpPath, baseDataName+"clearskyClumpsFullExtentSelectClumpsOpenClump.kea")
    tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall = os.path.join(tmpPath, baseDataName+"clearskyClumpsFullExtentSelectClumpsOpenClumpRMSmall.kea")
    tmpMorphOperator = os.path.join(tmpPath, 'CircularMorphOp.gmtxt')
    
    rsgislib.imagecalc.calcDist2ImgVals(cloudsImg, tmpCloudsImgDist2Clouds, pxlVals=[1,2])
        
    rsgislib.imageutils.maskImage(tmpCloudsImgDist2Clouds, validAreaImg, tmpCloudsImgDist2CloudsNoData, 'KEA', rsgislib.TYPE_32INT, -1, 0)    
            
    rsgislib.imagecalc.imageMath(tmpCloudsImgDist2CloudsNoData, tmpInitClearSkyRegions, 'b1 > '+str(initClearSkyRegionDist), outFormat, rsgislib.TYPE_32UINT)
    
    rsgislib.segmentation.clump(tmpInitClearSkyRegions, tmpInitClearSkyRegionsClumps, 'KEA', False, 0.0, False)
    
    rsgislib.rastergis.populateStats(tmpInitClearSkyRegionsClumps, True, True)
    
    rsgislib.segmentation.rmSmallClumps(tmpInitClearSkyRegionsClumps, tmpInitClearSkyRegionsRmSmall, initClearSkyRegionMinSize, 'KEA')
    
    rsgislib.segmentation.relabelClumps(tmpInitClearSkyRegionsRmSmall, tmpInitClearSkyRegionsFinal, 'KEA', False)
    
    rsgislib.rastergis.populateStats(tmpInitClearSkyRegionsFinal, True, True)
    
    rsgislib.imagecalc.imageMath(tmpCloudsImgDist2CloudsNoData, tmpClearSkyRegionsFullExtent, 'b1 > '+str(finalClearSkyRegionDist), outFormat, rsgislib.TYPE_32UINT)
    
    rsgislib.segmentation.clump(tmpClearSkyRegionsFullExtent, tmpClearSkyRegionsFullExtentClumps, 'KEA', False, 0.0, False)
    
    rsgislib.rastergis.populateStats(tmpClearSkyRegionsFullExtentClumps, True, True)
    
    rsgislib.rastergis.populateRATWithStats(tmpInitClearSkyRegionsFinal, tmpClearSkyRegionsFullExtentClumps, [rsgislib.rastergis.BandAttStats(band=1, maxField='InitRegionInter')])
    
    ratDataset = gdal.Open( tmpClearSkyRegionsFullExtentClumps, gdal.GA_Update )
    InitRegionInter = rat.readColumn(ratDataset, "InitRegionInter")
    ValidClumps = numpy.zeros_like(InitRegionInter, dtype=numpy.dtype('int'))
    ValidClumps[InitRegionInter>0] = 1
    rat.writeColumn(ratDataset, "ValidClumps", ValidClumps)
    ratDataset = None
    
    rsgislib.rastergis.collapseRAT(tmpClearSkyRegionsFullExtentClumps, 'ValidClumps', tmpClearSkyRegionsFullExtentSelectClumps, 'KEA', 1)
    
    rsgislib.rastergis.populateStats(tmpClearSkyRegionsFullExtentSelectClumps, True, True)
    
    rsgislib.imagemorphology.createCircularOp(outputFile=tmpMorphOperator, opSize=morphSize)
    
    rsgislib.imagemorphology.imageOpening(inputImage=tmpClearSkyRegionsFullExtentSelectClumps, outputImage=tmpClearSkyRegionsFullExtentSelectClumpsOpen, tempImage="", morphOperator=tmpMorphOperator, useOpFile=True, opSize=21, gdalFormat='KEA', dataType=rsgislib.TYPE_32UINT)
    
    rsgislib.segmentation.clump(tmpClearSkyRegionsFullExtentSelectClumpsOpen, tmpClearSkyRegionsFullExtentSelectClumpsOpenClump, 'KEA', False, 0.0, False)
    
    rsgislib.rastergis.populateStats(tmpClearSkyRegionsFullExtentSelectClumpsOpenClump, True, True)

    rsgislib.segmentation.rmSmallClumps(tmpClearSkyRegionsFullExtentSelectClumpsOpenClump, tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall, initClearSkyRegionMinSize, 'KEA')
    
    rsgislib.imagecalc.imageMath(tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall, outputClearSkyMask, "b1>0?1:0", outFormat, rsgislib.TYPE_8UINT)
        
    if deleteTmpFiles:
        rsgisUtils = rsgislib.RSGISPyUtils()
        rsgisUtils.deleteFileWithBasename(tmpCloudsImgDist2Clouds)
        rsgisUtils.deleteFileWithBasename(tmpCloudsImgDist2CloudsNoData)
        rsgisUtils.deleteFileWithBasename(tmpInitClearSkyRegions)
        rsgisUtils.deleteFileWithBasename(tmpInitClearSkyRegionsClumps)
        rsgisUtils.deleteFileWithBasename(tmpInitClearSkyRegionsRmSmall)
        rsgisUtils.deleteFileWithBasename(tmpInitClearSkyRegionsFinal)
        rsgisUtils.deleteFileWithBasename(tmpClearSkyRegionsFullExtent)
        rsgisUtils.deleteFileWithBasename(tmpClearSkyRegionsFullExtentClumps)
        rsgisUtils.deleteFileWithBasename(tmpClearSkyRegionsFullExtentSelectClumps)
        rsgisUtils.deleteFileWithBasename(tmpClearSkyRegionsFullExtentSelectClumpsOpen)
        rsgisUtils.deleteFileWithBasename(tmpClearSkyRegionsFullExtentSelectClumpsOpenClump)
        rsgisUtils.deleteFileWithBasename(tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall)
        rsgisUtils.deleteFileWithBasename(tmpMorphOperator)


