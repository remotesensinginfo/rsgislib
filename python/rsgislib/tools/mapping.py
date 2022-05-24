#!/usr/bin/env python
"""
The tools.mapping module contains functions for making maps with geospatial data.
"""

from typing import Tuple, List, Dict, Union
import os
import math

import rsgislib
import rsgislib.tools.projection
import rsgislib.tools.plotting
import rsgislib.imageutils

# import numpy
import geopandas


import matplotlib.pyplot as plt

# import matplotlib.colors as mclrs
# from matplotlib.patches import Patch
# from matplotlib import rcParams
# from matplotlib import gridspec

from matplotlib_scalebar.scalebar import ScaleBar


def create_vec_lyr_map(
    gp_vec: geopandas.GeoDataFrame,
    bbox: List[float],
    title_str: str = None,
    cx_src=None,
    out_file: str = None,
    out_dpi: int = 400,
    vec_fill_clr: str = "grey",
    vec_line_clr: str = "black",
    vec_line_width: float = 0.25,
    vec_fill_alpha: int = 1,
    use_grid: bool = False,
    show_map_axis: bool = True,
    sub_in_vec: bool = False,
    fig_x_size: int = 10,
    fig_y_size: int = None,
    cx_zoom_lvl: int = Union[int, "auto"],
    cx_attribution: Union[str, bool] = None,
    cx_att_size: int = 8,
):
    if cx_src is not None:
        import contextily

    if sub_in_vec:
        min_x_sub = math.floor(bbox[0] - 4)
        max_x_sub = math.ceil(bbox[1] + 4)
        min_y_sub = math.floor(bbox[2] - 4)
        max_y_sub = math.ceil(bbox[3] + 4)
        gp_vec_sub = gp_vec.cx[min_x_sub:max_x_sub, min_y_sub:max_y_sub]
    else:
        gp_vec_sub = gp_vec

    if fig_y_size is None:
        x_bbox_size = bbox[1] - bbox[0]
        y_bbox_size = bbox[3] - bbox[2]
        bbox_xy_ratio = y_bbox_size / x_bbox_size
        fig_y_size = fig_x_size * bbox_xy_ratio

    fig, ax = plt.subplots(1, 1, figsize=(fig_x_size, fig_y_size))

    gp_vec_sub.plot(
        ax=ax,
        color=vec_fill_clr,
        edgecolor=vec_line_clr,
        linewidth=vec_line_width,
        alpha=vec_fill_alpha,
    )
    ax.set_xlim([bbox[0], bbox[1]])
    ax.set_ylim([bbox[2], bbox[3]])

    if cx_src is not None:
        contextily.add_basemap(
            ax,
            zoom=cx_zoom_lvl,
            crs=gp_vec_sub.crs,
            source=cx_src,
            attribution=cx_attribution,
            attribution_size=cx_att_size,
        )

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    distance_meters = rsgislib.tools.projection.great_circle_distance(
        wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
    )
    ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)

    plt.tight_layout()
    if out_file is None:
        plt.show()
    else:
        plt.savefig(out_file, dpi=out_dpi)


def create_raster_img_map(
    input_img,
    img_bands,
    img_stch,
    bbox=None,
    title_str=None,
    out_file=None,
    out_dpi=400,
    use_grid: bool = False,
    show_map_axis: bool = True,
    fig_x_size: int = 10,
    fig_y_size: int = None,
    img_no_data_val=None,
    stch_min_max_vals=None,
    stch_n_stdevs=2.0,
):
    if bbox is None:
        bbox = rsgislib.imageutils.get_img_bbox(input_img)

    if img_no_data_val is None:
        img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

    if fig_y_size is None:
        x_bbox_size = bbox[1] - bbox[0]
        y_bbox_size = bbox[3] - bbox[2]
        bbox_xy_ratio = y_bbox_size / x_bbox_size
        fig_y_size = fig_x_size * bbox_xy_ratio

    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        input_img, bands=img_bands, bbox=bbox
    )

    if img_stch == rsgislib.IMG_STRETCH_USER:
        if stch_min_max_vals is None:
            raise rsgislib.RSGISPyException(
                "Manual stretch requires user parameters to be passed"
            )
        img_data_strch = rsgislib.tools.plotting.manual_stretch_np_arr(
            img_data, min_max_vals=stch_min_max_vals, no_data_val=img_no_data_val
        )
    elif img_stch == rsgislib.IMG_STRETCH_LINEAR:
        img_data_strch = rsgislib.tools.plotting.linear_stretch_np_arr(
            img_data, no_data_val=img_no_data_val
        )
    elif img_stch == rsgislib.IMG_STRETCH_STDEV:
        img_data_strch = rsgislib.tools.plotting.stdev_stretch_np_arr(
            img_data, n_stdevs=stch_n_stdevs, no_data_val=img_no_data_val
        )
    elif img_stch == rsgislib.IMG_STRECTH_CUMULATIVE:
        img_data_strch = rsgislib.tools.plotting.cumulative_stretch_np_arr(
            img_data, no_data_val=img_no_data_val
        )
    else:
        print("No stretch is being used - is this what you intended?!")
        img_data_strch = img_data

    fig, ax = plt.subplots(1, 1, figsize=(fig_x_size, fig_y_size))
    ax.imshow(img_data_strch, extent=img_coords)
    ax.set_xlim([img_coords[0], img_coords[1]])
    ax.set_ylim([img_coords[2], img_coords[3]])

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    distance_meters = rsgislib.tools.projection.great_circle_distance(
        wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
    )
    ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)

    plt.tight_layout()
    if out_file is None:
        plt.show()
    else:
        plt.savefig(out_file, dpi=out_dpi)


def create_thematic_raster_map(
    input_img: Union[str,List],
    bbox: List[float] = None,
    title_str: str = None,
    cx_src=None,
    alpha_backgd: bool = True,
    out_file: str = None,
    out_dpi: int = 400,
    use_grid: bool = False,
    show_map_axis: bool = True,
    fig_x_size: int = 10,
    fig_y_size: int = None,
    cx_zoom_lvl: int = Union[int, "auto"],
    cx_attribution: Union[str, bool] = None,
    cx_att_size: int = 8,
):
    if cx_src is not None:
        import contextily

    input_imgs = list()
    if isinstance(input_img, str):
        input_imgs.append(input_img)
    else:
        input_imgs = input_img

    if bbox is None:
        bbox = rsgislib.imageutils.get_img_bbox(input_imgs[0])

    img_epsg = rsgislib.imageutils.get_epsg_proj_from_img(input_imgs[0])

    img_data_arr_scns = list()
    img_coords_scns = list()
    lgd_patches_scns = list()
    for input_img in input_imgs:
        (
            img_data_arr,
            img_coords,
            lgd_patches,
        ) = rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(
            input_img, band=1, bbox=bbox, alpha_lyr=alpha_backgd
        )
        img_data_arr_scns.append(img_data_arr)
        img_coords_scns.append(img_coords)
        lgd_patches_scns.append(lgd_patches)

    if fig_y_size is None:
        x_bbox_size = bbox[1] - bbox[0]
        y_bbox_size = bbox[3] - bbox[2]
        bbox_xy_ratio = y_bbox_size / x_bbox_size
        fig_y_size = fig_x_size * bbox_xy_ratio

    fig, ax = plt.subplots(1, 1, figsize=(fig_x_size, fig_y_size))

    ax.set_xlim([img_coords_scns[0][0], img_coords_scns[0][1]])
    ax.set_ylim([img_coords_scns[0][2], img_coords_scns[0][3]])

    if cx_src is not None:
        contextily.add_basemap(
            ax,
            zoom=cx_zoom_lvl,
            crs=f"epsg:{img_epsg}",
            source=cx_src,
            attribution=cx_attribution,
            attribution_size=cx_att_size,
        )
    for img_data_arr, img_coords in zip(img_data_arr_scns, img_coords_scns):
        ax.imshow(img_data_arr, extent=img_coords)

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    distance_meters = rsgislib.tools.projection.great_circle_distance(
        wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
    )
    ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)

    plt.tight_layout()
    if out_file is None:
        plt.show()
    else:
        plt.savefig(out_file, dpi=out_dpi)

def create_thematic_raster_img_base_map(
    in_base_img: str,
    img_bands:List[int],
    img_stch:int,
    in_them_img:Union[str,List],
    bbox: List[float] = None,
    title_str: str = None,
    alpha_backgd: bool = True,
    out_file: str = None,
    out_dpi: int = 400,
    use_grid: bool = False,
    show_map_axis: bool = True,
    fig_x_size: int = 10,
    fig_y_size: int = None,
    img_no_data_val=None,
    stch_min_max_vals=None,
    stch_n_stdevs=2.0,
):
    if img_no_data_val is None:
        img_no_data_val = rsgislib.imageutils.get_img_no_data_value(in_base_img)

    if bbox is None:
        bbox = rsgislib.imageutils.get_img_bbox(in_base_img)

    in_them_imgs = list()
    if isinstance(in_them_img, str):
        in_them_imgs.append(in_them_img)
    else:
        in_them_imgs = in_them_img

    img_data_arr_scns = list()
    img_coords_scns = list()
    lgd_patches_scns = list()
    for in_them_img in in_them_imgs:
        (
            img_data_arr,
            img_coords,
            lgd_patches,
        ) = rsgislib.tools.plotting.get_gdal_thematic_raster_mpl_imshow(
            in_them_img, band=1, bbox=bbox, alpha_lyr=alpha_backgd
        )
        img_data_arr_scns.append(img_data_arr)
        img_coords_scns.append(img_coords)
        lgd_patches_scns.append(lgd_patches)

    img_data, img_coords = rsgislib.tools.plotting.get_gdal_raster_mpl_imshow(
        in_base_img, bands=img_bands, bbox=bbox
    )

    if img_stch == rsgislib.IMG_STRETCH_USER:
        if stch_min_max_vals is None:
            raise rsgislib.RSGISPyException(
                "Manual stretch requires user parameters to be passed"
            )
        img_data_strch = rsgislib.tools.plotting.manual_stretch_np_arr(
            img_data, min_max_vals=stch_min_max_vals, no_data_val=img_no_data_val
        )
    elif img_stch == rsgislib.IMG_STRETCH_LINEAR:
        img_data_strch = rsgislib.tools.plotting.linear_stretch_np_arr(
            img_data, no_data_val=img_no_data_val
        )
    elif img_stch == rsgislib.IMG_STRETCH_STDEV:
        img_data_strch = rsgislib.tools.plotting.stdev_stretch_np_arr(
            img_data, n_stdevs=stch_n_stdevs, no_data_val=img_no_data_val
        )
    elif img_stch == rsgislib.IMG_STRECTH_CUMULATIVE:
        img_data_strch = rsgislib.tools.plotting.cumulative_stretch_np_arr(
            img_data, no_data_val=img_no_data_val
        )
    else:
        print("No stretch is being used - is this what you intended?!")
        img_data_strch = img_data

    if fig_y_size is None:
        x_bbox_size = bbox[1] - bbox[0]
        y_bbox_size = bbox[3] - bbox[2]
        bbox_xy_ratio = y_bbox_size / x_bbox_size
        fig_y_size = fig_x_size * bbox_xy_ratio

    fig, ax = plt.subplots(1, 1, figsize=(fig_x_size, fig_y_size))

    # base image data
    ax.imshow(img_data_strch, extent=img_coords)
    ax.set_xlim([img_coords[0], img_coords[1]])
    ax.set_ylim([img_coords[2], img_coords[3]])

    # thematic data
    for img_data_arr, img_therm_coords in zip(img_data_arr_scns, img_coords_scns):
        ax.imshow(img_data_arr, extent=img_therm_coords)



    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    distance_meters = rsgislib.tools.projection.great_circle_distance(
        wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
    )
    ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)

    plt.tight_layout()
    if out_file is None:
        plt.show()
    else:
        plt.savefig(out_file, dpi=out_dpi)
