#!/usr/bin/env python
"""
The tools.validate_datesets module contains functions for checking that datasets are valid.
"""

import os
import osgeo.gdal as gdal
import osgeo.osr as osr
import rsgislib

def checkGDALImageFile(gdal_img, check_bands=True, nbands=0, chk_proj=False, epsg_code=0, read_img=False, n_smp_pxls=10):
    """
    A function which checks a GDAL compatible image file and returns an error message if appropriate.

    :param gdal_img: the file path to the gdal image file.
    :param check_bands: boolean specifying whether individual image bands should be
                        opened and checked (Default: True)
    :param nbands: int specifying the number of expected image bands. Ignored if 0; Default is 0.
    :param chk_proj: boolean specifying whether to check that the projection has been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image is not that projection.
    :param read_img: boolean specifying whether to try reading some image pixel values from the image.
                     This option will read 10 random image pixel values from a randomly selected band.
    :param n_smp_pxls: The number of sample pixels to be read from the image if the read_img option is True.
                       Default is 10.
    :return: boolean (True: file ok; False: Error found), string (error message if required otherwise empty string)

    """
    file_ok = True
    err_str = ''
    if os.path.exists(gdal_img):
        err = rsgislib.RSGISGDALErrorHandler()
        err_handler = err.handler
        gdal.PushErrorHandler(err_handler)
        gdal.UseExceptions()
        try:
            if os.path.splitext(gdal_img)[1].lower() == '.kea':
                file_ok = checkHDF5File(gdal_img)
                if not file_ok:
                    err_str = "Error with KEA/HDF5 file."
            if file_ok:
                raster_ds = gdal.Open(gdal_img, gdal.GA_ReadOnly)
                if raster_ds is None:
                    file_ok = False
                    err_str = 'GDAL could not open the dataset, returned None.'

                if file_ok and (nbands > 0):
                    n_img_bands = raster_ds.RasterCount
                    if n_img_bands != nbands:
                        file_ok = False
                        err_str = 'Image should have {} image bands but {} found.'.format(nbands, n_img_bands)

                if file_ok and check_bands:
                    n_img_bands = raster_ds.RasterCount
                    if n_img_bands < 1:
                        file_ok = False
                        err_str = 'Image says it does not have any image bands.'
                    else:
                        for n in range(n_img_bands):
                            band = n + 1
                            img_band = raster_ds.GetRasterBand(band)
                            if img_band is None:
                                file_ok = False
                                err_str = 'GDAL could not open band {} in the dataset, returned None.'.format(band)
                                break

                if file_ok and chk_proj:
                    proj_obj = raster_ds.GetProjection()
                    if proj_obj is None:
                        file_ok = False
                        err_str = 'Image projection is None.'
                    elif proj_obj is '':
                        file_ok = False
                        err_str = 'Image projection is empty.'

                    if file_ok and (epsg_code > 0):
                        spat_ref = osr.SpatialReference()
                        spat_ref.ImportFromWkt(proj_obj)
                        spat_ref.AutoIdentifyEPSG()
                        img_epsg_code = spat_ref.GetAuthorityCode(None)
                        if img_epsg_code is None:
                            file_ok = False
                            err_str = 'Image projection returned a None EPSG code.'
                        elif int(img_epsg_code) != int(epsg_code):
                            file_ok = False
                            err_str = 'Image EPSG ({}) does not match that specified ({})'.format(img_epsg_code, epsg_code)

                if file_ok and read_img:
                    import numpy
                    n_img_bands = raster_ds.RasterCount
                    xSize = raster_ds.RasterXSize
                    ySize = raster_ds.RasterYSize

                    if n_img_bands == 1:
                        band = 1
                    else:
                        band = int(numpy.random.randint(1, high=n_img_bands, size=1))

                    img_band = raster_ds.GetRasterBand(band)
                    x_pxls = numpy.random.choice(xSize, n_smp_pxls)
                    y_pxls = numpy.random.choice(ySize, n_smp_pxls)
                    for i in range(n_smp_pxls):
                        img_data = img_band.ReadRaster(xoff=int(x_pxls[i]), yoff=int(y_pxls[i]), xsize=1, ysize=1,
                                                       buf_xsize=1, buf_ysize=1, buf_type=gdal.GDT_Float32)

                raster_ds = None
        except Exception as e:
            file_ok = False
            err_str = str(e)
        else:
            if err.err_level >= gdal.CE_Warning:
                file_ok = False
                err_str = str(err.err_msg)
        finally:
            gdal.PopErrorHandler()
    else:
        file_ok = False
        err_str = 'File does not exist.'
    return file_ok, err_str


def checkGDALVectorFile(gdal_vec):
    """
    A function which checks a GDAL compatible vector file and returns an error message if appropriate.

    :param gdal_vec: the file path to the gdal vector file.
    :return: boolean (True: file OK; False: Error found), string (error message if required otherwise empty string)

    """
    file_ok = True
    err_str = ''
    if os.path.exists(gdal_vec):
        err = rsgislib.RSGISGDALErrorHandler()
        err_handler = err.handler
        gdal.PushErrorHandler(err_handler)
        gdal.UseExceptions()

        try:
            vec_ds = gdal.OpenEx(gdal_vec, gdal.OF_VECTOR)
            if vec_ds is None:
                file_ok = False
                err_str = 'GDAL could not open the data source, returned None.'
            else:
                for lyr_idx in range(vec_ds.GetLayerCount()):
                    vec_lyr = vec_ds.GetLayerByIndex(lyr_idx)
                    if vec_lyr is None:
                        file_ok = False
                        err_str = 'GDAL could not open all the vector layers.'
                        break
            vec_ds = None
        except Exception as e:
            file_ok = False
            err_str = str(e)
        else:
            if err.err_level >= gdal.CE_Warning:
                file_ok = False
                err_str = str(err.err_msg)
        finally:
            gdal.PopErrorHandler()
    else:
        file_ok = False
        err_str = 'File does not exist.'
    return file_ok, err_str


def checkHDF5File(input_file):
    """
    A function which checks whether a HDF5 file is valid.
    :param input_file: the file path to the input file.
    :return: a boolean - True file is valid. False file is not valid.

    """
    import h5py

    def _check_h5_var(h5_obj):
        lcl_ok = True
        try:
            if isinstance(h5_obj, h5py.Dataset):
                lcl_ok = True
            elif isinstance(h5_obj, h5py.Group):
                for var in h5_obj.keys():
                    lcl_ok = _check_h5_var(h5_obj[var])
                    if not lcl_ok:
                        break
        except RuntimeError:
            lcl_ok = False
        return lcl_ok

    glb_ok = True
    if not os.path.exists(input_file):
        glb_ok = False
    else:
        fH5 = h5py.File(input_file, 'r')
        if fH5 is None:
            glb_ok = False
        else:
            for var in fH5.keys():
                glb_ok = _check_h5_var(fH5[var])
                if not glb_ok:
                    break
    return glb_ok





