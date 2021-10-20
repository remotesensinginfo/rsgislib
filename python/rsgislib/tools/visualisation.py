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

def create_kmz_img(inputImg, outputFile, bands, reprojLatLong=True, finiteMsk=False):
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
    import rsgislib.tools.utils
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')

    nImgBands = rsgislib.imageutils.get_img_band_count(inputImg)

    uid_str = rsgislib.tools.utils.uid_generator()
    tmpDIR = os.path.join(os.path.dirname(inputImg), uid_str)
    if not os.path.exists(tmpDIR):
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
        rsgislib.imageutils.select_img_bands(inputImg, selImgBandsImg, 'KEA', rsgislib.imageutils.get_rsgislib_datatype_from_img(inputImg), sBands)
    
    img2Stch = selImgBandsImg
    if finiteMsk:
        finiteMskImg = os.path.join(tmpDIR, baseName+'_FiniteMsk.kea')
        rsgislib.imageutils.gen_finite_mask(selImgBandsImg, finiteMskImg, 'KEA')
        img2Stch = os.path.join(tmpDIR, baseName+'_Msk2FiniteRegions.kea')
        rsgislib.imageutils.mask_img(selImgBandsImg, finiteMskImg, img2Stch, 'KEA', rsgislib.imageutils.get_rsgislib_datatype_from_img(inputImg), 0, 0)
    
    stretchImg = os.path.join(tmpDIR, baseName+'_stretch.kea')
    rsgislib.imageutils.stretch_img(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    
    gdalInFile = stretchImg
    if reprojLatLong:
        latLongImg = os.path.join(tmpDIR, baseName+'_latlong.kea')
        outWKT = os.path.join(tmpDIR, baseName+'_latlong.wkt')
        rsgislib.tools.utils.write_list_to_file(['GEOGCS["WGS_1984",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.2572235630016],TOWGS84[0,0,0,0,0,0,0]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433],AUTHORITY["EPSG","4326"]]'], outWKT)
        rsgislib.imageutils.reprojectImage(stretchImg, latLongImg, outWKT, gdalformat='KEA', interp='cubic', inWKT=None, noData=0.0, outPxlRes='auto', snap2Grid=True)
        gdalInFile = latLongImg
    
    cmd = 'gdal_translate -of KMLSUPEROVERLAY ' + gdalInFile + ' ' + outputFile
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
       raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
        
    shutil.rmtree(tmpDIR)


def create_web_tiles_img_no_stats_msk(input_img, out_dir, bands, zoom_levels='2-10', resample='average', finite_msk=False, tms=True):
    """
    A function to convert an input image to a tile cache for web map servers, where the input image
    is stretched and bands sub-selected / ordered as required for visualisation.
    
    Where:
    
    :param input_img: input image file (any format that gdal supports)
    :param out_dir: output directory within which the cache will be created.
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param zoom_levels: The zoom levels to be created for the web tile cache.
    :param resample: Method of resampling (average,near,bilinear,cubic,cubicspline,lanczos,antialias)
    :param finite_msk: specify whether the image data should be masked so all values are finite before stretching.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
                (TMS: gdal2tiles.py native. XYZ: GIS Compatible)
    
    """
    import rsgislib.tools.utils
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    nImgBands = rsgislib.imageutils.get_img_band_count(input_img)
    
    tmpDIR = os.path.join(os.path.dirname(input_img), rsgislib.tools.utils.uid_generator())
    os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(input_img))[0]
    
    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = input_img
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = input_img
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName+'_sband.kea')
        rsgislib.imageutils.select_img_bands(input_img, selImgBandsImg, 'KEA', rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), sBands)
    
    img2Stch = selImgBandsImg
    if finite_msk:
        finiteMskImg = os.path.join(tmpDIR, baseName+'_FiniteMsk.kea')
        rsgislib.imageutils.gen_finite_mask(selImgBandsImg, finiteMskImg, 'KEA')
        img2Stch = os.path.join(tmpDIR, baseName+'_Msk2FiniteRegions.kea')
        rsgislib.imageutils.mask_img(selImgBandsImg, finiteMskImg, img2Stch, 'KEA', rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), 0, 0)
    
    stretchImg = os.path.join(tmpDIR, baseName+'_stretch.kea')
    rsgislib.imageutils.stretch_img(img2Stch, stretchImg, False, '', True, False, 'KEA', rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    
    cmd = 'gdal2tiles.py -r {0} -z {1} -a  0 {2} {3}'.format(resample, zoom_levels, stretchImg, out_dir)
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
       raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    if not tms:
        from rsgislib.tools import convert_between_tms_xyz
        convert_between_tms_xyz(out_dir)

    shutil.rmtree(tmpDIR)


def create_webtiles_img(input_img, bands, out_dir, zoom_levels='2-10', img_stats_msk=None, img_msk_vals=1, tmp_dir=None, webview=True, tms=True):
    """
    A function to produce a web cache for the input image.

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param out_dir: output directory within which the cache will be created.
    :param zoom_levels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param webview: Provide default GDAL leaflet web viewer.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
                (TMS: gdal2tiles.py native. XYZ: GIS Compatible)

    """
    import rsgislib.tools.utils
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    nImgBands = rsgislib.imageutils.get_img_band_count(input_img)
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    uid_str = rsgislib.tools.utils.uid_generator()
    tmpDIR = os.path.join(os.path.dirname(input_img), uid_str)
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(input_img))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = input_img
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = input_img
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.select_img_bands(input_img, selImgBandsImg, 'KEA',
                                             rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.mask_img(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                     rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretch_img(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretch_img_with_stats(img2Stch, stretchImg, stretchImgStats, 'KEA', rsgislib.TYPE_8UINT,
                                                  img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA', rsgislib.TYPE_8UINT,
                                         rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    webview_opt = 'none'
    if webview:
        webview_opt = 'leaflet'

    cmd = 'gdal2tiles.py -r average -z {0} -a 0 -w {1} {2} {3}'.format(zoom_levels, webview_opt, stretchImg, out_dir)
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(tmpDIR)

    if not tms:
        from rsgislib.tools import convert_between_tms_xyz
        convert_between_tms_xyz(out_dir)

def create_quicklook_imgs(inputImg, bands, outputImgs='quicklook.jpg', output_img_sizes=250, scale_axis='auto',
                          img_stats_msk=None, img_msk_vals=1, stretch_file=None, tmp_dir=None):
    """
    A function to produce

    :param inputImg: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param outputImgs: a single output image or list of output images. The same size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output image sizes.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width, height or auto.
                       Auto applies the output_img_sizes to the longest of the two axes.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param stretch_file: a stretch stats file to standardise the stretch between a number of input files.
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.

    """
    import rsgislib.tools.utils
    if scale_axis not in ['width', 'height', 'auto']:
        raise rsgislib.RSGISPyException("Input parameter 'scale_axis' must have the value 'width', 'height' or 'auto'.")

    n_out_imgs = 1
    if type(outputImgs) is list:
        n_out_imgs = len(outputImgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException("If the outputImgs input is a list so must output_img_sizes.")
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException("outputImgs and output_img_sizes must be the same length")

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
    nImgBands = rsgislib.imageutils.get_img_band_count(inputImg)
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(inputImg)

    tmpDIR = os.path.join(os.path.dirname(inputImg), rsgislib.tools.utils.uid_generator())
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, rsgislib.tools.utils.uid_generator())
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
        rsgislib.imageutils.select_img_bands(inputImg, selImgBandsImg, 'KEA',
                                             rsgislib.imageutils.get_rsgislib_datatype_from_img(inputImg), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(img2Stch, stretchImg, stretch_file, 'KEA', rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    elif img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.mask_img(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgislib.imageutils.get_rsgislib_datatype_from_img(inputImg), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretch_img(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretch_img_with_stats(img2Stch, stretchImg, stretchImgStats, 'KEA',
                                                        rsgislib.TYPE_8UINT, img_no_data_val,
                                                        rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    if scale_axis == 'auto':
        x_size, y_size = rsgislib.imageutils.getImageSize(stretchImg)
        if x_size > y_size:
            scale_axis = 'width'
        else:
            scale_axis = 'height'

    if n_out_imgs == 1:
        if scale_axis == 'width':
            out_size = '-outsize {0} 0'.format(output_img_sizes)
        else:
            out_size = '-outsize 0 {0}'.format(output_img_sizes)

        cmd = 'gdal_translate -of JPEG -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg, outputImgs)
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            if scale_axis == 'width':
                out_size = '-outsize {0} 0'.format(output_img_sizes[i])
            else:
                out_size = '-outsize 0 {0}'.format(output_img_sizes[i])

            cmd = 'gdal_translate -of JPEG -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg,
                                                                                    outputImgs[i])
            print(cmd)
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(tmpDIR)


def create_mbtile_file(input_img, bands, output_mbtiles, scale_input_img=50, img_stats_msk=None, img_msk_vals=1,
                       tmp_dir=None, tile_format='PNG'):
    """
    A function to produce

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param output_mbtiles: output MBTiles file which will be created.
    :param scale_input_img: The scale of the output image with respect to the input as a percentage (e.g., 50% reduction in size).
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param tile_format: Specify the tile file format to use, options: PNG, PNG8 and JPG. Default: PNG

    """
    import rsgislib.tools.utils
    from osgeo import gdal
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    nImgBands = rsgislib.imageutils.get_img_band_count(input_img)
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    uid_str = rsgislib.tools.utils.uid_generator()
    tmpDIR = os.path.join(os.path.dirname(input_img), uid_str)
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(input_img))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = input_img
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = input_img
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.select_img_bands(input_img, selImgBandsImg, 'KEA',
                                             rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.mask_img(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretch_img(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
        rsgislib.imageutils.stretch_img_with_stats(img2Stch, stretchImg, stretchImgStats, 'KEA',
                                                        rsgislib.TYPE_8UINT, img_no_data_val,
                                                        rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    stretchImgReProj = os.path.join(tmpDIR, baseName + '_stretch_epsg3857.kea')
    rsgislib.imageutils.gdal_warp(stretchImg, stretchImgReProj, 3857, interp='near', gdalformat='KEA', options=[])

    try:
        import tqdm
        pbar = tqdm.tqdm(total=100)
        callback = lambda *args, **kw: pbar.update()
    except:
        callback = gdal.TermProgress

    trans_opt = gdal.TranslateOptions(format='MBTiles', widthPct=scale_input_img, heightPct=scale_input_img, noData=0,
                                      options=["TILE_FORMAT={}".format(tile_format)], callback=callback)
    gdal.Translate(output_mbtiles, stretchImgReProj, options=trans_opt)
    rsgislib.imageutils.pop_img_stats(output_mbtiles, use_no_data=True, no_data_val=0, calc_pyramids=True)
    shutil.rmtree(tmpDIR)


def create_webtiles_vis_gtiff_img(input_img, bands, output_dir, scaled_gtiff_img, zoomLevels='2-10', img_stats_msk=None,
                                  img_msk_vals=1, stretch_file=None, tmp_dir=None, webview=True, scale=0, tms=True):
    """
    A function to produce web cache and scaled and stretched geotiff.

    :param input_img: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param output_dir: output directory within which the cache will be created.
    :param scaled_gtiff_img: output geotiff image path and file name.
    :param zoomLevels: The zoom levels to be created for the web tile cache.
    :param img_stats_msk: Optional (default=None) input image which is used to define regions calculate
                          the image stats for stretch.
    :param img_msk_vals: The pixel(s) value define the region of interest in the image mask
                        (can be list of values or single value).
    :param stretch_file: a stretch stats file to standardise the stretch between a number of input files.
    :param tmp_dir: an input directory which can be used to write tempory files/directories. If not provided
                    (i.e., input is None) then a local directory will be define in the same folder as the input
                    image.
    :param webview: Provide default GDAL leaflet web viewer.
    :param scale: the scale output geotiff. Input is percentage in the x-axis. If zero (default) then no scaling
                  will be applied.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
                (TMS: gdal2tiles.py native. XYZ: GIS Compatible)

    """
    import rsgislib.tools.utils
    from osgeo import gdal
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')
    nImgBands = rsgislib.imageutils.get_img_band_count(input_img)
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    uid_str = rsgislib.tools.utils.uid_generator()
    tmpDIR = os.path.join(os.path.dirname(input_img), uid_str)
    if tmp_dir is not None:
        tmpDIR = os.path.join(tmp_dir, uid_str)
    if not os.path.exists(tmpDIR):
        os.makedirs(tmpDIR)
    baseName = os.path.splitext(os.path.basename(input_img))[0]

    selImgBandsImg = ''
    if (nImgBands == 1) and (not multiBand):
        selImgBandsImg = input_img
    elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
        selImgBandsImg = input_img
    else:
        sBands = []
        for strBand in bandLst:
            sBands.append(int(strBand))
        selImgBandsImg = os.path.join(tmpDIR, baseName + '_sband.kea')
        rsgislib.imageutils.select_img_bands(input_img, selImgBandsImg, 'KEA',
                                             rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), sBands)

    img2Stch = selImgBandsImg
    stretchImg = os.path.join(tmpDIR, baseName + '_stretch.kea')
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(img2Stch, stretchImg, stretch_file, 'KEA', rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    elif img_stats_msk is not None:
        img2StchMskd = os.path.join(tmpDIR, baseName + '_MskdImg.kea')
        rsgislib.imageutils.mask_img(selImgBandsImg, img_stats_msk, img2StchMskd, 'KEA',
                                      rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img), img_no_data_val, img_msk_vals)
        stretchImgStats = os.path.join(tmpDIR, baseName + '_stretch_statstmp.txt')
        stretchImgTmp = os.path.join(tmpDIR, baseName + '_stretch_tmp.kea')
        rsgislib.imageutils.stretch_img(img2StchMskd, stretchImgTmp, True, stretchImgStats, img_no_data_val,
                                               False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

        rsgislib.imageutils.stretch_img_with_stats(img2Stch, stretchImg, stretchImgStats, 'KEA',
                                                        rsgislib.TYPE_8UINT, img_no_data_val,
                                                        rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(img2Stch, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    webview_opt = 'none'
    if webview:
        webview_opt = 'leaflet'

    cmd = 'gdal2tiles.py -r average -z {0} -a 0 -w {1} {2} {3}'.format(zoomLevels, webview_opt, stretchImg, output_dir)
    print(cmd)
    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    if not tms:
        from rsgislib.tools import convert_between_tms_xyz
        convert_between_tms_xyz(output_dir)

    if scale > 0:
        cmd = 'gdal_translate -of GTIFF -co TILED=YES -co COMPRESS=JPEG -co BIGTIFF=NO -ot Byte -outsize {0}% 0 -r average {1} {2}'.format(scale, stretchImg, scaled_gtiff_img)
    else:
        cmd = 'gdal_translate -of GTIFF -co TILED=YES -co COMPRESS=JPEG -co BIGTIFF=NO -ot Byte -r average {0} {1}'.format(stretchImg, scaled_gtiff_img)

    try:
        subprocess.check_call(cmd, shell=True)
    except OSError as e:
        raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    rsgislib.imageutils.pop_img_stats(scaled_gtiff_img, use_no_data=True, no_data_val=0, calc_pyramids=True)
    shutil.rmtree(tmpDIR)


def create_quicklook_overview_imgs(input_imgs, bands, tmp_dir, outputImgs='quicklook.jpg', output_img_sizes=250,
                                   scale_axis='auto', stretch_file=None):
    """
    A function to produce an overview quicklook for a number of input images.

    :param input_imgs: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param tmp_dir: an input directory which can be used to write tempory files/directories.
    :param outputImgs: a single output image or list of output images. The same size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output image sizes.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width, height or auto.
                       Auto applies the output_img_sizes to the longest of the two axes.
    :param stretch_file: a stretch stats file to standardise the stretch between a number of input files.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    if scale_axis not in ['width', 'height', 'auto']:
        raise rsgislib.RSGISPyException("Input parameter 'scale_axis' must have the value 'width', 'height' or 'auto'.")

    if type(input_imgs) is not list:
        raise rsgislib.RSGISPyException("Input images must be a list")
    n_in_imgs = len(input_imgs)

    n_out_imgs = 1
    if type(outputImgs) is list:
        n_out_imgs = len(outputImgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException("If the outputImgs input is a list so must output_img_sizes.")
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException("outputImgs and output_img_sizes must be the same length")

        if n_out_imgs == 1:
            outputImgs = outputImgs[0]
            output_img_sizes = output_img_sizes[0]

    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "qklk_tmp_{}".format(uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    img_basename = rsgislib.tools.filetools.get_file_basename(input_imgs[0], check_valid=True)

    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')

    sBands = []
    for strBand in bandLst:
        sBands.append(int(strBand))

    nImgBands = rsgislib.imageutils.get_img_band_count(input_imgs[0])
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_imgs[0])

    b_sel_imgs = []
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        tmp_no_data_val = rsgislib.imageutils.get_img_no_data_value(img)

        if tmp_n_bands != nImgBands:
            raise rsgislib.RSGISPyException('The number of bands in the input images is different.')

        if tmp_no_data_val != img_no_data_val:
            raise rsgislib.RSGISPyException('The no data value is different between the input images.')

        selImgBandsImg = ''
        if (nImgBands == 1) and (not multiBand):
            selImgBandsImg = img
        elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
            selImgBandsImg = img
        else:
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(img, check_valid=True)
            selImgBandsImg = os.path.join(usr_tmp_dir, '{}_sband.kea'.format(lcl_img_basename))
            rsgislib.imageutils.select_img_bands(img, selImgBandsImg, 'KEA',
                                                 rsgislib.imageutils.get_rsgislib_datatype_from_img(img), sBands)
            rsgislib.imageutils.set_img_no_data_value(selImgBandsImg, img_no_data_val)
        b_sel_imgs.append(selImgBandsImg)

    tmp_vrt_img = os.path.join(usr_tmp_dir, "{}_{}.vrt".format(img_basename, uid_str))
    rsgislib.imageutils.gdal_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img)

    stretchImg = os.path.join(usr_tmp_dir, '{}_stretch.kea'.format(img_basename))
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(tmp_vrt_img, stretchImg, stretch_file, 'KEA',
                                                        rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(tmp_vrt_img, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    if scale_axis == 'auto':
        x_size, y_size = rsgislib.imageutils.getImageSize(stretchImg)
        if x_size > y_size:
            scale_axis = 'width'
        else:
            scale_axis = 'height'

    if n_out_imgs == 1:
        if scale_axis == 'width':
            out_size = '-outsize {0} 0'.format(output_img_sizes)
        else:
            out_size = '-outsize 0 {0}'.format(output_img_sizes)

        cmd = 'gdal_translate -of JPEG -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg, outputImgs)
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            if scale_axis == 'width':
                out_size = '-outsize {0} 0'.format(output_img_sizes[i])
            else:
                out_size = '-outsize 0 {0}'.format(output_img_sizes[i])

            cmd = 'gdal_translate -of JPEG -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg,
                                                                                          outputImgs[i])
            print(cmd)
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(usr_tmp_dir)


def burn_in_binary_msk(base_image, mask_img, output_img, gdalformat, msk_colour=None):
    """
A function which is used for visualisation applications where a rasterised binary mask is
'burnt' into a base image with a user selected colour.

Where:

:param base_image: the base image with continous output values. It is expected that this image
                   has 8uint pixel values.
:param mask_img: the binary mask (value 1 will be used) to be 'burnt' into the base_image.
:param output_img: the output image file name and path
:param gdalformat: the GDAL image file format of the output image file.
:param msk_colour: the colour of the pixels burnt in to the base image. Should have the same number
                   of dimensions as the input base image has image bands. Default, if None is 255.

"""
    from rios import applier
    import numpy

    n_img_bands = rsgislib.imageutils.get_img_band_count(base_image)
    if msk_colour is None:
        msk_colour = list()
        for i in range(n_img_bands):
            msk_colour.append(255)
    elif len(msk_colour) != n_img_bands:
        raise rsgislib.RSGISPyException("The number of image bands and length of the msk_colour array should be equal.")

    try:
        import tqdm
        progress_bar = rsgislib.TQDMProgressBar()
    except:
        from rios import cuiprogress
        progress_bar = cuiprogress.GDALProgressBar()

    infiles = applier.FilenameAssociations()
    infiles.base_image = base_image
    infiles.mask_img = mask_img
    outfiles = applier.FilenameAssociations()
    outfiles.output_img = output_img
    otherargs = applier.OtherInputs()
    otherargs.msk_colour = msk_colour
    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    def _burnInValues(info, inputs, outputs, otherargs):
        """
        This is an internal rios function
        """
        outputs.output_img = numpy.copy(inputs.base_image)
        for n in range(inputs.base_image.shape[0]):
            outputs.output_img[n] = numpy.where(inputs.mask_img == 1, otherargs.msk_colour[n], outputs.output_img[n])

    applier.apply(_burnInValues, infiles, outfiles, otherargs, controls=aControls)


def create_quicklook_overview_imgs_vec_overlay(input_imgs, bands, tmp_dir, vec_overlay_file, vec_overlay_lyr,
                                               outputImgs='quicklook.jpg', output_img_sizes=250, gdalformat='JPEG',
                                               scale_axis='auto', stretch_file=None, overlay_clr=None):
    """
    A function to produce an overview quicklook with a vector overlain for a number of input images.

    :param input_imgs: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param tmp_dir: an input directory which can be used to write tempory files/directories.
    :param vec_overlay_file: an vector overlay which will be rasterised to the overlay on the output images.
    :param vec_overlay_lyr: the layer name for the vector overlay.
    :param outputImgs: a single output image or list of output images. The same size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output image sizes.
    :param gdalformat: the output file format - probably either JPG, PNG or GTIFF.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width, height or auto.
                       Auto applies the output_img_sizes to the longest of the two axes.
    :param stretch_file: a stretch stats file to standardise the stretch between a number of input files.
    :param overlay_clr: output colour for the overlay image (value between 0-255). Default all values are 255.

    """
    import rsgislib.tools.utils
    import rsgislib.tools.filetools
    import rsgislib.vectorutils
    if scale_axis not in ['width', 'height', 'auto']:
        raise rsgislib.RSGISPyException("Input parameter 'scale_axis' must have the value 'width', 'height' or 'auto'.")

    if type(input_imgs) is not list:
        raise rsgislib.RSGISPyException("Input images must be a list")
    n_in_imgs = len(input_imgs)

    n_out_imgs = 1
    if type(outputImgs) is list:
        n_out_imgs = len(outputImgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException("If the outputImgs input is a list so must output_img_sizes.")
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException("outputImgs and output_img_sizes must be the same length")

        if n_out_imgs == 1:
            outputImgs = outputImgs[0]
            output_img_sizes = output_img_sizes[0]

    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "qklk_tmp_{}".format(uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    img_basename = rsgislib.tools.filetools.get_file_basename(input_imgs[0], checkvalid=True)
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')

    sBands = []
    for strBand in bandLst:
        sBands.append(int(strBand))

    nImgBands = rsgislib.imageutils.get_img_band_count(input_imgs[0])
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_imgs[0])

    b_sel_imgs = []
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        tmp_no_data_val = rsgislib.imageutils.get_img_no_data_value(img)

        if tmp_n_bands != nImgBands:
            raise rsgislib.RSGISPyException('The number of bands in the input images is different.')

        if tmp_no_data_val != img_no_data_val:
            raise rsgislib.RSGISPyException('The no data value is different between the input images.')

        selImgBandsImg = ''
        if (nImgBands == 1) and (not multiBand):
            selImgBandsImg = img
        elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
            selImgBandsImg = img
        else:
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(img, checkvalid=True)
            selImgBandsImg = os.path.join(usr_tmp_dir, '{}_sband.kea'.format(lcl_img_basename))
            rsgislib.imageutils.select_img_bands(img, selImgBandsImg, 'KEA',
                                                 rsgislib.imageutils.get_rsgislib_datatype_from_img(img), sBands)
            rsgislib.imageutils.set_img_no_data_value(selImgBandsImg, img_no_data_val)
        b_sel_imgs.append(selImgBandsImg)

    if overlay_clr is None:
        overlay_clr = list()
        for i in range(nImgBands):
            overlay_clr.append(255)

    tmp_vrt_img = os.path.join(usr_tmp_dir, "{}_{}.vrt".format(img_basename, uid_str))
    rsgislib.imageutils.gdal_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img)

    stretchImg = os.path.join(usr_tmp_dir, '{}_stretch.kea'.format(img_basename))
    if stretch_file is not None:
        rsgislib.imageutils.stretch_img_with_stats(tmp_vrt_img, stretchImg, stretch_file, 'KEA',
                                                        rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(tmp_vrt_img, stretchImg, False, '', img_no_data_val, False, 'KEA',
                                               rsgislib.TYPE_8UINT, rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)

    if scale_axis == 'auto':
        x_size, y_size = rsgislib.imageutils.getImageSize(stretchImg)
        if x_size > y_size:
            scale_axis = 'width'
        else:
            scale_axis = 'height'

    if n_out_imgs == 1:
        if scale_axis == 'width':
            out_size = '-outsize {0} 0'.format(output_img_sizes)
        else:
            out_size = '-outsize 0 {0}'.format(output_img_sizes)
        # Create the resized output image.
        lcl_img_basename = rsgislib.tools.filetools.get_file_basename(outputImgs, check_valid=True)
        tmp_resized_img = os.path.join(usr_tmp_dir, '{}_resized.kea'.format(lcl_img_basename))
        cmd = 'gdal_translate -of KEA -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg,
                                                                                     tmp_resized_img)
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
        # Rasterise the overlay vector to the output raster grid.
        tmp_vec_overlay_img = os.path.join(usr_tmp_dir, '{}_vec_overlay.kea'.format(lcl_img_basename))
        rsgislib.vectorutils.convertvector.rasterise_vec_lyr(vec_overlay_file, vec_overlay_lyr, tmp_resized_img, tmp_vec_overlay_img,
                                                             gdalformat="KEA", burn_val=1, datatype=rsgislib.TYPE_8UINT, att_column=None,
                                                             thematic=True, no_data_val=0)
        # Merge the overlay and base image
        tmp_final_img = os.path.join(usr_tmp_dir, '{}_final.kea'.format(lcl_img_basename))
        burn_in_binary_msk(tmp_resized_img, tmp_vec_overlay_img, tmp_final_img, 'KEA', overlay_clr)
        # Convert to final format (e.g., JPG, TIFF or PNG)
        rsgislib.imageutils.gdal_translate(tmp_final_img, outputImgs, gdal_format=gdalformat, options='')

    elif n_out_imgs > 1:
        for i in range(n_out_imgs):
            if scale_axis == 'width':
                out_size = '-outsize {0} 0'.format(output_img_sizes[i])
            else:
                out_size = '-outsize 0 {0}'.format(output_img_sizes[i])

            # Create the resized output image.
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(outputImgs[i], check_valid=True)
            tmp_resized_img = os.path.join(usr_tmp_dir, '{}_resized.kea'.format(lcl_img_basename))
            cmd = 'gdal_translate -of KEA -ot Byte -scale {0} -r average {1} {2}'.format(out_size, stretchImg,
                                                                                         tmp_resized_img)
            print(cmd)
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
            # Rasterise the overlay vector to the output raster grid.
            tmp_vec_overlay_img = os.path.join(usr_tmp_dir, '{}_vec_overlay.kea'.format(lcl_img_basename))
            rsgislib.vectorutils.convertvector.rasterise_vec_lyr(vec_overlay_file, vec_overlay_lyr, tmp_resized_img,
                                                                 tmp_vec_overlay_img, gdalformat="KEA", burn_val=1,
                                                                 datatype=rsgislib.TYPE_8UINT, att_column=None, thematic=True, no_data_val=0)
            # Merge the overlay and base image
            tmp_final_img = os.path.join(usr_tmp_dir, '{}_final.kea'.format(lcl_img_basename))
            burn_in_binary_msk(tmp_resized_img, tmp_vec_overlay_img, tmp_final_img, 'KEA', overlay_clr)
            # Convert to final format (e.g., JPG, TIFF or PNG)
            rsgislib.imageutils.gdal_translate(tmp_final_img, outputImgs[i], gdal_format=gdalformat, options='')
    shutil.rmtree(usr_tmp_dir)


def create_visual_overview_imgs_vec_extent(input_imgs, bands, tmp_dir, vec_extent_file, vec_extent_lyr,
                                           outputImgs='quicklook.tif', output_img_sizes=500, gdalformat='GTIFF',
                                           scale_axis='auto', stretch_file=None, export_stretch_file=False):
    """
    A function to produce an 8bit overview image (i.e., stretched visualisation) with an optional specified
    extent.

    :param input_imgs: input image file (any format that gdal supports)
    :param bands: a string (comma seperated) with the bands to be selected. (e.g., '1', '1,2,3', '5,6,4')
    :param tmp_dir: an input directory which can be used to write tempory files/directories.
    :param vec_extent_file: an vector file to define the extent of the output image files.
    :param vec_extent_lyr: the layer name for the vector extent.
    :param outputImgs: a single output image or list of output images. The same size as output_img_sizes.
    :param output_img_sizes: the output image size (in pixels) or list of output image sizes.
    :param gdalformat: the output file format - probably either JPG, PNG or GTIFF.
    :param scale_axis: the axis to which the output_img_sizes refer. Options: width, height or auto.
                       Auto applies the output_img_sizes to the longest of the two axes.
    :param stretch_file: a stretch stats file to standardise the stretch between a number of input files. If
                         export_stretch_file is True then this variable is used as the output stretch file path.
    :param export_stretch_file: If true then the stretch parameters are outputted as a text file to the path defined
                                by stretch_file.

    """
    import rsgislib.vectorutils
    import rsgislib.tools.filetools
    import rsgislib.tools.utils
    import rsgislib.imageutils
    if scale_axis not in ['width', 'height', 'auto']:
        raise rsgislib.RSGISPyException("Input parameter 'scale_axis' must have the value 'width', 'height' or 'auto'.")

    if export_stretch_file and (stretch_file is None):
        raise rsgislib.RSGISPyException("If export_stretch_file is True then a stretch_file path must be provided.")

    if type(input_imgs) is not list:
        raise rsgislib.RSGISPyException("Input images must be a list")
    n_in_imgs = len(input_imgs)

    n_out_imgs = 1
    if type(outputImgs) is list:
        n_out_imgs = len(outputImgs)
        if type(output_img_sizes) is not list:
            raise rsgislib.RSGISPyException("If the outputImgs input is a list so must output_img_sizes.")
        if len(output_img_sizes) != n_out_imgs:
            raise rsgislib.RSGISPyException("outputImgs and output_img_sizes must be the same length")

        if n_out_imgs == 1:
            outputImgs = outputImgs[0]
            output_img_sizes = output_img_sizes[0]

    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "qklk_tmp_{}".format(uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    img_basename = rsgislib.tools.filetools.get_file_basename(input_imgs[0], check_valid=True)
    bandLst = bands.split(',')
    multiBand = False
    if len(bandLst) == 3:
        multiBand = True
    elif len(bandLst) == 1:
        multiBand = False
    else:
        print(bandLst)
        raise rsgislib.RSGISPyException('You need to either provide 1 or 3 bands.')

    sBands = []
    for strBand in bandLst:
        sBands.append(int(strBand))

    nImgBands = rsgislib.imageutils.get_img_band_count(input_imgs[0])
    img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_imgs[0])

    b_sel_imgs = []
    for img in input_imgs:
        tmp_n_bands = rsgislib.imageutils.get_img_band_count(img)
        tmp_no_data_val = rsgislib.imageutils.get_img_no_data_value(img)

        if tmp_n_bands != nImgBands:
            raise rsgislib.RSGISPyException('The number of bands in the input images is different.')

        if tmp_no_data_val != img_no_data_val:
            raise rsgislib.RSGISPyException('The no data value is different between the input images.')

        selImgBandsImg = ''
        if (nImgBands == 1) and (not multiBand):
            selImgBandsImg = img
        elif (nImgBands == 3) and (multiBand) and (bandLst[0] == '1') and (bandLst[1] == '2') and (bandLst[2] == '3'):
            selImgBandsImg = img
        else:
            lcl_img_basename = rsgislib.tools.filetools.get_file_basename(img, checkvalid=True)
            selImgBandsImg = os.path.join(usr_tmp_dir, '{}_sband.kea'.format(lcl_img_basename))
            rsgislib.imageutils.select_img_bands(img, selImgBandsImg, 'KEA',
                                                 rsgislib.imageutils.get_rsgislib_datatype_from_img(img), sBands)
            rsgislib.imageutils.set_img_no_data_value(selImgBandsImg, img_no_data_val)
        b_sel_imgs.append(selImgBandsImg)

    tmp_vrt_img = os.path.join(usr_tmp_dir, "{}_{}.vrt".format(img_basename, uid_str))
    if vec_extent_file is None:
        rsgislib.imageutils.gdal_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img)
    else:
        vec_extent_bbox = rsgislib.vectorutils.getVecLayerExtent(vec_extent_file, vec_extent_lyr, computeIfExp=True)
        vrt_extent = [vec_extent_bbox[0], vec_extent_bbox[2], vec_extent_bbox[1], vec_extent_bbox[3]]
        rsgislib.imageutils.gdal_mosaic_images_vrt(b_sel_imgs, tmp_vrt_img, vrt_extent)

    stretchImg = os.path.join(usr_tmp_dir, '{}_stretch.kea'.format(img_basename))
    if (stretch_file is not None) and (export_stretch_file == False):
        rsgislib.imageutils.stretch_img_with_stats(tmp_vrt_img, stretchImg, stretch_file, 'KEA',
                                                        rsgislib.TYPE_8UINT,
                                                        img_no_data_val, rsgislib.imageutils.STRETCH_LINEARMINMAX, 2)
    else:
        rsgislib.imageutils.stretch_img(tmp_vrt_img, stretchImg, export_stretch_file, stretch_file,
                                               img_no_data_val, False, 'KEA', rsgislib.TYPE_8UINT,
                                               rsgislib.imageutils.STRETCH_LINEARSTDDEV, 2)
    if scale_axis == 'auto':
        x_size, y_size = rsgislib.imageutils.getImageSize(stretchImg)
        if x_size > y_size:
            scale_axis = 'width'
        else:
            scale_axis = 'height'

    if n_out_imgs == 1:
        if scale_axis == 'width':
            out_size = '-outsize {0} 0'.format(output_img_sizes)
        else:
            out_size = '-outsize 0 {0}'.format(output_img_sizes)
        # Create the resized output image.
        cmd = 'gdal_translate -of {0} -ot Byte -scale {1} -r average {2} {3}'.format(gdalformat, out_size,
                                                                                     stretchImg, outputImgs)
        print(cmd)
        try:
            subprocess.check_call(cmd, shell=True)
        except OSError as e:
            raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    else:
        for i in range(n_out_imgs):
            if scale_axis == 'width':
                out_size = '-outsize {0} 0'.format(output_img_sizes[i])
            else:
                out_size = '-outsize 0 {0}'.format(output_img_sizes[i])

            # Create the resized output image.
            cmd = 'gdal_translate -of {0} -ot Byte -scale {1} -r average {2} {3}'.format(gdalformat, out_size,
                                                                                         stretchImg, outputImgs[i])
            print(cmd)
            try:
                subprocess.check_call(cmd, shell=True)
            except OSError as e:
                raise rsgislib.RSGISPyException('Could not execute command: ' + cmd)
    shutil.rmtree(usr_tmp_dir)


def overlay_vec_on_img(input_img, output_img, vec_overlay_file, vec_overlay_lyr, tmp_dir, gdalformat='PNG',
                       overlay_clr=None):
    """
    A function to overlay a vector layer on to a raster image 'burning' in the vector as a particular
    colour. This is most appropriate for polyline vectors, polygons will be filled.

    :param input_img: The input image, usually a stretched 8uint image ready for visualisation but needs
                      spatial header information associated.
    :param output_img: A output image file commonly in none specialist format such as JPEG, PNG or TIFF.
    :param vec_overlay_file: The vector file to overlay (recommended to be a polyline vector)
    :param vec_overlay_lyr: The name of the vector layer
    :param tmp_dir: A temporary were processing stage file can be written during processing.
    :param gdalformat: The output format - commonly PNG, JPEG or GTIFF.
    :param overlay_clr: An array with the output pixel values for the vector overlay, needs
                        the same length as the number of image bands. If None then the overlay
                        will be white (i.e., [255, 255, 255].

    """
    import rsgislib.vectorutils
    import rsgislib.tools.utils
    import rsgislib.tools.filetools

    # Check the overlay colour is defined and correct.
    n_img_bands = rsgislib.imageutils.get_img_band_count(input_img)
    if overlay_clr is None:
        overlay_clr = list()
        for i in range(n_img_bands):
            overlay_clr.append(255)
    elif len(overlay_clr) != n_img_bands:
        raise Exception("The overlay colour does not have the same number of "
                        "values as the number of bands within the input image.")

    # Create a tempory directory for processing stage outputs.
    img_basename = rsgislib.tools.filetools.get_file_basename(input_img)
    uid_str = rsgislib.tools.utils.uid_generator()
    usr_tmp_dir = os.path.join(tmp_dir, "{}_qklk_overlay_tmp_{}".format(img_basename, uid_str))
    if not os.path.exists(usr_tmp_dir):
        os.makedirs(usr_tmp_dir)

    # Create raster of the vector to be overlain.
    tmp_vec_overlay_img = os.path.join(usr_tmp_dir, '{}_vec_overlay.kea'.format(img_basename))
    rsgislib.vectorutils.convertvector.rasterise_vec_lyr(vec_overlay_file, vec_overlay_lyr, input_img, tmp_vec_overlay_img,
                                                         gdalformat="KEA", burn_val=1, datatype=rsgislib.TYPE_8UINT, att_column=None,
                                                         thematic=True, no_data_val=0)

    # Merge the overlay and base image
    tmp_final_img = os.path.join(usr_tmp_dir, '{}_final.kea'.format(img_basename))
    burn_in_binary_msk(input_img, tmp_vec_overlay_img, tmp_final_img, 'KEA', overlay_clr)

    # Convert to final format (e.g., JPG, TIFF or PNG)
    rsgislib.imageutils.gdal_translate(tmp_final_img, output_img, gdal_format=gdalformat)

    # Remove the temporary directory.
    shutil.rmtree(usr_tmp_dir)

