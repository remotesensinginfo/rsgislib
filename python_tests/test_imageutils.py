import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imageutils")


def test_set_env_vars_lzw_gtiff_outs():
    import rsgislib.imageutils

    rsgislib.imageutils.set_env_vars_lzw_gtiff_outs(bigtiff=False)
    assert os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"] == "TILED=YES:COMPRESS=LZW"


def test_set_env_vars_lzw_gtiff_outs_bigtiff():
    import rsgislib.imageutils

    rsgislib.imageutils.set_env_vars_lzw_gtiff_outs(bigtiff=True)
    assert (
        os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"]
        == "TILED=YES:COMPRESS=LZW:BIGTIFF=YES"
    )


def test_get_rsgislib_datatype_from_img():
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        == rsgislib.TYPE_16UINT
    )


def test_get_gdal_datatype_from_img():
    import osgeo.gdal as gdal
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.imageutils.get_gdal_datatype_from_img(input_img) == gdal.GDT_UInt16


def test_get_gdal_datatype_name_from_img():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.imageutils.get_gdal_datatype_name_from_img(input_img) == "UInt16"


def test_get_image_res():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    x_res, y_res = rsgislib.imageutils.get_img_res(input_img)
    assert (x_res == 10) and (y_res == -10)


def test_get_image_res_abs():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    x_res, y_res = rsgislib.imageutils.get_img_res(input_img, abs_vals=True)
    assert (x_res == 10) and (y_res == 10)


def get_img_size():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    x_size, y_size = rsgislib.imageutils.get_img_size(input_img)
    assert (x_size == 931) and (y_size == 947)


def test_get_image_band_count():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.imageutils.get_img_band_count(input_img) == 10


def test_get_image_bbox():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.get_img_bbox(input_img)
    assert (
        (abs(bbox[0] - 257060.59483340546) < 0.0001)
        and (abs(bbox[1] - 266370.59483340546) < 0.0001)
        and (abs(bbox[2] - 276258.7167059921) < 0.0001)
        and (abs(bbox[3] - 285728.7167059921) < 0.0001)
    )


def test_subset_roi_contained(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(IMGCALC_DATA_DIR, "roi_subset.geojson")
    vec_lyr = "roi_subset"
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")

    rsgislib.imageutils.subset(
        input_img, vec_file, vec_lyr, output_img, "KEA", rsgislib.TYPE_16UINT
    )

    n_bands = rsgislib.imageutils.get_img_band_count(output_img)
    if n_bands != 10:
        assert False

    x_res, y_res = rsgislib.imageutils.get_img_res(output_img)
    if (x_res != 10) and (y_res != 10):
        assert False

    x_size, y_size = rsgislib.imageutils.get_img_size(output_img)
    if (x_size != 325) and (y_size != 340):
        assert False

    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(input_img, output_img)
    assert img_eq


def test_subset_roi_overlap(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(IMGCALC_DATA_DIR, "roi_subset_part_overlap.geojson")
    vec_lyr = "roi_subset_part_overlap"
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")

    rsgislib.imageutils.subset(
        input_img, vec_file, vec_lyr, output_img, "KEA", rsgislib.TYPE_16UINT
    )

    n_bands = rsgislib.imageutils.get_img_band_count(output_img)
    if n_bands != 10:
        assert False

    x_res, y_res = rsgislib.imageutils.get_img_res(output_img)
    if (x_res != 10) and (y_res != 10):
        assert False

    x_size, y_size = rsgislib.imageutils.get_img_size(output_img)
    if (x_size != 931) and (y_size != 238):
        assert False

    img_eq, prop_match = rsgislib.imagecalc.are_imgs_equal(input_img, output_img)
    assert img_eq
