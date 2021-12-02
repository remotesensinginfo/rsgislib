import os
import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_polygonise_raster_to_vec_lyr_4_conn(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.polygonise_raster_to_vec_lyr(
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        input_img=input_img,
        img_band=1,
        mask_img=input_img,
        mask_band=1,
        use_8_conn=False,
    )

    assert os.path.exists(out_vec_file)


def test_polygonise_raster_to_vec_lyr_8_conn(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.polygonise_raster_to_vec_lyr(
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        input_img=input_img,
        img_band=1,
        mask_img=input_img,
        mask_band=1,
        use_8_conn=True,
    )

    assert os.path.exists(out_vec_file)


def test_vectorise_pxls_to_pts(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")
    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.vectorise_pxls_to_pts(
        input_img,
        1,
        3,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        out_epsg_code=None,
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_extract_image_footprint(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.extract_image_footprint(
        input_img, out_vec_file, out_vec_lyr, "GPKG", tmp_dir=tmp_path, reproj_to=None
    )

    assert os.path.exists(out_vec_file)


def test_extract_image_footprint_exception(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    with pytest.raises(rsgislib.RSGISPyException):
        rsgislib.vectorutils.createvectors.extract_image_footprint(
            input_img,
            out_vec_file,
            out_vec_lyr,
            "GPKG",
            tmp_dir=tmp_path,
            reproj_to=None,
        )


def test_extract_image_footprint(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.extract_image_footprint(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        tmp_dir=tmp_path,
        reproj_to=None,
        no_data_val=0,
    )

    assert os.path.exists(out_vec_file)


def test_extract_image_footprint_reproj(tmp_path):
    import rsgislib.vectorutils.createvectors

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.extract_image_footprint(
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        tmp_dir=tmp_path,
        reproj_to="epsg:4326",
        no_data_val=0,
    )

    assert os.path.exists(out_vec_file)
