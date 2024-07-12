#!/usr/bin/env python
"""
Tools for accessing (searching and downloading) datasets from Planet.

Note to use this module you need to have the planet API installed which
can be used from pip:

```
pip install planet
```

Item Types
-----------
    * RSGIS_PLANET_ITEM_PSScene = 1
    * RSGIS_PLANET_ITEM_REOrthoTile = 2
    * RSGIS_PLANET_ITEM_REScene = 3
    * RSGIS_PLANET_ITEM_SkySatScene = 4
    * RSGIS_PLANET_ITEM_SkySatCollect = 5
    * RSGIS_PLANET_ITEM_SkySatVideo = 6

Asset Types
------------
    * RSGIS_PLANET_ASSET_basic_analytic_4b = 1
    * RSGIS_PLANET_ASSET_basic_analytic_4b_rpc = 2
    * RSGIS_PLANET_ASSET_basic_analytic_4b_xml = 3
    * RSGIS_PLANET_ASSET_basic_analytic_8b = 4
    * RSGIS_PLANET_ASSET_basic_analytic_8b_xml = 5
    * RSGIS_PLANET_ASSET_basic_udm2 = 6
    * RSGIS_PLANET_ASSET_ortho_analytic_4b = 7
    * RSGIS_PLANET_ASSET_ortho_analytic_4b_sr = 8
    * RSGIS_PLANET_ASSET_ortho_analytic_4b_xml = 9
    * RSGIS_PLANET_ASSET_ortho_analytic_8b = 10
    * RSGIS_PLANET_ASSET_ortho_analytic_8b_sr = 11
    * RSGIS_PLANET_ASSET_ortho_analytic_8b_xml = 12
    * RSGIS_PLANET_ASSET_ortho_udm2 = 13
    * RSGIS_PLANET_ASSET_ortho_visual = 14

Bundle Types
---------------
    * RSGIS_PLANET_BUNDLE_analytic_udm2 = 1
    * RSGIS_PLANET_BUNDLE_analytic_3b_udm2 = 2
    * RSGIS_PLANET_BUNDLE_analytic_8b_udm2 = 3
    * RSGIS_PLANET_BUNDLE_visual = 4
    * RSGIS_PLANET_BUNDLE_basic_analytic_udm2 = 5
    * RSGIS_PLANET_BUNDLE_basic_analytic_8b_udm2 = 6
    * RSGIS_PLANET_BUNDLE_analytic_sr_udm2 = 7
    * RSGIS_PLANET_BUNDLE_analytic_8b_sr_udm2 = 8

"""

from typing import Dict, List, Tuple, Union
import asyncio
import datetime
import rsgislib.tools.utils
import rsgislib.tools.geometrytools

PLANET_AVAIL = True
try:
    import planet
except ImportError:
    PLANET_AVAIL = False

# Sensor Options
# PlanetScope 3, 4, and 8 band scenes captured by the Dove satellite constellation
RSGIS_PLANET_ITEM_PSScene = 1
# RapidEye OrthoTiles captured by the RapidEye satellite constellation
RSGIS_PLANET_ITEM_REOrthoTile = 2
# Unorthorectified strips captured by the RapidEye satellite constellation
RSGIS_PLANET_ITEM_REScene = 3
# SkySat Scenes captured by the SkySat satellite constellation
RSGIS_PLANET_ITEM_SkySatScene = 4
# Orthorectified scene composite of a SkySat collection
RSGIS_PLANET_ITEM_SkySatCollect = 5
# Full motion videos collected by a single camera from any of the active SkySats
RSGIS_PLANET_ITEM_SkySatVideo = 6

# Asset Options
RSGIS_PLANET_ASSET_basic_analytic_4b = 1
RSGIS_PLANET_ASSET_basic_analytic_4b_rpc = 2
RSGIS_PLANET_ASSET_basic_analytic_4b_xml = 3
RSGIS_PLANET_ASSET_basic_analytic_8b = 4
RSGIS_PLANET_ASSET_basic_analytic_8b_xml = 5
RSGIS_PLANET_ASSET_basic_udm2 = 6
RSGIS_PLANET_ASSET_ortho_analytic_4b = 7
RSGIS_PLANET_ASSET_ortho_analytic_4b_sr = 8
RSGIS_PLANET_ASSET_ortho_analytic_4b_xml = 9
RSGIS_PLANET_ASSET_ortho_analytic_8b = 10
RSGIS_PLANET_ASSET_ortho_analytic_8b_sr = 11
RSGIS_PLANET_ASSET_ortho_analytic_8b_xml = 12
RSGIS_PLANET_ASSET_ortho_udm2 = 13
RSGIS_PLANET_ASSET_ortho_visual = 14

# Bundle Options
RSGIS_PLANET_BUNDLE_analytic_udm2 = 1
RSGIS_PLANET_BUNDLE_analytic_3b_udm2 = 2
RSGIS_PLANET_BUNDLE_analytic_8b_udm2 = 3
RSGIS_PLANET_BUNDLE_visual = 4
RSGIS_PLANET_BUNDLE_basic_analytic_udm2 = 5
RSGIS_PLANET_BUNDLE_basic_analytic_8b_udm2 = 6
RSGIS_PLANET_BUNDLE_analytic_sr_udm2 = 7
RSGIS_PLANET_BUNDLE_analytic_8b_sr_udm2 = 8


def get_item_type_str(item_type: int) -> str:
    """
    Get the string representation of a given item type (sensor).

    :param item_type: RSGIS_PLANET_ITEM_* value
    :return: string representation of a given item type

    """
    if item_type == RSGIS_PLANET_ITEM_PSScene:
        item_type_str = "PSScene"
    elif item_type == RSGIS_PLANET_ITEM_REOrthoTile:
        item_type_str = "REOrthoTile"
    elif item_type == RSGIS_PLANET_ITEM_REScene:
        item_type_str = "REScene"
    elif item_type == RSGIS_PLANET_ITEM_SkySatScene:
        item_type_str = "SkySatScene"
    elif item_type == RSGIS_PLANET_ITEM_SkySatCollect:
        item_type_str = "SkySatCollect"
    elif item_type == RSGIS_PLANET_ITEM_SkySatVideo:
        item_type_str = "SkySatVideo"
    else:
        raise rsgislib.RSGISPyException("item_type was not recognised")
    return item_type_str


def get_asset_type_str(asset_type: int) -> str:
    """
    A function to get the string representation of a given asset type.
    :param asset_type: RSGIS_PLANET_ASSET_*
    :return: string representation of a given asset type

    """
    if asset_type == RSGIS_PLANET_ASSET_basic_analytic_4b:
        asset_type_str = "basic_analytic_4b"
    elif asset_type == RSGIS_PLANET_ASSET_basic_analytic_4b_rpc:
        asset_type_str = "basic_analytic_4b_rpc"
    elif asset_type == RSGIS_PLANET_ASSET_basic_analytic_4b_xml:
        asset_type_str = "basic_analytic_4b_xml"
    elif asset_type == RSGIS_PLANET_ASSET_basic_analytic_8b:
        asset_type_str = "basic_analytic_8b"
    elif asset_type == RSGIS_PLANET_ASSET_basic_analytic_8b_xml:
        asset_type_str = "basic_analytic_8b_xml"
    elif asset_type == RSGIS_PLANET_ASSET_basic_udm2:
        asset_type_str = "basic_udm2"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_analytic_4b:
        asset_type_str = "ortho_analytic_4b"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_analytic_4b_sr:
        asset_type_str = "ortho_analytic_4b_sr"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_analytic_4b_xml:
        asset_type_str = "ortho_analytic_4b_xml"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_analytic_8b:
        asset_type_str = "ortho_analytic_8b"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_analytic_8b_sr:
        asset_type_str = "ortho_analytic_8b_sr"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_analytic_8b_xml:
        asset_type_str = "ortho_analytic_8b_xml"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_udm2:
        asset_type_str = "ortho_udm2"
    elif asset_type == RSGIS_PLANET_ASSET_ortho_visual:
        asset_type_str = "ortho_visual"
    else:
        raise rsgislib.RSGISPyException("asset_type was not recognised")
    return asset_type_str


def get_bundle_type_str(bundle_type: int) -> str:
    """
    A function to get the string representation of a given bundle type.
    :param bundle_type: RSGIS_PLANET_ASSET_*
    :return: string representation of a given bundle type

    """
    if bundle_type == RSGIS_PLANET_BUNDLE_analytic_udm2:
        bundle_type_str = "analytic_udm2"
    elif bundle_type == RSGIS_PLANET_BUNDLE_analytic_3b_udm2:
        bundle_type_str = "analytic_3b_udm2"
    elif bundle_type == RSGIS_PLANET_BUNDLE_analytic_8b_udm2:
        bundle_type_str = "analytic_8b_udm2"
    elif bundle_type == RSGIS_PLANET_BUNDLE_visual:
        bundle_type_str = "visual"
    elif bundle_type == RSGIS_PLANET_BUNDLE_basic_analytic_udm2:
        bundle_type_str = "basic_analytic_udm2"
    elif bundle_type == RSGIS_PLANET_BUNDLE_basic_analytic_8b_udm2:
        bundle_type_str = "basic_analytic_8b_udm2"
    elif bundle_type == RSGIS_PLANET_BUNDLE_analytic_sr_udm2:
        bundle_type_str = "analytic_sr_udm2"
    elif bundle_type == RSGIS_PLANET_BUNDLE_analytic_8b_sr_udm2:
        bundle_type_str = "analytic_8b_sr_udm2"
    else:
        raise rsgislib.RSGISPyException("bundle_type was not recognised")
    return bundle_type_str


def planet_auth(username: str = None, password: str = None, api_key: str = None):
    """
    A function to authenticate with planet.

    :param username: Your username for the planet. If RSGIS_PLANET_USER
                     environmental variable is specified then username
                     will read from there is None is passed (Default: None)
    :param password: Your password for the Planet. If RSGIS_PLANET_PASS
                     environmental variable is specified then password
                     will read from there is None is passed (Default: None)
    :param api_key: Your api key for Planet API. If RSGIS_PLANET_API_KEY then
                    the api key will be read from there. Alternatively, the
                    PL_API_KEY environment variable can be specified.
                    (Default: None)
    :return: returns a planet.Auth object

    """
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    if username is None:
        username = rsgislib.tools.utils.get_environment_variable("RSGIS_PLANET_USER")

    if password is None:
        password = rsgislib.tools.utils.get_environment_variable("RSGIS_PLANET_PASS")

    if api_key is None:
        api_key = rsgislib.tools.utils.get_environment_variable("RSGIS_PLANET_API_KEY")
    if api_key is None:
        api_key = rsgislib.tools.utils.get_environment_variable("PL_API_KEY")

    if api_key is not None:
        auth = planet.Auth.from_key(api_key)
    else:
        auth = planet.Auth.from_login(username, password)

    return auth


async def _search_planet_items(
    planet_auth,
    item_type: int,
    bbox: Union[Tuple[float, float, float, float], List[float]],
    start_date: datetime.datetime = None,
    end_date: datetime.datetime = None,
    cloud_cover: float = None,
    sun_elevation_min: float = None,
    sun_elevation_max: float = None,
    view_angle_min: float = None,
    view_angle_max: float = None,
    max_n_rslts: int = 25,
) -> List[Dict]:
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    item_type_str = get_item_type_str(item_type)

    data_filters = list()
    data_filters.append(planet.data_filter.permission_filter())

    bbox_geo_dict = rsgislib.tools.geometrytools.get_bbox_geojson_poly(bbox)
    data_filters.append(planet.data_filter.geometry_filter(bbox_geo_dict))

    if (start_date is not None) or (end_date is not None):
        data_filters.append(
            planet.data_filter.date_range_filter(
                "acquired", gte=start_date, lte=end_date
            )
        )

    if cloud_cover is not None:
        data_filters.append(
            planet.data_filter.range_filter("cloud_percent", lte=cloud_cover)
        )

    if (sun_elevation_min is not None) or (sun_elevation_max is not None):
        data_filters.append(
            planet.data_filter.range_filter(
                "sun_elevation", gte=sun_elevation_min, lte=sun_elevation_max
            )
        )

    if (view_angle_min is not None) or (view_angle_max is not None):
        data_filters.append(
            planet.data_filter.range_filter(
                "view_angle", gte=view_angle_min, lte=view_angle_max
            )
        )

    planet_data_filter = planet.data_filter.and_filter(data_filters)

    async with planet.Session(auth=planet_auth) as planet_sess:
        # perform operations here
        planet_data_cl = planet_sess.client("data")
        items = [
            i
            async for i in planet_data_cl.search(
                item_types=[item_type_str],
                search_filter=planet_data_filter,
                sort="acquired asc",
                limit=max_n_rslts,
            )
        ]

    return items


def run_search_planet_items(
    planet_auth,
    item_type: int,
    bbox: Union[Tuple[float, float, float, float], List[float]],
    start_date: datetime.datetime = None,
    end_date: datetime.datetime = None,
    cloud_cover: float = None,
    sun_elevation_min: float = None,
    sun_elevation_max: float = None,
    view_angle_min: float = None,
    view_angle_max: float = None,
    max_n_rslts: int = 25,
) -> List[Dict]:
    """
    A function which searches the planet API to find scenes/items

    :param planet_auth: A planet.Auth object which can be created uing the
                        rsgislib.dataccess.planet_data_api.planet_auth function.
    :param item_type: The type of item to be downloaded (RSGIS_PLANET_ITEM_*)
    :param bbox: is a bbox (xMin, xMax, yMin, yMax) in EPSG:4326 defining the region
                 of interest.
    :param start_date: a datetime object representing the start date
                       (i.e., earlier date)
    :param end_date: a datetime object representing the end date
                       (i.e., later date)
    :param cloud_cover: value between 0-100 where scenes with cloud cover less
                        than the threshold will be returned. If None (default)
                        then ignored.
    :param sun_elevation_min: the minimum solar elevation (in degrees). If None
                              (default) then ignored.
    :param sun_elevation_max: the maximum solar elevation (in degrees). If None
                              (default) then ignored.
    :param view_angle_min: the minimum view angle (in degrees). If None
                           (default) then ignored.
    :param view_angle_max: the maximum view angle (in degrees). If None
                           (default) then ignored.
    :param max_n_rslts: The maximum number of results to return.
    :return: A list of dictionaries containing all scene items.

    """
    return asyncio.run(
        _search_planet_items(
            planet_auth,
            item_type,
            bbox,
            start_date,
            end_date,
            cloud_cover,
            sun_elevation_min,
            sun_elevation_max,
            view_angle_min,
            view_angle_max,
            max_n_rslts,
        )
    )


async def _download_and_validate_item(
    planet_auth,
    item_type: int,
    item_id: str,
    asset_type: int,
    out_file_path: str,
    overwrite: bool = False,
):
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    item_type_str = get_item_type_str(item_type)
    asset_type_str = get_asset_type_str(asset_type)

    async with planet.Session(auth=planet_auth) as planet_sess:
        planet_cl = planet_sess.client("data")

        with planet.reporting.StateBar(state="Get Asset Info") as bar:
            # get asset description
            planet_asset = await planet_cl.get_asset(
                item_type_str, item_id, asset_type_str
            )
            bar.update(state="Activate")

            # activate asset
            await planet_cl.activate_asset(planet_asset)
            bar.update(state="Wait for Activation")

            # wait for asset to become active
            asset = await planet_cl.wait_asset(planet_asset, callback=bar.update_state)

        # download asset
        path = await planet_cl.download_asset(
            asset, directory=out_file_path, overwrite=overwrite, progress_bar=True
        )

        # validate download file
        planet_cl.validate_checksum(asset, path)


def run_download_and_validate_item(
    planet_auth,
    item_type: int,
    item_id: str,
    asset_type: int,
    out_file_path: str,
    overwrite: bool = False,
):
    """
    A function which can be used to download a single item from a planet

    :param planet_auth: A planet.Auth object which can be created uing the
                        rsgislib.dataccess.planet_data_api.planet_auth function.
    :param item_type: The type of item to be downloaded (RSGIS_PLANET_ITEM_*)
    :param item_id: The unique id for the item to be downloaded.
    :param asset_type: The type of asset type to be downloaded (RSGIS_PLANET_ASSET_*)
    :param out_file_path: The output file path to download the file to.
    :param overwrite: Boolean to overwrite existing files if it exists.

    """
    asyncio.run(
        _download_and_validate_item(
            planet_auth,
            item_type,
            item_id,
            asset_type,
            out_file_path,
            overwrite,
        )
    )


async def _create_planet_order(
    planet_auth,
    order_name: str,
    items: List[Dict],
    item_type: int,
    bundle_type: int,
    email_notification: bool = True,
) -> Dict:
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    item_type_str = get_item_type_str(item_type)

    image_ids = list()
    for item in items:
        image_ids.append(item["id"])

    products_lst = list()
    bundle_type_str = get_bundle_type_str(bundle_type)
    products_lst.append(
        planet.order_request.product(
            item_ids=image_ids,
            product_bundle=bundle_type_str,
            item_type=item_type_str,
        )
    )

    planet_request = planet.order_request.build_request(
        name=order_name,
        products=products_lst,
        notifications=planet.order_request.notifications(email=email_notification),
    )

    async with planet.Session(auth=planet_auth) as planet_sess:
        planet_cl = planet_sess.client("orders")
        planet_order = await planet_cl.create_order(planet_request)

    return planet_order


def run_create_planet_order(
    planet_auth,
    order_name: str,
    items: List[Dict],
    item_type: int,
    bundle_type: int,
    email_notification: bool = True,
) -> Dict:
    """
    A function which creates an order for a list of items

    :param planet_auth: A planet.Auth object which can be created uing the
                        rsgislib.dataccess.planet_data_api.planet_auth function.
    :param order_name: A name for the new order.
    :param items: A list of dictionaries containing the items to be created.
    :param item_type: The type of item to be included in the order (RSGIS_PLANET_ITEM_*)
    :param bundle_type: The bundle type for the order (RSGIS_PLANET_BUNDLE_*)
    :param email_notification: Boolean specifying whether you will receive an
                               email notification when the order is ready to download.
                               (Default: True)
    :return: dict of information for the created order (including the order id)

    """
    return asyncio.run(
        _create_planet_order(
            planet_auth, order_name, items, item_type, bundle_type, email_notification
        )
    )


async def _download_planet_order(
    planet_auth,
    order_id: str,
    out_file_path: str,
    overwrite: bool = False,
) -> List:
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    async with planet.Session(auth=planet_auth) as planet_sess:
        planet_cl = planet_sess.client("orders")
        planet_order = await planet_cl.get_order(order_id)

        if planet_order["state"] == "success":
            dwnld_paths = await planet_cl.download_order(
                order_id,
                directory=out_file_path,
                overwrite=overwrite,
                progress_bar=True,
            )
        else:
            raise rsgislib.RSGISPyException(
                "There is an error with the order - likely that is not yet available for downloading."
            )
        return dwnld_paths


def run_download_planet_order(
    planet_auth,
    order_id: str,
    out_file_path: str,
    overwrite: bool = False,
) -> List:
    """
    A function which downloads an order which has been processed and ready
    to download. If the order is not ready do download then an expection
    will be thrown.

    :param planet_auth: A planet.Auth object which can be created uing the
                        rsgislib.dataccess.planet_data_api.planet_auth function.
    :param order_id: The order ID (not name) of the order to be downloaded.
    :param out_file_path: the output directory where the order will be downloaded.
    :param overwrite: Specify whether downloads should overwrite existing files.
                      (Default = False)
    :return: list of downloaded file paths.

    """
    return asyncio.run(
        _download_planet_order(planet_auth, order_id, out_file_path, overwrite)
    )


async def _get_planet_orders(
    planet_auth,
) -> List[Dict]:
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    async with planet.Session(auth=planet_auth) as planet_sess:
        planet_cl = planet_sess.client("orders")
        orders_lst = planet_cl.list_orders()

    return orders_lst


def run_get_planet_orders(
    planet_auth,
) -> List[Dict]:
    """
    A function which gets a list of all the current planet orders.

    :param planet_auth: A planet.Auth object which can be created uing the
                        rsgislib.dataccess.planet_data_api.planet_auth function.
    :return: List of orders

    """
    return asyncio.run(_get_planet_orders(planet_auth))


async def _cancel_planet_orders(
    planet_auth,
    order_ids: List[str] = None,
) -> Dict:
    if not PLANET_AVAIL:
        raise rsgislib.RSGISPyException(
            "The Planet API (pip install planet) needs to be available."
        )

    async with planet.Session(auth=planet_auth) as planet_sess:
        planet_cl = planet_sess.client("orders")
        orders_info = planet_cl.cancel_orders(order_ids=order_ids)

    return orders_info


def run_cancel_planet_orders(
    planet_auth,
    order_ids: List[str] = None,
) -> Dict:
    """
    A function which the cancels planet orders.

    :param planet_auth: A planet.Auth object which can be created uing the
                        rsgislib.dataccess.planet_data_api.planet_auth function.
    :param order_ids: optional list of order IDs to be cancelled. If None
                      (Default) then all are cancelled.
    :return: Dictionary of orders

    """
    return asyncio.run(_cancel_planet_orders(planet_auth, order_ids))
