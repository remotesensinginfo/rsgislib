RSGISLib Image Filter
=========================
.. automodule:: rsgislib.imagefilter

Tiled (Multi Processing Core) Filtering
---------------------------------------
.. autofunction:: rsgislib.imagefilter.tiledfilter.perform_tiled_img_filter
.. autofunction:: rsgislib.imagefilter.tiledfilter.perform_tiled_img_multi_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISAbstractFilter
   :members:

Smoothing Filters
-----------------
.. autofunction:: rsgislib.imagefilter.apply_median_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMedianFilter
.. autofunction:: rsgislib.imagefilter.apply_mean_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMeanFilter
.. autofunction:: rsgislib.imagefilter.apply_gaussian_smooth_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISGaussianSmoothFilter


Edge Filters
------------
.. autofunction:: rsgislib.imagefilter.apply_sobel_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISSobelFilter
.. autofunction:: rsgislib.imagefilter.apply_sobel_x_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISSobelXFilter
.. autofunction:: rsgislib.imagefilter.apply_sobel_y_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISSobelYFilter
.. autofunction:: rsgislib.imagefilter.apply_prewitt_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISPrewittFilter
.. autofunction:: rsgislib.imagefilter.apply_prewitt_x_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISPrewittXFilter
.. autofunction:: rsgislib.imagefilter.apply_prewitt_y_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISPrewittYFilter
.. autofunction:: rsgislib.imagefilter.apply_gaussian_1st_deriv_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISGaussian1stDerivFilter
.. autofunction:: rsgislib.imagefilter.apply_gaussian_2nd_deriv_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISGaussian2ndDerivFilter
.. autofunction:: rsgislib.imagefilter.apply_laplacian_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISLaplacianFilter

SAR Filters
-----------
.. autofunction:: rsgislib.imagefilter.apply_lee_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISLeeFilter

Statatics Filters
-----------------
.. autofunction:: rsgislib.imagefilter.apply_min_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMinFilter
.. autofunction:: rsgislib.imagefilter.apply_max_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMaxFilter
.. autofunction:: rsgislib.imagefilter.apply_mode_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISModeFilter
.. autofunction:: rsgislib.imagefilter.apply_stddev_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISStdDevFilter
.. autofunction:: rsgislib.imagefilter.apply_range_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISRangeFilter
.. autofunction:: rsgislib.imagefilter.apply_coeff_of_var_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISCoeffOfVarFilter
.. autofunction:: rsgislib.imagefilter.apply_total_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTotalFilter
.. autofunction:: rsgislib.imagefilter.apply_norm_var_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISNormVarFilter
.. autofunction:: rsgislib.imagefilter.apply_norm_var_sqrt_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISNormVarSqrtFilter
.. autofunction:: rsgislib.imagefilter.apply_norm_var_ln_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISNormVarLnFilter
.. autofunction:: rsgislib.imagefilter.apply_texture_var_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTextureVarFilter
.. autofunction:: rsgislib.imagefilter.apply_kuwahara_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISKuwaharaFilter
.. autofunction:: rsgislib.imagefilter.apply_mean_diff_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMeanDiffFilter
.. autofunction:: rsgislib.imagefilter.apply_mean_diff_abs_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISMeanDiffAbsFilter
.. autofunction:: rsgislib.imagefilter.apply_total_diff_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTotalDiffFilter
.. autofunction:: rsgislib.imagefilter.apply_total_diff_abs_filter
.. autoclass:: rsgislib.imagefilter.tiledfilter.RSGISTotalDiffAbsFilter

Filter Banks
-------------
.. autofunction:: rsgislib.imagefilter.apply_filters
.. autofunction:: rsgislib.imagefilter.leung_malik_filter_bank

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

