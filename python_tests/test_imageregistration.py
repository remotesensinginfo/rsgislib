import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGREG_DATA_DIR = os.path.join(DATA_DIR, "imageregistration")


def test_find_image_offset():
    import rsgislib.imageregistration

    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    in_float_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset.kea"
    )
    x_off, y_off = rsgislib.imageregistration.find_image_offset(
        in_ref_img,
        in_float_img,
        [1, 2, 3],
        [1, 2, 3],
        rsgislib.imageregistration.METRIC_CORELATION,
        4,
        4,
        4,
    )
    print("x_off: {}".format(x_off))
    print("y_off: {}".format(y_off))
    assert abs((x_off - 3) < 0.5) and abs((y_off - 3) < 0.5)


def test_apply_offset_to_image(tmp_path):
    import rsgislib.imageregistration

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageregistration.apply_offset_to_image(
        input_img, output_img, "KEA", rsgislib.TYPE_16UINT, 50, 30
    )
    assert os.path.exists(output_img)


def test_basic_registration(tmp_path):
    import rsgislib.imageregistration

    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    in_float_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset.kea"
    )
    out_gcp_file = os.path.join(tmp_path, "out_gcps.txt")
    rsgislib.imageregistration.basic_registration(
        in_ref_img,
        in_float_img,
        out_gcp_file,
        50,
        0.8,
        50,
        4,
        2,
        2,
        4,
        rsgislib.imageregistration.METRIC_CORELATION,
        rsgislib.imageregistration.TYPE_RSGIS_IMG2MAP,
    )

    assert os.path.exists(out_gcp_file)


def test_single_layer_registration(tmp_path):
    import rsgislib.imageregistration

    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    in_float_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset.kea"
    )
    out_gcp_file = os.path.join(tmp_path, "out_gcps.txt")
    rsgislib.imageregistration.single_layer_registration(
        in_ref_img,
        in_float_img,
        out_gcp_file,
        50,
        0.8,
        50,
        4,
        2,
        2,
        5,
        6,
        4,
        0.5,
        2,
        rsgislib.imageregistration.METRIC_CORELATION,
        rsgislib.imageregistration.TYPE_RSGIS_IMG2MAP,
    )
    assert os.path.exists(out_gcp_file)


def test_gcp_to_gdal(tmp_path):
    import rsgislib.imageregistration
    import rsgislib.imageutils

    input_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset.kea"
    )
    in_gcp_file = os.path.join(IMGREG_DATA_DIR, "reg_gcps.txt")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageregistration.gcp_to_gdal(
        input_img, in_gcp_file, output_img, "KEA", rsgislib.TYPE_16UINT
    )
    assert os.path.exists(output_img) and rsgislib.imageutils.has_gcps(output_img)


def test_warp_with_gcps_with_gdal(tmp_path):
    import rsgislib.imageregistration

    in_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    in_process_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset_gcps.kea"
    )
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageregistration.warp_with_gcps_with_gdal(
        in_ref_img,
        in_process_img,
        output_img,
        "KEA",
        rsgislib.INTERP_NEAREST_NEIGHBOUR,
        use_tps=False,
        use_poly=True,
        poly_order=2,
        use_multi_thread=False,
    )


def test_add_vec_pts_as_gcps_to_img(tmp_path):
    import rsgislib.imageregistration
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    vec_gcp_pts_file = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_gcp_pts.geojson"
    )
    vec_gcp_pts_lyr = "sen2_20210527_aber_subset_b123_gcp_pts"
    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.imageregistration.add_vec_pts_as_gcps_to_img(
        input_img,
        output_img,
        vec_gcp_pts_file,
        vec_gcp_pts_lyr,
        gcp_x_col="x_match",
        gcp_y_col="y_match",
        gcp_z_col=None,
        gcp_epsg=32630,
    )

    assert os.path.exists(output_img) and rsgislib.imageutils.has_gcps(output_img)

