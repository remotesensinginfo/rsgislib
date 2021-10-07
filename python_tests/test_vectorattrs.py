import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
VECTORATTRS_DATA_DIR = os.path.join(DATA_DIR, "vectorattrs")


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
    rsgislib.vectorutils.createCopyVectorLyr(
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
    rsgislib.vectorutils.createCopyVectorLyr(
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
    rsgislib.vectorutils.createCopyVectorLyr(
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
