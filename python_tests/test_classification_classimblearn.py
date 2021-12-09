import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

IMBLEARN_NOT_AVAIL = False
try:
    import imblearn
except ImportError:
    IMBLEARN_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_rand_oversample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.rand_oversample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_smote_oversample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.smote_oversample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_adasyn_oversample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    # Use same training input file so there must be 'neighbours'.
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.adasyn_oversample_smpls(
        cls_in_info, cls_out_info
    )

    assert os.path.exists(
        os.path.join(tmp_path, "cls_forest_out_smpls.h5")
    ) and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_borderline_smote_oversample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.borderline_smote_oversample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_cluster_centroid_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.cluster_centroid_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_random_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.random_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_near_miss_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.near_miss_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_edited_near_neigh_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.edited_near_neigh_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_repeat_edited_near_neigh_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.borderline_smote_oversample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_all_knn_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.borderline_smote_oversample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_condensed_near_neigh_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.condensed_near_neigh_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_one_sided_sel_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.one_sided_sel_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_neighbourhood_clean_undersample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.neighbourhood_clean_undersample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_smoteenn_combined_sample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.smoteenn_combined_sample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or IMBLEARN_NOT_AVAIL),
    reason="h5py or imblearn dependencies not available",
)
def test_smotetomek_combined_sample_smpls(tmp_path):
    import rsgislib.classification
    import rsgislib.classification.classimblearn

    cls_in_info = dict()
    cls_in_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_forest_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_grass_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_urban_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_in_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(CLASSIFICATION_DATA_DIR, "cls_water_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    cls_out_info = dict()
    cls_out_info["Forest"] = rsgislib.classification.ClassSimpleInfoObj(
        id=1,
        file_h5=os.path.join(tmp_path, "cls_forest_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Grass"] = rsgislib.classification.ClassSimpleInfoObj(
        id=2,
        file_h5=os.path.join(tmp_path, "cls_grass_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Urban"] = rsgislib.classification.ClassSimpleInfoObj(
        id=3,
        file_h5=os.path.join(tmp_path, "cls_urban_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )
    cls_out_info["Water"] = rsgislib.classification.ClassSimpleInfoObj(
        id=4,
        file_h5=os.path.join(tmp_path, "cls_water_out_smpls.h5"),
        red=120,
        green=120,
        blue=120,
    )

    rsgislib.classification.classimblearn.smotetomek_combined_sample_smpls(
        cls_in_info, cls_out_info
    )

    assert (
        os.path.exists(os.path.join(tmp_path, "cls_forest_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_grass_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_urban_out_smpls.h5"))
        and os.path.exists(os.path.join(tmp_path, "cls_water_out_smpls.h5"))
    )
