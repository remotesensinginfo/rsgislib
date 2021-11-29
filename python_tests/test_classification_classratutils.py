import os
import pytest
from shutil import copy2

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")


def test_populate_clumps_with_class_training(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classratutils
    import rsgislib.rastergis.ratutils
    import numpy

    cls_vec_smpls = os.path.join(CLASSIFICATION_DATA_DIR, "cls_poly_smpls.gpkg")

    clumps_img_ref = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_s2means.kea")
    copy2(clumps_img_ref, clumps_img)

    class_vec_sample_info = []
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=1,
            class_name="Forest",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_forest_smpls",
            file_h5="cls_forest_smpls.h5",
        )
    )
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=2,
            class_name="Grass",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_grass_smpls",
            file_h5="cls_grass_smpls.h5",
        )
    )
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=3,
            class_name="Urban",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_urban_smpls",
            file_h5="cls_urban_smpls.h5",
        )
    )
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=4,
            class_name="Water",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_water_smpls",
            file_h5="cls_water_smpls.h5",
        )
    )

    rsgislib.classification.classratutils.populate_clumps_with_class_training(
        clumps_img, class_vec_sample_info, "./tmp", "cls_int", "cls_name"
    )

    clumps_cls_img = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means_cls.kea"
    )

    vars_to_test = ["cls_int", "cls_name"]
    vars_eq_vals = True
    for var in vars_to_test:
        print("Testing: {}".format(var))

        ref_vals = rsgislib.rastergis.ratutils.get_column_data(clumps_cls_img, var)
        calcd_vals = rsgislib.rastergis.ratutils.get_column_data(clumps_img, var)
        if not numpy.array_equal(ref_vals, calcd_vals):
            vars_eq_vals = False
            break

    assert vars_eq_vals


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_extract_rat_col_data(tmp_path):
    import rsgislib.classification.classratutils

    clumps_img = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means_cls.kea"
    )

    cols = [
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
    out_h5_file = os.path.join(tmp_path, "forest_smpls.h5")
    rsgislib.classification.classratutils.extract_rat_col_data(
        clumps_img, cols, sel_col="cls_int", sel_col_val=1, out_h5_file=out_h5_file
    )

    assert os.path.exists(out_h5_file)
