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


def test_linear_stretch_np_arr_3_bands():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1, 2, 3], bbox=None
    )
    rsgislib.tools.plotting.linear_stretch_np_arr(img_data, no_data_val=0.0)


def test_linear_stretch_np_arr_1_band():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1], bbox=None
    )
    rsgislib.tools.plotting.linear_stretch_np_arr(img_data, no_data_val=0.0)


def test_cumulative_stretch_np_arr_3_bands():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1, 2, 3], bbox=None
    )
    rsgislib.tools.plotting.cumulative_stretch_np_arr(img_data, no_data_val=0.0)


def test_cumulative_stretch_np_arr_1_band():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1], bbox=None
    )
    rsgislib.tools.plotting.cumulative_stretch_np_arr(img_data, no_data_val=0.0)


def test_stdev_stretch_np_arr_3_bands():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1, 2, 3], bbox=None
    )
    rsgislib.tools.plotting.stdev_stretch_np_arr(img_data, no_data_val=0.0)


def test_stdev_stretch_np_arr_1_band():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1], bbox=None
    )
    rsgislib.tools.plotting.stdev_stretch_np_arr(img_data, no_data_val=0.0)


def test_get_gdal_thematic_raster_mpl_imshow_basic():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")
    rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(input_img)


def test_get_gdal_raster_mpl_imshow_bands_bbox():
    import rsgislib.tools.plotting
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")
    sub_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(
        input_img, bbox=sub_bbox
    )


def test_get_gdal_thematic_raster_mpl_imshow_patches():
    import rsgislib.tools.plotting

    cls_names_lut = dict()
    cls_names_lut[1] = "Forest"
    cls_names_lut[2] = "Grass"
    cls_names_lut[3] = "Urban"
    cls_names_lut[4] = "Water"

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")
    rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(
        input_img, out_patches=True, cls_names_lut=cls_names_lut
    )
