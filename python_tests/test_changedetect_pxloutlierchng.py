import os
import pytest

PYOD_NOT_AVAIL = False
try:
    import pyod
except ImportError:
    PYOD_NOT_AVAIL = True

MATPLOTLIB_NOT_AVAIL = False
try:
    import matplotlib.pyplot
except ImportError:
    MATPLOTLIB_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
CHANGEDETECT_DATA_DIR = os.path.join(DATA_DIR, "changedetect")


@pytest.mark.skipif(PYOD_NOT_AVAIL, reason="pyod dependency not available")
def test_find_class_pyod_outliers(tmp_path):
    import rsgislib.changedetect.pxloutlierchng
    import pyod.models.knn

    pyod_obj = pyod.models.knn.KNN()
    input_img = os.path.join(CHANGEDETECT_DATA_DIR, "LS8_20180608_ndvi_sub.kea")
    in_msk_img = os.path.join(CHANGEDETECT_DATA_DIR, "base_1997_class_img_sub.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    out_scores_img = os.path.join(tmp_path, "out_score_img.kea")

    rsgislib.changedetect.pxloutlierchng.find_class_pyod_outliers(
        pyod_obj,
        input_img,
        in_msk_img,
        output_img,
        out_scores_img=out_scores_img,
        img_mask_val=1,
        img_bands=None,
        gdalformat="KEA",
    )

    assert os.path.exists(output_img) and os.path.exists(out_scores_img)


@pytest.mark.skipif(MATPLOTLIB_NOT_AVAIL, reason="matplotlib dependency not available")
def test_find_class_kurt_skew_outliers(tmp_path):
    from rsgislib.changedetect.pxloutlierchng import find_class_kurt_skew_outliers

    input_img = os.path.join(CHANGEDETECT_DATA_DIR, "LS8_20180608_ndvi_sub.kea")
    in_msk_img = os.path.join(CHANGEDETECT_DATA_DIR, "base_1997_class_img_sub.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    plot_thres_file = os.path.join(tmp_path, "out_plot.png")

    find_class_kurt_skew_outliers(
        input_img,
        in_msk_img,
        output_img,
        vld_min=-1,
        vld_max=1,
        init_thres=0.25,
        low_thres=True,
        contamination=10.0,
        only_kurtosis=False,
        img_mask_val=1,
        img_band=1,
        img_val_no_data=-999,
        gdalformat="KEA",
        plot_thres_file=plot_thres_file,
    )

    assert os.path.exists(output_img)


@pytest.mark.skipif(MATPLOTLIB_NOT_AVAIL, reason="matplotlib dependency not available")
def test_find_class_otsu_outliers(tmp_path):
    from rsgislib.changedetect.pxloutlierchng import find_class_otsu_outliers

    input_img = os.path.join(CHANGEDETECT_DATA_DIR, "LS8_20180608_ndvi_sub.kea")
    in_msk_img = os.path.join(CHANGEDETECT_DATA_DIR, "base_1997_class_img_sub.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    plot_thres_file = os.path.join(tmp_path, "out_plot.png")

    find_class_otsu_outliers(
        input_img,
        in_msk_img,
        output_img,
        low_thres=True,
        img_mask_val=1,
        img_band=1,
        img_val_no_data=-999,
        gdalformat="KEA",
        plot_thres_file=plot_thres_file,
    )

    assert os.path.exists(output_img)


@pytest.mark.skipif(MATPLOTLIB_NOT_AVAIL, reason="matplotlib dependency not available")
def test_find_class_li_outliers(tmp_path):
    from rsgislib.changedetect.pxloutlierchng import find_class_li_outliers

    input_img = os.path.join(CHANGEDETECT_DATA_DIR, "LS8_20180608_ndvi_sub.kea")
    in_msk_img = os.path.join(CHANGEDETECT_DATA_DIR, "base_1997_class_img_sub.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    plot_thres_file = os.path.join(tmp_path, "out_plot.png")

    find_class_li_outliers(
        input_img,
        in_msk_img,
        output_img,
        low_thres=True,
        tolerance=None,
        init_thres=0.35,
        img_mask_val=1,
        img_band=1,
        img_val_no_data=-999,
        gdalformat="KEA",
        plot_thres_file=plot_thres_file,
    )

    assert os.path.exists(output_img)
