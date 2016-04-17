RSGISLib Image Segmentation Module
===================================

.. automodule:: rsgislib.segmentation
   :members:
   :undoc-members:

Utilities
---------
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentation
.. autofunction:: rsgislib.segmentation.tiledsegsingle.performTiledSegmentation
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentationPreCalcdStats
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentationTestMinObjSize
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentationTestNumClumps


Clump
------
.. autofunction:: rsgislib.segmentation.clump
.. autofunction:: rsgislib.segmentation.tiledclump.performClumpingSingleThread
.. autofunction:: rsgislib.segmentation.tiledclump.performClumpingMultiProcess
.. autofunction:: rsgislib.segmentation.tiledclump.performUnionClumpingSingleThread
.. autofunction:: rsgislib.segmentation.tiledclump.performUnionClumpingMultiProcess

Label
------
.. autofunction:: rsgislib.segmentation.labelPixelsFromClusterCentres
.. autofunction:: rsgislib.segmentation.relabelClumps

Elimination
--------------
.. autofunction:: rsgislib.segmentation.eliminateSinglePixels
.. autofunction:: rsgislib.segmentation.rmSmallClumps
.. autofunction:: rsgislib.segmentation.rmSmallClumpsStepwise

Join / Union
-------------
.. autofunction:: rsgislib.segmentation.unionOfClumps


Visualisation
--------------
.. autofunction:: rsgislib.segmentation.meanImage


Tiles
-------
.. autofunction:: rsgislib.segmentation.mergeSegmentationTiles
.. autofunction:: rsgislib.segmentation.tiledsegsingle.performTiledSegmentation

.. automodule:: rsgislib.segmentation.tiledclump
   :members:
   :undoc-members:

Other
-----
.. autofunction:: rsgislib.segmentation.generateRegularGrid
.. autofunction:: rsgislib.segmentation.dropSelectedClumps
.. autofunction:: rsgislib.segmentation.findTileBordersMask
.. autofunction:: rsgislib.segmentation.includeRegionsInClumps
.. autofunction:: rsgislib.segmentation.mergeClumpImages
.. autofunction:: rsgislib.segmentation.mergeEquivClumps
.. autofunction:: rsgislib.segmentation.mergeSegments2Neighbours


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

