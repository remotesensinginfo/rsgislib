RSGISLib Zonal Stats Module
=================================

For undertaking a pixel-in-polygon analysis you need to consider the size of the
polygons with respect to the size of the pixels being intersected.

Where the pixels are small with respect to the polygons so there is at least one pixel
within the polygon then the best function to use is:

* rsgislib.zonalstats.calc_zonal_band_stats

If the pixels are large with respect to the polygons then use the following function
which intersects the polygon centroid.

* rsgislib.zonalstats.calc_zonal_poly_pts_band_stats

If the pixel size in between and/or polygons are varied in size such that it is not
certain that all polygons will contain a pixel then the following function will first
attempt to intersect the polygon with the pixels and if there is not a pixel within
the polygon then the centriod is used.

* rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts

Points
-------
These functions extract values from an image for a set of vector points.

.. autofunction:: rsgislib.zonalstats.ext_point_band_values_file
.. autofunction:: rsgislib.zonalstats.ext_point_band_values


Polygons
----------

.. autofunction:: rsgislib.zonalstats.calc_zonal_band_stats_file
.. autofunction:: rsgislib.zonalstats.calc_zonal_band_stats
.. autofunction:: rsgislib.zonalstats.calc_zonal_poly_pts_band_stats_file
.. autofunction:: rsgislib.zonalstats.calc_zonal_poly_pts_band_stats
.. autofunction:: rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file
.. autofunction:: rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts


Extracting Pixels to HDF5
--------------------------

.. autofunction:: rsgislib.zonalstats.image_zone_to_hdf
.. autofunction:: rsgislib.zonalstats.extract_zone_img_values_to_hdf
.. autofunction:: rsgislib.zonalstats.extract_zone_img_band_values_to_hdf
.. autofunction:: rsgislib.zonalstats.random_sample_hdf5_file
.. autofunction:: rsgislib.zonalstats.split_sample_hdf5_file
.. autofunction:: rsgislib.zonalstats.merge_extracted_hdf5_data
.. autofunction:: rsgislib.zonalstats.merge_extracted_hdf5_vars_data

Extracting Image Chips to HDF5
-------------------------------

.. autofunction:: rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf
.. autofunction:: rsgislib.zonalstats.split_sample_chip_hdf5_file
.. autofunction:: rsgislib.zonalstats.merge_extracted_hdf5_chip_data
.. autofunction:: rsgislib.zonalstats.extract_ref_chip_zone_image_band_values_to_hdf
.. autofunction:: rsgislib.zonalstats.split_sample_ref_chip_hdf5_file
.. autofunction:: rsgislib.zonalstats.merge_extracted_hdf5_chip_ref_data

HDF Utilities
-------------------------------
.. autofunction:: rsgislib.zonalstats.msk_h5_smpls_to_finite_values
.. autofunction:: rsgislib.zonalstats.get_hdf5_data
.. autofunction:: rsgislib.zonalstats.get_var_from_hdf5_data


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

