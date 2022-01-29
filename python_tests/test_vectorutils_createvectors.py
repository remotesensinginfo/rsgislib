import os
import pytest

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

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


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
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
        input_img,
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        tmp_dir=tmp_path,
        reproj_to=None,
        no_data_val=0,
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


def test_define_grid(tmp_path):
    import rsgislib.vectorutils.createvectors

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.define_grid(
        [0, 5, 10, 15],
        1,
        1,
        4326,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        out_epsg_code=None,
        utm_grid=False,
        utm_hemi=False,
    )

    assert os.path.exists(out_vec_file)


def test_define_grid_utm(tmp_path):
    import rsgislib.vectorutils.createvectors

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.define_grid(
        [0, 5, 10, 15],
        5000,
        5000,
        4326,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        out_epsg_code=None,
        utm_grid=True,
        utm_hemi=False,
    )

    assert os.path.exists(out_vec_file)


def test_create_poly_vec_bboxs(tmp_path):
    import rsgislib.vectorutils.createvectors
    from osgeo import ogr

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    bboxs = [[1, 2, 1, 2], [2, 3, 2, 3], [3, 4, 3, 4], [4, 5, 4, 5]]

    atts = dict()
    atts["name"] = ["hello", "to", "the", "world"]
    atts["id"] = [1, 2, 3, 4]

    att_types = dict()
    att_types["names"] = ["name", "id"]
    att_types["types"] = [ogr.OFTString, ogr.OFTInteger]

    rsgislib.vectorutils.createvectors.create_poly_vec_bboxs(
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        4326,
        bboxs,
        atts=atts,
        att_types=att_types,
        overwrite=True,
    )

    assert os.path.exists(out_vec_file)


def test_write_pts_to_vec(tmp_path):
    import rsgislib.vectorutils.createvectors
    from osgeo import ogr

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    pts_x = [1, 2, 3, 4]
    pts_y = [1, 2, 3, 4]

    atts = dict()
    atts["name"] = ["hello", "to", "the", "world"]
    atts["id"] = [1, 2, 3, 4]

    att_types = dict()
    att_types["names"] = ["name", "id"]
    att_types["types"] = [ogr.OFTString, ogr.OFTInteger]

    rsgislib.vectorutils.createvectors.write_pts_to_vec(
        out_vec_file,
        out_vec_lyr,
        "GPKG",
        4326,
        pts_x,
        pts_y,
        atts=atts,
        att_types=att_types,
        replace=True,
        file_opts=[],
        lyr_opts=[],
    )

    assert os.path.exists(out_vec_file)


def test_create_bboxs_for_pts(tmp_path):
    import rsgislib.vectorutils.createvectors

    pts_vec_file = os.path.join(tmp_path, "pts_vec.gpkg")
    pts_vec_lyr = "pts_vec"

    pts_x = [1, 2, 3, 4]
    pts_y = [1, 2, 3, 4]

    rsgislib.vectorutils.createvectors.write_pts_to_vec(
        pts_vec_file,
        pts_vec_lyr,
        "GPKG",
        4326,
        pts_x,
        pts_y,
        atts=None,
        att_types=None,
        replace=True,
        file_opts=[],
        lyr_opts=[],
    )

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.createvectors.create_bboxs_for_pts(
        pts_vec_file,
        pts_vec_lyr,
        0.25,
        0.35,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )
    assert os.path.exists(out_vec_file)
