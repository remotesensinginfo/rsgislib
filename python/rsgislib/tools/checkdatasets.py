#!/usr/bin/env python
"""
The tools.validate_datesets module contains functions for checking that datasets are valid.
"""

import os
import glob
from typing import List, Dict

import numpy
from osgeo import gdal, osr

import rsgislib
import rsgislib.tools.filetools
import rsgislib.imageutils


def check_gdal_image_file(
    input_img: str,
    check_bands: bool = True,
    n_bands: int = 0,
    chk_proj: bool = False,
    epsg_code: int = 0,
    read_img: bool = False,
    smpl_n_pxls: int = 10,
    calc_chk_sum: bool = False,
    max_file_size: int = None,
):
    """
    A function which checks a GDAL compatible image file and returns an error
    message if appropriate.

    :param input_img: the file path to the gdal image file.
    :param check_bands: boolean specifying whether individual image bands should be
                        opened and checked (Default: True)
    :param n_bands: int specifying the number of expected image bands. Ignored if 0;
                    Default is 0.
    :param chk_proj: boolean specifying whether to check that the projection has
                     been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image
                      is not that projection.
    :param read_img: boolean specifying whether to try reading some image pixel values
                     from the image. This option will read npxls (e.g., 10) random
                     image pixel values from a randomly selected band.
    :param smpl_n_pxls: The number of pixel values to be randomly selected (default =
                        10). More values = longer runtime.
    :param calc_chk_sum: boolean specifying whether a checksum should be calculated
                         for each band to check validity
    :param max_file_size: int specifying the maximum file size for the input file.
                          If None then ignored.
    :return: boolean (True: file ok; False: Error found), string (error message if
                      required otherwise empty string)

    """
    file_ok = True
    err_str = ""
    if os.path.exists(input_img):
        if max_file_size is not None:
            file_size_bytes = rsgislib.tools.filetools.get_file_size(input_img)
            if file_size_bytes == 0:
                file_ok = False
                err_str = "File size is zero."
            elif file_size_bytes > max_file_size:
                file_ok = False
                err_str = "File size is greater than max. threshold ({}).".format(
                    file_size_bytes
                )
        if file_ok:
            err = rsgislib.RSGISGDALErrorHandler()
            err_handler = err.handler
            gdal.PushErrorHandler(err_handler)
            gdal.UseExceptions()
            try:
                if os.path.splitext(input_img)[1].lower() == ".kea":
                    file_ok = check_hdf5_file(input_img)
                    if not file_ok:
                        err_str = "Error with KEA/HDF5 file."
                if file_ok:
                    raster_ds = gdal.Open(input_img, gdal.GA_ReadOnly)
                    if raster_ds is None:
                        file_ok = False
                        err_str = "GDAL could not open the dataset, returned None."

                    if file_ok and (n_bands > 0):
                        n_img_bands = raster_ds.RasterCount
                        if n_img_bands != n_bands:
                            file_ok = False
                            err_str = (
                                "Image should have {} image bands "
                                "but {} found.".format(n_bands, n_img_bands)
                            )

                    if file_ok and check_bands:
                        n_img_bands = raster_ds.RasterCount
                        if n_img_bands < 1:
                            file_ok = False
                            err_str = "Image says it does not have any image bands."
                        else:
                            for n in range(n_img_bands):
                                band = n + 1
                                img_band = raster_ds.GetRasterBand(band)
                                if img_band is None:
                                    file_ok = False
                                    err_str = (
                                        "GDAL could not open band {} in "
                                        "the dataset, returned None.".format(band)
                                    )
                                    break

                    if file_ok and chk_proj:
                        proj_obj = raster_ds.GetProjection()
                        if proj_obj is None:
                            file_ok = False
                            err_str = "Image projection is None."
                        elif proj_obj == "":
                            file_ok = False
                            err_str = "Image projection is empty."

                        if file_ok and (epsg_code > 0):
                            spat_ref = osr.SpatialReference()
                            spat_ref.ImportFromWkt(proj_obj)
                            spat_ref.AutoIdentifyEPSG()
                            img_epsg_code = spat_ref.GetAuthorityCode(None)
                            if img_epsg_code is None:
                                file_ok = False
                                err_str = "Image projection returned a None EPSG code."
                            elif int(img_epsg_code) != int(epsg_code):
                                file_ok = False
                                err_str = (
                                    "Image EPSG ({}) does not match "
                                    "that specified ({})".format(
                                        img_epsg_code, epsg_code
                                    )
                                )

                    if file_ok and read_img:
                        n_img_bands = raster_ds.RasterCount
                        xSize = raster_ds.RasterXSize
                        ySize = raster_ds.RasterYSize

                        if n_img_bands == 1:
                            band = 1
                        else:
                            band = int(
                                numpy.random.randint(1, high=n_img_bands, size=1)
                            )

                        img_band = raster_ds.GetRasterBand(band)
                        x_pxls = numpy.random.choice(xSize, smpl_n_pxls)
                        y_pxls = numpy.random.choice(ySize, smpl_n_pxls)
                        for i in range(smpl_n_pxls):
                            img_data = img_band.ReadRaster(
                                xoff=int(x_pxls[i]),
                                yoff=int(y_pxls[i]),
                                xsize=1,
                                ysize=1,
                                buf_xsize=1,
                                buf_ysize=1,
                                buf_type=gdal.GDT_Float32,
                            )

                    if file_ok and calc_chk_sum:
                        n_img_bands = raster_ds.RasterCount
                        for n in range(n_img_bands):
                            raster_ds.GetRasterBand(n + 1).Checksum()

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
        err_str = "File does not exist."
    return file_ok, err_str


def _run_check_gdal_image_file(img_params: list):
    """
    Internal function called by run_check_gdal_image_file - not intended to be called
    by end user.

    :param img_params: list of function inputs.
    :return: boolean whether the specified file is OK or not.

    """
    input_img = img_params[0]
    check_bands = img_params[1]
    n_bands = img_params[2]
    chk_proj = img_params[3]
    epsg_code = img_params[4]
    read_img = img_params[5]
    smpl_n_pxls = img_params[6]
    calc_chk_sum = img_params[7]
    max_file_size = img_params[8]
    rm_err = img_params[9]
    print_err = img_params[10]

    try:
        file_ok, err_str = check_gdal_image_file(
            input_img,
            check_bands=check_bands,
            n_bands=n_bands,
            chk_proj=chk_proj,
            epsg_code=epsg_code,
            read_img=read_img,
            smpl_n_pxls=smpl_n_pxls,
            calc_chk_sum=calc_chk_sum,
            max_file_size=max_file_size,
        )
        if print_err and (not file_ok):
            print("Error: '{}'".format(err_str))
        if not file_ok:
            if rm_err:
                rsgislib.tools.filetools.delete_file_silent(input_img)
                print("Removed {}".format(input_img))
            else:
                print("rm {}".format(input_img))
    except Exception as e:
        if print_err:
            print("Error: '{}'".format(e))
        if rm_err:
            rsgislib.tools.filetools.delete_file_silent(input_img)
            print("Removed {}".format(input_img))
        else:
            print("rm {}".format(input_img))
        file_ok = False

    return file_ok


def run_check_gdal_image_file(
    input_img: str,
    check_bands: bool = True,
    n_bands: int = 0,
    chk_proj: bool = False,
    epsg_code: int = 0,
    read_img: bool = False,
    smpl_n_pxls: int = 10,
    calc_chk_sum: bool = False,
    max_file_size: int = None,
    rm_err: bool = False,
    print_err: bool = True,
    timeout: int = 4,
):
    """
    A function which checks a GDAL compatible image file using the check_gdal_image_file
    function where a mutliprocessing object is used to catch errors which can crash
    Python and still continue without crashing the Python environment.

    You probably want to call this function rather than calling check_gdal_image_file
    directly.

    :param input_img: the file path to the gdal image file.
    :param check_bands: boolean specifying whether individual image bands should be
                        opened and checked (Default: True)
    :param n_bands: int specifying the number of expected image bands. Ignored if 0;
                    Default is 0.
    :param chk_proj: boolean specifying whether to check that the projection has
                     been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image
                      is not that projection.
    :param read_img: boolean specifying whether to try reading some image pixel values
                     from the image. This option will read npxls (e.g., 10) random
                     image pixel values from a randomly selected band.
    :param smpl_n_pxls: The number of pixel values to be randomly selected (default =
                        10). More values = longer runtime.
    :param calc_chk_sum: boolean specifying whether a checksum should be calculated
                         for each band to check validity
    :param max_file_size: int specifying the maximum file size for the input file.
                          If None then ignored.
    :param rm_err: boolean specifying whether to delete the file if an error is found
    :param print_err: print any errors associated with the file to the console
    :param timeout: a timeout in seconds (Default = 4) for the tests to be undertaken.
    :return: boolean whether the file is OK (i.e., passed tests) or not.

    """
    from multiprocessing import Pool

    processes_pool = Pool(1)
    params = [
        input_img,
        check_bands,
        n_bands,
        chk_proj,
        epsg_code,
        read_img,
        smpl_n_pxls,
        calc_chk_sum,
        max_file_size,
        rm_err,
        print_err,
    ]
    try:
        result = processes_pool.apply_async(_run_check_gdal_image_file, args=[params])
        file_ok = result.get(timeout=timeout)
    except Exception as e:
        file_ok = False
        if rm_err:
            rsgislib.tools.filetools.delete_file_silent(input_img)
            print("Removed {}".format(input_img))
        else:
            print("rm {}".format(input_img))
    processes_pool.close()
    # processes_pool.join()

    return file_ok


def run_check_gdal_image_files(
    input_imgs: list,
    check_bands: bool = True,
    n_bands: int = 0,
    chk_proj: bool = False,
    epsg_code: int = 0,
    read_img: bool = False,
    smpl_n_pxls: int = 10,
    calc_chk_sum: bool = False,
    max_file_size: int = None,
    rm_err: bool = False,
    print_err: bool = True,
    print_file_names: bool = False,
    timeout: int = 4,
):
    """
    A function which checks a list of GDAL compatible image files using the
    check_gdal_image_file function where a mutliprocessing object is used to
    catch errors which can crash Python and still continue without crashing the
    Python environment.

    You probably want to call this function rather than calling check_gdal_image_file
    directly.

    :param input_imgs: a list of input images.
    :param check_bands: boolean specifying whether individual image bands should be
                        opened and checked (Default: True)
    :param n_bands: int specifying the number of expected image bands. Ignored if 0;
                    Default is 0.
    :param chk_proj: boolean specifying whether to check that the projection has
                     been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image
                      is not that projection.
    :param read_img: boolean specifying whether to try reading some image pixel values
                     from the image. This option will read npxls (e.g., 10) random
                     image pixel values from a randomly selected band.
    :param smpl_n_pxls: The number of pixel values to be randomly selected (default =
                        10). More values = longer runtime.
    :param calc_chk_sum: boolean specifying whether a checksum should be calculated
                         for each band to check validity
    :param max_file_size: int specifying the maximum file size for the input file.
                          If None then ignored.
    :param rm_err: boolean specifying whether to delete the file if an error is found
    :param print_err: print any errors associated with the file to the console
    :param print_file_names: print the names of the file before they are tested.
    :param timeout: a timeout in seconds (Default = 4) for the tests to be undertaken.
    :return: boolean whether all the files are OK (i.e., passed tests) or not.

    """
    from multiprocessing import Pool

    files_ok = True
    try:
        processes_pool = Pool(1)
        for input_img in input_imgs:
            if print_file_names:
                print(input_img)
            params = [
                input_img,
                check_bands,
                n_bands,
                chk_proj,
                epsg_code,
                read_img,
                smpl_n_pxls,
                calc_chk_sum,
                max_file_size,
                rm_err,
                print_err,
            ]
            try:
                result = processes_pool.apply_async(
                    _run_check_gdal_image_file, args=[params]
                )
                file_ok = result.get(timeout=timeout)
                if not file_ok:
                    files_ok = False
            except Exception as e:
                files_ok = False
                if rm_err:
                    rsgislib.tools.filetools.delete_file_silent(input_img)
                    print("Removed {}".format(input_img))
                else:
                    print("rm {}".format(input_img))
                continue
        processes_pool.close()
        # processes_pool.join()
    except Exception as inst:
        print("Finished with pool")

    return files_ok


def check_gdal_vector_file(
    vec_file: str, chk_proj: bool = True, epsg_code: int = 0, max_file_size: int = None
):
    """
    A function which checks a GDAL compatible vector file and returns an
    error message if appropriate.

    :param vec_file: the file path to the gdal vector file.
    :param chk_proj: boolean specifying whether to check that the projection has
                     been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image
                      is not that projection.
    :param max_file_size: int specifying the maximum file size for the input file.
                          If None then ignored.
    :return: boolean (True: file OK; False: Error found), string (error message
             if required otherwise empty string)

    """
    file_ok = True
    err_str = ""
    if os.path.exists(vec_file):
        if max_file_size is not None:
            file_size_bytes = rsgislib.tools.filetools.get_file_size(vec_file)
            if file_size_bytes == 0:
                file_ok = False
                err_str = "File size is zero."
            elif file_size_bytes > max_file_size:
                file_ok = False
                err_str = "File size is greater than max. threshold ({}).".format(
                    file_size_bytes
                )
        if file_ok:
            err = rsgislib.RSGISGDALErrorHandler()
            err_handler = err.handler
            gdal.PushErrorHandler(err_handler)
            gdal.UseExceptions()

            try:
                vec_ds = gdal.OpenEx(vec_file, gdal.OF_VECTOR)
                if vec_ds is None:
                    file_ok = False
                    err_str = "GDAL could not open the data source, returned None."
                else:
                    for lyr_idx in range(vec_ds.GetLayerCount()):
                        vec_lyr = vec_ds.GetLayerByIndex(lyr_idx)
                        if vec_lyr is None:
                            file_ok = False
                            err_str = "GDAL could not open all the vector layers."
                            break
                    if file_ok and chk_proj:
                        vec_lyr = vec_ds.GetLayer()
                        if vec_lyr is None:
                            raise rsgislib.RSGISPyException(
                                "Something has gone wrong "
                                "checking projection - layer not present"
                            )
                        vec_lyr_spt_ref = vec_lyr.GetSpatialRef()
                        if vec_lyr_spt_ref is None:
                            file_ok = False
                            err_str = "Vector projection is None."
                        if file_ok:
                            spt_ref_wkt = vec_lyr_spt_ref.ExportToWkt()
                            if spt_ref_wkt is None:
                                file_ok = False
                                err_str = "Vector projection WKT is None."
                            elif spt_ref_wkt == "":
                                file_ok = False
                                err_str = "Vector projection is empty."

                            if file_ok and (epsg_code > 0):
                                vec_lyr_spt_ref.AutoIdentifyEPSG()
                                vec_epsg_code = vec_lyr_spt_ref.GetAuthorityCode(None)
                                if vec_epsg_code is None:
                                    file_ok = False
                                    err_str = (
                                        "Vector projection returned a None EPSG code."
                                    )
                                elif int(vec_epsg_code) != int(epsg_code):
                                    file_ok = False
                                    err_str = (
                                        "Vector EPSG ({}) does not match "
                                        "that specified ({})".format(
                                            vec_epsg_code, epsg_code
                                        )
                                    )

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
        err_str = "File does not exist."
    return file_ok, err_str


def _run_check_gdal_vector_file(file_params: list):
    """
    Internal function called by check_gdal_vector_file - not intended to be called
    by end user.

    :param file_params: list of function inputs.
    :return: boolean whether the specified file is OK or not.

    """
    vec_file = file_params[0]
    chk_proj = file_params[1]
    epsg_code = file_params[2]
    max_file_size = file_params[3]
    rm_err = file_params[4]
    print_err = file_params[5]
    multi_file = file_params[6]

    try:
        file_ok, err_str = check_gdal_vector_file(
            vec_file,
            chk_proj=chk_proj,
            epsg_code=epsg_code,
            max_file_size=max_file_size,
        )
        if print_err and (not file_ok):
            print("Error: '{}'".format(err_str))
        if not file_ok:
            if rm_err:
                if multi_file:
                    rsgislib.tools.filetools.delete_file_with_basename(vec_file)
                else:
                    rsgislib.tools.filetools.delete_file_silent(vec_file)
                    print("Removed {}".format(vec_file))
            else:
                print("rm {}".format(vec_file))
    except Exception as e:
        if print_err:
            print("Error: '{}'".format(e))
        if rm_err:
            if multi_file:
                rsgislib.tools.filetools.delete_file_with_basename(vec_file)
            else:
                rsgislib.tools.filetools.delete_file_silent(vec_file)
                print("Removed {}".format(vec_file))
        else:
            print("rm {}".format(vec_file))
        file_ok = False

    return file_ok


def run_check_gdal_vector_file(
    vec_file: str,
    chk_proj: bool = True,
    epsg_code: int = 0,
    max_file_size: int = None,
    rm_err: bool = False,
    print_err: bool = True,
    multi_file: bool = False,
    timeout: int = 4,
):
    """
    A function which checks a GDAL compatible vector file using the
    check_gdal_vector_file function where a mutliprocessing object is used to
    catch errors which can crash Python and still continue without crashing
    the Python environment.

    You probably want to call this function rather than calling check_gdal_vector_file
    directly.

    :param vec_file: the file path to the gdal vector file.
    :param chk_proj: boolean specifying whether to check that the projection has
                     been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image
                      is not that projection.
    :param max_file_size: int specifying the maximum file size for the input file.
                          If None then ignored.
    :param rm_err: boolean specifying whether to delete the file if an error is found
    :param print_err: print any errors associated with the file to the console
    :param multi_file: if True (Default: False) then remove files with the same
                       basename. Useful for ESRI Shapefiles which are made up of
                       multiple files.
    :param timeout: a timeout in seconds (Default = 4) for the tests to be undertaken.
    :return: boolean specifying whether the file is OK (i.e., tests passed) or not.

    """
    from multiprocessing import Pool

    processes_pool = Pool(1)
    params = [
        vec_file,
        chk_proj,
        epsg_code,
        max_file_size,
        rm_err,
        print_err,
        multi_file,
    ]
    try:
        result = processes_pool.apply_async(_run_check_gdal_vector_file, args=[params])
        file_ok = result.get(timeout=timeout)
    except Exception as e:
        file_ok = False
        if rm_err:
            if multi_file:
                rsgislib.tools.filetools.delete_file_with_basename(vec_file)
            else:
                rsgislib.tools.filetools.delete_file_silent(vec_file)
                print("Removed {}".format(vec_file))
        else:
            print("rm {}".format(vec_file))
    processes_pool.close()
    # processes_pool.join()

    return file_ok


def run_check_gdal_vector_files(
    vec_files: list,
    chk_proj: bool = True,
    epsg_code: int = 0,
    max_file_size: int = None,
    rm_err: bool = False,
    print_err: bool = True,
    multi_file: bool = False,
    print_file_names: bool = False,
    timeout: int = 4,
):
    """
    A function which checks a list of GDAL compatible vector files using the
    check_gdal_vector_file function where a mutliprocessing object is used to
    catch errors which can crash Python and still continue without crashing
    the Python environment.

    You probably want to call this function rather than calling check_gdal_vector_file
    directly.

    :param vec_files: list of input file paths.
    :param chk_proj: boolean specifying whether to check that the projection has
                     been defined.
    :param epsg_code: int for the EPSG code for the projection. Error raised if image
                      is not that projection.
    :param max_file_size: int specifying the maximum file size for the input file.
                          If None then ignored.
    :param rm_err: boolean specifying whether to delete the file if an error is found
    :param print_err: print any errors associated with the file to the console
    :param multi_file: if True (Default: False) then remove files with the same
                       basename. Useful for ESRI Shapefiles which are made up of
                       multiple files.
    :param print_file_names: print the names of the file before they are tested.
    :param timeout: a timeout in seconds (Default = 4) for the tests to be undertaken.
    :return: boolean specifying whether all the files are OK
             (i.e., tests passed) or not.

    """
    from multiprocessing import Pool

    files_ok = True
    try:
        processes_pool = Pool(1)
        for vec_file in vec_files:
            if print_file_names:
                print(vec_file)
            params = [
                vec_file,
                chk_proj,
                epsg_code,
                max_file_size,
                rm_err,
                print_err,
                multi_file,
            ]
            try:
                result = processes_pool.apply_async(
                    _run_check_gdal_vector_file, args=[params]
                )
                file_ok = result.get(timeout=timeout)
                if not file_ok:
                    files_ok = False
            except Exception as e:
                files_ok = False
                if rm_err:
                    if multi_file:
                        rsgislib.tools.filetools.delete_file_with_basename(vec_file)
                    else:
                        rsgislib.tools.filetools.delete_file_silent(vec_file)
                        print("Removed {}".format(vec_file))
                else:
                    print("rm {}".format(vec_file))
                continue
        processes_pool.close()
        # processes_pool.join()
    except Exception as inst:
        print("Finished with pool")

    return files_ok


def check_hdf5_file(input_file: str):
    """
    A function which checks whether a HDF5 file is valid.

    :param input_file: the file path to the input file.
    :return: a boolean - True file is valid. False file is not valid.

    """
    import h5py

    def _check_h5_var(h5_obj):
        lcl_ok = True
        lcl_err = ""
        try:
            if isinstance(h5_obj, h5py.Dataset):
                lcl_ok = True
            elif isinstance(h5_obj, h5py.Group):
                for var in h5_obj.keys():
                    lcl_ok, lcl_err = _check_h5_var(h5_obj[var])
                    if not lcl_ok:
                        break
        except RuntimeError as e:
            lcl_ok = False
            lcl_err = str(e)
        except Exception as e:
            lcl_ok = False
            lcl_err = str(e)
        return lcl_ok, lcl_err

    glb_ok = True
    err_str = ""
    if not os.path.exists(input_file):
        glb_ok = False
    else:
        try:
            fH5 = h5py.File(input_file, "r")
            if fH5 is None:
                glb_ok = False
                err_str = "Could not open HDF5 file"
            else:
                for var in fH5.keys():
                    glb_ok, err_str = _check_h5_var(fH5[var])
                    if not glb_ok:
                        break
        except RuntimeError as e:
            glb_ok = False
            err_str = str(e)
        except Exception as e:
            glb_ok = False
            err_str = str(e)
    return glb_ok, err_str


def _run_check_hdf5_file(file_params: list):
    """
    Internal function called by check_hdf5_file - not intended to be called
    by end user.

    :param file_params: list of function inputs.
    :return: boolean whether the specified file is OK or not.

    """
    input_file = file_params[0]
    rm_err = file_params[1]
    print_err = file_params[2]

    try:
        file_ok, err_str = check_hdf5_file(input_file)
        if print_err and (not file_ok):
            print("Error: '{}'".format(err_str))
        if not file_ok:
            if rm_err:
                rsgislib.tools.filetools.delete_file_silent(input_file)
                print("Removed {}".format(input_file))
            else:
                print("rm {}".format(input_file))
    except Exception as e:
        if print_err:
            print("Error: '{}'".format(e))
        if rm_err:
            rsgislib.tools.filetools.delete_file_silent(input_file)
            print("Removed {}".format(input_file))
        else:
            print("rm {}".format(input_file))
        file_ok = False

    return file_ok


def run_check_hdf5_file(
    input_file: str, rm_err: bool = False, print_err: bool = True, timeout: int = 4
):
    """
    A function which checks a HDF5 file using the check_hdf5_file function
    where a mutliprocessing object is used to catch errors which can crash Python
    and still continue without crashing the Python environment.

    You probably want to call this function rather than calling check_hdf5_file
    directly.

    :param input_file: the file path to the HDF5 file.
    :param rm_err: boolean specifying whether to delete the file if an error is found
    :param print_err: print any errors associated with the file to the console
    :param timeout: a timeout in seconds (Default = 4) for the tests to be undertaken.
    :return: boolean specifying whether the file is OK (i.e., tests passed) or not.

    """
    from multiprocessing import Pool

    processes_pool = Pool(1)
    params = [input_file, rm_err, print_err]
    try:
        result = processes_pool.apply_async(_run_check_hdf5_file, args=[params])
        file_ok = result.get(timeout=timeout)
    except Exception as e:
        file_ok = False
        if rm_err:
            rsgislib.tools.filetools.delete_file_silent(input_file)
            print("Removed {}".format(input_file))
        else:
            print("rm {}".format(input_file))
    processes_pool.close()
    # processes_pool.join()

    return file_ok


def run_check_hdf5_files(
    input_files: list,
    rm_err: bool = False,
    print_err: bool = True,
    print_file_names: bool = False,
    timeout: int = 4,
):
    """
    A function which checks a list of HDF5 files using the
    check_hdf5_file function where a mutliprocessing object is used to
    catch errors which can crash Python and still continue without crashing
    the Python environment.

    You probably want to call this function rather than calling check_hdf5_file
    directly.

    :param input_files: a list of input HDF5 file paths.
    :param rm_err: boolean specifying whether to delete the file if an error is found
    :param print_err: print any errors associated with the file to the console
    :param print_file_names: print the names of the file before they are tested.
    :param timeout: a timeout in seconds (Default = 4) for the tests to be undertaken.
    :return: boolean specifying whether the file is OK (i.e., tests passed) or not.

    """
    from multiprocessing import Pool

    files_ok = True
    try:
        processes_pool = Pool(1)
        for input_file in input_files:
            if print_file_names:
                print(input_file)
            params = [input_file, rm_err, print_err]
            try:
                result = processes_pool.apply_async(_run_check_hdf5_file, args=[params])
                file_ok = result.get(timeout=timeout)
                if not file_ok:
                    files_ok = False
            except Exception as e:
                files_ok = False
                if rm_err:
                    rsgislib.tools.filetools.delete_file_silent(input_file)
                    print("Removed {}".format(input_file))
                else:
                    print("rm {}".format(input_file))
                continue
        processes_pool.close()
        # processes_pool.join()
    except Exception as inst:
        print("Finished with pool")

    return files_ok


def cmp_to_ref_imgs(
    ref_imgs: List[str],
    input_img_dir: str,
    input_img_ext: str,
    rm_errs: bool = False,
    output_file: str = None,
) -> Dict:
    """
    A utility which checks an image against a reference image
    (i.e., projection matches, number of pixels and coordinates).
    Note. the reference image file name assumed to be within the name
    of the images within the --input directory. If there are multiple
    images which match the reference image file name they will all be checked.

    :param ref_imgs: List of reference image paths.
    :param input_img_dir: Input image directory, containing the images to be checked
    :param input_img_ext: The image extension to be checked (e.g., tif, kea)
    :param rm_errs: Boolean specifying whether to delete the file if an error is found
    :param output_file: optional output report with list of images checked,
                        not checked and errors.

    """
    import tqdm
    import rsgislib.tools.utils

    input_imgs = glob.glob(os.path.join(input_img_dir, f"*.{input_img_ext}"))
    input_imgs_lut = dict()
    for input_img in input_imgs:
        basename = rsgislib.tools.filetools.get_file_basename(input_img)
        input_imgs_lut[basename] = [input_img, False]

    out_report = dict()
    out_report["checked"] = list()
    out_report["not_checked"] = list()
    out_report["errors"] = list()
    out_report["ref_no_match"] = list()

    for ref_img in tqdm.tqdm(ref_imgs):
        ref_base = rsgislib.tools.filetools.get_file_basename(ref_img)
        chk_imgs = glob.glob(
            os.path.join(input_img_dir, f"*{ref_base}*.{input_img_ext}")
        )
        if len(chk_imgs) > 0:
            ref_img_size = rsgislib.imageutils.get_img_size(ref_img)
            ref_img_bbox = rsgislib.imageutils.get_img_bbox(ref_img)
            ref_img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(ref_img)

            for chk_img in chk_imgs:
                error = False
                chk_img_size = rsgislib.imageutils.get_img_size(chk_img)
                chk_img_bbox = rsgislib.imageutils.get_img_bbox(chk_img)
                chk_img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(chk_img)

                if (ref_img_size[0] != chk_img_size[0]) or (
                    ref_img_size[1] != chk_img_size[1]
                ):
                    out_report["errors"].append(chk_img)
                    error = True
                elif (
                    (ref_img_bbox[0] != chk_img_bbox[0])
                    or (ref_img_bbox[1] != chk_img_bbox[1])
                    or (ref_img_bbox[2] != chk_img_bbox[2])
                    or (ref_img_bbox[3] != chk_img_bbox[3])
                ):
                    out_report["errors"].append(chk_img)
                    error = True
                elif ref_img_epsg != chk_img_epsg:
                    out_report["errors"].append(chk_img)
                    error = True

                out_report["checked"].append(chk_img)
                chk_basename = rsgislib.tools.filetools.get_file_basename(chk_img)
                input_imgs_lut[chk_basename][1] = True

                if error and rm_errs:
                    rsgislib.tools.filetools.delete_file_silent(chk_img)
        else:
            out_report["ref_no_match"].append(ref_img)

    for input_base in input_imgs_lut:
        if not input_imgs_lut[input_base][1]:
            out_report["not_checked"].append(input_imgs_lut[input_base][0])

    if output_file is not None:
        rsgislib.tools.utils.write_dict_to_json(out_report, output_file)

    return out_report
