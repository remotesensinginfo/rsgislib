RSGISLib Image Utilities Module
=================================

.. automodule:: rsgislib.imageutils
   :undoc-members:

.. autoclass:: ImageBandInfo
    :members:
    
.. autoclass:: RSGISTimeseriesFillInfo
    :members:
    
.. autoclass:: SharpBandInfo
    :members:


Image Stats and Pyramids
------------------------
.. autofunction:: rsgislib.imageutils.popImageStats
.. autofunction:: rsgislib.imageutils.combineImageOverviews


Projection
-----------

.. autofunction:: rsgislib.imageutils.assignProj
.. autofunction:: rsgislib.imageutils.assignSpatialInfo
.. autofunction:: rsgislib.imageutils.copyProjFromImage
.. autofunction:: rsgislib.imageutils.copySpatialAndProjFromImage
.. autofunction:: rsgislib.imageutils.resampleImage2Match
.. autofunction:: rsgislib.imageutils.reprojectImage
.. autofunction:: rsgislib.imageutils.getWKTProjFromImage


Mosaic 
-------

.. autofunction:: rsgislib.imageutils.createImageMosaic
.. autofunction:: rsgislib.imageutils.includeImages
.. autofunction:: rsgislib.imageutils.includeImagesWithOverlap
.. autofunction:: rsgislib.imageutils.includeImagesIndImgIntersect
.. autofunction:: rsgislib.imageutils.combineImages2Band


Composite
----------
.. autofunction:: rsgislib.imageutils.imagecomp.createMaxNDVIComposite
.. autofunction:: rsgislib.imageutils.imagecomp.createMaxNDVINDWIComposite
.. autofunction:: rsgislib.imageutils.imagecomp.createMaxNDVINDWICompositeLandsat
.. autofunction:: rsgislib.imageutils.imagecomp.checkBuildLS8VRTs
.. autofunction:: rsgislib.imageutils.genTimeseriesFillCompositeImg
.. autofunction:: rsgislib.imageutils.createRefImgCompositeImg
.. autofunction:: rsgislib.imageutils.combineBinaryMasks
.. autofunction:: rsgislib.imageutils.exportSingleMergedImgBand


Tile
-------
.. autofunction:: rsgislib.imageutils.createTiles
.. autofunction:: rsgislib.imageutils.createTilesMultiCore
.. autofunction:: rsgislib.imageutils.tilingutils.createMinDataTiles
.. autofunction:: rsgislib.imageutils.tilingutils.createTileMaskImagesFromShp
.. autofunction:: rsgislib.imageutils.tilingutils.createTileMaskImagesFromClumps
.. autofunction:: rsgislib.imageutils.tilingutils.createTilesFromMasks

Visualisation / Normalisation
------------------------------

.. autofunction:: rsgislib.imageutils.stretchImage
.. autofunction:: rsgislib.imageutils.stretchImageWithStats
.. autofunction:: rsgislib.imageutils.normaliseImagePxlVals

Subset / Mask
--------------

.. autofunction:: rsgislib.imageutils.maskImage
.. autofunction:: rsgislib.imageutils.subset
.. autofunction:: rsgislib.imageutils.subset2img
.. autofunction:: rsgislib.imageutils.subset2polys
.. autofunction:: rsgislib.imageutils.subsetImgs2CommonExtent
.. autofunction:: rsgislib.imageutils.subsetbbox
.. autofunction:: rsgislib.imageutils.subsetPxlBBox
.. autofunction:: rsgislib.imageutils.buildImgSubDict
.. autofunction:: rsgislib.imageutils.genFiniteMask
.. autofunction:: rsgislib.imageutils.genValidMask

Extract
--------

.. autofunction:: rsgislib.imageutils.ImageBandInfo
.. autofunction:: rsgislib.imageutils.extractZoneImageValues2HDF
.. autofunction:: rsgislib.imageutils.extractZoneImageBandValues2HDF
.. autofunction:: rsgislib.imageutils.mergeExtractedHDF5Data
.. autofunction:: rsgislib.imageutils.randomSampleHDF5File
.. autofunction:: rsgislib.imageutils.performRandomPxlSampleInMask
.. autofunction:: rsgislib.imageutils.performRandomPxlSampleInMaskLowPxlCount
.. autofunction:: rsgislib.imageutils.extractImgPxlSample
.. autofunction:: rsgislib.imageutils.extractImgPxlValsInMsk
.. autofunction:: rsgislib.imageutils.splitSampleHDF5File


Create
---------

.. autofunction:: rsgislib.imageutils.createBlankImage
.. autofunction:: rsgislib.imageutils.createBlankImgFromBBOX
.. autofunction:: rsgislib.imageutils.createBlankImgFromRefVector
.. autofunction:: rsgislib.imageutils.createCopyImage
.. autofunction:: rsgislib.imageutils.createCopyImageDefExtent
.. autofunction:: rsgislib.imageutils.createCopyImageVecExtent
.. autofunction:: rsgislib.imageutils.createCopyImageVecExtentSnap2Grid
.. autofunction:: rsgislib.imageutils.createImageForEachVecFeat


Select / Stack bands
---------------------

.. autofunction:: rsgislib.imageutils.selectImageBands
.. autofunction:: rsgislib.imageutils.stackImageBands

Sharpen Image Bands
-------------------

.. autofunction:: rsgislib.imageutils.panSharpenHCS
.. autofunction:: rsgislib.imageutils.SharpBandInfo
.. autofunction:: rsgislib.imageutils.sharpenLowResBands


Band Names
----------

.. autofunction:: rsgislib.imageutils.setBandNames
.. autofunction:: rsgislib.imageutils.getBandNames


Image Data Types
-----------------

.. autofunction:: rsgislib.imageutils.getGDALDataType
.. autofunction:: rsgislib.imageutils.getRSGISLibDataType


Other
------

.. autofunction:: rsgislib.imageutils.stackStats
.. autofunction:: rsgislib.imageutils.orderImageUsingValidPxls
.. autofunction:: rsgislib.imageutils.genSamplingGrid
.. autofunction:: rsgislib.imageutils.calcPixelLocations
.. autofunction:: rsgislib.imageutils.hasGCPs
.. autofunction:: rsgislib.imageutils.copyGCPs
.. autofunction:: rsgislib.imageutils.setImgThematic
.. autofunction:: rsgislib.imageutils.doImagesOverlap
.. autofunction:: rsgislib.imageutils.generateRandomPxlValsImg
.. autofunction:: rsgislib.imageutils.getUniqueValues
.. autofunction:: rsgislib.imageutils.getGDALImageCreationOpts



* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

