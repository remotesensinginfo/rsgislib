import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

LIGHTGBM_NOT_AVAIL = False
try:
    import lightgbm
except ImportError:
    LIGHTGBM_NOT_AVAIL = True

SKOPT_NOT_AVAIL = False
try:
    import skopt
except ImportError:
    SKOPT_NOT_AVAIL = True

BAYESOPT_NOT_AVAIL = False
try:
    import bayes_opt
except ImportError:
    BAYESOPT_NOT_AVAIL = True

OPTUNA_NOT_AVAIL = False
try:
    import optuna
except ImportError:
    OPTUNA_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or SKOPT_NOT_AVAIL),
    reason="h5py, skopt or lightgbm dependencies not available",
)
def test_optimise_lightgbm_binary_classifier_skopt(tmp_path):
    import rsgislib.classification.classlightgbm

    cls1_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
    )
    cls1_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
    )
    cls2_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
    )
    cls2_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
    )

    out_params_file = os.path.join(tmp_path, "out_params_file.json")
    rsgislib.classification.classlightgbm.optimise_lightgbm_binary_classifier(
        out_params_file,
        cls1_train_file,
        cls1_valid_file,
        cls2_train_file,
        cls2_valid_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_SKOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
    )

    assert os.path.exists(out_params_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or BAYESOPT_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_optimise_lightgbm_binary_classifier_bayesopt(tmp_path):
    import rsgislib.classification.classlightgbm

    cls1_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
    )
    cls1_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
    )
    cls2_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
    )
    cls2_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
    )

    out_params_file = os.path.join(tmp_path, "out_params_file.json")
    rsgislib.classification.classlightgbm.optimise_lightgbm_binary_classifier(
        out_params_file,
        cls1_train_file,
        cls1_valid_file,
        cls2_train_file,
        cls2_valid_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYSIANOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
    )

    assert os.path.exists(out_params_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or OPTUNA_NOT_AVAIL),
    reason="h5py, optuna or lightgbm dependencies not available",
)
def test_optimise_lightgbm_binary_classifier_optuna(tmp_path):
    import rsgislib.classification.classlightgbm

    cls1_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_train.h5"
    )
    cls1_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_forest_smpls_bal_valid.h5"
    )
    cls2_train_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_train.h5"
    )
    cls2_valid_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "cls_grass_smpls_bal_valid.h5"
    )

    out_params_file = os.path.join(tmp_path, "out_params_file.json")
    rsgislib.classification.classlightgbm.optimise_lightgbm_binary_classifier(
        out_params_file,
        cls1_train_file,
        cls1_valid_file,
        cls2_train_file,
        cls2_valid_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_OPTUNA,
        n_opt_iters=10,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
    )

    assert os.path.exists(out_params_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL),
    reason="h5py or lightgbm dependencies not available",
)
def test_train_lightgbm_binary_classifier(tmp_path):
    import rsgislib.classification.classlightgbm

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

    cls_params_file = os.path.join(CLASSIFICATION_DATA_DIR, "lgbm_params_file.json")
    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_lightgbm_binary_classifier(
        out_mdl_file,
        cls_params_file,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        n_threads=1,
        mdl_cls_obj=None,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or SKOPT_NOT_AVAIL),
    reason="h5py, skopt or lightgbm dependencies not available",
)
def test_train_opt_lightgbm_binary_classifier_skopt(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_params_file = os.path.join(tmp_path, "out_cls_params.json")
    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier(
        out_mdl_file,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_SKOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
        out_params_file=out_params_file,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or BAYESOPT_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_train_opt_lightgbm_binary_classifier_bayesopt(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_params_file = os.path.join(tmp_path, "out_cls_params.json")
    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier(
        out_mdl_file,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYSIANOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
        out_params_file=out_params_file,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or OPTUNA_NOT_AVAIL),
    reason="h5py, optuna or lightgbm dependencies not available",
)
def test_train_opt_lightgbm_binary_classifier_optuna(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_params_file = os.path.join(tmp_path, "out_cls_params.json")
    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier(
        out_mdl_file,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_OPTUNA,
        n_opt_iters=10,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
        out_params_file=out_params_file,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or BAYESOPT_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_apply_lightgbm_binary_classifier_bayesopt(tmp_path):
    import rsgislib.classification.classlightgbm
    import rsgislib.imageutils

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

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_binary_classifier(
        out_mdl_file,
        cls1_train_file,
        cls1_valid_file,
        cls1_test_file,
        cls2_train_file,
        cls2_valid_file,
        cls2_test_file,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYSIANOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
        scale_pos_weight=None,
        mdl_cls_obj=None,
    )

    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    img_band_info = []
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    )

    out_prob_img = os.path.join(tmp_path, "out_prob_cls.kea")
    out_class_img = os.path.join(tmp_path, "out_cls_img.kea")
    rsgislib.classification.classlightgbm.apply_lightgbm_binary_classifier(
        out_mdl_file,
        in_msk_img,
        1,
        img_band_info,
        out_prob_img,
        "KEA",
        out_class_img,
        class_thres=5000,
    )

    assert os.path.exists(out_prob_img) and os.path.exists(out_class_img)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or SKOPT_NOT_AVAIL),
    reason="h5py, skopt or lightgbm dependencies not available",
)
def test_optimise_lightgbm_multiclass_classifier_skopt(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_params_file = os.path.join(tmp_path, "out_params_files.json")
    rsgislib.classification.classlightgbm.optimise_lightgbm_multiclass_classifier(
        out_params_file,
        cls_info_dict,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_SKOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
    )

    assert os.path.exists(out_params_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or BAYESOPT_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_optimise_lightgbm_multiclass_classifier_bayesopt(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_params_file = os.path.join(tmp_path, "out_params_files.json")
    rsgislib.classification.classlightgbm.optimise_lightgbm_multiclass_classifier(
        out_params_file,
        cls_info_dict,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYSIANOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
    )

    assert os.path.exists(out_params_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or OPTUNA_NOT_AVAIL),
    reason="h5py, optuna or lightgbm dependencies not available",
)
def test_optimise_lightgbm_multiclass_classifier_optuna(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_params_file = os.path.join(tmp_path, "out_params_files.json")
    rsgislib.classification.classlightgbm.optimise_lightgbm_multiclass_classifier(
        out_params_file,
        cls_info_dict,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_OPTUNA,
        n_opt_iters=10,
        rnd_seed=None,
        n_threads=1,
    )

    assert os.path.exists(out_params_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL),
    reason="h5py or lightgbm dependencies not available",
)
def test_train_lightgbm_multiclass_classifier(tmp_path):
    import rsgislib.classification.classlightgbm

    cls_params_file = os.path.join(
        CLASSIFICATION_DATA_DIR, "lgbm_mulcls_params_file.json"
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

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_lightgbm_multiclass_classifier(
        cls_params_file,
        out_mdl_file,
        cls_info_dict,
        out_info_file=None,
        unbalanced=False,
        n_threads=1,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or SKOPT_NOT_AVAIL),
    reason="h5py, skopt or lightgbm dependencies not available",
)
def test_train_opt_lightgbm_multiclass_classifier_skopt(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_multiclass_classifier(
        out_mdl_file,
        cls_info_dict,
        out_info_file=None,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_SKOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or BAYESOPT_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_train_opt_lightgbm_multiclass_classifier_bayesopt(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_multiclass_classifier(
        out_mdl_file,
        cls_info_dict,
        out_info_file=None,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYSIANOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or OPTUNA_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_train_opt_lightgbm_multiclass_classifier_optuna(tmp_path):
    import rsgislib.classification.classlightgbm

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

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_multiclass_classifier(
        out_mdl_file,
        cls_info_dict,
        out_info_file=None,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_OPTUNA,
        n_opt_iters=10,
        rnd_seed=None,
        n_threads=1,
    )

    assert os.path.exists(out_mdl_file)


@pytest.mark.skipif(
    (H5PY_NOT_AVAIL or LIGHTGBM_NOT_AVAIL or BAYESOPT_NOT_AVAIL),
    reason="h5py, bayes_opt or lightgbm dependencies not available",
)
def test_apply_lightgbm_multiclass_classifier_bayesopt(tmp_path):
    import rsgislib.classification.classlightgbm
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

    out_mdl_file = os.path.join(tmp_path, "out_mdl_file.txt")
    rsgislib.classification.classlightgbm.train_opt_lightgbm_multiclass_classifier(
        out_mdl_file,
        cls_info_dict,
        out_info_file=None,
        unbalanced=False,
        op_mthd=rsgislib.OPT_MTHD_BAYSIANOPT,
        n_opt_iters=1,
        rnd_seed=None,
        n_threads=1,
    )

    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    img_band_info = []
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(s2_img, "s2", [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    )

    out_class_img = os.path.join(tmp_path, "out_cls_img.kea")
    rsgislib.classification.classlightgbm.apply_lightgbm_multiclass_classifier(
        cls_info_dict,
        out_mdl_file,
        in_msk_img,
        1,
        img_band_info,
        out_class_img,
        "KEA",
        class_clr_names=True,
    )

    assert os.path.exists(out_class_img)
