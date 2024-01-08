import os
import pytest

MATPLOTLIB_NOT_AVAIL = False
try:
    import matplotlib
except ImportError:
    MATPLOTLIB_NOT_AVAIL = True

MATPLOTLIB_SCALEBAR_NOT_AVAIL = False
try:
    import matplotlib_scalebar
except ImportError:
    MATPLOTLIB_SCALEBAR_NOT_AVAIL = True


GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_raster_img_map(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_raster_img_map(
        ax,  # specify the axis to which the image will be drawn
        input_img,  # specify the file path to the image
        img_bands=[8, 9, 3],  # specify the band combination
        img_stch=rsgislib.IMG_STRECTH_CUMULATIVE,  # specify the stretch to use
    )

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_raster_img_map_subset(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt

    bbox = [257500, 260200, 281000, 282500]

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_raster_img_map(
        ax,  # specify the axis to which the image will be drawn
        input_img,  # specify the file path to the image
        img_bands=[8, 9, 3],  # specify the band combination
        img_stch=rsgislib.IMG_STRECTH_CUMULATIVE,  # specify the stretch to use
        bbox=bbox,
    )

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_raster_cmap_img_map(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_raster_cmap_img_map(
        ax,  # specify the axis to which the image will be drawn
        input_img,  # specify the file path to the image
        img_band=1,  # specify the band
    )

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_raster_cmap_img_map_subset(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt

    bbox = [257500, 260200, 281000, 282500]

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_raster_cmap_img_map(
        ax,  # specify the axis to which the image will be drawn
        input_img,  # specify the file path to the image
        img_band=1,  # specify the band
        bbox=bbox,
    )

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_thematic_raster_map(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_thematic_raster_map(
        ax,  # specify the axis to which the image will be drawn
        input_img,  # specify the file path to the image
    )

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_thematic_raster_map_subset(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt

    bbox = [257500, 260200, 281000, 282500]

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_cls.kea")

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_thematic_raster_map(
        ax,  # specify the axis to which the image will be drawn
        input_img,  # specify the file path to the image
        bbox=bbox,
    )

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (GEOPANDAS_NOT_AVAIL or MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="geopandas, matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_vec_lyr_map_sgl_vec(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt
    import geopandas

    bbox = [257500, 260200, 281000, 282500]

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    data_gdf = geopandas.read_file(vec_file)

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_vec_lyr_map(ax, gp_vecs=data_gdf, bbox=bbox)

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)


@pytest.mark.skipif(
    (GEOPANDAS_NOT_AVAIL or MATPLOTLIB_NOT_AVAIL or MATPLOTLIB_SCALEBAR_NOT_AVAIL),
    reason="geopandas, matplotlib or matplotlib_scalebar dependency not available",
)
def test_create_vec_lyr_map_multi_vec(tmp_path):
    import rsgislib.tools.mapping
    import matplotlib.pyplot as plt
    import geopandas

    bbox = [257500, 260200, 281000, 282500]

    data_gdfs = []
    data_gdfs.append(
        geopandas.read_file(os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson"))
    )
    data_gdfs.append(
        geopandas.read_file(
            os.path.join(DATA_DIR, "aber_osgb_single_poly_hole.geojson")
        )
    )

    fig, ax = plt.subplots(figsize=(5, 5))
    rsgislib.tools.mapping.create_vec_lyr_map(ax, gp_vecs=data_gdfs, bbox=bbox)

    out_plot_img = os.path.join(tmp_path, "out_map_img.png")
    plt.savefig(out_plot_img)

    assert os.path.exists(out_plot_img)
