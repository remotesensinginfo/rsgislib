RSGISLib Vector Attributes Module
=================================


Basic Read and Writing Columns
-------------------------------
.. autofunction:: rsgislib.vectorattrs.write_vec_column
.. autofunction:: rsgislib.vectorattrs.write_vec_column_to_layer
.. autofunction:: rsgislib.vectorattrs.read_vec_column
.. autofunction:: rsgislib.vectorattrs.read_vec_columns
.. autofunction:: rsgislib.vectorattrs.get_vec_cols_as_array


Add Columns
---------------
.. autofunction:: rsgislib.vectorattrs.add_fid_col
.. autofunction:: rsgislib.vectorattrs.add_numeric_col_lut
.. autofunction:: rsgislib.vectorattrs.add_numeric_col
.. autofunction:: rsgislib.vectorattrs.add_string_col
.. autofunction:: rsgislib.vectorattrs.add_string_col_lut
.. autofunction:: rsgislib.vectorattrs.create_name_col

Column Utilities
------------------
.. autofunction:: rsgislib.vectorattrs.drop_vec_cols
.. autofunction:: rsgislib.vectorattrs.rename_vec_cols

Joins
--------
.. autofunction:: rsgislib.vectorattrs.perform_spatial_join


Calculate Column Values
--------------------------
.. autofunction:: rsgislib.vectorattrs.pop_bbox_cols
.. autofunction:: rsgislib.vectorattrs.add_geom_bbox_cols
.. autofunction:: rsgislib.vectorattrs.add_unq_numeric_col
.. autofunction:: rsgislib.vectorattrs.calc_npts_in_radius
.. autofunction:: rsgislib.vectorattrs.create_angle_sets

Get Column Summaries
----------------------
.. autofunction:: rsgislib.vectorattrs.get_unq_col_values

Sort By Attributes
-------------------
.. autofunction:: rsgislib.vectorattrs.sort_vec_lyr

Change Attribute Values
------------------------
.. autofunction:: rsgislib.vectorattrs.find_replace_str_vec_lyr

Geometry Intersections
-----------------------
.. autofunction:: rsgislib.vectorattrs.count_pt_intersects
.. autofunction:: rsgislib.vectorattrs.annotate_vec_selection

Export Attribute Table
------------------------
.. autofunction:: rsgislib.vectorattrs.export_vec_attrs_to_csv
.. autofunction:: rsgislib.vectorattrs.export_vec_attrs_to_excel
.. autofunction:: rsgislib.vectorattrs.export_vec_attrs_to_parquet

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

