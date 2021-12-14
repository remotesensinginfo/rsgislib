import os
from shutil import copy2

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
RASTERGIS_DATA_DIR = os.path.join(DATA_DIR, "rastergis")


def test_get_rat_length():
    import rsgislib.rastergis

    ref_clumps_img = os.path.join(
        RASTERGIS_DATA_DIR, "sen2_20210527_aber_clumps_attref.kea"
    )

    n_clumps = rsgislib.rastergis.get_rat_length(ref_clumps_img)
    assert n_clumps == 11949


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


def test_get_rat_columns_info():
    import rsgislib.rastergis
    import osgeo.gdal as gdal

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


def test_populate_rat_with_stats(tmp_path):
    import rsgislib.rastergis
    import rsgislib.rastergis.ratutils
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

        ref_vals = rsgislib.rastergis.ratutils.get_column_data(ref_clumps_img, var)
        calcd_vals = rsgislib.rastergis.ratutils.get_column_data(clumps_img, var)
        if calcd_vals.shape[0] != ref_vals.shape[0]:
            vars_eq_vals = False
            break
        if numpy.sum(calcd_vals) != numpy.sum(ref_vals):
            vars_eq_vals = False
            break

    assert vars_eq_vals


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


# TODO rsgislib.rastergis.collapse_rat


def test_calc_border_length(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.calc_border_length(clumps_img, "out_col", True)


# TODO rsgislib.rastergis.calc_rel_border
# TODO rsgislib.rastergis.calc_rel_diff_neigh_stats


def test_define_border_clumps(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.define_border_clumps(clumps_img, "out_col")


# TODO rsgislib.rastergis.define_clump_tile_positions


def test_find_boundary_pixels(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.rastergis.find_boundary_pixels(clumps_img, output_img, "KEA", 1)

    assert os.path.exists(output_img)


def test_find_neighbours(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.find_neighbours(clumps_img, 1)


# TODO rsgislib.rastergis.populate_rat_with_cat_proportions
# TODO rsgislib.rastergis.populate_rat_with_percentiles
# TODO rsgislib.rastergis.populate_rat_with_meanlit_stats
# TODO rsgislib.rastergis.select_clumps_on_grid


def test_clumps_spatial_location(tmp_path):
    import rsgislib.rastergis

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    clumps_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, clumps_img)

    rsgislib.rastergis.clumps_spatial_location(
        clumps_img, eastings="eastings", northings="northings"
    )


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


# TODO rsgislib.rastergis.str_class_majority
# TODO rsgislib.rastergis.populate_rat_with_mode
# TODO rsgislib.rastergis.populate_rat_with_prop_valid_pxls
# TODO rsgislib.rastergis.histo_sampling
# TODO rsgislib.rastergis.class_split_fit_hist_gausian_mixture_model
# TODO rsgislib.rastergis.apply_rat_knn
# TODO rsgislib.rastergis.get_global_class_stats
# TODO rsgislib.rastergis.fit_hist_gausian_mixture_model
# TODO rsgislib.rastergis.calc_1d_jm_distance
# TODO rsgislib.rastergis.calc_2d_jm_distance
# TODO rsgislib.rastergis.calc_bhattacharyya_distance
# TODO rsgislib.rastergis.export_rat_cols_to_ascii
# TODO rsgislib.rastergis.export_col_to_gdal_img
# TODO rsgislib.rastergis.export_cols_to_gdal_img
# TODO rsgislib.rastergis.export_clumps_to_images
# TODO rsgislib.rastergis.copy_gdal_rat_columns
# TODO rsgislib.rastergis.copy_rat
# TODO rsgislib.rastergis.import_vec_atts
# TODO rsgislib.rastergis.colour_rat_classes
