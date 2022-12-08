import os
import pytest
from shutil import copy2

GEOPANDAS_NOT_AVAIL = False
try:
    import geopandas
except ImportError:
    GEOPANDAS_NOT_AVAIL = True

ALPHASHAPE_NOT_AVAIL = False
try:
    import alphashape
except ImportError:
    ALPHASHAPE_NOT_AVAIL = True

RTREE_NOT_AVAIL = False
try:
    import rtree
except ImportError:
    RTREE_NOT_AVAIL = True


DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
VECTORUTILS_DATA_DIR = os.path.join(DATA_DIR, "vectorutils")
VECTORGEOMS_DATA_DIR = os.path.join(DATA_DIR, "vectorgeoms")


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


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_convert_polys_to_lines_gp(tmp_path):
    import rsgislib.vectorgeoms

    vec_poly_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_poly_lyr = "welsh_land_area"

    vec_line_file = os.path.join(tmp_path, "out_vec.gpkg")
    vec_line_lyr = "out_vec"

    rsgislib.vectorgeoms.convert_polys_to_lines_gp(
        vec_poly_file,
        vec_poly_lyr,
        vec_line_file,
        vec_line_lyr,
        out_format="GPKG",
        del_exist_vec=False,
        exterior_lines=False,
    )

    assert os.path.exists(vec_line_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_convert_polys_to_lines_gp_exterior(tmp_path):
    import rsgislib.vectorgeoms

    vec_poly_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_poly_lyr = "welsh_land_area"

    vec_line_file = os.path.join(tmp_path, "out_vec.gpkg")
    vec_line_lyr = "out_vec"

    rsgislib.vectorgeoms.convert_polys_to_lines_gp(
        vec_poly_file,
        vec_poly_lyr,
        vec_line_file,
        vec_line_lyr,
        out_format="GPKG",
        del_exist_vec=False,
        exterior_lines=True,
    )

    assert os.path.exists(vec_line_file)


def test_get_pt_on_line():
    import rsgislib.vectorgeoms
    from osgeo import ogr

    pt1 = ogr.Geometry(ogr.wkbPoint)
    pt1.AddPoint(200, 500)
    pt2 = ogr.Geometry(ogr.wkbPoint)
    pt2.AddPoint(1200, 1500)

    new_pt_x, new_pt_y = rsgislib.vectorgeoms.get_pt_on_line(pt1, pt2, 250)

    assert (abs(new_pt_x - 376.7767) < 1) and (abs(new_pt_y - 676.7767) < 1)


def test_find_pt_to_side_right():
    import rsgislib.vectorgeoms
    from osgeo import ogr

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
    from osgeo import ogr

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

    vec_line_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_coastal_lines.geojson")
    vec_line_lyr = "welsh_coastal_lines"

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

    vec_line_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_coastal_lines.geojson")
    vec_line_lyr = "welsh_coastal_lines"

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


def test_closest_line_intersection(tmp_path):
    import rsgislib.vectorgeoms

    vec_line_file = os.path.join(
        VECTORGEOMS_DATA_DIR, "welsh_coastal_orthg_lines.geojson"
    )
    vec_line_lyr = "welsh_coastal_orthg_lines"

    vec_objs_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_objs_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.closest_line_intersection(
        vec_line_file,
        vec_line_lyr,
        vec_objs_file,
        vec_objs_lyr,
        out_vec_file,
        out_vec_lyr,
        start_x_field="start_x",
        start_y_field="start_y",
        uid_field="uid",
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_line_intersection_range(tmp_path):
    import rsgislib.vectorgeoms

    vec_line_file = os.path.join(
        VECTORGEOMS_DATA_DIR, "welsh_coastal_orthg_lines.geojson"
    )
    vec_line_lyr = "welsh_coastal_orthg_lines"

    vec_objs_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_objs_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.line_intersection_range(
        vec_line_file,
        vec_line_lyr,
        vec_objs_file,
        vec_objs_lyr,
        out_vec_file,
        out_vec_lyr,
        start_x_field="start_x",
        start_y_field="start_y",
        uid_field="uid",
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_scnd_line_intersection_range(tmp_path):
    import rsgislib.vectorgeoms

    vec_line_file = os.path.join(
        VECTORGEOMS_DATA_DIR, "welsh_coastal_orthg_lines.geojson"
    )
    vec_line_lyr = "welsh_coastal_orthg_lines"

    vec_objs_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_objs_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.scnd_line_intersection_range(
        vec_line_file,
        vec_line_lyr,
        vec_objs_file,
        vec_objs_lyr,
        out_vec_file,
        out_vec_lyr,
        start_x_field="start_x",
        start_y_field="start_y",
        uid_field="uid",
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_calc_poly_centroids(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.calc_poly_centroids(
        vec_file, vec_lyr, "GPKG", out_vec_file, out_vec_lyr
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_vec_lyr_intersection_gp(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_intersection_gp(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_vec_lyr_difference_gp(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_difference_gp(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_vec_lyr_sym_difference_gp(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_sym_difference_gp(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_vec_lyr_identity_gp(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_identity_gp(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_vec_lyr_union_gp(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_union_gp(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_get_vec_lyr_as_pts():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"

    pts = rsgislib.vectorgeoms.get_vec_lyr_as_pts(vec_file, vec_lyr)

    assert len(pts) == 41


@pytest.mark.skipif(ALPHASHAPE_NOT_AVAIL, reason="alphashape dependency not available")
def test_create_alpha_shape(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.create_alpha_shape(
        vec_file,
        vec_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        alpha_val=2,
        alpha_vals=None,
        max_iter=100,
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_explode_vec_lyr(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.explode_vec_lyr(
        vec_file, vec_lyr, out_vec_file, out_vec_lyr, out_format="GPKG"
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_explode_vec_files(tmp_path):
    import rsgislib.vectorgeoms
    import glob

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")

    rsgislib.vectorgeoms.explode_vec_files([vec_file], output_dir=tmp_path)

    assert len(glob.glob(os.path.join(tmp_path, "*.gpkg"))) == 1


def test_convert_multi_geoms_to_single(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.convert_multi_geoms_to_single(
        vec_file, vec_lyr, "GPKG", out_vec_file, out_vec_lyr
    )

    assert os.path.exists(out_vec_file)


def test_simplify_geometries(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly.geojson")
    vec_lyr = "aber_osgb_single_poly"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.simplify_geometries(
        vec_file, vec_lyr, 20, "GPKG", out_vec_file, out_vec_lyr, del_exist_vec=False
    )

    assert os.path.exists(out_vec_file)


def test_delete_polygon_holes(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly_hole.geojson")
    vec_lyr = "aber_osgb_single_poly_hole"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.delete_polygon_holes(
        vec_file,
        vec_lyr,
        "GPKG",
        out_vec_file,
        out_vec_lyr,
        area_thres=None,
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_get_poly_hole_area():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_single_poly_hole.geojson")
    vec_lyr = "aber_osgb_single_poly_hole"

    areas = rsgislib.vectorgeoms.get_poly_hole_area(vec_file, vec_lyr)

    assert (len(areas) == 1) and (abs(areas[0] - 524701) < 1)


def test_rm_polys_area_lessthan(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.rm_polys_area(
        vec_file,
        vec_lyr,
        area_thres=10000,
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format="GPKG",
        less_than=True,
    )

    assert os.path.exists(out_vec_file)


def test_rm_polys_area_greatthan(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.rm_polys_area(
        vec_file,
        vec_lyr,
        area_thres=10000,
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format="GPKG",
        less_than=False,
    )

    assert os.path.exists(out_vec_file)


def test_vec_lyr_intersection(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_intersection(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_vec_lyr_difference(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_difference(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


def test_vec_lyr_difference_sym(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    vec_over_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_over_lyr = "welsh_land_area"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.vec_lyr_difference(
        vec_file,
        vec_lyr,
        vec_over_file,
        vec_over_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
        symmetric=True,
        del_exist_vec=False,
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_clip_vec_lyr(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.clip_vec_lyr(
        vec_file,
        vec_lyr,
        vec_roi_file,
        vec_roi_lyr,
        out_vec_file,
        out_vec_lyr,
        out_format="GPKG",
    )

    assert os.path.exists(out_vec_file)


def test_get_geom_pts():
    import rsgislib.vectorgeoms
    from osgeo import ogr

    # Create ring
    ring = ogr.Geometry(ogr.wkbLinearRing)
    ring.AddPoint(1179091.1646903288, 712782.8838459781)
    ring.AddPoint(1161053.0218226474, 667456.2684348812)
    ring.AddPoint(1214704.933941905, 641092.8288590391)
    ring.AddPoint(1228580.428455506, 682719.3123998424)
    ring.AddPoint(1218405.0658121984, 721108.1805541387)
    ring.AddPoint(1179091.1646903288, 712782.8838459781)

    # Create polygon
    poly = ogr.Geometry(ogr.wkbPolygon)
    poly.AddGeometry(ring)

    pts = rsgislib.vectorgeoms.get_geom_pts(poly)

    assert len(pts) == 6


def test_vec_intersects_vec():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_bool = rsgislib.vectorgeoms.vec_intersects_vec(
        vec_file, vec_lyr, vec_roi_file, vec_roi_lyr
    )

    assert out_bool


def test_vec_overlaps_vec():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_bool = rsgislib.vectorgeoms.vec_overlaps_vec(
        vec_file, vec_lyr, vec_roi_file, vec_roi_lyr
    )

    assert out_bool


def test_vec_within_vec():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_bool = rsgislib.vectorgeoms.vec_within_vec(
        vec_file, vec_lyr, vec_roi_file, vec_roi_lyr
    )

    assert not out_bool


def test_vec_contains_vec():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_bool = rsgislib.vectorgeoms.vec_contains_vec(
        vec_file, vec_lyr, vec_roi_file, vec_roi_lyr
    )

    assert not out_bool


def test_vec_touches_vec():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_bool = rsgislib.vectorgeoms.vec_touches_vec(
        vec_file, vec_lyr, vec_roi_file, vec_roi_lyr
    )

    assert not out_bool


def test_vec_crosses_vec():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    vec_roi_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_roi_lyr = "aber_osgb_multi_polys"

    out_bool = rsgislib.vectorgeoms.vec_crosses_vec(
        vec_file, vec_lyr, vec_roi_file, vec_roi_lyr
    )

    assert not out_bool


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_get_geoms_as_bboxs():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    bboxes = rsgislib.vectorgeoms.get_geoms_as_bboxs(vec_file, vec_lyr)

    assert len(bboxes) == 4


def test_bbox_intersects_vec_lyr():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(VECTORGEOMS_DATA_DIR, "welsh_land_area.gpkg")
    vec_lyr = "welsh_land_area"

    bbox = [257774, 261752, 279153, 283287]

    out_bool = rsgislib.vectorgeoms.bbox_intersects_vec_lyr(vec_file, vec_lyr, bbox)

    assert out_bool


@pytest.mark.skipif(GEOPANDAS_NOT_AVAIL, reason="geopandas dependency not available")
def test_shiftxy_vec_lyr(tmp_path):
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    out_vec_file = os.path.join(tmp_path, "out_vec.gpkg")
    out_vec_lyr = "out_vec"

    rsgislib.vectorgeoms.shiftxy_vec_lyr(
        vec_file, vec_lyr, 200, 300, out_vec_file, out_vec_lyr, out_format="GPKG"
    )

    assert os.path.exists(out_vec_file)


@pytest.mark.skipif(RTREE_NOT_AVAIL, reason="tree dependency not available")
def test_create_rtree_index():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    idx_obj, geom_lst = rsgislib.vectorgeoms.create_rtree_index(vec_file, vec_lyr)

    assert len(geom_lst) == 4


@pytest.mark.skipif(RTREE_NOT_AVAIL, reason="tree dependency not available")
def test_bbox_intersects_index():
    import rsgislib.vectorgeoms

    vec_file = os.path.join(DATA_DIR, "aber_osgb_multi_polys.geojson")
    vec_lyr = "aber_osgb_multi_polys"

    rt_idx, geom_lst = rsgislib.vectorgeoms.create_rtree_index(vec_file, vec_lyr)

    bbox = [257774, 261752, 279153, 283287]

    out_bool = rsgislib.vectorgeoms.bbox_intersects_index(rt_idx, geom_lst, bbox)

    assert out_bool
