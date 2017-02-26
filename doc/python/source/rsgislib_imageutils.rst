RSGISLib Image Utilities Module
=================================

.. automodule:: rsgislib.imageutils
   :undoc-members:

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
----------

.. autofunction:: rsgislib.imageutils.createImageMosaic
.. autofunction:: rsgislib.imageutils.includeImages
.. autofunction:: rsgislib.imageutils.includeImagesWithOverlap
.. autofunction:: rsgislib.imageutils.combineImages2Band

Tile
-------
.. autofunction:: rsgislib.imageutils.createTiles
.. autofunction:: rsgislib.imageutils.tilingutils.createMinDataTiles
.. autofunction:: rsgislib.imageutils.tilingutils.createTileMaskImagesFromShp
.. autofunction:: rsgislib.imageutils.tilingutils.createTileMaskImagesFromClumps
.. autofunction:: rsgislib.imageutils.tilingutils.createTilesFromMasks

Visualisation / Normalisation
------------------------------

.. autofunction:: rsgislib.imageutils.stretchImage
.. autofunction:: rsgislib.imageutils.stretchImageWithStats


Subset / Mask
--------------

.. autofunction:: rsgislib.imageutils.maskImage
.. autofunction:: rsgislib.imageutils.subset
.. autofunction:: rsgislib.imageutils.subset2img
.. autofunction:: rsgislib.imageutils.subset2polys
.. autofunction:: rsgislib.imageutils.subsetImgs2CommonExtent
.. autofunction:: rsgislib.imageutils.subsetbbox
.. autofunction:: rsgislib.imageutils.buildImgSubDict
.. autofunction:: rsgislib.imageutils.genFiniteMask
.. autofunction:: rsgislib.imageutils.genValidMask

Extract
--------

.. autofunction:: rsgislib.imageutils.ImageBandInfo
.. autofunction:: rsgislib.imageutils.extractZoneImageValues2HDF
.. autofunction:: rsgislib.imageutils.extractZoneImageBandValues2HDF
.. autofunction:: rsgislib.imageutils.mergeExtractedHDF5Data
.. autofunction:: rsgislib.imageutils.performRandomPxlSampleInMask
.. autofunction:: rsgislib.imageutils.performRandomPxlSampleInMaskLowPxlCount


Create
---------

.. autofunction:: rsgislib.imageutils.createBlankImage
.. autofunction:: rsgislib.imageutils.createCopyImage
.. autofunction:: rsgislib.imageutils.createCopyImageVecExtent

Select / Stack bands
---------------------

.. autofunction:: rsgislib.imageutils.selectImageBands
.. autofunction:: rsgislib.imageutils.stackImageBands


Band Names
----------

.. autofunction:: rsgislib.imageutils.setBandNames
.. autofunction:: rsgislib.imageutils.getBandNames


Image Data Types
----------------
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


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

