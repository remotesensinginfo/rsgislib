import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGUTILS_DATA_DIR = os.path.join(DATA_DIR, "imageutils")


def test_create_img_extent_lut(tmp_path):
    import rsgislib.imageutils.imagelut
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    assert os.path.exists(vec_file)


def test_create_img_extent_lut_wgs84(tmp_path):
    import rsgislib.imageutils.imagelut
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=True,
        overwrite_lut_file=False,
    )

    assert os.path.exists(vec_file)


def test_get_all_lut_imgs(tmp_path):
    import rsgislib.imageutils.imagelut
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    rsgislib.imageutils.imagelut.get_all_lut_imgs(vec_file, vec_lyr)


def test_query_img_lut(tmp_path):
    import rsgislib.imageutils.imagelut
    import rsgislib.imageutils
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    scn_bbox = rsgislib.imageutils.get_img_bbox(
        os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    )

    rsgislib.imageutils.imagelut.query_img_lut(scn_bbox, vec_file, vec_lyr)


def test_get_raster_lyr(tmp_path):
    import rsgislib.imageutils.imagelut
    import rsgislib.imageutils
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    scn_bbox = rsgislib.imageutils.get_img_bbox(
        os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    )

    out_img = rsgislib.imageutils.imagelut.get_raster_lyr(
        scn_bbox, vec_file, vec_lyr, tmp_path
    )
    assert os.path.exists(out_img)


def test_query_file_lut(tmp_path):
    import rsgislib.imageutils.imagelut
    import rsgislib.imageutils
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    roi_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_lyr = "aber_osgb_single_poly"

    rsgislib.imageutils.imagelut.query_file_lut(
        vec_file, vec_lyr, roi_file, roi_lyr, None, False, False
    )


def test_query_file_lut_gztar(tmp_path):
    import rsgislib.imageutils.imagelut
    import rsgislib.imageutils
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    roi_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_lyr = "aber_osgb_single_poly"

    out_file = os.path.join(tmp_path, "out.tar.gz")
    rsgislib.imageutils.imagelut.query_file_lut(
        vec_file, vec_lyr, roi_file, roi_lyr, out_file, True, False
    )


def test_query_file_lut_cp(tmp_path):
    import rsgislib.imageutils.imagelut
    import rsgislib.imageutils
    import glob

    input_imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    vec_file = os.path.join(tmp_path, "test_lut.gpkg")
    vec_lyr = "test_lut"
    rsgislib.imageutils.imagelut.create_img_extent_lut(
        input_imgs,
        vec_file,
        vec_lyr,
        "GPKG",
        ignore_none_imgs=False,
        out_proj_wgs84=False,
        overwrite_lut_file=False,
    )

    roi_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_lyr = "aber_osgb_single_poly"

    rsgislib.imageutils.imagelut.query_file_lut(
        vec_file, vec_lyr, roi_file, roi_lyr, tmp_path, False, True
    )
