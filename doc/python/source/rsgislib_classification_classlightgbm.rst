RSGISLib LightGBM Pixel Classification
========================================

LightGBM (https://lightgbm.readthedocs.io) is an alternative library to scikit-learn which has specialist implementation of Gradient Boosted Decision Tree (GBDT), but it also implements random forests, Dropouts meet Multiple Additive Regression Trees (DART), and Gradient Based One-Side Sampling (Goss).

When considering ensemble learning, there are two primary methods: bagging and boosting. Bagging involves the training of many independent models and combines their predictions through some form of aggregation (averaging, voting etc.). An example of a bagging ensemble is a Random Forest.

Boosting instead trains models sequentially, where each model learns from the errors of the previous model. Starting with a weak base model, models are trained iteratively, each adding to the prediction of the previous model to produce a strong overall prediction. In the case of gradient boosted decision trees, successive models are found by applying gradient descent in the direction of the average gradient, calculated with respect to the error residuals of the loss function, of the leaf nodes of previous models.

.. seealso:: For an easy to follow and understandable background to LightGBM `see this blog post <https://medium.com/@pushkarmandot/https-medium-com-pushkarmandot-what-is-lightgbm-how-to-implement-it-how-to-fine-tune-the-parameters-60347819b7fc>`_

.. seealso:: For an an academic paper see: Ke, G., Meng, Q., Finley, T., Wang, T., Chen, W., Ma, W., Ye, Q., Liu, T.LightGBM: A Highly Efficient Gradient Boosting Decision Tree. 31st Conference on Neural Information Processing Systems (NIPS 2017), Long Beach, CA, USA.

LightGBM is a binary classifier (i.e., separates two classes, e.g., mangroves and other) but it has a multi-class mode which applies a number of binary classification to produce a multi-class classification result.

**Steps to applying a LightGBM Classification:**

  * Extract training
  * Split training: Training, Validation, Testing
  * Train Classifier and Optimise Hyperparameters
  * Apply Classifier

To define training a raster with a unique value for each class, or multiple binary rasters one for each class. Commonly the training regions might be defined using a vector layer which would require rasterising::

    import rsgislib.vectorutils

    sen2_img = 'sen2_srefimg.kea'
    mangroves_sample_vec_file = 'mangrove_cls_samples.geojson'
    mangroves_sample_vec_lyr = 'mangrove_cls_samples'
    mangroves_sample_img = 'mangrove_cls_samples.kea'
    rsgislib.vectorutils.rasteriseVecLyr(mangroves_sample_vec_file, mangroves_sample_vec_lyr, sen2_img, mangroves_sample_img, gdalformat='KEA')

    other_sample_vec_file = 'other_cls_samples.geojson'
    other_sample_vec_lyr = 'other_cls_samples'
    other_sample_img = 'other_cls_samples.kea'
    rsgislib.vectorutils.rasteriseVecLyr(other_sample_vec_file, other_sample_vec_lyr, sen2_img, other_sample_img, gdalformat='KEA')


To extract the image pixel values, which are stored within a HDF5 file (see https://portal.hdfgroup.org/display/HDF5/HDF5 for more information) the following functions are used. To define the images and associated bands to be used for the classification and therefore values need to be extracted then a list of rsgislib.imageutils.ImageBandInfo classes needs to be provided::

    import rsgislib.imageutils

    imgs_info = []
    imgs_info.append(rsgislib.imageutils.ImageBandInfo(fileName='sen2_srefimg.kea', name='sen2', bands=[1,2,3,4,5,6,7,8,9,10]))
    imgs_info.append(rsgislib.imageutils.ImageBandInfo(fileName='sen1_dBimg.kea', name='sen1', bands=[1,2]))

    mangroves_sample_h5 = 'mangrove_cls_samples.h5'
    rsgislib.imageutils.extractZoneImageBandValues2HDF(imgs_info, mangroves_sample_img, mangroves_sample_h5, 1)

    other_sample_h5 = 'other_cls_samples.h5'
    rsgislib.imageutils.extractZoneImageBandValues2HDF(imgs_info, other_sample_img, other_sample_h5, 1)

If training data is extracted from multiple input images then it will need to be merged using the following function::

    rsgislib.imageutils.mergeExtractedHDF5Data(['mang_samples_1.h5', 'mang_samples_2.h5'], 'mangrove_cls_samples.h5')
    rsgislib.imageutils.mergeExtractedHDF5Data(['other_samples_1.h5', 'other_samples_2.h5'], 'other_cls_samples.h5')

To split the extracted samples into a training, validation and testing sets you can use the rsgislib.classification.split_sample_train_valid_test function::

    import rsgislib.classification

    mangroves_sample_h5_train = 'mangrove_cls_samples_train.h5'
    mangroves_sample_h5_valid = 'mangrove_cls_samples_valid.h5'
    mangroves_sample_h5_test = 'mangrove_cls_samples_test.h5'
    rsgislib.classification.split_sample_train_valid_test(mangroves_sample_h5, mangroves_sample_h5_train, mangroves_sample_h5_valid, mangroves_sample_h5_test, test_sample=500, valid_sample=500, train_sample=2000)

    other_sample_h5_train = 'other_cls_samples_train.h5'
    other_sample_h5_valid = 'other_cls_samples_valid.h5'
    other_sample_h5_test = 'other_cls_samples_test.h5'
    rsgislib.classification.split_sample_train_valid_test(other_sample_h5, other_sample_h5_train, other_sample_h5_valid, other_sample_h5_test, test_sample=500, valid_sample=500, train_sample=2000)

.. note::  Training samples are used to train the classifier. Validation samples are used to test the accuracy of the classifier during the parameter optimisation process and are therefore part of the training process and not independent. Testing samples completely independent of the training process and are used as an independent sample to test the overall accuracy of the classifier.

**Apply a LightGBM Binary Classifier**

To train a single binary classifier you need to use the following function::

    import rsgislib.classification
    import rsgislib.classification.classlightgbm

    out_mdl_file = 'model_file.txt'
    rsgislib.classification.classlightgbm.train_lightgbm_binary_classifier(out_mdl_file, mangroves_sample_h5_train, mangroves_sample_h5_valid, mangroves_sample_h5_test, other_sample_h5_train, other_sample_h5_valid, other_sample_h5_test)

To apply the binary classifier use the following function::

    img_mask = 'mangrove_habitat_img.kea'
    out_prob_img = 'mangrove_prob_img.kea'
    out_cls_img = 'mangrove_cls_img.kea'
    rsgislib.classification.classlightgbm.apply_lightgbm_binary_classifier(out_mdl_file, img_mask, 1, imgs_info, out_prob_img, 'KEA', out_cls_img, class_thres=5000)

.. note:: Class probability values are multipled by 10,000 so a threshold of 5000 is really 0.5.

**Apply a LightGBM Multi-Class Classifier**

To train a multi-class classifier you need to use the following function::

    import rsgislib.classification
    import rsgislib.classification.classlightgbm

    out_mdl_file = 'model_file.txt'
    clsinfodict = dict()
    clsinfodict['Mangroves'] = rsgislib.classification.ClassInfoObj(id=0, out_id=1, trainfileH5=mangroves_sample_h5_train, testfileH5=mangroves_sample_h5_test, validfileH5=mangroves_sample_h5_valid, red=0, green=255, blue=0)
    clsinfodict['Other'] = rsgislib.classification.ClassInfoObj(id=1, out_id=2, trainfileH5=other_sample_h5_train, testfileH5=other_sample_h5_test, validfileH5=other_sample_h5_valid, red=100, green=100, blue=100)
    # Note. Water samples not shown above but would be extracted and generated using the same functions.
    clsinfodict['Water'] = rsgislib.classification.ClassInfoObj(id=2, out_id=3, trainfileH5=water_sample_h5_train, testfileH5=water_sample_h5_test, validfileH5=water_sample_h5_valid,, red=0, green=0, blue=255)

    rsgislib.classification.classlightgbm.train_lightgbm_multiclass_classifier(out_mdl_file, clsinfodict)

To apply the multi-class classifier use the following function::

    img_mask = 'mangrove_habitat_img.kea'
    out_prob_img = 'class_prob_img.kea'
    out_cls_img = 'class_out_img.kea'

    rsgislib.classification.classlightgbm.apply_lightgbm_multiclass_classifier(clsinfodict, out_mdl_file, img_mask, 1, imgs_info, out_prob_img, out_cls_img, 'KEA')

.. note:: Within the rsgislib.classification.ClassInfoObj class you need to provide an id and out_id value. The id must start from zero and be consecutive while the out_id will be used as the pixel value for the output classification image and can be any integer value.

Binary Classification Functions
--------------------------------

Train Classifier
^^^^^^^^^^^^^^^^^^
.. autofunction:: rsgislib.classification.classlightgbm.optimise_lightgbm_binary_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_binary_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier

Apply Classifier
^^^^^^^^^^^^^^^^^^^^
.. autofunction:: rsgislib.classification.classlightgbm.apply_lightgbm_binary_classifier


Multi-Class Classification Functions
-------------------------------------

Train Classifier
^^^^^^^^^^^^^^^^^^
.. autofunction:: rsgislib.classification.classlightgbm.optimise_lightgbm_multiclass_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_multiclass_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_multiclass_classifier

Apply Classifier
^^^^^^^^^^^^^^^^^^^^
.. autofunction:: rsgislib.classification.classlightgbm.apply_lightgbm_multiclass_classifier
