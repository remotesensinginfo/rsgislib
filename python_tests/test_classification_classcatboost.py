import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

CATBOOST_NOT_AVAIL = False
try:
    import catboost
except ImportError:
    CATBOOST_NOT_AVAIL = True


DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or CATBOOST_NOT_AVAIL),
    reason="h5py or catboost dependencies not available",
)
def test_train_catboost_binary_classifier(tmp_path):
    import rsgislib.classification.classcatboost

    mdl_cls_obj = rsgislib.classification.classcatboost.get_catboost_mdl()

    cls1_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
    )
    cls1_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
    )
    cls1_test_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_test.h5"
    )
    cls2_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
    )
    cls2_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
    )
    cls2_test_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_test.h5"
    )

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.json")
    rsgislib.classification.classcatboost.train_catboost_binary_classifier(
        mdl_cls_obj,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        cat_cols=None,
        out_mdl_file=out_mdl_file,
        verbose_training=False,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or CATBOOST_NOT_AVAIL),
    reason="h5py or catboost dependencies not available",
)
def test_apply_catboost_binary_classifier(tmp_path):
    import rsgislib.classification.classcatboost
    import rsgislib.imageutils

    mdl_cls_obj = rsgislib.classification.classcatboost.get_catboost_mdl()

    cls1_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
    )
    cls1_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
    )
    cls1_test_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_test.h5"
    )
    cls2_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
    )
    cls2_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
    )
    cls2_test_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_test.h5"
    )

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.json")
    rsgislib.classification.classcatboost.train_catboost_binary_classifier(
        mdl_cls_obj,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        cat_cols=None,
        out_mdl_file=out_mdl_file,
        verbose_training=False,
    )

    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.tif")
    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.tif")

    img_band_info = []
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    )

    out_prob_img = os.path.join(tmp_path, "out_prob_cls.tif")
    out_class_img = os.path.join(tmp_path, "out_cls_img.tif")

    rsgislib.classification.classcatboost.apply_catboost_binary_classifier(
        mdl_cls_obj,
        in_msk_img,
        1,
        img_band_info,
        out_class_img,
        "GTIFF",
        out_prob_img,
    )

    assert os.path.exists(out_prob_img) and os.path.exists(out_class_img)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or CATBOOST_NOT_AVAIL),
    reason="h5py or catboost dependencies not available",
)
def test_train_catboost_multiclass_classifier(tmp_path):
    import rsgislib.classification.classcatboost

    cls_info_dict = dict()
    cls_info_dict["Forest"] = rsgislib.classification.ClassInfoObj(
        id=0,
        out_id=1,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )
    cls_info_dict["Grass"] = rsgislib.classification.ClassInfoObj(
        id=1,
        out_id=2,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )
    cls_info_dict["Urban"] = rsgislib.classification.ClassInfoObj(
        id=2,
        out_id=3,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )
    cls_info_dict["Water"] = rsgislib.classification.ClassInfoObj(
        id=3,
        out_id=4,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )

    mdl_cls_obj = rsgislib.classification.classcatboost.get_catboost_mdl()
    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.json")
    rsgislib.classification.classcatboost.train_catboost_multiclass_classifier(
        mdl_cls_obj,
        cls_info_dict,
        cat_cols=None,
        out_mdl_file=out_mdl_file,
        verbose_training=False,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or CATBOOST_NOT_AVAIL),
    reason="h5py or catboost dependencies not available",
)
def test_apply_catboost_multiclass_classifier(tmp_path):
    import rsgislib.classification.classcatboost
    import rsgislib.imageutils

    cls_info_dict = dict()
    cls_info_dict["Forest"] = rsgislib.classification.ClassInfoObj(
        id=0,
        out_id=1,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )
    cls_info_dict["Grass"] = rsgislib.classification.ClassInfoObj(
        id=1,
        out_id=2,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )
    cls_info_dict["Urban"] = rsgislib.classification.ClassInfoObj(
        id=2,
        out_id=3,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )
    cls_info_dict["Water"] = rsgislib.classification.ClassInfoObj(
        id=3,
        out_id=4,
        train_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal_train.h5"
        ),
        test_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal_test.h5"
        ),
        valid_file_h5=os.path.join(
            CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal_valid.h5"
        ),
        red=120,
        green=120,
        blue=120,
    )

    mdl_cls_obj = rsgislib.classification.classcatboost.get_catboost_mdl()
    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.json")
    rsgislib.classification.classcatboost.train_catboost_multiclass_classifier(
        mdl_cls_obj,
        cls_info_dict,
        cat_cols=None,
        out_mdl_file=out_mdl_file,
        verbose_training=False,
    )

    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.tif")
    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.tif")

    img_band_info = []
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    )

    out_class_img = os.path.join(tmp_path, "out_cls_img.tif")
    rsgislib.classification.classcatboost.apply_catboost_multiclass_classifier(
        mdl_cls_obj,
        cls_info_dict,
        in_msk_img,
        1,
        img_band_info,
        out_class_img,
        gdalformat="GTIFF",
        class_clr_names=False,
    )

    assert os.path.exists(out_class_img)
