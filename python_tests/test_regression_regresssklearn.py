import os
import pytest

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
REGRESS_DATA_DIR = os.path.join(DATA_DIR, "regression")


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_get_obj_params():
    import rsgislib.regression.regresssklearn

    rsgislib.regression.regresssklearn.get_ann_obj_params(1)
    rsgislib.regression.regresssklearn.get_en_obj_params(1)
    rsgislib.regression.regresssklearn.get_knn_obj_params(1)
    rsgislib.regression.regresssklearn.get_kr_obj_params(1)
    rsgislib.regression.regresssklearn.get_et_obj_params(1)
    rsgislib.regression.regresssklearn.get_pls_obj_params(1)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_create_search_obj():
    import rsgislib.regression.regresssklearn
    import numpy
    from sklearn.ensemble import ExtraTreesRegressor

    et_grid = {
        "n_estimators": numpy.arange(100, 200, 100, dtype="uint16").tolist(),
        "max_features": numpy.arange(1, 2, 1, dtype="uint8").tolist(),
        "max_depth": numpy.arange(1, 2, 1, dtype="uint8").tolist(),
    }
    et_obj = ExtraTreesRegressor()
    rsgislib.regression.regresssklearn.create_search_obj(
        et_obj, et_grid, n_runs=25, n_cv=2, n_cores=1
    )


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_perform_search_param_opt(tmp_path):
    import rsgislib.regression.regresssklearn
    import rsgislib.vectorattrs
    import numpy
    from sklearn.ensemble import ExtraTreesRegressor

    et_grid = {
        "n_estimators": numpy.arange(100, 200, 100, dtype="uint16").tolist(),
        "max_features": numpy.arange(1, 2, 1, dtype="uint8").tolist(),
        "max_depth": numpy.arange(1, 2, 1, dtype="uint8").tolist(),
    }
    et_obj = ExtraTreesRegressor()
    skl_srch_obj = rsgislib.regression.regresssklearn.create_search_obj(
        et_obj, et_grid, n_runs=25, n_cv=2, n_cores=1
    )

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts_test.geojson")
    vec_lyr = "sample_pts_test"

    y = rsgislib.vectorattrs.get_vec_cols_as_array(vec_file, vec_lyr, cols=["value"])
    x = rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols=["re_b4", "re_b5", "re_b6", "nir_b7", "nir_b8"]
    )

    opt_params_file = os.path.join(tmp_path, "out.json")
    rsgislib.regression.regresssklearn.perform_search_param_opt(
        opt_params_file, x, y, skl_srch_obj, data_scaler=None
    )

    assert os.path.exists(opt_params_file)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_perform_kfold_fit():
    import rsgislib.regression.regresssklearn
    import rsgislib.vectorattrs
    from sklearn.ensemble import ExtraTreesRegressor

    et_obj = ExtraTreesRegressor()

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts_train.geojson")
    vec_lyr = "sample_pts_train"

    y = rsgislib.vectorattrs.get_vec_cols_as_array(vec_file, vec_lyr, cols=["value"])
    x = rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols=["re_b4", "re_b5", "re_b6", "nir_b7", "nir_b8"]
    )
    rsgislib.regression.regresssklearn.perform_kfold_fit(
        et_obj, x, y, n_splits=2, repeats=2, shuffle=True, data_scaler=None
    )


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_apply_regress_sklearn_mdl(tmp_path):
    import rsgislib.regression.regresssklearn
    import rsgislib.vectorattrs
    from sklearn.ensemble import ExtraTreesRegressor

    et_obj = ExtraTreesRegressor()

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts_train.geojson")
    vec_lyr = "sample_pts_train"

    y = rsgislib.vectorattrs.get_vec_cols_as_array(vec_file, vec_lyr, cols=["value"])
    x = rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols=["re_b4", "re_b5", "re_b6", "nir_b7", "nir_b8"]
    )

    et_obj.fit(x, y)

    s2_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.tif")
    vld_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.tif")
    out_img = os.path.join(tmp_path, "out_img.tif")

    rsgislib.regression.regresssklearn.apply_regress_sklearn_mdl(
        et_obj,
        1,
        s2_img,
        [4, 5, 6, 7, 8],
        vld_msk_img,
        1,
        out_img,
        gdalformat="GTIFF",
        out_band_names=None,
        calc_stats=True,
        out_no_date_val=0.0,
    )

    assert os.path.exists(out_img)
