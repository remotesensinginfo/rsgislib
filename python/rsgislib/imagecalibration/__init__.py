#!/usr/bin/env python
"""
The image calibration module contains functions for calibrating optical data from DN to radience and top of atmosphere reflectance and, using coefficients from 6S, surface reflectance.

For obtaining the correct parameters for each function it is recomented that rather than running directly they are called through the Atmospheric and Radiometric Correction of Satellite Imagery (ARCSI) software. 

More details on ARCSI are avaialble from https://remotesensing.info/arcsi
"""

# import the C++ extension into this level
from ._imagecalibration import *

import rsgislib


class ElevLUTFeat(object):
    """
    Create a list of these objects to pass to the rsgislib.imagecalibration.apply6SCoeffElevLUTParam

    :param Elev: is the elevation value.
    :param Coeffs: is the 6S coeffients as a Band6S object.
    """

    def __init__(self, Elev=None, Coeffs=None):
        """
        :param Elev: is the elevation value.
        :param Coeffs: is the 6S coeffients as a Band6S object.
        """
        self.Elev = Elev
        self.Coeffs = Coeffs


class AOTLUTFeat(object):
    """
    Create a list of these objects to pass to the rsgislib.imagecalibration.apply6SCoeffElevAOTLUTParam

    :param AOT: is the AOT value.
    :param Coeffs: is the 6S coeffients as a Band6S object.
    """

    def __init__(self, AOT=None, Coeffs=None):
        """
        :param AOT: is the AOT value.
        :param Coeffs: is the 6S coeffients as a Band6S object.
        """
        self.AOT = AOT
        self.Coeffs = Coeffs


class Band6SCoeff(object):
    """
    Create a list of these objects to provide the Coeffs for ElevLUTFeat and AOTLUTFeat.
    """

    def __init__(
        self,
        band=None,
        aX=None,
        bX=None,
        cX=None,
        DirIrr=None,
        DifIrr=None,
        EnvIrr=None,
    ):
        self.band = band
        self.aX = aX
        self.bX = bX
        self.cX = cX
        self.DirIrr = DirIrr
        self.DifIrr = DifIrr
        self.EnvIrr = EnvIrr


def perform_dos_calc(
    inputFile,
    outputFile,
    gdalformat="KEA",
    nonNegative=True,
    noDataVal=0,
    darkObjReflVal=0,
    darkObjPercentile=0.01,
    copyBandNames=True,
    calcStatsPyd=True,
):
    """
    A command to perform a dark object subtraction (DOS) on an input image.

    :param inputFile: input image to which the DOS method is to be applied. Typically, this image with be in top of atmosphere reflectance (TOA)
    :param outputFile: the output image file
    :param gdalformat: the output image file format (default = KEA)
    :param nonNegative: is a boolean specifying where negative output pixel values will be accepted (Dafualt is True; i.e., no negative values)
    :param noDataVal: is the no data value within the input image file.
    :param darkObjReflVal: is an offset which is applied to all pixel values to make a minimum reflectance value (Default = 0)
    :param darkObjPercentile: is the percentile of the input image used to define the dark object threshold, range is 0 - 1 (Default is 0.01; i.e., 1%).
    :param copyBandNames: is a boolean specifying that the band names of the input image should be copied to the output image file (Default: True)
    :param calcStatsPyd: is a boolean specifying that the image stats and pyramids should be calculated on the output image (Default: True)

    .. code:: python

        import rsgislib.imagecalibration
        rsgislib.imagecalibration.perform_dos_calc("LS5TM_20110701_lat52lon421_r24p204_rad_toa.kea", 'LS5TM_20110701_lat52lon421_r24p204_rad_toa_dos.kea")

    """
    import rsgislib
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import collections

    outDataType = rsgislib.imageutils.get_rsgislib_datatype_from_img(inputFile)

    percentList = rsgislib.imagecalc.calc_band_percentile(
        inputFile, darkObjPercentile, noDataVal
    )

    offsetsList = list()
    OffVal = collections.namedtuple("DOSOffset", ["offset"])
    for val in percentList:
        offsetsList.append(OffVal(offset=val))

    apply_subtract_single_offsets(
        inputFile,
        outputFile,
        gdalformat,
        outDataType,
        nonNegative,
        True,
        noDataVal,
        darkObjReflVal,
        offsetsList,
    )

    if copyBandNames:
        bandNames = rsgislib.imageutils.getBandNames(inputFile)
        rsgislib.imageutils.set_band_names(outputFile, bandNames)

    if calcStatsPyd:
        rsgislib.imageutils.pop_img_stats(
            outputFile, use_no_data=True, no_data_val=noDataVal, calc_pyramids=True
        )


def calc_clear_sky_regions(
    cloudsImg,
    validAreaImg,
    outputClearSkyMask,
    outFormat,
    tmpPath="tmpClearSky",
    deleteTmpFiles=True,
    initClearSkyRegionDist=5000,
    initClearSkyRegionMinSize=3000,
    finalClearSkyRegionDist=1000,
    morphSize=21,
):
    """
    Given a cloud mask, identify the larger extent regions of useful clear-sky regions.

    :param cloudsImg: An image with the input mask of the cloud (pixel == 1) and shadow (pixel == 2)
    :param validAreaImg: A mask of the image data area (1 = valid and 0 = not-valid; i.e., outside of the data area)
    :param outputClearSkyMask: The output mask of the clear sky areas
    :param outFormat: The output image format.
    :param tmpPath: The path for temporay images produced during the processing to be stored (Default: 'tmpClearSky'; Note. all temp files are generated as KEA files).
    :param deleteTmpFiles: Boolean as to whether the intermediate files should be deleted following processing (Default: True - delete files).
    :param initClearSkyRegionDist: The distance in metres from a cloud/shadow object for the initial identification of clear sky regions (Default: 5000)
    :param initClearSkyRegionMinSize: The minimum size (in pixels) of the initial clear sky regions (Default: 3000 pixels)
    :param finalClearSkyRegionDist: The distance in metres from a cloud/shadow object for the final boundaries of the clear sky regions (Default: 1000)
    :param morphSize: the size of the circular morphological operator used to tidy up the result (Default: 21)

    .. code:: python

        import rsgislib.imagecalibration
        cloudsImg = "Outputs/LS8_20160605_lat52lon261_r24p203_clouds.kea"
        validAreaImg = "Outputs/LS8_20160605_lat52lon261_r24p203_valid.kea"
        outputMask = "Outputs/LS8_20160605_lat52lon261_r24p203_openskyvalid.kea"
        tmpPath = "temp"
        rsgislib.imagecalibration.calc_clear_sky_regions(cloudsImg, validAreaImg, outputMask, 'KEA', tmpPath)

    """

    import rsgislib
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.segmentation
    import rsgislib.rastergis
    import rsgislib.imagemorphology
    import os
    from osgeo import gdal
    from rios import rat
    import numpy

    if morphSize % 2 == 0:
        raise rsgislib.RSGISPyException(
            "The size of the morphology operator must be odd."
        )

    baseDataName = os.path.splitext(os.path.basename(cloudsImg))[0]
    tmpCloudsImgDist2Clouds = os.path.join(tmpPath, baseDataName + "_dist2clouds.kea")
    tmpCloudsImgDist2CloudsNoData = os.path.join(
        tmpPath, baseDataName + "_dist2clouds_masked.kea"
    )
    tmpInitClearSkyRegions = os.path.join(tmpPath, baseDataName + "initclearsky.kea")
    tmpInitClearSkyRegionsClumps = os.path.join(
        tmpPath, baseDataName + "initclearskyClumps.kea"
    )
    tmpInitClearSkyRegionsRmSmall = os.path.join(
        tmpPath, baseDataName + "initclearskyClumpsRMSmall.kea"
    )
    tmpInitClearSkyRegionsFinal = os.path.join(
        tmpPath, baseDataName + "initclearskyClumpsFinal.kea"
    )
    tmpClearSkyRegionsFullExtent = os.path.join(
        tmpPath, baseDataName + "clearskyClumpsFullExtent.kea"
    )
    tmpClearSkyRegionsFullExtentClumps = os.path.join(
        tmpPath, baseDataName + "clearskyClumpsFullExtentClumps.kea"
    )
    tmpClearSkyRegionsFullExtentSelectClumps = os.path.join(
        tmpPath, baseDataName + "clearskyClumpsFullExtentSelectClumps.kea"
    )
    tmpClearSkyRegionsFullExtentSelectClumpsOpen = os.path.join(
        tmpPath, baseDataName + "clearskyClumpsFullExtentSelectClumpsOpen.kea"
    )
    tmpClearSkyRegionsFullExtentSelectClumpsOpenClump = os.path.join(
        tmpPath, baseDataName + "clearskyClumpsFullExtentSelectClumpsOpenClump.kea"
    )
    tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall = os.path.join(
        tmpPath,
        baseDataName + "clearskyClumpsFullExtentSelectClumpsOpenClumpRMSmall.kea",
    )
    tmpMorphOperator = os.path.join(tmpPath, "CircularMorphOp.gmtxt")

    rsgislib.imagecalc.calc_dist_to_img_vals(
        cloudsImg, tmpCloudsImgDist2Clouds, pxl_vals=[1, 2], out_no_data_val=-9999
    )

    rsgislib.imageutils.mask_img(
        tmpCloudsImgDist2Clouds,
        validAreaImg,
        tmpCloudsImgDist2CloudsNoData,
        "KEA",
        rsgislib.TYPE_32INT,
        -1,
        0,
    )

    rsgislib.imagecalc.image_math(
        tmpCloudsImgDist2CloudsNoData,
        tmpInitClearSkyRegions,
        "b1 > " + str(initClearSkyRegionDist),
        outFormat,
        rsgislib.TYPE_32UINT,
    )

    rsgislib.segmentation.clump(
        tmpInitClearSkyRegions, tmpInitClearSkyRegionsClumps, "KEA", False, 0.0, False
    )

    rsgislib.rastergis.pop_rat_img_stats(tmpInitClearSkyRegionsClumps, True, True)

    rsgislib.segmentation.rm_small_clumps(
        tmpInitClearSkyRegionsClumps,
        tmpInitClearSkyRegionsRmSmall,
        initClearSkyRegionMinSize,
        "KEA",
    )

    rsgislib.segmentation.relabel_clumps(
        tmpInitClearSkyRegionsRmSmall, tmpInitClearSkyRegionsFinal, "KEA", False
    )

    rsgislib.rastergis.pop_rat_img_stats(tmpInitClearSkyRegionsFinal, True, True)

    rsgislib.imagecalc.image_math(
        tmpCloudsImgDist2CloudsNoData,
        tmpClearSkyRegionsFullExtent,
        "b1 > " + str(finalClearSkyRegionDist),
        outFormat,
        rsgislib.TYPE_32UINT,
    )

    rsgislib.segmentation.clump(
        tmpClearSkyRegionsFullExtent,
        tmpClearSkyRegionsFullExtentClumps,
        "KEA",
        False,
        0.0,
        False,
    )

    rsgislib.rastergis.pop_rat_img_stats(tmpClearSkyRegionsFullExtentClumps, True, True)

    rsgislib.rastergis.populate_rat_with_stats(
        tmpInitClearSkyRegionsFinal,
        tmpClearSkyRegionsFullExtentClumps,
        [rsgislib.rastergis.BandAttStats(band=1, max_field="InitRegionInter")],
    )

    ratDataset = gdal.Open(tmpClearSkyRegionsFullExtentClumps, gdal.GA_Update)
    InitRegionInter = rat.readColumn(ratDataset, "InitRegionInter")
    ValidClumps = numpy.zeros_like(InitRegionInter, dtype=numpy.dtype("int"))
    ValidClumps[InitRegionInter > 0] = 1
    rat.writeColumn(ratDataset, "ValidClumps", ValidClumps)
    ratDataset = None

    rsgislib.rastergis.collapse_rat(
        tmpClearSkyRegionsFullExtentClumps,
        "ValidClumps",
        tmpClearSkyRegionsFullExtentSelectClumps,
        "KEA",
        1,
    )

    rsgislib.rastergis.pop_rat_img_stats(
        tmpClearSkyRegionsFullExtentSelectClumps, True, True
    )

    rsgislib.imagemorphology.create_circular_op(
        output_file=tmpMorphOperator, op_size=morphSize
    )

    rsgislib.imagemorphology.image_opening(
        input_img=tmpClearSkyRegionsFullExtentSelectClumps,
        output_img=tmpClearSkyRegionsFullExtentSelectClumpsOpen,
        tmp_img="",
        morph_op_file=tmpMorphOperator,
        use_op_file=True,
        op_size=21,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_32UINT,
    )

    rsgislib.segmentation.clump(
        tmpClearSkyRegionsFullExtentSelectClumpsOpen,
        tmpClearSkyRegionsFullExtentSelectClumpsOpenClump,
        "KEA",
        False,
        0.0,
        False,
    )

    rsgislib.rastergis.pop_rat_img_stats(
        tmpClearSkyRegionsFullExtentSelectClumpsOpenClump, True, True
    )

    rsgislib.segmentation.rm_small_clumps(
        tmpClearSkyRegionsFullExtentSelectClumpsOpenClump,
        tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall,
        initClearSkyRegionMinSize,
        "KEA",
    )

    rsgislib.imagecalc.image_math(
        tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall,
        outputClearSkyMask,
        "b1>0?1:0",
        outFormat,
        rsgislib.TYPE_8UINT,
    )

    if deleteTmpFiles:
        import rsgislib.tools.filetools

        rsgislib.tools.filetools.delete_file_with_basename(tmpCloudsImgDist2Clouds)
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpCloudsImgDist2CloudsNoData
        )
        rsgislib.tools.filetools.delete_file_with_basename(tmpInitClearSkyRegions)
        rsgislib.tools.filetools.delete_file_with_basename(tmpInitClearSkyRegionsClumps)
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpInitClearSkyRegionsRmSmall
        )
        rsgislib.tools.filetools.delete_file_with_basename(tmpInitClearSkyRegionsFinal)
        rsgislib.tools.filetools.delete_file_with_basename(tmpClearSkyRegionsFullExtent)
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpClearSkyRegionsFullExtentClumps
        )
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpClearSkyRegionsFullExtentSelectClumps
        )
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpClearSkyRegionsFullExtentSelectClumpsOpen
        )
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpClearSkyRegionsFullExtentSelectClumpsOpenClump
        )
        rsgislib.tools.filetools.delete_file_with_basename(
            tmpClearSkyRegionsFullExtentSelectClumpsOpenClumpRMSmall
        )
        rsgislib.tools.filetools.delete_file_with_basename(tmpMorphOperator)


def get_esun_value(radiance, toaRefl, day, month, year, solarZenith):
    """
    Get the ESUN value where a radiance and TOA Reflectance value are known for a pixel.

    :param radiance: input at sensor radiance value.
    :param toaRefl: input the known at sensor (top of atmosphere) reflectance value for the given radiance.
    :param day: input the day of the acquisition.
    :param month: input the month of the acquisition.
    :param year: input the year of the acquisition.
    :param solarZenith: input the solar zenith angle for the acquisition.
    :returns: esun radiance
    """
    import rsgislib.imagecalibration
    import math

    julianDay = rsgislib.imagecalibration.get_julian_day(year, month, day)
    solarDist = rsgislib.imagecalibration.calc_solar_distance(julianDay)
    # pi * L * d2
    step1 = math.pi * radiance * (solarDist * solarDist)
    # step1 / toaRefl
    step2 = step1 / toaRefl
    # step2 / cos(solarZenith)
    esun = step2 / math.cos(math.radians(solarZenith))
    return esun


"""
def createEstimateSREFSurface(inputTOAImg, imgBands, bandRescale, winSize, outImage, gdalformat, dataType, tmpDIR):
    ###
    Estimate SREF surface from TOA input image using the method details in He, K., Sun, J., & Tang, X. (2011). 
    'Single image haze removal using dark channel prior'. IEEE Transactions on Pattern Analysis and Machine Intelligence.
    Method details were also identified with reference to https://www.kaggle.com/ivanl1/haze-removal-using-dark-channel-prior
    
    **WARNING DO NOT USE - THIS FUNCTION IS STILL A WORK IN PROGRESS**
    
    ###
    raise rsgislib.RSGISPyException("This function is not yet ready - needs more thought as how to get a fully working implementation.")
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
    rsgislib.imagecalc.rescale_img_pxl_vals(inputTOAImg, rescaledInputImg, 'KEA', rsgislib.TYPE_32FLOAT, bandRescale, trim2Limits=True)
    
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
    percentVals = rsgislib.imagecalc.calc_band_percentile(minValInWinImg, 0.99, 0)
    
    # Create mask of regions over 99th percentile
    atmosMaskImg = os.path.join(tmpImgDIR, baseName+'_atmosMask.kea')
    rsgislib.imagecalc.image_math(minValInWinImg, atmosMaskImg, '(b1==0)?0:(b1>='+str(percentVals[0])+')?1:0', 'KEA', rsgislib.TYPE_8UINT)
    
    # Calculate the mean value within the atmospheric light mask
    imgMeanVal = rsgislib.imagecalc.calcImgMeanInMask(rescaledInputImg, atmosMaskImg, 1, imgBands, 1, True)
    
    # Calculate the atmospheric transmission
    # transmission = 1 - omega * imgDark / atomsphericLight
    transImg = os.path.join(tmpImgDIR, baseName+'_transmission.kea')
    rsgislib.imagecalc.image_math(minValInWinImg, transImg, '(b1==0)?0:(1 - (1 * (b1/'+str(imgMeanVal)+')))<0.025?0.025:(1 - (1 * (b1/'+str(imgMeanVal)+')))', 'KEA', rsgislib.TYPE_32FLOAT)
    
    #####################################################################
    # Calculate the scaled SREF pixel values.
    srefScaledImg = os.path.join(tmpImgDIR, baseName+'_scaledSREF.kea')
    
    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        progress_bar = cuiprogress.GDALProgressBar()
    
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
    aControls.progress = progress_bar
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
        
    rsgislib.imagecalc.rescale_img_pxl_vals(srefScaledImg, outImage, gdalformat, dataType, bandRescaleSREF, trim2Limits=True)
    rsgislib.imageutils.pop_img_stats(outImage, use_no_data=True, no_data_val=0, calc_pyramids=True)
    
    if not tmpPresent:
        shutil.rmtree(tmpImgDIR, ignore_errors=True)
"""
