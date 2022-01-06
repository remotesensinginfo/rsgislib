import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_get_gdal_raster_mpl_imshow_basic():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(input_img, bands=None, bbox=None)


def test_get_gdal_raster_mpl_imshow_bands():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[8, 9, 3], bbox=None
    )


def test_get_gdal_raster_mpl_imshow_bands_bbox():
    import rsgislib.tools.plotting
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    sub_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[8, 9, 3], bbox=sub_bbox
    )
