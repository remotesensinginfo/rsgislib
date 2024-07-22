import os
import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imagecalc")

RTREE_NOT_AVAIL = False
try:
    import rtree
except ImportError:
    RTREE_NOT_AVAIL = True

SKLEARN_NOT_AVAIL = False
try:
    import sklearn
except ImportError:
    SKLEARN_NOT_AVAIL = True


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


def test_count_pxls_of_val_band1_sel_vals():
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


def test_are_img_bands_equal_dif_bands_false():
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


def test_buffer_img_pxl_vals(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    output_img = os.path.join(tmp_path, "test_buf_vals.kea")

    rsgislib.imagecalc.buffer_img_pxl_vals(
        input_img,
        output_img,
        pxl_vals=[2, 3],
        buf_thres=100,
        tmp_dir=tmp_path,
        gdalformat="KEA",
        img_band=1,
        unit_geo=True,
    )

    assert os.path.exists(output_img)


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


def test_calc_dist_to_img_vals_sgl_val_pxl_out_no_data(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    output_img = os.path.join(tmp_path, "test_calc_dist_to_img_vals.kea")
    rsgislib.imagecalc.calc_dist_to_img_vals(
        input_img,
        output_img,
        2,
        img_band=1,
        gdalformat="KEA",
        max_dist=10,
        no_data_val=0,
        out_no_data_val=11,
        unit_geo=False,
    )


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


def test_calc_prop_true_exp_no_vld_msk():
    import rsgislib.imagecalc

    ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")

    band_defns = [rsgislib.imagecalc.BandDefn("ndvi", ndvi_img, 1)]
    prop = rsgislib.imagecalc.calc_prop_true_exp("ndvi>0.5?1:0", band_defns)

    assert abs(prop - 0.855) < 0.001


def test_calc_prop_true_exp_vld_msk():
    import rsgislib.imagecalc

    ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    vld_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")

    band_defns = [rsgislib.imagecalc.BandDefn("ndvi", ndvi_img, 1)]
    prop = rsgislib.imagecalc.calc_prop_true_exp("ndvi>0.5?1:0", band_defns, vld_img)

    assert abs(prop - 0.655) < 0.001


def test_calc_band_percentile():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    percent_val = rsgislib.imagecalc.calc_band_percentile(input_img, 0.5, 0)

    assert ((percent_val[0] - 43) < 1) and ((percent_val[5] - 458) < 1)


def test_calc_img_rescale_sgl_img(tmp_path):
    import rsgislib.imagecalc

    rescale_ndvi_ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_g100_o100_int.kea"
    )
    ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    print(ndvi_img)

    output_img = os.path.join(tmp_path, "rescaled_ndvi_img.kea")

    rsgislib.imagecalc.calc_img_rescale(
        ndvi_img, output_img, "KEA", rsgislib.TYPE_16INT, -999, 0, 1, 999, 100, 100
    )

    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, rescale_ndvi_ref_img
    )
    assert img_eq


def test_calc_img_rescale_multi_imgs(tmp_path):
    import rsgislib.imagecalc

    rescale_ndvi_ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_bs2_g100_o100_int.kea"
    )
    ndvi_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")
    print(ndvi_img)

    output_img = os.path.join(tmp_path, "rescaled_ndvi_imgs.kea")

    rsgislib.imagecalc.calc_img_rescale(
        [ndvi_img, ndvi_img],
        output_img,
        "KEA",
        rsgislib.TYPE_16INT,
        -999,
        0,
        1,
        999,
        100,
        100,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, rescale_ndvi_ref_img
    )
    assert img_eq


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_perform_image_pca(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    output_img = os.path.join(tmp_path, "pca_result_img.kea")
    out_eigen_vec_file = os.path.join(tmp_path, "pca_eign_vec.mtxt")

    rsgislib.imagecalc.perform_image_pca(
        input_img,
        output_img,
        out_eigen_vec_file,
        n_comps=None,
        pxl_n_sample=100,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_32FLOAT,
        no_data_val=None,
        calc_stats=True,
    )

    assert os.path.exists(output_img) and os.path.exists(out_eigen_vec_file)


@pytest.mark.skipif(SKLEARN_NOT_AVAIL, reason="scikit-learn dependency not available")
def test_perform_image_mnf(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    output_img = os.path.join(tmp_path, "pca_result_img.kea")
    tmp_dir = os.path.join(tmp_path, "tmp")

    rsgislib.imagecalc.perform_image_mnf(
        input_img,
        output_img,
        n_comps=None,
        pxl_n_sample=100,
        in_img_no_data=0,
        tmp_dir=tmp_dir,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_32FLOAT,
        calc_stats=True,
    )

    assert os.path.exists(output_img)


def test_recode_int_raster(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")

    output_img = os.path.join(tmp_path, "recoded_ndvi_cats.kea")

    recode_dict = dict()
    recode_dict[1] = 100
    recode_dict[2] = 200
    recode_dict[3] = 300

    rsgislib.imagecalc.recode_int_raster(
        input_img,
        output_img,
        recode_dict,
        keep_vals_not_in_dict=True,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_16UINT,
    )
    assert os.path.exists(output_img)


def test_all_bands_equal_to(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.all_bands_equal_to(
        input_img, output_img, 0, 1, 2, "KEA", rsgislib.TYPE_8UINT
    )
    assert os.path.exists(output_img)


def test_kmeans_clustering(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_file = os.path.join(tmp_path, "out_file")
    rsgislib.imagecalc.kmeans_clustering(
        input_img,
        out_file,
        10,
        20,
        100,
        True,
        0.0025,
        rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH,
    )

    out_ext_file = os.path.join(tmp_path, "out_file.gmtxt")
    assert os.path.exists(out_ext_file)


def test_isodata_clustering(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_file = os.path.join(tmp_path, "out_file")
    rsgislib.imagecalc.isodata_clustering(
        input_img,
        out_file,
        10,
        20,
        100,
        True,
        0.0025,
        rsgislib.INITCLUSTER_DIAGONAL_FULL_ATTACH,
        2,
        5,
        5,
        5,
        8,
        18,
    )
    out_ext_file = os.path.join(tmp_path, "out_file.gmtxt")
    assert os.path.exists(out_ext_file)


def test_image_pixel_column_summary(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    stats_to_calc = rsgislib.imagecalc.StatsSummary()
    stats_to_calc.calc_min = True
    stats_to_calc.calc_max = True
    stats_to_calc.calc_sum = True
    stats_to_calc.calc_mean = True
    stats_to_calc.calc_stdev = True
    stats_to_calc.calc_median = True
    stats_to_calc.calc_mode = True
    rsgislib.imagecalc.image_pixel_column_summary(
        input_img, output_img, stats_to_calc, "KEA", rsgislib.TYPE_32FLOAT, 0.0, True
    )
    assert os.path.exists(output_img)


def test_get_img_band_stats_in_env():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_wgs84.kea")
    stats = rsgislib.imagecalc.get_img_band_stats_in_env(
        input_img, 1, 0, -4.0932, -4.0774, 52.4033, 52.4229
    )

    print(stats)
    assert (
        ((stats[0] - 30) < 1)
        and ((stats[1] - 1066) < 1)
        and ((stats[2] - 74.6039) < 1)
        and ((stats[3] - 34.9773) < 1)
        and ((stats[4] - 1904637.0) < 2)
    )


def test_get_img_band_mode_in_env_0():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_wgs84.kea")
    mode = rsgislib.imagecalc.get_img_band_mode_in_env(
        input_img, 1, 1, -999, -4.0932, -4.0774, 52.4033, 52.4229
    )

    assert mode[0] == 0


def test_get_img_band_mode_in_env_2():
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_wgs84.kea")
    mode = rsgislib.imagecalc.get_img_band_mode_in_env(
        input_img, 1, 1, -999, -4.0154, -3.9880, 52.4004, 52.4257
    )

    print(mode)
    assert mode[0] == 2


def test_calc_multi_img_band_stats(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calc_multi_img_band_stats(
        [input_img, input_img],
        output_img,
        rsgislib.SUMTYPE_MEAN,
        "KEA",
        rsgislib.TYPE_32FLOAT,
        0,
        True,
    )

    assert os.path.exists(output_img)


def test_get_img_band_min_max():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    min, max = rsgislib.imagecalc.get_img_band_min_max(input_img, 1, True, 0)
    assert min == 1 and max == 1066


# TODO rsgislib.imagecalc.get_img_sum_stats_in_pxl
# TODO rsgislib.imagecalc.get_img_idx_for_stat
# TODO rsgislib.imagecalc.identify_min_pxl_value_in_win


def test_calc_img_mean_in_mask():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    val = rsgislib.imagecalc.calc_img_mean_in_mask(
        input_img,
        in_msk_img,
        msk_val=1,
        img_bands=[1, 2, 3],
        no_data_val=0,
        use_no_data=True,
    )
    assert (val - 71.5) < 0.1


def test_calc_img_basic_stats_for_ref_region(tmp_path):
    import rsgislib.imagecalc

    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    in_stats_imgs = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calc_img_basic_stats_for_ref_region(
        in_ref_img, [in_stats_imgs], output_img, gdalformat="KEA"
    )

    assert os.path.exists(output_img)


@pytest.mark.skipif(RTREE_NOT_AVAIL, reason="rtree dependency not available")
def test_calc_fill_regions_knn(tmp_path):
    import rsgislib.imagecalc

    in_ref_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_subset.kea"
    )
    in_fill_regions_img = os.path.join(
        IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats_subset.kea"
    )
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calc_fill_regions_knn(
        in_ref_img,
        0,
        in_fill_regions_img,
        0,
        output_img,
        k=5,
        summary=rsgislib.SUMTYPE_MODE,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_16INT,
    )
    assert os.path.exists(output_img)


def test_image_pixel_linear_fit(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    band_values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.image_pixel_linear_fit(
        input_img, output_img, "KEA", band_values, 0, True
    )
    assert os.path.exists(output_img)


def test_calculate_img_band_rmse():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imagecalc.calculate_img_band_rmse(input_img, 1, input_img, 2)


def test_correlation_window(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.correlation_window(
        input_img, output_img, 5, 1, 2, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_calc_mask_img_pxl_val_prob(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    in_msk_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi_cats.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calc_mask_img_pxl_val_prob(
        input_img, [1, 2, 3], in_msk_img, 2, output_img, "KEA", [5, 5, 5], False, False
    )
    assert os.path.exists(output_img)


def test_calc_img_difference(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calc_img_difference(
        input_img, input_img, output_img, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_histogram(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    output_file = os.path.join(tmp_path, "out_img.txt")
    rsgislib.imagecalc.histogram(
        input_img, in_msk_img, output_file, 1, 1, 10, True, 0, 0
    )

    assert os.path.exists(output_file)


def test_get_histogram():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imagecalc.get_histogram(input_img, 1, 1, True, 0, 0)


def test_get_2d_img_histogram(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.get_2d_img_histogram(
        input_img,
        input_img,
        output_img,
        "KEA",
        1,
        2,
        100,
        1,
        1,
        1000,
        1000,
        1,
        1,
        0,
        0,
        False,
    )
    assert os.path.exists(output_img)


def test_calc_histograms_for_msk_vals():
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")

    rsgislib.imagecalc.calc_histograms_for_msk_vals(
        input_img, 1, in_msk_img, 1, 1, 1000, 1, msk_vals=None
    )


def test_normalise_image_band(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.normalise_image_band(input_img, 1, output_img, gdalformat="KEA")
    assert os.path.exists(output_img)


def test_rescale_img_pxl_vals(tmp_path):
    import rsgislib.imagecalc

    band_rescale_objs = list()
    band_rescale_objs.append(
        rsgislib.imagecalc.ImageBandRescale(
            band=1,
            in_min=1.0,
            in_max=1000.0,
            no_data_val=0,
            out_min=200.0,
            out_max=500.0,
            out_no_data=-999.0,
        )
    )
    band_rescale_objs.append(
        rsgislib.imagecalc.ImageBandRescale(
            band=2,
            in_min=1.0,
            in_max=1000.0,
            no_data_val=0,
            out_min=200.0,
            out_max=500.0,
            out_no_data=-999.0,
        )
    )
    band_rescale_objs.append(
        rsgislib.imagecalc.ImageBandRescale(
            band=3,
            in_min=1.0,
            in_max=1000.0,
            no_data_val=0,
            out_min=200.0,
            out_max=500.0,
            out_no_data=-999.0,
        )
    )

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.rescale_img_pxl_vals(
        input_img,
        output_img,
        "KEA",
        rsgislib.TYPE_32FLOAT,
        band_rescale_objs,
        trim_to_limits=True,
    )
    assert os.path.exists(output_img)


def test_mahalanobis_dist_filter(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.mahalanobis_dist_filter(
        input_img, output_img, 5, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_mahalanobis_dist_to_img_filter(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.mahalanobis_dist_to_img_filter(
        input_img, output_img, 5, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_calc_split_win_thresholds_otsu(tmp_path):
    import rsgislib.imagecalc

    input_img = os.path.join(IMGCALC_DATA_DIR, "sen2_20210527_aber_ndvi.kea")

    thres_vals = rsgislib.imagecalc.calc_split_win_thresholds(
        input_img,
        win_size=500,
        thres_meth=rsgislib.THRES_METH_OTSU,
        output_file=None,
        no_data_val=0.0,
        lower_valid=None,
        upper_valid=None,
        min_n_vals=100,
        # thres_kwrds = None,
    )

    n_thres = len(thres_vals[1])
    assert (
        (n_thres == 4)
        and ((thres_vals[1][0] - 0.335) < 0.01)
        and ((thres_vals[1][1] - 0.692) < 0.01)
        and ((thres_vals[1][2] - 0.461) < 0.01)
        and ((thres_vals[1][3] - 0.708) < 0.01)
    )
