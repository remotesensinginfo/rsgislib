RSGISLib Image Segmentation Module
===================================

.. automodule:: rsgislib.segmentation
   :members:
   :undoc-members:

Utilities
---------
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentation
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentationTestMinObjSize
.. autofunction:: rsgislib.segmentation.segutils.runShepherdSegmentationTestNumClumps


Clump
------
.. autofunction:: rsgislib.segmentation.clump

Label
------
.. autofunction:: rsgislib.segmentation.labelPixelsFromClusterCentres
.. autofunction:: rsgislib.segmentation.relabelClumps


Elimination
--------------
.. autofunction:: rsgislib.segmentation.eliminateSinglePixels
.. autofunction:: rsgislib.segmentation.rmSmallClumps
.. autofunction:: rsgislib.segmentation.RMSmallClumpsStepwise

Join / Union
-------------
.. autofunction:: rsgislib.segmentation.UnionOfClumps


Visualisation
--------------
.. autofunction:: rsgislib.segmentation.meanImage


Tiles
------
.. autofunction:: rsgislib.segmentation.mergeSegmentationTiles

Other
-----
.. autofunction:: rsgislib.segmentation.generateRegularGrid

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

