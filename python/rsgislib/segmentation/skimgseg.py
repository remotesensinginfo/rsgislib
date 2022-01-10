#!/usr/bin/env python

import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis

from osgeo import gdal

import numpy

HAVE_SKI_SEG = True
try:
    import skimage.segmentation
except ImportError:
    HAVE_SKI_SEG = False

import os


def perform_felsenszwalb_segmentation(
    input_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    tmp_dir="./tmp",
    calc_stats=True,
    use_pca=False,
    n_pca_bands=3,
    pca_pxl_sample=100,
    scale=1,
    sigma=0.8,
    min_size=20,
):
    """
    A function to perform the Felsenszwalb segmentation algorithm from the
    scikit-image library
    (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

    :param input_img: input image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param tmp_dir: temp DIR used to output PCA files
    :param calc_stats: calculate image pixel statistics, histogram and image
                       pyramids - note if you are not using a KEA file then the
                       format needs to support RATs for this option as histogram
                       and colour table are written to RAT.
    :param use_pca: if there are not 1 or 3 image bands in the input file then
                    you can use PCA to reduce the number of image bands.
    :param n_pca_bands: the number of principle components outputs from the
                        PCA - needs to be either 1 or 3.
    :param scale: scikit-image Felsenszwalb parameter: 'Free parameter.
                  Higher means larger clusters.'
    :param sigma: scikit-image Felsenszwalb parameter: 'Width of Gaussian
                  kernel used in preprocessing.'
    :param min_size: scikit-image Felsenszwalb parameter: 'Minimum component
                     size. Enforced using postprocessing.'

    """
    if not HAVE_SKI_SEG:
        raise rsgislib.RSGISPyException(
            "Have not been able to import skimage.segmentation check it is installed."
        )
    # Create output image
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_32UINT
    )

    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    pcaImg = ""
    if use_pca and (n_pca_bands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            1,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca and (n_pca_bands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            3,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca:
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "In the number of bands given the number of principle components."
        )

    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "Input image should have either a single band or three (RGB). Consider performing PCA to reduce."
        )

    pxlValsArr = []
    multichannel = True
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand + 1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
        multichannel = True
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
        multichannel = False

    segResult = skimage.segmentation.felzenszwalb(
        pxlVals, scale=scale, sigma=sigma, min_size=min_size, multichannel=multichannel
    )
    segResult = segResult + 1

    gdalOutDS = gdal.Open(output_img, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )


def perform_quickshift_segmentation(
    input_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    tmp_dir="./tmp",
    calc_stats=True,
    use_pca=False,
    pca_pxl_sample=100,
    ratio=1.0,
    kernel_size=5,
    max_dist=10,
    sigma=0,
    convert_to_lab=True,
    random_seed=42,
):
    """
    A function to perform the quickshift segmentation algorithm from the
    scikit-image library
    (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

    :param input_img: input image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param tmp_dir: temp DIR used to output PCA files
    :param calc_stats: calculate image pixel statistics, histogram and image
                       pyramids - note if you are not using a KEA file then the
                       format needs to support RATs for this option as histogram
                       and colour table are written to RAT.
    :param use_pca: if there are not 3 image bands in the input file then you can
                    use PCA to reduce the number of image bands.
    :param ratio: scikit-image Quickshift parameter: 'Balances color-space proximity
                  and image-space proximity. Higher values give more weight to
                  color-space. (between 0 and 1)'
    :param kernel_size: scikit-image Quickshift parameter: 'Width of Gaussian kernel
                        used in smoothing the sample density. Higher means
                        fewer clusters.'
    :param max_dist: scikit-image Quickshift parameter: 'Cut-off point for
                     data distances. Higher means fewer clusters.'
    :param sigma: scikit-image Quickshift parameter: 'Width for Gaussian
                  smoothing as preprocessing. Zero means no smoothing.'
    :param convert_to_lab: scikit-image Quickshift parameter: 'Whether the input
                           should be converted to Lab colorspace prior to segmentation.
                           For this purpose, the input is assumed to be RGB.'
    :param random_seed: scikit-image Quickshift parameter: 'Random seed used
                        for breaking ties.'

    """
    if not HAVE_SKI_SEG:
        raise rsgislib.RSGISPyException(
            "Have not been able to import skimage.segmentation check it is installed."
        )
    # Create output image
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_32UINT
    )

    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    pcaImg = ""
    if use_pca and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            3,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca:
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "In the number of bands given the number of principle components."
        )

    if not (gdalDS.RasterCount == 3):
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "Input image should have three bands (RGB). Consider performing PCA to reduce."
        )

    pxlValsArr = []
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand + 1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()

    segResult = skimage.segmentation.quickshift(
        pxlVals,
        ratio=ratio,
        kernel_size=kernel_size,
        max_dist=max_dist,
        return_tree=False,
        sigma=sigma,
        convert2lab=convert_to_lab,
        random_seed=random_seed,
    )
    segResult = segResult + 1

    gdalOutDS = gdal.Open(output_img, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )


def perform_slic_segmentation(
    input_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    tmp_dir="./tmp",
    calc_stats=True,
    use_pca=False,
    n_pca_bands=3,
    pca_pxl_sample=100,
    n_segments=100,
    compactness=10.0,
    max_iter=10,
    sigma=0,
    spacing=None,
    convert_to_lab=None,
    enforce_connectivity=True,
    min_size_factor=0.5,
    max_size_factor=3,
    slic_zero=False,
):
    """
    A function to perform the slic segmentation algorithm from the
    scikit-image library
    (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

    :param input_img: input image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param tmp_dir: temp DIR used to output PCA files
    :param calc_stats: calculate image pixel statistics, histogram and image
                       pyramids - note if you are not using a KEA file then the
                       format needs to support RATs for this option as histogram
                       and colour table are written to RAT.
    :param use_pca: if there are not 1 or 3 image bands in the input file then
                    you can use PCA to reduce the number of image bands.
    :param n_pca_bands: the number of principle components outputs from the
                        PCA - needs to be either 1 or 3.
    :param n_segments: scikit-image Slic parameter: 'The (approximate) number
                       of labels in the segmented output image.'
    :param compactness: scikit-image Slic parameter: 'Balances color proximity
                        and space proximity. Higher values give more weight to space
                        proximity, making superpixel shapes more square/cubic. In
                        SLICO mode, this is the initial compactness. This parameter
                        depends strongly on image contrast and on the shapes of
                        objects in the image. We recommend exploring possible values
                        on a log scale, e.g., 0.01, 0.1, 1, 10, 100, before refining
                        around a chosen value.'
    :param max_iter: scikit-image Slic parameter: 'Maximum number of iterations
                     of k-means.'
    :param sigma: scikit-image Slic parameter: 'Width of Gaussian smoothing kernel
                  for pre-processing for each dimension of the image. The same sigma
                  is applied to each dimension in case of a scalar value. Zero means
                  no smoothing. Note, that sigma is automatically scaled if it is
                  scalar and a manual voxel spacing is provided (see Notes section).'
    :param spacing: scikit-image Slic parameter: 'The voxel spacing along each image
                    dimension. By default, slic assumes uniform spacing (same voxel
                    resolution along z, y and x). This parameter controls the weights
                    of the distances along z, y, and x during k-means clustering.'
    :param convert_to_lab: scikit-image Slic parameter: 'Whether the input should be
                           converted to Lab colorspace prior to segmentation. The
                           input image must be RGB. Highly recommended.'
    :param enforce_connectivity: scikit-image Slic parameter: 'Whether the generated
                                 segments are connected or not'
    :param min_size_factor: scikit-image Slic parameter: 'Proportion of the minimum
                            segment size to be removed with respect to the supposed
                            segment size "depth:paramwidth*height/n_segments"'
    :param max_size_factor: scikit-image Slic parameter: 'Proportion of the maximum
                            connected segment size. A value of 3 works in most of
                            the cases.'
    :param slic_zero: scikit-image Slic parameter: 'Run SLIC-zero, the zero-parameter
                      mode of SLIC.'

    """
    if not HAVE_SKI_SEG:
        raise rsgislib.RSGISPyException(
            "Have not been able to import skimage.segmentation check it is installed."
        )
    # Create output image
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_32UINT
    )

    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    pcaImg = ""
    if use_pca and (n_pca_bands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            1,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca and (n_pca_bands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            3,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca:
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "In the number of bands given the number of principle components."
        )

    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "Input image should have either a single band or three (RGB). Consider performing PCA to reduce."
        )

    pxlValsArr = []
    multichannel = False
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand + 1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
        multichannel = True
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
        multichannel = False

    segResult = skimage.segmentation.slic(
        pxlVals,
        n_segments=n_segments,
        compactness=compactness,
        max_iter=max_iter,
        sigma=sigma,
        spacing=spacing,
        multichannel=multichannel,
        convert2lab=convert_to_lab,
        enforce_connectivity=enforce_connectivity,
        min_size_factor=min_size_factor,
        max_size_factor=max_size_factor,
        slic_zero=slic_zero,
    )
    segResult = segResult + 1

    gdalOutDS = gdal.Open(output_img, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )


def perform_watershed_segmentation(
    input_img,
    in_markers_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    tmp_dir="./tmp",
    calc_stats=True,
    use_pca=False,
    n_pca_bands=3,
    pca_pxl_sample=100,
    compactness=0,
    watershed_line=False,
):
    """
    A function to perform the watershed segmentation algorithm from the
    scikit-image library
    (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

    :param input_img: input image file.
    :param in_markers_img: input markers image file.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param tmp_dir: temp DIR used to output PCA files
    :param calc_stats: calculate image pixel statistics, histogram and image
                       pyramids - note if you are not using a KEA file then the
                       format needs to support RATs for this option as histogram
                       and colour table are written to RAT.
    :param use_pca: if there are not 1 or 3 image bands in the input file then you
                    can use PCA to reduce the number of image bands.
    :param n_pca_bands: the number of principle components outputs from the
                        PCA - needs to be either 1 or 3.
    :param compactness: scikit-image Watershed parameter: 'Use compact watershed
                        with given compactness parameter. Higher values result in
                        more regularly-shaped watershed basins; Peer Neubert & Peter
                        Protzel (2014). Compact Watershed and Preemptive SLIC: On
                        Improving Trade-offs of Superpixel Segmentation
                        Algorithms. ICPR 2014'
    :param watershed_line: scikit-image Watershed parameter: 'If watershed_line is
                           True, a one-pixel wide line separates the regions obtained
                           by the watershed algorithm. The line has the label 0.'

    """
    if not HAVE_SKI_SEG:
        raise rsgislib.RSGISPyException(
            "Have not been able to import skimage.segmentation check it is installed."
        )

    # Create output image
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_32UINT
    )

    gdalMarkersDS = gdal.Open(in_markers_img, gdal.GA_ReadOnly)
    if gdalMarkersDS.RasterCount != 1:
        gdalMarkersDS = None
        raise rsgislib.RSGISPyException(
            "Markers image should only have one image band."
        )

    gdalMarkerBand = gdalMarkersDS.GetRasterBand(1)
    markerArr = gdalMarkerBand.ReadAsArray()

    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    pcaImg = ""
    if use_pca and (n_pca_bands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            1,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca and (n_pca_bands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            3,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca:
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "In the number of bands given the number of principle components."
        )

    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "Input image should have either a single band or three (RGB). Consider performing PCA to reduce."
        )

    pxlValsArr = []
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand + 1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()

    segResult = skimage.segmentation.watershed(
        pxlVals, markerArr, compactness=compactness, watershed_line=watershed_line
    )
    if not watershed_line:
        segResult = segResult + 1

    gdalOutDS = gdal.Open(output_img, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )


def perform_random_walker_segmentation(
    input_img,
    in_markers_img,
    output_img,
    gdalformat="KEA",
    no_data_val=0,
    tmp_dir="./tmp",
    calc_stats=True,
    use_pca=False,
    n_pca_bands=3,
    pca_pxl_sample=100,
    beta=130,
    mode="bf",
    tol=0.001,
    spacing=None,
):
    """
    A function to perform the random walker segmentation algorithm from the
    scikit-image library
    (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

    :param input_img: input image file.
    :param in_markers_img: input markers image file - markers must be uniquely numbered.
    :param output_img: output image file.
    :param gdalformat: output image file format.
    :param tmp_dir: temp DIR used to output PCA files
    :param calc_stats: calculate image pixel statistics, histogram and image
                       pyramids - note if you are not using a KEA file then the format
                       needs to support RATs for this option as histogram and colour
                       table are written to RAT.
    :param use_pca: if there are not 1 or 3 image bands in the input file then you
                    can use PCA to reduce the number of image bands.
    :param n_pca_bands: the number of principle components outputs from the
                        PCA - needs to be either 1 or 3.
    :param beta: scikit-image random_walker parameter: 'Penalization coefficient for
                 the random walker motion (the greater beta, the more difficult
                 the diffusion).'
    :param mode: scikit-image random_walker parameter: 'Mode for solving the linear
                 system in the random walker algorithm. Available options
                 {'cg_mg', 'cg', 'bf'}.'
            * 'bf' (brute force): an LU factorization of the Laplacian is computed.
               This is fast for small images (<1024x1024), but very slow and
               memory-intensive for large images (e.g., 3-D volumes).
            * 'cg' (conjugate gradient): the linear system is solved iteratively
               using the Conjugate Gradient method from scipy.sparse.linalg. This is
               less memory-consuming than the brute force method for large images,
               but it is quite slow.
            * 'cg_mg' (conjugate gradient with multigrid preconditioner): a
               preconditioner is computed using a multigrid solver, then the
               solution is computed with the Conjugate Gradient method. This mode
               requires that the pyamg module (http://pyamg.org/) is installed. For
               images of size > 512x512, this is the recommended (fastest) mode.
    :param tol: scikit-image random_walker parameter: 'tolerance to achieve when
                solving the linear system, in cg’ and ‘cg_mg’ modes.'
    :param spacing: scikit-image random_walker parameter: 'Spacing between voxels
                    in each spatial dimension. If None, then the spacing between
                    pixels/voxels in each dimension is assumed 1.'

    """
    if not HAVE_SKI_SEG:
        raise rsgislib.RSGISPyException(
            "Have not been able to import skimage.segmentation check it is installed."
        )

    # Create output image
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 1, 0, gdalformat, rsgislib.TYPE_32UINT
    )

    gdalMarkersDS = gdal.Open(in_markers_img, gdal.GA_ReadOnly)
    if gdalMarkersDS.RasterCount != 1:
        gdalMarkersDS = None
        raise rsgislib.RSGISPyException(
            "Markers image should only have one image band."
        )

    gdalMarkerBand = gdalMarkersDS.GetRasterBand(1)
    markerArr = gdalMarkerBand.ReadAsArray()

    gdalDS = gdal.Open(input_img, gdal.GA_ReadOnly)
    pcaImg = ""
    if use_pca and (n_pca_bands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            1,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca and (n_pca_bands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        inImgBaseName = os.path.splitext(os.path.basename(input_img))[0]
        pcaImg = os.path.join(tmp_dir, inImgBaseName + "_pca.kea")
        pcaEigenValsFile = os.path.join(tmp_dir, inImgBaseName + "_eigen.mtxt")
        inDType = rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        rsgislib.imagecalc.perform_image_pca(
            input_img,
            pcaImg,
            pcaEigenValsFile,
            3,
            pca_pxl_sample,
            "KEA",
            inDType,
            no_data_val,
            False,
        )
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif use_pca:
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "In the number of bands given the number of principle components."
        )

    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise rsgislib.RSGISPyException(
            "Input image should have either a single band or three (RGB). Consider performing PCA to reduce."
        )

    multichannel = True
    pxlValsArr = []
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand + 1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
        multichannel = True
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
        multichannel = False

    segResult = skimage.segmentation.random_walker(
        pxlVals,
        markerArr,
        beta=beta,
        mode=mode,
        tol=tol,
        multichannel=multichannel,
        spacing=spacing,
    )
    segResult = segResult + 1

    gdalOutDS = gdal.Open(output_img, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None

    if calc_stats:
        rsgislib.rastergis.pop_rat_img_stats(
            clumps_img=output_img,
            add_clr_tab=True,
            calc_pyramids=True,
            ignore_zero=True,
        )
