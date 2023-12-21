import os
import pytest

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")
CALCPTWIN_DATA_DIR = os.path.join(IMGCALC_DATA_DIR, "calc_pt_win_smpls")


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_count_pxls_of_val_band1(tmp_path):
    import rsgislib.imagecalc.calc_pt_win_smpls

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    vec_file = os.path.join(CALCPTWIN_DATA_DIR, "calc_pt_win_smpls_debug.geojson")
    vec_lyr = "calc_pt_win_smpls_debug"
    sum_img_obj = rsgislib.imagecalc.calc_pt_win_smpls.RSGISDebugExportImg(
        tmp_path, "test_img"
    )

    out_vec_file = os.path.join(tmp_path, "debug_output.geojson")
    out_vec_lyr = "debug_output"

    rsgislib.imagecalc.calc_pt_win_smpls.calc_pt_smpl_img_vals(
        input_img,
        vec_file,
        vec_lyr,
        calc_objs=[sum_img_obj],
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col="angle",
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0.0,
    )

    success = True
    if not os.path.exists(out_vec_file):
        success = False

    out_img_0 = os.path.join(tmp_path, "0_test_img.tif")
    if not os.path.exists(out_img_0):
        success = False
    else:
        ref_img_0 = os.path.join(CALCPTWIN_DATA_DIR, "0_test_img_debug_ref.tif")
        img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(out_img_0, ref_img_0)
        if not img_eq:
            success = False

    out_img_1 = os.path.join(tmp_path, "1_test_img.tif")
    if not os.path.exists(out_img_1):
        success = False
    else:
        ref_img_1 = os.path.join(CALCPTWIN_DATA_DIR, "1_test_img_debug_ref.tif")
        img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(out_img_1, ref_img_1)
        if not img_eq:
            success = False

    out_img_2 = os.path.join(tmp_path, "2_test_img.tif")
    if not os.path.exists(out_img_2):
        success = False
    else:
        ref_img_2 = os.path.join(CALCPTWIN_DATA_DIR, "2_test_img_debug_ref.tif")
        img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(out_img_2, ref_img_2)
        if not img_eq:
            success = False

    assert success
