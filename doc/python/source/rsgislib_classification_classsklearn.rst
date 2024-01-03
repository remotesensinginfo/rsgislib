RSGISLib Scikit-Learn Pixel Classification
=============================================

These functions allow a classifier from the scikit-learn (https://scikit-learn.org) library to be trained and applied on an individual image pixel basis. This requires a number of processing steps to be undertaken:

  * Extract training
  * Split training: Training, Validation, Testing
  * Train Classifier and Optimise Hyperparameters
  * Apply Classifier
 
However, fist we'll create a couple of directories for our outputs and intermediary files::

    import os

    out_dir = "baseline_cls_skl_rf"
    if not os.path.exists(out_dir):
        os.mkdir(out_dir)

    tmp_dir = "tmp_skl_rf"
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)

We will also define the input file path and the list ImageBandInfo objects, which specifies which images and bands are used for the analysis::

    import rsgislib.imageutils

    input_img = "./LS5TM_19970716_vmsk_mclds_topshad_rad_srefdem_stdsref_subset.tif"

    imgs_info = []
    imgs_info.append(
        rsgislib.imageutils.ImageBandInfo(
            file_name=input_img, name="ls97", bands=[1, 2, 3, 4, 5, 6]
        )
    )

When applying a classifier a mask image needs to be provided where a pixel value within that mask specifying which pixels should be classified. While defining the input image we can also define that valid mask image using the rsgislib.imageutils.gen_valid_mask function, which simply creates a mask of pixels which are not 'no data'::

    vld_msk_img = os.path.join(out_dir, "LS5TM_19970716_vmsk.kea")
    rsgislib.imageutils.gen_valid_mask(
        input_img, output_img=vld_msk_img, gdalformat="KEA", no_data_val=0.0
    )

To define training a raster with a unique value for each class, or multiple binary rasters one for each class. Commonly the training regions might be defined using a vector layer which would require rasterising::

    import rsgislib.vectorutils.createrasters

    mangrove_vec_file = "./training/mangroves.geojson"
    mangrove_vec_lyr = "mangroves"
    mangrove_smpls_img = os.path.join(tmp_dir, "mangrove_smpls.kea")
    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_file=mangrove_vec_file,
        vec_lyr=mangrove_vec_lyr,
        input_img=input_img,
        output_img=mangrove_smpls_img,
        gdalformat="KEA",
        burn_val=1,
    )

    other_terrestrial_vec_file = "./training/other_terrestrial.geojson"
    other_terrestrial_vec_lyr = "other_terrestrial"
    other_terrestrial_smpls_img = os.path.join(tmp_dir, "other_terrestrial_smpls.kea")
    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_file=other_terrestrial_vec_file,
        vec_lyr=other_terrestrial_vec_lyr,
        input_img=input_img,
        output_img=other_terrestrial_smpls_img,
        gdalformat="KEA",
        burn_val=1,
    )

    water_vec_file = "./training/water.geojson"
    water_vec_lyr = "water"
    water_smpls_img = os.path.join(tmp_dir, "water_smpls.kea")
    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_file=water_vec_file,
        vec_lyr=water_vec_lyr,
        input_img=input_img,
        output_img=water_smpls_img,
        gdalformat="KEA",
        burn_val=1,
    )


To extract the image pixel values, which are stored within a HDF5 file (see https://portal.hdfgroup.org/display/HDF5/HDF5 for more information) the following functions are used. To define the images and associated bands to be used for the classification and therefore values need to be extracted then a list of rsgislib.imageutils.ImageBandInfo classes needs to be provided::

    import rsgislib.zonalstats

    mangrove_all_smpls_h5_file = os.path.join(out_dir, "mangrove_all_smpls.h5")
    rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(
        imgs_info,
        in_msk_img=mangrove_smpls_img,
        out_h5_file=mangrove_all_smpls_h5_file,
        mask_val=1,
        datatype=rsgislib.TYPE_16UINT,
    )

    other_terrestrial_all_smpls_h5_file = os.path.join(
        out_dir, "other_terrestrial_all_smpls.h5"
    )
    rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(
        imgs_info,
        in_msk_img=other_terrestrial_smpls_img,
        out_h5_file=other_terrestrial_all_smpls_h5_file,
        mask_val=1,
        datatype=rsgislib.TYPE_16UINT,
    )

    water_all_smpls_h5_file = os.path.join(out_dir, "water_all_smpls.h5")
    rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(
        imgs_info,
        in_msk_img=water_smpls_img,
        out_h5_file=water_all_smpls_h5_file,
        mask_val=1,
        datatype=rsgislib.TYPE_16UINT,
    )



If training data is extracted from multiple input images then it will need to be merged using the following function. In this case, for illustration we'll merge the water and terrestrial samples::

    other_all_smpls_h5_file = os.path.join(out_dir, "other_all_smpls.h5")
    rsgislib.zonalstats.merge_extracted_hdf5_data(
        h5_files=[other_terrestrial_all_smpls_h5_file, water_all_smpls_h5_file],
        out_h5_file=other_all_smpls_h5_file,
        datatype=rsgislib.TYPE_16UINT,
    )

To split the extracted samples into a training, validation and testing sets you can use the rsgislib.classification.split_sample_train_valid_test function. Note, this function is also used to standardise the number of samples used to train the classifier so the training data are balanced::

    import rsgislib.classification

    mangrove_train_smpls_h5_file = os.path.join(out_dir, "mangrove_train_smpls.h5")
    mangrove_valid_smpls_h5_file = os.path.join(out_dir, "mangrove_valid_smpls.h5")
    mangrove_test_smpls_h5_file = os.path.join(out_dir, "mangrove_test_smpls.h5")
    rsgislib.classification.split_sample_train_valid_test(
        in_h5_file=mangrove_all_smpls_h5_file,
        train_h5_file=mangrove_train_smpls_h5_file,
        valid_h5_file=mangrove_valid_smpls_h5_file,
        test_h5_file=mangrove_test_smpls_h5_file,
        test_sample=10000,
        valid_sample=10000,
        train_sample=35000,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16UINT,
    )


    other_terrestrial_train_smpls_h5_file = os.path.join(
        out_dir, "other_terrestrial_train_smpls.h5"
    )
    other_terrestrial_valid_smpls_h5_file = os.path.join(
        out_dir, "other_terrestrial_valid_smpls.h5"
    )
    other_terrestrial_test_smpls_h5_file = os.path.join(
        out_dir, "other_terrestrial_test_smpls.h5"
    )
    rsgislib.classification.split_sample_train_valid_test(
        in_h5_file=other_terrestrial_all_smpls_h5_file,
        train_h5_file=other_terrestrial_train_smpls_h5_file,
        valid_h5_file=other_terrestrial_valid_smpls_h5_file,
        test_h5_file=other_terrestrial_test_smpls_h5_file,
        test_sample=10000,
        valid_sample=10000,
        train_sample=35000,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16UINT,
    )


    water_train_smpls_h5_file = os.path.join(out_dir, "water_train_smpls.h5")
    water_valid_smpls_h5_file = os.path.join(out_dir, "water_valid_smpls.h5")
    water_test_smpls_h5_file = os.path.join(out_dir, "water_test_smpls.h5")
    rsgislib.classification.split_sample_train_valid_test(
        in_h5_file=water_all_smpls_h5_file,
        train_h5_file=water_train_smpls_h5_file,
        valid_h5_file=water_valid_smpls_h5_file,
        test_h5_file=water_test_smpls_h5_file,
        test_sample=10000,
        valid_sample=10000,
        train_sample=35000,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16UINT,
    )

.. note::  Training samples are used to train the classifier. Validation samples are used to test the accuracy of the classifier during the parameter optimisation process and are therefore part of the training process and not independent. Testing samples completely independent of the training process and are used as an independent sample to test the overall accuracy of the classifier.

**Apply a Scikit-Learn Random Forests Classifier**

To train a multi-class classifier you first need to specify the reference samples as a dict of rsgislib.classification.ClassInfoObj objects::

    import rsgislib.classification

    cls_info_dict = dict()
    cls_info_dict["Mangrove"] = rsgislib.classification.ClassInfoObj(
        id=0,
        out_id=1,
        train_file_h5=mangrove_train_smpls_h5_file,
        test_file_h5=mangrove_test_smpls_h5_file,
        valid_file_h5=mangrove_valid_smpls_h5_file,
        red=0,
        green=255,
        blue=0,
    )
    cls_info_dict["Other Terrestrial"] = rsgislib.classification.ClassInfoObj(
        id=1,
        out_id=2,
        train_file_h5=other_terrestrial_train_smpls_h5_file,
        test_file_h5=other_terrestrial_test_smpls_h5_file,
        valid_file_h5=other_terrestrial_valid_smpls_h5_file,
        red=100,
        green=100,
        blue=100,
    )
    cls_info_dict["Water"] = rsgislib.classification.ClassInfoObj(
        id=2,
        out_id=3,
        train_file_h5=water_train_smpls_h5_file,
        test_file_h5=water_test_smpls_h5_file,
        valid_file_h5=water_valid_smpls_h5_file,
        red=0,
        green=0,
        blue=255,
    )


To train the Random Forest classifier we need to first optimise the algorithm
parameters. For this we'll use a Grid Search::

    import rsgislib.classification.classsklearn
    from sklearn.ensemble import RandomForestClassifier
    from sklearn.model_selection import GridSearchCV

    grid_search = GridSearchCV(
        RandomForestClassifier(),
        param_grid={"n_estimators": [10, 20, 50, 100], "max_depth": [2, 4, 8]},
    )
    skl_rf_clf_obj = (
        rsgislib.classification.classsklearn.perform_sklearn_classifier_param_search(
            cls_train_info=cls_info_dict, search_obj=grid_search
        )
    )

Once we have an instance of the classifier with the optimal parameters we can then train the classifier::

    rsgislib.classification.classsklearn.train_sklearn_classifier(
        cls_train_info=cls_info_dict, sk_classifier=skl_rf_clf_obj
    )

Using the trained classifier we can then applied it to the image data::

    out_cls_img = os.path.join(out_dir, "LS5TM_19970716_skl_rf_cls_img.kea")
    out_score_img = os.path.join(out_dir, "LS5TM_19970716_skl_rf_cls_score_img.kea")
    rsgislib.classification.classsklearn.apply_sklearn_classifier(
        cls_train_info=cls_info_dict,
        sk_classifier=skl_rf_clf_obj,
        in_msk_img=vld_msk_img,
        img_msk_val=1,
        img_file_info=imgs_info,
        out_class_img=out_cls_img,
        gdalformat="KEA",
        class_clr_names=True,
        out_score_img=out_score_img,
        ignore_consec_cls_ids=False,
    )

The output image file name needs to be defined and an image mask also needs to provided which defines the parts of the image to be classified. This is useful as by using a previous classification result as the mask for another classifier a hierarchical classification process could be built.


Training Functions
-------------------

.. autofunction:: rsgislib.classification.classsklearn.perform_sklearn_classifier_param_search
.. autofunction:: rsgislib.classification.classsklearn.train_sklearn_classifier

Classify Functions
-------------------

.. autofunction:: rsgislib.classification.classsklearn.apply_sklearn_classifier
.. autofunction:: rsgislib.classification.classsklearn.apply_sklearn_classifier_rat


Other Utility Functions
---------------------------

.. autofunction:: rsgislib.classification.classsklearn.feat_sel_sklearn_multiclass_borutashap
