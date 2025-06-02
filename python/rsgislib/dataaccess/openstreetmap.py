"""
Tools for downloading openstreetmap (OSM) data
"""

from typing import Union, Tuple, List, Dict
import rsgislib

OSMNX_AVAIL = True
try:
    import osmnx
except ImportError:
    OSMNX_AVAIL = False

GEOPANDAS_AVAIL = True
try:
    import geopandas
except ImportError:
    GEOPANDAS_AVAIL = False


def get_osm_geom_type_name(geom_type: int) -> str:
    """
    A function which returns a string with a human-readable name of the
    geometry type.

    :param geom_type: the numerical type (e.g., rsgislib.GEOM_POLY)
    :return: the name of the geometry type

    """
    geom_type_name = ""
    if geom_type == rsgislib.GEOM_POLY:
        geom_type_name = "Polygon"
    elif geom_type == rsgislib.GEOM_PT:
        geom_type_name = "Point"
    elif geom_type == rsgislib.GEOM_LINE:
        geom_type_name = "LineString"
    else:
        raise Exception(f"Do not recognise Geometry Type: '{geom_type}'")
    return geom_type_name


def get_osm_gdf(
    bbox: Union[Tuple[float, float, float, float], List[float]],
    osm_tags: Dict[str, Union[bool, str]],
    rsgis_geom_type: int = rsgislib.GEOM_POLY,
    bbox_epsg: int = 4326,
    clip_to_roi: bool = False,
) -> geopandas.GeoDataFrame:
    """
    A function to downloads OpenStreetMap data into a geopandas GeoDataFrame for
    specified tags and geometry type. For information on the OSM tags, which you need
    to specify for the download see here: https://wiki.openstreetmap.org/wiki/Tags.

    You might also find https://wiki.openstreetmap.org/wiki/Map_features useful as
    these are the most common tags you might have to download. It is recommended to
    use the higher level tags such as 'building' (rsgislib.GEOM_POLY),
    'highway' (rsgislib.GEOM_LINE), 'railway' (rsgislib.GEOM_LINE),
    'ferry_terminal' (rsgislib.GEOM_PT), 'aeroway' (rsgislib.GEOM_PT),

    :param bbox: is a bbox (xMin, xMax, yMin, yMax) in defining the region
                 of interest. Ideally in EPSG:4326 but will reproject if bbox_epsg
                 is correctly defined.
    :param osm_tags: The OSM tags to download (e.g., {"building": True},
                     {"highway": True}, {"railway": True},
                     {"amenity": "ferry_terminal"}, {"aeroway": True})
    :param rsgis_geom_type: The geometry type for the data of interest (e.g.,
                            rsgislib.GEOM_PT, rsgislib.GEOM_LINE, rsgislib.GEOM_POLY)
    :param bbox_epsg: the EPSG code of the bounding box - define is 4326.
    :param clip_to_roi: Open to clip the OSM output to the bbox specified for the
                        region of interest. (Default: False)
    :return: a geopandas GeoDataFrame with the OSM data - note this dataframe could
             be empty if there is no data for the bbox for the tags and geometry type
             specified.

    """
    import rsgislib
    import rsgislib.tools.geometrytools

    if not OSMNX_AVAIL:
        raise rsgislib.RSGISPyException(
            "osmnx module is not available, "
            "please install: https://osmnx.readthedocs.io"
        )
    if not GEOPANDAS_AVAIL:
        raise rsgislib.RSGISPyException(
            "geopandas module is not available, " "please install"
        )

    try:
        geom_type_str = get_osm_geom_type_name(rsgis_geom_type)

        if bbox_epsg != 4326:
            wgs84_bbox = rsgislib.tools.geometrytools.reproj_bbox_epsg(
                bbox, in_epsg=bbox_epsg, out_epsg=4326
            )
        else:
            wgs84_bbox = bbox

        # RSGIS Order: West, East, South, North
        # OSM Order: left, bottom, right, top
        osm_bbox = (wgs84_bbox[0], wgs84_bbox[2], wgs84_bbox[1], wgs84_bbox[3])
        data_gdf = osmnx.features.features_from_bbox(bbox=osm_bbox, tags=osm_tags)

        data_gdf = data_gdf.loc[:, data_gdf.columns.str.contains("addr:|geometry")]
        data_gdf = data_gdf.loc[data_gdf.geometry.type == geom_type_str]

        if clip_to_roi:
            data_gdf = data_gdf.clip(osm_bbox)

        return data_gdf
    except Exception as e:
        print(f"Error: Unable to download OSM {osm_tags}")
        raise e


def get_osm_to_file(
    out_vec_file: str,
    out_vec_lyr: str,
    bbox: Union[Tuple[float, float, float, float], List[float]],
    osm_tags: Dict[str, Union[bool, str]],
    rsgis_geom_type: int = rsgislib.GEOM_POLY,
    bbox_epsg: int = 4326,
    out_format: str = "GPKG",
    out_vec_epsg: int = 4326,
    clip_to_roi: bool = False,
):
    """
    A function to downloads OpenStreetMap data and save it to a vector file for
    specified tags and geometry type. For information on the OSM tags, which you need
    to specify for the download see here: https://wiki.openstreetmap.org/wiki/Tags.

    You might also find https://wiki.openstreetmap.org/wiki/Map_features useful as
    these are the most common tags you might have to download. It is recommended to
    use the higher level tags such as 'building' (rsgislib.GEOM_POLY),
    'highway' (rsgislib.GEOM_LINE), 'railway' (rsgislib.GEOM_LINE),
    'ferry_terminal' (rsgislib.GEOM_PT), 'aeroway' (rsgislib.GEOM_PT),

    :param bbox: is a bbox (xMin, xMax, yMin, yMax) in defining the region
                 of interest. Ideally in EPSG:4326 but will reproject if bbox_epsg
                 is correctly defined.
    :param osm_tags: The OSM tags to download (e.g., {"building": True},
                     {"highway": True}, {"railway": True},
                     {"amenity": "ferry_terminal"}, {"aeroway": True})
    :param rsgis_geom_type: The geometry type for the data of interest (e.g.,
                            rsgislib.GEOM_PT, rsgislib.GEOM_LINE, rsgislib.GEOM_POLY)
    :param bbox_epsg: the EPSG code of the bounding box - define is 4326.
    :param clip_to_roi: Open to clip the OSM output to the bbox specified for the
                        region of interest. (Default: False)
    :return: a geopandas GeoDataFrame with the OSM data - note this dataframe could
             be empty if there is no data for the bbox for the tags and geometry type
             specified.

    
    """

    data_gdf = get_osm_gdf(
        bbox=bbox,
        osm_tags=osm_tags,
        rsgis_geom_type=rsgis_geom_type,
        bbox_epsg=bbox_epsg,
        clip_to_roi=clip_to_roi,
    )
    if out_vec_epsg != 4326:
        data_gdf = data_gdf.to_crs(epsg=out_vec_epsg)

    if len(data_gdf) > 0:
        if out_format == "GPKG":
            data_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
        else:
            data_gdf.to_file(out_vec_file, driver=out_format)
    else:
        print("The geodataframe was empty - check the geometry type, tags and ROI.")
