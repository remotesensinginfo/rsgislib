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
    "landsat_mss_c2_l1": "5e83d0e09a752cff",
    "landsat_tm_c2_l2": "5e83d11933473426",
    "landsat_etm_c2_l2": "5e83d12aada2e3c5",
    "landsat_ot_c2_l2": "5e83d14f30ea90a9",
    "sentinel_2a": "5e83a42c6eba8084",
}

USGS_WRS1 = 1
USGS_WRS2 = 2


USGS_M2M_URL = "https://m2m.cr.usgs.gov/api/api/json/stable/"

# Note to download data you need to registered for m2m access:
# https://ers.cr.usgs.gov/profile/access

def _check_usgs_response(data:Dict)->Dict:
    """
    A function which checks the response for an error (producing an exception)
    and extracts the text output and returns it.

    :param data:
    :return:
    """

    if rsgislib.tools.utils.dict_struct_does_path_exist(data, ["errorCode"]) and data["errorCode"] != None:
        raise rsgislib.RSGISPyException(
            "{} - {}".format(data["errorCode"], data["errorMessage"])
            )

    if not rsgislib.tools.utils.dict_struct_does_path_exist(data, ["data"]):
        print(data)
        raise rsgislib.RSGISPyException("Data structure is not as expected - check.")

    return data["data"]


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
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(login_url, login_data)
    api_key = _check_usgs_response(rtnd_data)
    return api_key


def usgs_logout(api_key: str):
    """
    Log out of the USGS m2m system using the api_key created at login.
    :param api_key: The API key created at login to authenticate.

    """
    logout_url = USGS_M2M_URL + "logout"
    rsgislib.tools.httptools.send_http_json_request(logout_url, None, api_key)


def can_user_dwnld(api_key: str) -> bool:
    """
    Does the user logged in with the api_key have permission to download data.

    :param api_key: The API key created at login to authenticate.
    :return: boolean - True does have permission.

    """
    permissions_url = USGS_M2M_URL + "permissions"
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        permissions_url, api_key=api_key
    )
    permissions_info = _check_usgs_response(rtnd_data)

    have_dwn_per = False
    if (permissions_info is not None) and ("download" in permissions_info):
        have_dwn_per = True
    return have_dwn_per


def can_user_order(api_key: str) -> bool:
    """
    Does the user logged in with the api_key have permission to order data.

    :param api_key: The API key created at login to authenticate.
    :return: boolean - True does have permission.
    """
    permissions_url = USGS_M2M_URL + "permissions"
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        permissions_url, api_key=api_key
    )
    permissions_info = _check_usgs_response(rtnd_data)
    have_ord_per = False
    if (permissions_info is not None) and ("order" in permissions_info):
        have_ord_per = True
    return have_ord_per


def get_wrs_pt(
    api_key: str, row: int, path: int, grid_version: int = USGS_WRS2
) -> (float, float):
    """
    Get a point for the WRS row/path which can be used for a query.

    :param api_key: The API key created at login to authenticate.
    :param row: integer for row
    :param path: integer for path
    :param grid_version: Whether the row/path is WRS1 or WRS2. Default: WRS2.
    :return: longitude, latitude

    """
    grid2ll_url = USGS_M2M_URL + "grid2ll"
    grid2ll_data = dict()
    if grid_version == USGS_WRS1:
        grid2ll_data["gridType"] = "WRS1"
    else:
        grid2ll_data["gridType"] = "WRS2"

    grid2ll_data["path"] = path
    grid2ll_data["row"] = row
    grid2ll_data["responseShape"] = "point"

    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        grid2ll_url, grid2ll_data, api_key=api_key
    )
    grid2ll_info = _check_usgs_response(rtnd_data)

    if grid2ll_info["shape"] == "point":
        lat = grid2ll_info["coordinates"][0]["latitude"]
        lon = grid2ll_info["coordinates"][0]["longitude"]
    else:
        raise rsgislib.RSGISPyException("The returned shape was not a point.")

    return lon, lat


def get_wrs_bbox(
    api_key: str, row: int, path: int, grid_version: int = USGS_WRS2
) -> (float, float, float, float):
    """
    Get a bbox for the WRS row/path which can be used for a query.

    :param api_key: The API key created at login to authenticate.
    :param row: integer for row
    :param path: integer for path
    :param grid_version: Whether the row/path is WRS1 or WRS2. Default: WRS2.
    :return: BBOX in lon/lat (x_min, x_max, y_min, y_max)

    """
    grid2ll_url = USGS_M2M_URL + "grid2ll"
    grid2ll_data = dict()
    if grid_version == USGS_WRS1:
        grid2ll_data["gridType"] = "WRS1"
    else:
        grid2ll_data["gridType"] = "WRS2"

    grid2ll_data["path"] = path
    grid2ll_data["row"] = row
    grid2ll_data["responseShape"] = "polygon"

    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        grid2ll_url, grid2ll_data, api_key=api_key
    )
    grid2ll_info = _check_usgs_response(rtnd_data)

    lat_vals = []
    lon_vals = []

    if grid2ll_info["shape"] == "polygon":
        for coord in grid2ll_info["coordinates"]:
            lat_vals.append(coord["latitude"])
            lon_vals.append(coord["longitude"])
    else:
        raise rsgislib.RSGISPyException("The returned shape was not a polygon.")

    min_lon = min(lon_vals)
    max_lon = max(lon_vals)
    min_lat = min(lat_vals)
    max_lat = max(lat_vals)

    return min_lon, max_lon, min_lat, max_lat


def get_dataset_info(dataset: str, api_key: str) -> Dict:
    """
    Get information on a particular dataset.

    :parma dataset: the name of the dataset
    :param api_key: The API key created at login to authenticate.
    :return: dict of dataset info.

    """
    dataset_url = USGS_M2M_URL + "dataset"
    dataset_data = {"datasetName": dataset}
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        dataset_url, dataset_data, api_key=api_key
    )
    dataset_info = _check_usgs_response(rtnd_data)
    return dataset_info


def get_earth_explorer_datasets(api_key: str) -> Dict[str, List[str]]:
    """
    Get a structured dict of datasets available on the system. Structure
    is category and then a list of datasets under each category. Note,
    subcategories have been removed and lists merged.

    :param api_key: The API key created at login to authenticate.
    :return: a dict with lists of dataset names

    """
    dataset_url = USGS_M2M_URL + "dataset-categories"
    dataset_data = {"catalog": "EE"}
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        dataset_url, dataset_data, api_key=api_key
    )
    dataset_info = _check_usgs_response(rtnd_data)

    dataset_names = dict()
    for dataset_id in dataset_info:
        dataset_names[dataset_info[dataset_id]["categoryName"]] = list()
        if "subCategories" in dataset_info[dataset_id]:
            for sub_cat_id in dataset_info[dataset_id]["subCategories"]:
                if type(sub_cat_id) != dict:
                    for dataset_item in dataset_info[dataset_id]["subCategories"][
                        sub_cat_id
                    ]["datasets"]:
                        dataset_names[dataset_info[dataset_id]["categoryName"]].append(
                            dataset_item["datasetAlias"]
                        )
        for dataset_item in dataset_info[dataset_id]["datasets"]:
            dataset_names[dataset_info[dataset_id]["categoryName"]].append(
                dataset_item["datasetAlias"]
            )
    return dataset_names


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
    A function to search for landsat imagery from the USGS.

    :param dataset: The name of the dataset to query.
    :param api_key: The API key created at login to authenticate.
    :param start_date: Start date as a datetime object. (Earlier date)
    :param end_date: End date as a datetime object. (Later date)
    :param cloud_min: Minimum cloud cover (Default: 0)
    :param cloud_max: Maximum cloud cover.
    :param bbox: (MinX, MaxX, MinY, MaxY)
    :param pt: (X, Y)
    :param poly_geom: NOT IMPLEMENTED YET!
    :param months: List of months as ints (1-12) you want to limit the search for.
    :param full_meta: Full metadata returned (Default: False)
    :param max_n_rslts: the maximum number of scenes to be returned (cannot be
                        larger than 100 - if larger than 100 then use
                        get_all_usgs_search function.
    :param start_n: The scene number to start the data retrieval from. Note
                    you probably don't want to use this parameter but use the
                    get_all_usgs_search function.
    :return: List of scenes found and Dict of meta-data for the number of scenes
             available.

    """
    if dataset not in DATA_PRODUCTS:
        all_datasets = get_earth_explorer_datasets(api_key)
        found_dataset = False
        for cat in all_datasets:
            if dataset in all_datasets[cat]:
                found_dataset = True
                break
        if not found_dataset:
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

    search_url = USGS_M2M_URL + "scene-search"
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        search_url, search_data, api_key=api_key
    )
    srch_data = _check_usgs_response(rtnd_data)

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
    Uses the usgs_search function to retrive multiple 'pages' of search results. So, if
    you need more than 100 scenes you can use this function to undertake the multiple
    queries required and merge the results into a single list.

    :param dataset: The name of the dataset to query.
    :param api_key: The API key created at login to authenticate.
    :param max_n_rslts: The maximum number of scenes you want returned.
    :param start_date: Start date as a datetime object. (Earlier date)
    :param end_date: End date as a datetime object. (Later date)
    :param cloud_min: Minimum cloud cover (Default: 0)
    :param cloud_max: Maximum cloud cover.
    :param bbox: (MinX, MaxX, MinY, MaxY)
    :param pt: (X, Y)
    :param poly_geom: NOT IMPLEMENTED YET!
    :param months: List of months as ints (1-12) you want to limit the search for.
    :param full_meta: Full metadata returned (Default: False)
    :return: List of scenes found through the query.

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


def get_download_ids(scns, bulk=False) -> (List, List):
    """
    A function for extracting a list of display and entity IDs from a list
    of scenes as would have been returned by from a search query.

    :param scns: a list of the scenes
    :param bulk: If True then only scenes available for bulk download will be outputted.
    :return: List of display IDs, List of Entity IDs

    """
    scn_dsp_ids = list()
    scn_ent_ids = list()
    for scn in scns:
        if scn["options"]["download"] == True:
            if bulk and (scn["options"]["bulk"] == True):
                scn_dsp_ids.append(scn["displayId"])
                scn_ent_ids.append(scn["entityId"])
            elif not bulk:
                scn_dsp_ids.append(scn["displayId"])
                scn_ent_ids.append(scn["entityId"])
    return scn_dsp_ids, scn_ent_ids


def create_scene_list(
    api_key: str,
    dataset: str,
    scn_ent_ids: List[str],
    lst_name: str,
    lst_period: str = "P1W",
) -> int:
    """
    A function which creates a list of scenes on the system which could be
    downloaded.

    ISO 8601 duration format:
    P(n)Y(n)M(n)DT(n)H(n)M(n)S

    Where:
        P is the duration designator (referred to as "period"), and is always placed
          at the beginning of the duration.
        Y is the year designator that follows the value for the number of years.
        M is the month designator that follows the value for the number of months.
        W is the week designator that follows the value for the number of weeks.
        D is the day designator that follows the value for the number of days.
        T is the time designator that precedes the time components.
        H is the hour designator that follows the value for the number of hours.
        M is the minute designator that follows the value for the number of minutes.
        S is the second designator that follows the value for the number of seconds.

    For example: "P3Y6M4DT12H30M5S" = A duration of three years, six months, four
    days, twelve hours, thirty minutes, and five seconds.

    :param api_key: The API key created at login to authenticate user.
    :param dataset: name of the dataset
    :param scn_ent_ids: list of entity IDs
    :param lst_name: a name for the list - can be anything you want but should
                     be meaningful to you.
    :param lst_period: Period the list will exist for in ISO 8601 duration format.
                       Default is P1W (i.e., 1 week).
    :return: Number of scenes added.

    """
    if dataset not in DATA_PRODUCTS:
        all_datasets = get_earth_explorer_datasets(api_key)
        found_dataset = False
        for cat in all_datasets:
            if dataset in all_datasets[cat]:
                found_dataset = True
                break
        if not found_dataset:
            raise rsgislib.RSGISPyException(f"No not recognise dataset: {dataset}")

    add_scn_info = dict()
    add_scn_info["listId"] = lst_name
    add_scn_info["datasetName"] = dataset
    add_scn_info["idField"] = "entityId"
    add_scn_info["entityIds"] = scn_ent_ids
    add_scn_info["timeToLive"] = lst_period

    scn_lst_add_url = USGS_M2M_URL + "scene-list-add"
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        scn_lst_add_url, add_scn_info, api_key=api_key
    )
    scn_lst_add_info = _check_usgs_response(rtnd_data)
    return scn_lst_add_info


def remove_scene_list(api_key: str, lst_name: str):
    """
    A function to remove a scene list from the system.

    :param api_key: The API key created at login to authenticate user.
    :param lst_name: a name for the list. Defined by create_scene_list.

    """
    rm_scn_info = {"listId": lst_name}
    scn_lst_add_url = USGS_M2M_URL + "scene-list-remove"
    rsgislib.tools.httptools.send_http_json_request(
        scn_lst_add_url, rm_scn_info, api_key=api_key
    )


def check_dwnld_opts(
    api_key: str,
    lst_name: str,
    dataset: str,
    dwnld_filetype: str = "bundle",
    rm_lst: bool = True,
) -> List[Dict[str, str]]:
    """

    :param api_key: The API key created at login to authenticate user.
    :param lst_name: A name for the list - Defined by create_scene_list.
    :param dataset: name of the dataset
    :param dwnld_filetype: What you want to download. Options: bundle, band or all
                           Default: is bundle which will be a tar.gz with all the
                           files for the scene.
    :param rm_lst: bool specifying whether the list should be deleted
                   once the processing has finished.
    :return: returns a list of dicts with the entityId and productId.

    """
    if not can_user_dwnld(api_key):
        raise rsgislib.RSGISPyException(
            "Your user account does not have permission to download data."
        )

    dwnld_opts_params = {"listId": lst_name, "datasetName": dataset}

    dwnld_opts_url = USGS_M2M_URL + "download-options"
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        dwnld_opts_url, dwnld_opts_params, api_key
    )
    dnwld_opt_out_info = _check_usgs_response(rtnd_data)

    # Select products
    dwlds_lst = []
    if dwnld_filetype == "bundle":
        # select bundle files
        for prod in dnwld_opt_out_info:
            if prod["bulkAvailable"]:
                dwlds_lst.append(
                    {"entityId": prod["entityId"], "productId": prod["id"]}
                )
    elif dwnld_filetype == "band":
        # select band files
        for prod in dnwld_opt_out_info:
            if (prod["secondaryDownloads"] is not None) and (
                len(prod["secondaryDownloads"]) > 0
            ):
                for sec_dwnld in prod["secondaryDownloads"]:
                    if sec_dwnld["bulkAvailable"]:
                        dwlds_lst.append(
                            {
                                "entityId": sec_dwnld["entityId"],
                                "productId": sec_dwnld["id"],
                            }
                        )
    elif dwnld_filetype == "all":
        # select all available files
        for prod in dnwld_opt_out_info:
            if prod["bulkAvailable"]:
                dwlds_lst.append(
                    {"entityId": prod["entityId"], "productId": prod["id"]}
                )
                if (prod["secondaryDownloads"] is not None) and (
                    len(prod["secondaryDownloads"]) > 0
                ):
                    for sec_dwnld in prod["secondaryDownloads"]:
                        if sec_dwnld["bulkAvailable"]:
                            dwlds_lst.append(
                                {
                                    "entityId": sec_dwnld["entityId"],
                                    "productId": sec_dwnld["id"],
                                }
                            )
    else:
        raise rsgislib.RSGISPyException(
            "dwnld_filetype is not recognised - must be either bundle, band or all."
        )

    if rm_lst:
        remove_scene_list(api_key, lst_name)

    return dwlds_lst


def request_downloads(api_key: str, dwlds_lst: str, dwnld_label: str) -> (Dict, List):
    """
    A function to request download URLs for a download list (from check_dwnld_opts)

    :param api_key: The API key created at login to authenticate user.
    :param dwlds_lst: The available download list created by check_dwnld_opts.
    :param dwnld_label: A name for the download - can be anything you want but should
                        be meaningful to you.
    :return: a dict of download IDs and URL which are ready to be downloaded and
             a list of downloads IDs for scenes which are being prepared for download.

    """
    avail_dwn_urls = dict()
    dwnld_reqst_url = USGS_M2M_URL + "download-request"
    dwnld_reqst_data = {
        "downloads": dwlds_lst,
        "label": dwnld_label,
        "returnAvailable": True,
    }
    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        dwnld_reqst_url, dwnld_reqst_data, api_key=api_key
    )
    dwnld_reqst_info = _check_usgs_response(rtnd_data)

    for dwnld_info in dwnld_reqst_info["availableDownloads"]:
        avail_dwn_urls[dwnld_info["downloadId"]] = dwnld_info["url"]

    prep_dwnld_count = len(dwnld_reqst_info["preparingDownloads"])
    prep_dwnld_ids = list()
    if prep_dwnld_count > 0:
        for dwnld_info in dwnld_reqst_info["preparingDownloads"]:
            prep_dwnld_ids.append(dwnld_info["downloadId"])

    return avail_dwn_urls, prep_dwnld_ids
