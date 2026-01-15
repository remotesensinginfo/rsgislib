import os
from shutil import copy2
import sys
import pytest

os_pltform = sys.platform

ON_MACOS = False
if os_pltform == "darwin":
    ON_MACOS = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
RASTERGIS_DATA_DIR = os.path.join(DATA_DIR, "rastergis")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_get_rat_length():
    import rsgislib.rastergis

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    n_clumps = rsgislib.rastergis.get_rat_length(ref_clumps_img)
    assert n_clumps == 11949

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_get_rat_columns():
    import rsgislib.rastergis

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    rat_columns = rsgislib.rastergis.get_rat_columns(ref_clumps_img)
    ref_columns = ["Histogram", "Red", "Green", "Blue", "Alpha"]
    for i in [1, 2, 3, 4]:
        ref_columns.append("b{}Min".format(i))
        ref_columns.append("b{}Max".format(i))
        ref_columns.append("b{}Mean".format(i))
        ref_columns.append("b{}Sum".format(i))
        ref_columns.append("b{}StdDev".format(i))

    cols_match = True
    if len(rat_columns) != len(ref_columns):
        cols_match = False
        print(
            "Number of columns is different: {} != {}".format(
                len(rat_columns), len(ref_columns)
            )
        )

    if cols_match:
        for col in rat_columns:
            if col not in ref_columns:
                print("Col '{}' is not within the reference list".format(col))
                cols_match = False
                break
    assert cols_match

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_get_rat_columns_info():
    import rsgislib.rastergis
    from osgeo import gdal

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    rat_columns_info = rsgislib.rastergis.get_rat_columns_info(ref_clumps_img)

    correct_info = True
    if rat_columns_info["Histogram"]["type"] != gdal.GFT_Real:
        correct_info = False
    if rat_columns_info["Histogram"]["usage"] != gdal.GFU_PixelCount:
        correct_info = False

    if rat_columns_info["Red"]["type"] != gdal.GFT_Integer:
        correct_info = False
    if rat_columns_info["Red"]["usage"] != gdal.GFU_Red:
        correct_info = False

    if rat_columns_info["b1Mean"]["type"] != gdal.GFT_Real:
        correct_info = False
    if rat_columns_info["b1Mean"]["usage"] != gdal.GFU_Generic:
        correct_info = False

    assert correct_info

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_populate_rat_with_stats(tmp_path):
    import rsgislib.rastergis
    import numpy

    base_clumps_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(base_clumps_img, clumps_img)

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    band_stats = list()
    band_stats.append(
        rsgislib.rastergis.BandAttStats(
            band=1,
            min_field="b1Min",
            max_field="b1Max",
            sum_field="b1Sum",
            std_dev_field="b1StdDev",
            mean_field="b1Mean",
        )
    )
    band_stats.append(
        rsgislib.rastergis.BandAttStats(
            band=2,
            min_field="b2Min",
            max_field="b2Max",
            sum_field="b2Sum",
            std_dev_field="b2StdDev",
            mean_field="b2Mean",
        )
    )
    band_stats.append(
        rsgislib.rastergis.BandAttStats(
            band=3,
            min_field="b3Min",
            max_field="b3Max",
            sum_field="b3Sum",
            std_dev_field="b3StdDev",
            mean_field="b3Mean",
        )
    )
    band_stats.append(
        rsgislib.rastergis.BandAttStats(
            band=4,
            min_field="b4Min",
            max_field="b4Max",
            sum_field="b4Sum",
            std_dev_field="b4StdDev",
            mean_field="b4Mean",
        )
    )

    rsgislib.rastergis.populate_rat_with_stats(input_img, clumps_img, band_stats)

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    vars_to_test = [
        "Histogram",
        "b1Min",
        "b2Max",
        "b3Sum",
        "b4StdDev",
        "b1Mean",
        "b2Mean",
        "b3Mean",
        "b4Mean",
    ]
    vars_eq_vals = True
    for var in vars_to_test:
        print("Testing: {}".format(var))

        ref_vals = rsgislib.rastergis.get_column_data(ref_clumps_img, var)
        calcd_vals = rsgislib.rastergis.get_column_data(clumps_img, var)
        if calcd_vals.shape[0] != ref_vals.shape[0]:
            vars_eq_vals = False
            break
        if numpy.sum(calcd_vals) != numpy.sum(ref_vals):
            vars_eq_vals = False
            break

    assert vars_eq_vals

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_pop_rat_img_stats(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_nostats.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_nostats.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.pop_rat_img_stats(
        clumps_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_collapse_rat(tmp_path):
    import rsgislib.rastergis

    clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_cls_out.kea"
    )
    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.rastergis.collapse_rat(clumps_img, "OutClassName", output_img, "KEA")

    assert os.path.exists(output_img)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_calc_border_length(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.calc_border_length(clumps_img, "out_col", True)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_calc_rel_border(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_cls_out.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_cls_out.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.calc_rel_border(
        clumps_img, "rel_border_forest", "OutClassName", "Forest", True
    )


@pytest.mark.skipif(
    True,
    reason="Sometimes a seg fault with calc_rel_diff_neigh_stats which haven't found fix for yet.",
)
def test_calc_rel_diff_neigh_stats(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_cls_out.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_cls_out.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.find_neighbours(clumps_img, 1)

    fieldInfo = rsgislib.rastergis.FieldAttStats(
        field="b8_mean", min_field="MinNIRMeanDiff", max_field="MaxNIRMeanDiff"
    )
    rsgislib.rastergis.calc_rel_diff_neigh_stats(clumps_img, fieldInfo, False, 1)


@pytest.mark.skipif(
    True,
    reason="Sometimes a seg fault with calc_rel_diff_neigh_stats which haven't found fix for yet.",
)
def test_calc_rel_diff_neigh_stats_abs(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_cls_out.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_cls_out.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.find_neighbours(clumps_img, 1)

    fieldInfo = rsgislib.rastergis.FieldAttStats(
        field="b8_mean", min_field="MinNIRMeanDiff", max_field="MaxNIRMeanDiff"
    )
    rsgislib.rastergis.calc_rel_diff_neigh_stats(clumps_img, fieldInfo, True, 1)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_define_border_clumps(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.define_border_clumps(clumps_img, "out_col")


# TODO rsgislib.rastergis.define_clump_tile_positions

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_find_boundary_pixels(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.rastergis.find_boundary_pixels(clumps_img, output_img, "KEA", 1)

    assert os.path.exists(output_img)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_find_neighbours(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.find_neighbours(clumps_img, 1)


# TODO rsgislib.rastergis.populate_rat_with_cat_proportions

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_populate_rat_with_percentiles(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    band_percents = []
    band_percents.append(
        rsgislib.rastergis.BandAttPercentiles(percentile=25.0, field_name="B1Per25")
    )
    band_percents.append(
        rsgislib.rastergis.BandAttPercentiles(percentile=50.0, field_name="B1Per50")
    )
    band_percents.append(
        rsgislib.rastergis.BandAttPercentiles(percentile=75.0, field_name="B1Per75")
    )
    rsgislib.rastergis.populate_rat_with_percentiles(
        input_img, clumps_img, 1, band_percents
    )

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_populate_rat_with_meanlit_stats(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_cls_out.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_cls_out.kea")
    copy2(input_ref_img, clumps_img)

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    input_ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")

    band_stats = list()
    band_stats.append(rsgislib.rastergis.BandAttStats(band=1, mean_field="b1_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=2, mean_field="b2_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=3, mean_field="b3_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=4, mean_field="b4_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=5, mean_field="b5_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=6, mean_field="b6_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=7, mean_field="b7_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=8, mean_field="b8_meanlit"))
    band_stats.append(rsgislib.rastergis.BandAttStats(band=9, mean_field="b9_meanlit"))
    band_stats.append(
        rsgislib.rastergis.BandAttStats(band=10, mean_field="b10_meanlit")
    )

    rsgislib.rastergis.populate_rat_with_meanlit_stats(
        input_img,
        clumps_img,
        input_ndvi_img,
        1,
        "ndvi_mean",
        "meanlit_pxl_ct",
        band_stats,
    )


# TODO rsgislib.rastergis.select_clumps_on_grid

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_clumps_spatial_location(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.clumps_spatial_location(
        clumps_img, eastings="eastings", northings="northings"
    )

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_clumps_spatial_extent(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.clumps_spatial_extent(
        clumps_img,
        min_xx="min_xx",
        min_xy="min_xy",
        max_xx="max_xx",
        max_xy="max_xy",
        min_yx="min_yx",
        min_yy="min_yy",
        max_yx="max_yx",
        max_yy="max_yy",
        rat_band=1,
    )

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_populate_rat_with_mode(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    in_cls_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")

    rsgislib.rastergis.populate_rat_with_mode(
        in_cls_img,
        clumps_img,
        out_cols_name="cls_val",
        use_no_data=True,
        no_data_val=0,
        out_no_data=0,
        mode_band=1,
        rat_band=1,
    )

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_populate_rat_with_prop_valid_pxls(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    in_cls_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")

    rsgislib.rastergis.populate_rat_with_prop_valid_pxls(
        in_cls_img, clumps_img, out_col="cls_val", no_data_val=0, rat_band=1
    )

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_export_col_to_gdal_img(tmp_path):
    import rsgislib.rastergis

    clumps_img = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.rastergis.export_col_to_gdal_img(
        clumps_img, output_img, "KEA", rsgislib.TYPE_32FLOAT, "b6Mean", rat_band=1
    )

    assert os.path.exists(output_img)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_export_cols_to_gdal_img(tmp_path):
    import rsgislib.rastergis

    clumps_img = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.rastergis.export_cols_to_gdal_img(
        clumps_img,
        output_img,
        "KEA",
        rsgislib.TYPE_32FLOAT,
        ["b1Mean", "b2Mean", "b3Mean"],
        rat_band=1,
    )

    assert os.path.exists(output_img)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_export_rat_cols_to_ascii(tmp_path):
    import rsgislib.rastergis

    clumps_img = os.path.join(
        CLASSIFICATION_DATA_DIR, "sen2_20210527_aber_clumps_s2means.kea"
    )

    out_file = os.path.join(tmp_path, "out_data.txt")
    rsgislib.rastergis.export_rat_cols_to_ascii(
        clumps_img, out_file, ["b1Mean", "b2Mean", "b3Mean"], rat_band=1
    )

    assert os.path.exists(out_file)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_export_clumps_to_images(tmp_path):
    import rsgislib.rastergis
    import glob

    clumps_img = os.path.join(RASTERGIS_DATA_DIR, "sen2_grid_clumps.kea")
    out_img_base = os.path.join(tmp_path, "out_img_")

    rsgislib.rastergis.export_clumps_to_images(
        clumps_img, out_img_base, True, "kea", "KEA", rat_band=1
    )

    assert len(glob.glob("{}*.kea".format(out_img_base))) == 4

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_get_column_data():
    import rsgislib.rastergis
    import numpy

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    hist_col_vals = rsgislib.rastergis.get_column_data(ref_clumps_img, "Histogram")

    hist_vals_range_ok = False
    if (
        (numpy.min(hist_col_vals) >= 0)
        and (numpy.max(hist_col_vals) <= 80174)
        and (hist_col_vals.shape[0] == 11949)
    ):
        hist_col_vals = True

    assert hist_col_vals

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_set_column_data(tmp_path):
    import rsgislib.rastergis
    import numpy

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    n_rows = rsgislib.rastergis.get_rat_length(clumps_img)
    uid_col = numpy.arange(0, n_rows, 1, dtype=numpy.uint32)
    rsgislib.rastergis.set_column_data(clumps_img, "test_col", uid_col)

    read_col_vals = rsgislib.rastergis.get_column_data(clumps_img, "test_col")
    assert numpy.array_equal(read_col_vals, uid_col)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_create_uid_col(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.create_uid_col(clumps_img)

@pytest.mark.skipif(ON_MACOS, reason="skipping MacOS due to KEA/HDF5 issues")
def test_get_global_class_stats(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_cls_out.kea"
    )
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps_cls_out.kea")
    copy2(input_ref_img, clumps_img)

    change_feat_vals = list()
    change_feat_vals.append(rsgislib.rastergis.ChangeFeats(cls_name="Forest"))
    change_feat_vals.append(rsgislib.rastergis.ChangeFeats(cls_name="Grass"))

    rsgislib.rastergis.get_global_class_stats(
        clumps_img, "OutClassName", ["ndvi_mean"], change_feat_vals
    )


# TODO rsgislib.rastergis.str_class_majority
# TODO rsgislib.rastergis.histo_sampling
# TODO rsgislib.rastergis.class_split_fit_hist_gausian_mixture_model
# TODO rsgislib.rastergis.apply_rat_knn
# TODO rsgislib.rastergis.get_global_class_stats
# TODO rsgislib.rastergis.fit_hist_gausian_mixture_model
# TODO rsgislib.rastergis.calc_1d_jm_distance
# TODO rsgislib.rastergis.calc_2d_jm_distance
# TODO rsgislib.rastergis.calc_bhattacharyya_distance
# TODO rsgislib.rastergis.copy_gdal_rat_columns
# TODO rsgislib.rastergis.copy_rat
# TODO rsgislib.rastergis.import_vec_atts
# TODO rsgislib.rastergis.colour_rat_classes
# TODO rsgislib.rastergis.define_class_names
# TODO rsgislib.rastergis.take_random_sample
# TODO rsgislib.rastergis.set_class_names_colours
# TODO rsgislib.rastergis.calc_dist_between_clumps
# TODO rsgislib.rastergis.calc_dist_to_large_clumps
# TODO rsgislib.rastergis.calc_dist_to_classes
# TODO rsgislib.rastergis.identify_small_units
