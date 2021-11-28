import os
import pytest

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CLASS_ACC_DATA_DIR = os.path.join(DATA_DIR, "classification", "accuracy")

def test_calc_acc_ptonly_metrics_vecsamples(tmp_path):
    import rsgislib.classification.classaccuracymetrics

    vec_file = os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson")
    vec_lyr = "gmw_acc_roi_1_cls_acc_pts_1"

    out_json_file = os.path.join(tmp_path, "out_acc_stats.json")
    out_csv_file = os.path.join(tmp_path, "out_acc_stats.csv")

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples(vec_file, vec_lyr, ref_col, cls_col, out_json_file, out_csv_file)

    assert os.path.exists(out_json_file) and os.path.exists(out_csv_file)

def test_calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval(tmp_path):
    import rsgislib.classification.classaccuracymetrics

    vec_file = os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson")
    vec_lyr = "gmw_acc_roi_1_cls_acc_pts_1"

    out_json_file = os.path.join(tmp_path, "out_acc_stats.json")

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_bootstrap_conf_interval(vec_file, vec_lyr, ref_col, cls_col, out_json_file, sample_frac=0.2, sample_n_smps=100, bootstrap_n=100)

    assert os.path.exists(out_json_file)

def test_calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets(tmp_path):
    import rsgislib.classification.classaccuracymetrics
    import rsgislib.tools.filetools

    vec_files = [os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_1.geojson"),
                 os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_2.geojson"),
                 os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_3.geojson"),
                 os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_4.geojson"),
                 os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_5.geojson")]

    vec_lyrs = list()
    for vec_file in vec_files:
        vec_lyrs.append(rsgislib.tools.filetools.get_file_basename(vec_file))

    ref_col = "ref_cls"
    cls_col = "gmw_v25_cls"

    out_plot_file = os.path.join(tmp_path, "out_plot.png")

    conf_thres_met, conf_thres_met_idx, f1_scores, f1_scr_intervals_rgn = rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples_f1_conf_inter_sets(vec_files, vec_lyrs, ref_col, cls_col, tmp_path, conf_inter=95, conf_thres=0.05, out_plot_file=out_plot_file, sample_frac=0.5, sample_n_smps=100, bootstrap_n=100)

    assert os.path.exists(out_plot_file)


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
        os.path.join(CLASS_ACC_DATA_DIR, "gmw_acc_roi_1_cls_acc_pts_5.geojson")]

    acc_json_files = list()

    for vec_file in vec_files:
        vec_lyr = rsgislib.tools.filetools.get_file_basename(vec_file)
        out_json_file = os.path.join(tmp_path, "{}_stats.json".format(vec_lyr))
        rsgislib.classification.classaccuracymetrics.calc_acc_ptonly_metrics_vecsamples(vec_file, vec_lyr, ref_col, cls_col, out_json_file)
        acc_json_files.append(out_json_file)

    out_acc_json_sum_file = os.path.join(tmp_path, "out_acc_stats.json")
    rsgislib.classification.classaccuracymetrics.summarise_multi_acc_ptonly_metrics(acc_json_files, out_acc_json_sum_file)

    assert os.path.exists(out_json_file)

