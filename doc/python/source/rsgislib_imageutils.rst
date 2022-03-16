RSGISLib Image Utilities Module
=================================


Image Stats and Pyramids
------------------------
.. autofunction:: rsgislib.imageutils.pop_img_stats
.. autofunction:: rsgislib.imageutils.pop_thmt_img_stats
.. autofunction:: rsgislib.imageutils.get_img_band_stats
.. autofunction:: rsgislib.imageutils.get_img_no_data_value
.. autofunction:: rsgislib.imageutils.set_img_no_data_value


Image Creation Options
------------------------

.. autofunction:: rsgislib.imageutils.get_gdal_img_creation_opts
.. autofunction:: rsgislib.imageutils.set_env_vars_lzw_gtiff_outs
.. autofunction:: rsgislib.imageutils.set_env_vars_deflate_gtiff_outs


Get Image Info
---------------
.. autofunction:: rsgislib.imageutils.get_img_res
.. autofunction:: rsgislib.imageutils.get_img_size
.. autofunction:: rsgislib.imageutils.get_img_band_count
.. autofunction:: rsgislib.imageutils.get_img_bbox
.. autofunction:: rsgislib.imageutils.get_img_bbox_in_proj
.. autofunction:: rsgislib.imageutils.get_img_subset_pxl_bbox
.. autofunction:: rsgislib.imageutils.get_img_pxl_spatial_coords
.. autofunction:: rsgislib.imageutils.get_img_files
.. autofunction:: rsgislib.imageutils.has_gcps
.. autofunction:: rsgislib.imageutils.get_gdal_format_name
.. autofunction:: rsgislib.imageutils.is_img_thematic
.. autofunction:: rsgislib.imageutils.get_img_pxl_coords

Spatial Header info
--------------------
.. autofunction:: rsgislib.imageutils.assign_wkt_proj
.. autofunction:: rsgislib.imageutils.assign_spatial_info
.. autofunction:: rsgislib.imageutils.copy_proj_from_img
.. autofunction:: rsgislib.imageutils.copy_spatial_and_proj_from_img
.. autofunction:: rsgislib.imageutils.get_wkt_proj_from_img
.. autofunction:: rsgislib.imageutils.get_epsg_proj_from_img
.. autofunction:: rsgislib.imageutils.get_utm_zone
.. autofunction:: rsgislib.imageutils.copy_gcps

Image Reprojection / Warp
---------------------------
.. autofunction:: rsgislib.imageutils.resample_img_to_match
.. autofunction:: rsgislib.imageutils.reproject_image
.. autofunction:: rsgislib.imageutils.gdal_warp


Mosaic 
-------
.. autofunction:: rsgislib.imageutils.create_img_mosaic
.. autofunction:: rsgislib.imageutils.include_imgs
.. autofunction:: rsgislib.imageutils.include_imgs_with_overlap
.. autofunction:: rsgislib.imageutils.include_imgs_ind_img_intersect
.. autofunction:: rsgislib.imageutils.combine_imgs_to_band


Composite
----------

.. autofunction:: rsgislib.imageutils.create_ref_img_composite_img
.. autofunction:: rsgislib.imageutils.combine_binary_masks
.. autofunction:: rsgislib.imageutils.export_single_merged_img_band
.. autofunction:: rsgislib.imageutils.imagecomp.create_max_ndvi_composite
.. autofunction:: rsgislib.imageutils.imagecomp.create_max_ndvi_ndwi_composite_landsat
.. autofunction:: rsgislib.imageutils.imagecomp.create_max_ndvi_ndwi_composite
.. autofunction:: rsgislib.imageutils.imagecomp.create_max_scaled_ndvi_ndwi_composite
.. autofunction:: rsgislib.imageutils.imagecomp.check_build_ls8_vrts
.. autofunction:: rsgislib.imageutils.order_img_using_prop_valid_pxls
.. autofunction:: rsgislib.imageutils.gen_timeseries_fill_composite_img
.. autoclass:: rsgislib.imageutils.RSGISTimeseriesFillInfo
    :members:


Tile
-------
.. autofunction:: rsgislib.imageutils.create_tiles
.. autofunction:: rsgislib.imageutils.create_tiles_multi_core
.. autofunction:: rsgislib.imageutils.tilingutils.create_min_data_tiles
.. autofunction:: rsgislib.imageutils.tilingutils.create_tiles_from_masks
.. autofunction:: rsgislib.imageutils.tilingutils.create_tile_mask_images_from_shp
.. autofunction:: rsgislib.imageutils.tilingutils.create_tile_mask_images_from_clumps


Visualisation / Normalisation
------------------------------

.. autofunction:: rsgislib.imageutils.stretch_img
.. autofunction:: rsgislib.imageutils.stretch_img_with_stats
.. autofunction:: rsgislib.imageutils.normalise_img_pxl_vals
.. autofunction:: rsgislib.imageutils.get_img_band_colour_interp
.. autofunction:: rsgislib.imageutils.set_img_band_colour_interp
.. autofunction:: rsgislib.imageutils.set_img_thematic
.. autofunction:: rsgislib.imageutils.set_img_not_thematic
.. autofunction:: rsgislib.imageutils.define_colour_table


Masking
---------
.. autofunction:: rsgislib.imageutils.mask_img
.. autofunction:: rsgislib.imageutils.gen_finite_mask
.. autofunction:: rsgislib.imageutils.gen_valid_mask
.. autofunction:: rsgislib.imageutils.gen_img_edge_mask
.. autofunction:: rsgislib.imageutils.mask_img_with_vec
.. autofunction:: rsgislib.imageutils.mask_all_band_zero_vals
.. autofunction:: rsgislib.imageutils.create_valid_mask
.. autoclass:: rsgislib.imageutils.ImageBandInfo
    :members:


Subsetting
------------

.. autofunction:: rsgislib.imageutils.subset
.. autofunction:: rsgislib.imageutils.subset_to_img
.. autofunction:: rsgislib.imageutils.subset_to_geoms_bbox
.. autofunction:: rsgislib.imageutils.subset_bbox
.. autofunction:: rsgislib.imageutils.subset_pxl_bbox
.. autofunction:: rsgislib.imageutils.subset_to_vec


Extract / Sample
-----------------
.. autofunction:: rsgislib.imageutils.perform_random_pxl_sample_in_mask
.. autofunction:: rsgislib.imageutils.perform_random_pxl_sample_in_mask_low_pxl_count
.. autofunction:: rsgislib.imageutils.extract_img_pxl_sample
.. autofunction:: rsgislib.imageutils.extract_img_pxl_vals_in_msk


Create
---------
.. autofunction:: rsgislib.imageutils.create_blank_img
.. autofunction:: rsgislib.imageutils.create_blank_img_py
.. autofunction:: rsgislib.imageutils.create_copy_img
.. autofunction:: rsgislib.imageutils.create_copy_img_def_extent
.. autofunction:: rsgislib.imageutils.create_blank_buf_img_from_ref_img
.. autofunction:: rsgislib.imageutils.create_blank_img_from_ref_vector
.. autofunction:: rsgislib.imageutils.create_copy_img_vec_extent_snap_to_grid
.. autofunction:: rsgislib.imageutils.create_blank_img_from_bbox
.. autofunction:: rsgislib.imageutils.create_img_for_each_vec_feat
.. autofunction:: rsgislib.imageutils.gdal_translate
.. autofunction:: rsgislib.imageutils.create_copy_img_vec_extent

Create VRT
------------
.. autofunction:: rsgislib.imageutils.create_stack_images_vrt
.. autofunction:: rsgislib.imageutils.create_mosaic_images_vrt
.. autofunction:: rsgislib.imageutils.create_vrt_band_subset


Select / Stack bands
---------------------
.. autofunction:: rsgislib.imageutils.select_img_bands
.. autofunction:: rsgislib.imageutils.stack_img_bands


Sharpen Image Bands
-------------------
.. autofunction:: rsgislib.imageutils.pan_sharpen_hcs
.. autofunction:: rsgislib.imageutils.sharpen_low_res_bands
.. autoclass:: rsgislib.imageutils.SharpBandInfo
    :members:



Band Names
----------
.. autofunction:: rsgislib.imageutils.set_band_names
.. autofunction:: rsgislib.imageutils.get_band_names


Image Data Types
-----------------
.. autofunction:: rsgislib.imageutils.get_gdal_datatype_from_img
.. autofunction:: rsgislib.imageutils.get_rsgislib_datatype_from_img
.. autofunction:: rsgislib.imageutils.get_gdal_datatype_name_from_img


Look Up Tables
--------------
.. autofunction:: rsgislib.imageutils.imagelut.query_img_lut
.. autofunction:: rsgislib.imageutils.imagelut.get_all_lut_imgs
.. autofunction:: rsgislib.imageutils.imagelut.create_img_extent_lut
.. autofunction:: rsgislib.imageutils.imagelut.query_file_lut
.. autofunction:: rsgislib.imageutils.imagelut.get_raster_lyr



Check Images
---------------

.. autofunction:: rsgislib.imageutils.do_img_res_match
.. autofunction:: rsgislib.imageutils.check_img_lst
.. autofunction:: rsgislib.imageutils.check_img_file_comparison
.. autofunction:: rsgislib.imageutils.do_images_overlap
.. autofunction:: rsgislib.imageutils.do_gdal_layers_have_same_proj
.. autofunction:: rsgislib.imageutils.test_img_lst_intersects


Pixel Size / Area
-------------------
.. autofunction:: rsgislib.imageutils.calc_wgs84_pixel_area
.. autofunction:: rsgislib.imageutils.calc_pixel_locations


Image File Operations:
------------------------

.. autofunction:: rsgislib.imageutils.get_gdal_format_from_ext
.. autofunction:: rsgislib.imageutils.get_file_img_extension
.. autofunction:: rsgislib.imageutils.rename_gdal_layer
.. autofunction:: rsgislib.imageutils.delete_gdal_layer


Image I/O
----------

.. autofunction:: rsgislib.imageutils.get_img_pxl_values
.. autofunction:: rsgislib.imageutils.set_img_pxl_values
.. autofunction:: rsgislib.imageutils.get_img_pxl_column
.. autofunction:: rsgislib.imageutils.assign_random_pxls
.. autofunction:: rsgislib.imageutils.generate_random_pxl_vals_img


Other
------
.. autofunction:: rsgislib.imageutils.gen_sampling_grid
.. autofunction:: rsgislib.imageutils.whiten_image
.. autofunction:: rsgislib.imageutils.spectral_smoothing


Get Image Metadata
-------------------
.. autofunction:: rsgislib.imageutils.set_img_band_metadata
.. autofunction:: rsgislib.imageutils.get_img_band_metadata
.. autofunction:: rsgislib.imageutils.get_img_band_metadata_fields
.. autofunction:: rsgislib.imageutils.get_img_band_metadata_fields_dict
.. autofunction:: rsgislib.imageutils.set_img_metadata
.. autofunction:: rsgislib.imageutils.get_img_metadata
.. autofunction:: rsgislib.imageutils.get_img_metadata_fields
.. autofunction:: rsgislib.imageutils.get_img_metadata_fields_dict


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

