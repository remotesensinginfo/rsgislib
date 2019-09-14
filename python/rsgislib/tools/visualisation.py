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
    :param reprojLatLong: specify whether the image should be explicitly reprojected to WGS84 Lat/Long before transformation to KML.
    :param finiteMsk: specify whether the image data should be masked so all values are finite before stretching.
    :param resample: Method of resampling (average,near,bilinear,cubic,cubicspline,lanczos,antialias)
    
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