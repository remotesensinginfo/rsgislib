#!/usr/bin/env python
"""
Tools for accessing (searching and downloading) datasets from the USGS

"""

import datetime
from typing import List, Dict

import rsgislib
import rsgislib.tools.utils
import rsgislib.tools.httptools

DATA_PRODUCTS = {
    "landsat_tm_c1": "5e83d08fd9932768",
    "landsat_etm_c1": "5e83a507d6aaa3db",
    "landsat_8_c1": "5e83d0b84df8d8c2",
    "landsat_tm_c2_l1": "5e83d0a0f94d7d8d",
    "landsat_etm_c2_l1": "5e83d0d0d2aaa488",
    "landsat_ot_c2_l1": "5e81f14ff4f9941c",
    "landsat_tm_c2_l2": "5e83d11933473426",
    "landsat_etm_c2_l2": "5e83d12aada2e3c5",
    "landsat_ot_c2_l2": "5e83d14f30ea90a9",
    "sentinel_2a": "5e83a42c6eba8084",
}

USGS_M2M_URL = "https://m2m.cr.usgs.gov/api/api/json/stable/"

# Note to download data you need to registered for m2m access:
# https://ers.cr.usgs.gov/profile/access


def usgs_login(username: str = None, password: str = None) -> str:
    """
    A function to login to the USGS m2m service.

    :param username: Your username for USGS EarthExplorer. If RSGIS_USGS_USER
                     environmental variable is specified then username will
                     read from there is None is passed (Default: None)
    :param password: Your password for USGS EarthExplorer. If RSGIS_USGS_PASS
                     environmental variable is specified then password will
                     read from there is None is passed (Default: None)
    :return: the API key for the USGS session.
    """
    if username is None:
        username = rsgislib.tools.utils.get_environment_variable("RSGIS_USGS_USER")

    if password is None:
        password = rsgislib.tools.utils.get_environment_variable("RSGIS_USGS_PASS")

    login_data = {"username": username, "password": password}
    login_url = USGS_M2M_URL + "login"
    api_key = rsgislib.tools.httptools.send_http_json_request(login_url, login_data)
    return api_key


def usgs_logout(api_key: str):
    """

    :param api_key:

    """
    logout_url = USGS_M2M_URL + "logout"
    rsgislib.tools.httptools.send_http_json_request(logout_url, None, api_key)


def usgs_search(
    dataset: str,
    api_key: str,
    start_date: datetime.datetime = None,
    end_date: datetime.datetime = None,
    cloud_min: int = 0,
    cloud_max: int = None,
    pt: List = None,
    bbox: List = None,
    poly_geom: str = None,
    months: List[int] = None,
    full_meta: bool = False,
    max_n_rslts: int = None,
    start_n: int = None,
) -> (List, Dict):
    """

    :param dataset:
    :param api_key:
    :param start_date:
    :param end_date:
    :param cloud_min:
    :param cloud_max:
    :param bbox: (MinX, MaxX, MinY, MaxY)
    :param pt: (X, Y)
    :param poly_geom:
    :param months:
    :param max_n_rslts:
    :param start_n:
    :return:

    """
    if dataset not in DATA_PRODUCTS:
        raise rsgislib.RSGISPyException(f"No not recognise dataset: {dataset}")

    if (max_n_rslts is not None) and (max_n_rslts > 100):
        raise rsgislib.RSGISPyException(
            "The maximum number of results requested should not be higher than 100."
        )

    search_data = dict()
    search_data["datasetName"] = dataset

    if end_date is None:
        end_date = datetime.datetime.now()

    scn_filter = dict()

    if start_date is not None:
        scn_filter["acquisitionFilter"] = dict()
        scn_filter["acquisitionFilter"]["start"] = "{}".format(start_date)
        scn_filter["acquisitionFilter"]["end"] = "{}".format(end_date)

    if cloud_max is not None:
        scn_filter["cloudCoverFilter"] = dict()
        scn_filter["cloudCoverFilter"]["min"] = cloud_min
        scn_filter["cloudCoverFilter"]["max"] = cloud_max

    if pt is not None:
        scn_filter["spatialFilter"] = dict()
        scn_filter["spatialFilter"]["filterType"] = "geojson"
        scn_filter["spatialFilter"]["geoJson"] = dict()
        scn_filter["spatialFilter"]["geoJson"]["type"] = "Point"
        scn_filter["spatialFilter"]["geoJson"]["coordinates"] = [pt[1], pt[0]]
        # raise rsgislib.RSGISPyException("Trying to point to filter but not implemented yet.")
    elif bbox is not None:
        print("Use bbox to filter.")
        scn_filter["spatialFilter"] = dict()
        scn_filter["spatialFilter"]["filterType"] = "mbr"
        scn_filter["spatialFilter"]["lowerLeft"] = {
            "longitude": bbox[0],
            "latitude": bbox[2],
        }
        scn_filter["spatialFilter"]["upperRight"] = {
            "longitude": bbox[1],
            "latitude": bbox[3],
        }
    elif poly_geom is not None:
        """
        scn_filter["spatialFilter"] = dict()
        scn_filter["spatialFilter"]["filterType"] = "geoJson"
        scn_filter["spatialFilter"]["geoJson"] = dict()
        scn_filter["spatialFilter"]["geoJson"]["type"] = "Polygon"
        #self["geoJson"] = GeoJson(shape)
        """
        raise rsgislib.RSGISPyException(
            "Trying to use a polygon to filter but not implemented yet."
        )

    if months is not None:
        scn_filter["seasonalFilter"] = dict()
        scn_filter["seasonalFilter"] = months

    if len(scn_filter) > 0:
        search_data["sceneFilter"] = scn_filter

    if full_meta:
        search_data["metadataType"] = "full"

    if max_n_rslts is not None:
        search_data["maxResults"] = max_n_rslts

    if start_n is not None:
        search_data["startingNumber"] = start_n

    # import pprint
    # pprint.pprint(search_data)

    search_url = USGS_M2M_URL + "scene-search"
    srch_data = rsgislib.tools.httptools.send_http_json_request(
        search_url, search_data, api_key=api_key
    )

    if srch_data["totalHits"] > 0:
        scns = srch_data["results"]
    else:
        scns = None

    srch_meta = {
        "totalHits": srch_data["totalHits"],
        "startingNumber": srch_data["startingNumber"],
        "nextRecord": srch_data["nextRecord"],
        "recordsReturned": srch_data["recordsReturned"],
    }

    return scns, srch_meta


def get_all_usgs_search(
    dataset: str,
    api_key: str,
    max_n_rslts: int = 1000,
    start_date: datetime.datetime = None,
    end_date: datetime.datetime = None,
    cloud_min: int = 0,
    cloud_max: int = None,
    pt: List = None,
    bbox: List = None,
    poly_geom: str = None,
    months: List[int] = None,
    full_meta: bool = False,
) -> List:
    """

    :param dataset:
    :param api_key:
    :param max_n_rslts:
    :param start_date:
    :param end_date:
    :param cloud_min:
    :param cloud_max:
    :param pt:
    :param bbox:
    :param poly_geom:
    :param months:
    :param full_meta:
    :return:

    """
    if max_n_rslts <= 100:
        scns, srch_meta = usgs_search(
            dataset,
            api_key,
            start_date,
            end_date,
            cloud_min,
            cloud_max,
            pt,
            bbox,
            poly_geom,
            months,
            full_meta,
        )
    else:
        scns, srch_meta = usgs_search(
            dataset,
            api_key,
            start_date,
            end_date,
            cloud_min,
            cloud_max,
            pt,
            bbox,
            poly_geom,
            months,
            full_meta,
        )
        tot_n_scns = srch_meta["recordsReturned"]
        total_hits = srch_meta["totalHits"]
        if max_n_rslts > total_hits:
            max_n_rslts = total_hits
        while tot_n_scns <= max_n_rslts:
            xtr_scns, srch_meta = usgs_search(
                dataset,
                api_key,
                start_date,
                end_date,
                cloud_min,
                cloud_max,
                pt,
                bbox,
                poly_geom,
                months,
                full_meta,
                start_n=srch_meta["nextRecord"],
            )
            scns += xtr_scns
            tot_n_scns += len(xtr_scns)

    if len(scns) > max_n_rslts:
        scns = scns[0:max_n_rslts]

    return scns
