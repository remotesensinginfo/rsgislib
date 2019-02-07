
import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis

from osgeo import gdal

import numpy

import skimage.segmentation

import os.path


def performFelsenszwalbSegmentation(inputImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100, scale=1, sigma=0.8, min_size=20):
    """
A function to perform the Felsenszwalb segmentation algorithm from the
scikit-image library (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* tmpDIR - temp DIR used to output PCA files
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* usePCA - if there are not 1 or 3 image bands in the input file then you can use PCA to reduce the number of image bands.
* nPCABands - the number of principle components outputs from the PCA - needs to be either 1 or 3.
* scale - scikit-image Felsenszwalb parameter: 'Free parameter. Higher means larger clusters.'
* sigma - scikit-image Felsenszwalb parameter: 'Width of Gaussian kernel used in preprocessing.'
* min_size - scikit-image Felsenszwalb parameter: 'Minimum component size. Enforced using postprocessing.'

"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
            
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    pcaImg = ''
    if usePCA and (nPCABands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 1, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA and (nPCABands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 3, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA:
        gdalDS = None
        raise Exception('In the number of bands given the number of principle components.')    
    
    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise Exception('Input image should have either a single band or three (RGB). Consider performing PCA to reduce.')
    
    pxlValsArr = []
    multichannel=True
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand+1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
        multichannel=True
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
        multichannel=False
    
    segResult = skimage.segmentation.felzenszwalb(pxlVals, scale=scale, sigma=sigma, min_size=min_size, multichannel=multichannel)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)
        
        
def performQuickshiftSegmentation(inputImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, pcaPxlSample=100, ratio=1.0, kernel_size=5, max_dist=10, sigma=0, convert2lab=True, random_seed=42):
    """
A function to perform the quickshift segmentation algorithm from the
scikit-image library (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* tmpDIR - temp DIR used to output PCA files
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* usePCA - if there are not 3 image bands in the input file then you can use PCA to reduce the number of image bands.
* ratio - scikit-image Quickshift parameter: 'Balances color-space proximity and image-space proximity. Higher values give more weight to color-space. (between 0 and 1)'
* kernel_size - scikit-image Quickshift parameter: 'Width of Gaussian kernel used in smoothing the sample density. Higher means fewer clusters.'
* max_dist - scikit-image Quickshift parameter: 'Cut-off point for data distances. Higher means fewer clusters.'
* sigma - scikit-image Quickshift parameter: 'Width for Gaussian smoothing as preprocessing. Zero means no smoothing.'
* convert2lab - scikit-image Quickshift parameter: 'Whether the input should be converted to Lab colorspace prior to segmentation. For this purpose, the input is assumed to be RGB.'
* random_seed - scikit-image Quickshift parameter: 'Random seed used for breaking ties.'
"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
            
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    pcaImg = ''
    if usePCA and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 3, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA:
        gdalDS = None
        raise Exception('In the number of bands given the number of principle components.')
    
    if not (gdalDS.RasterCount == 3):
        gdalDS = None
        raise Exception('Input image should have three bands (RGB). Consider performing PCA to reduce.')
    
    pxlValsArr = []
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand+1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
    
    segResult = skimage.segmentation.quickshift(pxlVals, ratio=ratio, kernel_size=kernel_size, max_dist=max_dist, return_tree=False, sigma=sigma, convert2lab=convert2lab, random_seed=random_seed)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)

def performSlicSegmentation(inputImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100, n_segments=100, compactness=10.0, max_iter=10, sigma=0, spacing=None, convert2lab=None, enforce_connectivity=True, min_size_factor=0.5, max_size_factor=3, slic_zero=False):
    """
A function to perform the slic segmentation algorithm from the
scikit-image library (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* tmpDIR - temp DIR used to output PCA files
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* usePCA - if there are not 1 or 3 image bands in the input file then you can use PCA to reduce the number of image bands.
* nPCABands - the number of principle components outputs from the PCA - needs to be either 1 or 3.
* n_segments - scikit-image Slic parameter: 'The (approximate) number of labels in the segmented output image.'
* compactness - scikit-image Slic parameter: 'Balances color proximity and space proximity. Higher values give more weight to space proximity, making superpixel shapes more square/cubic. In SLICO mode, this is the initial compactness. This parameter depends strongly on image contrast and on the shapes of objects in the image. We recommend exploring possible values on a log scale, e.g., 0.01, 0.1, 1, 10, 100, before refining around a chosen value.'
* max_iter - scikit-image Slic parameter: 'Maximum number of iterations of k-means.'
* sigma - scikit-image Slic parameter: 'Width of Gaussian smoothing kernel for pre-processing for each dimension of the image. The same sigma is applied to each dimension in case of a scalar value. Zero means no smoothing. Note, that sigma is automatically scaled if it is scalar and a manual voxel spacing is provided (see Notes section).'
* spacing - scikit-image Slic parameter: 'The voxel spacing along each image dimension. By default, slic assumes uniform spacing (same voxel resolution along z, y and x). This parameter controls the weights of the distances along z, y, and x during k-means clustering.'
* convert2lab - scikit-image Slic parameter: 'Whether the input should be converted to Lab colorspace prior to segmentation. The input image must be RGB. Highly recommended.'
* enforce_connectivity - scikit-image Slic parameter: 'Whether the generated segments are connected or not'
* min_size_factor - scikit-image Slic parameter: 'Proportion of the minimum segment size to be removed with respect to the supposed segment size "depth*width*height/n_segments"'
* max_size_factor - scikit-image Slic parameter: 'Proportion of the maximum connected segment size. A value of 3 works in most of the cases.'
* slic_zero - scikit-image Slic parameter: 'Run SLIC-zero, the zero-parameter mode of SLIC.'
"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
            
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    pcaImg = ''
    if usePCA and (nPCABands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 1, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA and (nPCABands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 3, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA:
        gdalDS = None
        raise Exception('In the number of bands given the number of principle components.')   
    
    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise Exception('Input image should have either a single band or three (RGB). Consider performing PCA to reduce.')
    
    pxlValsArr = []
    multichannel = False
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand+1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
        multichannel = True
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
        multichannel = False
    
    segResult = skimage.segmentation.slic(pxlVals, n_segments=n_segments, compactness=compactness, max_iter=max_iter, sigma=sigma, spacing=spacing, multichannel=multichannel, convert2lab=convert2lab, enforce_connectivity=enforce_connectivity, min_size_factor=min_size_factor, max_size_factor=max_size_factor, slic_zero=slic_zero)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def performWatershedSegmentation(inputImg, markersImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100, compactness=0, watershed_line=False):
    """
A function to perform the watershed segmentation algorithm from the
scikit-image library (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

* inputImg - input image file.
* markersImg - input markers image file.
* outputImg - output image file.
* gdalformat - output image file format.
* tmpDIR - temp DIR used to output PCA files
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* usePCA - if there are not 1 or 3 image bands in the input file then you can use PCA to reduce the number of image bands.
* nPCABands - the number of principle components outputs from the PCA - needs to be either 1 or 3.
* compactness - scikit-image Watershed parameter: 'Use compact watershed with given compactness parameter. Higher values result in more regularly-shaped watershed basins; Peer Neubert & Peter Protzel (2014). Compact Watershed and Preemptive SLIC: On Improving Trade-offs of Superpixel Segmentation Algorithms. ICPR 2014'
* watershed_line - scikit-image Watershed parameter: 'If watershed_line is True, a one-pixel wide line separates the regions obtained by the watershed algorithm. The line has the label 0.'
"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
            
    
    gdalMarkersDS = gdal.Open(markersImg, gdal.GA_ReadOnly)
    if gdalMarkersDS.RasterCount != 1:
        gdalMarkersDS = None
        raise Exception('Markers image should only have one image band.')
    
    gdalMarkerBand = gdalMarkersDS.GetRasterBand(1)
    markerArr = gdalMarkerBand.ReadAsArray()
    
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    pcaImg = ''
    if usePCA and (nPCABands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 1, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA and (nPCABands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 3, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA:
        gdalDS = None
        raise Exception('In the number of bands given the number of principle components.')    
    
    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise Exception('Input image should have either a single band or three (RGB). Consider performing PCA to reduce.')
    
    pxlValsArr = []
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand+1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
    
    segResult = skimage.segmentation.watershed(pxlVals, markerArr, compactness=compactness, watershed_line=watershed_line)
    if not watershed_line:
        segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def performRandomWalkerSegmentation(inputImg, markersImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100, beta=130, mode='bf', tol=0.001, spacing=None):
    """
A function to perform the random walker segmentation algorithm from the
scikit-image library (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

* inputImg - input image file.
* markersImg - input markers image file - markers must be uniquely numbered.
* outputImg - output image file.
* gdalformat - output image file format.
* tmpDIR - temp DIR used to output PCA files
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* usePCA - if there are not 1 or 3 image bands in the input file then you can use PCA to reduce the number of image bands.
* nPCABands - the number of principle components outputs from the PCA - needs to be either 1 or 3.
* beta - scikit-image random_walker parameter: 'Penalization coefficient for the random walker motion (the greater beta, the more difficult the diffusion).'
* mode - scikit-image random_walker parameter: 'Mode for solving the linear system in the random walker algorithm. Available options {'cg_mg', 'cg', 'bf'}.'
        ‘bf’ (brute force): an LU factorization of the Laplacian is computed. This is fast for small images (<1024x1024), but very slow and memory-intensive for large images (e.g., 3-D volumes).
        ‘cg’ (conjugate gradient): the linear system is solved iteratively using the Conjugate Gradient method from scipy.sparse.linalg. This is less memory-consuming than the brute force method for large images, but it is quite slow.
        ‘cg_mg’ (conjugate gradient with multigrid preconditioner): a preconditioner is computed using a multigrid solver, then the solution is computed with the Conjugate Gradient method. This mode requires that the pyamg module (http://pyamg.org/) is installed. For images of size > 512x512, this is the recommended (fastest) mode.
* tol - scikit-image random_walker parameter: 'tolerance to achieve when solving the linear system, in cg’ and ‘cg_mg’ modes.'
* spacing - scikit-image random_walker parameter: 'Spacing between voxels in each spatial dimension. If None, then the spacing between pixels/voxels in each dimension is assumed 1.'
"""
    # Create output image
    rsgislib.imageutils.createCopyImage(inputImg, outputImg, 1, 0, gdalformat, rsgislib.TYPE_32UINT)
    
    gdalMarkersDS = gdal.Open(markersImg, gdal.GA_ReadOnly)
    if gdalMarkersDS.RasterCount != 1:
        gdalMarkersDS = None
        raise Exception('Markers image should only have one image band.')
    
    gdalMarkerBand = gdalMarkersDS.GetRasterBand(1)
    markerArr = gdalMarkerBand.ReadAsArray()
    
    
    gdalDS = gdal.Open(inputImg, gdal.GA_ReadOnly)
    pcaImg = ''
    if usePCA and (nPCABands == 1) and (gdalDS.RasterCount > 1):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 1, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA and (nPCABands == 3) and (gdalDS.RasterCount > 3):
        # Perform PCA
        gdalDS = None
        rsgisUtils = rsgislib.RSGISPyUtils()
        inImgBaseName = os.path.splitext(os.path.basename(inputImg))[0]
        pcaImg = os.path.join(tmpDIR, inImgBaseName+'_pca.kea')
        pcaEigenValsFile = os.path.join(tmpDIR, inImgBaseName+'_eigen.mtxt')
        inDType = rsgisUtils.getRSGISLibDataTypeFromImg(inputImg)
        rsgislib.imagecalc.performImagePCA(inputImg, pcaImg, pcaEigenValsFile, 3, pcaPxlSample, 'KEA', inDType, noDataVal, False)
        gdalDS = gdal.Open(pcaImg, gdal.GA_ReadOnly)
    elif usePCA:
        gdalDS = None
        raise Exception('In the number of bands given the number of principle components.')  
    
    if not ((gdalDS.RasterCount == 1) or (gdalDS.RasterCount == 3)):
        gdalDS = None
        raise Exception('Input image should have either a single band or three (RGB). Consider performing PCA to reduce.')
    
    multichannel = True
    pxlValsArr = []
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand+1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
        multichannel = True
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
        multichannel = False
    
    segResult = skimage.segmentation.random_walker(pxlVals, markerArr, beta=beta, mode=mode, tol=tol, multichannel=multichannel, spacing=spacing)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)

