from typing import List
import numpy
import tqdm

import rsgislib
import rsgislib.imageutils
import rsgislib.tools.stats


def change_vector_analysis(
    in_base_img: str,
    in_chng_img: str,
    out_chng_img: str,
    out_chng_dist_img: str,
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
):
    """
    A function which performs Change vector analysis (CVA) between two images.
    The two images must have exactly the extent, resolution and image size
    (width and height).

    :param in_base_img: The input base image file path
    :param in_chng_img: The input change image file path
    :param out_chng_img: Output binary change image file path
    :param out_chng_dist_img: Output change distance image file path
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image

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

    img_size_x, img_size_y = rsgislib.imageutils.get_img_size(in_base_img)

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )

    if apply_std:
        base_img_data = rsgislib.tools.stats.standarise_img_data(base_img_data)
        chng_img_data = rsgislib.tools.stats.standarise_img_data(chng_img_data)

    img_diff = base_img_data - chng_img_data
    l2_norm = numpy.sqrt(numpy.sum(numpy.square(img_diff), axis=0))

    otsu_thres = rsgislib.tools.stats.calc_otsu_threshold(l2_norm.reshape(1, -1))

    chng_arr = l2_norm - otsu_thres
    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=chng_arr,
        output_img=out_chng_dist_img,
        ref_img=in_base_img,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
        no_data_val=None,
    )

    chng_arr = numpy.zeros((img_size_y, img_size_x))
    chng_arr[l2_norm > otsu_thres] = 1

    rsgislib.imageutils.create_img_from_array_ref_img(
        data_arr=chng_arr,
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
    gdalformat: str = "KEA",
    apply_std: bool = False,
    img_base_bands: List[int] = None,
    img_chng_bands: List[int] = None,
    sfa_max_iter: int = 50,
    sfa_epsilon: float = 1e-6,
    sfa_norm_trans: bool = False,
    sfa_regular: bool = False,
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
    :param gdalformat: Output GDAL image file format
    :param apply_std: Optionally apply standardisation to the input images.
    :param img_base_bands: Optionally specified a list of bands for the base image
    :param img_chng_bands: Optionally specified a list of bands for the change image
    :param sfa_max_iter: Maximum number of iterations for the algorithm to converge.
    :param sfa_epsilon: Threshold for convergence.
    :param sfa_norm_trans: Specifies whether to normalise the transformation matrix
    :param sfa_regular: Specifies whether to regularise the transformation matrix

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

    base_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_base_img, img_bands=img_base_bands
    )
    chng_img_data = rsgislib.imageutils.get_img_data_as_arr(
        in_chng_img, img_bands=img_chng_bands
    )

    if apply_std:
        base_img_data = rsgislib.tools.stats.standarise_img_data(base_img_data)
        chng_img_data = rsgislib.tools.stats.standarise_img_data(chng_img_data)

    # Initialise Omega
    arr_L = numpy.zeros((n_bands - 2, n_bands))  # (C-2, C)
    for i in range(n_bands - 2):
        arr_L[i, i] = 1
        arr_L[i, i + 1] = -2
        arr_L[i, i + 2] = 1
    arr_omega = numpy.dot(arr_L.T, arr_L)  # (C, C)


    P = img_size_y * img_size_x
    # row-major order after reshape
    img_X = numpy.reshape(base_img_data,
                       (-1, img_size_y * img_size_x))  # (band, width * height)
    img_Y = numpy.reshape(chng_img_data,
                       (-1, img_size_y * img_size_x))  # (band, width * height)
    lamb = 100 * numpy.ones((n_bands, 1))
    all_lambda = []
    weight = numpy.ones((img_size_x, img_size_y))

    weight = numpy.reshape(weight, (-1, img_size_x * img_size_y))
    for _iter in tqdm.tqdm(range(sfa_max_iter)):
        sum_w = numpy.sum(weight)
        mean_X = numpy.sum(weight * img_X, axis=1, keepdims=True) / numpy.sum(weight)  # (band, 1)
        mean_Y = numpy.sum(weight * img_Y, axis=1, keepdims=True) / numpy.sum(weight)  # (band, 1)
        center_X = (img_X - mean_X)
        center_Y = (img_Y - mean_Y)

        # cov_XY = covw(center_X, center_Y, weight)  # (2 * band, 2 * band)
        # cov_X = cov_XY[0:n_bands, 0:n_bands]
        # cov_Y = cov_XY[n_bands:2 * n_bands, n_bands:2 * n_bands]
        var_X = numpy.sum(weight * numpy.power(center_X, 2), axis=1, keepdims=True) / ((P - 1) * sum_w / P)
        var_Y = numpy.sum(weight * numpy.power(center_Y, 2), axis=1, keepdims=True) / ((P - 1) * sum_w / P)
        std_X = numpy.reshape(numpy.sqrt(var_X), (n_bands, 1))
        std_Y = numpy.reshape(numpy.sqrt(var_Y), (n_bands, 1))

        # normalize image
        norm_X = center_X / std_X
        norm_Y = center_Y / std_Y
        diff_img = (norm_X - norm_Y)
        mat_A = numpy.dot(weight * diff_img, diff_img.T) / ((P - 1) * sum_w / P)
        mat_B = (numpy.dot(weight * norm_X, norm_X.T) +
                 numpy.dot(weight * norm_Y, norm_Y.T)) / (2 * (P - 1) * sum_w / P)
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
        #if (_iter + 1) == 1:
        #    print('sqrt lambda:')
        #print(numpy.sqrt(eigenvalue))

        eigenvalue = numpy.reshape(eigenvalue, (n_bands, 1))  # (band, 1)
        threshold = numpy.max(numpy.abs(numpy.sqrt(lamb) - numpy.sqrt(eigenvalue)))
        # if sqrt(lambda) converge
        if threshold < sfa_epsilon:
            break
        lamb = eigenvalue
        all_lambda = lamb if (_iter + 1) == 1 else numpy.concatenate((all_lambda, lamb), axis=1)

        # the order of the slowest features is determined by the order of the eigenvalues
        trans_mat = eigenvector[:, idx]
        # satisfy the constraints(3)
        if sfa_norm_trans:
            output_signal_std = 1 / numpy.sqrt(
                numpy.diag(numpy.dot(trans_mat.T, numpy.dot(mat_B, trans_mat))))
            trans_mat = output_signal_std * trans_mat
        isfa_variable = numpy.dot(trans_mat.T, norm_X) - numpy.dot(trans_mat.T, norm_Y)

        if (_iter + 1) == 1:
            T = numpy.sum(numpy.square(isfa_variable) / numpy.sqrt(lamb), axis=0,
                       keepdims=True)  # chi square
        else:
            T = numpy.sum(numpy.square(isfa_variable) / numpy.sqrt(lamb), axis=0,
                       keepdims=True)  # IWD
        weight = 1 - scipy.stats.chi2.cdf(T, n_bands)

    if (_iter + 1) == sfa_max_iter:
        print('Warning: The lambda may not have converged')
    else:
        print(f'Lambda has converged on iteration {_iter + 1}')
    # return isfa_variable, lamb, all_lambda, trans_mat, T, weight

    sqrt_chi2 = numpy.sqrt(T)
    sqrt_chi2_img = sqrt_chi2.reshape((1, img_size_y, img_size_x))

    otsu_thres = rsgislib.tools.stats.calc_otsu_threshold(sqrt_chi2)

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

    chng_arr = numpy.zeros((img_size_y, img_size_x))
    chng_arr[sqrt_chi2_img[0] > otsu_thres] = 1

    rsgislib.imageutils.create_img_from_array_ref_img(
            data_arr=chng_arr,
            output_img=out_chng_img,
            ref_img=in_base_img,
            gdalformat=gdalformat,
            datatype=rsgislib.TYPE_8UINT,
            options=rsgislib.imageutils.get_rios_img_creation_opts(gdalformat),
            no_data_val=255,
    )
