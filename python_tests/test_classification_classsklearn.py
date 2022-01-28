import os
from shutil import copy2
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL),
    reason="h5py or scikit-learn dependencies not available",
)
def test_perform_sklearn_classifier_param_search():
    import rsgislib.classification
    import rsgislib.classification.classsklearn
    from sklearn.model_selection import GridSearchCV
    from sklearn.ensemble import RandomForestClassifier

    grid_search = GridSearchCV(
        RandomForestClassifier(), {"n_estimators": [10, 20, 30], "max_depth": [2, 3, 4]}
    )

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

    rsgislib.classification.classsklearn.perform_sklearn_classifier_param_search(
        cls_info_dict, search_obj=grid_search
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL),
    reason="h5py or scikit-learn dependencies not available",
)
def test_train_sklearn_classifier():
    import rsgislib.classification
    import rsgislib.classification.classsklearn
    from sklearn.ensemble import RandomForestClassifier

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

    sk_classifier = RandomForestClassifier()

    rsgislib.classification.classsklearn.train_sklearn_classifier(
        cls_info_dict, sk_classifier
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL),
    reason="h5py or scikit-learn dependencies not available",
)
def test_apply_sklearn_classifier(tmp_path):
    import rsgislib.imageutils
    import rsgislib.classification
    import rsgislib.classification.classsklearn
    from sklearn.ensemble import RandomForestClassifier

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

    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    s2_vld_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")

    img_band_info = []
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    )

    sk_classifier = RandomForestClassifier()
    rsgislib.classification.classsklearn.train_sklearn_classifier(
        cls_info_dict, sk_classifier
    )

    output_img = os.path.join(tmp_path, "out_cls_img.kea")
    out_score_img = os.path.join(tmp_path, "out_cls_scr_img.kea")
    rsgislib.classification.classsklearn.apply_sklearn_classifier(
        cls_info_dict,
        sk_classifier,
        s2_vld_img,
        1,
        img_band_info,
        output_img,
        "KEA",
        class_clr_names=True,
        out_score_img=out_score_img,
    )

    assert os.path.exists(output_img) and os.path.exists(out_score_img)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or SKLEARN_NOT_AVAIL),
    reason="h5py or scikit-learn dependencies not available",
)
def test_apply_sklearn_classifier_rat(tmp_path):
    import rsgislib.classification.classsklearn
    import rsgislib.rastergis
    from sklearn.ensemble import RandomForestClassifier

    ref_clumps_img = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means_cls.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_s2means_cls.kea")
    copy2(ref_clumps_img, clumps_img)

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

    sk_classifier = RandomForestClassifier()

    rsgislib.classification.classsklearn.train_sklearn_classifier(
        cls_info_dict, sk_classifier
    )

    variables = [
        "b1Mean",
        "b2Mean",
        "b3Mean",
        "b4Mean",
        "b5Mean",
        "b6Mean",
        "b7Mean",
        "b8Mean",
        "b9Mean",
        "b10Mean",
    ]
    rsgislib.classification.classsklearn.apply_sklearn_classifier_rat(
        clumps_img,
        variables,
        sk_classifier,
        cls_info_dict,
        out_col_int="OutClass",
        out_col_str="OutClassName",
        roi_col=None,
        roi_val=1,
        class_colours=True,
    )

    read_out_cls = False
    try:
        cls_col_vals = rsgislib.rastergis.get_column_data(clumps_img, "OutClass")
        read_out_cls = True
    except:
        read_out_cls = False

    assert read_out_cls
