import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_INDICES_DATA_DIR = os.path.join(DATA_DIR, "imagecalc", "calcindices")


def test_calc_ndvi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_ndvi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_ndvi(
        input_img, 3, 8, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_wbi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_wbi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_wbi(
        input_img, 1, 8, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_ndwi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_ndwi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_ndwi(
        input_img, 8, 9, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_gndwi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_gndwi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_gndwi(
        input_img, 2, 8, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_gmndwi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_gmndwi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_gmndwi(
        input_img, 2, 9, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_whiteness(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_whiteness_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_whiteness(
        input_img, 1, 2, 3, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_brightness(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_brightness_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_brightness(
        input_img,
        1,
        2,
        3,
        output_img,
        calc_stats=False,
        gdalformat="KEA",
        scale_factor=1000,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_brightness_scaled(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_brightness_scaled_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_brightness_scaled(
        input_img,
        1,
        2,
        3,
        output_img,
        calc_stats=False,
        gdalformat="KEA",
        scale_factor=1000,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_ctvi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_ctvi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_ctvi(
        input_img, 1, 8, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_ndsi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_ndsi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_ndsi(
        input_img, 2, 9, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_nbr(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_nbr_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_nbr(
        input_img, 8, 10, output_img, calc_stats=False, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_bai(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_bai_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_bai(
        input_img,
        8,
        3,
        output_img,
        calc_stats=False,
        gdalformat="KEA",
        scale_factor=1000,
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95


def test_calc_mvi(tmp_path):
    import rsgislib.imagecalc
    import rsgislib.imagecalc.calcindices

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    ref_img = os.path.join(
        IMGCALC_INDICES_DATA_DIR, "sen2_20210527_aber_subset_mvi_ref.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imagecalc.calcindices.calc_mvi(
        input_img, 2, 8, 9, output_img, calc_stats=True, gdalformat="KEA"
    )

    img_eq, prop_match = rsgislib.imagecalc.are_img_bands_equal(
        output_img, 1, ref_img, 1
    )
    assert prop_match > 0.95
