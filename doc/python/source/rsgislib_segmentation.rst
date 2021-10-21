RSGISLib Image Segmentation Module
===================================

Commands to perform a series of segmentations on input imagery

Utilities
---------
.. autofunction:: rsgislib.segmentation.segutils.run_shepherd_segmentation
.. autofunction:: rsgislib.segmentation.tiledsegsingle.perform_tiled_segmentation
.. autofunction:: rsgislib.segmentation.segutils.run_shepherd_segmentation_pre_calcd_stats
.. autofunction:: rsgislib.segmentation.segutils.run_shepherd_segmentation_test_min_obj_size
.. autofunction:: rsgislib.segmentation.segutils.run_shepherd_segmentation_test_num_clumps


Clump
------
.. autofunction:: rsgislib.segmentation.clump
.. autofunction:: rsgislib.segmentation.tiledclump.perform_clumping_single_thread
.. autofunction:: rsgislib.segmentation.tiledclump.perform_clumping_multi_process
.. autofunction:: rsgislib.segmentation.tiledclump.perform_union_clumping_single_thread
.. autofunction:: rsgislib.segmentation.tiledclump.perform_union_clumping_multi_process

Label
------
.. autofunction:: rsgislib.segmentation.label_pixels_from_cluster_centres
.. autofunction:: rsgislib.segmentation.relabel_clumps

Elimination
--------------
.. autofunction:: rsgislib.segmentation.eliminate_single_pixels
.. autofunction:: rsgislib.segmentation.rm_small_clumps
.. autofunction:: rsgislib.segmentation.rm_small_clumps_stepwise

Join / Union
-------------
.. autofunction:: rsgislib.segmentation.union_of_clumps


Visualisation
--------------
.. autofunction:: rsgislib.segmentation.mean_image


Tiles
-------
.. autofunction:: rsgislib.segmentation.merge_segmentation_tiles
.. autofunction:: rsgislib.segmentation.tiledsegsingle.perform_tiled_segmentation


scikit-image
------------
.. automodule:: rsgislib.segmentation.skimgseg
   :members:
   :undoc-members:

Other
-----
.. autofunction:: rsgislib.segmentation.generate_regular_grid
.. autofunction:: rsgislib.segmentation.drop_selected_clumps
.. autofunction:: rsgislib.segmentation.find_tile_borders_mask
.. autofunction:: rsgislib.segmentation.include_regions_in_clumps
.. autofunction:: rsgislib.segmentation.merge_clump_images
.. autofunction:: rsgislib.segmentation.merge_equiv_clumps
.. autofunction:: rsgislib.segmentation.merge_segments_to_neighbours


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

