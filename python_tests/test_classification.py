import os
import pytest
from shutil import copy2

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

PLOTLY_NOT_AVAIL = False
try:
    import plotly
except ImportError:
    PLOTLY_NOT_AVAIL = True

KALEIDO_NOT_AVAIL = False
try:
    import kaleido
except ImportError:
    KALEIDO_NOT_AVAIL = True

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
ZONALSTATS_DATA_DIR = os.path.join(DATA_DIR, "zonalstats")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_get_num_samples_pts():
    import rsgislib.classification

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")

    n_smpls = rsgislib.classification.get_num_samples(in_h5_file)

    assert n_smpls == 703


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_get_num_samples_chips():
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_chip_vals.h5"
    )

    n_smpls = rsgislib.classification.get_num_samples(in_h5_file)

    assert n_smpls == 6


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_get_num_samples_refchips():
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_refchip_vals.h5"
    )

    n_smpls = rsgislib.classification.get_num_samples(in_h5_file)

    assert n_smpls == 6


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_split_sample_train_valid_test(tmp_path):
    import rsgislib.classification

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")

    train_h5_file = os.path.join(tmp_path, "out_train_data.h5")
    valid_h5_file = os.path.join(tmp_path, "out_valid_data.h5")
    test_h5_file = os.path.join(tmp_path, "out_test_data.h5")

    rsgislib.classification.split_sample_train_valid_test(
        in_h5_file,
        train_h5_file,
        valid_h5_file,
        test_h5_file,
        100,
        100,
        train_sample=500,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16INT,
    )

    assert (
        os.path.exists(train_h5_file)
        and os.path.exists(valid_h5_file)
        and os.path.exists(test_h5_file)
    )


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_create_train_valid_test_sets(tmp_path):
    import rsgislib.classification

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassInfoObj(
        id=0,
        out_id=1,
        train_file_h5=os.path.join(tmp_path, "cls_forest_smpls_bal_train.h5"),
        test_file_h5=os.path.join(tmp_path, "cls_forest_smpls_bal_test.h5"),
        valid_file_h5=os.path.join(tmp_path, "cls_forest_smpls_bal_valid.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassInfoObj(
        id=1,
        out_id=2,
        train_file_h5=os.path.join(tmp_path, "cls_grass_smpls_bal_train.h5"),
        test_file_h5=os.path.join(tmp_path, "cls_grass_smpls_bal_test.h5"),
        valid_file_h5=os.path.join(tmp_path, "cls_grass_smpls_bal_valid.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassInfoObj(
        id=2,
        out_id=3,
        train_file_h5=os.path.join(tmp_path, "cls_urban_smpls_bal_train.h5"),
        test_file_h5=os.path.join(tmp_path, "cls_urban_smpls_bal_test.h5"),
        valid_file_h5=os.path.join(tmp_path, "cls_urban_smpls_bal_valid.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassInfoObj(
        id=3,
        out_id=4,
        train_file_h5=os.path.join(tmp_path, "cls_water_smpls_bal_train.h5"),
        test_file_h5=os.path.join(tmp_path, "cls_water_smpls_bal_test.h5"),
        valid_file_h5=os.path.join(tmp_path, "cls_water_smpls_bal_valid.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.create_train_valid_test_sets(
        cls_in_info, cls_out_info, 500, 500, train_sample=None
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_smpls_bal_train.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_smpls_bal_test.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_smpls_bal_valid.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_smpls_bal_train.h5"))
    )


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_split_chip_sample_train_valid_test(tmp_path):
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_chip_vals.h5"
    )

    train_h5_file = os.path.join(tmp_path, "out_train_data.h5")
    valid_h5_file = os.path.join(tmp_path, "out_valid_data.h5")
    test_h5_file = os.path.join(tmp_path, "out_test_data.h5")

    rsgislib.classification.split_chip_sample_train_valid_test(
        in_h5_file,
        train_h5_file,
        valid_h5_file,
        test_h5_file,
        2,
        2,
        train_sample=None,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16INT,
    )

    assert (
        os.path.exists(train_h5_file)
        and os.path.exists(valid_h5_file)
        and os.path.exists(test_h5_file)
    )


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_split_chip_sample_ref_train_valid_test(tmp_path):
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_refchip_vals.h5"
    )

    train_h5_file = os.path.join(tmp_path, "out_train_data.h5")
    valid_h5_file = os.path.join(tmp_path, "out_valid_data.h5")
    test_h5_file = os.path.join(tmp_path, "out_test_data.h5")

    rsgislib.classification.split_chip_sample_ref_train_valid_test(
        in_h5_file,
        train_h5_file,
        valid_h5_file,
        test_h5_file,
        2,
        2,
        train_sample=None,
        rnd_seed=42,
        datatype=rsgislib.TYPE_16INT,
    )

    assert (
        os.path.exists(train_h5_file)
        and os.path.exists(valid_h5_file)
        and os.path.exists(test_h5_file)
    )


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_flip_chip_hdf5_file(tmp_path):
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_chip_vals.h5"
    )

    out_h5_file = os.path.join(tmp_path, "out_data.h5")

    rsgislib.classification.flip_chip_hdf5_file(
        in_h5_file, out_h5_file, datatype=rsgislib.TYPE_16INT
    )

    assert os.path.exists(out_h5_file)


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_flip_ref_chip_hdf5_file(tmp_path):
    import rsgislib.classification

    in_h5_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_refchip_vals.h5"
    )

    out_h5_file = os.path.join(tmp_path, "out_data.h5")

    rsgislib.classification.flip_ref_chip_hdf5_file(
        in_h5_file, out_h5_file, datatype=rsgislib.TYPE_16INT
    )

    assert os.path.exists(out_h5_file)


def test_get_class_training_data(tmp_path):
    import rsgislib.imageutils
    import rsgislib.classification

    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    cls_vec_smpls = os.path.join(CLASSIFICATION_DATA_DIR, "cls_poly_smpls.gpkg")

    img_band_info = []
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    )

    class_vec_sample_info = []
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=1,
            class_name="Forest",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_forest_smpls",
            file_h5=os.path.join(tmp_path, "cls_forest_smpls.h5"),
        )
    )
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=2,
            class_name="Grass",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_grass_smpls",
            file_h5=os.path.join(tmp_path, "cls_grass_smpls.h5"),
        )
    )
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=3,
            class_name="Urban",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_urban_smpls",
            file_h5=os.path.join(tmp_path, "cls_urban_smpls.h5"),
        )
    )
    class_vec_sample_info.append(
        rsgislib.classification.ClassVecSamplesInfoObj(
            id=4,
            class_name="Water",
            vec_file=cls_vec_smpls,
            vec_lyr="cls_water_smpls",
            file_h5=os.path.join(tmp_path, "cls_water_smpls.h5"),
        )
    )

    cls_info = rsgislib.classification.get_class_training_data(
        img_band_info,
        class_vec_sample_info,
        tmp_dir=tmp_path,
        sub_sample=None,
        ref_img=s2_img,
    )

    all_files_present = True
    for cls in cls_info:
        if not os.path.exists(cls_info[cls].file_h5):
            all_files_present = False

    assert all_files_present


"""
@pytest.mark.skipif(
    (PLOTLY_NOT_AVAIL or KALEIDO_NOT_AVAIL),
    reason="plotly or kaleido dependencies not available",
)
def test_plot_train_data(tmp_path):
    import glob
    import rsgislib.classification

    forest_h5 = os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5")
    urban_h5 = os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5")

    rsgislib.classification.plot_train_data(
        forest_h5,
        urban_h5,
        tmp_path,
        cls1_name="Forest",
        cls2_name="Urban",
        var_names=None,
    )

    plot_files = glob.glob(os.path.join(tmp_path, "*.png"))

    assert len(plot_files) == 100
"""


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_create_acc_pt_sets(tmp_path):
    import rsgislib.classification
    import glob

    vec_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "gmw_acc_roi_30_cls_acc_pts.geojson"
    )
    vec_lyr = "gmw_acc_roi_30_cls_acc_pts"

    out_vec_file_base = os.path.join(tmp_path, "gmw_acc_roi_30_cls_acc_pts_")

    rsgislib.classification.create_acc_pt_sets(
        vec_file,
        vec_lyr,
        out_vec_file_base,
        "gmw_acc_roi_30_cls_acc_pts",
        "gmw_v25_cls",
        10,
        sets_col="set_id",
        out_format="GeoJSON",
        out_ext="geojson",
        shuffle_rows=True,
        rnd_seed=42,
    )
    assert (
        len(glob.glob(os.path.join(tmp_path, "gmw_acc_roi_30_cls_acc_pts_*.geojson")))
        == 10
    )


def test_generate_random_accuracy_pts(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "gmw_acc_roi_1_cls.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "cls_name",
        "gmw_v2_cls",
        "ref_cls",
        1000,
        42,
        del_exist_vec=False,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_stratified_random_accuracy_pts(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "gmw_acc_roi_1_cls.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_stratified_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "cls_name",
        "gmw_v2_cls",
        "ref_cls",
        1000,
        42,
        False,
        False,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_stratified_random_accuracy_pts_pxllst(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "gmw_acc_roi_1_cls.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_stratified_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "cls_name",
        "gmw_v2_cls",
        "ref_cls",
        1000,
        42,
        False,
        True,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_stratified_prop_random_accuracy_pts(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "gmw_acc_roi_1_cls.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_stratified_prop_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "cls_name",
        "gmw_v2_cls",
        "ref_cls",
        100,
        10,
        42,
        False,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_random_accuracy_pts_clslut(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "cls_1985_amazon_manaus_img.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "class_names",
        "img_cls",
        "ref_cls",
        1000,
        42,
        del_exist_vec=False,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_stratified_random_accuracy_pts_clslut(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "cls_1985_amazon_manaus_img.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_stratified_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "class_names",
        "img_cls",
        "ref_cls",
        1000,
        42,
        False,
        False,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_stratified_random_accuracy_pts_pxllst_clslut(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "cls_1985_amazon_manaus_img.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_stratified_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "class_names",
        "img_cls",
        "ref_cls",
        1000,
        42,
        False,
        True,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_generate_stratified_prop_random_accuracy_pts_clslut(tmp_path):
    import rsgislib.classification
    import rsgislib.vectorutils

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "cls_1985_amazon_manaus_img.kea")

    out_vec_file = os.path.join(tmp_path, "out_vecs.gpkg")
    out_vec_lyr = "out_vecs"

    rsgislib.classification.generate_stratified_prop_random_accuracy_pts(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        "class_names",
        "img_cls",
        "ref_cls",
        100,
        10,
        42,
        False,
    )

    n_pts = rsgislib.vectorutils.get_vec_feat_count(out_vec_file, out_vec_lyr)

    assert os.path.exists(out_vec_file) and (n_pts > 0)


def test_pop_class_info_accuracy_pts_only_cls_col(tmp_path):
    import rsgislib.classification

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "gmw_acc_roi_1_cls.kea")

    vec_ref_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "gmw_acc_set_1_acc_pts.geojson"
    )
    vec_file = os.path.join(tmp_path, "gmw_acc_set_1_acc_pts.geojson")
    copy2(vec_ref_file, vec_file)
    vec_lyr = "gmw_acc_set_1_acc_pts"

    rsgislib.classification.pop_class_info_accuracy_pts(
        input_img, vec_file, vec_lyr, "cls_name", "gmw_new_cls"
    )


def test_pop_class_info_accuracy_pts_all_col(tmp_path):
    import rsgislib.classification

    input_img = os.path.join(CLASSIFICATION_DATA_DIR, "gmw_acc_roi_1_cls.kea")

    vec_ref_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "gmw_acc_set_1_acc_pts.geojson"
    )
    vec_file = os.path.join(tmp_path, "gmw_acc_set_1_acc_pts.geojson")
    copy2(vec_ref_file, vec_file)
    vec_lyr = "gmw_acc_set_1_acc_pts"

    rsgislib.classification.pop_class_info_accuracy_pts(
        input_img,
        vec_file,
        vec_lyr,
        "cls_name",
        "gmw_new_cls",
        "ref_new_col",
        "new_process",
    )


def test_get_class_info_dict(tmp_path):
    import rsgislib.classification

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls_bal.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = rsgislib.classification.get_class_info_dict(
        cls_in_info, smpls_dir=tmp_path
    )
