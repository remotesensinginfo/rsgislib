RSGISLib Image Utilities Module
=================================

.. automodule:: rsgislib.imageutils
   :undoc-members:

Projection
-----------

.. autofunction:: rsgislib.imageutils.assignProj
.. autofunction:: rsgislib.imageutils.assignSpatialInfo
.. autofunction:: rsgislib.imageutils.copyProjFromImage
.. autofunction:: rsgislib.imageutils.copySpatialAndProjFromImage
.. autofunction:: rsgislib.imageutils.resampleImage2Match
.. autofunction:: rsgislib.imageutils.reprojectImage

Mosaic
----------

.. autofunction:: rsgislib.imageutils.createImageMosaic
.. autofunction:: rsgislib.imageutils.includeImages
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

.. autofunction:: rsgislib.imageutils.extractZoneImageValues2HDF


Create
---------

.. autofunction:: rsgislib.imageutils.createBlankImage
.. autofunction:: rsgislib.imageutils.createCopyImage
.. autofunction:: rsgislib.imageutils.createCopyImageVecExtent

Select / Stack bands
---------------------

.. autofunction:: rsgislib.imageutils.selectImageBands
.. autofunction:: rsgislib.imageutils.stackImageBands


Other
------

.. autofunction:: rsgislib.imageutils.popImageStats
.. autofunction:: rsgislib.imageutils.stackStats
.. autofunction:: rsgislib.imageutils.orderImageUsingValidPxls
.. autofunction:: rsgislib.imageutils.setBandNames
.. autofunction:: rsgislib.imageutils.genSamplingGrid
.. autofunction:: rsgislib.imageutils.getGDALDataType

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

