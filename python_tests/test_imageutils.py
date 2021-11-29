import os
from shutil import copy2

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
IMGUTILS_DATA_DIR = os.path.join(DATA_DIR, "imageutils")


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

def test_get_img_bbox_in_proj_4326():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.get_img_bbox_in_proj(input_img, out_epsg=4326)
    print(bbox)
    assert (
            (abs(bbox[0] - -4.105) < 0.01)
            and (abs(bbox[1] - -3.964) < 0.01)
            and (abs(bbox[2] - 52.366) < 0.01)
            and (abs(bbox[3] - 52.453) < 0.01)
    )


def test_get_img_bbox_in_proj_utm30n():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")
    bbox = rsgislib.imageutils.get_img_bbox_in_proj(input_img, out_epsg=32630)
    print(bbox)
    assert (
            (abs(bbox[0] - 424918.88) < 0.01)
            and (abs(bbox[1] - 434357.09) < 0.01)
            and (abs(bbox[2] - 5802283.59) < 0.01)
            and (abs(bbox[3] - 5811879.561707063) < 0.01)
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

    rsgislib.imageutils.pop_img_stats(input_img, use_no_data=True, no_data_val=0, calc_pyramids=True)


def test_pop_thmt_img_stats(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset_vldmsk.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset_vldmsk.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.pop_thmt_img_stats(input_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True)



def test_get_img_band_stats():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")

    min, max, mean, std = rsgislib.imageutils.get_img_band_stats(input_img, img_band=1, compute=True)

    assert (min == 1.0) and (max == 1066.0) and (abs(mean - 63.13) < 0.01) and (abs(std - 32.44) < 0.01)

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
    assert "British National Grid" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)

def test_get_wkt_proj_from_img_utm():
    import rsgislib.imageutils

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)


def test_assign_proj_file(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")

    rsgislib.imageutils.assign_proj(input_img, None, wkt_file)

    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)

def test_assign_proj_str(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    wkt_str = """PROJCS["WGS 84 / UTM zone 30N",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",-3],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AXIS["Easting",EAST],AXIS["Northing",NORTH],AUTHORITY["EPSG","32630"]]"""

    rsgislib.imageutils.assign_proj(input_img, wkt_str, None)

    assert "UTM zone 30N" in rsgislib.imageutils.get_wkt_proj_from_img(input_img)

def test_assign_spatial_info(tmp_path):
    import rsgislib.imageutils

    input_ref_img = os.path.join(DATA_DIR, "sen2_20210527_aber_subset.kea")
    input_img = os.path.join(tmp_path, "sen2_20210527_aber_subset.kea")
    copy2(input_ref_img, input_img)

    rsgislib.imageutils.assign_spatial_info(input_img, None, None, 100, -100, None, None)
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

# TODO: Test for copy_gcps
#def test_copy_gcps(tmp_path):


def test_resample_img_to_match(tmp_path):
    import rsgislib.imageutils

    input_utm_img = os.path.join(DATA_DIR, "sen2_20210527_aber_utm30n.kea")
    input_osgb_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    output_img = os.path.join(tmp_path, "output_img.kea")
    rsgislib.imageutils.resample_img_to_match(input_utm_img, input_osgb_img, output_img, "KEA")

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
    rsgislib.imageutils.gdal_warp(input_img, output_img, 4326, gdalformat='KEA', use_multi_threaded=False)

    assert os.path.exists(output_img)












