#!/usr/bin/env python
"""
Tools for accessing (searching and downloading) datasets from the NASA
EOSDIS Common Metadata Repository API (https://cmr.earthdata.nasa.gov/search/)

"""

import datetime
from typing import Union, List, Dict
import json
import pprint

import rsgislib
import rsgislib.tools.utils
import rsgislib.tools.httptools

#CMR_OPS = "https://cmr.earthdata.nasa.gov/search/"
#CMR_UAT = "https://cmr.uat.earthdata.nasa.gov/search/"
#CMR_SIT = "https://cmr.sit.earthdata.nasa.gov/search/"

CMR_COLLECTS_URL = "https://cmr.earthdata.nasa.gov/search/collections.json"
CMR_GRANULES_URL = "https://cmr.earthdata.nasa.gov/search/granules.json"


def _check_cmr_response(data: Dict) -> Union[str, List, Dict]:
    """
    A function which checks the response for an error (producing an exception)
    and extracts the text output and returns it.

    :param data:
    :return:
    """

    if not rsgislib.tools.utils.dict_struct_does_path_exist(data, ["feed", "entry"]):
        print(data)
        raise rsgislib.RSGISPyException("Data structure is not as expected - check.")

    return data["feed"]["entry"]


def get_prods_info(prod_short_name: str) -> List[Dict]:
    """
    A function which returns information for a product available from the CMR.

    :param prod_short_name: The name of the product you are interested in.
    :return: A list of products (probably different versions).

    """
    prod_srch_params = {"short_name": prod_short_name}
    header_info = {"Accept": "application/json"}

    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        CMR_COLLECTS_URL,
        data=prod_srch_params,
        convert_to_json=False,
        header_data=header_info,
    )
    prod_lst = _check_cmr_response(rtnd_data)

    return prod_lst


def check_prod_version_avail(prod_short_name: str, version: str) -> bool:
    """

    :param prod_short_name:
    :param version:
    :return:
    """
    prod_lst = get_prods_info(prod_short_name)
    found_version = False
    for prod in prod_lst:
        if prod["version_id"] == version:
            found_version = True
            break
    return found_version


def get_max_prod_version(prod_short_name: str) -> str:
    """

    :param prod_short_name:
    :return:
    """
    prod_lst = get_prods_info(prod_short_name)
    first = True
    max_version_int = 0
    max_version = ""
    for prod in prod_lst:
        version_int = int(prod["version_id"])
        if first:
            max_version_int = version_int
            max_version = prod["version_id"]
            first = False
        elif version_int > max_version_int:
            max_version_int = version_int
            max_version = prod["version_id"]
    return max_version


def find_granules(
    prod_short_name: str,
    version: str,
    only_dnwld: bool = True,
    bbox: List[float] = None,
    pt: List[float] = None,
    start_date: datetime.datetime = None,
    end_date: datetime.datetime = None,
    cloud_min: int = 0,
    cloud_max: int = None,
    sort_date: bool = True,
    sort_desc: bool = True,
    page_size: int = 100,
    page_num: int = 1,
    other_params: Dict[str, str] = None,
):
    """
    A function which will find granules from the CMR system for the product of
    interest using the search parameters provided.

    :param prod_short_name:
    :param version:
    :param only_dnwld:
    :param bbox: (MinX, MaxX, MinY, MaxY)
    :param pt: (X, Y)
    :param start_date: Start date as a datetime object. (Earlier date)
    :param end_date: End date as a datetime object. (Later date)
    :param cloud_min: Minimum cloud cover (Default: 0)
    :param cloud_max: Maximum cloud cover.
    :param sort_date: Sort the response by the acquisition date
    :param sort_desc: Sort order (ascending or descending). Ascending: oldest version.
                      Descending: newest version.
    :param page_size: The number of records to be returned by a single query as a
                      'page'.
    :param page_num: The page number to be retrieved allowing results greater than
                     the number which will fit on a single page to be retrieved.
    :param other_params: A dict of other parameters where the key is the search
                         parameter name and the value is the value to search with.
    :return:
    """
    # https://cmr.earthdata.nasa.gov/search/site/docs/search/api.html#granule-search-by-parameters

    prod_srch_params = {"short_name": prod_short_name, "version": version}
    header_info = {"Accept": "application/json"}

    prod_srch_params["page_size"] = page_size
    prod_srch_params["page_num"] = page_num

    if only_dnwld:
        prod_srch_params["downloadable"] = True

    if (start_date is not None) or (end_date is not None):
        start_date_str = ""
        if start_date is not None:
            start_date_str = start_date.strftime("%Y-%m-%dT00:00:00Z")

        end_date_str = ""
        if end_date is not None:
            end_date_str = end_date.strftime("%Y-%m-%dT00:00:00Z")

        prod_srch_params["temporal"] = f"{start_date_str},{end_date_str}"

    if (cloud_min > 0) or (cloud_max is not None):
        cloud_min_str = ""
        if cloud_min > 0:
            cloud_min_str = f"{cloud_min}"

        cloud_max_str = ""
        if cloud_max is not None:
            cloud_max_str = f"{cloud_max}"

        prod_srch_params["cloud_cover"] = f"{cloud_min_str},{cloud_max_str}"

    if pt is not None:
        pt_str = "{},{}".format(pt[0], pt[1])
        prod_srch_params["point"] = pt_str
    elif bbox is not None:
        bbox_str = "{},{},{},{}".format(bbox[0], bbox[2], bbox[1], bbox[3])
        prod_srch_params["bounding_box"] = bbox_str

    if sort_date:
        if sort_desc:
            prod_srch_params["sort_key"] = "-start_date"
        else:
            prod_srch_params["sort_key"] = "%2Bstart_date"

    rtnd_data = rsgislib.tools.httptools.send_http_json_request(
        CMR_GRANULES_URL,
        data=prod_srch_params,
        convert_to_json=False,
        header_data=header_info,
    )
    granules_lst = _check_cmr_response(rtnd_data)

    return granules_lst
