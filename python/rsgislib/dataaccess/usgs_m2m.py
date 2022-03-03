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

def usgs_login(username:str=None, password:str=None)->str:
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

    login_data = {'username': username, 'password': password}
    login_url = USGS_M2M_URL + "login"
    api_key = rsgislib.tools.httptools.send_http_json_request(login_url, login_data)
    return api_key

def usgs_logout(api_key:str):
    """

    :param api_key:

    """
    logout_url = USGS_M2M_URL + "logout"
    rsgislib.tools.httptools.send_http_json_request(logout_url, None, api_key)

def usgs_search(dataset:str,
                api_key:str,
                start_date:datetime.datetime,
                end_date:datetime.datetime=None,
                cloud_min:int=0,
                cloud_max:int=None,
                pt:List=None,
                bbox:List=None,
                poly_geom:str=None,
                sort_field:str=None,
                sort_direct:str="DESC")->Dict:
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
    :param sort_field:
    :param sort_direct:
    :return:
    """

    if dataset not in DATA_PRODUCTS:
        raise rsgislib.RSGISPyException(f"No not recognise dataset: {dataset}")

    search_data = dict()
    search_data["datasetName"] = dataset

    if end_date is None:
        end_date = datetime.datetime.now()

    search_data["sceneFilter"] = dict()
    search_data["sceneFilter"]["acquisitionFilter"] = dict()
    search_data["sceneFilter"]["acquisitionFilter"]["start"] = "{}".format(start_date)
    search_data["sceneFilter"]["acquisitionFilter"]["end"] = "{}".format(end_date)

    if cloud_max is not None:
        search_data["sceneFilter"]["cloudCoverFilter"] = dict()
        search_data["sceneFilter"]["cloudCoverFilter"]["min"] = cloud_min
        search_data["sceneFilter"]["cloudCoverFilter"]["max"] = cloud_max

    if pt is not None:
        print("Use point to filter.")
        search_data["sceneFilter"]["spatialFilter"] = dict()
        search_data["sceneFilter"]["spatialFilter"]["filterType"] = "geoJson"
        search_data["sceneFilter"]["spatialFilter"]["geoJson"] = dict()
        search_data["sceneFilter"]["spatialFilter"]["geoJson"]["type"] = "point"
        search_data["sceneFilter"]["spatialFilter"]["geoJson"]["coordinates"] = [{"longitude": pt[0], "latitude": pt[1]}]
    elif bbox is not None:
        print("Use bbox to filter.")
        search_data["sceneFilter"]["spatialFilter"] = dict()
        search_data["sceneFilter"]["spatialFilter"]["filterType"] = "mbr"
        search_data["sceneFilter"]["spatialFilter"]["lowerLeft"] = {"longitude": bbox[0], "latitude": bbox[2]}
        search_data["sceneFilter"]["spatialFilter"]["upperRight"] = {"longitude": bbox[1], "latitude": bbox[3]}
    elif poly_geom is not None:
        print("Use poly_geom to filter.")
        search_data["sceneFilter"]["spatialFilter"] = dict()
        search_data["sceneFilter"]["spatialFilter"]["filterType"] = "geoJson"
        search_data["sceneFilter"]["spatialFilter"]["geoJson"] = dict()
        search_data["sceneFilter"]["spatialFilter"]["geoJson"]["type"] = "polygon"
        #self["geoJson"] = GeoJson(shape)

    if sort_field is not None:
        search_data["sortCustomization"] = dict()
        search_data["sortCustomization"]["field_name"] = sort_field
        search_data["sortCustomization"]["direction"] = sort_direct


    import pprint
    pprint.pprint(search_data)

    search_url = USGS_M2M_URL + "scene-search"
    out_info = rsgislib.tools.httptools.send_http_json_request(search_url, search_data, api_key=api_key)

    n_scns = out_info["totalHits"]

    print(n_scns)