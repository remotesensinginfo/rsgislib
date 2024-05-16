import os
import pytest
import rsgislib.tools.filetools

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
TOOLS_VIS_DATA_DIR = os.path.join(DATA_DIR, "tools", "visualisation")

gdal2tiles_cmd_avail = rsgislib.tools.filetools.is_cmd_tool_avail(
    "gdal2tiles.py", test_call_cmd=["gdal2tiles.py", "-h"]
)
gdal_translate_cmd_avail = rsgislib.tools.filetools.is_cmd_tool_avail(
    "gdal_translate", test_call_cmd=["gdal_translate", "-h"]
)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_kmz_img(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_file = os.path.join(tmp_path, "out_img.kmz")
    rsgislib.tools.visualisation.create_kmz_img(
        input_img,
        output_file,
        "3,2,1",
        reproj_wgs84=True,
        finite_msk=False,
        tmp_dir=tmp_path,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal2tiles_cmd_avail), reason="gdal2tiles.py command not available"
)
def test_create_webtiles_img_no_stats_msk_tms_true(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    os.mkdir(output_dir)

    rsgislib.tools.visualisation.create_webtiles_img_no_stats_msk(
        input_img,
        output_dir,
        "3,2,1",
        zoom_levels="2-10",
        resample="average",
        finite_msk=False,
        tms=True,
    )


@pytest.mark.skipif(
    (not gdal2tiles_cmd_avail), reason="gdal2tiles.py command not available"
)
def test_create_webtiles_img_no_stats_msk_tms_false(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    os.mkdir(output_dir)

    rsgislib.tools.visualisation.create_webtiles_img_no_stats_msk(
        input_img,
        output_dir,
        "3,2,1",
        zoom_levels="2-10",
        resample="average",
        finite_msk=False,
        tms=False,
    )


@pytest.mark.skipif(
    (not gdal2tiles_cmd_avail), reason="gdal2tiles.py command not available"
)
def test_create_webtiles_img_no_stats_msk_tms_true_finite(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    os.mkdir(output_dir)

    rsgislib.tools.visualisation.create_webtiles_img_no_stats_msk(
        input_img,
        output_dir,
        "3,2,1",
        zoom_levels="2-10",
        resample="average",
        finite_msk=True,
        tms=True,
    )


@pytest.mark.skipif(
    (not gdal2tiles_cmd_avail), reason="gdal2tiles.py command not available"
)
def test_create_webtiles_img_nodata_exp(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    os.mkdir(output_dir)
    with pytest.raises(rsgislib.RSGISPyException):
        rsgislib.tools.visualisation.create_webtiles_img(
            input_img,
            "3,2,1",
            output_dir,
            zoom_levels="2-10",
            tmp_dir=tmp_path,
            tms=True,
        )


@pytest.mark.skipif(
    (not gdal2tiles_cmd_avail), reason="gdal2tiles.py command not available"
)
def test_create_webtiles_img_tms_true(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    os.mkdir(output_dir)

    rsgislib.tools.visualisation.create_webtiles_img(
        input_img,
        "3,2,1",
        output_dir,
        zoom_levels="2-10",
        tmp_dir=tmp_path,
        tms=True,
        no_data_val=0.0,
    )


@pytest.mark.skipif(
    (not gdal2tiles_cmd_avail), reason="gdal2tiles.py command not available"
)
def test_create_webtiles_img_tms_false(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    os.mkdir(output_dir)

    rsgislib.tools.visualisation.create_webtiles_img(
        input_img,
        "3,2,1",
        output_dir,
        zoom_levels="2-10",
        tmp_dir=tmp_path,
        tms=False,
        no_data_val=0.0,
    )


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_imgs_sgl(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_file = os.path.join(tmp_path, "out_img.jpg")
    rsgislib.tools.visualisation.create_quicklook_imgs(
        input_img,
        "3,2,1",
        output_file,
        output_img_sizes=250,
        tmp_dir=tmp_path,
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_imgs_multi(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_files = [
        os.path.join(tmp_path, "out_img_1.jpg"),
        os.path.join(tmp_path, "out_img_2.jpg"),
    ]
    rsgislib.tools.visualisation.create_quicklook_imgs(
        input_img,
        "3,2,1",
        output_files,
        output_img_sizes=[250, 500],
        tmp_dir=tmp_path,
        no_data_val=0.0,
    )

    assert os.path.exists(output_files[0]) and os.path.exists(output_files[1])


def test_create_mbtile_file(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_file = os.path.join(tmp_path, "out_img.mbtiles")

    rsgislib.tools.visualisation.create_mbtile_file(
        input_img=input_img,
        bands="3,2,1",
        output_mbtiles=output_file,
        scale_input_img=50,
        tmp_dir=tmp_path,
        tile_format="PNG",
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail) or (not gdal2tiles_cmd_avail),
    reason="gdal_translate or gdal2tiles.py command not available",
)
def test_create_webtiles_vis_gtiff_img(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_dir = os.path.join(tmp_path, "out_files")
    output_file = os.path.join(tmp_path, "out_img.tif")

    rsgislib.tools.visualisation.create_webtiles_vis_gtiff_img(
        input_img,
        "3,2,1",
        output_dir,
        output_file,
        tmp_dir=tmp_path,
        webview=True,
        scale=0,
        tms=True,
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_overview_imgs_sgl(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea"),
    ]
    output_file = os.path.join(tmp_path, "out_img.jpg")

    rsgislib.tools.visualisation.create_quicklook_overview_imgs(
        input_imgs,
        "3,2,1",
        tmp_path,
        output_file,
        output_img_sizes=250,
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_overview_imgs_multi(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea"),
    ]
    output_files = [
        os.path.join(tmp_path, "out_img_1.jpg"),
        os.path.join(tmp_path, "out_img_2.jpg"),
    ]

    rsgislib.tools.visualisation.create_quicklook_overview_imgs(
        input_imgs,
        "3,2,1",
        tmp_path,
        output_files,
        output_img_sizes=[250, 500],
        no_data_val=0.0,
    )

    assert os.path.exists(output_files[0]) and os.path.exists(output_files[1])


def test_burn_in_binary_msk(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.tools.visualisation.burn_in_binary_msk(
        input_img, msk_img, output_img, "KEA", msk_colour=None
    )

    assert os.path.exists(output_img)


def test_create_quicklook_overview_imgs_vec_overlay_sgl(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [os.path.join(DATA_DIR, "sen2_20210527_aber.kea")]
    vec_overlay_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_overlay_lyr = "aber_osgb_single_poly"
    output_file = os.path.join(tmp_path, "out_img.jpg")

    rsgislib.tools.visualisation.create_quicklook_overview_imgs_vec_overlay(
        input_imgs,
        "3,2,1",
        tmp_path,
        vec_overlay_file,
        vec_overlay_lyr,
        output_file,
        output_img_sizes=250,
        gdalformat="JPEG",
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_overview_imgs_vec_overlay_sgl_clr(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [os.path.join(DATA_DIR, "sen2_20210527_aber.kea")]
    vec_overlay_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_overlay_lyr = "aber_osgb_single_poly"
    output_file = os.path.join(tmp_path, "out_img.jpg")

    rsgislib.tools.visualisation.create_quicklook_overview_imgs_vec_overlay(
        input_imgs,
        "3,2,1",
        tmp_path,
        vec_overlay_file,
        vec_overlay_lyr,
        output_file,
        output_img_sizes=250,
        gdalformat="JPEG",
        overlay_clrs=[255, 0, 0],
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_overview_imgs_vec_overlay_multi(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
    ]
    vec_overlay_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_overlay_lyr = "aber_osgb_single_poly"
    output_file = os.path.join(tmp_path, "out_img.jpg")

    rsgislib.tools.visualisation.create_quicklook_overview_imgs_vec_overlay(
        input_imgs,
        "3,2,1",
        tmp_path,
        vec_overlay_file,
        vec_overlay_lyr,
        output_file,
        output_img_sizes=250,
        gdalformat="JPEG",
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_overview_imgs_vec_overlay_multi_clr(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
    ]
    vec_overlay_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_overlay_lyr = "aber_osgb_single_poly"
    output_file = os.path.join(tmp_path, "out_img.jpg")

    rsgislib.tools.visualisation.create_quicklook_overview_imgs_vec_overlay(
        input_imgs,
        "3,2,1",
        tmp_path,
        vec_overlay_file,
        vec_overlay_lyr,
        output_file,
        output_img_sizes=250,
        gdalformat="JPEG",
        overlay_clrs=[255, 0, 0],
        no_data_val=0.0,
    )

    assert os.path.exists(output_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_quicklook_overview_imgs_vec_overlay_multi_clr_multi_out(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
        os.path.join(DATA_DIR, "sen2_20210527_aber.kea"),
    ]
    vec_overlay_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_overlay_lyr = "aber_osgb_single_poly"
    output_files = [
        os.path.join(tmp_path, "out_img_0.jpg"),
        os.path.join(tmp_path, "out_img_1.jpg"),
    ]

    rsgislib.tools.visualisation.create_quicklook_overview_imgs_vec_overlay(
        input_imgs,
        "3,2,1",
        tmp_path,
        vec_overlay_file,
        vec_overlay_lyr,
        output_files,
        output_img_sizes=[250, 500],
        gdalformat="JPEG",
        overlay_clrs=[255, 0, 0],
        no_data_val=0.0,
    )

    assert os.path.exists(output_files[0]) and os.path.exists(output_files[1])


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_visual_overview_imgs_vec_extent(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [os.path.join(DATA_DIR, "sen2_20210527_aber.kea")]
    output_file = os.path.join(tmp_path, "out_img.tif")
    stretch_file = os.path.join(tmp_path, "strch_info.txt")
    rsgislib.tools.visualisation.create_visual_overview_imgs_vec_extent(
        input_imgs,
        "3,2,1",
        tmp_dir=tmp_path,
        vec_extent_file=None,
        vec_extent_lyr=None,
        output_imgs=output_file,
        output_img_sizes=500,
        gdalformat="GTIFF",
        scale_axis="auto",
        stretch_file=stretch_file,
        export_stretch_file=True,
        no_data_val=0.0,
    )

    assert os.path.exists(output_file) and os.path.exists(stretch_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_visual_overview_imgs_vec_extent_multi_out(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [os.path.join(DATA_DIR, "sen2_20210527_aber.kea")]
    output_files = [
        os.path.join(tmp_path, "out_img_0.tif"),
        os.path.join(tmp_path, "out_img_1.tif"),
    ]
    stretch_file = os.path.join(tmp_path, "strch_info.txt")

    rsgislib.tools.visualisation.create_visual_overview_imgs_vec_extent(
        input_imgs,
        "3,2,1",
        tmp_dir=tmp_path,
        vec_extent_file=None,
        vec_extent_lyr=None,
        output_imgs=output_files,
        output_img_sizes=[250, 500],
        gdalformat="GTIFF",
        scale_axis="auto",
        stretch_file=stretch_file,
        export_stretch_file=True,
        no_data_val=0.0,
    )

    assert (
        os.path.exists(output_files[0])
        and os.path.exists(output_files[1])
        and os.path.exists(stretch_file)
    )


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_visual_overview_imgs_vec_extent_vec_ext(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [os.path.join(DATA_DIR, "sen2_20210527_aber.kea")]
    output_file = os.path.join(tmp_path, "out_img.tif")
    stretch_file = os.path.join(tmp_path, "strch_info.txt")
    vec_extent_file = os.path.join(TOOLS_VIS_DATA_DIR, "wales_bbox.geojson")
    vec_extent_lyr = "wales_bbox"

    rsgislib.tools.visualisation.create_visual_overview_imgs_vec_extent(
        input_imgs,
        "3,2,1",
        tmp_dir=tmp_path,
        vec_extent_file=vec_extent_file,
        vec_extent_lyr=vec_extent_lyr,
        output_imgs=output_file,
        output_img_sizes=500,
        gdalformat="GTIFF",
        scale_axis="auto",
        stretch_file=stretch_file,
        export_stretch_file=True,
        no_data_val=0.0,
    )

    assert os.path.exists(output_file) and os.path.exists(stretch_file)


@pytest.mark.skipif(
    (not gdal_translate_cmd_avail), reason="gdal_translate command not available"
)
def test_create_visual_overview_imgs_vec_extent_multi_out_vec_ext(tmp_path):
    import rsgislib.tools.visualisation

    input_imgs = [os.path.join(DATA_DIR, "sen2_20210527_aber.kea")]
    output_files = [
        os.path.join(tmp_path, "out_img_0.tif"),
        os.path.join(tmp_path, "out_img_1.tif"),
    ]
    stretch_file = os.path.join(tmp_path, "strch_info.txt")
    vec_extent_file = os.path.join(TOOLS_VIS_DATA_DIR, "wales_bbox.geojson")
    vec_extent_lyr = "wales_bbox"

    rsgislib.tools.visualisation.create_visual_overview_imgs_vec_extent(
        input_imgs,
        "3,2,1",
        tmp_dir=tmp_path,
        vec_extent_file=vec_extent_file,
        vec_extent_lyr=vec_extent_lyr,
        output_imgs=output_files,
        output_img_sizes=[250, 500],
        gdalformat="GTIFF",
        scale_axis="auto",
        stretch_file=stretch_file,
        export_stretch_file=True,
        no_data_val=0.0,
    )

    assert (
        os.path.exists(output_files[0])
        and os.path.exists(output_files[1])
        and os.path.exists(stretch_file)
    )


def test_overlay_vec_on_img(tmp_path):
    import rsgislib.tools.visualisation

    input_img = os.path.join(
        TOOLS_VIS_DATA_DIR, "sen2_20210527_aber_subset_b123_stch.kea"
    )
    output_img = os.path.join(tmp_path, "out_img.png")
    vec_overlay_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_overlay_lyr = "aber_osgb_single_poly"

    rsgislib.tools.visualisation.overlay_vec_on_img(
        input_img,
        output_img,
        vec_overlay_file,
        vec_overlay_lyr,
        tmp_dir=tmp_path,
        gdalformat="PNG",
        overlay_clrs=[0, 255, 0],
    )

    assert os.path.exists(output_img)
