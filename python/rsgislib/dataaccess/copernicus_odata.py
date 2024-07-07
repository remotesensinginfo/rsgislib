"""
Tools for accessing (searching and downloading) datasets from the
Copernicus Data Space Ecosystem OData API.

Sensors:
--------
    * RSGIS_ODATA_SEN1 = 1
    * RSGIS_ODATA_SEN1_RTC = 11
    * RSGIS_ODATA_SEN2 = 2
    * RSGIS_ODATA_SEN3 = 3
    * RSGIS_ODATA_SEN5P = 5
    * RSGIS_ODATA_SEN6 = 6
    * RSGIS_ODATA_COP_DEM = 100
    * RSGIS_ODATA_SMOS = 101

Orbit Direction:
-----------------
    * RSGIS_ODATA_ORBIT_DIR_ASC = 1
    * RSGIS_ODATA_ORBIT_DIR_DESC = 2

Order By Direction:
---------------------
    * RSGIS_ODATA_ORDERBY_ASC = 1
    * RSGIS_ODATA_ORDERBY_DESC = 2


Product Types:
--------------
    * RSGIS_ODATA_PROD_TYPE_S1_SLC = 101
    * RSGIS_ODATA_PROD_TYPE_S1_GRD = 102
    * RSGIS_ODATA_PROD_TYPE_S1_OCN = 103

    * RSGIS_ODATA_PROD_TYPE_S2_MSI_1C = 201
    * RSGIS_ODATA_PROD_TYPE_S2_MSI_2A = 202

    * RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA = 301
    * RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA_A = 302
    * RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA_BS = 303
    * RSGIS_ODATA_PROD_TYPE_S3_SR_2_LAN = 304
    * RSGIS_ODATA_PROD_TYPE_S3_OL_1_EFR = 305
    * RSGIS_ODATA_PROD_TYPE_S3_OL_1_ERR = 306
    * RSGIS_ODATA_PROD_TYPE_S3_OL_2_LFR = 307
    * RSGIS_ODATA_PROD_TYPE_S3_OL_2_LRR = 308
    * RSGIS_ODATA_PROD_TYPE_S3_SL_1_RBT = 309
    * RSGIS_ODATA_PROD_TYPE_S3_SL_2_LST = 310
    * RSGIS_ODATA_PROD_TYPE_S3_SY_2_SYN = 311
    * RSGIS_ODATA_PROD_TYPE_S3_SY_2_V10 = 312
    * RSGIS_ODATA_PROD_TYPE_S3_SY_2_VG1 = 313
    * RSGIS_ODATA_PROD_TYPE_S3_SY_2_VGP = 314
    * RSGIS_ODATA_PROD_TYPE_S3_SY_2_AOD = 315
    * RSGIS_ODATA_PROD_TYPE_S3_SL_2_FRP = 316

    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_IR_SIR = 501
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_IR_UVN = 502
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD1 = 503
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD2 = 504
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD3 = 505
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD4 = 506
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD5 = 507
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD6 = 508
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD7 = 509
    * RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD8 = 510
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_AER_AI = 511
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_AER_LH = 512
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_CH4 = 513
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_CLOUD = 514
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_CO = 515
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_HCHO = 516
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_NO2 = 517
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD3 = 518
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD6 = 519
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD7 = 520
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_O3_TCL = 521
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_O3 = 522
    * RSGIS_ODATA_PROD_TYPE_S5P_L2_SO2 = 523
    * RSGIS_ODATA_PROD_TYPE_S5P_AUX_CTMFCT = 524
    * RSGIS_ODATA_PROD_TYPE_S5P_AUX_CTMANA = 525

"""

import os
from typing import List, Tuple, Union, Dict
import datetime
import requests
import tqdm
import rsgislib.tools.utils
import rsgislib.tools.httptools
import rsgislib.tools.geometrytools
import rsgislib.tools.filetools

RSGIS_ODATA_SEN1 = 1
RSGIS_ODATA_SEN1_RTC = 11
RSGIS_ODATA_SEN2 = 2
RSGIS_ODATA_SEN3 = 3
RSGIS_ODATA_SEN5P = 5
RSGIS_ODATA_SEN6 = 6
RSGIS_ODATA_COP_DEM = 100
RSGIS_ODATA_SMOS = 101

RSGIS_ODATA_ORBIT_DIR_ASC = 1
RSGIS_ODATA_ORBIT_DIR_DESC = 2

RSGIS_ODATA_ORDERBY_ASC = 1
RSGIS_ODATA_ORDERBY_DESC = 2


RSGIS_ODATA_PROD_TYPE_S1_SLC = 101
RSGIS_ODATA_PROD_TYPE_S1_GRD = 102
RSGIS_ODATA_PROD_TYPE_S1_OCN = 103

RSGIS_ODATA_PROD_TYPE_S2_MSI_1C = 201
RSGIS_ODATA_PROD_TYPE_S2_MSI_2A = 202

RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA = 301
RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA_A = 302
RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA_BS = 303
RSGIS_ODATA_PROD_TYPE_S3_SR_2_LAN = 304
RSGIS_ODATA_PROD_TYPE_S3_OL_1_EFR = 305
RSGIS_ODATA_PROD_TYPE_S3_OL_1_ERR = 306
RSGIS_ODATA_PROD_TYPE_S3_OL_2_LFR = 307
RSGIS_ODATA_PROD_TYPE_S3_OL_2_LRR = 308
RSGIS_ODATA_PROD_TYPE_S3_SL_1_RBT = 309
RSGIS_ODATA_PROD_TYPE_S3_SL_2_LST = 310
RSGIS_ODATA_PROD_TYPE_S3_SY_2_SYN = 311
RSGIS_ODATA_PROD_TYPE_S3_SY_2_V10 = 312
RSGIS_ODATA_PROD_TYPE_S3_SY_2_VG1 = 313
RSGIS_ODATA_PROD_TYPE_S3_SY_2_VGP = 314
RSGIS_ODATA_PROD_TYPE_S3_SY_2_AOD = 315
RSGIS_ODATA_PROD_TYPE_S3_SL_2_FRP = 316

RSGIS_ODATA_PROD_TYPE_S5P_L1B_IR_SIR = 501
RSGIS_ODATA_PROD_TYPE_S5P_L1B_IR_UVN = 502
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD1 = 503
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD2 = 504
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD3 = 505
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD4 = 506
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD5 = 507
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD6 = 508
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD7 = 509
RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD8 = 510
RSGIS_ODATA_PROD_TYPE_S5P_L2_AER_AI = 511
RSGIS_ODATA_PROD_TYPE_S5P_L2_AER_LH = 512
RSGIS_ODATA_PROD_TYPE_S5P_L2_CH4 = 513
RSGIS_ODATA_PROD_TYPE_S5P_L2_CLOUD = 514
RSGIS_ODATA_PROD_TYPE_S5P_L2_CO = 515
RSGIS_ODATA_PROD_TYPE_S5P_L2_HCHO = 516
RSGIS_ODATA_PROD_TYPE_S5P_L2_NO2 = 517
RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD3 = 518
RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD6 = 519
RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD7 = 520
RSGIS_ODATA_PROD_TYPE_S5P_L2_O3_TCL = 521
RSGIS_ODATA_PROD_TYPE_S5P_L2_O3 = 522
RSGIS_ODATA_PROD_TYPE_S5P_L2_SO2 = 523
RSGIS_ODATA_PROD_TYPE_S5P_AUX_CTMFCT = 524
RSGIS_ODATA_PROD_TYPE_S5P_AUX_CTMANA = 525

RSGIS_ODATA_PRODUCTS_URL = "https://catalogue.dataspace.copernicus.eu/odata/v1/Products"
RSGIS_ODATA_TOKEN_URL = "https://identity.dataspace.copernicus.eu/auth/realms/CDSE/protocol/openid-connect/token"


def get_sensor_collection_name(sensor: int) -> str:
    """
    A function which returns the name of a sensor collection
    for the RSGIS_ODATA_* sensor specified. This function is
    primarily used internally by the functions in this module.

    :param sensor: RSGIS_ODATA_* sensor
    :return: ODATA string for the sensor

    """
    if sensor == RSGIS_ODATA_SEN1:
        return "SENTINEL-1"
    elif sensor == RSGIS_ODATA_SEN1_RTC:
        return "SENTINEL-1-RTC"
    elif sensor == RSGIS_ODATA_SEN2:
        return "SENTINEL-2"
    elif sensor == RSGIS_ODATA_SEN3:
        return "SENTINEL-3"
    elif sensor == RSGIS_ODATA_SEN5P:
        return "SENTINEL-5P"
    elif sensor == RSGIS_ODATA_SEN6:
        return "SENTINEL-6"
    elif sensor == RSGIS_ODATA_COP_DEM:
        return "COP-DEM"
    elif sensor == RSGIS_ODATA_SMOS:
        return "SMOS"
    else:
        raise rsgislib.RSGISPyException("Did not recognise sensor type")


def get_access_token(username: str = None, password: str = None) -> str:
    """
    A function to get the access token from the Copernicus Data Space Ecosystem.

    :param username: Your username for the Copernicus Data Space Ecosystem.
                     If RSGIS_COP_USER environmental variable is specified
                     then username will read from there is None is passed
                     (Default: None)
    :param password: Your password for the Copernicus Data Space Ecosystem.
                     If RSGIS_COP_PASS environmental variable is specified
                     then password will read from there is None is passed
                     (Default: None)
    :return: the access token Copernicus Open Access Hub.
    """
    if username is None:
        username = rsgislib.tools.utils.get_environment_variable("RSGIS_COP_USER")

    if password is None:
        password = rsgislib.tools.utils.get_environment_variable("RSGIS_COP_PASS")

    data = {
        "client_id": "cdse-public",
        "username": username,
        "password": password,
        "grant_type": "password",
    }
    resp_obj = requests.post(RSGIS_ODATA_TOKEN_URL, data=data)
    try:
        if rsgislib.tools.httptools.check_http_response(
            resp_obj, RSGIS_ODATA_TOKEN_URL
        ):
            resp_dict = resp_obj.json()
            if "access_token" not in resp_dict:
                raise Exception(
                    f"Failed to get access token. Returned Data: {resp_dict}"
                )
            access_token = resp_dict["access_token"]
        else:
            access_token = None
    except Exception as e:
        raise rsgislib.RSGISPyException(
            "Failed to get access token - check username and password"
        )
    return access_token


def query_scn_lst(
    sensor: int,
    bbox: Union[Tuple[float, float, float, float], List[float]],
    start_date: datetime.datetime = None,
    end_date: datetime.datetime = None,
    cloud_cover: float = None,
    orbit_dir: int = None,
    product_type: int = None,
    order_by: int = RSGIS_ODATA_ORDERBY_ASC,
    max_n_rslts: int = 25,
    start_n: int = None,
) -> List[Dict]:
    """
    A function which uses the Copernicus Data Space Ecosystem OData API
    to find lists of scenes using criteria to filter the return list of
    scenes.

    :param sensor: The sensor / product being search for (e.g.,
                   RSGIS_ODATA_SEN1 or RSGIS_ODATA_SEN2)
    :param bbox: is a bbox (xMin, xMax, yMin, yMax) in EPSG:4326
    :param start_date: a datetime object representing the start date
                       (i.e., earlier date)
    :param end_date: a datetime object representing the end date
                       (i.e., later date)
    :param cloud_cover: value between 0-100 where scenes with cloud cover less
                        than the threshold will be returned
    :param orbit_dir: The orbit direction ascending (RSGIS_ODATA_ORBIT_DIR_ASC)
                      or descending (RSGIS_ODATA_ORBIT_DIR_DESC)
    :param product_type: The product type specified as RSGIS_ODATA_PROD_TYPE*
                         (e.g., RSGIS_ODATA_PROD_TYPE_S1_SLC,
                         RSGIS_ODATA_PROD_TYPE_S1_GRD,
                         RSGIS_ODATA_PROD_TYPE_S2_MSI_1C,
                         RSGIS_ODATA_PROD_TYPE_S2_MSI_2A)
    :param order_by: Order by date either ascending (RSGIS_ODATA_ORDERBY_ASC)
                     or descending (RSGIS_ODATA_ORDERBY_DESC)
    :param max_n_rslts: Maximum number of scenes that will be returned (default: 25)
    :param start_n: An offset skipping the first n scenes. Can be used in combination
                    with max_n_rslts to query in 'pages'
    :return: returns list of dictionaries containing all scene information

    """

    sensor_name = get_sensor_collection_name(sensor)
    query_url = f"{RSGIS_ODATA_PRODUCTS_URL}?$filter=Collection/Name eq '{sensor_name}'"

    bbox_wkt_str = rsgislib.tools.geometrytools.get_bbox_wkt_poly(bbox)
    bbox_url_str = f"OData.CSC.Intersects(area=geography'SRID=4326;{bbox_wkt_str}')"
    query_url = f"{query_url} and {bbox_url_str}"

    if (start_date is not None) and (end_date is None):
        raise rsgislib.RSGISPyException(
            "If a start_date is provided then an end_date must also be provided"
        )
    elif (start_date is None) and (end_date is not None):
        raise rsgislib.RSGISPyException(
            "If an end_date is provided then a start_date must also be provided"
        )
    elif (start_date is not None) and (end_date is not None):
        start_date_str = start_date.strftime("%Y-%m-%dT%H:%M:%S.000Z")
        end_date_str = end_date.strftime("%Y-%m-%dT%H:%M:%S.000Z")

        dates_url_str = f"ContentDate/Start gt {start_date_str} and ContentDate/End lt {end_date_str}"
        query_url = f"{query_url} and {dates_url_str}"

    if cloud_cover is not None:
        if cloud_cover < 0 or cloud_cover > 100:
            raise rsgislib.RSGISPyException("Cloud cover must be between 0 and 100")
        cloud_cover_str = f"Attributes/OData.CSC.DoubleAttribute/any(att:att/Name eq 'cloudCover' and att/OData.CSC.DoubleAttribute/Value le {cloud_cover})"
        query_url = f"{query_url} and {cloud_cover_str}"

    if orbit_dir is not None:
        if orbit_dir == RSGIS_ODATA_ORBIT_DIR_ASC:
            orbit_dir_str = "ASCENDING"
        elif orbit_dir == RSGIS_ODATA_ORBIT_DIR_DESC:
            orbit_dir_str = "DESCENDING"
        else:
            raise rsgislib.RSGISPyException(
                "orbit_dir must be either Ascending or Descending"
            )

        orbit_dir_query_str = f"Attributes/OData.CSC.StringAttribute/any(att:att/Name eq 'orbitDirection' and att/OData.CSC.StringAttribute/Value eq '{orbit_dir_str}')"
        query_url = f"{query_url} and {orbit_dir_query_str}"

    if product_type is not None:
        if product_type == RSGIS_ODATA_PROD_TYPE_S1_SLC:
            product_type_str = "SLC"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S1_GRD:
            product_type_str = "GRD"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S1_OCN:
            product_type_str = "OCN"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S2_MSI_1C:
            product_type_str = "S2MSI1C"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S2_MSI_2A:
            product_type_str = "S2MSI2A"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA:
            product_type_str = "SR_1_SRA___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA_A:
            product_type_str = "SR_1_SRA_A"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SR_1_SRA_BS:
            product_type_str = "SR_1_SRA_BS"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SR_2_LAN:
            product_type_str = "SR_2_LAN___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_OL_1_EFR:
            product_type_str = "OL_1_EFR___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_OL_1_ERR:
            product_type_str = "OL_1_ERR___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_OL_2_LFR:
            product_type_str = "OL_2_LFR___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_OL_2_LRR:
            product_type_str = "OL_2_LRR___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SL_1_RBT:
            product_type_str = "SL_1_RBT___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SL_2_LST:
            product_type_str = "SL_2_LST___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SY_2_SYN:
            product_type_str = "SY_2_SYN___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SY_2_V10:
            product_type_str = "SY_2_V10___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SY_2_VG1:
            product_type_str = "SY_2_VG1___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SY_2_VGP:
            product_type_str = "SY_2_VGP___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SY_2_AOD:
            product_type_str = "SY_2_AOD__"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S3_SL_2_FRP:
            product_type_str = "SL_2_FRP__"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_IR_SIR:
            product_type_str = "L1B_IR_SIR"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_IR_UVN:
            product_type_str = "L1B_IR_UVN"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD1:
            product_type_str = "L1B_RA_BD1"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD2:
            product_type_str = "L1B_RA_BD2"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD3:
            product_type_str = "L1B_RA_BD3"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD4:
            product_type_str = "L1B_RA_BD4"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD5:
            product_type_str = "L1B_RA_BD5"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD6:
            product_type_str = "L1B_RA_BD6"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD7:
            product_type_str = "L1B_RA_BD7"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L1B_RA_BD8:
            product_type_str = "L1B_RA_BD8"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_AER_AI:
            product_type_str = "L2__AER_AI"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_AER_LH:
            product_type_str = "L2__AER_LH"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_CH4:
            product_type_str = "L2__CH4"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_CLOUD:
            product_type_str = "L2__CLOUD_"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_CO:
            product_type_str = "L2__CO____"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_HCHO:
            product_type_str = "L2__HCHO__"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_NO2:
            product_type_str = "L2__NO2___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD3:
            product_type_str = "L2__NP_BD3"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD6:
            product_type_str = "L2__NP_BD6"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_NP_BD7:
            product_type_str = "L2__NP_BD7"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_O3_TCL:
            product_type_str = "L2__O3_TCL"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_O3:
            product_type_str = "L2__O3____"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_L2_SO2:
            product_type_str = "L2__SO2___"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_AUX_CTMFCT:
            product_type_str = "AUX_CTMFCT"
        elif product_type == RSGIS_ODATA_PROD_TYPE_S5P_AUX_CTMANA:
            product_type_str = "AUX_CTMANA"
        else:
            raise rsgislib.RSGISPyException(
                "product_type must be either Ascending or Descending"
            )

        product_type_query_str = f"Attributes/OData.CSC.StringAttribute/any(att:att/Name eq 'productType' and att/OData.CSC.StringAttribute/Value eq '{product_type_str}')"
        query_url = f"{query_url} and {product_type_query_str}"

    order_by_str = "$orderby=ContentDate/Start"
    if order_by == RSGIS_ODATA_ORBIT_DIR_ASC:
        order_by_str = f"{order_by_str} asc"
    elif order_by == RSGIS_ODATA_ORBIT_DIR_DESC:
        order_by_str = f"{order_by_str} desc"
    else:
        raise rsgislib.RSGISPyException(
            "order_by must be either ascending or descending"
        )
    query_url = f"{query_url}&{order_by_str}"

    query_url = f"{query_url}&$top={max_n_rslts}"
    if (start_n is not None) and (start_n > 0):
        query_url = f"{query_url}&skip={start_n}"

    query_url = f"{query_url}&$expand=Locations"

    resp = requests.get(query_url)
    resp_dict = resp.json()
    if "value" in resp_dict:
        out_scns_dict = resp_dict["value"]
    else:
        raise rsgislib.RSGISPyException("Error - response didn't have a 'value'.")

    return out_scns_dict


def query_scn(scn_name: str) -> Dict:
    """
    A function which queries for a single scene using the scene name
    for example: S2B_MSIL2A_20240602T112119_N0510_R037_T30UVD_20240602T125034.SAFE

    :param scn_name: name of the scene to be found
    :return: dictionary of the information for the scene

    """
    query_url = (
        f"{RSGIS_ODATA_PRODUCTS_URL}?$filter=Name eq '{scn_name}'&$expand=Locations"
    )
    resp_obj = requests.get(query_url)

    if rsgislib.tools.httptools.check_http_response(resp_obj, query_url):
        resp_dict = resp_obj.json()
        if "value" in resp_dict:
            if len(resp_dict["value"]) == 1:
                out_scn_dict = resp_dict["value"][0]
            else:
                n_scns = len(resp_dict["value"])
                raise rsgislib.RSGISPyException(
                    f"There were {n_scns} scenes in the list. Was expecting one."
                )
        else:
            raise rsgislib.RSGISPyException("Error - response didn't have a 'value'.")
    else:
        out_scn_dict = None

    return out_scn_dict


def download_scn(access_token: str, scn_info: Dict, out_path: str):
    """
    A function which downloads a single scene to the out_path.
    Note, during the download the file will be given the extension
    .incomplete until the is complete when it will be renamed. If
    available the MD5 checksum of the file will be checked.

    :param access_token: The access token to download the scene use get_access_token
                         function to generate the access token.
    :param scn_info: A dictionary with the scene information from query_scn or
                     query_scn_lst functions.
    :param out_path: The output path where the scene will be saved.

    """
    scn_name = scn_info["Name"]
    print(f"Downloading {scn_name}")

    out_file_path = os.path.join(out_path, f"{scn_name}.zip")
    if not os.path.exists(out_file_path):
        scn_locs = scn_info["Locations"]
        scn_dwnld_info = None
        for scn_loc in scn_locs:
            if scn_loc["FormatType"] == "Extracted":
                scn_dwnld_info = scn_loc
                break

        if scn_dwnld_info is None:
            raise rsgislib.RSGISPyException("Error - no download location found.")

        scn_url = scn_dwnld_info["DownloadLink"]
        scn_size = scn_dwnld_info["ContentLength"]
        scn_checksum = None
        for checksum_info in scn_dwnld_info["Checksum"]:
            if checksum_info["Algorithm"] == "MD5":
                scn_checksum = checksum_info["Value"]

        headers = dict()
        headers["Authorization"] = f"Bearer {access_token}"
        headers["User-Agent"] = f"rsgislib/{rsgislib.get_rsgislib_version()}"
        ses = requests.Session()
        ses.headers.update(headers)

        # Need to change the URL to download rather than catalogue.
        # Get a 401 authorisation error without this change to the URL.
        if "//catalogue." in scn_url:
            scn_url = scn_url.replace("//catalogue.", "//download.")

        # Perform the GET request
        resp_obj = ses.get(scn_url, stream=True, verify=True, allow_redirects=True)

        tmp_file_path = os.path.join(out_path, f"{scn_name}.incomplete")
        chunk_size = 8192
        n_chunks = int(scn_size / chunk_size) + 1

        if rsgislib.tools.httptools.check_http_response(resp_obj, scn_url):
            with open(tmp_file_path, "wb") as file:
                for chunk in tqdm.tqdm(
                    resp_obj.iter_content(chunk_size=chunk_size), total=n_chunks
                ):
                    if chunk:  # filter out keep-alive new chunks
                        file.write(chunk)

        if os.path.exists(tmp_file_path):
            if scn_checksum is not None:
                md5_checksum = rsgislib.tools.filetools.create_md5_hash(tmp_file_path)
                if md5_checksum != scn_checksum:
                    raise rsgislib.RSGISPyException("Error - checksum mismatch.")
            os.rename(tmp_file_path, out_file_path)
            print(f"Download Complete: {scn_name}")
        else:
            raise rsgislib.RSGISPyException(f"Failed to download file - unknown error")

    else:
        print(f"File already exists: {out_file_path}")


def download_scns(
    access_token: str,
    scns_info: List[Dict],
    out_path: str,
    no_except: bool = True,
):
    """
    A function which loops through a list of scenes and downloads
    the datasets using the download_scn function. Option to print
    exceptions rather than stopping so all available scenes are
    downloaded.

    :param access_token: The access token to download the scene use get_access_token
                         function to generate the access token.
    :param scns_info: A list of dictionaries with the scene information
                      from query_scn or query_scn_lst functions.
    :param out_path: The output path where the scene will be saved.
    :param no_except: If True (Default) then expections are not outputted.

    """
    for scn_info in scns_info:
        try:
            download_scn(access_token, scn_info, out_path)
        except rsgislib.RSGISPyException as e:
            if no_except:
                print(e)
            else:
                raise e
