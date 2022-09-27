RSGISLib Image Registration
==============================

The image registration module contains algorithms for matching images together, either
to find a whole image shift or for generating tie points matching two image which
can then be used to warp the input image.

There are two tie point generation algorithms are available for registration:
basic, and singlelayer. The single layer algorithm is a simplified version of the
algorithm proposed in:

Bunting, P.J., Labrosse, F. & Lucas, R.M., 2010. A multi-resolution area-based
technique for automatic multi-modal image registration. Image and Vision Computing,
28(8), pp.1203-1219.


Image distance metrics:

    * METRIC_EUCLIDEAN = 1
    * METRIC_SQDIFF = 2
    * METRIC_MANHATTEN = 3
    * METRIC_CORELATION = 4


GCP Output Types:

    * TYPE_ENVI_IMG2IMG = 1
    * TYPE_ENVI_IMG2MAP = 2
    * TYPE_RSGIS_IMG2MAP = 3
    * TYPE_RSGIS_MAPOFFS = 4



Whole Image Shift
---------------------
.. autofunction:: rsgislib.imageregistration.find_image_offset
.. autofunction:: rsgislib.imageregistration.apply_offset_to_image


Tie Point Generation
---------------------
.. autofunction:: rsgislib.imageregistration.basic_registration
.. autofunction:: rsgislib.imageregistration.single_layer_registration

Warping
--------
.. autofunction:: rsgislib.imageregistration.warp_with_gcps_with_gdal

Other
------
.. autofunction:: rsgislib.imageregistration.gcp_to_gdal
.. autofunction:: rsgislib.imageregistration.add_vec_pts_as_gcps_to_img

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

