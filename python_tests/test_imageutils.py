import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imageutils")


def test_setEnvVarsLZWGTiffOuts():
    import rsgislib.imageutils

    rsgislib.imageutils.setEnvVarsLZWGTiffOuts(bigtiff=False)
    assert os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"] == "TILED=YES:COMPRESS=LZW"


def test_setEnvVarsLZWGTiffOuts_BigTiff():
    import rsgislib.imageutils

    rsgislib.imageutils.setEnvVarsLZWGTiffOuts(bigtiff=True)
    assert (
        os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"]
        == "TILED=YES:COMPRESS=LZW:BIGTIFF=YES"
    )


def test_getRSGISLibDataTypeFromImg():
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        rsgislib.imageutils.getRSGISLibDataTypeFromImg(input_img)
        == rsgislib.TYPE_16UINT
    )


def test_getGDALDataTypeFromImg():
    import osgeo.gdal as gdal
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.imageutils.getGDALDataTypeFromImg(input_img) == gdal.GDT_UInt16


def test_getGDALDataTypeNameFromImg():
    import osgeo.gdal as gdal
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.imageutils.getGDALDataTypeNameFromImg(input_img) == "UInt16"


def test_getImageRes():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    x_res, y_res = rsgislib.imageutils.getImageRes(input_img)
    assert (x_res == 10) and (y_res == -10)


def test_getImageRes_abs():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    x_res, y_res = rsgislib.imageutils.getImageRes(input_img, abs_vals=True)
    assert (x_res == 10) and (y_res == 10)


def test_getImageSize():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    x_size, y_size = rsgislib.imageutils.getImageSize(input_img)
    assert (x_size == 931) and (y_size == 947)


def test_getImageBandCount():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert rsgislib.imageutils.getImageBandCount(input_img) == 10


def test_getImageBBOX():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.getImageBBOX(input_img)
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

    n_bands = rsgislib.imageutils.getImageBandCount(output_img)
    if n_bands != 10:
        assert False

    x_res, y_res = rsgislib.imageutils.getImageRes(output_img)
    if (x_res != 10) and (y_res != 10):
        assert False

    x_size, y_size = rsgislib.imageutils.getImageSize(output_img)
    if (x_size != 325) and (y_size != 340):
        assert False

    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(input_img, output_img)
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

    n_bands = rsgislib.imageutils.getImageBandCount(output_img)
    if n_bands != 10:
        assert False

    x_res, y_res = rsgislib.imageutils.getImageRes(output_img)
    if (x_res != 10) and (y_res != 10):
        assert False

    x_size, y_size = rsgislib.imageutils.getImageSize(output_img)
    if (x_size != 931) and (y_size != 238):
        assert False

    img_eq, prop_match = rsgislib.imagecalc.areImgsEqual(input_img, output_img)
    assert img_eq
