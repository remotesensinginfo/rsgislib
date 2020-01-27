RSGISLib Classification Module
=================================

.. automodule:: rsgislib.classification

The classification module has functions which allows classifiers to be applied to image data, either on a per pixel based or following an image segmentation and the classification of the resultant segments/clumps/objects.

The classification functions are available within a number of sub-modules for interfacing with different libraries and methods:

.. toctree::
   :maxdepth: 3

   rsgislib_classification_classsklearn
   rsgislib_classification_classlightgbm
   rsgislib_classification_classkeraspxl
   rsgislib_classification_classkeraschips
   rsgislib_classification_clustersklearn
   rsgislib_classification_classratutils


This rsgislib.classification module provides functions for dealing with training data, undertaking an accuracy assessment and other useful utilities, see below.


Training Data
--------------
.. autofunction:: rsgislib.classification.get_class_training_data
.. autofunction:: rsgislib.classification.split_sample_train_valid_test
.. autofunction:: rsgislib.classification.get_class_training_chips_data
.. autofunction:: rsgislib.classification.split_chip_sample_train_valid_test
.. autofunction:: rsgislib.classification.get_num_samples

Utilities
-----------
.. autofunction:: rsgislib.classification.collapseClasses
.. autofunction:: rsgislib.classification.colour3bands

Accuracy Assessment
---------------------
.. autofunction:: rsgislib.classification.generateRandomAccuracyPts
.. autofunction:: rsgislib.classification.generateStratifiedRandomAccuracyPts
.. autofunction:: rsgislib.classification.generateTransectAccuracyPts
.. autofunction:: rsgislib.classification.popClassInfoAccuracyPts


Classification Utility Classes
-------------------------------

.. autoclass:: rsgislib.classification.ClassSimpleInfoObj
.. autoclass:: rsgislib.classification.ClassInfoObj
.. autoclass:: rsgislib.classification.ClassVecSamplesInfoObj
.. autoclass:: rsgislib.classification.SamplesInfoObj

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

