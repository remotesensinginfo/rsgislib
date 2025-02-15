RSGISLib Classification
===========================

The classification module has functions which allows classifiers to be applied to image data, either on a per pixel based or following an image segmentation and the classification of the resultant segments/clumps/objects.

The classification functions are available within a number of sub-modules for interfacing with different libraries and methods:

.. toctree::
   :maxdepth: 3

   rsgislib_classification_classsklearn
   rsgislib_classification_classlightgbm
   rsgislib_classification_classxgboost
   rsgislib_classification_classcatboost
   rsgislib_classification_classkeraspxl
   rsgislib_classification_classkeraschips
   rsgislib_classification_classratutils
   rsgislib_classification_clustersklearn
   rsgislib_classification_classimblearn


This rsgislib.classification module provides functions for dealing with training data, undertaking an accuracy assessment and other useful utilities, see below.


Pixel Training Data
-------------------
.. autofunction:: rsgislib.classification.get_class_training_data
.. autofunction:: rsgislib.classification.split_sample_train_valid_test
.. autofunction:: rsgislib.classification.create_train_valid_test_sets
.. autofunction:: rsgislib.classification.get_class_info_dict
.. autofunction:: rsgislib.classification.get_num_samples
.. autofunction:: rsgislib.classification.plot_train_data
.. autofunction:: rsgislib.classification.convert_cls_smpls_to_pandas_df
.. autofunction:: rsgislib.classification.convert_mutli_cls_smpls_to_pandas_df


Chips Training Data
--------------------
.. autofunction:: rsgislib.classification.get_class_training_chips_data
.. autofunction:: rsgislib.classification.split_chip_sample_train_valid_test
.. autofunction:: rsgislib.classification.split_chip_sample_ref_train_valid_test
.. autofunction:: rsgislib.classification.flip_chip_hdf5_file
.. autofunction:: rsgislib.classification.flip_ref_chip_hdf5_file
.. autofunction:: rsgislib.classification.label_pxl_sample_chips

Post Classification Refinement
--------------------------------
.. autofunction:: rsgislib.classification.fill_class_timeseries


Utilities
-----------
.. autofunction:: rsgislib.classification.collapse_classes
.. autofunction:: rsgislib.classification.gen_rgb_img_from_clr_tbl

Accuracy Assessment Samples
----------------------------

.. autofunction:: rsgislib.classification.classrefpts.create_random_ref_smpls_darts
.. autofunction:: rsgislib.classification.classrefpts.create_stratified_random_ref_smpls_darts
.. autofunction:: rsgislib.classification.generate_random_accuracy_pts
.. autofunction:: rsgislib.classification.generate_stratified_random_accuracy_pts
.. autofunction:: rsgislib.classification.generate_stratified_prop_random_accuracy_pts
.. autofunction:: rsgislib.classification.pop_class_info_accuracy_pts
.. autofunction:: rsgislib.classification.create_acc_pt_sets
.. autofunction:: rsgislib.classification.classaccuracymetrics.create_modelled_acc_pts
.. autofunction:: rsgislib.classification.classaccuracymetrics.create_norm_modelled_err_matrix

Accuracy Assessment Stats
----------------------------
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_metrics_vecsamples
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_metrics_vecsamples_img
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets
.. autofunction:: rsgislib.classification.classaccuracymetrics.summarise_multi_acc_ptonly_metrics
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_class_pt_accuracy_metrics
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_class_accuracy_metrics
.. autofunction:: rsgislib.classification.classaccuracymetrics.cls_quantity_accuracy
.. autofunction:: rsgislib.classification.classaccuracymetrics.calc_sampled_acc_metrics

Classification Utility Classes
-------------------------------

.. autoclass:: rsgislib.classification.ClassSimpleInfoObj
.. autoclass:: rsgislib.classification.ClassInfoObj
.. autoclass:: rsgislib.classification.ClassVecSamplesInfoObj
.. autoclass:: rsgislib.classification.SamplesInfoObj

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
