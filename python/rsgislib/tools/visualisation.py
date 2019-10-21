#!/usr/bin/env python
"""
The tools.visualisation module contains functions for aiding visualisation of data.
"""

# Import modules
import rsgislib
import rsgislib.imageutils

import os.path
import os
import shutil
import subprocess
import math


def createKMZImg(inputImg, outputFile, bands, reprojLatLong=True, finiteMsk=False):
    """
    A function to convert an input image to a KML/KMZ file, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.
    
    Where:
    
    :param inputImg: input image file (any format that gdal supports)
    :param outputFile: output image file (extension kmz for KMZ output / kml for KML output)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param reprojLatLong: specify whether the image should be explicitly reprojected to WGS84 Lat/Long before transformation to KML.
    :param finiteMsk: specify whether the image data should be masked so all values are finite before stretching.
    
    """
    
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    
    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgisUtils.uidGenerator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]
    
    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName+'_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)
    
    img2Stch = selImgBandsImg
    if finiteMsk:
        finiteMskImg = os.path.join(tmpDIR, baseName+'_FiniteMsk.kea')
        rsgislib.imageutils.genFiniteMask(selImgBandsImg, finiteMskImg, 'KEA')
        img2Stch = os.path.join(tmpDIR, baseName+'_Msk2FiniteRegions.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, finiteMskImg, img2Stch, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), 0, 0)
    
    stretchImg = os.path.join(tmpDIR, baseName+'_stretch.kea')
    rsgislib.imageutils.stretchImage(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    
    gdalInFile = stretchImg
    if reprojLatLong:
        latLongImg = os.path.join(tmpDIR, baseName+'_latlong.kea')
        outWKT = os.path.join(tmpDIR, baseName+'_latlong.wkt')
        rsgisUtils.writeList2File(['GEOGCS["WGS_1984",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.2572235630016],TOWGS84[0,0,0,0,0,0,0]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433],AUTHORITY["EPSG","4326"]]'], outWKT)
        rsgislib.imageutils.reprojectImage(stretchImg, latLongImg, outWKT, gdalformat='KEA', interp='cubic', inWKT=None, noData=0.0, outPxlRes='auto', snap2Grid=True)
        gdalInFile = latLongImg
    
    cmd = 'gdal_translate -of KMLSUPEROVERLAY ' + gdalInFile + ' ' + outputFile
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
       raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
        
    shutil.rmtree(tmpDIR)


def createWebTilesImg(inputImg, outputDIR, bands, zoomLevels='2-10', resample='average', finiteMsk=False):
    """
    A function to convert an input image to a tile cache for web map servers, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.
    
    Where:
    
    :param inputImg: input image file (any format that gdal supports)
    :param outputDIR: output directory within which the cache will be created.
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param zoomLevels: The zoom levels to be created for the web tile cache.
    :param resample: Method of resampling (average,near,bilinear,cubic,cubicspline,lanczos,antialias)
    :param finiteMsk: specify whether the image data should be masked so all values are finite before stretching.
    
    """
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    
    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgisUtils.uidGenerator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]
    
    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName+'_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)
    
    img2Stch = selImgBandsImg
    if finiteMsk:
        finiteMskImg = os.path.join(tmpDIR, baseName+'_FiniteMsk.kea')
        rsgislib.imageutils.genFiniteMask(selImgBandsImg, finiteMskImg, 'KEA')
        img2Stch = os.path.join(tmpDIR, baseName+'_Msk2FiniteRegions.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, finiteMskImg, img2Stch, 'KEA', rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), 0, 0)
    
    stretchImg = os.path.join(tmpDIR, baseName+'_stretch.kea')
    rsgislib.imageutils.stretchImage(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    
    cmd = 'gdal2tiles.py -r ' + resample + ' -z ' + zoomLevels + ' -a  0 ' + stretchImg + ' ' + outputDIR
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
       raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
        
    shutil.rmtree(tmpDIR)


def createWebTilesImg(inputImg, bands, outputDIR, zoomLevels='2-10', img_stats_msk=None, img_msk_vals=1, tmp_dir=None,
                      webview=True):
    """
    A function to produce

    :param inputImg: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param outputDIR: output directory within which the cache will be created.
    :param zoomLevels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param webview: Provide default GDAL leaflet web viewer.

    """
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    img_no_data_val = rsgisUtils.getImageNoDataValue(inputImg)

    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgisUtils.uidGenerator())
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, rsgisUtils.uidGenerator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA',
                                             rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretchImage(img2StchMskd, stretchImgTmp, True, stretchImgStats, True, False, 'KEA',
                                         rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretchImageWithStats(img2Stch, stretchImg, stretchImgStats, 'KEA', rsgislib.TYPE_8UINT,
                                                  rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretchImage(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT,
                                         rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    webview_opt = 'none'
    if webview:
        webview_opt = 'leaflet'

    cmd = 'gdal2tiles.py -r average -z {0} -a 0 -w {1} {2} {3}'.format(zoomLevels, webview_opt, stretchImg, outputDIR)
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(tmpDIR)


def createQuicklookImgs(inputImg, bands, outputImgs='quicklook.jpg', output_img_sizes=250, img_stats_msk=None, img_msk_vals=1, tmp_dir=None):
    """
    A function to produce

    :param inputImg: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param outputImgs: a single output image or list of output images. The same size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels; of x axis) or list of output image sizes.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.

    """
    n_out_imgs = 1
    if type(outputImgs) is list:
        n_out_imgs = len(outputImgs)
        if type(output_img_sizes) is not list:
            raise Exception("If the outputImgs input is a list so must output_img_sizes.")
            if len(output_img_sizes) != n_out_imgs:
                raise Exception("outputImgs and output_img_sizes must be the same length")

        if n_out_imgs == 1:
            outputImgs = outputImgs[0]
            output_img_sizes = output_img_sizes[0]

    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    rsgisUtils = rsgislib.RSGISPyUtils()
    nImgBands = rsgisUtils.getImageBandCount(inputImg)
    img_no_data_val = rsgisUtils.getImageNoDataValue(inputImg)

    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgisUtils.uidGenerator())
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, rsgisUtils.uidGenerator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(inputImg))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = inputImg
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = inputImg
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.selectImageBands(inputImg, selImgBandsImg, 'KEA',
                                             rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.maskImage(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgisUtils.getRSGISLibDataTypeFromImg(inputImg), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretchImage(img2StchMskd, stretchImgTmp, True, stretchImgStats, True, False, 'KEA',
                                         rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretchImageWithStats(img2Stch, stretchImg, stretchImgStats, 'KEA', rsgislib.TYPE_8UINT,
                                                  rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretchImage(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT,
                                         rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)



    if n_out_imgs == 1:
        cmd = 'gdal_translate -of JPEG -ot Byte -scale -outsize {0} 0 -r average {1} {2}'.format(output_img_sizes,
                                                                                                stretchImg, outputImgs)
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            cmd = 'gdal_translate -of JPEG -ot Byte -scale -outsize {0} 0 -r average {1} {2}'.format(output_img_sizes[i],
                                                                                                    stretchImg,
                                                                                                    outputImgs[i])
            print(cmd)
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(tmpDIR)

