RSGISLib Raster GIS Module
=================================


Utilities
----------

.. autofunction:: rsgislib.rastergis.pop_rat_img_stats
.. autofunction:: rsgislib.rastergis.collapse_rat
.. autofunction:: rsgislib.rastergis.ratutils.create_clumps_shp_bbox
.. autofunction:: rsgislib.rastergis.ratutils.get_column_data

Attribute Clumps
-------------------

.. autofunction:: rsgislib.rastergis.calc_border_length
.. autofunction:: rsgislib.rastergis.ratutils.calc_dist_between_clumps
.. autofunction:: rsgislib.rastergis.ratutils.calc_dist_to_large_clumps
.. autofunction:: rsgislib.rastergis.calc_rel_border
.. autofunction:: rsgislib.rastergis.calc_rel_diff_neigh_stats
.. autofunction:: rsgislib.rastergis.define_border_clumps
.. autofunction:: rsgislib.rastergis.define_clump_tile_positions
.. autofunction:: rsgislib.rastergis.find_boundary_pixels
.. autofunction:: rsgislib.rastergis.find_neighbours
.. autofunction:: rsgislib.rastergis.populate_rat_with_cat_proportions
.. autofunction:: rsgislib.rastergis.populate_rat_with_percentiles
.. autofunction:: rsgislib.rastergis.populate_rat_with_stats
.. autofunction:: rsgislib.rastergis.populate_rat_with_meanlit_stats
.. autofunction:: rsgislib.rastergis.select_clumps_on_grid
.. autofunction:: rsgislib.rastergis.clumps_spatial_location
.. autofunction:: rsgislib.rastergis.clumps_spatial_extent
.. autofunction:: rsgislib.rastergis.str_class_majority
.. autofunction:: rsgislib.rastergis.populate_rat_with_mode
.. autofunction:: rsgislib.rastergis.populate_rat_with_prop_valid_pxls
.. autofunction:: rsgislib.rastergis.ratutils.define_class_names
.. autofunction:: rsgislib.rastergis.ratutils.calc_dist_to_classes
.. autofunction:: rsgislib.rastergis.ratutils.set_column_data


Sampling
--------
.. autofunction:: rsgislib.rastergis.histo_sampling
.. autofunction:: rsgislib.rastergis.ratutils.take_random_sample

Classification
--------------

.. autofunction:: rsgislib.rastergis.ratutils.identify_small_units
.. autofunction:: rsgislib.rastergis.ratutils.populate_clumps_with_class_training
.. autofunction:: rsgislib.rastergis.class_split_fit_hist_gausian_mixture_model

Extrapolation
-------------
.. autofunction:: rsgislib.rastergis.apply_rat_knn

Change Detection
-----------------

.. autofunction:: rsgislib.rastergis.get_global_class_stats
.. autofunction:: rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test
.. autofunction:: rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test_lower
.. autofunction:: rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test_upper
.. autofunction:: rsgislib.rastergis.ratutils.find_change_clumps_hist_skew_kurt_test_vote_multi_vars
.. autofunction:: rsgislib.rastergis.ratutils.find_clumps_within_existing_thresholds

Statistics
----------
.. autofunction:: rsgislib.rastergis.fit_hist_gausian_mixture_model
.. autofunction:: rsgislib.rastergis.ratutils.calc_plot_gaussian_histo_model
.. autofunction:: rsgislib.rastergis.calc_1d_jm_distance
.. autofunction:: rsgislib.rastergis.calc_2d_jm_distance
.. autofunction:: rsgislib.rastergis.calc_bhattacharyya_distance


Copy & Export
----------------
.. autofunction:: rsgislib.rastergis.export_rat_cols_to_ascii
.. autofunction:: rsgislib.rastergis.export_col_to_gdal_img
.. autofunction:: rsgislib.rastergis.export_cols_to_gdal_image
.. autofunction:: rsgislib.rastergis.export_clumps_to_images
.. autofunction:: rsgislib.rastergis.copy_gdal_rat_columns
.. autofunction:: rsgislib.rastergis.copy_rat
.. autofunction:: rsgislib.rastergis.import_vec_atts

Colour Tables
---------------
.. autofunction:: rsgislib.rastergis.colour_rat_classes
.. autofunction:: rsgislib.rastergis.ratutils.set_class_names_colours

Data Structures / Enums
-----------------------

.. autofunction:: rsgislib.rastergis.BandAttStats
.. autofunction:: rsgislib.rastergis.FieldAttStats
.. autofunction:: rsgislib.rastergis.BandAttPercentiles
.. autofunction:: rsgislib.rastergis.ShapeIndex
.. autofunction:: rsgislib.rastergis.ratutils.RSGISRATChangeVarInfo
.. autofunction:: rsgislib.rastergis.ratutils.RSGISRATThresMeasure
.. autofunction:: rsgislib.rastergis.ratutils.RSGISRATThresDirection


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

