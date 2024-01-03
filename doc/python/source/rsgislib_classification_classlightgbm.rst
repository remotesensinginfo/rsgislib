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

However, fist we'll create a couple of directories for our outputs and intermediary files::

    import os

    out_dir = "baseline_cls_lgbm"
    if not os.path.exists(out_dir):
        os.mkdir(out_dir)

    tmp_dir = "tmp_lgbm"
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

If training data is extracted from multiple input images then it will need to be merged using the following function. In this case we'll merge the water and terrestrial samples and use the merged class to create a mangrove binary classifier::

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


    other_train_smpls_h5_file = os.path.join(out_dir, "other_train_smpls.h5")
    other_valid_smpls_h5_file = os.path.join(out_dir, "other_valid_smpls.h5")
    other_test_smpls_h5_file = os.path.join(out_dir, "other_test_smpls.h5")
    rsgislib.classification.split_sample_train_valid_test(
        in_h5_file=other_all_smpls_h5_file,
        train_h5_file=other_train_smpls_h5_file,
        valid_h5_file=other_valid_smpls_h5_file,
        test_h5_file=other_test_smpls_h5_file,
        test_sample=10000,
        valid_sample=10000,
        train_sample=35000,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16UINT,
    )


.. note::  Training samples are used to train the classifier. Validation samples are used to test the accuracy of the classifier during the parameter optimisation process and are therefore part of the training process and not independent. Testing samples completely independent of the training process and are used as an independent sample to test the overall accuracy of the classifier.



**Apply a LightGBM Binary Classifier**

To train a single binary classifier you need to use the following function::

    import rsgislib.classification.classlightgbm

    cls_bin_mdl_file = os.path.join(out_dir, "lgbm_mng_bin_mdl.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier(
        out_mdl_file=cls_bin_mdl_file,
        cls1_train_file=mangrove_train_smpls_h5_file,
        cls1_valid_file=mangrove_valid_smpls_h5_file,
        cls1_test_file=mangrove_test_smpls_h5_file,
        cls2_train_file=other_train_smpls_h5_file,
        cls2_valid_file=other_valid_smpls_h5_file,
        cls2_test_file=other_test_smpls_h5_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYESOPT,
        n_opt_iters=100,
        rnd_seed=42,
        n_threads=1,
        scale_pos_weight=None,
        early_stopping_rounds=None,
        num_iterations=100,
        max_n_leaves=50,
        learning_rate=0.1,
        mdl_cls_obj=None,
        out_params_file=None,
    )

To apply the binary classifier use the following function::

    cls_score_img = os.path.join(out_dir, "LS5TM_19970716_bin_cls_score_img.kea")
    out_class_img = os.path.join(out_dir, "LS5TM_19970716_bin_cls_img.kea")
    rsgislib.classification.classlightgbm.apply_lightgbm_binary_classifier(
        model_file=cls_bin_mdl_file,
        in_msk_img=vld_msk_img,
        img_msk_val=1,
        img_file_info=imgs_info,
        out_score_img=cls_score_img,
        gdalformat="KEA",
        out_class_img=out_class_img,
        class_thres=5000,
    )


.. note:: Class probability values are multipled by 10,000 so a threshold of 5000 is really 0.5.

**Apply a LightGBM Multi-Class Classifier**

To train a multi-class classifier you first need to specify the reference samples as a dict of rsgislib.classification.ClassInfoObj objects::

    import rsgislib.classification
    import rsgislib.classification.classlightgbm

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

You can then train a multi-class lightgbm classifier using the following function::

    import rsgislib.classification.classlightgbm

    cls_mcls_mdl_file = os.path.join(out_dir, "lgbm_mng_mcls_mdl.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_multiclass_classifier(
        out_mdl_file=cls_mcls_mdl_file,
        cls_info_dict=cls_info_dict,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYESOPT,
        n_opt_iters=100,
        rnd_seed=42,
        n_threads=1,
        early_stopping_rounds=None,
        num_iterations=100,
        max_n_leaves=50,
        learning_rate=0.1,
        mdl_cls_obj=None,
        out_params_file=None,
        out_info_file=None,
    )

To apply the multi-class classifier use the following function::

    out_class_img = os.path.join(out_dir, "LS5TM_19970716_mcls_img.kea")
    rsgislib.classification.classlightgbm.apply_lightgbm_multiclass_classifier(
        model_file=cls_mcls_mdl_file,
        cls_info_dict=cls_info_dict,
        in_msk_img=vld_msk_img,
        img_msk_val=1,
        img_file_info=imgs_info,
        out_class_img=out_class_img,
        gdalformat="KEA",
        class_clr_names=True,
    )

.. note:: Within the rsgislib.classification.ClassInfoObj class you need to provide an id and out_id value. The id must start from zero and be consecutive while the out_id will be used as the pixel value for the output classification image and can be any integer value.

Binary Classification Functions
--------------------------------

.. autofunction:: rsgislib.classification.classlightgbm.optimise_lightgbm_binary_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_binary_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier
.. autofunction:: rsgislib.classification.classlightgbm.apply_lightgbm_binary_classifier


Multi-Class Classification Functions
-------------------------------------

.. autofunction:: rsgislib.classification.classlightgbm.optimise_lightgbm_multiclass_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_lightgbm_multiclass_classifier
.. autofunction:: rsgislib.classification.classlightgbm.train_opt_lightgbm_multiclass_classifier
.. autofunction:: rsgislib.classification.classlightgbm.apply_lightgbm_multiclass_classifier
