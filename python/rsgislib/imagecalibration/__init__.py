"""
The image calibration module contains functions for calibrating optical data from DN to radience and top of atmosphere reflectance and, using coefficients from 6S, surface reflectance.

For obtaining the correct parameters for each function it is recomented that rather than running directly they are called through the Atmospheric and Radiometric Correction of Satellite Imagery (ARCSI) software. 

More details on ARCSI are avaialble from http://rsgislib.org/arcsi
"""

# import the C++ extension into this level
from ._imagecalibration import *


class ElevLUTFeat(object):
    """
Create a list of these objects to pass to the rsgislib.imagecalibration.apply6SCoeffElevLUTParam

* Elev - is the elevation value.
* Coeffs - is the 6S coeffients as a Band6S object. 

"""
    def __init__(self, Elev=None, Coeffs=None):
        """
        * Elev - is the elevation value.
        * Coeffs - is the 6S coeffients as a Band6S object. 
        """
        self.Elev = Elev
        self.Coeffs = Coeffs


class AOTLUTFeat(object):
    """
Create a list of these objects to pass to the rsgislib.imagecalibration.apply6SCoeffElevAOTLUTParam

* AOT - is the AOT value.
* Coeffs - is the 6S coeffients as a Band6S object. 

"""
    def __init__(self, AOT=None, Coeffs=None):
        """
        * AOT - is the AOT value.
        * Coeffs - is the 6S coeffients as a Band6S object. 
        """
        self.AOT = AOT
        self.Coeffs = Coeffs
        
class Band6SCoeff(object):
    """
Create a list of these objects to provide the Coeffs for ElevLUTFeat and AOTLUTFeat.

"""
    def __init__(self, band=None, aX=None, bX=None, cX=None, DirIrr=None, DifIrr=None, EnvIrr=None):
        """
        """
        self.band = band
        self.aX = aX
        self.bX = bX
        self.cX = cX
        self.DirIrr = DirIrr
        self.DifIrr = DifIrr
        self.EnvIrr = EnvIrr
        


def performDOSCalc(inputFile, outputFile, gdalformat='KEA', nonNegative=True, noDataVal=0, darkObjReflVal=0, darkObjPercentile=0.01, copyBandNames=True, calcStatsPyd=True):
    """
A command to perform a dark object subtraction (DOS) on an input image.

* inputFile - input image to which the DOS method is to be applied. Typically, this image with be in top of atmosphere reflectance (TOA)
* outputFile - the output image file
* gdalformat - the output image file format (default = KEA)
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
    
    applySubtractSingleOffsets(inputFile, outputFile, gdalformat, outDataType, nonNegative, True, noDataVal, darkObjReflVal, offsetsList)
    
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
    
    rsgislib.imagemorphology.imageOpening(inputImage=tmpClearSkyRegionsFullExtentSelectClumps, outputImage=tmpClearSkyRegionsFullExtentSelectClumpsOpen, tempImage="", morphOperator=tmpMorphOperator, useOpFile=True, opSize=21, gdalformat='KEA', dataType=rsgislib.TYPE_32UINT)
    
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


def getESUNValue(radiance, toaRefl, day, month, year, solarZenith):
    """
Get the ESUN value where a radiance and TOA Reflectance value are known for a pixel.

* radiance:
* toaRefl:
* day:
* month:
* year:
* solarZenith:

"""
    import rsgislib.imagecalibration
    import math
    julianDay = rsgislib.imagecalibration.getJulianDay(year, month, day)
    solarDist = rsgislib.imagecalibration.calcSolarDistance(julianDay)
    # pi * L * d2
    step1 = math.pi * radiance * (solarDist * solarDist)
    # step1 / toaRefl
    step2 = step1 / toaRefl
    # step2 / cos(solarZenith)
    esun = step2 / math.cos(math.radians(solarZenith))
    return esun



def createEstimateSREFSurface(inputTOAImg, imgBands, bandRescale, winSize, outImage, gdalformat, dataType, tmpDIR):
    """
    Estimate SREF surface from TOA input image using the method details in He, K., Sun, J., & Tang, X. (2011). 
    'Single image haze removal using dark channel prior'. IEEE Transactions on Pattern Analysis and Machine Intelligence.
    Method details were also identified with reference to https://www.kaggle.com/ivanl1/haze-removal-using-dark-channel-prior
    
    **** WARNING DO NOT USE - THIS FUNCTION IS STILL A WORK IN PROGRESS ****
    
    """
    raise Exception("This function is not yet ready - needs more thought as how to get a fully working implementation.")
    import rsgislib
    import rsgislib.imagecalc
    import rsgislib.imageutils
    
    import os.path
    import shutil
    
    from rios import applier
    from rios import cuiprogress
    
    import numpy
    
    baseName = os.path.splitext(os.path.basename(inputTOAImg))[0]    
    tmpImgDIR = os.path.join(tmpDIR, baseName)
    
    tmpPresent = True
    if not os.path.exists(tmpImgDIR):
        os.makedirs(tmpImgDIR)
        tmpPresent = False 
    
    ###############################################
    ########## NEEDS TO CHANGE SCALING ############
    ###############################################
    #
    # The scaling used seems to have a significant 
    # effect on the results. Should a histogram or
    # standard deviation approach be adopted?
    #
    ###############################################
    
    # Rescale input TOA 
    rescaledInputImg = os.path.join(tmpImgDIR, baseName+'_rescaled.kea')
    rsgislib.imagecalc.rescaleImgPxlVals(inputTOAImg, rescaledInputImg, 'KEA', rsgislib.TYPE_32FLOAT, bandRescale, trim2Limits=True)
    
    # Calculate the minium value (across all bands) within the window.
    minValInWinImg = os.path.join(tmpImgDIR, baseName+'_minValInWin.kea')
    minValInWinRefImg = os.path.join(tmpImgDIR, baseName+'_minValInWinRef.kea')
    rsgislib.imagecalc.identifyMinPxlValueInWin(rescaledInputImg, minValInWinImg, minValInWinRefImg, imgBands, winSize, 'KEA', nodataval=0.0, usenodata=True)
    
    ###############################################
    ########## NEEDS TO ADD FILTERING #############
    ###############################################
    #
    # Published paper uses the Soft Matting algorithm 
    # but this uses a lot of memory etc. so prob not
    # suitable.
    #
    # A Guided Filter is an alternative and should be 
    # implmented in RSGISLIb - would generally be useful.
    #
    ###############################################
    
    # Calculate the 99th percentile.
    percentVals = rsgislib.imagecalc.bandPercentile(minValInWinImg, 0.99, 0)
    
    # Create mask of regions over 99th percentile
    atmosMaskImg = os.path.join(tmpImgDIR, baseName+'_atmosMask.kea')
    rsgislib.imagecalc.imageMath(minValInWinImg, atmosMaskImg, '(b1==0)?0:(b1>='+str(percentVals[0])+')?1:0', 'KEA', rsgislib.TYPE_8UINT)
    
    # Calculate the mean value within the atmospheric light mask
    imgMeanVal = rsgislib.imagecalc.calcImgMeanInMask(rescaledInputImg, atmosMaskImg, 1, imgBands, 1, True)
    
    # Calculate the atmospheric transmission
    # transmission = 1 - omega * imgDark / atomsphericLight
    transImg = os.path.join(tmpImgDIR, baseName+'_transmission.kea')
    rsgislib.imagecalc.imageMath(minValInWinImg, transImg, '(b1==0)?0:(1 - (1 * (b1/'+str(imgMeanVal)+')))<0.025?0.025:(1 - (1 * (b1/'+str(imgMeanVal)+')))', 'KEA', rsgislib.TYPE_32FLOAT)
    
    #####################################################################
    # Calculate the scaled SREF pixel values.
    srefScaledImg = os.path.join(tmpImgDIR, baseName+'_scaledSREF.kea')

    infiles = applier.FilenameAssociations()
    infiles.toaScaledImage = rescaledInputImg
    infiles.transImg = transImg
    outfiles = applier.FilenameAssociations()
    outfiles.outimage = srefScaledImg
    otherargs = applier.OtherInputs()
    otherargs.atmosLight = imgMeanVal
    otherargs.imgBands = imgBands
    otherargs.numpyDT = numpy.float32
    aControls = applier.ApplierControls()
    aControls.progress = cuiprogress.CUIProgressBar()
    aControls.drivername = 'KEA'
    aControls.omitPyramids = True
    aControls.calcStats = False
    
    def _calcScaleSREFVals(info, inputs, outputs, otherargs):
        # This is an internal rios function 
        outImgShape = [len(otherargs.imgBands), inputs.transImg.shape[1], inputs.transImg.shape[2]]
        outputs.outimage = numpy.zeros(outImgShape, dtype=otherargs.numpyDT)
        for idx in range(len(imgBands)):
            bandIdx = imgBands[idx]-1
            outputs.outimage[idx] = numpy.where(inputs.toaScaledImage[bandIdx] == 0.0, 0.0, (((inputs.toaScaledImage[bandIdx]-otherargs.atmosLight))/inputs.transImg[0])+otherargs.atmosLight)

    applier.apply(_calcScaleSREFVals, infiles, outfiles, otherargs, controls=aControls)
    # End of RIOS Section
    #####################################################################
    
    ###############################################
    ########## NEEDS TO CHANGE SCALING ############
    ###############################################
    #
    # How to reliably rescale the results back to 
    # to the original range. (I guess we need the
    # original data range rather than the editted
    # range from the stretch?)
    #
    ###############################################
    
    # Rescale the SREF image
    bandRescaleSREF = []
    outBand = 1
    for band in imgBands:
        bandRescaleSREF.append(rsgislib.imagecalc.ImageBandRescale(outBand, bandRescale[band-1].outMin, bandRescale[band-1].outMax, bandRescale[band-1].outNoData, 5, (bandRescale[band-1].inMax-bandRescale[band-1].inMin), 0))
        outBand = outBand + 1
        
    rsgislib.imagecalc.rescaleImgPxlVals(srefScaledImg, outImage, gdalformat, dataType, bandRescaleSREF, trim2Limits=True)
    rsgislib.imageutils.popImageStats(outImage, usenodataval=True, nodataval=0, calcpyramids=True)
    
    if not tmpPresent:
        shutil.rmtree(tmpImgDIR, ignore_errors=True)
