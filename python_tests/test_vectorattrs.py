import os
import pytest
from shutil import copy2

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
VECTORATTRS_DATA_DIR = os.path.join(DATA_DIR, "vectorattrs")

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True


def test_read_vec_column_IntCol():
    import rsgislib.vectorattrs

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"
    vals = rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, "IntCol")
    ref_vals = [1, 2, 3, 4, 5, 6]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if val != ref_val:
            vals_eq = False
            break
    assert vals_eq


def test_read_vec_column_FloatCol():
    import rsgislib.vectorattrs

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"
    vals = rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, "FloatCol")
    ref_vals = [1.1, 2.2, 3.3, 4.4, 5.5, 6.6]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if val != ref_val:
            vals_eq = False
            break
    assert vals_eq


def test_read_vec_column_StrCol():
    import rsgislib.vectorattrs

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"
    vals = rsgislib.vectorattrs.read_vec_column(vec_file, vec_lyr, "StrCol")
    ref_vals = ["One", "Two", "Three", "Four", "Five", "Six"]
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if val != ref_val:
            vals_eq = False
            break
    assert vals_eq


def test_read_vec_columns():
    import rsgislib.vectorattrs

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"
    att_columns = ["IntCol", "FloatCol", "StrCol"]
    vals = rsgislib.vectorattrs.read_vec_columns(vec_file, vec_lyr, att_columns)

    ref_int_vals = [1, 2, 3, 4, 5, 6]
    ref_flt_vals = [1.1, 2.2, 3.3, 4.4, 5.5, 6.6]
    ref_str_vals = ["One", "Two", "Three", "Four", "Five", "Six"]
    vals_eq = True
    for i in range(len(vals)):
        if (
            (vals[i]["IntCol"] != ref_int_vals[i])
            and (vals[i]["FloatCol"] != ref_flt_vals[i])
            and (vals[i]["StrCol"] != ref_str_vals[i])
        ):
            vals_eq = False
            break

    assert vals_eq


def test_write_vec_column_Int(tmp_path):
    import rsgislib.vectorutils
    import rsgislib.vectorattrs
    import osgeo.ogr as ogr

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    ref_vals = [1, 2, 3, 4, 5, 6]
    rsgislib.vectorattrs.write_vec_column(
        out_vec_file, out_vec_lyr, "TestCol", ogr.OFTInteger, ref_vals
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "TestCol")
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if val != ref_val:
            vals_eq = False
            break
    assert vals_eq


def test_write_vec_column_Float(tmp_path):
    import rsgislib.vectorutils
    import rsgislib.vectorattrs
    import osgeo.ogr as ogr

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    ref_vals = [1.1, 2.2, 3.3, 4.4, 5.5, 6.6]
    rsgislib.vectorattrs.write_vec_column(
        out_vec_file, out_vec_lyr, "TestCol", ogr.OFTReal, ref_vals
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "TestCol")
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if val != ref_val:
            vals_eq = False
            break
    assert vals_eq


def test_write_vec_column_String(tmp_path):
    import rsgislib.vectorutils
    import rsgislib.vectorattrs
    import osgeo.ogr as ogr

    vec_file = os.path.join(VECTORATTRS_DATA_DIR, "sen2_20210527_aber_att_vals.geojson")
    vec_lyr = "sen2_20210527_aber_att_vals"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorutils.create_copy_vector_lyr(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        "GEOJSON",
        replace=True,
        in_memory=True,
    )

    ref_vals = ["One", "Two", "Three", "Four", "Five", "Six"]
    rsgislib.vectorattrs.write_vec_column(
        out_vec_file, out_vec_lyr, "TestCol", ogr.OFTString, ref_vals
    )

    vals = rsgislib.vectorattrs.read_vec_column(out_vec_file, out_vec_lyr, "TestCol")
    vals_eq = True
    for val, ref_val in zip(vals, ref_vals):
        if val != ref_val:
            vals_eq = False
            break
    assert vals_eq


def test_pop_bbox_cols(tmp_path):
    import rsgislib.vectorattrs

    vec_in_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_file = os.path.join(tmp_path, "aber_osgb_multi_polys.geojson")
    copy2(vec_in_file, vec_file)

    vec_lyr = "aber_osgb_multi_polys"
    rsgislib.vectorattrs.pop_bbox_cols(vec_file, vec_lyr)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_create_name_col(tmp_path):
    import rsgislib.vectorattrs

    vec_in_file = os.path.join(DATA_DIR, "degree_grid_examples.geojson")
    vec_file = os.path.join(tmp_path, "degree_grid_examples.geojson")
    copy2(vec_in_file, vec_file)
    vec_lyr = "degree_grid_examples"
    rsgislib.vectorattrs.pop_bbox_cols(vec_file, vec_lyr)

    out_vec_file = os.path.join(tmp_path, "degree_grid_named.geojson")
    out_vec_lyr = "degree_grid_named"
    rsgislib.vectorattrs.create_name_col(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        out_col="names",
        x_col="xmin",
        y_col="ymax",
        prefix="hello",
        postfix="world",
        coords_lat_lon=True,
        int_coords=True,
        zero_x_pad=3,
        zero_y_pad=2,
        round_n_digts=3,
        non_neg=True,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_add_unq_numeric_col(tmp_path):
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorattrs.add_unq_numeric_col(
        vec_file,
        vec_lyr,
        "names",
        "unq_id",
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_add_numeric_col_lut(tmp_path):
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    val_lut = dict()
    val_lut["helloN63W126world"] = 10
    val_lut["helloN03E134world"] = 20
    val_lut["helloN02W071world"] = 30
    val_lut["helloS51E056world"] = 40
    val_lut["helloS50E049world"] = 50

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorattrs.add_numeric_col_lut(
        vec_file,
        vec_lyr,
        "names",
        val_lut,
        "int_id",
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_add_numeric_col_float(tmp_path):
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorattrs.add_numeric_col(
        vec_file,
        vec_lyr,
        "new_col",
        out_vec_file,
        out_vec_lyr,
        out_val=101,
        out_format="GeoJSON",
        out_col_int=False,
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_add_numeric_col_float(tmp_path):
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorattrs.add_numeric_col(
        vec_file,
        vec_lyr,
        "new_col",
        out_vec_file,
        out_vec_lyr,
        out_val=101,
        out_format="GeoJSON",
        out_col_int=True,
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_add_string_col(tmp_path):
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorattrs.add_string_col(
        vec_file,
        vec_lyr,
        "new_col",
        out_vec_file,
        out_vec_lyr,
        out_val="Hello World",
        out_format="GeoJSON",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_get_unq_col_values():
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    rsgislib.vectorattrs.get_unq_col_values(vec_file, vec_lyr, col_name="names")


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_add_fid_col(tmp_path):
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    out_vec_file = os.path.join(tmp_path, "out_vec.geojson")
    out_vec_lyr = "out_vec"
    rsgislib.vectorattrs.add_fid_col(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GeoJSON",
        out_col="new_fid",
    )
    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_get_vec_cols_as_array():
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    cols = ["xmin", "xmax", "ymin", "ymax"]
    rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols, lower_limit=None, upper_limit=None
    )


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_get_vec_cols_as_array_lower_limit():
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    cols = ["xmin", "xmax", "ymin", "ymax"]
    rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols, lower_limit=50, upper_limit=None
    )


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_get_vec_cols_as_array_upper_limit():
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    cols = ["xmin", "xmax", "ymin", "ymax"]
    rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols, lower_limit=None, upper_limit=100
    )


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_get_vec_cols_as_array_limits():
    import rsgislib.vectorattrs

    vec_file = os.path.join(DATA_DIR, "degree_grid_named.geojson")
    vec_lyr = "degree_grid_named"

    cols = ["xmin", "xmax", "ymin", "ymax"]
    rsgislib.vectorattrs.get_vec_cols_as_array(
        vec_file, vec_lyr, cols, lower_limit=50, upper_limit=100
    )
