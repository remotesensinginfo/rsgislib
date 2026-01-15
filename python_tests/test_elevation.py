import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data", "elevation")


def test_fft_dem_fusion(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    high_res_dem_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    low_res_dem_img = os.path.join(DATA_DIR, "SRTM_aber_low_res_thresh.tif")
    output_img = os.path.join(tmp_path, "out_fused_dem.tif")
    c_size = 20

    rsgislib.elevation.fft_dem_fusion(
        high_res_dem_img, low_res_dem_img, output_img, c_size, gdalformat="GTIFF",
    )

    fused_ref_img = os.path.join(DATA_DIR, "SRTM_aber_fusion.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, fused_ref_img)
    assert img_eq


def test_slope(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    output_img = os.path.join(tmp_path, "out_slope.tif")
    out_angle_unit = "degrees"
    gdalformat = "GTIFF"
    rsgislib.elevation.slope(input_img, output_img, gdalformat, out_angle_unit)

    slope_ref_img = os.path.join(DATA_DIR, "SRTM_aber_slope_deg.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, slope_ref_img)
    assert img_eq


def test_slope_pxl_res_img(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    in_dem_img = os.path.join(DATA_DIR, "AberSRTM_4326.tif")
    in_pxl_res_img = os.path.join(DATA_DIR, "AberSRTM_4326_PxlRes.tif")
    output_img = os.path.join(tmp_path, "out_slope.tif")
    out_angle_unit = "degrees"
    gdalformat = "GTIFF"
    rsgislib.elevation.slope_pxl_res_img(
        in_dem_img, in_pxl_res_img, output_img, gdalformat, out_angle_unit
    )

    slope_ref_img = os.path.join(DATA_DIR, "AberSRTM_4326_slope.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, slope_ref_img)
    assert img_eq


def test_aspect(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    output_img = os.path.join(tmp_path, "out_aspect.tif")
    gdalformat = "GTIFF"
    rsgislib.elevation.aspect(input_img, output_img, gdalformat)

    aspect_ref_img = os.path.join(DATA_DIR, "SRTM_aber_aspect.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, aspect_ref_img)
    print(prop_match)
    assert img_eq


def test_aspect_pxl_res_img(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    in_dem_img = os.path.join(DATA_DIR, "AberSRTM_4326.tif")
    in_pxl_res_img = os.path.join(DATA_DIR, "AberSRTM_4326_PxlRes.tif")
    output_img = os.path.join(tmp_path, "out_aspect.tif")
    gdalformat = "GTIFF"
    rsgislib.elevation.aspect_pxl_res_img(
        in_dem_img, in_pxl_res_img, output_img, gdalformat
    )

    aspect_ref_img = os.path.join(DATA_DIR, "AberSRTM_4326_aspect.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, aspect_ref_img)
    assert img_eq


def test_dtm_aspect_median_filter(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    input_aspect_img = os.path.join(DATA_DIR, "SRTM_aber_aspect.tif")
    output_img = os.path.join(tmp_path, "out_aspect_med_filter.tif")
    aspect_range = 0.5
    win_half_size = 10
    gdalformat = "GTIFF"
    rsgislib.elevation.dtm_aspect_median_filter(
        input_img, input_aspect_img, output_img, aspect_range, win_half_size, gdalformat
    )

    aspect_med_filter_ref_img = os.path.join(
        DATA_DIR, "SRTM_aber_aspect_med_filter.tif"
    )
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, aspect_med_filter_ref_img
    )
    print(prop_match)
    assert img_eq


def test_fill_dem_soille_gratin_1994(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    valid_mask_img = os.path.join(DATA_DIR, "SRTM_aber_valid_mask.tif")
    output_img = os.path.join(tmp_path, "out_fill_soille_gratin_1994_tmpout.tif")
    rsgislib.elevation.fill_dem_soille_gratin_1994(
        input_img, valid_mask_img, output_img, "GTIFF"
    )

    fill_ref_img = os.path.join(DATA_DIR, "SRTM_aber_fill_soille_gratin_1994.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, fill_ref_img)
    assert img_eq


def test_plane_fit_detreat_dem(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    output_img = os.path.join(tmp_path, "out_SRTM_planefitdetreat.tif")
    gdalformat = "GTIFF"
    window_size = 5
    rsgislib.elevation.plane_fit_detreat_dem(
        input_img, output_img, gdalformat, window_size
    )

    plane_fit_ref_img = os.path.join(DATA_DIR, "SRTM_aber_plane_fit_detreat_dem.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, plane_fit_ref_img
    )
    print(prop_match)
    assert prop_match > 0.99


def test_shadow_mask(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber_subset.tif")
    output_img = os.path.join(tmp_path, "out_SRTM_shadow_mask.tif")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    max_height = 5000
    gdalformat = "GTIFF"
    rsgislib.elevation.shadow_mask(
        input_img, output_img, solar_azimuth, solar_zenith, max_height, gdalformat
    )

    shadow_mask_ref_img = os.path.join(DATA_DIR, "SRTM_aber_shadow_mask.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, shadow_mask_ref_img
    )
    assert img_eq


def test_local_incidence_angle(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    output_img = os.path.join(tmp_path, "out_SRTM_localincangle.tif")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "GTIFF"
    rsgislib.elevation.local_incidence_angle(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    inc_angle_ref_img = os.path.join(DATA_DIR, "SRTM_aber_localincangle.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, inc_angle_ref_img
    )
    assert img_eq


def test_local_existance_angle(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    output_img = os.path.join(tmp_path, "out_SRTM_localexangle.tif")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "GTIFF"
    rsgislib.elevation.local_existance_angle(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    ex_angle_ref_img = os.path.join(DATA_DIR, "SRTM_aber_localexangle.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(output_img, ex_angle_ref_img)
    assert img_eq


def test_hillshade(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.tif")
    output_img = os.path.join(tmp_path, "out_SRTM_hillshade.tif")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "GTIFF"
    rsgislib.elevation.hillshade(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    hillshade_ref_img = os.path.join(DATA_DIR, "SRTM_aber_hillshade.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, hillshade_ref_img
    )
    assert img_eq


def test_hillshade_pxl_res_img(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    in_dem_img = os.path.join(DATA_DIR, "AberSRTM_4326.tif")
    in_pxl_res_img = os.path.join(DATA_DIR, "AberSRTM_4326_PxlRes.tif")
    output_img = os.path.join(tmp_path, "out_hillshade.tif")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "GTIFF"
    rsgislib.elevation.hillshade_pxl_res_img(
        in_dem_img, in_pxl_res_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    hillshade_ref_img = os.path.join(DATA_DIR, "AberSRTM_4326_hillshade.tif")
    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(
        output_img, hillshade_ref_img
    )
    assert img_eq
