RSGISLib Image Calibration Module
=================================
.. automodule:: rsgislib.imagecalibration
   :members:
   :undoc-members:

Radiance
---------

.. autofunction:: rsgislib.imagecalibration.landsat2Radiance
.. autofunction:: rsgislib.imagecalibration.landsat2RadianceMultiAdd
.. autofunction:: rsgislib.imagecalibration.spot5ToRadiance
.. autofunction:: rsgislib.imagecalibration.worldview2ToRadiance

Top-of Atmosphere Reflectance
-------------------------------

.. autofunction:: rsgislib.imagecalibration.radiance2TOARefl
.. autofunction:: rsgislib.imagecalibration.landsatThermalRad2Brightness


Surface Reflectance (6S)
------------------------

.. autofunction:: rsgislib.imagecalibration.apply6SCoeffSingleParam
.. autofunction:: rsgislib.imagecalibration.apply6SCoeffElevAOTLUTParam
.. autofunction:: rsgislib.imagecalibration.apply6SCoeffElevLUTParam

Surface Reflectance (DOS)
-------------------------

.. autofunction:: rsgislib.imagecalibration.applySubtractOffsets
.. autofunction:: rsgislib.imagecalibration.applySubtractSingleOffsets
.. autofunction:: rsgislib.imagecalibration.performDOSCalc

Cloud Masking
--------------

.. autofunction:: rsgislib.imagecalibration.applyLandsatTMCloudFMask


Utilities
---------

.. autofunction:: rsgislib.imagecalibration.saturatedPixelsMask


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

