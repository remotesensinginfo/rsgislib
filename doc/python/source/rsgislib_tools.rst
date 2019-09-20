RSGISLib Tools Module
=================================

.. automodule:: rsgislib.tools
   :members:
   :undoc-members:

Plotting
---------
These functions make use of rsgislib functions and matplotlib to provide useful plotting functionality.

.. autofunction:: rsgislib.tools.plotImageSpectra
.. autofunction:: rsgislib.tools.plotImageComparison
.. autofunction:: rsgislib.tools.plotImageHistogram

Projection
-----------

.. autofunction:: rsgislib.tools.degrees_to_metres
.. autofunction:: rsgislib.tools.metres_to_degrees

UTM
----

.. autofunction:: rsgislib.tools.utm.check_valid_zone
.. autofunction:: rsgislib.tools.utm.epsg_for_UTM
.. autofunction:: rsgislib.tools.utm.from_latlon
.. autofunction:: rsgislib.tools.utm.latitude_to_zone_letter
.. autofunction:: rsgislib.tools.utm.latlon_to_zone_number
.. autofunction:: rsgislib.tools.utm.min_max_eastings_for_lat
.. autofunction:: rsgislib.tools.utm.split_wgs84_bbox_utm_zones
.. autofunction:: rsgislib.tools.utm.to_latlon
.. autofunction:: rsgislib.tools.utm.zone_number_to_central_longitude


File Manipulation
------------------

.. autofunction:: rsgislib.tools.sortImgsUTM2DIRs


Visualisation
--------------

.. autofunction:: rsgislib.tools.visualisation.createKMZImg
.. autofunction:: rsgislib.tools.visualisation.createWebTilesImg


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

