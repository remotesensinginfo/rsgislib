#! /usr/bin/env python
"""
The tools.utm module contains some useful tools for work with UTM projection.

This code has been adapted from https://github.com/Turbo87/utm/blob/master/utm/conversion.py

"""
import numpy

import rsgislib

K0 = 0.9996

E = 0.00669438
E2 = E * E
E3 = E2 * E
E_P2 = E / (1.0 - E)

SQRT_E = numpy.sqrt(1 - E)
_E = (1 - SQRT_E) / (1 + SQRT_E)
_E2 = _E * _E
_E3 = _E2 * _E
_E4 = _E3 * _E
_E5 = _E4 * _E

M1 = 1 - E / 4 - 3 * E2 / 64 - 5 * E3 / 256
M2 = 3 * E / 8 + 3 * E2 / 32 + 45 * E3 / 1024
M3 = 15 * E2 / 256 + 45 * E3 / 1024
M4 = 35 * E3 / 3072

P2 = 3.0 / 2 * _E - 27.0 / 32 * _E3 + 269.0 / 512 * _E5
P3 = 21.0 / 16 * _E2 - 55.0 / 32 * _E4
P4 = 151.0 / 96 * _E3 - 417.0 / 128 * _E5
P5 = 1097.0 / 512 * _E4

R = 6378137

ZONE_LETTERS = "CDEFGHJKLMNPQRSTUVWXX"


def min_max_eastings_for_lat(latitude: float, zone: int = 1) -> (float, float):
    """
    Find the minimum and maximum Eastings for a specified latitude within a specific zone.
    This should be the same for all zones.

    :param latitude: float
    :param zone: int
    :return: tuple [min, max]
    """
    if isinstance(latitude, numpy.ndarray):
        raise rsgislib.RSGISPyException("Don't input an array.")

    central_long = zone_number_to_central_longitude(zone)
    min_long = central_long - 3.0
    max_long = central_long + 3.0

    min_utm = from_latlon(latitude, min_long, force_zone_number=zone)
    max_utm = from_latlon(latitude, max_long, force_zone_number=zone)

    return min_utm[0], max_utm[0]


def epsg_for_utm(zone: int, hemisphere: str) -> int:
    """
    Return EPSG code for given UTM zone and hemisphere using WGS84 datum.

    :param zone: UTM zone
    :param hemisphere: hemisphere either 'N' or 'S'
    :return: corresponding EPSG code
    """
    if hemisphere not in ["N", "S"]:
        raise rsgislib.RSGISPyException('Invalid hemisphere ("N" or "S").')

    if zone < 0 or zone > 60:
        raise rsgislib.RSGISPyException("UTM zone ouside valid range.")

    if hemisphere == "N":
        ns = 600
    else:
        ns = 700

    if zone == 0:
        zone = 61

    return int(32000 + ns + zone)


def utm_from_epsg(epsg_code: int) -> (int, str):
    """
    Return UTM zone and hemisphere from a EPSG code using WGS84 datum.

    :param epsg_code: epsg code for the UTM projection.
    :return: zone, hemisphere
    """
    h_zone = epsg_code - 32000

    if h_zone < 700:
        hemisphere = "N"
        zone = h_zone - 600
    else:
        hemisphere = "S"
        zone = h_zone - 700

    return zone, hemisphere


def split_wgs84_bbox_utm_zones(wgs84_bbox):
    """
    Function which splits a bounding box (MinX, MaxX, MinY, MaxY) into
    a number of small bounding boxes (MinX, MaxX, MinY, MaxY) at UTM zone
    boundaries.

    :param wgs84_bbox: bounding box in lat/long WGS84 (MinX, MaxX, MinY, MaxY)
    :return: list [utm zone, bbox]
    """
    utm_tl = from_latlon(wgs84_bbox[3], wgs84_bbox[0])
    utm_tr = from_latlon(wgs84_bbox[3], wgs84_bbox[1])
    utm_br = from_latlon(wgs84_bbox[2], wgs84_bbox[1])
    utm_bl = from_latlon(wgs84_bbox[2], wgs84_bbox[0])

    utm_zones = []
    for utm_coord in [utm_tl, utm_tr, utm_br, utm_bl]:
        utm_zones.append(utm_coord[2])

    min_zone = numpy.min(utm_zones)
    max_zone = numpy.max(utm_zones)

    zones = numpy.arange(min_zone, max_zone + 1, 1)

    out_bboxs = []
    for zone in zones:
        zone_min = ((zone * 6.0) - 6.0) - 180.0
        zone_max = (zone * 6.0) - 180.0

        # Intersect input BBOX with zone bound
        x_min = zone_min
        if wgs84_bbox[0] > zone_min:
            x_min = wgs84_bbox[0]

        x_max = zone_max
        if wgs84_bbox[1] < zone_max:
            x_max = wgs84_bbox[1]

        out_bboxs.append([zone, [x_min, x_max, wgs84_bbox[2], wgs84_bbox[3]]])

    return out_bboxs


def check_valid_zone(
    zone_number: int, zone_letter: str = None, use_exp: bool = False
) -> bool:
    """
    Check that the UTM zone is valid.

    :param zone_number: int for the UTM zone
    :param zone_letter: string for the zone letter.
    :param use_exp: If True then an exception will be thrown if the zone is invalid
                    if False (default) then a boolean value of False will be returned
                    by the function if zone is not valid.
    :return: True if zone is valid and Falsei if not.

    """
    valid_zone = True
    if not 1 <= zone_number <= 60:
        if use_exp:
            raise rsgislib.RSGISPyException(
                "zone number out of range (must be between 1 and 60)"
            )
        valid_zone = False

    if zone_letter:
        zone_letter = zone_letter.upper()

        if not "C" <= zone_letter <= "X" or zone_letter in ["I", "O"]:
            if use_exp:
                raise rsgislib.RSGISPyException(
                    "zone letter out of range (must be between C and X)"
                )
            valid_zone = False

    return valid_zone


def to_latlon(
    easting, northing, zone_number, zone_letter=None, northern=None, strict=True
):
    """
    This function convert an UTM coordinate into Latitude and Longitude

    :param easting: int or array: Easting value of UTM coordinate
    :param northing: int or array: Northing value of UTM coordinate
    :param zone number: int: Zone Number is represented with global map numbers of an UTM Zone
        Numbers Map. More information see http://www.jaworski.ca/utmzones.htm
    :param zone_letter: str: Zone Letter can be represented as string values.
        Where UTM Zone Designators can be accessed in http://www.jaworski.ca/utmzones.htm
    :param northern: bool: You can set True or False to set this parameter. Default is None
    """
    import rsgislib.tools.utils

    if not zone_letter and northern is None:
        raise ValueError("either zone_letter or northern needs to be set")

    elif zone_letter and northern is not None:
        raise ValueError("set either zone_letter or northern, but not both")

    if strict:
        if not rsgislib.tools.utils.in_bounds(
            easting, 100000, 1000000, upper_strict=True
        ):
            raise rsgislib.RSGISPyException(
                "easting out of range (must be between 100.000 m and 999.999 m)"
            )
        if not rsgislib.tools.utils.in_bounds(northing, 0, 10000000):
            raise rsgislib.RSGISPyException(
                "northing out of range (must be between 0 m and 10.000.000 m)"
            )

    check_valid_zone(zone_number, zone_letter)

    if zone_letter:
        zone_letter = zone_letter.upper()
        northern = zone_letter >= "N"

    x = easting - 500000
    y = northing

    if not northern:
        y -= 10000000

    m = y / K0
    mu = m / (R * M1)

    p_rad = (
        mu
        + P2 * numpy.sin(2 * mu)
        + P3 * numpy.sin(4 * mu)
        + P4 * numpy.sin(6 * mu)
        + P5 * numpy.sin(8 * mu)
    )

    p_sin = numpy.sin(p_rad)
    p_sin2 = p_sin * p_sin

    p_cos = numpy.cos(p_rad)

    p_tan = p_sin / p_cos
    p_tan2 = p_tan * p_tan
    p_tan4 = p_tan2 * p_tan2

    ep_sin = 1 - E * p_sin2
    ep_sin_sqrt = numpy.sqrt(1 - E * p_sin2)

    n = R / ep_sin_sqrt
    r = (1 - E) / ep_sin

    c = _E * p_cos ** 2
    c2 = c * c

    d = x / (n * K0)
    d2 = d * d
    d3 = d2 * d
    d4 = d3 * d
    d5 = d4 * d
    d6 = d5 * d

    latitude = (
        p_rad
        - (p_tan / r)
        * (d2 / 2 - d4 / 24 * (5 + 3 * p_tan2 + 10 * c - 4 * c2 - 9 * E_P2))
        + d6 / 720 * (61 + 90 * p_tan2 + 298 * c + 45 * p_tan4 - 252 * E_P2 - 3 * c2)
    )

    longitude = (
        d
        - d3 / 6 * (1 + 2 * p_tan2 + c)
        + d5 / 120 * (5 - 2 * c + 28 * p_tan2 - 3 * c2 + 8 * E_P2 + 24 * p_tan4)
    ) / p_cos

    return (
        numpy.degrees(latitude),
        numpy.degrees(longitude) + zone_number_to_central_longitude(zone_number),
    )


def from_latlon(
    latitude: numpy.array,
    longitude: numpy.array,
    force_zone_number: int = None,
    force_zone_letter: str = None,
) -> (numpy.array, numpy.array, numpy.array, numpy.array):
    """
    This function convert Latitude and Longitude to UTM coordinate

    :param latitude: float or array. Latitude between 80 deg S and 84 deg N, e.g. (-80.0 to 84.0)
    :param longitude: float or array: Longitude between 180 deg W and 180 deg E, e.g. (-180.0 to 180.0).
    :param force_zone number: int: Zone Number is represented with global map numbers of an UTM Zone
        Numbers Map. You may force conversion including one UTM Zone Number.
        More information see http://www.jaworski.ca/utmzones.htm
    :return: eastings, northings, zone_number, zone_letter
    """
    import rsgislib.tools.utils

    if not rsgislib.tools.utils.in_bounds(latitude, -80.0, 84.0):
        raise rsgislib.RSGISPyException(
            "latitude out of range (must be between 80 deg S and 84 deg N)"
        )
    if not rsgislib.tools.utils.in_bounds(longitude, -180.0, 180.0):
        raise rsgislib.RSGISPyException(
            "longitude out of range (must be between 180 deg W and 180 deg E)"
        )
    if force_zone_number is not None:
        check_valid_zone(force_zone_number, force_zone_letter)

    lat_rad = numpy.radians(latitude)
    lat_sin = numpy.sin(lat_rad)
    lat_cos = numpy.cos(lat_rad)

    lat_tan = lat_sin / lat_cos
    lat_tan2 = lat_tan * lat_tan
    lat_tan4 = lat_tan2 * lat_tan2

    if force_zone_number is None:
        zone_number = latlon_to_zone_number(latitude, longitude)
    else:
        zone_number = force_zone_number

    if force_zone_letter is None:
        zone_letter = latitude_to_zone_letter(latitude)
    else:
        zone_letter = force_zone_letter

    lon_rad = numpy.radians(longitude)
    central_lon = zone_number_to_central_longitude(zone_number)
    central_lon_rad = numpy.radians(central_lon)

    n = R / numpy.sqrt(1 - E * lat_sin ** 2)
    c = E_P2 * lat_cos ** 2

    a = lat_cos * (lon_rad - central_lon_rad)
    a2 = a * a
    a3 = a2 * a
    a4 = a3 * a
    a5 = a4 * a
    a6 = a5 * a

    m = R * (
        M1 * lat_rad
        - M2 * numpy.sin(2 * lat_rad)
        + M3 * numpy.sin(4 * lat_rad)
        - M4 * numpy.sin(6 * lat_rad)
    )

    easting = (
        K0
        * n
        * (
            a
            + a3 / 6 * (1 - lat_tan2 + c)
            + a5 / 120 * (5 - 18 * lat_tan2 + lat_tan4 + 72 * c - 58 * E_P2)
        )
        + 500000
    )

    northing = K0 * (
        m
        + n
        * lat_tan
        * (
            a2 / 2
            + a4 / 24 * (5 - lat_tan2 + 9 * c + 4 * c ** 2)
            + a6 / 720 * (61 - 58 * lat_tan2 + lat_tan4 + 600 * c - 330 * E_P2)
        )
    )

    if rsgislib.tools.utils.mixed_signs(latitude):
        raise ValueError("latitudes must all have the same sign")
    elif rsgislib.tools.utils.negative(latitude):
        northing += 10000000

    return easting, northing, zone_number, zone_letter


def latitude_to_zone_letter(latitude):
    """
    Find the zone letter for the latitude. If the input is a numpy array, just use the first element
    user responsibility to make sure that all points are in one zone

    :param latitude:
    :return: string if found otherwise None.
    """
    if isinstance(latitude, numpy.ndarray):
        latitude = latitude.flat[0]

    if -80 <= latitude <= 84:
        return ZONE_LETTERS[int(latitude + 80) >> 3]
    else:
        return None


def latlon_to_zone_number(latitude, longitude):
    """
    Find the UTM zone number for a give latitude and longitude. If the input is a numpy array, just use the
    first element user responsibility to make sure that all points are in one zone

    :param latitude: float
    :param longitude: float

    :return: int
    """
    if isinstance(latitude, numpy.ndarray):
        latitude = latitude.flat[0]
    if isinstance(longitude, numpy.ndarray):
        longitude = longitude.flat[0]

    if 56 <= latitude < 64 and 3 <= longitude < 12:
        return 32

    if 72 <= latitude <= 84 and longitude >= 0:
        if longitude < 9:
            return 31
        elif longitude < 21:
            return 33
        elif longitude < 33:
            return 35
        elif longitude < 42:
            return 37

    return int((longitude + 180) / 6) + 1


def latlon_arr_to_utm_zone_number(
    latitude: numpy.array, longitude: numpy.array
) -> numpy.array:
    """
    Find the UTM zone number for a give latitude and longitude. UTM zone will be
    returned for all the lat/longs within the input arrays, which must be of the same
    length. Function will also work with a single value, at which point a single
    int will be returned.

    :param latitude: numpy array of floats
    :param longitude: numpy array of floats

    :return: int or array of ints.
    """
    # utm_zones = numpy.zeros_like(latitude, dtype=numpy.dtype(int))
    utm_zones = ((longitude + 180) / 6) + 1
    utm_zones = numpy.rint(utm_zones).astype(int)

    utm_zones[
        (56 <= latitude) & (latitude < 64) & (3 <= longitude) & (longitude < 12)
    ] = 32
    utm_zones[
        (72 <= latitude) & (latitude <= 84) & (longitude >= 0) & (longitude < 9)
    ] = 31
    utm_zones[
        (72 <= latitude) & (latitude <= 84) & (longitude >= 0) & (longitude < 21)
    ] = 33
    utm_zones[
        (72 <= latitude) & (latitude <= 84) & (longitude >= 0) & (longitude < 33)
    ] = 35
    utm_zones[
        (72 <= latitude) & (latitude <= 84) & (longitude >= 0) & (longitude < 42)
    ] = 37

    return utm_zones


def latlon_to_mode_utm_zone_number(
    latitude: numpy.array, longitude: numpy.array
) -> int:
    """
    Find the mode UTM zone for a list of lat/lon values.

    :param latitude: numpy array of floats
    :param longitude: numpy array of floats
    :return: int (mode UTM zone)

    """
    import scipy.stats

    utm_zones = latlon_arr_to_utm_zone_number(latitude, longitude)
    mode, count = scipy.stats.mode(utm_zones)
    return mode[0]


def zone_number_to_central_longitude(zone_number: int) -> int:
    """
    Find the central longitude for the given zone.

    :param zone_number: int
    :return: float
    """
    return (zone_number - 1) * 6 - 180 + 3
