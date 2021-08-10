import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGCALC_DATA_DIR = os.path.join(DATA_DIR, "imageutils")


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
