RSGISLib Classification
===========================

The classification module has functions which allows classifiers to be applied to image data, either on a per pixel based or following an image segmentation and the classification of the resultant segments/clumps/objects.

The classification functions are available within a number of sub-modules for interfacing with different libraries and methods:

.. toctree::
   :maxdepth: 3

   rsgislib_classification_classsklearn
   rsgislib_classification_classlightgbm
   rsgislib_classification_classxgboost
   rsgislib_classification_classkeraspxl
   rsgislib_classification_classkeraschips
   rsgislib_classification_classratutils
   rsgislib_classification_clustersklearn


This rsgislib.classification module provides functions for dealing with training data, undertaking an accuracy assessment and other useful utilities, see below.


Pixel Training Data
-------------------
.. autofunction:: rsgislib.classification.get_class_training_data
.. autofunction:: rsgislib.classification.split_sample_train_valid_test
.. autofunction:: rsgislib.classification.get_class_training_chips_data
.. autofunction:: rsgislib.classification.get_num_samples
.. autofunction:: rsgislib.classification.plot_train_data

Chips Training Data
--------------------
.. autofunction:: rsgislib.classification.split_chip_sample_train_valid_test
.. autofunction:: rsgislib.classification.split_chip_sample_ref_train_valid_test
.. autofunction:: rsgislib.classification.flip_chip_hdf5_file
.. autofunction:: rsgislib.classification.flip_ref_chip_hdf5_file
.. autofunction:: rsgislib.classification.label_pxl_sample_chips


Utilities
-----------
.. autofunction:: rsgislib.classification.collapse_classes
.. autofunction:: rsgislib.classification.gen_rgb_img_from_clr_tbl

Accuracy Assessment
---------------------
.. autofunction:: rsgislib.classification.generate_random_accuracy_pts
.. autofunction:: rsgislib.classification.generate_stratified_random_accuracy_pts
.. autofunction:: rsgislib.classification.pop_class_info_accuracy_pts
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_metrics_vecsamples
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets
.. autofunction:: rsgislib.classification.classaccuracymetrics.summarise_multi_acc_ptonly_metrics
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_class_pt_accuracy_metrics
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_class_accuracy_metrics
.. autofunction:: rsgislib.classification.classaccuracymetrics.cls_quantity_accuracy

Classification Utility Classes
-------------------------------

.. autoclass:: rsgislib.classification.ClassSimpleInfoObj
.. autoclass:: rsgislib.classification.ClassInfoObj
.. autoclass:: rsgislib.classification.ClassVecSamplesInfoObj
.. autoclass:: rsgislib.classification.SamplesInfoObj

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

