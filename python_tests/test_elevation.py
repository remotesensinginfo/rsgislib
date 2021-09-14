import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data", "elevation")


def test_fftDEMFusion(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    high_res_dem_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    low_res_dem_img = os.path.join(DATA_DIR, "SRTM_aber_low_res_thresh.kea")
    output_img = os.path.join(tmp_path, "out_fused_dem.kea")
    c_size = 20

    rsgislib.elevation.fftDEMFusion(
        high_res_dem_img, low_res_dem_img, output_img, c_size
    )

    fused_ref_img = os.path.join(DATA_DIR, "SRTM_aber_fusion.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, fused_ref_img)
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
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, slope_ref_img)
    assert img_eq


def test_aspect(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_aspect.kea")
    gdalformat = "KEA"
    rsgislib.elevation.aspect(input_img, output_img, gdalformat)

    aspect_ref_img = os.path.join(DATA_DIR, "SRTM_aber_aspect.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, aspect_ref_img)
    print(prop_match)
    assert img_eq


def test_dtmAspectMedianFilter(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    input_aspect_img = os.path.join(DATA_DIR, "SRTM_aber_aspect.kea")
    output_img = os.path.join(tmp_path, "out_aspect_med_filter.kea")
    aspect_range = 0.5
    win_half_size = 10
    gdalformat = "KEA"
    rsgislib.elevation.dtmAspectMedianFilter(
        input_img, input_aspect_img, output_img, aspect_range, win_half_size, gdalformat
    )

    aspect_med_filter_ref_img = os.path.join(
        DATA_DIR, "SRTM_aber_aspect_med_filter.kea"
    )
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(
        output_img, aspect_med_filter_ref_img
    )
    print(prop_match)
    assert img_eq


def test_fillDEMSoilleGratin1994(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    valid_mask_img = os.path.join(DATA_DIR, "SRTM_aber_valid_mask.kea")
    output_img = os.path.join(tmp_path, "out_fill_Gratin94.kea")
    rsgislib.elevation.fillDEMSoilleGratin1994(
        input_img, valid_mask_img, output_img, "KEA"
    )

    fill_ref_img = os.path.join(DATA_DIR, "SRTM_aber_fill_Gratin94.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, fill_ref_img)
    assert img_eq


def test_planeFitDetreatDEM(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_planefitdetreat.kea")
    gdalformat = "KEA"
    window_size = 5
    rsgislib.elevation.planeFitDetreatDEM(
        input_img, output_img, gdalformat, window_size
    )

    plane_fit_ref_img = os.path.join(DATA_DIR, "SRTM_aber_planeFitDetreatDEM.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, plane_fit_ref_img)
    print(prop_match)
    assert img_eq


def test_shadowMask(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_shadowmask.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    max_height = 5000
    gdalformat = "KEA"
    rsgislib.elevation.shadowMask(
        input_img, output_img, solar_azimuth, solar_zenith, max_height, gdalformat
    )

    shadowmask_ref_img = os.path.join(DATA_DIR, "SRTM_aber_shadowmask.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, shadowmask_ref_img)
    assert img_eq


def test_localIncidenceAngle(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_localincangle.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "KEA"
    rsgislib.elevation.localIncidenceAngle(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    inc_angle_ref_img = os.path.join(DATA_DIR, "SRTM_aber_localincangle.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, inc_angle_ref_img)
    assert img_eq


def test_localExistanceAngle(tmp_path):
    import rsgislib.elevation
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "SRTM_aber.kea")
    output_img = os.path.join(tmp_path, "out_SRTM_localexangle.kea")
    solar_azimuth = 126.45
    solar_zenith = 35.67
    gdalformat = "KEA"
    rsgislib.elevation.localExistanceAngle(
        input_img, output_img, solar_azimuth, solar_zenith, gdalformat
    )

    ex_angle_ref_img = os.path.join(DATA_DIR, "SRTM_aber_localexangle.kea")
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, ex_angle_ref_img)
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
    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(output_img, hillshade_ref_img)
    assert img_eq
