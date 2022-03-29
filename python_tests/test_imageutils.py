import os
import pytest
from shutil import copy2

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGUTILS_DATA_DIR = os.path.join(DATA_DIR, "imageutils")
IMGREG_DATA_DIR = os.path.join(DATA_DIR, "imageregistration")


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


def test_set_env_vars_deflate_gtiff_outs():
    import rsgislib.imageutils

    rsgislib.imageutils.set_env_vars_deflate_gtiff_outs(bigtiff=False)
    assert os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"] == "TILED=YES:COMPRESS=DEFLATE"


def test_set_env_vars_deflate_gtiff_outs_bigtiff():
    import rsgislib.imageutils

    rsgislib.imageutils.set_env_vars_deflate_gtiff_outs(bigtiff=True)
    assert (
        os.environ["RSGISLIB_IMG_CRT_OPTS_GTIFF"]
        == "TILED=YES:COMPRESS=DEFLATE:BIGTIFF=YES"
    )


def test_get_gdal_img_creation_opts_none():
    import rsgislib.imageutils

    create_opts = rsgislib.imageutils.get_gdal_img_creation_opts(gdalformat="KEA")
    assert create_opts is None


def test_get_gdal_img_creation_opts_gtiff():
    import rsgislib.imageutils

    rsgislib.imageutils.set_env_vars_lzw_gtiff_outs(bigtiff=False)
    create_opts = rsgislib.imageutils.get_gdal_img_creation_opts(gdalformat="GTIFF")
    assert (create_opts["TILED"] == "YES") and (create_opts["COMPRESS"] == "LZW")


def test_get_rsgislib_datatype_from_img():
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert (
        rsgislib.imageutils.get_rsgislib_datatype_from_img(input_img)
        == rsgislib.TYPE_16UINT
    )


def test_get_gdal_datatype_from_img():
    from osgeo import gdal
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


def test_get_img_bbox_in_proj_4326():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.get_img_bbox_in_proj(input_img, out_epsg=4326)
    assert (
        (abs(bbox[0] - -4.105) < 0.1)
        and (abs(bbox[1] - -3.964) < 0.1)
        and (abs(bbox[2] - 52.366) < 0.1)
        and (abs(bbox[3] - 52.453) < 0.1)
    )


def test_get_img_bbox_in_proj_utm30n():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.get_img_bbox_in_proj(input_img, out_epsg=32630)
    assert (
        (abs(bbox[0] - 424918.88) < 3.0)
        and (abs(bbox[1] - 434357.09) < 3.0)
        and (abs(bbox[2] - 5802283.59) < 3.0)
        and (abs(bbox[3] - 5811879.561707063) < 3.0)
    )


def test_get_img_subset_pxl_bbox():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    sub_bbox = [260000, 264000, 280000, 285000]
    bbox = rsgislib.imageutils.get_img_subset_pxl_bbox(input_img, sub_bbox=sub_bbox)
    assert (
        (bbox[0] == 294) and (bbox[1] == 694) and (bbox[2] == 73) and (bbox[3] == 573)
    )


def test_get_img_pxl_spatial_coords():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    sub_pxl_bbox = [294, 694, 73, 573]
    bbox = rsgislib.imageutils.get_img_pxl_spatial_coords(
        input_img, sub_pxl_bbox=sub_pxl_bbox
    )
    assert (
        (abs(bbox[0] - 260000.59) < 1.0)
        and (abs(bbox[1] - 264000.59) < 1.0)
        and (abs(bbox[2] - 279998.72) < 1.0)
        and (abs(bbox[3] - 284998.72) < 1.0)
    )


def test_subset_roi_contained(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import rsgislib.imagecalc

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(IMGUTILS_DATA_DIR, "roi_subset.geojson")
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
    vec_file = os.path.join(IMGUTILS_DATA_DIR, "roi_subset_part_overlap.geojson")
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


def test_pop_img_stats(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.pop_img_stats(
        input_img, use_no_data=True, no_data_val=0, calc_pyramids=True
    )


def test_pop_thmt_img_stats(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.pop_thmt_img_stats(
        input_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
    )


def test_get_img_band_stats():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    min, max, mean, std = rsgislib.imageutils.get_img_band_stats(
        input_img, img_band=1, compute=True
    )

    assert (
        (min == 1.0)
        and (max == 1066.0)
        and (abs(mean - 63.13) < 0.01)
        and (abs(std - 32.44) < 0.01)
    )


def test_get_img_no_data_value_zero():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")

    no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    assert no_data_val == 0


def test_get_img_no_data_value_none():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    assert no_data_val is None


def test_set_img_no_data_value(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_no_data_value(input_img, no_data_val=100)
    no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    assert no_data_val == 100


def test_get_img_files():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    files = rsgislib.imageutils.get_img_files(input_img)
    assert len(files) == 1


def test_has_gcps_false():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    assert not rsgislib.imageutils.has_gcps(input_img)


def test_has_gcps_true():
    import rsgislib.imageutils

    input_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset_gcps.kea"
    )
    assert rsgislib.imageutils.has_gcps(input_img)


def test_is_img_thematic_False():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    assert not rsgislib.imageutils.is_img_thematic(input_img)


def test_is_img_thematic_True():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    assert rsgislib.imageutils.is_img_thematic(input_img)


def test_get_gdal_format_name():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    assert "KEA" == rsgislib.imageutils.get_gdal_format_name(input_img)


def test_get_epsg_proj_from_img_27700():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    assert 27700 == rsgislib.imageutils.get_epsg_proj_from_img(input_img)


def test_get_epsg_proj_from_img_32630():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    assert 32630 == rsgislib.imageutils.get_epsg_proj_from_img(input_img)


def test_get_wkt_proj_from_img_osgb():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    assert "British National Grid" in rsgislib.imageutils.get_wkt_proj_from_img(
        input_img
    )


def test_get_wkt_proj_from_img_utm():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)


def test_assign_wkt_proj_file(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")

    rsgislib.imageutils.assign_wkt_proj(input_img, None, wkt_file)

    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)


def test_assign_wkt_proj_str(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    wkt_str = """PROJCS["WGS 84 / UTM zone 30N",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",-3],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AXIS["Easting",EAST],AXIS["Northing",NORTH],AUTHORITY["EPSG","32630"]]"""

    rsgislib.imageutils.assign_wkt_proj(input_img, wkt_str, None)

    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)


def test_assign_spatial_info(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.assign_spatial_info(
        input_img, None, None, 100, -100, None, None
    )
    x_res, y_res = rsgislib.imageutils.get_img_res(input_img)
    assert (x_res == 100) and (y_res == -100)


def test_copy_proj_from_img(tmp_path):
    import rsgislib.imageutils

    input_utm_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.copy_proj_from_img(input_img, input_utm_img)

    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)


def test_copy_spatial_and_proj_from_img(tmp_path):
    import rsgislib.imageutils

    input_utm_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.copy_spatial_and_proj_from_img(input_img, input_utm_img)

    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)


def test_get_utm_zone():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    assert "30N" == rsgislib.imageutils.get_utm_zone(input_img)


def test_copy_gcps(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset_gcps.kea"
    )

    output_ref_img = os.path.join(
        IMGREG_DATA_DIR, "sen2_20210527_aber_subset_b123_offset.kea"
    )
    output_img = os.path.join(tmp_path, "sen2_20210527_aber_subset_b123_offset.kea")
    copy2(output_ref_img, output_img)

    rsgislib.imageutils.copy_gcps(input_img, output_img)

    assert rsgislib.imageutils.has_gcps(output_img)


def test_resample_img_to_match(tmp_path):
    import rsgislib.imageutils

    input_utm_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    input_osgb_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.resample_img_to_match(
        input_utm_img, input_osgb_img, output_img, "KEA"
    )

    assert os.path.exists(output_img)


def test_reproject_image(tmp_path):
    import rsgislib.imageutils

    input_osgb_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")

    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.reproject_image(input_osgb_img, output_img, wkt_file)

    assert os.path.exists(output_img)


def test_gdal_warp(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.gdal_warp(
        input_img, output_img, 4326, gdalformat="KEA", use_multi_threaded=False
    )

    assert os.path.exists(output_img)


def test_create_img_mosaic(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_img_mosaic(
        imgs, output_img, 0, 0, 1, 0, "KEA", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_include_imgs(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img(
        input_ref_img, output_img, 10, 0, "KEA", rsgislib.TYPE_16UINT
    )

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    rsgislib.imageutils.include_imgs(output_img, imgs)

    assert os.path.exists(output_img)


def test_include_imgs_with_overlap(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img(
        input_ref_img, output_img, 10, 0, "KEA", rsgislib.TYPE_16UINT
    )

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_over_tiles", "*.kea"))
    rsgislib.imageutils.include_imgs_with_overlap(output_img, imgs, overlap=20)

    assert os.path.exists(output_img)


def test_include_imgs_ind_img_intersect(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img(
        input_ref_img, output_img, 10, 0, "KEA", rsgislib.TYPE_16UINT
    )

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    rsgislib.imageutils.include_imgs_ind_img_intersect(output_img, imgs)

    assert os.path.exists(output_img)


def test_combine_imgs_to_band(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "cls_imgs", "*.kea"))
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.combine_imgs_to_band(
        imgs, output_img, gdalformat="KEA", datatype=rsgislib.TYPE_8UINT, no_data_val=0
    )

    assert os.path.exists(output_img)


# TODO rsgislib.imageutils.create_ref_img_composite_img


def test_combine_binary_masks(tmp_path):
    import rsgislib
    import rsgislib.imageutils

    msk_imgs_dict = dict()
    msk_imgs_dict["forest"] = os.path.join(
        IMGUTILS_DATA_DIR, "cls_imgs", "Forest_1226565d33.kea"
    )
    msk_imgs_dict["grass"] = os.path.join(
        IMGUTILS_DATA_DIR, "cls_imgs", "Grass_1226565d33.kea"
    )
    msk_imgs_dict["urban"] = os.path.join(
        IMGUTILS_DATA_DIR, "cls_imgs", "Urban_1226565d33.kea"
    )
    msk_imgs_dict["water"] = os.path.join(
        IMGUTILS_DATA_DIR, "cls_imgs", "Water_1226565d33.kea"
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    out_lut_file = os.path.join(tmp_path, "out_lut.json")
    rsgislib.imageutils.combine_binary_masks(
        msk_imgs_dict, output_img, out_lut_file, gdalformat="KEA"
    )

    assert os.path.exists(output_img) and os.path.exists(out_lut_file)


# TODO rsgislib.imageutils.export_single_merged_img_band
# TODO rsgislib.imageutils.order_img_using_prop_valid_pxls
# TODO rsgislib.imageutils.gen_timeseries_fill_composite_img


def test_create_tiles(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_img_base = os.path.join(tmp_path, "out_img")
    rsgislib.imageutils.create_tiles(
        input_img, out_img_base, 200, 200, 0, False, "KEA", rsgislib.TYPE_16UINT, "kea"
    )

    assert len(glob.glob("{}*.kea".format(out_img_base))) == 25


def test_create_tiles_outpath_exp(tmp_path):
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_img_base = os.path.join(tmp_path, "output1", "output2", "out_img")
    with pytest.raises(Exception):
        rsgislib.imageutils.create_tiles(
            input_img,
            out_img_base,
            200,
            200,
            0,
            False,
            "KEA",
            rsgislib.TYPE_16UINT,
            "kea",
        )


def test_create_tiles_multi_core(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_img_base = os.path.join(tmp_path, "out_img")
    rsgislib.imageutils.create_tiles_multi_core(
        input_img, out_img_base, 200, 200, "KEA", rsgislib.TYPE_16UINT, "kea", 1
    )

    assert len(glob.glob("{}*.kea".format(out_img_base))) == 25


def test_create_tiles_multi_core_outpath_exp(tmp_path):
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    out_img_base = os.path.join(tmp_path, "output1", "output2", "out_img")
    print(out_img_base)
    with pytest.raises(Exception):
        rsgislib.imageutils.create_tiles_multi_core(
            input_img, out_img_base, 200, 200, "KEA", rsgislib.TYPE_16UINT, "kea", 1
        )


def test_stretch_img(tmp_path):
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.stretch_img(
        input_img,
        output_img,
        False,
        "",
        0,
        False,
        "KEA",
        rsgislib.TYPE_8UINT,
        rsgislib.imageutils.STRETCH_LINEARPERCENT,
        2,
    )

    assert os.path.exists(output_img)


def test_stretch_img_with_stats(tmp_path):
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    in_stats_file = os.path.join(IMGUTILS_DATA_DIR, "s2_stch_stats.txt")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.stretch_img_with_stats(
        input_img,
        output_img,
        in_stats_file,
        "KEA",
        rsgislib.TYPE_8UINT,
        0,
        rsgislib.imageutils.STRETCH_LINEARMINMAX,
        2,
    )

    assert os.path.exists(output_img)


def test_normalise_img_pxl_vals(tmp_path):
    import rsgislib
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.normalise_img_pxl_vals(
        input_img,
        output_img,
        "KEA",
        rsgislib.TYPE_32FLOAT,
        in_no_data_val=0,
        out_no_data_val=0,
        out_min=0,
        out_max=1,
        stretch_type=rsgislib.imageutils.STRETCH_LINEARPERCENT,
        stretch_param=2,
    )

    assert os.path.exists(output_img)


def test_get_img_band_colour_interp():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    assert rsgislib.imageutils.get_img_band_colour_interp(input_img, 1) == 1


def test_set_img_band_colour_interp(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_band_colour_interp(
        input_img, img_band=1, clr_itrp_val=3
    )

    assert rsgislib.imageutils.get_img_band_colour_interp(input_img, 1) == 3


def test_set_img_thematic(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_thematic(input_img)

    assert rsgislib.imageutils.is_img_thematic(input_img)


def test_set_img_not_thematic(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_not_thematic(input_img)

    assert not rsgislib.imageutils.is_img_thematic(input_img)


def test_define_colour_table(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_clumps.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_clumps.kea")
    copy2(input_ref_img, input_img)

    clr_lut = dict()
    clr_lut[1] = "#009600"
    clr_lut[2] = "#FFE5CC"
    clr_lut[3] = "#CCFFE5"
    rsgislib.imageutils.define_colour_table(input_img, clr_lut)


def test_mask_img(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.mask_img(
        input_img, in_msk_img, output_img, "KEA", rsgislib.TYPE_16UINT, 0, 0
    )

    assert os.path.exists(output_img)


def test_gen_finite_mask(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.gen_finite_mask(input_img, output_img, gdalformat="KEA")

    assert os.path.exists(output_img)


def test_gen_valid_mask(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.gen_valid_mask(
        input_img, output_img, gdalformat="KEA", no_data_val=0.0
    )

    assert os.path.exists(output_img)


def test_gen_img_edge_mask(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.gen_img_edge_mask(
        input_img, output_img, gdalformat="KEA", n_edge_pxls=10
    )

    assert os.path.exists(output_img)


def test_mask_img_with_vec(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    roi_vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_vec_lyr = "aber_osgb_single_poly"
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.mask_img_with_vec(
        input_img,
        output_img,
        "KEA",
        roi_vec_file,
        roi_vec_lyr,
        tmp_path,
        outvalue=0,
        datatype=rsgislib.TYPE_16UINT,
        vec_epsg=None,
    )

    assert os.path.exists(output_img)


def test_mask_all_band_zero_vals(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.mask_all_band_zero_vals(
        input_img, output_img, gdalformat="KEA", out_val=1
    )

    assert os.path.exists(output_img)


def test_create_valid_mask(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber.kea")
    copy2(input_ref_img, input_img)
    rsgislib.imageutils.set_img_no_data_value(input_img, 0.0)

    img_band_info = list()
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(
            file_name=input_img, name="s2_1", bands=[1, 2]
        )
    )
    img_band_info.append(
        rsgislib.imageutils.ImageBandInfo(
            file_name=input_img, name="s2_2", bands=[3, 4]
        )
    )

    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_valid_mask(
        img_band_info, output_img, gdalformat="KEA", tmp_dir=tmp_path
    )

    assert os.path.exists(output_img)


def test_subset(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.subset(
        input_img, vec_file, vec_lyr, output_img, "KEA", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


def test_subset_to_img(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_roi_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.subset_to_img(
        input_img, in_roi_img, output_img, "KEA", rsgislib.TYPE_16UINT
    )

    assert os.path.exists(output_img)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_subset_to_geoms_bbox(tmp_path):
    import rsgislib.imageutils
    import glob

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"
    out_img_base = os.path.join(tmp_path, "out_img")
    rsgislib.imageutils.subset_to_geoms_bbox(
        input_img,
        vec_file,
        vec_lyr,
        "val",
        out_img_base,
        gdalformat="KEA",
        datatype=None,
        out_img_ext="kea",
    )

    assert len(glob.glob("{}*.kea".format(out_img_base))) == 4


def test_subset_bbox(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_roi_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    bbox = rsgislib.imageutils.get_img_bbox(in_roi_img)
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.subset_bbox(
        input_img,
        output_img,
        "KEA",
        rsgislib.TYPE_16UINT,
        bbox[0],
        bbox[1],
        bbox[2],
        bbox[3],
    )

    assert os.path.exists(output_img)


def test_subset_pxl_bbox(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.subset_pxl_bbox(
        input_img, output_img, "KEA", rsgislib.TYPE_16UINT, 200, 300, 200, 300
    )

    assert os.path.exists(output_img)


def test_subset_to_vec(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    roi_vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    roi_vec_lyr = "aber_osgb_single_poly"
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.subset_to_vec(
        input_img,
        output_img,
        "KEA",
        roi_vec_file,
        roi_vec_lyr,
        datatype=rsgislib.TYPE_16UINT,
        vec_epsg=27700,
    )

    assert os.path.exists(output_img)


def test_perform_random_pxl_sample_in_mask(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.perform_random_pxl_sample_in_mask(
        input_img, output_img, gdalformat="KEA", mask_vals=1, n_samples=10
    )
    assert os.path.exists(output_img)


def test_perform_random_pxl_sample_in_mask_low_pxl_count(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.perform_random_pxl_sample_in_mask_low_pxl_count(
        input_img, output_img, gdalformat="KEA", mask_vals=1, n_samples=10
    )
    assert os.path.exists(output_img)


def test_extract_img_pxl_sample():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")
    vals = rsgislib.imageutils.extract_img_pxl_sample(input_img, pxl_n_sample=10)
    assert vals.shape[0] == 88165


def test_extract_img_pxl_vals_in_msk():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    in_msk_img = os.path.join(DATA_DIR, "aber_osgb_multi_polys_rasters.kea")
    vals = rsgislib.imageutils.extract_img_pxl_vals_in_msk(
        input_img, img_bands=[1, 2, 3], in_msk_img=in_msk_img, img_mask_val=4
    )
    assert vals.shape[0] == 21957


def test_create_blank_img(tmp_path):
    import rsgislib.imageutils

    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_blank_img(
        output_img,
        1,
        200,
        200,
        20000,
        500000,
        50,
        -100,
        0,
        wkt_file,
        "",
        "KEA",
        rsgislib.TYPE_32INT,
    )
    assert os.path.exists(output_img)


def test_create_blank_img_py(tmp_path):
    import rsgislib.imageutils
    import rsgislib.tools.utils

    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")
    wkt_str = rsgislib.tools.utils.read_text_file_no_new_lines(wkt_file)
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_blank_img_py(
        output_img,
        3,
        500,
        500,
        20000,
        500000,
        50,
        -10,
        wkt_str,
        "KEA",
        rsgislib.TYPE_32UINT,
    )
    assert os.path.exists(output_img)


def test_create_copy_img(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img(
        input_img, output_img, 3, 5, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_create_copy_img_def_extent(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    bbox = rsgislib.imageutils.get_img_bbox(input_img)
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img_def_extent(
        input_img,
        output_img,
        2,
        1,
        bbox[0],
        bbox[1],
        bbox[2],
        bbox[3],
        20,
        -20,
        "KEA",
        rsgislib.TYPE_32FLOAT,
    )
    assert os.path.exists(output_img)


def test_create_blank_buf_img_from_ref_img(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_blank_buf_img_from_ref_img(
        input_img, output_img, "KEA", rsgislib.TYPE_32FLOAT, 50, no_data_val=0
    )
    assert os.path.exists(output_img)


def test_create_blank_img_from_ref_vector(tmp_path):
    import rsgislib.imageutils

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_blank_img_from_ref_vector(
        vec_file, vec_lyr, output_img, 20, 4, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_create_copy_img_vec_extent_snap_to_grid(tmp_path):
    import rsgislib.imageutils

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img_vec_extent_snap_to_grid(
        vec_file, vec_lyr, output_img, 20, 4, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_create_blank_img_from_bbox(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    bbox = rsgislib.imageutils.get_img_bbox(input_img)
    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")
    wkt_str = rsgislib.tools.utils.read_text_file_no_new_lines(wkt_file)
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_blank_img_from_bbox(
        bbox, wkt_str, output_img, 5, 10, 5, "KEA", rsgislib.TYPE_32FLOAT, True
    )
    assert os.path.exists(output_img)


def test_create_img_for_each_vec_feat(tmp_path):
    import rsgislib.imageutils
    import glob

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"
    rsgislib.imageutils.create_img_for_each_vec_feat(
        vec_file,
        vec_lyr,
        file_name_col="tile_name",
        out_img_path=tmp_path,
        out_img_ext="kea",
        out_img_pxl_val=0,
        out_img_n_bands=1,
        out_img_res=25,
        gdalformat="KEA",
        datatype=rsgislib.TYPE_16INT,
        snap_to_grid=True,
    )
    assert len(glob.glob(os.path.join(tmp_path, "*.kea"))) == 4


def test_create_copy_img_vec_extent(tmp_path):
    import rsgislib.imageutils

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"
    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.create_copy_img_vec_extent(
        input_img, vec_file, vec_lyr, output_img, 1, 10, "KEA", rsgislib.TYPE_32FLOAT
    )
    assert os.path.exists(output_img)


def test_gdal_translate(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.tif")
    rsgislib.imageutils.gdal_translate(input_img, output_img, gdalformat="GTIFF")
    assert os.path.exists(output_img)


def test_create_stack_images_vrt(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.vrt")
    rsgislib.imageutils.create_stack_images_vrt([input_img, input_img], output_img)
    assert os.path.exists(output_img)


def test_create_mosaic_images_vrt(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    output_img = os.path.join(tmp_path, "out_img.vrt")
    rsgislib.imageutils.create_mosaic_images_vrt(imgs, output_img)

    assert os.path.exists(output_img)


def test_create_mosaic_images_vrt_extent(tmp_path):
    import rsgislib
    import rsgislib.imageutils
    import glob

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.get_img_bbox(input_img)

    imgs = glob.glob(os.path.join(IMGUTILS_DATA_DIR, "s2_tiles", "*.kea"))
    output_img = os.path.join(tmp_path, "out_img.vrt")
    rsgislib.imageutils.create_mosaic_images_vrt(imgs, output_img, vrt_extent=bbox)

    assert os.path.exists(output_img)


def test_create_vrt_band_subset(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.vrt")
    rsgislib.imageutils.create_vrt_band_subset(input_img, [1, 2, 3], output_img)
    assert os.path.exists(output_img)


def test_select_img_bands(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.select_img_bands(
        input_img, output_img, "KEA", rsgislib.TYPE_16UINT, [1, 2, 3]
    )
    assert os.path.exists(output_img)


def test_stack_img_bands(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")
    rsgislib.imageutils.stack_img_bands(
        [input_img, input_img], None, output_img, 0, 0, "KEA", rsgislib.TYPE_16UINT
    )
    assert os.path.exists(output_img)


# TODO rsgislib.imageutils.pan_sharpen_hcs
# TODO rsgislib.imageutils.sharpen_low_res_bands


def test_get_band_names():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    band_names = rsgislib.imageutils.get_band_names(input_img)
    assert len(band_names) == 3


def test_set_band_names(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset_b123.kea")
    copy2(input_ref_img, input_img)
    rsgislib.imageutils.set_band_names(input_img, band_names=["b1", "b2", "b3"])

    band_names = rsgislib.imageutils.get_band_names(input_img)
    assert band_names[0] == "b1" and band_names[1] == "b2" and band_names[2] == "b3"


def test_do_img_res_match():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    assert rsgislib.imageutils.do_img_res_match(input_img, input_img)


def test_check_img_lst():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    input_imgs = [input_img, input_img, input_img]
    bbox = rsgislib.imageutils.get_img_bbox(input_img)
    rsgislib.imageutils.check_img_lst(
        input_imgs, 10, -10, bbox, print_errors=True, abs_res=True
    )


def test_check_img_file_comparison():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    rsgislib.imageutils.check_img_file_comparison(
        input_img, input_img, test_n_bands=True, test_eql_bbox=True, print_errors=True
    )


def test_do_images_overlap():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    input_sub_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")

    assert rsgislib.imageutils.do_images_overlap(input_img, input_sub_img)


def test_do_gdal_layers_have_same_proj_true():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    input_sub_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")

    assert rsgislib.imageutils.do_gdal_layers_have_same_proj(input_img, input_sub_img)


def test_do_gdal_layers_have_same_proj_false():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    input_utm_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")

    assert not rsgislib.imageutils.do_gdal_layers_have_same_proj(
        input_img, input_utm_img
    )


def test_test_img_lst_intersects():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    input_sub_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_b123.kea")
    rsgislib.imageutils.test_img_lst_intersects([input_img, input_sub_img])


def test_calc_wgs84_pixel_area(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_wgs84.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.imageutils.calc_wgs84_pixel_area(input_img, output_img, gdalformat="KEA")

    assert os.path.exists(output_img)


def test_calc_wsg84_pixel_size(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_wgs84.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.imageutils.calc_wsg84_pixel_size(input_img, output_img, gdalformat="KEA")

    assert os.path.exists(output_img)


def test_calc_pixel_locations(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    output_img = os.path.join(tmp_path, "out_img.kea")

    rsgislib.imageutils.calc_pixel_locations(input_img, output_img, gdalformat="KEA")

    assert os.path.exists(output_img)


def test_get_file_img_extension_kea():
    import rsgislib.imageutils

    assert rsgislib.imageutils.get_file_img_extension("KEA") == "kea"


def test_get_file_img_extension_tif():
    import rsgislib.imageutils

    assert rsgislib.imageutils.get_file_img_extension("GTIFF") == "tif"


def test_get_file_img_extension_hfa():
    import rsgislib.imageutils

    assert rsgislib.imageutils.get_file_img_extension("HFA") == "img"


def test_get_file_img_extension_envi():
    import rsgislib.imageutils

    assert rsgislib.imageutils.get_file_img_extension("ENVI") == "env"


def test_get_file_img_extension_pcidsk():
    import rsgislib.imageutils

    assert rsgislib.imageutils.get_file_img_extension("PCIDSK") == "pix"


def test_rename_gdal_layer(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.rename_gdal_layer(input_img, output_img)

    assert os.path.exists(output_img)


def test_delete_gdal_layer(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.delete_gdal_layer(input_img)

    assert not os.path.exists(input_img)


def test_get_img_pxl_values():
    import rsgislib.imageutils
    import numpy

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    pxl_vals = rsgislib.imageutils.get_img_pxl_values(
        input_img, 1, numpy.array([20, 40]), numpy.array([10, 20])
    )

    assert pxl_vals[0] == 79.0 and pxl_vals[1] == 80.0


def test_set_img_pxl_values(tmp_path):
    import rsgislib.imageutils
    import numpy

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_pxl_values(
        input_img, 1, numpy.array([20, 40]), numpy.array([10, 20]), 1
    )
    pxl_vals = rsgislib.imageutils.get_img_pxl_values(
        input_img, 1, numpy.array([20, 40]), numpy.array([10, 20])
    )
    assert pxl_vals[0] == 1 and pxl_vals[1] == 1


def test_get_img_pxl_column():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    pxl_vals = rsgislib.imageutils.get_img_pxl_column(input_img, 20, 10)

    assert (
        pxl_vals[0] == 79.0
        and pxl_vals[1] == 83.0
        and pxl_vals[2] == 51.0
        and pxl_vals[3] == 48.0
        and pxl_vals[4] == 42.0
        and pxl_vals[5] == 40.0
        and pxl_vals[6] == 37.0
        and pxl_vals[7] == 37.0
        and pxl_vals[8] == 27.0
        and pxl_vals[9] == 26.0
    )


def test_assign_random_pxls(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.assign_random_pxls(
        input_img,
        output_img,
        n_pts=100,
        img_band=1,
        gdalformat="KEA",
        edge_pxl=0,
        use_no_data=True,
        rnd_seed=42,
    )

    assert os.path.exists(output_img)


def test_generate_random_pxl_vals_img(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.generate_random_pxl_vals_img(
        input_img, output_img, gdalformat="KEA", low_val=100, up_val=200
    )

    assert os.path.exists(output_img)


def test_gen_sampling_grid(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.gen_sampling_grid(
        input_img, output_img, "KEA", 10, 10, 100, True
    )

    assert os.path.exists(output_img)


def test_whiten_image(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.whiten_image(input_img, valid_msk_img, 1, output_img, "KEA")

    assert os.path.exists(output_img)


def test_spectral_smoothing(tmp_path):
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    valid_msk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.spectral_smoothing(input_img, valid_msk_img, 1, output_img)

    assert os.path.exists(output_img)


def test_set_img_band_metadata(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_band_metadata(input_img, 1, "Hello", "World")


def test_get_img_band_metadata():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imageutils.get_img_band_metadata(input_img, 1, "LAYER_TYPE")


def test_get_img_band_metadata_fields():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imageutils.get_img_band_metadata_fields(input_img, 1)


def test_get_img_band_metadata_fields_dict():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imageutils.get_img_band_metadata_fields_dict(input_img, 1)


def test_set_img_metadata(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_metadata(input_img, "Hello", "World")


def test_get_img_metadata(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.set_img_metadata(input_img, "Hello", "World")
    rsgislib.imageutils.get_img_metadata(input_img, "Hello")


def test_get_img_metadata_fields():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imageutils.get_img_metadata_fields(input_img)


def test_get_img_metadata_fields_dict():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    rsgislib.imageutils.get_img_metadata_fields_dict(input_img)


def test_get_img_pxl_coords():
    import rsgislib.imageutils
    import numpy

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    x_coords = numpy.array([260453.28, 260430.37])
    y_coords = numpy.array([281327.15, 279581.76])

    x_pxl_coords, y_pxl_coords = rsgislib.imageutils.get_img_pxl_coords(
        input_img, x_coords, y_coords
    )

    assert (
        (x_pxl_coords[0] == 286)
        and (y_pxl_coords[0] == 151)
        and (x_pxl_coords[1] == 284)
        and (y_pxl_coords[1] == 325)
    )
