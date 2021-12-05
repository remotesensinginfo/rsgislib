import os
from shutil import copy2

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True


DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
VECTORUTILS_DATA_DIR = os.path.join(DATA_DIR, "vectorutils")


def test_convert_polygon_to_polyline(tmp_path):
    import rsgislib.vectorgeoms

    vec_poly_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_poly_lyr = "aber_osgb_multi_polys"

    vec_line_file = os.path.join(tmp_path, "out_vec.gpkg")
    vec_line_lyr = "out_vec"

    rsgislib.vectorgeoms.convert_polygon_to_polyline(
        vec_poly_file,
        vec_poly_lyr,
        vec_line_file,
        vec_line_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(vec_line_file)


def test_get_pt_on_line():
    import rsgislib.vectorgeoms
    import osgeo.ogr as ogr

    pt1 = ogr.Geometry(ogr.wkbPoint)
    pt1.AddPoint(200, 500)
    pt2 = ogr.Geometry(ogr.wkbPoint)
    pt2.AddPoint(1200, 1500)

    new_pt_x, new_pt_y = rsgislib.vectorgeoms.get_pt_on_line(pt1, pt2, 250)

    assert (abs(new_pt_x - 376.7767) < 1) and (abs(new_pt_y - 676.7767) < 1)


def test_find_pt_to_side_right():
    import rsgislib.vectorgeoms
    import osgeo.ogr as ogr

    pt_start = ogr.Geometry(ogr.wkbPoint)
    pt_start.AddPoint(200, 500)

    pt = ogr.Geometry(ogr.wkbPoint)
    pt.AddPoint(376.7767, 676.7767)

    pt_end = ogr.Geometry(ogr.wkbPoint)
    pt_end.AddPoint(1200, 1500)

    new_pt_x, new_pt_y = rsgislib.vectorgeoms.find_pt_to_side(
        pt_start, pt, pt_end, 250, left_hand=False
    )

    assert (abs(new_pt_x - 553.5534) < 1) and (abs(new_pt_y - 500.000) < 1)


def test_find_pt_to_side_left():
    import rsgislib.vectorgeoms
    import osgeo.ogr as ogr

    pt_start = ogr.Geometry(ogr.wkbPoint)
    pt_start.AddPoint(200, 500)

    pt = ogr.Geometry(ogr.wkbPoint)
    pt.AddPoint(376.7767, 676.7767)

    pt_end = ogr.Geometry(ogr.wkbPoint)
    pt_end.AddPoint(1200, 1500)

    new_pt_x, new_pt_y = rsgislib.vectorgeoms.find_pt_to_side(
        pt_start, pt, pt_end, 250, left_hand=True
    )

    assert (abs(new_pt_x - 200.0000) < 1) and (abs(new_pt_y - 853.5534) < 1)


def test_create_orthg_lines_right(tmp_path):
    import rsgislib.vectorgeoms

    vec_poly_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_poly_lyr = "aber_osgb_multi_polys"

    vec_line_file = os.path.join(tmp_path, "lines_vec.gpkg")
    vec_line_lyr = "lines_vec"

    rsgislib.vectorgeoms.convert_polygon_to_polyline(
        vec_poly_file,
        vec_poly_lyr,
        vec_line_file,
        vec_line_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.create_orthg_lines(
        vec_line_file,
        vec_line_lyr,
        out_vec_file,
        out_vec_lyr,
        pt_step=1000,
        line_len=10000,
        left_hand=False,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_create_orthg_lines_left(tmp_path):
    import rsgislib.vectorgeoms

    vec_poly_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_poly_lyr = "aber_osgb_multi_polys"

    vec_line_file = os.path.join(tmp_path, "lines_vec.gpkg")
    vec_line_lyr = "lines_vec"

    rsgislib.vectorgeoms.convert_polygon_to_polyline(
        vec_poly_file,
        vec_poly_lyr,
        vec_line_file,
        vec_line_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.create_orthg_lines(
        vec_line_file,
        vec_line_lyr,
        out_vec_file,
        out_vec_lyr,
        pt_step=1000,
        line_len=10000,
        left_hand=True,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)
