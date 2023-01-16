#!/usr/bin/env python
"""
The tools.tilecache module contains functions working with tilecaches.
"""

import math

import rsgislib

TILE_SIZE = 256
EARTH_RADIUS = 6378137.0
ORIGIN_SHIFT = 2.0 * math.pi * EARTH_RADIUS / 2.0
INITIAL_RESOLUTION = 2.0 * math.pi * EARTH_RADIUS / float(TILE_SIZE)


def resolution(zoom):
    """
    Calculate the image resolution for a zoom level.

    :return: float image pixel resolution

    """
    return INITIAL_RESOLUTION / (2 ** zoom)


def truncate_lng_lat(lon_deg, lat_deg):
    """
    A function which truncates latitude and londitude values
    to be better -180 to 180 and -90 to 90.

    :param lon_deg: float with longitude value.
    :param lat_deg: float with latitude value.
    :return: (lon_deg, lat_deg)

    """
    if lon_deg > 180.0:
        lon_deg = 180.0
    elif lon_deg < -180.0:
        lon_deg = -180.0
    if lat_deg > 90.0:
        lat_deg = 90.0
    elif lat_deg < -90.0:
        lat_deg = -90.0
    return lon_deg, lat_deg


def get_tile_for_point(lon_deg, lat_deg, zoom, tms=True, tile_size=TILE_SIZE):
    """
    :param lon_deg: float with longitude value.
    :param lat_deg: float with latitude value.
    :param zoom: the zoom level for the tile.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :param tile_size: is the size, in pixels, of the tiles (default 256).
    :return: (tile_x, tile_y, zoom)

    """
    meter_x = lon_deg * ORIGIN_SHIFT / 180.0
    meter_y = math.log(math.tan((90.0 + lat_deg) * math.pi / 360.0)) / (math.pi / 180.0)
    meter_y = meter_y * ORIGIN_SHIFT / 180.0

    pixel_x = (meter_x + ORIGIN_SHIFT) / resolution(zoom)
    pixel_y = (meter_y - ORIGIN_SHIFT) / resolution(zoom)
    pixel_x = abs(round(pixel_x))
    pixel_y = abs(round(pixel_y))

    tile_x = int(math.ceil(pixel_x / float(tile_size)) - 1)
    tile_y = int(math.ceil(pixel_y / float(tile_size)) - 1)
    if tms:
        tile_y = (2 ** zoom - 1) - tile_y

    return tile_x, tile_y, zoom


def get_tile_tl_lonlat(tile_x, tile_y, zoom, tms=True):
    """
    Returns the Top-Left coordinate for the tile.

    :param tile_x: The x index for the tile.
    :param tile_y: The y index for the tile (will change if TMS or XYZ indexing).
    :param zoom: the zoom level for the tile.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :return: (longitude, latitude)

    """
    if tms:
        tile_y = (2 ** zoom - 1) - tile_y
    n = 2.0 ** zoom
    lon_deg = tile_x / n * 360.0 - 180.0
    lat_rad = math.atan(math.sinh(math.pi * (1 - 2 * tile_y / n)))
    lat_deg = math.degrees(lat_rad)
    return (lon_deg, lat_deg)


def get_tile_bbox(tile_x, tile_y, zoom, tms=True):
    """
    Returns the bounding box (BBOX) of a tile

    :param tile_x: The x index for the tile.
    :param tile_y: The y index for the tile (will change if TMS or XYZ indexing).
    :param zoom: the zoom level for the tile.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :return: (MinLon, MaxLon, MinLat, MaxLat)

    """
    a = get_tile_tl_lonlat(tile_x, tile_y, zoom, tms)
    b = get_tile_tl_lonlat(tile_x + 1, tile_y + 1, zoom, tms)
    return [a[0], b[0], b[1], a[1]]


def flip_xyz_tms_tiles(tile_x, tile_y, zoom):
    """
    If you put in a XYZ tile index you will have returned
    an TMS index. If you put in a TMS index you will have
    returned XYZ.

    :param tile_x: The x index for the tile.
    :param tile_y: The y index for the tile (will change if TMS or XYZ indexing).
    :param zoom: the zoom level for the tile.
    :return:  (tile_x, tile_y, zoom)

    """
    tile_y = (2 ** zoom - 1) - tile_y
    return tile_x, tile_y, zoom


def get_web_mercator_xy(lon_deg, lat_deg, truncate=False):
    """
    Convert a WGS84 longitude, latitude coordinate to a web mercator coordinate (meters).

    :param lon_deg: float with longitude value.
    :param lat_deg: float with latitude value.
    :param truncate: Whether to truncate or clip inputs to web mercator limits.
    :return: (meter_x, meter_y)
    """
    if truncate:
        lon_deg, lat_deg = truncate_lng_lat(lon_deg, lat_deg)

    meter_x = lon_deg * ORIGIN_SHIFT / 180.0
    meter_y = math.log(math.tan((90.0 + lat_deg) * math.pi / 360.0)) / (math.pi / 180.0)
    meter_y = meter_y * ORIGIN_SHIFT / 180.0

    return meter_x, meter_y


def get_xy_mercator_lonlat(meter_x, meter_y, truncate=False):
    """
    Convert a web mercator coordinate (meters) to a WGS84 longitude, latitude coordinate.

    :param meter_x: x coordinate in the web mercator coordinate system
    :param meter_y: y coordinate in the web mercator coordinate system
    :param truncate: Whether to truncate or clip inputs to web mercator limits.
    :return: (longitude, latitude)

    """
    lon_deg = (meter_x / ORIGIN_SHIFT) * 180.0
    lat_deg = (meter_y / ORIGIN_SHIFT) * 180.0
    lat_deg = (
        180.0
        / math.pi
        * (2 * math.atan(math.exp(lat_deg * math.pi / 180.0)) - math.pi / 2.0)
    )

    if truncate:
        lon_deg, lat_deg = truncate_lng_lat(lon_deg, lat_deg)

    return lon_deg, lat_deg


def xy_mercator_bounds(tile_x, tile_y, zoom, tms=True):
    """
    Returns the bounding box of a tile in the web mercator coordinate system (meters).

    :param tile_x: The x index for the tile.
    :param tile_y: The y index for the tile (will change if TMS or XYZ indexing).
    :param zoom: the zoom level for the tile.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :return: (MinX, MaxX, MinY, MaxY)

    """
    bbox = get_tile_bbox(tile_x, tile_y, zoom, tms)
    x_min, y_max = get_web_mercator_xy(bbox[0], bbox[3])
    x_max, y_min = get_web_mercator_xy(bbox[1], bbox[2])
    return (x_min, x_max, y_min, y_max)


def get_quadkey_for_tile_path(tile_x, tile_y, zoom, tms=True):
    """
    Get the quadkey of the tile

    :param tile_x: The x index for the tile.
    :param tile_y: The y index for the tile (will change if TMS or XYZ indexing).
    :param zoom: the zoom level for the tile.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :return: a quadkey for the tile.

    """
    if tms:
        tile_y = (2 ** zoom - 1) - tile_y
    qk = []
    for z in range(zoom, 0, -1):
        digit = 0
        mask = 1 << (z - 1)
        if tile_x & mask:
            digit += 1
        if tile_y & mask:
            digit += 2
        qk.append(str(digit))
    return "".join(qk)


def get_tile_path_from_quadkey(quadkey, tms=True):
    """
    Return the tile path for an inputted quadkey.

    :param quadkey: the inputted quadkey for the tile.
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :returns: (tile_x, tile_y, zoom)

    """
    if len(quadkey) == 0:
        return 0, 0, 0
    tile_x, tile_y = 0, 0
    for i, digit in enumerate(reversed(quadkey)):
        mask = 1 << i
        if digit == "1":
            tile_x = tile_x | mask
        elif digit == "2":
            tile_y = tile_y | mask
        elif digit == "3":
            tile_x = tile_x | mask
            tile_y = tile_y | mask
        elif digit != "0":
            raise rsgislib.RSGISPyException(
                "Unexpected quadkey digit: {}".format(digit)
            )
    zoom = i + 1
    if tms:
        tile_y = (2 ** zoom - 1) - tile_y
    return tile_x, tile_y, zoom


def get_tiles(bbox, zooms, tms=True, truncate=False):
    """
    Gets a list of tile indexes for all the zoom levels specified. The tile indexes are returned as a
    dict with the zoom as the key which references a list of tiles.

    :param bbox: A bounding box in WGS84 longitude/latitude values
                 (MinX, MaxX, MinY, MaxY).
    :param zooms: list of zoom levels
    :param tms: if TMS is True then a tile grid in TMS format is returned with
                the grid origin at the bottom-left. If False then an XYZ tile grid
                format is used with the origin in the top-left.
    :param truncate: Whether to truncate or clip inputs to web mercator limits.
    :return: dict[zoom] = [(tile_x, tile_y, zoom),  (tile_x, tile_y, zoom) ...  (tile_x, tile_y, zoom)]

    """
    import math
    from collections.abc import Sequence

    west = bbox[0]
    east = bbox[1]
    south = bbox[2]
    north = bbox[3]
    if truncate:
        west, south = truncate_lng_lat(west, south)
        east, north = truncate_lng_lat(east, north)
    if west > east:
        bbox_west = (-180.0, south, east, north)
        bbox_east = (west, south, 180.0, north)
        bboxes = [bbox_west, bbox_east]
    else:
        bboxes = [(west, south, east, north)]

    tiles = dict()

    for w, s, e, n in bboxes:
        # Clamp bounding values.
        w = max(-180.0, w)
        s = max(-85.051129, s)
        e = min(180.0, e)
        n = min(85.051129, n)

        if not isinstance(zooms, Sequence):
            zooms = [zooms]

        epsilon = 1.0e-9

        for z in zooms:
            if z not in tiles:
                tiles[z] = []
            print(z)
            llx, lly, llz = get_tile_for_point(w, s, z, tms)
            if lly % 1 < epsilon / 10:
                lly = lly - epsilon

            urx, ury, urz = get_tile_for_point(e, n, z, tms)
            if urx % 1 < epsilon / 10:
                urx = urx - epsilon

            # Clamp left x and top y at 0.
            llx = 0 if llx < 0 else llx
            ury = 0 if ury < 0 else ury

            llx, urx, lly, ury = map(lambda x: int(math.floor(x)), [llx, urx, lly, ury])

            min_x_tile = llx if llx < urx else urx

            if llx == urx:
                n_x_tiles = 1
            elif llx < urx:
                n_x_tiles = urx - min(llx + 1, 2 ** z)
            else:
                n_x_tiles = llx - min(urx + 1, 2 ** z)

            n_x_tiles = 1 if n_x_tiles == 0 else n_x_tiles
            print("\tx {} + {}".format(min_x_tile, n_x_tiles))

            min_y_tile = ury if ury < lly else lly

            if ury == lly:
                n_y_tiles = 1
            elif ury < lly:
                n_y_tiles = lly - min(ury + 1, 2 ** z)
            else:
                n_y_tiles = ury - min(lly + 1, 2 ** z)
            n_y_tiles = 1 if n_y_tiles == 0 else n_y_tiles
            print("\ty {} + {}".format(min_y_tile, n_y_tiles))

            for i in range(min_x_tile, min_x_tile + n_x_tiles):
                for j in range(min_y_tile, min_y_tile + n_y_tiles):
                    tiles[z].append((i, j, z))
    return tiles


def convert_between_tms_xyz(tiles_dir):
    """
    A function which converts between TMS and XYZ tile indexing. The conversion is performed in-situ
    with the files simply renamed. If a tile cache is in TMS format then this function will convert
    it to XYZ, while if it is in XYZ format then it will convert it to TMS (they are easily interchangeable)

    The TMS format uses a grid with the origin at the bottom-left. While the XYZ
    format is uses a grid with the origin in the top-left.

    XYZ grids can be loads into QGIS and other GIS systems as layers.
    TMS grids are outputted by gdal2tiles.py.

    :param tiles_dir: The input directory for the tile cache.

    """
    import os

    import tqdm

    import rsgislib.tools.utils

    c_pwd = os.getcwd()
    in_zoom_dirs = []
    lcl_files = os.listdir(tiles_dir)
    for lcl_file in lcl_files:
        lcl_name = lcl_file
        lcl_file = os.path.join(tiles_dir, lcl_file)
        if os.path.isdir(lcl_file) and rsgislib.tools.utils.is_number(lcl_name):
            in_zoom_dirs.append(os.path.abspath(lcl_file))
    for zoom_dir in tqdm.tqdm(in_zoom_dirs, desc="overall"):
        zoom = int(os.path.basename(zoom_dir))
        x_dirs = os.listdir(zoom_dir)
        for x_dir in tqdm.tqdm(x_dirs, desc="zoom {}".format(zoom), leave=False):
            if rsgislib.tools.utils.is_number(x_dir):
                x = int(x_dir)
                x_dir = os.path.join(zoom_dir, x_dir)
                if os.path.isdir(x_dir) and rsgislib.tools.utils.is_number(x):
                    y_files = os.listdir(x_dir)
                    os.chdir(x_dir)
                    for y_file in tqdm.tqdm(y_files, desc="y", leave=False):
                        y_cur_file = os.path.join(x_dir, y_file)
                        y_in_str = os.path.splitext(y_file)[0]
                        if rsgislib.tools.utils.is_number(y_in_str):
                            y_in = int(y_in_str)
                            img_ext = os.path.splitext(y_file)[1]
                            n_tile_x, n_tile_y, n_zoom = flip_xyz_tms_tiles(
                                x, y_in, zoom
                            )
                            out_img_file = "{}{}".format(n_tile_y, img_ext)
                            os.rename(y_cur_file, out_img_file)
    os.chdir(c_pwd)
