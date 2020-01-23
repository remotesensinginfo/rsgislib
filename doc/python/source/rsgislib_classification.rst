RSGISLib Classification Module
=================================

.. automodule:: rsgislib.classification

A number of options for classification are available within this module:

.. toctree::
   :maxdepth: 3

   rsgislib_classification_classsklearn
   rsgislib_classification_classlightgbm
   rsgislib_classification_classkeraspxl
   rsgislib_classification_clustersklearn
   rsgislib_classification_classratutils



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


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

