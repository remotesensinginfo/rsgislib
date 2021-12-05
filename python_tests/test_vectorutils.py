import os
import pytest
from shutil import copy2

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

RTREE_NOT_AVAIL = False
try:
    import rtree
except ImportError:
    RTREE_NOT_AVAIL = True

SHAPELY_NOT_AVAIL = False
try:
    import shapely
except ImportError:
    SHAPELY_NOT_AVAIL = True

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
VECTORUTILS_DATA_DIR = os.path.join(DATA_DIR, "vectorutils")
CLASSIFICATION_DATA_DIR = os.path.join(DATA_DIR, "classification")
IMGUTILS_DATA_DIR = os.path.join(DATA_DIR, "imageutils")
REGRESS_DATA_DIR = os.path.join(DATA_DIR, "regression")


def test_delete_vector_file(tmp_path):
    import rsgislib.vectorutils
    import shutil

    ref_vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    cp_vec_file = os.path.join(tmp_path, "./cls_forest_smpls.gpkg")
    shutil.copyfile(ref_vec_file, cp_vec_file)
    if not os.path.exists(cp_vec_file):
        raise Exception("Test file is not present")
    rsgislib.vectorutils.delete_vector_file(cp_vec_file)

    assert not os.path.exists(cp_vec_file)


def test_check_format_name_geojson1():
    import rsgislib.vectorutils

    assert "GeoJSON" == rsgislib.vectorutils.check_format_name("GEOJSON")


def test_check_format_name_geojson2():
    import rsgislib.vectorutils

    assert "GeoJSON" == rsgislib.vectorutils.check_format_name("geojson")


def test_check_format_name_geojson3():
    import rsgislib.vectorutils

    assert "GeoJSON" == rsgislib.vectorutils.check_format_name("GeoJSON")


def test_check_format_name_gpkg():
    import rsgislib.vectorutils

    assert "GPKG" == rsgislib.vectorutils.check_format_name("GPKG")


def test_check_format_name_shp():
    import rsgislib.vectorutils

    assert "ESRI Shapefile" == rsgislib.vectorutils.check_format_name("ESRI Shapefile")


def test_get_proj_wkt_from_vec_osgb():
    import rsgislib.vectorutils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"
    wkt_str = rsgislib.vectorutils.get_proj_wkt_from_vec(vec_file, vec_lyr)
    assert "British National Grid" in wkt_str


def test_get_proj_epsg_from_vec_osgb():
    import rsgislib.vectorutils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"
    epsg_code = rsgislib.vectorutils.get_proj_epsg_from_vec(vec_file, vec_lyr)
    assert epsg_code == 27700


def test_get_vec_feat_count():
    import rsgislib.vectorutils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"
    n_feats = rsgislib.vectorutils.get_vec_feat_count(vec_file, vec_lyr)
    assert n_feats == 9


def test_merge_vectors_to_gpkg(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    in_vec_files = [vec_file_1, vec_file_2, vec_file_3, vec_file_4]

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.merge_vectors_to_gpkg(in_vec_files, out_vec_file, out_vec_lyr)
    assert os.path.exists(out_vec_file)


def test_merge_vector_lyrs_to_gpkg(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(CLASSIFICATION_DATA_DIR, "cls_poly_smpls.gpkg")

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.merge_vector_lyrs_to_gpkg(vec_file, out_vec_file, out_vec_lyr)
    assert os.path.exists(out_vec_file)


def test_merge_vectors_to_gpkg_ind_lyrs(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    in_vec_files = [vec_file_1, vec_file_2, vec_file_3, vec_file_4]

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    rsgislib.vectorutils.merge_vectors_to_gpkg_ind_lyrs(in_vec_files, out_vec_file)

    assert os.path.exists(out_vec_file)


def test_merge_to_multi_layer_vec(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    input_file_lyrs = list()
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_1, "cls_forest_smpls", "cls_forest_smpls"
        )
    )
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_2, "cls_grass_smpls", "cls_grass_smpls"
        )
    )
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_3, "cls_urban_smpls", "cls_urban_smpls"
        )
    )
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_4, "cls_water_smpls", "cls_water_smpls"
        )
    )

    out_vec_file = os.path.join(tmp_path, "./cls_poly_smpls.gpkg")
    rsgislib.vectorutils.merge_to_multi_layer_vec(
        input_file_lyrs, out_vec_file, out_format="GPKG", overwrite=False
    )

    assert os.path.exists(out_vec_file)


def test_merge_to_multi_layer_vec_exists(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    input_file_lyrs = list()
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_1, "cls_forest_smpls", "cls_forest_smpls"
        )
    )
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_2, "cls_grass_smpls", "cls_grass_smpls"
        )
    )
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_3, "cls_urban_smpls", "cls_urban_smpls"
        )
    )
    input_file_lyrs.append(
        rsgislib.vectorutils.VecLayersInfoObj(
            vec_file_4, "cls_water_smpls", "cls_water_smpls"
        )
    )

    out_vec_file = os.path.join(tmp_path, "./cls_poly_smpls.gpkg")
    rsgislib.vectorutils.merge_to_multi_layer_vec(
        input_file_lyrs, out_vec_file, out_format="GPKG", overwrite=False
    )
    # Run it again so that file exists.
    rsgislib.vectorutils.merge_to_multi_layer_vec(
        input_file_lyrs, out_vec_file, out_format="GPKG", overwrite=True
    )

    assert os.path.exists(out_vec_file)


def test_get_vec_lyrs_lst():
    import rsgislib.vectorutils

    vec_file = os.path.join(CLASSIFICATION_DATA_DIR, "cls_poly_smpls.gpkg")
    lyrs = rsgislib.vectorutils.get_vec_lyrs_lst(vec_file)
    assert len(lyrs) == 4


def test_get_vec_layer_extent():
    import rsgislib.vectorutils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"
    bbox = rsgislib.vectorutils.get_vec_layer_extent(vec_file, vec_lyr)
    assert (
        (abs(bbox[0] - 259022.6875) < 1)
        and (abs(bbox[1] - 266288.4375) < 1)
        and (abs(bbox[2] - 277206.96875) < 1)
        and (abs(bbox[3] - 282311.96875) < 1)
    )


def test_split_vec_lyr(tmp_path):
    import rsgislib.vectorutils
    import glob

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"

    rsgislib.vectorutils.split_vec_lyr(
        vec_file, vec_lyr, 2, "GPKG", tmp_path, "out_vec", "gpkg"
    )

    out_files = glob.glob(os.path.join(tmp_path, "out_vec*.gpkg"))
    assert len(out_files) == 5


def test_reproj_vector_layer(tmp_path):
    import rsgislib.vectorutils
    import rsgislib.tools.utils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    wkt_file = os.path.join(IMGUTILS_DATA_DIR, "utm30n.wkt")
    out_proj_wkt = rsgislib.tools.utils.read_text_file_no_new_lines(wkt_file)

    rsgislib.vectorutils.reproj_vector_layer(
        vec_file,
        out_vec_file,
        out_proj_wkt,
        out_format="GPKG",
        out_vec_lyr=out_vec_lyr,
        vec_lyr=vec_lyr,
        in_proj_wkt=None,
        del_exist_vec=False,
    )
    assert os.path.exists(out_vec_file)


def test_reproj_vec_lyr_obj_4326(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    rsgislib.vectorutils.reproj_vec_lyr_obj(
        vec_lyr_obj, out_vec_file, 4326, out_format="GPKG", out_vec_lyr=out_vec_lyr
    )
    vec_obj_ds = None
    assert os.path.exists(out_vec_file)


def test_reproj_vec_lyr_obj_32630(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_lyr = "cls_forest_smpls"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    rsgislib.vectorutils.reproj_vec_lyr_obj(
        vec_lyr_obj, out_vec_file, 32630, out_format="GPKG", out_vec_lyr=out_vec_lyr
    )
    vec_obj_ds = None
    assert os.path.exists(out_vec_file)


def test_get_att_lst_select_feats():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_sel_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_sel_lyr = "cls_grass_smpls"

    att_names = ["re_b4", "re_b5"]

    out_vals = rsgislib.vectorutils.get_att_lst_select_feats(
        vec_file, vec_lyr, att_names, vec_sel_file, vec_sel_lyr
    )

    assert len(out_vals) == 3


def test_get_att_lst_select_feats_lyr_objs():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"
    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    vec_sel_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_sel_lyr = "cls_grass_smpls"
    vec_sel_obj_ds, vec_lyr_sel_obj = rsgislib.vectorutils.open_gdal_vec_lyr(
        vec_sel_file, vec_sel_lyr
    )

    att_names = ["re_b4", "re_b5"]
    out_vals = rsgislib.vectorutils.get_att_lst_select_feats_lyr_objs(
        vec_lyr_obj, att_names, vec_lyr_sel_obj
    )
    vec_obj_ds = None
    vec_sel_obj_ds = None

    assert len(out_vals) == 3


def test_get_att_lst_select_bbox_feats():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    bbox = [262987, 264883, 280213, 282551]

    att_names = ["re_b4", "re_b5"]
    out_vals = rsgislib.vectorutils.get_att_lst_select_bbox_feats(
        vec_file, vec_lyr, att_names, bbox, 27700
    )

    assert len(out_vals) == 7


def test_get_att_lst_select_bbox_feats_lyr_objs():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"
    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    bbox = [262987, 264883, 280213, 282551]

    att_names = ["re_b4", "re_b5"]
    out_vals = rsgislib.vectorutils.get_att_lst_select_bbox_feats_lyr_objs(
        vec_lyr_obj, att_names, bbox, 27700
    )
    vec_obj_ds = None
    vec_sel_obj_ds = None

    assert len(out_vals) == 7


def test_select_intersect_feats(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_roi_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_roi_lyr = "cls_grass_smpls"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.select_intersect_feats(
        vec_file,
        vec_lyr,
        vec_roi_file,
        vec_roi_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
    )

    assert os.path.exists(out_vec_file)


def test_export_spatial_select_feats(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_roi_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_roi_lyr = "cls_grass_smpls"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.export_spatial_select_feats(
        vec_file,
        vec_lyr,
        vec_roi_file,
        vec_roi_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
    )

    assert os.path.exists(out_vec_file)


def test_get_vec_lyr_cols():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    cols = rsgislib.vectorutils.get_vec_lyr_cols(vec_file, vec_lyr)

    assert len(cols) == 6


def test_subset_envs_vec_lyr_obj():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"
    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)

    bbox = [262987, 264883, 280213, 282551]

    vec_rsl_obj_ds, vec_lyr_rsl_obj = rsgislib.vectorutils.subset_envs_vec_lyr_obj(
        vec_lyr_obj, bbox, epsg=27700
    )
    vec_rsl_obj_ds = None


def test_subset_veclyr_to_featboxs(tmp_path):
    import rsgislib.vectorutils
    import glob

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_roi_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_roi_lyr = "cls_grass_smpls"

    out_file_base = os.path.join(tmp_path, "out_vec")

    rsgislib.vectorutils.subset_veclyr_to_featboxs(
        vec_roi_file,
        vec_roi_lyr,
        vec_file,
        vec_lyr,
        "sub_grasslands",
        out_file_base,
        out_file_end="gpkg",
        out_format="GPKG",
    )

    out_vecs = glob.glob(os.path.join(tmp_path, "out_vec*.gpkg"))

    assert len(out_vecs) == 11


def test_read_vec_lyr_to_mem():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.read_vec_lyr_to_mem(
        vec_file, vec_lyr
    )
    vec_obj_ds = None


def test_open_gdal_vec_lyr():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.open_gdal_vec_lyr(vec_file, vec_lyr)
    vec_obj_ds = None


def test_get_mem_vec_lyr_subset():
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    bbox = [262987, 264883, 280213, 282551]

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.get_mem_vec_lyr_subset(
        vec_file, vec_lyr, bbox
    )
    vec_obj_ds = None


def test_write_vec_lyr_to_file(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    bbox = [262987, 264883, 280213, 282551]

    vec_obj_ds, vec_lyr_obj = rsgislib.vectorutils.get_mem_vec_lyr_subset(
        vec_file, vec_lyr, bbox
    )

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.write_vec_lyr_to_file(
        vec_lyr_obj, out_vec_file, out_vec_lyr, out_format="GPKG"
    )
    vec_obj_ds = None

    assert os.path.exists(out_vec_file)


def test_create_copy_vector_lyr(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_format="GPKG"
    )

    assert os.path.exists(out_vec_file)


def test_get_ogr_vec_col_datatype_from_gdal_rat_col_datatype_int():
    import rsgislib.vectorutils
    import osgeo.gdal as gdal
    import osgeo.ogr as ogr

    assert (
        rsgislib.vectorutils.get_ogr_vec_col_datatype_from_gdal_rat_col_datatype(
            gdal.GFT_Integer
        )
        == ogr.OFTInteger
    )


def test_get_ogr_vec_col_datatype_from_gdal_rat_col_datatype_flt():
    import rsgislib.vectorutils
    import osgeo.gdal as gdal
    import osgeo.ogr as ogr

    assert (
        rsgislib.vectorutils.get_ogr_vec_col_datatype_from_gdal_rat_col_datatype(
            gdal.GFT_Real
        )
        == ogr.OFTReal
    )


def test_get_ogr_vec_col_datatype_from_gdal_rat_col_datatype_str():
    import rsgislib.vectorutils
    import osgeo.gdal as gdal
    import osgeo.ogr as ogr

    assert (
        rsgislib.vectorutils.get_ogr_vec_col_datatype_from_gdal_rat_col_datatype(
            gdal.GFT_String
        )
        == ogr.OFTString
    )


def test_copy_rat_cols_to_vector_lyr(tmp_path):
    import rsgislib.vectorutils
    import rsgislib.vectorutils.createrasters
    import rsgislib.rastergis

    input_img = os.path.join(DATA_DIR, "sen2_20210527_aber.kea")

    vec_ref_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_file = os.path.join(tmp_path, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    copy2(vec_ref_file, vec_file)

    clumps_img = os.path.join(tmp_path, "clumps_img.kea")

    rsgislib.vectorutils.createrasters.rasterise_vec_lyr(
        vec_file, vec_lyr, input_img, clumps_img, gdalformat="KEA", att_column="val"
    )

    bs = []
    bs.append(
        rsgislib.rastergis.BandAttStats(
            band=1, min_field="b1Min", max_field="b1Max", mean_field="b1Mean"
        )
    )
    bs.append(
        rsgislib.rastergis.BandAttStats(
            band=2, min_field="b2Min", max_field="b2Max", mean_field="b2Mean"
        )
    )
    rsgislib.rastergis.populate_rat_with_stats(input_img, clumps_img, bs)

    ratcols = ["b1Min", "b1Max", "b1Mean", "b2Min", "b2Max", "b2Mean"]
    out_col_names = ["b1Min", "b1Max", "b1Mean", "b2Min", "b2Max", "b2Mean"]

    rsgislib.vectorutils.copy_rat_cols_to_vector_lyr(
        vec_file, vec_lyr, "val", clumps_img, ratcols, out_col_names, out_col_types=None
    )


# TODO rsgislib.vectorutils.vector_maths
"""
def test_vector_maths(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    vars = list()
    vars.append(rsgislib.vectorutils.VecColVar(name="val", field_name="val"))

    rsgislib.vectorutils.vector_maths(vec_file, vec_lyr, out_vec_file, out_vec_lyr, "GPKG", "out_vals", "val * val", vars, True)

    assert os.path.exists(out_vec_file)
"""
# TODO rsgislib.vectorutils.create_lines_of_points
# TODO rsgislib.vectorutils.check_validate_geometries


@pytest.mark.skipif(
    (GEOPANDAS_NOT_AVAIL and RTREE_NOT_AVAIL),
    reason="geopandas or rtree dependencies not available",
)
def test_perform_spatial_join_empty(tmp_path):
    import rsgislib.vectorutils

    vec_base_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_base_lyr = "aber_osgb_multi_polys"

    vec_join_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_join_lyr = "sample_pts"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.perform_spatial_join(
        vec_base_file,
        vec_base_lyr,
        vec_join_file,
        vec_join_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        join_how="inner",
        join_op="within",
    )

    assert not os.path.exists(out_vec_file)


@pytest.mark.skipif(
    (GEOPANDAS_NOT_AVAIL and RTREE_NOT_AVAIL),
    reason="geopandas or rtree dependencies not available",
)
def test_perform_spatial_join(tmp_path):
    import rsgislib.vectorutils

    vec_join_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_join_lyr = "cls_grass_smpls"

    vec_base_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_base_lyr = "sample_pts"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.perform_spatial_join(
        vec_base_file,
        vec_base_lyr,
        vec_join_file,
        vec_join_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        join_how="inner",
        join_op="within",
    )

    assert os.path.exists(out_vec_file)


def test_does_vmsk_img_intersect(tmp_path):
    import rsgislib.vectorutils

    input_vmsk_img = os.path.join(DATA_DIR, "sen2_20210527_aber_vldmsk.kea")
    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    intersect = rsgislib.vectorutils.does_vmsk_img_intersect(
        input_vmsk_img, vec_roi_file, vec_roi_lyr, tmp_dir=tmp_path
    )
    assert intersect


def test_vector_translate(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.vector_translate(
        vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_format="GPKG"
    )

    assert os.path.exists(out_vec_file)


def test_reproj_wgs84_vec_to_utm(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(DATA_DIR, "degree_grid_examples_uk.geojson")
    vec_lyr = "degree_grid_examples_uk"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.reproj_wgs84_vec_to_utm(
        vec_file, vec_lyr, out_vec_file, out_vec_lyr, use_hemi=True, out_format="GPKG"
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_spatial_select(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    vec_roi_file = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_roi_lyr = "cls_grass_smpls"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.spatial_select(
        vec_file,
        vec_lyr,
        vec_roi_file,
        vec_roi_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_split_by_attribute_sgl_file(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(DATA_DIR, "degree_grid_named_subset.geojson")
    vec_lyr = "degree_grid_named_subset"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    split_col_name = "names"

    rsgislib.vectorutils.split_by_attribute(
        vec_file,
        vec_lyr,
        split_col_name,
        multi_layers=True,
        out_vec_file=out_vec_file,
        out_file_path=None,
        out_file_ext="gpkg",
        out_format="GPKG",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_split_by_attribute_mul_files(tmp_path):
    import rsgislib.vectorutils
    import glob

    vec_file = os.path.join(DATA_DIR, "degree_grid_named_subset.geojson")
    vec_lyr = "degree_grid_named_subset"

    split_col_name = "names"

    rsgislib.vectorutils.split_by_attribute(
        vec_file,
        vec_lyr,
        split_col_name,
        multi_layers=False,
        out_vec_file=None,
        out_file_path=tmp_path,
        out_file_ext="gpkg",
        out_format="GPKG",
    )
    assert len(glob.glob(os.path.join(tmp_path, "*.gpkg"))) == 13


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_subset_by_attribute_equals(tmp_path):
    import rsgislib.vectorutils
    import glob

    vec_file = os.path.join(DATA_DIR, "degree_grid_named_subset.geojson")
    vec_lyr = "degree_grid_named_subset"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    sub_col = "names"
    sub_vals = ["helloN63W125world", "helloN63W123world", "helloS52E053world"]
    rsgislib.vectorutils.subset_by_attribute(
        vec_file,
        vec_lyr,
        sub_col,
        sub_vals,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        match_type="equals",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_subset_by_attribute_contains(tmp_path):
    import rsgislib.vectorutils
    import glob

    vec_file = os.path.join(DATA_DIR, "degree_grid_named_subset.geojson")
    vec_lyr = "degree_grid_named_subset"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    sub_col = "names"
    sub_vals = ["N63"]
    rsgislib.vectorutils.subset_by_attribute(
        vec_file,
        vec_lyr,
        sub_col,
        sub_vals,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        match_type="contains",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_subset_by_attribute_start(tmp_path):
    import rsgislib.vectorutils
    import glob

    vec_file = os.path.join(DATA_DIR, "degree_grid_named_subset.geojson")
    vec_lyr = "degree_grid_named_subset"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    sub_col = "names"
    sub_vals = ["hello"]
    rsgislib.vectorutils.subset_by_attribute(
        vec_file,
        vec_lyr,
        sub_col,
        sub_vals,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        match_type="start",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_merge_vector_files(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    in_vec_files = [vec_file_1, vec_file_2, vec_file_3, vec_file_4]

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.merge_vector_files(
        in_vec_files, out_vec_file, out_vec_lyr, out_format="GPKG"
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_merge_vector_layers(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_forest_smpls.gpkg")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_grass_smpls.gpkg")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_urban_smpls.gpkg")
    vec_file_4 = os.path.join(VECTORUTILS_DATA_DIR, "./cls_water_smpls.gpkg")

    in_vec_files = list()
    in_vec_files.append({"file": vec_file_1, "layer": "cls_forest_smpls"})
    in_vec_files.append({"file": vec_file_2, "layer": "cls_grass_smpls"})
    in_vec_files.append({"file": vec_file_3, "layer": "cls_urban_smpls"})
    in_vec_files.append({"file": vec_file_4, "layer": "cls_water_smpls"})

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.merge_vector_layers(
        in_vec_files, out_vec_file, out_vec_lyr, out_format="GPKG"
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(
    (GEOPANDAS_NOT_AVAIL and SHAPELY_NOT_AVAIL),
    reason="geopandas or shapely dependencies not available",
)
def test_geopd_check_polys_wgs84_bounds_geometry():
    import rsgislib.vectorutils
    import geopandas

    vec_file = os.path.join(VECTORUTILS_DATA_DIR, "fiji_wgs84_poly.geojson")
    vec_lyr = "fiji_wgs84_poly"

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    gdf_out = rsgislib.vectorutils.geopd_check_polys_wgs84_bounds_geometry(data_gdf)
    assert len(gdf_out) > 0


@pytest.mark.skipif(
    (GEOPANDAS_NOT_AVAIL and SHAPELY_NOT_AVAIL),
    reason="geopandas or shapely dependencies not available",
)
def test_merge_utm_vecs_wgs84(tmp_path):
    import rsgislib.vectorutils

    vec_file_1 = os.path.join(VECTORUTILS_DATA_DIR, "./utm_1n_polys.geojson")
    vec_file_2 = os.path.join(VECTORUTILS_DATA_DIR, "./utm_30n_polys.geojson")
    vec_file_3 = os.path.join(VECTORUTILS_DATA_DIR, "./utm_60s_polys.geojson")
    in_vec_files = [vec_file_1, vec_file_2, vec_file_3]

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorutils.merge_utm_vecs_wgs84(
        in_vec_files, out_vec_file, out_vec_lyr, out_format="GPKG"
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_split_feats_to_mlyrs(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(DATA_DIR, "degree_grid_examples_uk.geojson")
    vec_lyr = "degree_grid_examples_uk"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")

    rsgislib.vectorutils.split_feats_to_mlyrs(
        vec_file, vec_lyr, out_vec_file, out_format="GPKG"
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_split_vec_lyr_random_subset(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    out_rmain_vec_file = os.path.join(tmp_path, "remain_out_vec.gpkg")
    out_rmain_vec_lyr = "remain_out_vec"

    out_smpl_vec_file = os.path.join(tmp_path, "sample_out_vec.gpkg")
    out_smpl_vec_lyr = "sample_out_vec"

    rsgislib.vectorutils.split_vec_lyr_random_subset(
        vec_file,
        vec_lyr,
        out_rmain_vec_file,
        out_rmain_vec_lyr,
        out_smpl_vec_file,
        out_smpl_vec_lyr,
        n_smpl=10,
        out_format="GPKG",
        rnd_seed=42,
    )

    assert os.path.exists(out_rmain_vec_file) and os.path.exists(out_smpl_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_create_train_test_smpls(tmp_path):
    import rsgislib.vectorutils

    vec_file = os.path.join(REGRESS_DATA_DIR, "sample_pts.geojson")
    vec_lyr = "sample_pts"

    out_train_vec_file = os.path.join(tmp_path, "train_out_vec.gpkg")
    out_train_vec_lyr = "train_out_vec"

    out_test_vec_file = os.path.join(tmp_path, "test_out_vec.gpkg")
    out_test_vec_lyr = "sample_out_vec"

    rsgislib.vectorutils.create_train_test_smpls(
        vec_file,
        vec_lyr,
        out_train_vec_file,
        out_train_vec_lyr,
        out_test_vec_file,
        out_test_vec_lyr,
        out_format="GPKG",
        prop_test=0.2,
        tmp_dir=tmp_path,
        rnd_seed=42,
    )
    assert os.path.exists(out_train_vec_file) and os.path.exists(out_test_vec_file)
