
import rsgislib
import rsgislib.imageutils
import rsgislib.imagecalc
import rsgislib.rastergis

from osgeo import gdal

import numpy

import skimage.segmentation

import os.path


def performFelsenszwalbSegmentation(inputImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100):
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
    if gdalDS.RasterCount > 1:
        for nBand in numpy.arange(gdalDS.RasterCount):
            gdalBand = gdalDS.GetRasterBand(int(nBand+1))
            pxlValsArr.append(gdalBand.ReadAsArray())
        pxlVals = numpy.stack(pxlValsArr, axis=-1)
    else:
        gdalBand = gdalDS.GetRasterBand(1)
        pxlVals = gdalBand.ReadAsArray()
    
    segResult = skimage.segmentation.felzenszwalb(pxlVals)
    print(segResult.shape)
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)
        
        
def performQuickshiftSegmentation(inputImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, pcaPxlSample=100):
    """
A function to perform the quickshift segmentation algorithm from the
scikit-image library (http://scikit-image.org/docs/stable/api/skimage.segmentation.html).

* inputImg - input image file.
* outputImg - output image file.
* gdalformat - output image file format.
* tmpDIR - temp DIR used to output PCA files
* calcStats - calculate image pixel statistics, histogram and image pyramids - note if you are not using a KEA file then the format needs to support RATs for this option as histogram and colour table are written to RAT.
* usePCA - if there are not 3 image bands in the input file then you can use PCA to reduce the number of image bands.
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
    
    segResult = skimage.segmentation.quickshift(pxlVals)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)

def performSlicSegmentation(inputImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100):
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
    
    segResult = skimage.segmentation.slic(pxlVals, multichannel=multichannel)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def performWatershedSegmentation(inputImg, markersImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100):
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
    
    segResult = skimage.segmentation.watershed(pxlVals, markerArr)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)


def performRandomWalkerSegmentation(inputImg, markersImg, outputImg, gdalformat='KEA', noDataVal=0, tmpDIR='./tmp', calcStats=True, usePCA=False, nPCABands=3, pcaPxlSample=100):
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
    
    segResult = skimage.segmentation.random_walker(pxlVals, markerArr)
    segResult = segResult + 1
    
    gdalOutDS = gdal.Open(outputImg, gdal.GA_Update)
    gdalOutBand = gdalOutDS.GetRasterBand(1)
    gdalOutBand.WriteArray(segResult)
    gdalOutDS = None
    gdalDS = None
    
    if calcStats:
        rsgislib.rastergis.populateStats(clumps=outputImg, addclrtab=True, calcpyramids=True, ignorezero=True)

