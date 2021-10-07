import os

import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")


def test_count_pxls_of_val_band1():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    val_counts = rsgislib.imagecalc.count_pxls_of_val(
        input_img, [1, 2, 3, 4], img_band=1
    )
    assert (
        (val_counts[0] == 614)
        and (val_counts[1] == 612)
        and (val_counts[2] == 618)
        and (val_counts[3] == 656)
    )


def test_count_pxls_of_val_band1_selVals():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    val_counts = rsgislib.imagecalc.count_pxls_of_val(input_img, [2, 1], img_band=1)
    assert (val_counts[0] == 612) and (val_counts[1] == 614)


def test_count_pxls_of_val_all_bands():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    val_counts = rsgislib.imagecalc.count_pxls_of_val(
        input_img, [1, 2, 3, 4], img_band=None
    )
    assert (
        (val_counts[0] == 1890)
        and (val_counts[1] == 1868)
        and (val_counts[2] == 1861)
        and (val_counts[3] == 1881)
    )


def test_get_unique_values():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    unq_vals = rsgislib.imagecalc.get_unique_values(input_img, img_band=1)
    for val in [1, 2, 3, 4]:
        if val not in unq_vals:
            assert False
    assert True


def test_are_imgs_equal_true():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(input_img, input_img)
    assert img_eq


def test_are_imgs_equal_false():
    import rsgislib.imagecalc

    in_ref_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    in_cmp_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls_v2.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(in_ref_img, in_cmp_img)
    assert not img_eq


def test_are_img_bands_equal_true():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        input_img, 1, input_img, 1
    )
    assert img_eq


def test_are_img_bands_equal_difbands_false():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        input_img, 1, input_img, 2
    )
    assert not img_eq


def test_are_img_bands_equal_false():
    import rsgislib.imagecalc

    in_ref_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls.kea")
    in_cmp_img = os.path.join(IMGCALC_DATA_DIR, "test_int_pxls_v2.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        in_ref_img, 1, in_cmp_img, 1
    )
    assert not img_eq


def test_band_maths_sgl_band(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    band_def_seq = list()
    band_def_seq.append(
        rsgislib.imagecalc.BandDefn(band_name="Blue", input_img=input_img, img_band=1)
    )
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_b1.kea")
    rsgislib.imagecalc.band_math(
        output_img, "Blue", "KEA", rsgislib.TYPE_16UINT, band_defs=band_def_seq
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        input_img, 1, output_img, 1
    )
    assert img_eq


def test_band_maths_multi_band(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    ref_ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    band_def_seq = list()
    band_def_seq.append(
        rsgislib.imagecalc.BandDefn(band_name="red", input_img=input_img, img_band=3)
    )
    band_def_seq.append(
        rsgislib.imagecalc.BandDefn(band_name="nir", input_img=input_img, img_band=8)
    )
    output_img = os.path.join(tmp_path, "ndvi_test_band_maths.kea")
    exp = "(nir-red)/(nir+red)"
    rsgislib.imagecalc.band_math(
        output_img, exp, "KEA", rsgislib.TYPE_32FLOAT, band_defs=band_def_seq
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_ndvi_img, 1, output_img, 1
    )
    assert img_eq


def test_band_maths_binary_out(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    ref_ndvi_cats_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea"
    )
    band_def_seq = list()
    band_def_seq.append(
        rsgislib.imagecalc.BandDefn(band_name="ndvi", input_img=input_img, img_band=1)
    )
    output_img = os.path.join(tmp_path, "ndvi_cats_test_band_maths.kea")
    exp = "ndvi>0.95?1:ndvi>0.85?2:ndvi>0.75?3:0"
    rsgislib.imagecalc.band_math(
        output_img, exp, "KEA", rsgislib.TYPE_8UINT, band_defs=band_def_seq
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_ndvi_cats_img, 1, output_img, 1
    )
    assert img_eq


def test_band_maths_exp_err(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    band_def_seq = list()
    band_def_seq.append(
        rsgislib.imagecalc.BandDefn(band_name="ndvi", input_img=input_img, img_band=1)
    )
    output_img = os.path.join(tmp_path, "ndvi_cats_test_err.kea")
    exp = "ndvi>0.5?1:ndvi>0.75?2:0?"
    with pytest.raises(Exception):
        rsgislib.imagecalc.band_math(
            output_img, exp, "KEA", rsgislib.TYPE_32FLOAT, band_defs=band_def_seq
        )


def test_image_band_math_multi_band(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    ref_ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    output_img = os.path.join(tmp_path, "ndvi_test_imageband_math.kea")
    exp = "(b8-b3)/(b8+b3)"
    rsgislib.imagecalc.image_band_math(
        input_img, output_img, exp, "KEA", rsgislib.TYPE_32FLOAT
    )
    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_ndvi_img, 1, output_img, 1
    )
    assert img_eq


def test_image_math_binary_out(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    ref_ndvi_cats_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea"
    )
    output_img = os.path.join(tmp_path, "ndvi_cats_test_image_math.kea")
    exp = "b1>0.95?1:b1>0.85?2:b1>0.75?3:0"
    rsgislib.imagecalc.image_math(
        input_img, output_img, exp, "KEA", rsgislib.TYPE_8UINT
    )
    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_ndvi_cats_img, 1, output_img, 1
    )
    assert img_eq


def test_calc_dist_to_img_vals_sgl_val_geo(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_val2_geo.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals.kea")
    rsgislib.imagecalc.calc_dist_to_img_vals(
        input_img,
        output_img,
        2,
        img_band=1,
        gdalformat="KEA",
        max_dist=100,
        no_data_val=0,
        unit_geo=True,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert img_eq


def test_calc_dist_to_img_vals_sgl_val_pxl(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_val2_pxl.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals.kea")
    rsgislib.imagecalc.calc_dist_to_img_vals(
        input_img,
        output_img,
        2,
        img_band=1,
        gdalformat="KEA",
        max_dist=10,
        no_data_val=0,
        unit_geo=False,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert img_eq


def test_calc_dist_to_img_vals_multi_val_geo(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_vals23_geo.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals.kea")
    rsgislib.imagecalc.calc_dist_to_img_vals(
        input_img,
        output_img,
        [2, 3],
        img_band=1,
        gdalformat="KEA",
        max_dist=100,
        no_data_val=0,
        unit_geo=True,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert img_eq


def test_calc_dist_to_img_vals_multi_val_pxl(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_vals23_pxl.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals.kea")
    rsgislib.imagecalc.calc_dist_to_img_vals(
        input_img,
        output_img,
        [2, 3],
        img_band=1,
        gdalformat="KEA",
        max_dist=10,
        no_data_val=0,
        unit_geo=False,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert img_eq


def test_calc_dist_to_img_vals_tiled_sgl_val_geo(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_val2_geo.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals_tiled.kea")
    tmp_dist_dir = os.path.join(tmp_path, "dist_tmp")
    rsgislib.imagecalc.calc_dist_to_img_vals_tiled(
        input_img,
        output_img,
        2,
        img_band=1,
        max_dist=100,
        no_data_val=0,
        gdalformat="KEA",
        unit_geo=True,
        tmp_dir=tmp_dist_dir,
        tile_size=250,
        n_cores=1,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert prop_match > 0.99


def test_calc_dist_to_img_vals_tiled_sgl_val_pxl(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_val2_pxl.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals_tiled.kea")
    tmp_dist_dir = os.path.join(tmp_path, "dist_tmp")
    rsgislib.imagecalc.calc_dist_to_img_vals_tiled(
        input_img,
        output_img,
        2,
        img_band=1,
        max_dist=10,
        no_data_val=0,
        gdalformat="KEA",
        unit_geo=False,
        tmp_dir=tmp_dist_dir,
        tile_size=250,
        n_cores=1,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert prop_match > 0.99


def test_calc_dist_to_img_vals_tiled_multi_val_geo(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_vals23_geo.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals_tiled.kea")
    tmp_dist_dir = os.path.join(tmp_path, "dist_tmp")
    rsgislib.imagecalc.calc_dist_to_img_vals_tiled(
        input_img,
        output_img,
        [2, 3],
        img_band=1,
        max_dist=100,
        no_data_val=0,
        gdalformat="KEA",
        unit_geo=True,
        tmp_dir=tmp_dist_dir,
        tile_size=250,
        n_cores=1,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert prop_match > 0.99


def test_calc_dist_to_img_vals_tiled_multi_val_pxl(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_dist2_vals23_pxl.kea"
    )

    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals_tiled.kea")
    tmp_dist_dir = os.path.join(tmp_path, "dist_tmp")
    rsgislib.imagecalc.calc_dist_to_img_vals_tiled(
        input_img,
        output_img,
        [2, 3],
        img_band=1,
        max_dist=10,
        no_data_val=0,
        gdalformat="KEA",
        unit_geo=False,
        tmp_dir=tmp_dist_dir,
        tile_size=250,
        n_cores=1,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        ref_img, 1, output_img, 1
    )
    assert prop_match > 0.99


def test_calc_prop_true_exp_NoVldMsk():
    import rsgislib.imagecalc

    ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")

    band_defns = [rsgislib.imagecalc.BandDefn("ndvi", ndvi_img, 1)]
    prop = rsgislib.imagecalc.calc_prop_true_exp("ndvi>0.5?1:0", band_defns)

    assert abs(prop - 0.855) < 0.001


def test_calc_prop_true_exp_VldMsk():
    import rsgislib.imagecalc

    ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    vld_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")

    band_defns = [rsgislib.imagecalc.BandDefn("ndvi", ndvi_img, 1)]
    prop = rsgislib.imagecalc.calc_prop_true_exp("ndvi>0.5?1:0", band_defns, vld_img)

    assert abs(prop - 0.655) < 0.001
