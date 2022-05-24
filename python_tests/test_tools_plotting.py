import os
import pytest

MATPLOTLIB_NOT_AVAIL = False
try:
    import matplotlib.pyplot
except ImportError:
    MATPLOTLIB_NOT_AVAIL = True


PIL_NOT_AVAIL = False
try:
    import PIL
except ImportError:
    PIL_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
TOOLS_DATA_DIR = os.path.join(DATA_DIR, "tools")

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


def test_manual_stretch_np_arr_3_bands():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1, 2, 3], bbox=None
    )

    min_max_vals = list()
    min_max_vals.append({"min": 10, "max": 400})
    min_max_vals.append({"min": 22, "max": 300})
    min_max_vals.append({"min": 1, "max": 120})

    rsgislib.tools.plotting.manual_stretch_np_arr(
        img_data, min_max_vals, no_data_val=0.0
    )


def test_manual_stretch_np_arr_1_band():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=[1], bbox=None
    )
    min_max_vals = {"min": 10, "max": 400}

    rsgislib.tools.plotting.manual_stretch_np_arr(
        img_data, min_max_vals, no_data_val=0.0
    )


def test_get_gdal_thematic_raster_mpl_imshow_basic():
    import rsgislib.tools.plotting

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")
    rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(input_img)


def test_get_gdal_thematic_raster_mpl_imshow_bbox():
    import rsgislib.tools.plotting
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")
    sub_bbox = rsgislib.imageutils.get_img_bbox(input_img)
    rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(
        input_img, bbox=sub_bbox
    )


@pytest.mark.skipif(MATPLOTLIB_NOT_AVAIL, reason="matplotlib dependency not available")
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


@pytest.mark.skipif(PIL_NOT_AVAIL, reason="PIL dependency not available")
def test_create_legend_img(tmp_path):
    import rsgislib.tools.plotting

    years = [
        "1996",
        "2007",
        "2008",
        "2009",
        "2010",
        "2015",
        "2016",
        "2017",
        "2018",
        "2019",
        "2020",
    ]
    lyr_clrs = [
        "#FFCD00",
        "#FF7700",
        "#FF0000",
        "#FF00E6",
        "#C400FF",
        "#5E00FF",
        "#001AFF",
        "#0080FF",
        "#00CDFF",
        "#00FF91",
        "#00FF22",
    ]

    legend_info_dict = dict()
    for year, clr in zip(years, lyr_clrs):
        legend_info_dict[year] = clr

    out_file = os.path.join(tmp_path, "gmw_loss_legend.png")
    font_file = os.path.join(TOOLS_DATA_DIR, "Palatino Font.ttf")

    rsgislib.tools.plotting.create_legend_img(
        legend_info_dict,
        out_img_file=out_file,
        n_cols=4,
        box_size=(60, 40),
        font=font_file,
        font_size=24,
        char_width=12,
        title_str="Loss",
        title_height=32,
        margin=5,
    )

    assert os.path.exists(out_file)
