RSGISLib Data Sources
========================

This module has functions to help with searching for and downloading data.
Tutorials showing how to use these functions available `here <https://github.com/remotesensinginfo/rsgislib-tutorials/tree/main/11_download_datasets>`_


USGS Earth Explorer
---------------------

.. autofunction:: rsgislib.dataaccess.usgs_m2m.usgs_login
.. autofunction:: rsgislib.dataaccess.usgs_m2m.usgs_logout
.. autofunction:: rsgislib.dataaccess.usgs_m2m.can_user_dwnld
.. autofunction:: rsgislib.dataaccess.usgs_m2m.can_user_order
.. autofunction:: rsgislib.dataaccess.usgs_m2m.get_wrs_pt
.. autofunction:: rsgislib.dataaccess.usgs_m2m.get_wrs_bbox
.. autofunction:: rsgislib.dataaccess.usgs_m2m.usgs_search
.. autofunction:: rsgislib.dataaccess.usgs_m2m.get_all_usgs_search
.. autofunction:: rsgislib.dataaccess.usgs_m2m.get_download_ids
.. autofunction:: rsgislib.dataaccess.usgs_m2m.create_scene_list
.. autofunction:: rsgislib.dataaccess.usgs_m2m.remove_scene_list
.. autofunction:: rsgislib.dataaccess.usgs_m2m.check_dwnld_opts
.. autofunction:: rsgislib.dataaccess.usgs_m2m.request_downloads


NASA Common Metadata Repository
---------------------------------

.. autofunction:: rsgislib.dataaccess.nasa_cmr.get_prods_info
.. autofunction:: rsgislib.dataaccess.nasa_cmr.check_prod_version_avail
.. autofunction:: rsgislib.dataaccess.nasa_cmr.get_max_prod_version
.. autofunction:: rsgislib.dataaccess.nasa_cmr.find_granules
.. autofunction:: rsgislib.dataaccess.nasa_cmr.find_all_granules
.. autofunction:: rsgislib.dataaccess.nasa_cmr.get_total_file_size
.. autofunction:: rsgislib.dataaccess.nasa_cmr.cmr_download_file_http
.. autofunction:: rsgislib.dataaccess.nasa_cmr.create_cmr_dwnld_db
.. autofunction:: rsgislib.dataaccess.nasa_cmr.download_granules_use_dwnld_db

Copernicus Data Space Ecosystem
---------------------------------

.. note::  See `Copernicus OData API <rsgislib_dataaccess_copernicus_odata.html>`_ for constants.

.. autofunction:: rsgislib.dataaccess.copernicus_odata.get_access_token
.. autofunction:: rsgislib.dataaccess.copernicus_odata.query_scn
.. autofunction:: rsgislib.dataaccess.copernicus_odata.query_scn_lst
.. autofunction:: rsgislib.dataaccess.copernicus_odata.download_scn
.. autofunction:: rsgislib.dataaccess.copernicus_odata.download_scns
.. autofunction:: rsgislib.dataaccess.copernicus_odata.get_sensor_collection_name


Planet Data
-------------

.. note::  See `Planet Data API <rsgislib_dataaccess_planet_data_api.html>`_ for constants.

.. autofunction:: rsgislib.dataaccess.planet_data_api.planet_auth
.. autofunction:: rsgislib.dataaccess.planet_data_api.run_search_planet_items
.. autofunction:: rsgislib.dataaccess.planet_data_api.run_create_planet_order
.. autofunction:: rsgislib.dataaccess.planet_data_api.run_download_planet_order
.. autofunction:: rsgislib.dataaccess.planet_data_api.run_get_planet_orders
.. autofunction:: rsgislib.dataaccess.planet_data_api.run_cancel_planet_orders
.. autofunction:: rsgislib.dataaccess.planet_data_api.run_download_and_validate_item
.. autofunction:: rsgislib.dataaccess.planet_data_api.get_item_type_str
.. autofunction:: rsgislib.dataaccess.planet_data_api.get_asset_type_str
.. autofunction:: rsgislib.dataaccess.planet_data_api.get_bundle_type_str

Open Street Map (OSM)
-----------------------

.. autofunction:: rsgislib.dataaccess.openstreetmap.get_osm_to_file
.. autofunction:: rsgislib.dataaccess.openstreetmap.get_osm_multi_to_file
.. autofunction:: rsgislib.dataaccess.openstreetmap.get_osm_gdf
