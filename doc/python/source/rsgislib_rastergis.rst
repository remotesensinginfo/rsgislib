RSGISLib Raster GIS Module
=================================


Utilities
----------

.. autofunction:: rsgislib.rastergis.pop_rat_img_stats
.. autofunction:: rsgislib.rastergis.collapse_rat
.. autofunction:: rsgislib.rastergis.get_column_data

Attribute Clumps
-------------------

.. autofunction:: rsgislib.rastergis.calc_border_length
.. autofunction:: rsgislib.rastergis.calc_dist_between_clumps
.. autofunction:: rsgislib.rastergis.calc_dist_to_large_clumps
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
.. autofunction:: rsgislib.rastergis.define_class_names
.. autofunction:: rsgislib.rastergis.calc_dist_to_classes
.. autofunction:: rsgislib.rastergis.set_column_data


Sampling
--------
.. autofunction:: rsgislib.rastergis.histo_sampling
.. autofunction:: rsgislib.rastergis.take_random_sample

Classification
--------------

.. autofunction:: rsgislib.rastergis.identify_small_units
.. autofunction:: rsgislib.rastergis.class_split_fit_hist_gausian_mixture_model

Extrapolation
-------------
.. autofunction:: rsgislib.rastergis.apply_rat_knn

Change Detection
-----------------

.. autofunction:: rsgislib.rastergis.get_global_class_stats

Statistics
----------
.. autofunction:: rsgislib.rastergis.fit_hist_gausian_mixture_model
.. autofunction:: rsgislib.rastergis.calc_1d_jm_distance
.. autofunction:: rsgislib.rastergis.calc_2d_jm_distance
.. autofunction:: rsgislib.rastergis.calc_bhattacharyya_distance


Copy & Export
----------------
.. autofunction:: rsgislib.rastergis.export_rat_cols_to_ascii
.. autofunction:: rsgislib.rastergis.export_col_to_gdal_img
.. autofunction:: rsgislib.rastergis.export_cols_to_gdal_img
.. autofunction:: rsgislib.rastergis.export_clumps_to_images
.. autofunction:: rsgislib.rastergis.copy_gdal_rat_columns
.. autofunction:: rsgislib.rastergis.copy_rat
.. autofunction:: rsgislib.rastergis.import_vec_atts

Colour Tables
---------------
.. autofunction:: rsgislib.rastergis.colour_rat_classes
.. autofunction:: rsgislib.rastergis.set_class_names_colours

Data Structures / Enums
-----------------------

.. autofunction:: rsgislib.rastergis.BandAttStats
.. autofunction:: rsgislib.rastergis.FieldAttStats
.. autofunction:: rsgislib.rastergis.BandAttPercentiles
.. autofunction:: rsgislib.rastergis.ShapeIndex


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

