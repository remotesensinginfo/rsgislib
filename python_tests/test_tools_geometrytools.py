
def test_reprojPoint_OSGB_UTM():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(27700)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32630)

    osgb_x = 258051.0
    osgb_y = 281605.0

    utm_x, utm_y = rsgislib.tools.geometrytools.reprojPoint(in_osr_proj_obj, out_osr_proj_obj, osgb_x, osgb_y)

    ref_utm_x = 425966.1
    ref_utm_y = 5807642.0
    assert (abs(ref_utm_x - utm_x) < 0.1) and (abs(ref_utm_y - utm_y) < 0.1)


def test_reprojPoint_OSGB_WGS84():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(27700)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(4326)

    osgb_x = 258051.0
    osgb_y = 281605.0

    lon, lat = rsgislib.tools.geometrytools.reprojPoint(in_osr_proj_obj, out_osr_proj_obj, osgb_x, osgb_y)

    ref_lon = -4.0885
    ref_lat = 52.4140
    assert (abs(ref_lon - lon) < 0.001) and (abs(ref_lat - lat) < 0.001)

def test_reprojPoint_WGS84_OSGB():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(4326)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(27700)

    lon = -4.0885
    lat = 52.4140

    osgb_x, osgb_y = rsgislib.tools.geometrytools.reprojPoint(in_osr_proj_obj, out_osr_proj_obj, lon, lat)

    ref_osgb_x = 258051.3
    ref_osgb_y = 281606.4
    assert (abs(ref_osgb_x - osgb_x) < 0.1) and (abs(ref_osgb_y - osgb_y) < 0.1)


def test_reprojPoint_WGS84_UTM():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(4326)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32630)

    lon = -4.0885
    lat = 52.4140

    utm_x, utm_y = rsgislib.tools.geometrytools.reprojPoint(in_osr_proj_obj, out_osr_proj_obj, lon, lat)

    ref_utm_x = 425966.3
    ref_utm_y = 5807643.4
    assert (abs(ref_utm_x - utm_x) < 0.1) and (abs(ref_utm_y - utm_y) < 0.1)


