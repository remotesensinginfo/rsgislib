RSGISLib Keras Image Chips Classification
============================================

These functions are first attempts and connecting spatial image data with window based (convolutional) neural networks. These functions and other will be developed as our research and understanding of these methods move forward.

The application is similar to the RSGISLib classification modules. However, custom functions have been created to extract image training data and you need to keep in mind some of things about the network structure to fit to the shape/order of the data extracted.

**Get Sample Data**

Training data is defined using a raster image where for each pixel defined as the training data for a given class a window centred on that pixel will be extracted.

If that raster is already available then you can use the rsgislib.imageutils.extractChipZoneImageBandValues2HDF function to extract the training data to a HDF5 file::

    import rsgislib.imageutils

    chip_h_size = 5

    imgBandInfo = []
    imgBandInfo.append(rsgislib.imageutils.ImageBandInfo('sen2_refl_img.kea', 'sen2', [7,9,3]))

    mangrove_train_pxls_img = 'mangrove_sample_pxls.kea'
    mangrove_train_chips_h5 = 'mangrove_sample_chips.h5'
    rsgislib.imageutils.extractChipZoneImageBandValues2HDF(imgBandInfo, mangrove_train_pxls_img, 1, chip_h_size, mangrove_train_chips_h5)

.. note:: The chip_h_size is half the size of the chip extracted. The chip size will always be an odd size, for example an input value of 5 will result in an output chip size of 11. A value of 10 a chip size of 21.

If vector layers are being used to define the training data then the helper function rsgislib.classification.get_class_training_chips_data can be used::

    import rsgislib.classification
    import rsgislib.imageutils

    chip_h_size = 5

    mangrove_samples = 'class_samples_mangrove.h5'
    other_samples = 'class_samples_other.h5'
    water_samples = 'class_samples_water.h5'

    classVecSampleInfo = []
    classVecSampleInfo.append(rsgislib.classification.ClassVecSamplesInfoObj(id=1, classname="Mangrove", vecfile="class_samples_mangrove.geojson", veclyr="class_samples_mangrove", fileH5=mangrove_samples))
    classVecSampleInfo.append(rsgislib.classification.ClassVecSamplesInfoObj(id=2, classname="Other", vecfile="class_samples_other.geojson", veclyr="class_samples_other", fileH5=other_samples))
    classVecSampleInfo.append(rsgislib.classification.ClassVecSamplesInfoObj(id=3, classname="Water", vecfile="class_samples_water.geojson", veclyr="class_samples_water", fileH5=water_samples))

    imgBandInfo = []
    imgBandInfo.append(rsgislib.imageutils.ImageBandInfo('sen2_refl_img.kea', 'sen2', [7,9,3]))

    tmp_path = './tmp'

    rsgislib.classification.get_class_training_chips_data(imgBandInfo, classVecSampleInfo, chip_h_size, tmp_path)

To split those input samples into training, validation and training data then the rsgislib.classification.split_chip_sample_train_valid_test function is used::

    mangrove_samples_train = 'class_samples_mangrove_train.h5'
    mangrove_samples_valid = 'class_samples_mangrove_valid.h5'
    mangrove_samples_test = 'class_samples_mangrove_test.h5'
    rsgislib.classification.split_chip_sample_train_valid_test(mangrove_samples, mangrove_samples_train, mangrove_samples_valid, mangrove_samples_test, 1000, 1000, 5000)

    other_samples_train = 'class_samples_other_train.h5'
    other_samples_valid = 'class_samples_other_valid.h5'
    other_samples_test = 'class_samples_other_test.h5'
    rsgislib.classification.split_chip_sample_train_valid_test(other_samples, other_samples_train, other_samples_valid, other_samples_test, 1000, 1000, 5000)

    water_samples_train = 'class_samples_water_train.h5'
    water_samples_valid = 'class_samples_water_valid.h5'
    water_samples_test = 'class_samples_water_test.h5'
    rsgislib.classification.split_chip_sample_train_valid_test(water_samples, water_samples_train, water_samples_valid, water_samples_test, 1000, 1000, 5000)

**Training Classifer**

First you need to define a classifier using Keras::

    import keras.optimizers
    import keras.losses
    from keras.models import Sequential
    from keras.layers import Dense, Conv2D, MaxPooling2D, Dropout, Flatten, Activation

    # input_shape: [n_bands, chip_size, chip_size]
    # chip_size = (chip_h_size * 2) + 1
    # data_format must be 'channels_first' (i.e., bands first)

    # THIS CLASSIFIER IS NOT AN EXAMPLE OF A GOOD CLASSIFIER STRUCTURE - DO NOT COPY AND USE!!
    cls_mdl = Sequential()
    cls_mdl.add(Conv2D(filters=32, kernel_size=(3,3), strides=(1,1), padding='same', input_shape=[3, 11, 11], data_format='channels_first'))
    cls_mdl.add(Activation('relu'))
    cls_mdl.add(MaxPooling2D(pool_size=(2,2), strides=1))
    cls_mdl.add(Flatten())
    cls_mdl.add(Dense(32))
    cls_mdl.add(Activation('relu'))
    cls_mdl.add(Dropout(0.25))
    cls_mdl.add(Dense(32))
    cls_mdl.add(Activation('relu'))
    cls_mdl.add(Dense(8))
    cls_mdl.add(Activation('relu'))
    # The final layer must be the same size as the number of classes and use a softmax activation.
    cls_mdl.add(Dense(3, activation='softmax'))

    cls_mdl.compile(optimizer=keras.optimizers.Adam(), loss=keras.losses.categorical_crossentropy, metrics=['accuracy'])

The classification model can then be trained, note this can take a lot of time (hours/days) and it might be recommended that you have your system setup so Keras/Tensorflow can use the GPU for processing::

    import rsgislib.classification.classkeraschips

    clsinfodict = dict()
    clsinfodict['Mangroves'] = rsgislib.classification.ClassInfoObj(id=0, out_id=1, trainfileH5=mangroves_samples_train, testfileH5=mangroves_samples_test, validfileH5=mangroves_samples_valid, red=0, green=255, blue=0)
    clsinfodict['Other'] = rsgislib.classification.ClassInfoObj(id=1, out_id=2, trainfileH5=other_samples_train, testfileH5=other_samples_test, validfileH5=other_samples_valid, red=100, green=100, blue=100)
    clsinfodict['Water'] = rsgislib.classification.ClassInfoObj(id=2, out_id=3, trainfileH5=water_samples_train, testfileH5=water_samples_test, validfileH5=water_samples_valid, red=0, green=0, blue=255)

    out_mdl_file = 'basic_chip_mdl.h5'

    rsgislib.classification.classkeraschips.train_keras_chips_pixel_classifier(cls_mdl, clsinfodict, out_mdl_file)

**Apply Classifier**

The following code allows you to re-opening the model you have trained, it would be common for the training phase to take a long period of time (hours, even days!!) so you will likely have separate training and application scripts::

    from keras.models import load_model
    cls_mdl = load_model('basic_chip_mdl.h5')

Apply the classifier to scene::

    img_mask = 'mangrove_habitat_img.kea'
    out_cls_img = 'class_out_img.kea'
    rsgislib.classification.classkeraspxl.apply_keras_pixel_classifier(clsinfodict, cls_mdl, img_mask, 1, imgs_info, chip_h_size, out_cls_img, 'KEA')


Training Functions
-------------------

.. autofunction:: rsgislib.classification.classkeraschips.train_keras_chips_pixel_classifier

Classify Functions
-------------------

.. autofunction:: rsgislib.classification.classkeraschips.apply_keras_chips_pixel_classifier
.. autofunction:: rsgislib.classification.classkeraschips.train_keras_chips_ref_classifier






