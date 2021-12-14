import os

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")


def test_get_epsg_code_from_wkt_utm():
    import rsgislib.tools.projection

    wkt_str = """PROJCS["WGS 84 / UTM zone 30N",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",-3],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AXIS["Easting",EAST],AXIS["Northing",NORTH],AUTHORITY["EPSG","32630"]]"""

    assert 32630 == rsgislib.tools.projection.get_epsg_code_from_wkt(wkt_str)


def test_get_wkt_from_epsg_code_32630():
    import rsgislib.tools.projection

    wkt_str = rsgislib.tools.projection.get_wkt_from_epsg_code(32630)
    assert (wkt_str is not None) and ("UTM zone 30N" in wkt_str)


def test_get_wkt_from_epsg_code_27700():
    import rsgislib.tools.projection

    wkt_str = rsgislib.tools.projection.get_wkt_from_epsg_code(27700)
    assert (wkt_str is not None) and ("British" in wkt_str)


def test_get_osr_prj_obj_32630():
    import rsgislib.tools.projection

    spat_ref = rsgislib.tools.projection.get_osr_prj_obj(32630)
    assert spat_ref is not None


def test_get_osr_prj_obj_27700():
    import rsgislib.tools.projection

    spat_ref = rsgislib.tools.projection.get_osr_prj_obj(27700)
    assert spat_ref is not None


def test_degrees_to_metres():
    import rsgislib.tools.projection

    x_size, y_size = rsgislib.tools.projection.degrees_to_metres(50, 0.1, 0.05)
    assert (abs(x_size - 7169.575) < 0.1) and (abs(y_size - 5561.453) < 0.1)


def test_metres_to_degrees():
    import rsgislib.tools.projection

    lon_size, lat_size = rsgislib.tools.projection.metres_to_degrees(-25, 250, 500)
    assert (abs(lon_size - 0.0025) < 0.001) and (abs(lat_size - 0.0045) < 0.001)


def test_get_deg_coord_as_str_default():
    import rsgislib.tools.projection

    coords_str = rsgislib.tools.projection.get_deg_coord_as_str(-25, 60)
    assert coords_str == "s0250e0600"


def test_get_deg_coord_as_str_chars3():
    import rsgislib.tools.projection

    coords_str = rsgislib.tools.projection.get_deg_coord_as_str(-25, 60, n_chars=3)
    assert coords_str == "s250e600"
