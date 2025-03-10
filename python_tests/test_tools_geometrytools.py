def test_reproj_point_OSGB_UTM():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 "
        "+x_0=400000 +y_0=-100000 +ellps=airy +datum=OSGB36 "
        "+units=m +no_defs"
    )

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32630)

    osgb_x = 258051.0
    osgb_y = 281605.0

    utm_x, utm_y = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, osgb_x, osgb_y
    )

    print(f"X: {utm_x} Y: {utm_y}")

    ref_utm_x = 425966.1
    ref_utm_y = 5807642.0
    assert (abs(ref_utm_x - utm_x) < 2) and (abs(ref_utm_y - utm_y) < 2)


def test_reproj_point_OSGB_WGS84():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 "
        "+x_0=400000 +y_0=-100000 +ellps=airy +datum=OSGB36 "
        "+units=m +no_defs"
    )

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(4326)

    osgb_x = 258051.0
    osgb_y = 281605.0

    lon, lat = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, osgb_x, osgb_y
    )

    print(f"lon: {lon} lat: {lat}")

    ref_lon = -4.0885
    ref_lat = 52.4140
    assert (abs(ref_lon - lon) < 0.001) and (abs(ref_lat - lat) < 0.001)


def test_reproj_point_WGS84_OSGB():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(4326)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 "
        "+x_0=400000 +y_0=-100000 +ellps=airy +datum=OSGB36 "
        "+units=m +no_defs"
    )

    lon = -4.0885
    lat = 52.4140

    osgb_x, osgb_y = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, lon, lat
    )

    print(f"X: {osgb_x} Y: {osgb_y}")

    ref_osgb_x = 258051.3
    ref_osgb_y = 281606.4
    assert (abs(ref_osgb_x - osgb_x) < 2) and (abs(ref_osgb_y - osgb_y) < 2)


def test_reproj_point_WGS84_UTM_1():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(4326)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32630)

    lon = -4.0885
    lat = 52.4140

    utm_x, utm_y = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, lon, lat
    )

    print(f"X: {utm_x} Y: {utm_y}")

    ref_utm_x = 425966.3
    ref_utm_y = 5807643.4
    assert (abs(ref_utm_x - utm_x) < 0.1) and (abs(ref_utm_y - utm_y) < 0.1)


def test_reproj_point_WGS84_UTM_2():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(4326)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32702)

    lon = -170.6827
    lat = -14.2655

    utm_x, utm_y = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, lon, lat
    )

    print(f"X: {utm_x} Y: {utm_y}")

    ref_utm_x = 534225.924
    ref_utm_y = 8422887.035
    assert (abs(ref_utm_x - utm_x) < 0.1) and (abs(ref_utm_y - utm_y) < 0.1)


def test_reproj_point_WGS84_UTM_3():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(4326)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32602)

    lon = -170.6827
    lat = -14.2655

    utm_x, utm_y = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, lon, lat
    )

    print(f"X: {utm_x} Y: {utm_y}")

    ref_utm_x = 534225.924
    ref_utm_y = -1577112.965
    assert (abs(ref_utm_x - utm_x) < 0.1) and (abs(ref_utm_y - utm_y) < 0.1)


def test_reproj_point_UTM_WGS84():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(32630)

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(4326)

    utm_x = 425966.3
    utm_y = 5807643.4

    lon, lat = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, utm_x, utm_y
    )

    print(f"lon: {lon} lat: {lat}")

    ref_lon = -4.0885
    ref_lat = 52.4140
    assert (abs(ref_lon - lon) < 0.001) and (abs(ref_lat - lat) < 0.001)


def test_reproj_point_NZTM_WGS84():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=0 +lon_0=173 +k=0.9996 "
        "+x_0=1600000 +y_0=10000000 +ellps=GRS80 "
        "+towgs84=0,0,0,0,0,0,0 +units=m +no_defs"
    )

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(4326)

    nztm_x = 1822776.0
    nztm_y = 5527532.6

    lon, lat = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, nztm_x, nztm_y
    )

    print(f"lon: {lon} lat: {lat}")

    ref_lon = 175.6240
    ref_lat = -40.3730
    assert (abs(ref_lat - lat) < 0.001) and (abs(ref_lon - lon) < 0.001)


def test_reproj_point_NZTM_UTM():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=0 +lon_0=173 +k=0.9996 "
        "+x_0=1600000 +y_0=10000000 +ellps=GRS80 "
        "+towgs84=0,0,0,0,0,0,0 +units=m +no_defs"
    )

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(32760)

    nztm_x = 1822776.0
    nztm_y = 5527532.6

    utm_x, utm_y = rsgislib.tools.geometrytools.reproj_point(
        in_osr_proj_obj, out_osr_proj_obj, nztm_x, nztm_y
    )

    print(f"X: {utm_x} Y: {utm_y}")

    ref_utm_x = 383187.9
    ref_utm_y = 5529929.6
    assert (abs(ref_utm_x - utm_x) < 0.1) and (abs(ref_utm_y - utm_y) < 0.1)


def test_reproj_point_to_wgs84_osgb():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 "
        "+x_0=400000 +y_0=-100000 +ellps=airy +datum=OSGB36 "
        "+units=m +no_defs"
    )

    osgb_x = 258051.0
    osgb_y = 281605.0

    lon, lat = rsgislib.tools.geometrytools.reproj_point_to_wgs84(
        in_osr_proj_obj, osgb_x, osgb_y
    )

    print(f"lon: {lon} lat: {lat}")

    ref_lon = -4.0885
    ref_lat = 52.4140
    assert (abs(ref_lon - lon) < 0.001) and (abs(ref_lat - lat) < 0.001)


def test_reproj_point_to_wgs84_utm():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(32630)

    utm_x = 425966.3
    utm_y = 5807643.4

    lon, lat = rsgislib.tools.geometrytools.reproj_point_to_wgs84(
        in_osr_proj_obj, utm_x, utm_y
    )

    print(f"lon: {lon} lat: {lat}")

    ref_lon = -4.0885
    ref_lat = 52.4140
    assert (abs(ref_lon - lon) < 0.001) and (abs(ref_lat - lat) < 0.001)


def test_reproj_point_to_wgs84_nztm():
    import rsgislib.tools.geometrytools
    from osgeo import osr

    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromProj4(
        "+proj=tmerc +lat_0=0 +lon_0=173 +k=0.9996 "
        "+x_0=1600000 +y_0=10000000 +ellps=GRS80 "
        "+towgs84=0,0,0,0,0,0,0 +units=m +no_defs"
    )

    nztm_x = 1822776.0
    nztm_y = 5527532.6

    lon, lat = rsgislib.tools.geometrytools.reproj_point_to_wgs84(
        in_osr_proj_obj, nztm_x, nztm_y
    )

    print(f"lon: {lon} lat: {lat}")

    ref_lon = 175.6240
    ref_lat = -40.3730
    assert (abs(ref_lon - lon) < 0.001) and (abs(ref_lat - lat) < 0.001)
