RSGISLib Zonal Stats Module
=================================

.. automodule:: rsgislib.zonalstats
   :members:
   :undoc-members:

Points
-------
These functions take a shapefile with point geomoetry and extract the value of the pixel the point falls within.

.. autofunction:: rsgislib.zonalstats.pointValue2SHP
.. autofunction:: rsgislib.zonalstats.pointValue2TXT


Polygons
---------
These functions take a polygons as input and either calculate statistics from pixels falling within the polygon or save out all pixel values.

.. autofunction:: rsgislib.zonalstats.pixelVals2TXT
.. autofunction:: rsgislib.zonalstats.pixelStats2SHP
.. autofunction:: rsgislib.zonalstats.pixelStats2TXT
.. autofunction:: rsgislib.zonalstats.imageZoneToHDF

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

