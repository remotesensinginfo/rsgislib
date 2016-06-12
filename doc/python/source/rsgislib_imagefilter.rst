RSGISLib Image Filter Module
=================================
.. automodule:: rsgislib.imagefilter

Tiled (Multi Processing Core) Filtering
---------------------------------------
.. autofunction:: rsgislib.imagefilter.tiledfilter.performTiledImgFilter
.. autofunction:: rsgislib.imagefilter.tiledfilter.performTiledImgMultiFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISAbstractFilter
   :members:

Smoothing Filters
-----------------
.. autofunction:: rsgislib.imagefilter.applyMedianFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMedianFilter
.. autofunction:: rsgislib.imagefilter.applyMeanFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMeanFilter
.. autofunction:: rsgislib.imagefilter.applyGaussianSmoothFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISGaussianSmoothFilter


Edge Filters
------------
.. autofunction:: rsgislib.imagefilter.applySobelFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISSobelFilter
.. autofunction:: rsgislib.imagefilter.applySobelXFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISSobelXFilter
.. autofunction:: rsgislib.imagefilter.applySobelYFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISSobelYFilter
.. autofunction:: rsgislib.imagefilter.applyPrewittFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISPrewittFilter
.. autofunction:: rsgislib.imagefilter.applyPrewittXFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISPrewittXFilter
.. autofunction:: rsgislib.imagefilter.applyPrewittYFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISPrewittYFilter
.. autofunction:: rsgislib.imagefilter.applyGaussian1stDerivFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISGaussian1stDerivFilter
.. autofunction:: rsgislib.imagefilter.applyGaussian2ndDerivFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISGaussian2ndDerivFilter
.. autofunction:: rsgislib.imagefilter.applyLaplacianFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISLaplacianFilter

SAR Filters
-----------
.. autofunction:: rsgislib.imagefilter.applyLeeFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISLeeFilter

Statatics Filters
-----------------
.. autofunction:: rsgislib.imagefilter.applyMinFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMinFilter
.. autofunction:: rsgislib.imagefilter.applyMaxFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMaxFilter
.. autofunction:: rsgislib.imagefilter.applyModeFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISModeFilter
.. autofunction:: rsgislib.imagefilter.applyStdDevFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISStdDevFilter
.. autofunction:: rsgislib.imagefilter.applyRangeFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISRangeFilter
.. autofunction:: rsgislib.imagefilter.applyCoeffOfVarFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISCoeffOfVarFilter
.. autofunction:: rsgislib.imagefilter.applyTotalFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTotalFilter
.. autofunction:: rsgislib.imagefilter.applyNormVarFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISNormVarFilter
.. autofunction:: rsgislib.imagefilter.applyNormVarSqrtFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISNormVarSqrtFilter
.. autofunction:: rsgislib.imagefilter.applyNormVarLnFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISNormVarLnFilter
.. autofunction:: rsgislib.imagefilter.applyTextureVarFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTextureVarFilter
.. autofunction:: rsgislib.imagefilter.applyKuwaharaFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISKuwaharaFilter
.. autofunction:: rsgislib.imagefilter.applyMeanDiffFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMeanDiffFilter
.. autofunction:: rsgislib.imagefilter.applyMeanDiffAbsFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMeanDiffAbsFilter
.. autofunction:: rsgislib.imagefilter.applyTotalDiffFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTotalDiffFilter
.. autofunction:: rsgislib.imagefilter.applyTotalDiffAbsFilter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTotalDiffAbsFilter

Filter Banks
-------------
.. autofunction:: rsgislib.imagefilter.applyfilters
.. autofunction:: rsgislib.imagefilter.LeungMalikFilterBank

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

