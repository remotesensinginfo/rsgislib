import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data", "elevation")


def test_fft_dem_fusion(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    high_res_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    low_res_dem_img = os.path.join(DATA_DIR, "SRTM_aber_low_res_thresh.kea")
    output_img = os.path.join(tmp_path, "out_fused_dem.kea")
    c_size = 20

    rsgislib.elevation.fft_dem_fusion(
        high_res_dem_img, low_res_dem_img, output_img, c_size
    )

    fused_ref_img = os.path.join(DATA_DIR, "SRTM_aber_fusion.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, fused_ref_img)
    assert img_eq


def test_slope(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_slope.kea")
    out_angle_unit = "degrees"
    gdalformat = "KEA"
    rsgislib.elevation.slope(input_img, output_img, out_angle_unit, gdalformat)

    slope_ref_img = os.path.join(DATA_DIR, "SRTM_aber_slope_deg.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, slope_ref_img)
    assert img_eq


def test_aspect(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_aspect.kea")
    gdalformat = "KEA"
    rsgislib.elevation.aspect(input_img, output_img, gdalformat)

    aspect_ref_img = os.path.join(DATA_DIR, "SRTM_aber_aspect.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, aspect_ref_img)
    print(prop_match)
    assert img_eq


def test_dtm_aspect_median_filter(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    input_aspect_img = os.path.join(DATA_DIR, "SRTM_aber_aspect.kea")
    output_img = os.path.join(tmp_path, "out_aspect_med_filter.kea")
    aspect_range = 0.5
    win_half_size = 10
    gdalformat = "KEA"
    rsgislib.elevation.dtm_aspect_median_filter(
        input_img, input_aspect_img, output_img, aspect_range, win_half_size, gdalformat
    )

    aspect_med_filter_ref_img = os.path.join(
        DATA_DIR, "SRTM_aber_aspect_med_filter.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, aspect_med_filter_ref_img
    )
    print(prop_match)
    assert img_eq


def test_fill_dem_soille_gratin_1994(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    valid_mask_img = os.path.join(DATA_DIR, "SRTM_aber_valid_mask.kea")
    output_img = os.path.join(tmp_path, "out_fill_soille_gratin_1994_tmpout.kea")
    rsgislib.elevation.fill_dem_soille_gratin_1994(
        input_img, valid_mask_img, output_img, "KEA"
    )

    fill_ref_img = os.path.join(DATA_DIR, "SRTM_aber_fill_soille_gratin_1994.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, fill_ref_img)
    assert img_eq


def test_plane_fit_detreat_dem(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_planefitdetreat.kea")
    gdalformat = "KEA"
    window_size = 5
    rsgislib.elevation.plane_fit_detreat_dem(
        input_img, output_img, gdalformat, window_size
    )

    plane_fit_ref_img = os.path.join(DATA_DIR, "SRTM_aber_plane_fit_detreat_dem.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, plane_fit_ref_img
    )
    print(prop_match)
    assert img_eq


def test_shadow_mask(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_shadow_mask.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    max_height = 5000
    gdalformat = "KEA"
    rsgislib.elevation.shadow_mask(
        input_img, output_img, solar_azimuth, solar_zenith, max_height, gdalformat
    )

    shadow_mask_ref_img = os.path.join(DATA_DIR, "SRTM_aber_shadow_mask.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, shadow_mask_ref_img
    )
    assert img_eq


def test_local_incidence_angle(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_localincangle.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "KEA"
    rsgislib.elevation.local_incidence_angle(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    inc_angle_ref_img = os.path.join(DATA_DIR, "SRTM_aber_localincangle.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, inc_angle_ref_img
    )
    assert img_eq


def test_local_existance_angle(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_localexangle.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "KEA"
    rsgislib.elevation.local_existance_angle(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    ex_angle_ref_img = os.path.join(DATA_DIR, "SRTM_aber_localexangle.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, ex_angle_ref_img)
    assert img_eq


def test_hillshade(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_hillshade.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "KEA"
    rsgislib.elevation.hillshade(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    hillshade_ref_img = os.path.join(DATA_DIR, "SRTM_aber_hillshade.kea")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, hillshade_ref_img
    )
    assert img_eq
