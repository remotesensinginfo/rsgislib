import os
import pytest

H5PY_NOT_AVAIL = False
try:
    import h5py
except ImportError:
    H5PY_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
ZONALSTATS_DATA_DIR = os.path.join(DATA_DIR, "zonalstats")


def test_ext_point_band_values_file(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pt_samples.geojson"
    )
    vec_lyr = "sen2_20210527_aber_pt_samples"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.ext_point_band_values_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        "testcolval",
        reproj_vec=False,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [39.0, 35.0, 124.0, 63.0, 36.0, 34.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_ext_point_band_values_file_reproj(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    vec_file = os.path.join(
        ZONALSTATS_DATA_DIR, "sen2_20210527_aber_wgs84_pt_samples.geojson"
    )
    vec_lyr = "sen2_20210527_aber_wgs84_pt_samples"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.ext_point_band_values_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        "testcolval",
        reproj_vec=True,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [33.0, 188.0, 34.0, 26.0, 79.0, 67.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 1:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Min(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field="testcolval",
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [26.0, 38.0, 37.0, 163.0, 65.0, 41.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Max(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field="testcolval",
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [32.0, 48.0, 46.0, 417.0, 73.0, 52.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Mean(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field=None,
        mean_field="testcolval",
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [
        29.299145299145298,
        43.858108108108105,
        41.67605633802817,
        309.48275862068965,
        69.29411764705883,
        46.58974358974359,
    ]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_StdDev(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field="testcolval",
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [
        1.1822400674899987,
        2.708869653402217,
        2.1609508072124695,
        68.0732975598059,
        1.8395877377778693,
        2.168751841414411,
    ]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Sum(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field="testcolval",
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [6856.0, 12982.0, 2959.0, 8975.0, 2356.0, 1817.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Count(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field="testcolval",
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [234.0, 296.0, 71.0, 29.0, 34.0, 39.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Mode(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field="testcolval",
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 46.0, 40.0, 328.0, 69.0, 46.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Median(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=None,
        percentile_field=None,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field="testcolval",
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 45.0, 42.0, 326.0, 69.0, 47.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_test_poly_pts_file_Percentile(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_test_poly_pts_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        percentile=75,
        percentile_field="testcolval",
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [30.0, 46.0, 43.0, 356.0, 70.0, 48.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_poly_pts_band_stats_file(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_poly_pts_band_stats_file(
        out_vec_file, out_vec_lyr, input_img, 1, "testcolval", vec_def_epsg=None
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [31.0, 46.0, 46.0, 326.0, 69.0, 49.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Min(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field="testcolval",
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [26.0, 38.0, 37.0, 163.0, 65.0, 41.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Max(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field="testcolval",
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [32.0, 48.0, 46.0, 417.0, 73.0, 52.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Mean(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field=None,
        mean_field="testcolval",
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [
        29.299145299145298,
        43.858108108108105,
        41.67605633802817,
        309.48275862068965,
        69.29411764705883,
        46.58974358974359,
    ]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_StdDev(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field="testcolval",
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [
        1.1822400674899987,
        2.708869653402217,
        2.1609508072124695,
        68.0732975598059,
        1.8395877377778693,
        2.168751841414411,
    ]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Sum(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field="testcolval",
        count_field=None,
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [6856.0, 12982.0, 2959.0, 8975.0, 2356.0, 1817.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Count(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field="testcolval",
        mode_field=None,
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [234.0, 296.0, 71.0, 29.0, 34.0, 39.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Mode(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field="testcolval",
        median_field=None,
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 46.0, 40.0, 328.0, 69.0, 46.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_calc_zonal_band_stats_file_Median(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.vectorutils
    import rsgislib.vectorattrs

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    rsgislib.zonalstats.calc_zonal_band_stats_file(
        out_vec_file,
        out_vec_lyr,
        input_img,
        1,
        0,
        1000,
        0,
        min_field=None,
        max_field=None,
        mean_field=None,
        stddev_field=None,
        sum_field=None,
        count_field=None,
        mode_field=None,
        median_field="testcolval",
        vec_def_epsg=None,
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "testcolval")
    print(vals)
    ref_vals = [29.0, 45.0, 42.0, 326.0, 69.0, 47.0]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if abs(val - ref_val) > 0.0001:
            vals_eq = False
            break
    assert vals_eq


def test_image_zone_to_hdf(tmp_path):
    import rsgislib.zonalstats

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polygons.geojson")
    vec_lyr = "sen2_20210527_aber_polygons"

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.image_zone_to_hdf(input_img, vec_file, vec_lyr, out_h5_file, no_prj_warn=False, pxl_in_poly_method=rsgislib.zonalstats.METHOD_POLYCONTAINSPIXELCENTER)

    assert os.path.exists(out_h5_file)


def test_extract_zone_img_values_to_hdf(tmp_path):
    import rsgislib.zonalstats
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_msk_img = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polys.kea")

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.extract_zone_img_values_to_hdf(input_img, in_msk_img, out_h5_file, 1, rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

def test_extract_zone_img_band_values_to_hdf(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_msk_img = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_polys.kea")

    in_img_info = []
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image1', [1, 3, 4]))
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image2', [2]))

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.extract_zone_img_band_values_to_hdf(in_img_info, in_msk_img, out_h5_file, 1, rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

def test_random_sample_hdf5_file(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")
    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.random_sample_hdf5_file(in_h5_file, out_h5_file, 250, 42, rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)


def test_split_sample_hdf5_file(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")
    out_h5_p1_file = os.path.join(tmp_path, "out_h5_p1_file.h5")
    out_h5_p2_file = os.path.join(tmp_path, "out_h5_p2_file.h5")

    rsgislib.zonalstats.split_sample_hdf5_file(in_h5_file, out_h5_p1_file, out_h5_p2_file, 250, 42, rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_p1_file) and os.path.exists(out_h5_p2_file)

def test_merge_extracted_hdf5_data(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")
    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.merge_extracted_hdf5_data([in_h5_file, in_h5_file], out_h5_file, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_msk_h5_smpls_to_finite_values(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_vals.h5")
    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.msk_h5_smpls_to_finite_values(in_h5_file, out_h5_file, datatype=rsgislib.TYPE_16INT, lower_limit=0, upper_limit=1000)

    assert os.path.exists(out_h5_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_extract_chip_zone_image_band_values_to_hdf_no_rot(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_msk_img = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pts.kea")

    in_img_info = []
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image1', [1, 3, 4]))
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image2', [2]))

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf(in_img_info, in_msk_img, 1, 20, out_h5_file, rotate_chips=None, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_extract_chip_zone_image_band_values_to_hdf_with_rot(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_msk_img = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pts.kea")

    in_img_info = []
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image1', [1, 3, 4]))
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image2', [2]))

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.extract_chip_zone_image_band_values_to_hdf(in_img_info, in_msk_img, 1, 20, out_h5_file, rotate_chips=[30, 60], datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_extract_ref_chip_zone_image_band_values_to_hdf_no_rot(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    in_msk_img = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pts.kea")

    in_img_info = []
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image1', [1, 3, 4]))
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image2', [2]))

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.extract_ref_chip_zone_image_band_values_to_hdf(in_img_info, in_ref_img, 1, in_msk_img, 1, 20, out_h5_file, rotate_chips=None, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_extract_ref_chip_zone_image_band_values_to_hdf_with_rot(tmp_path):
    import rsgislib.zonalstats
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    in_msk_img = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_pts.kea")

    in_img_info = []
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image1', [1, 3, 4]))
    in_img_info.append(rsgislib.imageutils.ImageBandInfo(input_img, 'Image2', [2]))

    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.extract_ref_chip_zone_image_band_values_to_hdf(in_img_info, in_ref_img, 1, in_msk_img, 1, 20, out_h5_file, rotate_chips=[30, 60], datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_split_sample_chip_hdf5_file(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_chip_vals.h5")
    out_h5_p1_file = os.path.join(tmp_path, "out_h5_p1_file.h5")
    out_h5_p2_file = os.path.join(tmp_path, "out_h5_p2_file.h5")

    rsgislib.zonalstats.split_sample_chip_hdf5_file(in_h5_file, out_h5_p1_file, out_h5_p2_file, 3, 42, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_p1_file) and os.path.exists(out_h5_p2_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_merge_extracted_hdf5_chip_data(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_chip_vals.h5")
    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.merge_extracted_hdf5_chip_data([in_h5_file, in_h5_file], out_h5_file, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)


@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_split_sample_ref_chip_hdf5_file(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_refchip_vals.h5")
    out_h5_p1_file = os.path.join(tmp_path, "out_h5_p1_file.h5")
    out_h5_p2_file = os.path.join(tmp_path, "out_h5_p2_file.h5")

    rsgislib.zonalstats.split_sample_ref_chip_hdf5_file(in_h5_file, out_h5_p1_file, out_h5_p2_file, 3, 42, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_p1_file) and os.path.exists(out_h5_p2_file)

@pytest.mark.skipif(H5PY_NOT_AVAIL, reason="h5py dependency not available")
def test_merge_extracted_hdf5_chip_ref_data(tmp_path):
    import rsgislib.zonalstats

    in_h5_file = os.path.join(ZONALSTATS_DATA_DIR, "sen2_20210527_aber_b1-6_refchip_vals.h5")
    out_h5_file = os.path.join(tmp_path, "out_h5_file.h5")

    rsgislib.zonalstats.merge_extracted_hdf5_chip_ref_data([in_h5_file, in_h5_file], out_h5_file, datatype=rsgislib.TYPE_16INT)

    assert os.path.exists(out_h5_file)

