RSGISLib Vector Geometry Module
=================================

Convert Geometries
-------------------
.. autofunction:: rsgislib.vectorgeoms.convert_polygon_to_polyline
.. autofunction:: rsgislib.vectorgeoms.convert_polys_to_lines_gp
.. autofunction:: rsgislib.vectorgeoms.convert_multi_geoms_to_single
.. autofunction:: rsgislib.vectorgeoms.explode_vec_lyr
.. autofunction:: rsgislib.vectorgeoms.explode_vec_files
.. autofunction:: rsgislib.vectorgeoms.get_vec_lyr_as_pts
.. autofunction:: rsgislib.vectorgeoms.get_geom_pts
.. autofunction:: rsgislib.vectorgeoms.get_geoms_as_bboxs

Calculate New Geometries
------------------------
.. autofunction:: rsgislib.vectorgeoms.create_alpha_shape
.. autofunction:: rsgislib.vectorgeoms.calc_poly_centroids
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_intersection_gp
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_difference_gp
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_sym_difference_gp
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_identity_gp
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_union_gp
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_intersection
.. autofunction:: rsgislib.vectorgeoms.vec_lyr_difference
.. autofunction:: rsgislib.vectorgeoms.clip_vec_lyr
.. autofunction:: rsgislib.vectorgeoms.buffer_vec_layer_gp

Edit Geometries
-----------------
.. autofunction:: rsgislib.vectorgeoms.simplify_geometries
.. autofunction:: rsgislib.vectorgeoms.delete_polygon_holes
.. autofunction:: rsgislib.vectorgeoms.remove_polygon_area
.. autofunction:: rsgislib.vectorgeoms.shiftxy_vec_lyr

Geometry Tests
----------------
.. autofunction:: rsgislib.vectorgeoms.vec_intersects_vec
.. autofunction:: rsgislib.vectorgeoms.vec_overlaps_vec
.. autofunction:: rsgislib.vectorgeoms.vec_within_vec
.. autofunction:: rsgislib.vectorgeoms.vec_contains_vec
.. autofunction:: rsgislib.vectorgeoms.vec_touches_vec
.. autofunction:: rsgislib.vectorgeoms.vec_crosses_vec

Spatial Index
------------------
.. autofunction:: rsgislib.vectorgeoms.create_rtree_index
.. autofunction:: rsgislib.vectorgeoms.bbox_intersects_index


Internal Utilities
--------------------
.. autofunction:: rsgislib.vectorgeoms.get_pt_on_line
.. autofunction:: rsgislib.vectorgeoms.find_pt_to_side
.. autofunction:: rsgislib.vectorgeoms.create_orthg_lines
.. autofunction:: rsgislib.vectorgeoms.closest_line_intersection
.. autofunction:: rsgislib.vectorgeoms.line_intersection_range
.. autofunction:: rsgislib.vectorgeoms.scnd_line_intersection_range


Calculate Geometry Info
-------------------------
.. autofunction:: rsgislib.vectorgeoms.get_poly_hole_area



* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

