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

import geopandas

import matplotlib.pyplot as plt
import matplotlib.colors
import matplotlib.cm
from matplotlib_scalebar.scalebar import ScaleBar


def calc_y_fig_size(bbox: List[float], fig_x_size=Union[int, float]) -> float:
    """
    A function which calculates the y axis size give the bbox and x axis size
    and the BBOX for the area to be mapped. This is useful so the map fills
    the whole area rather than having lots of white space in the resulting
    figure image file.

    :param bbox: the (MinX, MaxX, MinY, MaxY) for the region of interest.
    :param fig_x_size: the size for the figure in the x axis.
    :return: the figure size in the y axis.

    """
    x_bbox_size = bbox[1] - bbox[0]
    y_bbox_size = bbox[3] - bbox[2]
    bbox_xy_ratio = y_bbox_size / x_bbox_size
    fig_y_size = fig_x_size * bbox_xy_ratio
    return fig_y_size


def get_overview_info(
    roi_bbox: List[float],
    roi_epsg: int = 4236,
    overview_buf: int = 30,
    out_epsg: int = 4236,
) -> (List[float], List[float]):
    """
    A function which uses the BBOX for the extent of the map being drawn to create
    the a BBOX for and centre point which can be used within the add_overview_maps
    function to add an overview map to an axis.

    :param roi_bbox: the region of interest (ROI) bbox (MinX, MaxX, MinY, MaxY)
                     representing the area for the which the map is being drawn
                     for.
    :param roi_epsg: the project (as an EPSG code) for the coordinates provided in
                     the roi_bbox.
    :param overview_buf: the buffer used to specify the overview area. This is taken
                         from the centre point of the roi_bbox.
    :return: the overview bbox (MinX, MaxX, MinY, MaxY) and point (X, Y)
    """
    import rsgislib.tools.geometrytools

    if roi_epsg != out_epsg:
        roi_proj_bbox = rsgislib.tools.geometrytools.reproj_bbox_epsg(
            roi_bbox, roi_epsg, out_epsg
        )
    else:
        roi_proj_bbox = roi_bbox

    c_pt_x = roi_proj_bbox[0] + ((roi_proj_bbox[1] - roi_proj_bbox[0]) / 2)
    c_pt_y = roi_proj_bbox[2] + ((roi_proj_bbox[3] - roi_proj_bbox[2]) / 2)
    over_min_x = c_pt_x - overview_buf
    over_max_x = c_pt_x + overview_buf
    over_min_y = c_pt_y - overview_buf
    over_max_y = c_pt_y + overview_buf

    return [over_min_x, over_max_x, over_min_y, over_max_y], [c_pt_x, c_pt_y]


def add_overview_maps(
    ax: plt.axis,
    overview_lyr: geopandas.GeoDataFrame,
    overview_bbox: List[float],
    overview_pt: List[float] = None,
    over_size_x: float = 0.1,
    over_size_y: float = 0.1,
    over_x_off: float = 0.0,
    over_y_off: float = 0.0,
    pt_clr="red",
    pt_size=10,
    fill_clr="white",
    line_clr="black",
    line_width=0.25,
):
    """
    Add an overview map in the top-left corner of the map.

    :param ax: The matplotlib axis to which to add the overview map.
    :param overview_lyr: The geopandas layer to used used for the overview map
                         usually this would be a global vector layer of the countries
                         but could be anything.
    :param overview_bbox: the bbox (MinX, MaxX, MinY, MaxY) for the area to be shown
                          within the overview map.
    :param overview_pt: an optional point (X, Y) to displayed on the overview map.
    :param over_size_x: the size of the overview map in the x-axis
    :param over_size_y: the size of the overview map in the y-axis
    :param over_x_off: the x-axis offset for the overmap position
    :param over_y_off: the y-axis offset for the overmap position
    :param pt_clr: the colour of the point (if specified). Default: Red.
    :param pt_size: the size of the point (if specified). Default: 10.
    :param fill_clr: the colour used to fill the polygons within the overview layer.
                     Default: white
    :param line_clr: the colour of the lines of the overview layer. Default: black
    :param line_width: the line width of the lines of the overview layer. Default: 0.25

    """
    ax_over = ax.inset_axes(
        [over_x_off, ((1.0 - over_size_y) + over_y_off), over_size_x, over_size_y]
    )
    ax_over.set_xticks([])
    ax_over.set_yticks([])
    overview_lyr.plot(
        ax=ax_over, color=fill_clr, edgecolor=line_clr, linewidth=line_width, alpha=1
    )
    if overview_pt is not None:
        ax_over.scatter(overview_pt[0], overview_pt[1], color=pt_clr, s=pt_size)
    ax_over.set_xlim([overview_bbox[0], overview_bbox[1]])
    ax_over.set_ylim([overview_bbox[2], overview_bbox[3]])


def add_contextily_basemap(
    ax: plt.axis,
    gp_ref_vec: geopandas.GeoDataFrame,
    cx_src,
    cx_zoom_lvl: int = Union[int, "auto"],
    cx_attribution: Union[str, bool] = None,
    cx_att_size: int = 8,
):
    import contextily

    contextily.add_basemap(
        ax,
        zoom=cx_zoom_lvl,
        crs=gp_ref_vec.crs,
        source=cx_src,
        attribution=cx_attribution,
        attribution_size=cx_att_size,
    )


def create_vec_lyr_map(
    ax: plt.axis,
    gp_vecs: Union[geopandas.GeoDataFrame, List[geopandas.GeoDataFrame]],
    bbox: List[float],
    title_str: str = None,
    vec_fill_clrs: Union[str, List[str]] = "grey",
    vec_line_clrs: Union[str, List[str]] = "black",
    vec_line_widths: Union[float, List[float]] = 0.25,
    vec_fill_alphas: Union[float, List[float]] = 1.0,
    show_scale_bar: bool = True,
    use_grid: bool = False,
    show_map_axis: bool = True,
    sub_in_vec: bool = False,
):
    """

    :param ax:
    :param gp_vecs:
    :param bbox:
    :param title_str:
    :param cx_src:
    :param vec_fill_clrs:
    :param vec_line_clrs:
    :param vec_line_widths:
    :param vec_fill_alphas:
    :param show_scale_bar:
    :param use_grid:
    :param show_map_axis:
    :param sub_in_vec:
    :param cx_zoom_lvl:
    :param cx_attribution:
    :param cx_att_size:
    :return:
    """
    n_vec_lyrs = 1
    if type(gp_vecs) is list:
        n_vec_lyrs = len(gp_vecs)
    else:
        # Create list of one
        gp_vecs = [gp_vecs]

    for i, gp_vec in enumerate(gp_vecs):
        if sub_in_vec:
            min_x_sub = math.floor(bbox[0] - 4)
            max_x_sub = math.ceil(bbox[1] + 4)
            min_y_sub = math.floor(bbox[2] - 4)
            max_y_sub = math.ceil(bbox[3] + 4)
            gp_vec_sub = gp_vec.cx[min_x_sub:max_x_sub, min_y_sub:max_y_sub]
        else:
            gp_vec_sub = gp_vec

        if type(vec_fill_clrs) is list:
            if len(vec_fill_clrs) != n_vec_lyrs:
                raise rsgislib.RSGISPyException(
                    "Number of fill colours not same as number of vector layers"
                )
            vec_fill_clr = vec_fill_clrs[i]
        else:
            vec_fill_clr = vec_fill_clrs

        if type(vec_line_clrs) is list:
            if len(vec_fill_clrs) != n_vec_lyrs:
                raise rsgislib.RSGISPyException(
                    "Number of line colours not same as number of vector layers"
                )
            vec_line_clr = vec_line_clrs[i]
        else:
            vec_line_clr = vec_line_clrs

        if type(vec_line_widths) is list:
            if len(vec_fill_clrs) != n_vec_lyrs:
                raise rsgislib.RSGISPyException(
                    "Number of line widths not same as number of vector layers"
                )
            vec_line_width = vec_line_widths[i]
        else:
            vec_line_width = vec_line_widths

        if type(vec_fill_alphas) is list:
            if len(vec_fill_clrs) != n_vec_lyrs:
                raise rsgislib.RSGISPyException(
                    "Number of fill alphas not same as number of vector layers"
                )
            vec_fill_alpha = vec_fill_alphas[i]
        else:
            vec_fill_alpha = vec_fill_alphas

        gp_vec_sub.plot(
            ax=ax,
            color=vec_fill_clr,
            edgecolor=vec_line_clr,
            linewidth=vec_line_width,
            alpha=vec_fill_alpha,
        )
    ax.set_xlim([bbox[0], bbox[1]])
    ax.set_ylim([bbox[2], bbox[3]])

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    if show_scale_bar:
        distance_meters = rsgislib.tools.projection.great_circle_distance(
            wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
        )
        ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)


def create_raster_img_map(
    ax: plt.axis,
    input_img: str,
    img_bands: List[int],
    img_stch: int,
    bbox=None,
    title_str=None,
    show_scale_bar: bool = True,
    use_grid: bool = False,
    show_map_axis: bool = True,
    img_no_data_val=None,
    stch_min_max_vals=None,
    stch_n_stdevs=2.0,
):
    """

    :param ax:
    :param input_img:
    :param img_bands:
    :param img_stch:
    :param bbox:
    :param title_str:
    :param show_scale_bar:
    :param use_grid:
    :param show_map_axis:
    :param img_no_data_val:
    :param stch_min_max_vals:
    :param stch_n_stdevs:
    :return:
    """
    if bbox is None:
        bbox = rsgislib.imageutils.get_img_bbox(input_img)

    if img_no_data_val is None:
        img_no_data_val = rsgislib.imageutils.get_img_no_data_value(input_img)

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

    ax.imshow(img_data_strch, extent=img_coords)
    ax.set_xlim([img_coords[0], img_coords[1]])
    ax.set_ylim([img_coords[2], img_coords[3]])

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    if show_scale_bar:
        distance_meters = rsgislib.tools.projection.great_circle_distance(
            wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
        )
        ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)


def create_thematic_raster_map(
    ax: plt.axis,
    input_img: Union[str, List],
    bbox: List[float] = None,
    title_str: str = None,
    alpha_backgd: bool = True,
    show_scale_bar: bool = True,
    use_grid: bool = False,
    show_map_axis: bool = True,
):
    """

    :param ax:
    :param input_img:
    :param bbox:
    :param title_str:
    :param cx_src:
    :param alpha_backgd:
    :param show_scale_bar:
    :param use_grid:
    :param show_map_axis:
    :param cx_zoom_lvl:
    :param cx_attribution:
    :param cx_att_size:
    :return:
    """
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

    ax.set_xlim([img_coords_scns[0][0], img_coords_scns[0][1]])
    ax.set_ylim([img_coords_scns[0][2], img_coords_scns[0][3]])

    for img_data_arr, img_coords in zip(img_data_arr_scns, img_coords_scns):
        ax.imshow(img_data_arr, extent=img_coords)

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    if show_scale_bar:
        distance_meters = rsgislib.tools.projection.great_circle_distance(
            wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
        )
        ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)


def create_choropleth_vec_lyr_map(
    ax: plt.axis,
    gp_vec: geopandas.GeoDataFrame,
    vec_col: str,
    bbox: List[float],
    title_str: str = None,
    vec_fill_cmap: matplotlib.colors.Colormap = None,
    vec_var_norm: matplotlib.colors.Normalize = None,
    vec_line_clr: str = "black",
    vec_line_width: float = 0.25,
    vec_fill_alpha: float = 1.0,
    show_scale_bar: bool = True,
    use_grid: bool = False,
    show_map_axis: bool = True,
    sub_in_vec: bool = False,
):
    """

    :param ax:
    :param gp_vec:
    :param vec_col:
    :param bbox:
    :param title_str:
    :param vec_fill_cmap:
    :param vec_var_norm:
    :param vec_line_clr:
    :param vec_line_width:
    :param vec_fill_alpha:
    :param show_scale_bar:
    :param use_grid:
    :param show_map_axis:
    :param sub_in_vec:

    """
    if vec_fill_cmap is not None:
        vec_fill_cmap = matplotlib.cm.get_cmap("viridis")

    if sub_in_vec:
        min_x_sub = math.floor(bbox[0] - 4)
        max_x_sub = math.ceil(bbox[1] + 4)
        min_y_sub = math.floor(bbox[2] - 4)
        max_y_sub = math.ceil(bbox[3] + 4)
        gp_vec_sub = gp_vec.cx[min_x_sub:max_x_sub, min_y_sub:max_y_sub]
    else:
        gp_vec_sub = gp_vec

    gp_vec_sub.plot(
        column=vec_col,
        ax=ax,
        cmap=vec_fill_cmap,
        norm=vec_var_norm,
        edgecolor=vec_line_clr,
        linewidth=vec_line_width,
        alpha=vec_fill_alpha,
    )
    ax.set_xlim([bbox[0], bbox[1]])
    ax.set_ylim([bbox[2], bbox[3]])

    if use_grid:
        ax.grid()

    if not show_map_axis:
        ax.set_axis_off()
        ax.set_axis_off()

    if show_scale_bar:
        distance_meters = rsgislib.tools.projection.great_circle_distance(
            wgs84_p1=[bbox[0], bbox[3]], wgs84_p2=[bbox[0] + 1, bbox[3]]
        )
        ax.add_artist(ScaleBar(distance_meters))

    if title_str is not None:
        ax.title.set_text(title_str)
