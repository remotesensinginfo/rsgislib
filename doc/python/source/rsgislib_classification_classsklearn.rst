RSGISLib Scikit-Learn Pixel Classification
=============================================

These functions allow a classifier from the scikit-learn (https://scikit-learn.org) library to be trained and applied on an individual image pixel basis. This requires a number of processing steps to be undertaken:

 * Define training pixels
 * Extract training pixels
 * Train classifier
 * Apply classifier
 
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
    
The data then needs splitting into training and testing datasets. The training data should also normally be balanced so there is the same number of samples per class::
    
    mangroves_sample_train_h5 = 'mangrove_cls_samples_train.h5'
    mangroves_sample_test_h5 = 'mangrove_cls_samples_test.h5'
    rsgislib.imageutils.splitSampleHDF5File(mangroves_sample_h5, mangroves_sample_train_h5, mangroves_sample_test_h5, 1000, 42)
    
    other_sample_train_h5 = 'other_cls_samples_train.h5'
    other_sample_test_h5 = 'other_cls_samples_test.h5'
    rsgislib.imageutils.splitSampleHDF5File(other_sample_h5, other_sample_train_h5, other_sample_test_h5, 1000, 42)


The classifier now needs training, so import rsgislib.classification.classsklearn modules::

    import rsgislib.classification
    import rsgislib.classification.classsklearn
    
You then have two options for training, providing the parameters yourself or performing a grid search to find the optimal parameters for the classifier given the input data.

To train a classifier with parameters defined by yourself then you would use the follow code, we will use the random forests classifier but you can use any other classifier from the scikit-learn library::

    from sklearn.ensemble import RandomForestClassifier
    skclf = RandomForestClassifier(n_estimators=100)

    cls_train_info = dict()
    cls_train_info['Mangroves'] = rsgislib.classification.ClassSimpleInfoObj(id=1, fileH5='mangrove_cls_samples_train.h5', red=0, green=255, blue=0)
    cls_train_info['Other'] = rsgislib.classification.ClassSimpleInfoObj(id=2, fileH5='other_cls_samples_train.h5', red=100, green=100, blue=100)

    rsgislib.classification.classsklearn.train_sklearn_classifier(cls_train_info, skclf)

To train a classifier using a grid search you need to define the classifier parameters to be searched and a range of valid values for those parameters, the optimal trained classifier (using all the training data) will be returned by the function::

    from sklearn.ensemble import RandomForestClassifier
    from sklearn.model_selection import GridSearchCV

    cls_train_info = dict()
    cls_train_info['Mangroves'] = rsgislib.classification.ClassSimpleInfoObj(id=1, fileH5='mangrove_cls_samples_train.h5', red=0, green=255, blue=0)
    cls_train_info['Other'] = rsgislib.classification.ClassSimpleInfoObj(id=2, fileH5='other_cls_samples_train.h5', red=100, green=100, blue=100)

    grid_search = GridSearchCV(RandomForestClassifier(), param_grid={'n_estimators':[10,20,50,100], 'max_depth':[2,4,8]})
    skclf = rsgislib.classification.classsklearn.train_sklearn_classifier_gridsearch(cls_train_info, 500, grid_search)

To apply trained classifier you need to use the following function::

    out_cls_img = 'mangrove_classification_result.kea'
    img_msk = 'valid_area_msk.kea'
    rsgislib.classification.classsklearn.apply_sklearn_classifier(cls_train_info, skclf, img_msk, 1, imgs_info, out_cls_img, 'KEA', classClrNames=True)

The output image file name needs to be defined and an image mask also needs to provided which defines the parts of the image to be classified. This is useful as by using a previous classification result as the mask for another classifier a hierarchical classification process could be built.


Training Functions
-------------------

.. autofunction:: rsgislib.classification.classsklearn.perform_sklearn_classifier_param_search
.. autofunction:: rsgislib.classification.classsklearn.train_sklearn_classifier

Classify Functions
-------------------

.. autofunction:: rsgislib.classification.classsklearn.apply_sklearn_classifier
.. autofunction:: rsgislib.classification.classsklearn.apply_sklearn_classifier_rat


