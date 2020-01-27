RSGISLib Keras Pixel Classification Module
===========================================

.. automodule:: rsgislib.classification.classkeraspxl

While per-pixel neural network based classifers can be created using the scikit-learn library the `Keras <https://keras.io>`_ library provide many more options and flexibility for the creation of a neural network.

.. note:: While these functions have been provided for completeness. Results from `Kaggle <https://www.kaggle.com>`_ competitions would indicate that for these type of problems that classifiers such as Gradient Boosted Decision Tree (i.e., LightGBM) will probably produce the best results. However, deep learning neural networks will probably produce the best classification for images where a window/chip of data are provided. Although, these image chip based classifiers are considerably more computational complex.

**Steps to applying a Classification:**

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

    water_sample_vec_file = 'water_cls_samples.geojson'
    water_sample_vec_lyr = 'water_cls_samples'
    water_sample_img = 'water_cls_samples.kea'
    rsgislib.vectorutils.rasteriseVecLyr(water_sample_vec_file, water_sample_vec_lyr, sen2_img, water_sample_img, gdalformat='KEA')


To extract the image pixel values, which are stored within a HDF5 file (see https://portal.hdfgroup.org/display/HDF5/HDF5 for more information) the following functions are used. To define the images and associated bands to be used for the classification and therefore values need to be extracted then a list of rsgislib.imageutils.ImageBandInfo classes needs to be provided::

    import rsgislib.imageutils

    imgs_info = []
    imgs_info.append(rsgislib.imageutils.ImageBandInfo(fileName='sen2_srefimg.kea', name='sen2', bands=[1,2,3,4,5,6,7,8,9,10]))
    imgs_info.append(rsgislib.imageutils.ImageBandInfo(fileName='sen1_dBimg.kea', name='sen1', bands=[1,2]))

    mangroves_sample_h5 = 'mangrove_cls_samples.h5'
    rsgislib.imageutils.extractZoneImageBandValues2HDF(imgs_info, mangroves_sample_img, mangroves_sample_h5, 1)

    other_sample_h5 = 'other_cls_samples.h5'
    rsgislib.imageutils.extractZoneImageBandValues2HDF(imgs_info, other_sample_img, other_sample_h5, 1)

    water_sample_h5 = 'water_cls_samples.h5'
    rsgislib.imageutils.extractZoneImageBandValues2HDF(imgs_info, water_sample_img, water_sample_h5, 1)

If training data is extracted from multiple input images then it will need to be merged using the following function::

    rsgislib.imageutils.mergeExtractedHDF5Data(['mang_samples_1.h5', 'mang_samples_2.h5'], 'mangrove_cls_samples.h5')
    rsgislib.imageutils.mergeExtractedHDF5Data(['other_samples_1.h5', 'other_samples_2.h5'], 'other_cls_samples.h5')
    rsgislib.imageutils.mergeExtractedHDF5Data(['water_samples_1.h5', 'water_samples_2.h5'], 'water_cls_samples.h5')

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

    water_sample_h5_train = 'water_cls_samples_train.h5'
    water_sample_h5_valid = 'water_cls_samples_valid.h5'
    water_sample_h5_test = 'water_cls_samples_test.h5'
    rsgislib.classification.split_sample_train_valid_test(water_sample_h5, water_sample_h5_train, water_sample_h5_valid, water_sample_h5_test, test_sample=500, valid_sample=500, train_sample=2000)

.. note::  Training samples are used to train the classifier. Validation samples are used to test the accuracy of the classifier during the parameter optimisation process and are therefore part of the training process and not independent. Testing samples completely independent of the training process and are used as an independent sample to test the overall accuracy of the classifier.

Defining a neural network structure::

    from keras.models import Sequential
    from keras.layers import Dense, Activation

    cls_mdl = Sequential()
    # The input_dim must be the same as the number of image bands used for the classification
    cls_mdl.add(Dense(32, activation='relu', input_dim=12))
    cls_mdl.add(Dense(16, activation='relu'))
    cls_mdl.add(Dense(8, activation='relu'))
    cls_mdl.add(Dense(32, activation='relu'))
    # the final layer of the network must use softmax activation and the size must be the same as the number of classes (i.e., 3)
    cls_mdl.add(Dense(3, activation='softmax'))
    cls_mdl.compile(optimizer='rmsprop', loss='categorical_crossentropy', metrics=['accuracy'])

.. note:: There are many optimizers, activation functions and structures of networks which could be used to create the network. You will need to do some reading and experimentation to understand what to use and what impact it has on the results.

Training the neural network::

    import rsgislib.classification
    rsgislib.classification.classkeraspxl

    clsinfodict = dict()
    clsinfodict['Mangroves'] = rsgislib.classification.ClassInfoObj(id=0, out_id=1, trainfileH5=mangroves_sample_h5_train, testfileH5=mangroves_sample_h5_test, validfileH5=mangroves_sample_h5_valid, red=0, green=255, blue=0)
    clsinfodict['Other'] = rsgislib.classification.ClassInfoObj(id=1, out_id=2, trainfileH5=other_sample_h5_train, testfileH5=other_sample_h5_test, validfileH5=other_sample_h5_valid, red=100, green=100, blue=100)
    # Note. Water samples not shown above but would be extracted and generated using the same functions.
    clsinfodict['Water'] = rsgislib.classification.ClassInfoObj(id=2, out_id=3, trainfileH5=water_sample_h5_train, testfileH5=water_sample_h5_test, validfileH5=water_sample_h5_valid, red=0, green=0, blue=255)

    out_mdl_file = 'basic_pxl_mdl.h5'

    rsgislib.classification.classkeraspxl.train_keras_pixel_classifer(cls_mdl, clsinfodict, out_mdl_file)

The following code allows you to re-opening the model you have trained, it would be common for the training phase to take a long period of time (hours, even days!!) so you will likely have separate training and application scripts::

    from keras.models import load_model
    cls_mdl = load_model('basic_pxl_mdl.h5')

Apply the classifier to scene::

    img_mask = 'mangrove_habitat_img.kea'
    out_cls_img = 'class_out_img.kea'
    rsgislib.classification.classkeraspxl.apply_keras_pixel_classifier(clsinfodict, cls_mdl, img_mask, 1, imgs_info, out_cls_img, 'KEA')

Training Functions
--------------------

.. autofunction:: rsgislib.classification.classkeraspxl.train_keras_pixel_classifer

Classify Functions
-------------------

.. autofunction:: rsgislib.classification.classkeraspxl.apply_keras_pixel_classifier




