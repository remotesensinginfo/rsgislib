#!/usr/bin/env python
"""
The tools.geometrytool module contains functions for manipulating and moving files around.
"""

import math
from typing import Dict, List

from osgeo import ogr, osr

import rsgislib


def reproj_bbox(
    bbox: List[float],
    in_osr_proj_obj: osr.SpatialReference(),
    out_osr_proj_obj: osr.SpatialReference(),
) -> List[float]:
    """
    A function to reproject a bounding box.

    :param bbox: input bounding box (MinX, MaxX, MinY, MaxY)
    :param in_osr_proj_obj: an osr.SpatialReference() object representing input projection.
    :param out_osr_proj_obj: an osr.SpatialReference() object representing output projection.

    :return: (MinX, MaxX, MinY, MaxY)

    """
    tlX = bbox[0]
    tlY = bbox[3]
    trX = bbox[1]
    trY = bbox[3]
    brX = bbox[1]
    brY = bbox[2]
    blX = bbox[0]
    blY = bbox[2]

    out_tlX, out_tlY = reproj_point(in_osr_proj_obj, out_osr_proj_obj, tlX, tlY)
    out_trX, out_trY = reproj_point(in_osr_proj_obj, out_osr_proj_obj, trX, trY)
    out_brX, out_brY = reproj_point(in_osr_proj_obj, out_osr_proj_obj, brX, brY)
    out_blX, out_blY = reproj_point(in_osr_proj_obj, out_osr_proj_obj, blX, blY)

    minX = out_tlX
    if out_blX < minX:
        minX = out_blX

    maxX = out_brX
    if out_trX > maxX:
        maxX = out_trX

    minY = out_brY
    if out_blY < minY:
        minY = out_blY

    maxY = out_tlY
    if out_trY > maxY:
        maxY = out_trY

    return [minX, maxX, minY, maxY]


def reproj_bbox_epsg(bbox: List[float], in_epsg: int, out_epsg: int):
    """
    A function to reproject a bounding box.

    :param bbox: input bounding box (MinX, MaxX, MinY, MaxY)
    :param in_epsg: an EPSG code representing input projection.
    :param out_epsg: an EPSG code representing output projection.
    :return: (MinX, MaxX, MinY, MaxY)

    """
    in_osr_proj_obj = osr.SpatialReference()
    in_osr_proj_obj.ImportFromEPSG(int(in_epsg))

    out_osr_proj_obj = osr.SpatialReference()
    out_osr_proj_obj.ImportFromEPSG(int(out_epsg))

    out_bbox = reproj_bbox(bbox, in_osr_proj_obj, out_osr_proj_obj)
    return out_bbox


def do_bboxes_intersect(bbox1: List[float], bbox2: List[float]) -> bool:
    """
    A function which tests whether two bboxes (MinX, MaxX, MinY, MaxY) intersect.

    :param bbox1: The first bounding box (MinX, MaxX, MinY, MaxY)
    :param bbox2: The second bounding box (MinX, MaxX, MinY, MaxY)
    :return: boolean (True they intersect; False they do not intersect)

    """
    x_min = 0
    x_max = 1
    y_min = 2
    y_max = 3
    intersect = (
        (bbox1[x_max] > bbox2[x_min])
        and (bbox2[x_max] > bbox1[x_min])
        and (bbox1[y_max] > bbox2[y_min])
        and (bbox2[y_max] > bbox1[y_min])
    )
    return intersect


def does_bbox_contain(bbox1: List[float], bbox2: List[float]) -> bool:
    """
    A function which tests whether bbox1 contains bbox2.

    :param bbox1: The first bounding box (MinX, MaxX, MinY, MaxY)
    :param bbox2: The second bounding box (MinX, MaxX, MinY, MaxY)
    :return: boolean (True bbox1 contains bbox2; False bbox1 does not contain bbox2)

    """
    x_min = 0
    x_max = 1
    y_min = 2
    y_max = 3
    contains = (
        (bbox1[x_min] < bbox2[x_min])
        and (bbox1[x_max] > bbox2[x_max])
        and (bbox1[y_min] < bbox2[y_min])
        and (bbox1[y_max] > bbox2[y_max])
    )
    return contains


def calc_bbox_area(bbox: List[float]) -> float:
    """
    Calculate the area of the bbox.

    :param bbox: bounding box (MinX, MaxX, MinY, MaxY)
    :return: area in projection of the bbox.

    """
    # width x height
    return (bbox[1] - bbox[0]) * (bbox[3] - bbox[2])


def bbox_equal(bbox1: List[float], bbox2: List[float]) -> bool:
    """
    A function which tests whether two bboxes (xMin, xMax, yMin, yMax) are equal.

    :param bbox1: is a bbox (xMin, xMax, yMin, yMax)
    :param bbox2: is a bbox (xMin, xMax, yMin, yMax)
    :return: boolean

    """
    bbox_eql = False
    if (
        (bbox1[0] == bbox2[0])
        and (bbox1[1] == bbox2[1])
        and (bbox1[2] == bbox2[2])
        and (bbox1[3] == bbox2[3])
    ):
        bbox_eql = True
    return bbox_eql


def bbox_intersection(bbox1: List[float], bbox2: List[float]) -> List[float]:
    """
    A function which calculates the intersection of the two bboxes (xMin, xMax, yMin, yMax).

    :param bbox1: is a bbox (xMin, xMax, yMin, yMax)
    :param bbox2: is a bbox (xMin, xMax, yMin, yMax)
    :return: bbox (xMin, xMax, yMin, yMax)

    """
    if not do_bboxes_intersect(bbox1, bbox2):
        raise rsgislib.RSGISPyException("Bounding Boxes do not intersect.")

    xMinOverlap = bbox1[0]
    xMaxOverlap = bbox1[1]
    yMinOverlap = bbox1[2]
    yMaxOverlap = bbox1[3]

    if bbox2[0] > xMinOverlap:
        xMinOverlap = bbox2[0]

    if bbox2[1] < xMaxOverlap:
        xMaxOverlap = bbox2[1]

    if bbox2[2] > yMinOverlap:
        yMinOverlap = bbox2[2]

    if bbox2[3] < yMaxOverlap:
        yMaxOverlap = bbox2[3]

    return [xMinOverlap, xMaxOverlap, yMinOverlap, yMaxOverlap]


def bboxes_intersection(bboxes: List[List[float]]) -> List[float]:
    """
    A function to find the intersection between a list of
    bboxes.

    :param bboxes: a list of bboxes [(xMin, xMax, yMin, yMax)]
    :return: bbox (xMin, xMax, yMin, yMax)

    """
    if len(bboxes) == 1:
        return bboxes[0]
    elif len(bboxes) == 2:
        return bbox_intersection(bboxes[0], bboxes[1])

    inter_bbox = bboxes[0]
    for bbox in bboxes[1:]:
        inter_bbox = bbox_intersection(inter_bbox, bbox)
    return inter_bbox


def buffer_bbox(bbox: List[float], buf: float) -> List[float]:
    """
    Buffer the input BBOX by a set amount.

    :param bbox: the bounding box (MinX, MaxX, MinY, MaxY)
    :param buf: the amount of buffer by
    :return: the buffered bbox (MinX, MaxX, MinY, MaxY)

    """
    out_bbox = [0, 0, 0, 0]
    out_bbox[0] = bbox[0] - buf
    out_bbox[1] = bbox[1] + buf
    out_bbox[2] = bbox[2] - buf
    out_bbox[3] = bbox[3] + buf
    return out_bbox


def find_bbox_union(bboxes: List[List[float]]) -> List[float]:
    """
    A function which finds the union of all the bboxes inputted.

    :param bboxes: a list of bboxes [(xMin, xMax, yMin, yMax), (xMin, xMax, yMin, yMax)]
    :return: bbox (xMin, xMax, yMin, yMax)

    """
    if len(bboxes) == 1:
        out_bbox = list(bboxes[0])
    elif len(bboxes) > 1:
        out_bbox = list(bboxes[0])
        for bbox in bboxes:
            if bbox[0] < out_bbox[0]:
                out_bbox[0] = bbox[0]
            if bbox[1] > out_bbox[1]:
                out_bbox[1] = bbox[1]
            if bbox[2] < out_bbox[2]:
                out_bbox[2] = bbox[2]
            if bbox[3] > out_bbox[3]:
                out_bbox[3] = bbox[3]
    else:
        out_bbox = None
    return out_bbox


def unwrap_wgs84_bbox(bbox: List[float]) -> List[List[float]]:
    """
    A function which unwraps a bbox if it projected in WGS84 and over the 180/-180 boundary.

    :param bbox: the bounding box (MinX, MaxX, MinY, MaxY)
    :return: list of bounding boxes [(MinX, MaxX, MinY, MaxY), (MinX, MaxX, MinY, MaxY)...]

    """
    bboxes = []
    if bbox[1] < bbox[0]:
        bbox1 = [-180.0, bbox[1], bbox[2], bbox[3]]
        bboxes.append(bbox1)
        bbox2 = [bbox[0], 180.0, bbox[2], bbox[3]]
        bboxes.append(bbox2)
    else:
        bboxes.append(bbox)
    return bboxes


def find_common_extent_on_grid(
    base_extent: List[float],
    base_grid: float,
    other_extent: List[float],
    full_contain: bool = True,
) -> List[float]:
    """
    A function which calculates the common extent between two extents but defines output on
    grid with defined resolutions. Useful for finding common extent on a particular image grid.

    :param base_extent: is a bbox (xMin, xMax, yMin, yMax) providing the base for the grid on which output will be defined.
    :param base_grid: the size of the (square) grid on which output will be defined.
    :param other_extent: is a bbox (xMin, xMax, yMin, yMax) to be intersected with the base_extent.
    :param full_contain: is a boolean. True: moving output onto grid will increase size of bbox (i.e., intersection fully contained)
                                      False: move output onto grid will decrease size of bbox (i.e., bbox fully contained within intesection)

    :return: bbox (xMin, xMax, yMin, yMax)

    """
    xMinOverlap = base_extent[0]
    xMaxOverlap = base_extent[1]
    yMinOverlap = base_extent[2]
    yMaxOverlap = base_extent[3]

    if other_extent[0] > xMinOverlap:
        if full_contain:
            diff = math.floor((other_extent[0] - xMinOverlap) / base_grid) * base_grid
        else:
            diff = math.ceil((other_extent[0] - xMinOverlap) / base_grid) * base_grid
        xMinOverlap = xMinOverlap + diff

    if other_extent[1] < xMaxOverlap:
        if full_contain:
            diff = math.floor((xMaxOverlap - other_extent[1]) / base_grid) * base_grid
        else:
            diff = math.ceil((xMaxOverlap - other_extent[1]) / base_grid) * base_grid
        xMaxOverlap = xMaxOverlap - diff

    if other_extent[2] > yMinOverlap:
        if full_contain:
            diff = (
                math.floor(abs(other_extent[2] - yMinOverlap) / base_grid) * base_grid
            )
        else:
            diff = math.ceil(abs(other_extent[2] - yMinOverlap) / base_grid) * base_grid
        yMinOverlap = yMinOverlap + diff

    if other_extent[3] < yMaxOverlap:
        if full_contain:
            diff = (
                math.floor(abs(yMaxOverlap - other_extent[3]) / base_grid) * base_grid
            )
        else:
            diff = math.ceil(abs(yMaxOverlap - other_extent[3]) / base_grid) * base_grid
        yMaxOverlap = yMaxOverlap - diff

    return [xMinOverlap, xMaxOverlap, yMinOverlap, yMaxOverlap]


def find_extent_on_grid(
    base_extent: List[float], base_grid: float, full_contain: bool = True
) -> List[float]:
    """
    A function which calculates the extent but defined on a grid with defined
    resolution. Useful for finding extent on a particular image grid.

    :param base_extent: is a bbox (xMin, xMax, yMin, yMax) providing the base for
                        the grid on which output will be defined.
    :param base_grid: the size of the (square) grid on which output will be defined.
    :param full_contain: is a boolean. True: moving output onto grid will increase
                                       size of bbox (i.e., intersection fully contained)
                                       False: move output onto grid will decrease size
                                       of bbox (i.e., bbox fully contained within
                                       intesection)

    :return: bbox (xMin, xMax, yMin, yMax)

    """
    xMin = base_extent[0]
    xMax = base_extent[1]
    yMin = base_extent[2]
    yMax = base_extent[3]

    xMin_floor = int(math.floor(xMin))
    yMax_ceil = int(math.ceil(yMax))

    xMin_diff_floor = xMin - xMin_floor
    yMax_diff_ceil = yMax_ceil - yMax

    n_x_grid_cells = math.floor(xMin_diff_floor / base_grid)
    n_y_grid_cells = math.floor(yMax_diff_ceil / base_grid)

    xMinOut = xMin_floor + (n_x_grid_cells * base_grid)
    yMaxOut = yMax_ceil - (n_y_grid_cells * base_grid)

    diffX = xMax - xMinOut
    diffY = abs(yMaxOut - yMin)

    nPxlX = 0.0
    nPxlY = 0.0
    if full_contain:
        nPxlX = math.ceil(diffX / base_grid)
        nPxlY = math.ceil(diffY / base_grid)
    else:
        nPxlX = math.floor(diffX / base_grid)
        nPxlY = math.floor(diffY / base_grid)

    xMaxOut = xMinOut + (nPxlX * base_grid)
    yMinOut = yMaxOut - (nPxlY * base_grid)

    return [xMinOut, xMaxOut, yMinOut, yMaxOut]


def find_extent_on_whole_num_grid(
    base_extent: List[float],
    base_grid: float,
    full_contain: bool = True,
    round_vals: bool = None,
) -> List[float]:
    """
    A function which calculates the extent but defined on a grid with defined resolution.
    Useful for finding extent on a particular image grid.

    :param base_extent: is a bbox (xMin, xMax, yMin, yMax) providing the base for the grid on which output will be defined.
    :param base_grid: the size of the (square) grid on which output will be defined.
    :param full_contain: is a boolean. True: moving output onto grid will increase size of bbox (i.e., intersection fully contained)
                                      False: move output onto grid will decrease size of bbox (i.e., bbox fully contained within intesection)
    :param round_vals: specify whether outputted values should be rounded. None for no rounding (default) or integer for number of
                       significant figures to round to.

    :return: bbox (xMin, xMax, yMin, yMax)

    """
    xMin = base_extent[0]
    xMax = base_extent[1]
    yMin = base_extent[2]
    yMax = base_extent[3]

    nPxlXMin = math.floor(xMin / base_grid)
    nPxlYMin = math.floor(yMin / base_grid)

    xMinOut = nPxlXMin * base_grid
    yMinOut = nPxlYMin * base_grid

    diffX = xMax - xMinOut
    diffY = abs(yMax - yMinOut)

    nPxlX = 0.0
    nPxlY = 0.0
    if full_contain:
        nPxlX = math.ceil(diffX / base_grid)
        nPxlY = math.ceil(diffY / base_grid)
    else:
        nPxlX = math.floor(diffX / base_grid)
        nPxlY = math.floor(diffY / base_grid)

    xMaxOut = xMinOut + (nPxlX * base_grid)
    yMaxOut = yMinOut + (nPxlY * base_grid)

    if round_vals is None:
        out_bbox = [xMinOut, xMaxOut, yMinOut, yMaxOut]
    else:
        out_bbox = [
            round(xMinOut, round_vals),
            round(xMaxOut, round_vals),
            round(yMinOut, round_vals),
            round(yMaxOut, round_vals),
        ]
    return out_bbox


def get_bbox_grid(bbox: List[float], x_size: int, y_size: int) -> List[List[float]]:
    """
    Create a grid with size x_size, y_size for the area represented by bbox.

    :param bbox: a bounding box within which the grid will be created (xMin, xMax, yMin, yMax)
    :param x_size: Output grid size in X axis (same unit as bbox).
    :param y_size: Output grid size in Y axis (same unit as bbox).

    :return: list of bounding boxes (xMin, xMax, yMin, yMax)

    """
    width = bbox[1] - bbox[0]
    height = bbox[3] - bbox[2]

    n_tiles_x = math.floor(width / x_size)
    n_tiles_y = math.floor(height / y_size)

    if (n_tiles_x > 10000) or (n_tiles_y > 10000):
        print(
            "WARNING: did you mean to product so many tiles (X: {}, Y: {}) "
            "might want to check your units".format(n_tiles_x, n_tiles_y)
        )

    full_tile_width = n_tiles_x * x_size
    full_tile_height = n_tiles_y * y_size

    x_remain = width - full_tile_width
    if x_remain < 0.000001:
        x_remain = 0.0
    y_remain = height - full_tile_height
    if y_remain < 0.000001:
        y_remain = 0.0

    c_min_y = bbox[2]
    c_max_y = c_min_y + y_size

    bboxs = list()
    for ny in range(n_tiles_y):
        c_min_x = bbox[0]
        c_max_x = c_min_x + x_size
        for nx in range(n_tiles_x):
            bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            c_min_x = c_max_x
            c_max_x = c_max_x + x_size
        if x_remain > 0:
            c_max_x = c_min_x + x_remain
            bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
        c_min_y = c_max_y
        c_max_y = c_max_y + y_size
    if y_remain > 0:
        c_max_y = c_min_y + y_remain
        c_min_x = bbox[0]
        c_max_x = c_min_x + x_size
        for nx in range(n_tiles_x):
            bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])
            c_min_x = c_max_x
            c_max_x = c_max_x + x_size
        if x_remain > 0:
            c_max_x = c_min_x + x_remain
            bboxs.append([c_min_x, c_max_x, c_min_y, c_max_y])

    return bboxs


def get_bbox_geojson_poly(bbox: List) -> Dict:
    """
    Get the bbox (xMin, xMax, yMin, yMax) represented as a GeoJSON polygon
    using dict and list.

    :param bbox: (xMin, xMax, yMin, yMax)
    :return: a dict with the geojson polygon representation of the bbox.

    """
    bbox_poly = dict()
    bbox_poly["coordinates"] = list()
    bbox_poly["coordinates"].append([])
    bbox_poly["coordinates"][0].append([bbox[0], bbox[3]])  # TL
    bbox_poly["coordinates"][0].append([bbox[1], bbox[3]])  # TR
    bbox_poly["coordinates"][0].append([bbox[1], bbox[2]])  # BR
    bbox_poly["coordinates"][0].append([bbox[0], bbox[2]])  # BL
    bbox_poly["coordinates"][0].append([bbox[0], bbox[3]])  # TL
    bbox_poly["type"] = "Polygon"

    return bbox_poly


def get_bbox_centre_pt(bbox: List) -> List:
    """
    Function which returns the centre point of a bbox (xMin, xMax, yMin, yMax)

    :param bbox: the bbox (xMin, xMax, yMin, yMax)
    :return: the centre point [xPt, yPt]
    """
    x_pt = bbox[0] + (bbox[1] - bbox[0]) / 2
    y_pt = bbox[2] + (bbox[3] - bbox[2]) / 2
    return [x_pt, y_pt]


def reproj_point(
    in_osr_proj_obj: osr.SpatialReference(),
    out_osr_proj_obj: osr.SpatialReference(),
    x: float,
    y: float,
) -> (float, float):
    """
    Reproject a point from 'in_osr_proj_obj' to 'out_osr_proj_obj' where they are gdal
    osgeo.osr.SpatialReference objects.

    :param in_osr_proj_obj: an osr.SpatialReference() object representing input
                            projection.
    :param out_osr_proj_obj: an osr.SpatialReference() object representing output
                             projection.
    :param x: the x coordinate to be reprojected
    :param y: the y coordinate to be reprojected
    :return: x, y.
    """
    if in_osr_proj_obj.EPSGTreatsAsLatLong():
        wktPt = f"POINT({y} {x})"
    else:
        wktPt = f"POINT({x} {y})"
    point = ogr.CreateGeometryFromWkt(wktPt)
    point.AssignSpatialReference(in_osr_proj_obj)
    point.TransformTo(out_osr_proj_obj)
    if out_osr_proj_obj.EPSGTreatsAsLatLong():
        outX = point.GetY()
        outY = point.GetX()
    else:
        outX = point.GetX()
        outY = point.GetY()
    return outX, outY


def reproj_point_to_wgs84(
    osr_proj_obj: osr.SpatialReference(), x: float, y: float
) -> (float, float):
    """
    A function which reprojects a point to the WGS84 projection

    :param osr_proj_obj: an osr.SpatialReference() object representing input projection.
    :param x: the x coordinate to be reprojected
    :param y: the y coordinate to be reprojected
    :return: lon, lat

    """
    import rsgislib.tools.projection

    out_osr_proj_obj = rsgislib.tools.projection.get_osr_prj_obj(4326)
    return reproj_point(osr_proj_obj, out_osr_proj_obj, x, y)


def calc_pt_distance(x1: float, y1: float, x2: float, y2: float) -> float:
    """
    Calculate the euclidean distance between two points

    :param x1: x1 coordinate
    :param y1: y1 coordinate
    :param x2: x2 coordinate
    :param y2: y2 coordinate
    :return: euclidean distance

    """
    return math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)


def pt_in_bbox(pt: List[float], bbox: List[float]) -> bool:
    """
    A function which tests whether a point is within a bbox.

    :param pt: the point (x, y)
    :param bbox: the bounding box (MinX, MaxX, MinY, MaxY)
    :return: returns a boolean specifying whether the point is within the bbox

    """
    return (
        (pt[0] >= bbox[0])
        and (pt[0] <= bbox[1])
        and (pt[1] >= bbox[2])
        and (pt[1] <= bbox[3])
    )


def find_point_on_whole_num_grid(
    pt: List[float],
    bbox: List[float],
    x_grid_res: float,
    y_grid_res: float,
) -> List[float]:
    """
    A function which returns places a point on to the grid defined by the
    bbox, base_x_grid and  base_y_grid

    :param pt: the point (x, y)
    :param bbox: the bounding box for the whole region defining the grid extent
                 (MinX, MaxX, MinY, MaxY)
    :param x_grid_res: the resolution of the grid in x axis
    :param y_grid_res: the resolution of the grid in y axis
    :return: returns a point (x, y) which now on the defined grid.

    """
    if not pt_in_bbox(pt, bbox):
        raise rsgislib.RSGISPyException("Point is not within the bbox!")

    diff_x = pt[0] - bbox[0]
    diff_y = bbox[3] - pt[1]

    n_pxl_x = math.ceil(diff_x / x_grid_res)
    n_pxl_y = math.ceil(diff_y / y_grid_res)

    out_pt = [(bbox[0] + (n_pxl_x * x_grid_res)), (bbox[3] - (n_pxl_y * y_grid_res))]
    return out_pt
