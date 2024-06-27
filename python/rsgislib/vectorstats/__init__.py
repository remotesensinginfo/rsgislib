#!/usr/bin/env python
"""
The vector stats module provides tools to calculate statistics on vector datasets
"""

import numpy
import rsgislib.vectorutils


def calc_empirical_variogram(
    vec_file: str,
    vec_lyr: str,
    vals_col: str,
    pts_sel_col: str = None,
    pts_sel_val: int = None,
    out_data_file: str = None,
    out_plot_file: str = None,
    max_lag: float | str = "median",
    n_lags: int = 25,
    normalize_vals: bool = False,
):
    """
    A function which calculates variogram for the vector layer provided using the
    skgstat module (https://scikit-gstat.readthedocs.io/). The layer needs to be
    use point geometries and projected so they have X and Y coordinates.

    :param vec_file: Input vector layer file
    :param vec_lyr: Input vector layer name
    :param vals_col: Column name with values to calculate variogram
    :param pts_sel_col: Optionally (Default: None) column name used to subset the
                        points within the vector layer for which the variogram is
                        calculated (e.g., just points within a particular angular
                        range)
    :param pts_sel_val: Optionally (Default: None) value to subset the points within
                        the vector layer using the pts_sel_col.
    :param out_data_file: Optionally output a CSV file with the lag_bins, variance
                          and count. Default is None but if file path provided the
                          output will be produced.
    :param out_plot_file: Optionally output a plot file of the lag_bins, variance
                          and count. Default is None but if file path provided the
                          output will be produced.
    :param max_lag: Can specify the maximum lag distance directly by giving a value
                    larger than 1. Can also be a string with value ‘median’, ‘mean’.
                    See skgstat.Variogram documentation.
    :param n_lags: Specify the number of lag classes to be defined by the binning
                   function. See skgstat.Variogram documentation.
    :param normalize_vals: Defaults to False. If True, the independent and dependent
                           variable will be normalized to the range [0,1].
                           See skgstat.Variogram documentation.
    :return: returns a pandas dataframe with the lag_bins, variance and count

    """
    import geopandas
    import rsgislib.tools.stats

    vec_geom_type = rsgislib.vectorutils.get_vec_lyr_geom_type(vec_file, vec_lyr)
    if vec_geom_type != rsgislib.GEOM_PT:
        raise Exception("Vector geometry type must be points.")

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    geom_pts = data_gdf["geometry"].get_coordinates()

    if not {"x", "y"}.issubset(geom_pts.columns):
        raise Exception(
            "The geometry is not as expected - need x and y fields. "
            "Might be in degrees (e.g., EPSG:4326)"
        )

    x = data_gdf["geometry"].x.values
    y = data_gdf["geometry"].y.values
    vals = data_gdf[vals_col].values

    if (pts_sel_col is not None) and (pts_sel_val is not None):
        pt_sets = data_gdf[pts_sel_col].values

        x = x[pt_sets == pts_sel_val]
        y = y[pt_sets == pts_sel_val]
        vals = vals[pt_sets == pts_sel_val]

    pts_coords = numpy.stack([x, y]).T

    vario_out_df = rsgislib.tools.stats.calc_variogram(
        pts_coords=pts_coords,
        data_vals=vals,
        out_data_file=out_data_file,
        out_plot_file=out_plot_file,
        max_lag=max_lag,
        n_lags=n_lags,
        normalize_vals=normalize_vals,
    )
    return vario_out_df
