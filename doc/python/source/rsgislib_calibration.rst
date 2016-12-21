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
.. autofunction:: rsgislib.imagecalibration.calcStandardisedReflectanceSD2010


Surface Reflectance (DOS)
-------------------------

.. autofunction:: rsgislib.imagecalibration.applySubtractOffsets
.. autofunction:: rsgislib.imagecalibration.applySubtractSingleOffsets
.. autofunction:: rsgislib.imagecalibration.performDOSCalc

Irradiance
-----------

.. autofunction:: rsgislib.imagecalibration.calcIrradianceImageElevLUT


Cloud Masking
--------------

.. autofunction:: rsgislib.imagecalibration.applyLandsatTMCloudFMask
.. autofunction:: rsgislib.imagecalibration.calcClearSkyRegions

Utilities
---------

.. autofunction:: rsgislib.imagecalibration.saturatedPixelsMask
.. autofunction:: rsgislib.imagecalibration.calcNadirImgViewAngle
.. autofunction:: rsgislib.imagecalibration.solarangles.getSolarIrrConventionSolarAzimuthFromUSGS
.. autofunction:: rsgislib.imagecalibration.solarangles.getSolarIrrConventionSolarAzimuthFromTrad
.. autofunction:: rsgislib.imagecalibration.solarangles.calcSolarAzimuthZenith

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

