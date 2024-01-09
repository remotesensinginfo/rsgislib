import os
import glob
import pytest

MATPLOTLIB_NOT_AVAIL = False
try:
    import matplotlib.pyplot
except ImportError:
    MATPLOTLIB_NOT_AVAIL = True

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data", "elevation")
ROUGH_DATA_DIR = os.path.join(DATA_DIR, "roughness")


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_calc_simple_roughness_profile_metrics(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    rsgislib.elevation.roughness.calc_simple_roughness_profile_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        detrend_poly_order=1,
        export_plot_path=None,
    )

    x_MIF_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="x_MIF"
    )
    y_rv_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="y_rv"
    )
    sum_rp_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="sum_rp"
    )
    avg_rz_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_rz"
    )
    x_rsk_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="x_rsk"
    )
    avg_rku_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_rku"
    )

    assert (
        (len(x_MIF_arr) == 3)
        and (len(y_rv_arr) == 3)
        and (len(sum_rp_arr) == 3)
        and (len(avg_rz_arr) == 3)
        and (len(x_rsk_arr) == 3)
        and (len(avg_rku_arr) == 3)
    )


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or GEOPANDAS_NOT_AVAIL),
    reason="matplotlib or geopandas dependency not available",
)
def test_calc_simple_roughness_profile_metrics_plots(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    out_plots_dir = os.path.join(tmp_path, "plots")
    if not os.path.exists(out_plots_dir):
        os.mkdir(out_plots_dir)

    rsgislib.elevation.roughness.calc_simple_roughness_profile_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        detrend_poly_order=1,
        export_plot_path=out_plots_dir,
    )

    plot_imgs = glob.glob(os.path.join(out_plots_dir, "*.png"))

    assert len(plot_imgs) == 3


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_calc_munro_roughness_profile_metrics(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    rsgislib.elevation.roughness.calc_munro_roughness_profile_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        detrend_poly_order=1,
        export_plot_path=None,
    )

    x_munro_peaks_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="x_munro_peaks"
    )
    y_munro_area_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="y_munro_area"
    )
    sum_munro_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="sum_munro_z0"
    )
    avg_munro_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_munro_z0"
    )

    assert (
        (len(x_munro_peaks_arr) == 3)
        and (len(y_munro_area_arr) == 3)
        and (len(sum_munro_z0_arr) == 3)
        and (len(avg_munro_z0_arr) == 3)
    )


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or GEOPANDAS_NOT_AVAIL),
    reason="matplotlib or geopandas dependency not available",
)
def test_calc_munro_roughness_profile_metrics_plots(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    out_plots_dir = os.path.join(tmp_path, "plots")
    if not os.path.exists(out_plots_dir):
        os.mkdir(out_plots_dir)

    rsgislib.elevation.roughness.calc_munro_roughness_profile_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        detrend_poly_order=1,
        export_plot_path=out_plots_dir,
    )

    plot_imgs = glob.glob(os.path.join(out_plots_dir, "*.png"))

    assert len(plot_imgs) == 3


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_calc_smith_roughness_metrics(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    rsgislib.elevation.roughness.calc_smith_roughness_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        export_plot_path=None,
    )

    smith_hs_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="smith_hs"
    )
    xe_smith_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="xe_smith_z0"
    )
    ys_smith_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="ys_smith_z0"
    )
    avg_smith_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_smith_z0"
    )

    assert (
        (len(smith_hs_arr) == 3)
        and (len(xe_smith_z0_arr) == 3)
        and (len(ys_smith_z0_arr) == 3)
        and (len(avg_smith_z0_arr) == 3)
    )


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or GEOPANDAS_NOT_AVAIL),
    reason="matplotlib or geopandas dependency not available",
)
def test_calc_smith_roughness_metrics_plots(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    out_plots_dir = os.path.join(tmp_path, "plots")
    if not os.path.exists(out_plots_dir):
        os.mkdir(out_plots_dir)

    rsgislib.elevation.roughness.calc_smith_roughness_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        export_plot_path=out_plots_dir,
    )

    plot_imgs = glob.glob(os.path.join(out_plots_dir, "*.png"))

    assert len(plot_imgs) == 3


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_calc_all_roughness_profile_metrics(tmp_path):
    import rsgislib.elevation.roughness
    import rsgislib.vectorattrs

    input_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    vec_file = os.path.join(ROUGH_DATA_DIR, "rough_smpl_pts.geojson")
    vec_lyr = "rough_smpl_pts"

    out_vec_file = os.path.join(tmp_path, "out_pts.geojson")
    out_vec_lyr = "out_pts"

    rsgislib.elevation.roughness.calc_all_roughness_profile_metrics(
        input_dem_img,
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        interp_method=rsgislib.INTERP_CUBIC,
        angle_col=None,
        x_box_col="xbox",
        y_box_col="ybox",
        no_data_val=0,
        detrend=True,
        detrend_poly_order=1,
    )

    x_MIF_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="x_MIF"
    )
    y_rv_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="y_rv"
    )
    sum_rp_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="sum_rp"
    )
    avg_rz_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_rz"
    )
    x_rsk_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="x_rsk"
    )
    avg_rku_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_rku"
    )
    x_munro_peaks_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="x_munro_peaks"
    )
    y_munro_area_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="y_munro_area"
    )
    sum_munro_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="sum_munro_z0"
    )
    avg_munro_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_munro_z0"
    )
    smith_hs_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="smith_hs"
    )
    xe_smith_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="xe_smith_z0"
    )
    ys_smith_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="ys_smith_z0"
    )
    avg_smith_z0_arr = rsgislib.vectorattrs.read_vec_column(
        out_vec_file, out_vec_lyr, att_column="avg_smith_z0"
    )

    assert (
        (len(x_MIF_arr) == 3)
        and (len(y_rv_arr) == 3)
        and (len(sum_rp_arr) == 3)
        and (len(avg_rz_arr) == 3)
        and (len(x_rsk_arr) == 3)
        and (len(avg_rku_arr) == 3)
        and (len(x_munro_peaks_arr) == 3)
        and (len(y_munro_area_arr) == 3)
        and (len(sum_munro_z0_arr) == 3)
        and (len(avg_munro_z0_arr) == 3)
        and (len(smith_hs_arr) == 3)
        and (len(xe_smith_z0_arr) == 3)
        and (len(ys_smith_z0_arr) == 3)
        and (len(avg_smith_z0_arr) == 3)
    )
