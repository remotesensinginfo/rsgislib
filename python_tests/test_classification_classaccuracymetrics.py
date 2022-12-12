import os
import pytest

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

MATPLOTLIB_NOT_AVAIL = False
try:
    import matplotlib.pyplot
except ImportError:
    MATPLOTLIB_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASS_ACC_DATA_DIR = os.path.join(DATA_DIR, "classification", "accuracy")


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_calc_acc_ptonly_metrics_vecsamples(tmp_path):
    import rsgislib.classification.classaccuracymetrics

    vec_file = os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson")
    vec_lyr = "gmw_acc_roi_1_cls_acc_pts_1"

    out_json_file = os.path.join(tmp_path, "out_acc_stats.json")
    out_csv_file = os.path.join(tmp_path, "out_acc_stats.csv")

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples(
        vec_file, vec_lyr, ref_col, cls_col, out_json_file, out_csv_file
    )

    assert os.path.exists(out_json_file) and os.path.exists(out_csv_file)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_calc_acc_ptonly_metrics_vecsamples_test_vals(tmp_path):
    from rsgislib.classification.classaccuracymetrics import (
        calc_acc_ptonly_metrics_vecsamples,
    )
    from rsgislib.tools.utils import (
        similar_numeric_vals,
        dict_struct_get_numeric_value,
        dict_struct_get_str_list_value,
    )

    vec_file = os.path.join(CLASS_ACC_DATA_DIR, "acc_ass_smpls_test_vals.geojson")
    vec_lyr = "acc_ass_smpls_test_vals"

    ref_col = "ref_smpls"
    cls_col = "img_smpls"

    acc_rslt = calc_acc_ptonly_metrics_vecsamples(vec_file, vec_lyr, ref_col, cls_col)

    thres_vals = dict()
    thres_vals["accuracy"] = {
        "path": ["accuracy"],
        "val": 0.8476,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["bal_accuracy_score"] = {
        "path": ["bal_accuracy_score"],
        "val": 0.8516,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["cohen_kappa"] = {
        "path": ["cohen_kappa"],
        "val": 0.773,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["forest_f1"] = {
        "path": ["forest", "f1-score"],
        "val": 0.814,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["grass_f1"] = {
        "path": ["grass", "f1-score"],
        "val": 0.8696,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["other_f1"] = {
        "path": ["other", "f1-score"],
        "val": 0.7407,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["urban_f1"] = {
        "path": ["urban", "f1-score"],
        "val": 0.921,
        "thres": 0.05,
        "list": False,
    }
    thres_vals["omission"] = {
        "path": ["omission"],
        "val": [16.67, 16.0, 20.0, 6.67],
        "thres": 1.0,
        "list": True,
    }
    thres_vals["commission"] = {
        "path": ["commission"],
        "val": [20.38, 9.87, 31.03, 9.09],
        "thres": 1.0,
        "list": True,
    }
    thres_vals["producer_accuracy"] = {
        "path": ["producer_accuracy"],
        "val": [83.3, 84.0, 80.0, 93.3],
        "thres": 1.0,
        "list": True,
    }
    thres_vals["user_accuracy"] = {
        "path": ["user_accuracy"],
        "val": [79.62, 90.13, 68.97, 90.91],
        "thres": 1.0,
        "list": True,
    }

    vals_corr = True

    for cls in thres_vals:
        print(thres_vals[cls]["path"])
        if thres_vals[cls]["list"]:
            cls_val = dict_struct_get_str_list_value(acc_rslt, thres_vals[cls]["path"])
        else:
            cls_val = dict_struct_get_numeric_value(acc_rslt, thres_vals[cls]["path"])

        if not similar_numeric_vals(
            cls_val, thres_vals[cls]["val"], eql_thres=thres_vals[cls]["thres"]
        ):
            vals_corr = False
            print("\tFAILED")
            break
        else:
            print("\tOK")

    assert vals_corr


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval(tmp_path):
    import rsgislib.classification.classaccuracymetrics

    vec_file = os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson")
    vec_lyr = "gmw_acc_roi_1_cls_acc_pts_1"

    out_json_file = os.path.join(tmp_path, "out_acc_stats.json")

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval(
        vec_file,
        vec_lyr,
        ref_col,
        cls_col,
        out_json_file,
        sample_frac=0.2,
        sample_n_smps=100,
        bootstrap_n=100,
    )

    assert os.path.exists(out_json_file)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets(tmp_path):
    import rsgislib.classification.classaccuracymetrics
    import rsgislib.tools.filetools

    vec_files = [
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_2.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_3.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_4.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_5.geojson"),
    ]

    vec_lyrs = list()
    for vec_file in vec_files:
        vec_lyrs.append(rsgislib.tools.filetools.get_file_basename(vec_file))

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    out_plot_file = os.path.join(tmp_path, "out_plot.png")

    (
        conf_thres_met,
        conf_thres_met_idx,
        f1_scores,
        f1_scr_intervals_rgn,
    ) = rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets(
        vec_files,
        vec_lyrs,
        ref_col,
        cls_col,
        tmp_path,
        conf_inter=95,
        conf_thres=0.05,
        out_plot_file=out_plot_file,
        sample_frac=0.5,
        sample_n_smps=100,
        bootstrap_n=100,
    )

    assert os.path.exists(out_plot_file)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_summarise_multi_acc_ptonly_metrics(tmp_path):
    import rsgislib.classification.classaccuracymetrics
    import rsgislib.tools.filetools

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    vec_files = [
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_2.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_3.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_4.geojson"),
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_5.geojson"),
    ]

    acc_json_files = list()

    for vec_file in vec_files:
        vec_lyr = rsgislib.tools.filetools.get_file_basename(vec_file)
        out_json_file = os.path.join(tmp_path, "{}_stats.json".format(vec_lyr))
        rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples(
            vec_file, vec_lyr, ref_col, cls_col, out_json_file
        )
        acc_json_files.append(out_json_file)

    out_acc_json_sum_file = os.path.join(tmp_path, "out_acc_stats.json")
    rsgislib.classification.classaccuracymetrics.summarise_multi_acc_ptonly_metrics(
        acc_json_files, out_acc_json_sum_file
    )

    assert os.path.exists(out_json_file)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_calc_acc_metrics_vecsamples(tmp_path):
    import rsgislib.classification.classaccuracymetrics

    vec_file = os.path.join(CLASS_ACC_DATA_DIR, "cls_acc_assessment_pts_ref.geojson")
    vec_lyr = "cls_acc_assessment_pts_ref"

    in_cls_img = os.path.join(CLASS_ACC_DATA_DIR, "cls_rf_refl.kea")

    out_json_file = os.path.join(tmp_path, "out_acc_stats.json")
    out_csv_file = os.path.join(tmp_path, "out_acc_stats.csv")

    rsgislib.classification.classaccuracymetrics.calc_acc_metrics_vecsamples(
        vec_file=vec_file,
        vec_lyr=vec_lyr,
        ref_col="ref_pts",
        cls_col="rf_rl_cls",
        cls_img=in_cls_img,
        img_cls_name_col="class_names",
        img_hist_col="Histogram",
        out_json_file=out_json_file,
        out_csv_file=out_csv_file,
    )

    assert os.path.exists(out_json_file) and os.path.exists(out_csv_file)


def test_create_norm_modelled_err_matrix():
    from rsgislib.classification.classaccuracymetrics import (
        create_norm_modelled_err_matrix,
    )

    cls_areas = [40, 40, 10, 10]

    rel_err_mtx = [
        [75.0, 5.0, 5.0, 15.0],
        [5.0, 75.0, 2.0, 18.0],
        [20.0, 2.5, 75.0, 2.5],
        [2.5, 20.0, 2.5, 75.0],
    ]

    err_mtx = create_norm_modelled_err_matrix(cls_areas, rel_err_mtx)


def test_create_modelled_acc_pts():
    import rsgislib.classification.classaccuracymetrics

    cls_lst = [
        "Mangroves",
        "Not Mangroves",
        "Mangroves to Not Mangroves",
        "Not Mangroves to Mangroves",
    ]

    err_mtx_unit_area = [
        [0.378, 0.02, 0.002, 0.0],
        [0.02, 0.478, 0.0, 0.002],
        [0.02, 0.0, 0.03, 0.0],
        [0.0, 0.02, 0.0, 0.03],
    ]

    (
        ref_samples,
        pred_samples,
    ) = rsgislib.classification.classaccuracymetrics.create_modelled_acc_pts(
        err_mtx_unit_area, cls_lst, 1600
    )


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or SKLEARN_NOT_AVAIL),
    reason="matplotlib or scikit-learn dependency not available",
)
def test_calc_sampled_acc_metrics(tmp_path):
    from rsgislib.classification import classaccuracymetrics

    cls_lst = [
        "Mangroves",
        "Not Mangroves",
        "Mangroves to Not Mangroves",
        "Not Mangroves to Mangroves",
    ]

    err_mtx_unit_area = [
        [0.378, 0.02, 0.002, 0.0],
        [0.02, 0.478, 0.0, 0.002],
        [0.02, 0.0, 0.03, 0.0],
        [0.0, 0.02, 0.0, 0.03],
    ]

    (
        ref_samples,
        pred_samples,
    ) = classaccuracymetrics.create_modelled_acc_pts(err_mtx_unit_area, cls_lst, 2500)

    smpls_lst = [500, 1000, 1500, 2000]

    out_metrics_file = os.path.join(tmp_path, "out_stats_file.json")
    out_usr_metrics_plot = os.path.join(tmp_path, "usr_metrics_plot.png")
    out_prod_metrics_plot = os.path.join(tmp_path, "prod_metrics_plot.png")
    out_ref_usr_plot = os.path.join(tmp_path, "usr_ref_metrics_plot.png")
    out_ref_prod_plot = os.path.join(tmp_path, "prod_ref_metrics_plot.png")

    classaccuracymetrics.calc_sampled_acc_metrics(
        ref_samples,
        pred_samples,
        cls_lst,
        smpls_lst,
        out_metrics_file,
        n_repeats=10,
        out_usr_metrics_plot=out_usr_metrics_plot,
        out_prod_metrics_plot=out_prod_metrics_plot,
        out_ref_usr_plot=out_ref_usr_plot,
        out_ref_prod_plot=out_ref_prod_plot,
    )
