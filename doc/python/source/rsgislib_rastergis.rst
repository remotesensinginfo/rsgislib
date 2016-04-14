RSGISLib Raster GIS Module
=================================

.. automodule:: rsgislib.rastergis
   :members:
   :undoc-members:

Utilities
---------

.. autofunction:: rsgislib.rastergis.ratutils.populateImageStats
.. autofunction:: rsgislib.rastergis.collapseRAT

Attribute Segments
-------------------

.. autofunction:: rsgislib.rastergis.calcBorderLength
.. autofunction:: rsgislib.rastergis.calcRelBorder
.. autofunction:: rsgislib.rastergis.calcRelDiffNeighStats
.. autofunction:: rsgislib.rastergis.defineBorderClumps
.. autofunction:: rsgislib.rastergis.defineClumpTilePositions
.. autofunction:: rsgislib.rastergis.findBoundaryPixels
.. autofunction:: rsgislib.rastergis.findChangeClumpsFromStdDev
.. autofunction:: rsgislib.rastergis.findNeighbours
.. autofunction:: rsgislib.rastergis.populateCategoryProportions
.. autofunction:: rsgislib.rastergis.populateRATWithPercentiles
.. autofunction:: rsgislib.rastergis.populateRATWithStats
.. autofunction:: rsgislib.rastergis.populateRATWithMeanLitStats
.. autofunction:: rsgislib.rastergis.populateStats
.. autofunction:: rsgislib.rastergis.selectClumpsOnGrid
.. autofunction:: rsgislib.rastergis.spatialLocation
.. autofunction:: rsgislib.rastergis.strClassMajority
.. autofunction:: rsgislib.rastergis.populateRATWithMode

Sampling
--------
.. autofunction:: rsgislib.rastergis.histoSampling

Classification
--------------

.. autofunction:: rsgislib.rastergis.binaryClassification
.. autofunction:: rsgislib.rastergis.regionGrowClass
.. autofunction:: rsgislib.rastergis.regionGrowClassNeighCritera

Extrapolation
-------------
.. autofunction:: rsgislib.rastergis.applyKNN

Change Detection
-----------------

.. autofunction:: rsgislib.rastergis.findChangeClumpsFromStdDev
.. autofunction:: rsgislib.rastergis.getGlobalClassStats
.. autofunction:: rsgislib.rastergis.classSplitFitHistGausianMixtureModel

Statistics
----------
.. autofunction:: rsgislib.rastergis.fitHistGausianMixtureModel
.. autofunction:: rsgislib.rastergis.ratutils.calcPlotGaussianHistoModel

Copy & Export
----------------
.. autofunction:: rsgislib.rastergis.export2Ascii
.. autofunction:: rsgislib.rastergis.exportCol2GDALImage
.. autofunction:: rsgislib.rastergis.exportCols2GDALImage
.. autofunction:: rsgislib.rastergis.copyGDALATTColumns
.. autofunction:: rsgislib.rastergis.copyRAT
.. autofunction:: rsgislib.rastergis.interpolateClumpValues2Image
.. autofunction:: rsgislib.rastergis.importVecAtts

Colour Tables
---------------
.. autofunction:: rsgislib.rastergis.colourClasses


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

