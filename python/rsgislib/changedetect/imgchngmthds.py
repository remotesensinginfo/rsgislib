"""
Functions for image to image change methods.

Some functions adapted from:
https://github.com/ChenHongruixuan/ChangeDetectionRepository

"""

from typing import List
import numpy
import tqdm

import rsgislib
import rsgislib.imageutils
import rsgislib.tools.stats


def _get_binary_change_map(data: numpy.array, max_iter: int = 1500) -> numpy.array:
    """
    A function which uses kmeans to create a binary output.

    :param data: a flattened numpy array
    :return: binary numpy array

    """
    from sklearn.cluster import KMeans

    cluster_center = (
        KMeans(n_clusters=2, max_iter=max_iter).fit(data.T).cluster_centers_.T
    )  # shape: (1, 2)

    dis_1 = numpy.linalg.norm(data - cluster_center[0, 0], axis=0, keepdims=True)
    dis_2 = numpy.linalg.norm(data - cluster_center[0, 1], axis=0, keepdims=True)

    bin_arr = numpy.zeros_like(data, dtype=int)  # binary change map
    if cluster_center[0, 0] > cluster_center[0, 1]:
        bin_arr[dis_1 > dis_2] = 0
        bin_arr[dis_1 <= dis_2] = 1
    else:
        bin_arr[dis_1 > dis_2] = 1
        bin_arr[dis_1 <= dis_2] = 0

    return bin_arr


def image_difference(
    in_base_img: str,
    in_chng_img: str,
    out_diff_img: str,
    vld_msk_img: str = None,
    vld_msk_val: int = 1,
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
    base_img_no_data: float = 0,
    chng_img_no_data: float = 0,
):
    """
    A function to calculate the difference between two images.

    :param in_base_img: The input base image file path
    :param in_chng_img: The input change image file path
    :param out_diff_img: Output differance image file path
    :param vld_msk_img: An optional valid pixel image mask - if not specified
                        the no data values of the input images will be used.
    :param vld_msk_val: the pixel value within in the mask defining which pixels
                        are valid. Default is 1.
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image
    :param base_img_no_data: Optionally specified nodata value for the base image
    :param chng_img_no_data: Optionally specified nodata value for the change image

    """
    imgs_match = rsgislib.imageutils.check_img_file_comparison(
        in_base_img,
        in_chng_img,
        test_n_bands=False,
        test_eql_bbox=True,
        print_errors=True,
    )
    if not imgs_match:
        raise rsgislib.RSGISPyException("Image does not match")

    if img_base_bands is not None:
        n_base_bands = len(img_base_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_base_bands is not None but img_chng_bands is None"
            )
    else:
        n_base_bands = rsgislib.imageutils.get_img_band_count(in_base_img)

    if img_chng_bands is not None:
        n_chng_bands = len(img_chng_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_chng_bands is not None but img_base_bands is None"
            )
    else:
        n_chng_bands = rsgislib.imageutils.get_img_band_count(in_chng_img)

    if n_base_bands != n_chng_bands:
        raise rsgislib.RSGISPyException(
            "The number of bands specified for the two images do not match"
        )
    n_bands = n_base_bands
    img_size_x, img_size_y = rsgislib.imageutils.get_img_size(in_base_img)
    n_pxls = img_size_x * img_size_y

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    base_img_data = base_img_data.astype(numpy.dtype("float32"))
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )
    chng_img_data = chng_img_data.astype(numpy.dtype("float32"))

    base_img_data = numpy.nan_to_num(
        base_img_data,
        copy=False,
        nan=base_img_no_data,
        posinf=base_img_no_data,
        neginf=base_img_no_data,
    )
    chng_img_data = numpy.nan_to_num(
        chng_img_data,
        copy=False,
        nan=chng_img_no_data,
        posinf=chng_img_no_data,
        neginf=chng_img_no_data,
    )

    # Get the valid mask - either read mask file or use no data value.
    if vld_msk_img is not None:
        vld_msk_arr = rsgislib.imageutils.get_img_data_as_arr(
            vld_msk_img, img_bands=[1]
        )
    else:
        vld_msk_arr = numpy.zeros([1, img_size_y, img_size_x], dtype=int)
        vld_msk_arr[base_img_data != base_img_no_data] = vld_msk_val
        vld_msk_arr[chng_img_data != chng_img_no_data] = vld_msk_val

    # Flatten / Reshape  data.
    base_flat_data = base_img_data.reshape([n_bands, n_pxls])
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = chng_img_data.reshape([n_bands, n_pxls])
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)
    vld_flat_data = vld_msk_arr.reshape([n_pxls])

    # Create Pixel IDs
    pxl_ids = numpy.arange(0, n_pxls, 1)
    pxl_ids = pxl_ids[vld_flat_data == vld_msk_val]

    # Mask the image data to the valid pixels
    base_flat_data = base_flat_data[vld_flat_data == vld_msk_val]
    chng_flat_data = chng_flat_data[vld_flat_data == vld_msk_val]

    # Reshape the arrays so bands are first dim
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)

    # If specified standardise the image bands.
    if apply_std:
        base_flat_data = rsgislib.tools.stats.standarise_img_data(base_flat_data)
        chng_flat_data = rsgislib.tools.stats.standarise_img_data(chng_flat_data)

    # Calculate the image difference
    img_diff = base_flat_data - chng_flat_data

    # Reshape and used pxl_ids to un-mask the output.
    img_diff = numpy.moveaxis(img_diff, 0, -1)
    diff_img_arr = numpy.zeros([n_pxls, n_bands], dtype=float)
    diff_img_arr[pxl_ids] = img_diff
    diff_img_arr = numpy.moveaxis(diff_img_arr, 0, -1)
    diff_img_arr = diff_img_arr.reshape([n_bands, img_size_y, img_size_x])

    # Write the output image file.
    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=diff_img_arr,
        output_img=out_diff_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )


def image_ratio(
    in_base_img: str,
    in_chng_img: str,
    out_ratio_img: str,
    vld_msk_img: str = None,
    vld_msk_val: int = 1,
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
    base_img_no_data: float = 0,
    chng_img_no_data: float = 0,
):
    """
    A function to calculate a ratio difference between two images.

    :param in_base_img: The input base image file path
    :param in_chng_img: The input change image file path
    :param out_ratio_img: Output ratio image file path
    :param vld_msk_img: An optional valid pixel image mask - if not specified
                        the no data values of the input images will be used.
    :param vld_msk_val: the pixel value within in the mask defining which pixels
                        are valid. Default is 1.
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image
    :param base_img_no_data: Optionally specified nodata value for the base image
    :param chng_img_no_data: Optionally specified nodata value for the change image

    """
    imgs_match = rsgislib.imageutils.check_img_file_comparison(
        in_base_img,
        in_chng_img,
        test_n_bands=False,
        test_eql_bbox=True,
        print_errors=True,
    )
    if not imgs_match:
        raise rsgislib.RSGISPyException("Image does not match")

    if img_base_bands is not None:
        n_base_bands = len(img_base_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_base_bands is not None but img_chng_bands is None"
            )
    else:
        n_base_bands = rsgislib.imageutils.get_img_band_count(in_base_img)

    if img_chng_bands is not None:
        n_chng_bands = len(img_chng_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_chng_bands is not None but img_base_bands is None"
            )
    else:
        n_chng_bands = rsgislib.imageutils.get_img_band_count(in_chng_img)

    if n_base_bands != n_chng_bands:
        raise rsgislib.RSGISPyException(
            "The number of bands specified for the two images do not match"
        )
    n_bands = n_base_bands
    img_size_x, img_size_y = rsgislib.imageutils.get_img_size(in_base_img)
    n_pxls = img_size_x * img_size_y

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    base_img_data = base_img_data.astype(numpy.dtype("float32"))
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )
    chng_img_data = chng_img_data.astype(numpy.dtype("float32"))

    base_img_data = numpy.nan_to_num(
        base_img_data,
        copy=False,
        nan=base_img_no_data,
        posinf=base_img_no_data,
        neginf=base_img_no_data,
    )
    chng_img_data = numpy.nan_to_num(
        chng_img_data,
        copy=False,
        nan=chng_img_no_data,
        posinf=chng_img_no_data,
        neginf=chng_img_no_data,
    )

    # Get the valid mask - either read mask file or use no data value.
    if vld_msk_img is not None:
        vld_msk_arr = rsgislib.imageutils.get_img_data_as_arr(
            vld_msk_img, img_bands=[1]
        )
    else:
        vld_msk_arr = numpy.zeros([1, img_size_y, img_size_x], dtype=int)
        vld_msk_arr[base_img_data != base_img_no_data] = vld_msk_val
        vld_msk_arr[chng_img_data != chng_img_no_data] = vld_msk_val

    # Flatten / Reshape  data.
    base_flat_data = base_img_data.reshape([n_bands, n_pxls])
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = chng_img_data.reshape([n_bands, n_pxls])
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)
    vld_flat_data = vld_msk_arr.reshape([n_pxls])

    # Create Pixel IDs
    pxl_ids = numpy.arange(0, n_pxls, 1)
    pxl_ids = pxl_ids[vld_flat_data == vld_msk_val]

    # Mask the image data to the valid pixels
    base_flat_data = base_flat_data[vld_flat_data == vld_msk_val]
    chng_flat_data = chng_flat_data[vld_flat_data == vld_msk_val]

    # Reshape the arrays so bands are first dim
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)

    # If specified standardise the image bands.
    if apply_std:
        base_flat_data = rsgislib.tools.stats.standarise_img_data(base_flat_data)
        chng_flat_data = rsgislib.tools.stats.standarise_img_data(chng_flat_data)

    # Calculate the image difference
    img_ratio = base_flat_data / chng_flat_data
    numpy.nan_to_num(img_ratio, copy=False, nan=0.0, posinf=0.0, neginf=0.0)

    # Reshape and used pxl_ids to un-mask the output.
    img_ratio = numpy.moveaxis(img_ratio, 0, -1)
    ratio_img_arr = numpy.zeros([n_pxls, n_bands], dtype=float)
    ratio_img_arr[pxl_ids] = img_ratio
    ratio_img_arr = numpy.moveaxis(ratio_img_arr, 0, -1)
    ratio_img_arr = ratio_img_arr.reshape([n_bands, img_size_y, img_size_x])

    # Write the output image file.
    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=ratio_img_arr,
        output_img=out_ratio_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )


def change_vector_analysis(
    in_base_img: str,
    in_chng_img: str,
    out_chng_img: str,
    out_chng_dist_img: str,
    vld_msk_img: str = None,
    vld_msk_val: int = 1,
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
    use_kmeans: bool = False,
    base_img_no_data: float = 0,
    chng_img_no_data: float = 0,
):
    """
    A function which performs Change vector analysis (CVA) between two images.
    The two images must have exactly the extent, resolution and image size
    (width and height).

    :param in_base_img: The input base image file path
    :param in_chng_img: The input change image file path
    :param out_chng_img: Output binary change image file path
    :param out_chng_dist_img: Output change distance image file path
    :param vld_msk_img: An optional valid pixel image mask - if not specified
                        the no data values of the input images will be used.
    :param vld_msk_val: the pixel value within in the mask defining which pixels
                        are valid. Default is 1.
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image
    :param base_img_no_data: Optionally specified nodata value for the base image
    :param chng_img_no_data: Optionally specified nodata value for the change image

    """
    imgs_match = rsgislib.imageutils.check_img_file_comparison(
        in_base_img,
        in_chng_img,
        test_n_bands=False,
        test_eql_bbox=True,
        print_errors=True,
    )
    if not imgs_match:
        raise rsgislib.RSGISPyException("Image does not match")

    if img_base_bands is not None:
        n_base_bands = len(img_base_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_base_bands is not None but img_chng_bands is None"
            )
    else:
        n_base_bands = rsgislib.imageutils.get_img_band_count(in_base_img)

    if img_chng_bands is not None:
        n_chng_bands = len(img_chng_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_chng_bands is not None but img_base_bands is None"
            )
    else:
        n_chng_bands = rsgislib.imageutils.get_img_band_count(in_chng_img)

    if n_base_bands != n_chng_bands:
        raise rsgislib.RSGISPyException(
            "The number of bands specified for the two images do not match"
        )
    n_bands = n_base_bands

    img_size_x, img_size_y = rsgislib.imageutils.get_img_size(in_base_img)
    n_pxls = img_size_y * img_size_x

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    base_img_data = base_img_data.astype(numpy.dtype("float32"))
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )
    chng_img_data = chng_img_data.astype(numpy.dtype("float32"))

    # Mask any nan values to the no data value.
    base_img_data = numpy.nan_to_num(
        base_img_data,
        copy=False,
        nan=base_img_no_data,
        posinf=base_img_no_data,
        neginf=base_img_no_data,
    )
    chng_img_data = numpy.nan_to_num(
        chng_img_data,
        copy=False,
        nan=chng_img_no_data,
        posinf=chng_img_no_data,
        neginf=chng_img_no_data,
    )

    # Get the valid mask - either read mask file or use no data value.
    if vld_msk_img is not None:
        vld_msk_arr = rsgislib.imageutils.get_img_data_as_arr(
            vld_msk_img, img_bands=[1]
        )
    else:
        vld_msk_arr = numpy.zeros([1, img_size_y, img_size_x], dtype=int)
        vld_msk_arr[base_img_data != base_img_no_data] = vld_msk_val
        vld_msk_arr[chng_img_data != chng_img_no_data] = vld_msk_val

    # Flatten / Reshape  data.
    base_flat_data = base_img_data.reshape([n_bands, n_pxls])
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = chng_img_data.reshape([n_bands, n_pxls])
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)
    vld_flat_data = vld_msk_arr.reshape([n_pxls])

    # Create Pixel IDs
    pxl_ids = numpy.arange(0, n_pxls, 1)
    pxl_ids = pxl_ids[vld_flat_data == vld_msk_val]

    # Mask the image data to the valid pixels
    base_flat_data = base_flat_data[vld_flat_data == vld_msk_val]
    chng_flat_data = chng_flat_data[vld_flat_data == vld_msk_val]

    # Reshape the arrays so bands are first dim
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)

    # Check there is a useful number of pixels
    if base_flat_data.shape[1] < 50:
        raise rsgislib.RSGISPyException("There are less then 50 valid pixels")

    # If specified standardise the image bands.
    if apply_std:
        base_flat_data = rsgislib.tools.stats.standarise_img_data(base_flat_data)
        chng_flat_data = rsgislib.tools.stats.standarise_img_data(chng_flat_data)

    # Calculate the difference between the base and change pixels
    img_diff = base_flat_data - chng_flat_data

    # Calculate the l2 normalised difference
    l2_norm = numpy.sqrt(numpy.sum(numpy.square(img_diff), axis=0))
    numpy.nan_to_num(l2_norm, copy=False, nan=0.0, posinf=0.0, neginf=0.0)

    # Calculate the otsu threshold
    otsu_thres = rsgislib.tools.stats.calc_otsu_threshold(l2_norm)

    # Reshape the l2 normalised difference and populate masked values.
    l2_norm_flat_arr = numpy.zeros([n_pxls], dtype=float)
    l2_norm_flat_arr[pxl_ids] = l2_norm
    l2_norm_img_arr = l2_norm_flat_arr.reshape([1, img_size_y, img_size_x])

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=l2_norm_img_arr - otsu_thres,
        output_img=out_chng_dist_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )

    if use_kmeans:
        print("Applying KMeans clustering...")
        chng_bin_data = _get_binary_change_map(l2_norm)
        chng_bin_data_flat_arr = numpy.zeros([n_pxls], dtype=int)
        chng_bin_data_flat_arr[pxl_ids] = chng_bin_data
        chng_bin_img_arr = chng_bin_data_flat_arr.reshape([1, img_size_y, img_size_x])
    else:
        chng_bin_img_arr = numpy.zeros((1, img_size_y, img_size_x))
        chng_bin_img_arr[l2_norm_img_arr > otsu_thres] = 1

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=chng_bin_img_arr,
        output_img=out_chng_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_8UINT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=255,
    )


def slow_feature_analysis(
    in_base_img: str,
    in_chng_img: str,
    out_chng_img: str,
    out_chng_dist_img: str,
    vld_msk_img: str = None,
    vld_msk_val: int = 1,
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
    sfa_max_iter: int = 50,
    sfa_epsilon: float = 1e-6,
    sfa_norm_trans: bool = False,
    sfa_regular: bool = False,
    use_kmeans: bool = False,
    base_img_no_data: float = 0,
    chng_img_no_data: float = 0,
):
    """
    A function which performs Slow Feature Analysis (SFA) between two images.
    The two images must have exactly the extent, resolution and image size
    (width and height). There must be at least three image bands.

    C. Wu, B. Du, and L. Zhang, “Slow feature analysis for change detection
    in multispectral imagery,” IEEE Trans. Geosci. Remote Sens.,
    vol. 52, no. 5, pp. 2858–2874, 2014.

    :param in_base_img: The input base image file path
    :param in_chng_img: The input change image file path
    :param out_chng_img: Output binary change image file path
    :param out_chng_dist_img: Output change distance image file path
    :param vld_msk_img: An optional valid pixel image mask - if not specified
                        the no data values of the input images will be used.
    :param vld_msk_val: the pixel value within in the mask defining which pixels
                        are valid. Default is 1.
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image
    :param sfa_max_iter: Maximum number of iterations for the algorithm to converge.
    :param sfa_epsilon: Threshold for convergence.
    :param sfa_norm_trans: Specifies whether to normalise the transformation matrix
    :param sfa_regular: Specifies whether to regularise the transformation matrix
    :param base_img_no_data: Optionally specified nodata value for the base image
    :param chng_img_no_data: Optionally specified nodata value for the change image

    """
    import scipy.linalg
    import scipy.stats

    imgs_match = rsgislib.imageutils.check_img_file_comparison(
        in_base_img,
        in_chng_img,
        test_n_bands=False,
        test_eql_bbox=True,
        print_errors=True,
    )
    if not imgs_match:
        raise rsgislib.RSGISPyException("Image does not match")

    if img_base_bands is not None:
        n_base_bands = len(img_base_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_base_bands is not None but img_chng_bands is None"
            )
    else:
        n_base_bands = rsgislib.imageutils.get_img_band_count(in_base_img)

    if img_chng_bands is not None:
        n_chng_bands = len(img_chng_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_chng_bands is not None but img_base_bands is None"
            )
    else:
        n_chng_bands = rsgislib.imageutils.get_img_band_count(in_chng_img)

    if n_base_bands != n_chng_bands:
        raise rsgislib.RSGISPyException(
            "The number of bands specified for the two images do not match"
        )
    n_bands = n_base_bands
    if n_bands < 3:
        raise rsgislib.RSGISPyException("There must be at least 3 bands")

    img_size_x, img_size_y = rsgislib.imageutils.get_img_size(in_base_img)
    n_pxls = img_size_y * img_size_x

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    base_img_data = base_img_data.astype(numpy.dtype("float32"))
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )
    chng_img_data = chng_img_data.astype(numpy.dtype("float32"))

    base_img_data = numpy.nan_to_num(
        base_img_data,
        copy=False,
        nan=base_img_no_data,
        posinf=base_img_no_data,
        neginf=base_img_no_data,
    )
    chng_img_data = numpy.nan_to_num(
        chng_img_data,
        copy=False,
        nan=chng_img_no_data,
        posinf=chng_img_no_data,
        neginf=chng_img_no_data,
    )

    # Get the valid mask - either read mask file or use no data value.
    if vld_msk_img is not None:
        vld_msk_arr = rsgislib.imageutils.get_img_data_as_arr(
            vld_msk_img, img_bands=[1]
        )
    else:
        vld_msk_arr = numpy.zeros([1, img_size_y, img_size_x], dtype=int)
        vld_msk_arr[base_img_data != base_img_no_data] = vld_msk_val
        vld_msk_arr[chng_img_data != chng_img_no_data] = vld_msk_val

    # Flatten / Reshape  data.
    base_flat_data = base_img_data.reshape([n_bands, n_pxls])
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = chng_img_data.reshape([n_bands, n_pxls])
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)
    vld_flat_data = vld_msk_arr.reshape([n_pxls])

    # Create Pixel IDs
    pxl_ids = numpy.arange(0, n_pxls, 1)
    pxl_ids = pxl_ids[vld_flat_data == vld_msk_val]

    # Mask the image data to the valid pixels
    base_flat_data = base_flat_data[vld_flat_data == vld_msk_val]
    chng_flat_data = chng_flat_data[vld_flat_data == vld_msk_val]

    # Reshape the arrays so bands are first dim
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)

    # The number of vaild pixels after masking
    n_vld_msks = base_flat_data.shape[1]

    # Check there is a useful number of pixels
    if n_vld_msks < 50:
        raise rsgislib.RSGISPyException("There are less then 50 valid pixels")

    # If specified standardise the image bands.
    if apply_std:
        base_flat_data = rsgislib.tools.stats.standarise_img_data(base_flat_data)
        chng_flat_data = rsgislib.tools.stats.standarise_img_data(chng_flat_data)

    # Initialise Omega
    arr_L = numpy.zeros((n_bands - 2, n_bands))  # (C-2, C)
    for i in range(n_bands - 2):
        arr_L[i, i] = 1
        arr_L[i, i + 1] = -2
        arr_L[i, i + 2] = 1
    arr_omega = numpy.dot(arr_L.T, arr_L)  # (C, C)

    P = n_vld_msks
    # row-major order after reshape
    img_X = base_flat_data
    img_Y = chng_flat_data
    lamb = 100 * numpy.ones((n_bands, 1))
    all_lambda = []
    weight = numpy.ones((n_vld_msks))

    weight = numpy.reshape(weight, (-1, n_vld_msks))
    for _iter in tqdm.tqdm(range(sfa_max_iter)):
        sum_w = numpy.sum(weight)
        mean_X = numpy.sum(weight * img_X, axis=1, keepdims=True) / numpy.sum(
            weight
        )  # (band, 1)
        mean_Y = numpy.sum(weight * img_Y, axis=1, keepdims=True) / numpy.sum(
            weight
        )  # (band, 1)
        center_X = img_X - mean_X
        center_Y = img_Y - mean_Y

        # cov_XY = covw(center_X, center_Y, weight)  # (2 * band, 2 * band)
        # cov_X = cov_XY[0:n_bands, 0:n_bands]
        # cov_Y = cov_XY[n_bands:2 * n_bands, n_bands:2 * n_bands]
        var_X = numpy.sum(weight * numpy.power(center_X, 2), axis=1, keepdims=True) / (
            (P - 1) * sum_w / P
        )
        var_Y = numpy.sum(weight * numpy.power(center_Y, 2), axis=1, keepdims=True) / (
            (P - 1) * sum_w / P
        )
        std_X = numpy.reshape(numpy.sqrt(var_X), (n_bands, 1))
        std_Y = numpy.reshape(numpy.sqrt(var_Y), (n_bands, 1))

        # normalize image
        norm_X = center_X / std_X
        norm_Y = center_Y / std_Y
        diff_img = norm_X - norm_Y
        mat_A = numpy.dot(weight * diff_img, diff_img.T) / ((P - 1) * sum_w / P)
        mat_B = (
            numpy.dot(weight * norm_X, norm_X.T) + numpy.dot(weight * norm_Y, norm_Y.T)
        ) / (2 * (P - 1) * sum_w / P)
        if sfa_regular:
            penalty = numpy.trace(mat_B) / numpy.trace(arr_omega)
            mat_B += penalty * arr_omega

        # solve generalized eigenvalue problem and get eigenvalues and eigenvector
        eigenvalue, eigenvector = scipy.linalg.eig(mat_A, mat_B)
        eigenvalue = eigenvalue.real  # discard imaginary part
        idx = eigenvalue.argsort()
        eigenvalue = eigenvalue[idx]

        # make sure the max absolute value of vector is 1,
        # and the final result will be more closer to the matlab result
        aux = numpy.reshape(numpy.abs(eigenvector).max(axis=0), (1, n_bands))
        eigenvector = eigenvector / aux

        # print sqrt(lambda)
        # if (_iter + 1) == 1:
        #    print('sqrt lambda:')
        # print(numpy.sqrt(eigenvalue))

        eigenvalue = numpy.reshape(eigenvalue, (n_bands, 1))  # (band, 1)
        threshold = numpy.max(numpy.abs(numpy.sqrt(lamb) - numpy.sqrt(eigenvalue)))
        # if sqrt(lambda) converge
        if threshold < sfa_epsilon:
            break
        lamb = eigenvalue
        all_lambda = (
            lamb if (_iter + 1) == 1 else numpy.concatenate((all_lambda, lamb), axis=1)
        )

        # the order of the slowest features is determined by the order of the eigenvalues
        trans_mat = eigenvector[:, idx]
        # satisfy the constraints(3)
        if sfa_norm_trans:
            output_signal_std = 1 / numpy.sqrt(
                numpy.diag(numpy.dot(trans_mat.T, numpy.dot(mat_B, trans_mat)))
            )
            trans_mat = output_signal_std * trans_mat
        isfa_variable = numpy.dot(trans_mat.T, norm_X) - numpy.dot(trans_mat.T, norm_Y)

        if (_iter + 1) == 1:
            T = numpy.sum(
                numpy.square(isfa_variable) / numpy.sqrt(lamb), axis=0, keepdims=True
            )  # chi square
        else:
            T = numpy.sum(
                numpy.square(isfa_variable) / numpy.sqrt(lamb), axis=0, keepdims=True
            )  # IWD
        weight = 1 - scipy.stats.chi2.cdf(T, n_bands)

    if (_iter + 1) == sfa_max_iter:
        print("Warning: The lambda may not have converged")
    else:
        print(f"Lambda has converged on iteration {_iter + 1}")
    # return isfa_variable, lamb, all_lambda, trans_mat, T, weight

    sqrt_chi2 = numpy.sqrt(T)
    numpy.nan_to_num(sqrt_chi2, copy=False, nan=0.0, posinf=0.0, neginf=0.0)
    otsu_thres = rsgislib.tools.stats.calc_otsu_threshold(sqrt_chi2)

    sqrt_chi2_flat_arr = numpy.zeros([n_pxls], dtype=float)
    sqrt_chi2_flat_arr[pxl_ids] = sqrt_chi2
    sqrt_chi2_img = sqrt_chi2_flat_arr.reshape([1, img_size_y, img_size_x])

    chng_arr = sqrt_chi2_img - otsu_thres
    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=chng_arr,
        output_img=out_chng_dist_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )

    if use_kmeans:
        print("Applying KMeans clustering...")
        chng_bin_data = _get_binary_change_map(sqrt_chi2)
        chng_bin_data_flat_arr = numpy.zeros([n_pxls], dtype=int)
        chng_bin_data_flat_arr[pxl_ids] = chng_bin_data
        chng_bin_img_arr = chng_bin_data_flat_arr.reshape([1, img_size_y, img_size_x])
    else:
        chng_bin_img_arr = numpy.zeros((img_size_y, img_size_x))
        chng_bin_img_arr[sqrt_chi2_img[0] > otsu_thres] = 1

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=chng_bin_img_arr,
        output_img=out_chng_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_8UINT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=255,
    )


def multivariate_alteration_detection(
    in_base_img: str,
    in_chng_img: str,
    out_chng_img: str,
    out_chng_dist_img: str,
    out_chng_mads_img: str,
    vld_msk_img: str = None,
    vld_msk_val: int = 1,
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
    mad_max_iter: int = 50,
    mad_epsilon: float = 1e-6,
    use_kmeans: bool = True,
    base_img_no_data: float = 0,
    chng_img_no_data: float = 0,
):
    """
    A function which performs Multivariate Alteration Detection (MAD) between
    two images. The two images must have exactly the extent, resolution and
    image size (width and height). MAD is a change detection algorithm based
    on canonical correlation analysis (CCA) that aims to maximize the variance
    of projection feature difference.

    :param in_base_img: The input base image file path
    :param in_chng_img: The input change image file path
    :param out_chng_img: Output binary change image file path
    :param out_chng_dist_img: Output change distance image file path
    :param out_chng_mads_img: Output change MADs variates image file path
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image
    :param mad_max_iter: Maximum number of iterations for the algorithm to converge.
    :param mad_epsilon: Threshold for convergence.
    :param base_img_no_data: The base image no data value.
    :param chng_img_no_data: The change image no data value.
    :param base_img_no_data: Optionally specified nodata value for the base image
    :param chng_img_no_data: Optionally specified nodata value for the change image

    """
    import numpy.linalg
    import scipy.stats

    def _covw(center_X, center_Y, w):
        n = w.shape[1]
        sqrt_w = numpy.sqrt(w)
        sum_w = w.sum()
        V = numpy.concatenate((center_X, center_Y), axis=0)
        V = sqrt_w * V
        dis = numpy.dot(V, V.T) / sum_w * (n / (n - 1))
        return dis

    imgs_match = rsgislib.imageutils.check_img_file_comparison(
        in_base_img,
        in_chng_img,
        test_n_bands=False,
        test_eql_bbox=True,
        print_errors=True,
    )
    if not imgs_match:
        raise rsgislib.RSGISPyException("Image does not match")

    if img_base_bands is not None:
        n_base_bands = len(img_base_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_base_bands is not None but img_chng_bands is None"
            )
    else:
        n_base_bands = rsgislib.imageutils.get_img_band_count(in_base_img)

    if img_chng_bands is not None:
        n_chng_bands = len(img_chng_bands)
        if img_chng_bands is None:
            raise rsgislib.RSGISPyException(
                "img_chng_bands is not None but img_base_bands is None"
            )
    else:
        n_chng_bands = rsgislib.imageutils.get_img_band_count(in_chng_img)

    if n_base_bands != n_chng_bands:
        raise rsgislib.RSGISPyException(
            "The number of bands specified for the two images do not match"
        )
    n_bands = n_base_bands
    img_size_x, img_size_y = rsgislib.imageutils.get_img_size(in_base_img)
    n_pxls = img_size_x * img_size_y

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    base_img_data = base_img_data.astype(numpy.dtype("float32"))
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )
    chng_img_data = chng_img_data.astype(numpy.dtype("float32"))

    base_img_data = numpy.nan_to_num(
        base_img_data,
        copy=False,
        nan=base_img_no_data,
        posinf=base_img_no_data,
        neginf=base_img_no_data,
    )
    chng_img_data = numpy.nan_to_num(
        chng_img_data,
        copy=False,
        nan=chng_img_no_data,
        posinf=chng_img_no_data,
        neginf=chng_img_no_data,
    )

    # Get the valid mask - either read mask file or use no data value.
    if vld_msk_img is not None:
        vld_msk_arr = rsgislib.imageutils.get_img_data_as_arr(
            vld_msk_img, img_bands=[1]
        )
    else:
        vld_msk_arr = numpy.zeros([1, img_size_y, img_size_x], dtype=int)
        vld_msk_arr[base_img_data != base_img_no_data] = vld_msk_val
        vld_msk_arr[chng_img_data != chng_img_no_data] = vld_msk_val

    # Flatten / Reshape  data.
    base_flat_data = base_img_data.reshape([n_bands, n_pxls])
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = chng_img_data.reshape([n_bands, n_pxls])
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)
    vld_flat_data = vld_msk_arr.reshape([n_pxls])

    # Create Pixel IDs
    pxl_ids = numpy.arange(0, n_pxls, 1)
    pxl_ids = pxl_ids[vld_flat_data == vld_msk_val]

    # Mask the image data to the valid pixels
    base_flat_data = base_flat_data[vld_flat_data == vld_msk_val]
    chng_flat_data = chng_flat_data[vld_flat_data == vld_msk_val]

    # Reshape the arrays so bands are first dim
    base_flat_data = numpy.moveaxis(base_flat_data, 0, -1)
    chng_flat_data = numpy.moveaxis(chng_flat_data, 0, -1)

    # The number of vaild pixels after masking
    n_vld_msks = base_flat_data.shape[1]

    # Check there is a useful number of pixels
    if n_vld_msks < 50:
        raise rsgislib.RSGISPyException("There are less then 50 valid pixels")

    # If specified standardise the image bands.
    if apply_std:
        base_flat_data = rsgislib.tools.stats.standarise_img_data(base_flat_data)
        chng_flat_data = rsgislib.tools.stats.standarise_img_data(chng_flat_data)

    img_X = base_flat_data
    img_Y = chng_flat_data

    weight = numpy.ones((1, n_vld_msks))  # (1, height * width)
    can_corr = 100 * numpy.ones((n_bands, 1))
    for _iter in tqdm.tqdm(range(mad_max_iter)):
        mean_X = numpy.sum(weight * img_X, axis=1, keepdims=True) / numpy.sum(weight)
        mean_Y = numpy.sum(weight * img_Y, axis=1, keepdims=True) / numpy.sum(weight)

        # Centralization
        center_X = img_X - mean_X
        center_Y = img_Y - mean_Y

        # Could also can use numpy.cov, but the result would be sightly different
        cov_XY = _covw(center_X, center_Y, weight)
        size = cov_XY.shape[0]
        sigma_11 = cov_XY[0:n_bands, 0:n_bands]  # + 1e-4 * numpy.identity(3)
        sigma_22 = cov_XY[n_bands:size, n_bands:size]  # + 1e-4 * numpy.identity(3)
        sigma_12 = cov_XY[0:n_bands, n_bands:size]  # + 1e-4 * numpy.identity(3)
        sigma_21 = sigma_12.T

        target_mat = numpy.dot(
            numpy.dot(
                numpy.dot(numpy.linalg.inv(sigma_11), sigma_12),
                numpy.linalg.inv(sigma_22),
            ),
            sigma_21,
        )
        numpy.nan_to_num(target_mat, copy=False, nan=0.0, posinf=0.0, neginf=0.0)
        eigenvalue, eigenvector_X = numpy.linalg.eig(
            target_mat
        )  # the eigenvalue and eigenvector of image X
        # sort eigenvector based on the size of eigenvalue
        eigenvalue = numpy.sqrt(eigenvalue)

        idx = eigenvalue.argsort()
        eigenvalue = eigenvalue[idx]

        # if (_iter + 1) == 1:
        #    print('Canonical correlations')
        # print(eigenvalue)

        eigenvector_X = eigenvector_X[:, idx]
        eigenvector_Y = numpy.dot(
            numpy.dot(numpy.linalg.inv(sigma_22), sigma_21), eigenvector_X
        )  # the eigenvector of image Y

        # tune the size of X and Y, so the constraint condition can be satisfied
        norm_X = numpy.sqrt(
            1
            / numpy.diag(numpy.dot(eigenvector_X.T, numpy.dot(sigma_11, eigenvector_X)))
        )
        norm_Y = numpy.sqrt(
            1
            / numpy.diag(numpy.dot(eigenvector_Y.T, numpy.dot(sigma_22, eigenvector_Y)))
        )
        eigenvector_X = norm_X * eigenvector_X
        eigenvector_Y = norm_Y * eigenvector_Y

        mad_variates = numpy.dot(eigenvector_X.T, center_X) - numpy.dot(
            eigenvector_Y.T, center_Y
        )  # (6, width * height)

        if numpy.max(numpy.abs(can_corr - eigenvalue)) < mad_epsilon:
            break
        can_corr = eigenvalue
        # calculate chi-square distance and probility of unchanged
        mad_var = numpy.reshape(2 * (1 - can_corr), (n_bands, 1))
        chi_square_dis = numpy.sum(
            mad_variates * mad_variates / mad_var, axis=0, dtype=float, keepdims=True
        )
        weight = 1 - scipy.stats.chi2.cdf(chi_square_dis, n_bands)

    if (_iter + 1) == mad_max_iter:
        print("Warning: The canonical correlation may not have converged")
    else:
        print(f"The canonical correlation has converged on iteration {_iter + 1}")

    # return mad_variates, can_corr, mad_var, eigenvector_X, eigenvector_Y, sigma_11, /
    # sigma_22, sigma_12, chi_square_dis, weight
    # mad, can_coo, mad_var, ev_1, ev_2, sigma_11, sigma_22, sigma_12, chi2, noc_weight

    sqrt_chi2 = numpy.sqrt(chi_square_dis)
    numpy.nan_to_num(sqrt_chi2, copy=False, nan=0.0, posinf=0.0, neginf=0.0)
    sqrt_chi2_flat_arr = numpy.zeros([n_pxls], dtype=float)
    sqrt_chi2_flat_arr[pxl_ids] = sqrt_chi2
    sqrt_chi2_img = sqrt_chi2_flat_arr.reshape([1, img_size_y, img_size_x])

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=sqrt_chi2_img,
        output_img=out_chng_dist_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )

    mad_variates = numpy.moveaxis(mad_variates, 0, -1)
    mad_variates_arr = numpy.zeros([n_pxls, n_bands], dtype=float)
    mad_variates_arr[pxl_ids] = mad_variates
    mad_variates_arr = numpy.moveaxis(mad_variates_arr, 0, -1)
    mad_variates_img = mad_variates_arr.reshape([n_bands, img_size_y, img_size_x])

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=mad_variates_img,
        output_img=out_chng_mads_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )

    if use_kmeans:
        print("Applying KMeans clustering...")
        chng_bin_data = _get_binary_change_map(sqrt_chi2)
        chng_bin_data_flat_arr = numpy.zeros([n_pxls], dtype=int)
        chng_bin_data_flat_arr[pxl_ids] = chng_bin_data
        chng_bin_img_arr = chng_bin_data_flat_arr.reshape([1, img_size_y, img_size_x])
    else:
        otsu_thres = rsgislib.tools.stats.calc_otsu_threshold(sqrt_chi2)
        chng_bin_img_arr = numpy.zeros((1, img_size_y, img_size_x))
        chng_bin_img_arr[sqrt_chi2_img > otsu_thres] = 1

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=chng_bin_img_arr,
        output_img=out_chng_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_8UINT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=255,
    )
