#!/usr/bin/env python
"""
Functions implementing species distribution modelling.
"""
from typing import Dict, List, Tuple, Any
import os

import rsgislib
import numpy
from osgeo import gdal, ogr
from rios import applier
from sklearn.base import BaseEstimator
from sklearn.model_selection._search import BaseSearchCV

GEOPANDAS_AVAIL = True
try:
    import geopandas
except ImportError:
    GEOPANDAS_AVAIL = False

PANDAS_AVAIL = True
try:
    import pandas
except ImportError:
    PANDAS_AVAIL = False

TQDM_AVAIL = True
try:
    import tqdm
except ImportError:
    import rios.cuiprogress

    TQDM_AVAIL = False


class EnvVarInfo(object):
    """
    This is a class to store the defining the parameters for each environment variable.

    :param name: Name of the variable.
    :param file: Image file path for the variable.
    :param band: Band in the image representing the variable.
    :param data_type: Variable type (Either rsgislib.VAR_TYPE_CONTINUOUS or
                 rsgislib.VAR_TYPE_CATEGORICAL).
    :param min_vld_val: The minimum valid value for the variable.
    :param max_vld_val: The maximum valid value for the variable.
    :param norm_mean: Mean value for normalisation.
    :param norm_std: standard deviation value for normalisation.

    """

    def __init__(
        self,
        name: str = None,
        file: str = None,
        band: int = 1,
        data_type: int = rsgislib.VAR_TYPE_UNDEFINED,
        min_vld_val: float = None,
        max_vld_val: float = None,
        norm_mean: float = None,
        norm_std: float = None,
    ):
        """

        :param name: Name of the variable.
        :param file: Image file path for the variable.
        :param band: Band in the image representing the variable.
        :param data_type: Variable type (Either rsgislib.VAR_TYPE_CONTINUOUS or
                          rsgislib.VAR_TYPE_CATEGORICAL).
        :param min_vld_val: The minimum valid value for the variable.
        :param max_vld_val: The maximum valid value for the variable.
        :param norm_mean: Mean value for normalisation.
        :param norm_std: standard deviation value for normalisation.

        """
        self.name = name
        self.file = file
        self.band = band
        self.data_type = data_type
        self.min_vld_val = min_vld_val
        self.max_vld_val = max_vld_val
        self.norm_mean = norm_mean
        self.norm_std = norm_std

    def set_norm_mean(self, norm_mean: float):
        self.norm_mean = norm_mean

    def set_norm_std(self, norm_std: float):
        self.norm_std = norm_std

    def set_norm_mean_std(self, norm_mean: float, norm_std: float):
        self.norm_mean = norm_mean
        self.norm_std = norm_std

    def __str__(self):
        type_str = "Unknown"
        if self.data_type == rsgislib.VAR_TYPE_CONTINUOUS:
            type_str = "Continuous"
        elif self.data_type == rsgislib.VAR_TYPE_CATEGORICAL:
            type_str = "Categorical"
        return (
            f"{self.name}: {self.file} (Band: {self.band}) (Type: {type_str}) "
            f"(Range: {self.min_vld_val}, {self.max_vld_val}) "
            f"(Normalisation: (x - {self.norm_mean}) / {self.norm_std})"
        )

    def __repr__(self):
        type_str = "Unknown"
        if self.data_type == rsgislib.VAR_TYPE_CONTINUOUS:
            type_str = "Continuous"
        elif self.data_type == rsgislib.VAR_TYPE_CATEGORICAL:
            type_str = "Categorical"
        return (
            f"{self.name}: {self.file} (Band: {self.band}) (Type: {type_str}) "
            f"(Range: {self.min_vld_val}, {self.max_vld_val}) "
            f"(Normalisation: (x - {self.norm_mean}) / {self.norm_std})"
        )


def gen_pseudo_absences_smpls(
    in_msk_img: str,
    img_msk_val: int,
    out_vec_file: str,
    out_vec_lyr: str,
    n_smpls: int = 10000,
    xtra_n_smpls: int = 30000,
    presence_smpls_vec_file: str = None,
    presence_smpls_vec_lyr: str = None,
    presence_smpls_dist_thres: float = 1000,
    out_format: str = "GeoJSON",
    rnd_seed: int = None,
):
    """
    A function which generates pseudo absences samples for species distribution
    modelling. Note, it is expected that all input files have the same projection.

    :param in_msk_img: File path to a valid mask image which defines the region
                       of interest.
    :param img_msk_val: The value within the mask image which defines the region
                        of interest
    :param out_vec_file: Output vector file path
    :param out_vec_lyr: Output vector layer name
    :param n_smpls: the maximum number of samples to be outputted
    :param xtra_n_smpls: the number of extra samples to be produced internally so
                         after the various masking steps the final number is likely
                         to be near or higher than n_smpls
    :param presence_smpls_vec_file: Optionally a set of presence samples can be
                                   provided and this is the file path to that file.
                                   If None then ignored (Default: None).
    :param presence_smpls_vec_lyr: Optionally a set of presence samples can be
                                  provided and this the layer name of the vector.
    :param presence_smpls_dist_thres: If provided this is a distance threshold
                                     to presence samples below which absences
                                     points are removed. Unit is the same as the
                                     projection of the input files.
    :param out_format: The output vector format (Default: GeoJSON)
    :param rnd_seed: Optionally provide a random seed for the random number generator.
                     Default: None.

    """
    import rsgislib.imageutils
    import rsgislib.vectorattrs
    import rsgislib.vectorutils.createvectors
    import rsgislib.vectorutils
    import rsgislib.zonalstats

    img_bbox = rsgislib.imageutils.get_img_bbox(in_msk_img)
    epsg_code = rsgislib.imageutils.get_epsg_proj_from_img(in_msk_img)

    rsgislib.vectorutils.createvectors.create_random_pts_in_bbox(
        img_bbox,
        n_pts=(n_smpls + xtra_n_smpls),
        epsg_code=epsg_code,
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format=out_format,
        rnd_seed=rnd_seed,
    )

    tmp_col_name = "tmp_msk"

    rsgislib.zonalstats.ext_point_band_values_file(
        vec_file=out_vec_file,
        vec_lyr=out_vec_lyr,
        input_img=in_msk_img,
        img_band=1,
        min_thres=-9999,
        max_thres=9999,
        out_no_data_val=-9999,
        out_field=tmp_col_name,
        reproj_vec=False,
        vec_def_epsg=None,
    )

    rsgislib.vectorutils.subset_by_attribute(
        vec_file=out_vec_file,
        vec_lyr=out_vec_lyr,
        sub_col=tmp_col_name,
        sub_vals=[img_msk_val],
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format=out_format,
        match_type="equals",
    )

    if presence_smpls_vec_file is not None:
        # calc distance to presence_smpls_vec_lyr
        rsgislib.vectorattrs.calc_near_dist_to_feats(
            vec_file=out_vec_file,
            vec_lyr=out_vec_lyr,
            vec_dist_file=presence_smpls_vec_file,
            vec_dist_lyr=presence_smpls_vec_lyr,
            out_vec_file=out_vec_file,
            out_vec_lyr=out_vec_lyr,
            out_col="presents_dist",
            out_format=out_format,
        )
        dist_col_arr = numpy.array(
            rsgislib.vectorattrs.read_vec_column(
                vec_file=out_vec_file, vec_lyr=out_vec_lyr, att_column="presents_dist"
            )
        )
        dist_sel_arr = numpy.where(dist_col_arr > presence_smpls_dist_thres, 1, 0)

        rsgislib.vectorattrs.write_vec_column(
            out_vec_file=out_vec_file,
            out_vec_lyr=out_vec_lyr,
            att_column="dist_sel",
            att_col_datatype=ogr.OFTInteger,
            att_col_data=dist_sel_arr.tolist(),
        )

        rsgislib.vectorutils.subset_by_attribute(
            vec_file=out_vec_file,
            vec_lyr=out_vec_lyr,
            sub_col="dist_sel",
            sub_vals=[1],
            out_vec_file=out_vec_file,
            out_vec_lyr=out_vec_lyr,
            out_format=out_format,
            match_type="equals",
        )

    n_ran_smpls = rsgislib.vectorutils.get_vec_feat_count(
        vec_file=out_vec_file, vec_lyr=out_vec_lyr, compute_count=True
    )

    if n_ran_smpls > n_smpls:
        rsgislib.vectorutils.vec_lyr_random_subset(
            vec_file=out_vec_file,
            vec_lyr=out_vec_lyr,
            out_vec_file=out_vec_file,
            out_vec_lyr=out_vec_lyr,
            n_smpl=n_smpls,
            out_format=out_format,
            rnd_seed=rnd_seed,
        )

    rsgislib.vectorattrs.drop_vec_cols(
        vec_file=out_vec_file,
        vec_lyr=out_vec_lyr,
        drop_cols=[tmp_col_name],
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format=out_format,
        chk_cols_present=True,
    )


def extract_env_var_data(
    env_vars: Dict[str, EnvVarInfo],
    smpls_vec_file: str,
    smpls_vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    out_no_data_val: float = -9999,
    replace: bool = False,
):
    """
    A function which extracts samples for the environment variables for a set
    of samples (either presences or absences).

    :param env_vars: A dictionary of environment variables.
    :param smpls_vec_file: path to the vector file.
    :param smpls_vec_lyr: layer name of the vector file.
    :param out_vec_file: output vector file populated with environment variables.
    :param out_vec_lyr: output vector layer name.
    :param out_format: output vector file format (Default: GPKG)
    :param out_no_data_val: output no data value (Default: -9999)

    """
    import rsgislib.vectorutils
    import rsgislib.zonalstats

    if replace and os.path.exists(out_vec_file):
        rsgislib.vectorutils.delete_vector_file(out_vec_file, feedback=True)

    vec_ds, vec_lyr = rsgislib.vectorutils.read_vec_lyr_to_mem(
        smpls_vec_file, smpls_vec_lyr
    )

    for var in env_vars:
        print(f"Processing '{var}':")
        rsgislib.zonalstats.ext_point_band_values(
            vec_lyr_obj=vec_lyr,
            input_img=env_vars[var].file,
            img_band=env_vars[var].band,
            min_thres=env_vars[var].min_vld_val,
            max_thres=env_vars[var].max_vld_val,
            out_no_data_val=out_no_data_val,
            out_field=env_vars[var].name,
            reproj_vec=False,
            vec_def_epsg=None,
        )

    rsgislib.vectorutils.write_vec_lyr_to_file(
        vec_lyr_obj=vec_lyr,
        out_vec_file=out_vec_file,
        out_vec_lyr=out_vec_lyr,
        out_format=out_format,
        options=[],
        replace=True,
    )
    vec_ds = None


def combine_presence_absence_data(
    presence_smpls_vec_file: str,
    presence_smpls_vec_lyr: str,
    absence_smpls_vec_file: str,
    absence_smpls_vec_lyr: str,
    env_vars: Dict[str, EnvVarInfo],
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
    equalise_smpls: bool = False,
    cls_col: str = "clsid",
    rnd_seed: int = None,
) -> List[str]:
    """
    A function which combines the presence and absence data into a single set
    with the option to equalise the number of samples within the two classes.
    The output file will only have the columns listed within env_vars and the
    classification column.

    :param presence_smpls_vec_file: path to the vector file with the presence data.
    :param presence_smpls_vec_lyr: layer name of the vector file with the presence data.
    :param absence_smpls_vec_file: path to the vector file with the absence data.
    :param absence_smpls_vec_lyr: layer name of the vector file with the absence data.
    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data.
    :param out_vec_file: the output vector file populated with presence and
                         absence data.
    :param out_vec_lyr: the output vector layer name.
    :param out_format: the output vector file format (Default: GPKG)
    :param equalise_smpls: optionally decide whether to equalise the number of samples.
                           This would normally be done if you are using a tree based
                           modelling (e.g., random forests)
    :param cls_col: the name of the classification column.
    :param rnd_seed: A seed for the random selection. Default: None.
    :return: a list of the variables the order in which they are present.

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    if not PANDAS_AVAIL:
        raise ImportError("Pandas is not available")
    import rsgislib.tools.filetools

    presence_gdf = geopandas.read_file(
        presence_smpls_vec_file, layer=presence_smpls_vec_lyr
    )
    absence_gdf = geopandas.read_file(
        absence_smpls_vec_file, layer=absence_smpls_vec_lyr
    )

    # Get the list of environmental variable columns
    analysis_vars = list()
    for var in env_vars:
        analysis_vars.append(env_vars[var].name)

    # Drop any columns not needed from presences data
    presence_cols_drop = list()
    for col in presence_gdf.columns:
        if (col not in analysis_vars) and (col != "geometry"):
            presence_cols_drop.append(col)

    if len(presence_cols_drop) > 0:
        print(f"Dropping from presence: {presence_cols_drop}")
        presence_gdf.drop(columns=presence_cols_drop, inplace=True)

    # Drop any columns not needed from absence data
    absence_cols_drop = list()
    for col in absence_gdf.columns:
        if (col not in analysis_vars) and (col != "geometry"):
            absence_cols_drop.append(col)

    if len(absence_cols_drop) > 0:
        print(f"Dropping from absence: {absence_cols_drop}")
        absence_gdf.drop(columns=absence_cols_drop, inplace=True)

    # Check all environmental variable columns are present: presence data.
    presence_data_cols = list(presence_gdf.columns)
    for col in analysis_vars:
        if col not in presence_data_cols:
            raise rsgislib.RSGISPyException(f"'{col}' is not within the presence data")

    # Check all environmental variable columns are present: absence data.
    absence_data_cols = list(absence_gdf.columns)
    for col in analysis_vars:
        if col not in absence_data_cols:
            raise rsgislib.RSGISPyException(f"'{col}' is not within the absence data")

    presence_gdf[cls_col] = numpy.ones((presence_gdf.shape[0]), dtype=int)
    absence_gdf[cls_col] = numpy.zeros((absence_gdf.shape[0]), dtype=int)

    if equalise_smpls:
        n_presence_smpls = presence_gdf.shape[0]
        n_absence_smpls = absence_gdf.shape[0]

        if n_presence_smpls > n_absence_smpls:
            print(
                f"Equalising number of samples: sampling presense to {n_absence_smpls}"
            )
            presence_gdf = presence_gdf.sample(
                n=n_absence_smpls, random_state=rnd_seed, axis=0
            )
        elif n_absence_smpls > n_presence_smpls:
            print(
                f"Equalising number of samples: sampling absence to {n_presence_smpls}"
            )
            absence_gdf = absence_gdf.sample(
                n=n_presence_smpls, random_state=rnd_seed, axis=0
            )

    data_gdf = pandas.concat([presence_gdf, absence_gdf])

    if out_format == "GPKG":
        if out_vec_lyr is None:
            out_vec_lyr = rsgislib.tools.filetools.get_file_basename(
                out_vec_file, check_valid=True
            )
        data_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gdf.to_file(out_vec_file, driver=out_format)

    return analysis_vars


def create_train_test_sets(
    vec_file: str,
    vec_lyr: str,
    train_vec_file: str,
    train_vec_lyr: str,
    test_vec_file: str,
    test_vec_lyr: str,
    split_prop: float = 0.8,
    cls_col: str = "clsid",
    out_format: str = "GPKG",
    rnd_seed: int = None,
):
    """
    A function which splits the input vector layer into training and testing sets.
    The presences (1) and absences (0) are processed separately so the proportion
    of the presence (1) and absence (0) are maintained in the training and testing
    sets.

    :param vec_file: Input vector file path.
    :param vec_lyr: Input vector layer name.
    :param train_vec_file: Output training vector file path.
    :param train_vec_lyr: Output training vector layer name.
    :param test_vec_file: Output testing vector file path.
    :param test_vec_lyr: Output testing vector layer name.
    :param split_prop: The proportion of the data to be used as the training set.
                       Default is 0.8 (i.e., 80 % for training and 20 % for testing).
    :param cls_col: the classification column name.
    :param out_format: the output vector file format
    :param rnd_seed: Optionally provide a random seed for the random number generator.
                     Default: None.

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    if not PANDAS_AVAIL:
        raise ImportError("Pandas is not available")
    import rsgislib.tools.filetools

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    presence_gdf = data_gdf[data_gdf[cls_col] == 1]
    absence_gdf = data_gdf[data_gdf[cls_col] == 0]

    n_presence_smpls = presence_gdf.shape[0]
    n_absence_smpls = absence_gdf.shape[0]

    n_train_presence_smpls = int(n_presence_smpls * split_prop)
    n_train_absence_smpls = int(n_absence_smpls * split_prop)

    train_presence_gdf = presence_gdf.sample(
        n=n_train_presence_smpls, random_state=rnd_seed
    )
    test_presence_gdf = presence_gdf.drop(train_presence_gdf.index)

    train_absence_gdf = absence_gdf.sample(
        n=n_train_absence_smpls, random_state=rnd_seed
    )
    test_absence_gdf = absence_gdf.drop(train_absence_gdf.index)

    train_data_gdf = pandas.concat([train_presence_gdf, train_absence_gdf])
    test_data_gdf = pandas.concat([test_presence_gdf, test_absence_gdf])

    if out_format == "GPKG":
        if train_vec_lyr is None:
            train_vec_lyr = rsgislib.tools.filetools.get_file_basename(
                train_vec_file, check_valid=True
            )
        train_data_gdf.to_file(train_vec_file, layer=train_vec_lyr, driver=out_format)

        if test_vec_lyr is None:
            test_vec_lyr = rsgislib.tools.filetools.get_file_basename(
                test_vec_file, check_valid=True
            )
        test_data_gdf.to_file(test_vec_file, layer=test_vec_lyr, driver=out_format)
    else:
        train_data_gdf.to_file(train_vec_file, driver=out_format)
        test_data_gdf.to_file(test_vec_file, driver=out_format)


def pop_normalise_coeffs(env_vars: Dict[str, EnvVarInfo], vec_file: str, vec_lyr: str):
    """
    A function which populates the env_vars dictionary of EnvVarInfo objects
    with the normalisation coefficients (mean and standard deviation) calculated
    from the inputted vector data. The inputted vector data is expected to be
    the combined presences and absences data.

    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data. The EnvVarInfo will be populated with
                     the mean and standard deviation.
    :param vec_file: the input vector file path
    :param vec_lyr: the input vector layer name

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    data_gdf.drop(columns=["geometry"], inplace=True)
    mean_data_gdf = data_gdf.mean()
    std_data_gdf = data_gdf.std()
    for var in env_vars:
        if env_vars[var].data_type == rsgislib.VAR_TYPE_CONTINUOUS:
            col_name = env_vars[var].name
            env_vars[var].set_norm_mean_std(
                mean_data_gdf[col_name], std_data_gdf[col_name]
            )
        else:
            env_vars[var].set_norm_mean_std(0, 1)


def apply_normalise_coeffs(
    env_vars: Dict[str, EnvVarInfo],
    vec_file: str,
    vec_lyr: str,
    out_vec_file: str,
    out_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which normalises the continuous variables using the mean and
    standard deviation provided within the env_vars dictionary.

    :param env_vars:
    :param vec_file:
    :param vec_lyr:
    :param out_vec_file:
    :param out_vec_lyr:
    :param out_format:

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    import rsgislib.tools.filetools

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    for var in env_vars:
        if env_vars[var].data_type == rsgislib.VAR_TYPE_CONTINUOUS:
            if (env_vars[var].norm_mean is not None) and (
                env_vars[var].norm_std is not None
            ):
                col_name = env_vars[var].name
                data_gdf[col_name] = (
                    data_gdf[col_name] - env_vars[var].norm_mean
                ) / env_vars[var].norm_std

    if out_format == "GPKG":
        if out_vec_lyr is None:
            out_vec_lyr = rsgislib.tools.filetools.get_file_basename(
                out_vec_file, check_valid=True
            )
        data_gdf.to_file(out_vec_file, layer=out_vec_lyr, driver=out_format)
    else:
        data_gdf.to_file(out_vec_file, driver=out_format)


def comparison_box_plots(
    env_vars: Dict[str, EnvVarInfo],
    vec_file: str,
    vec_lyr: str,
    out_dir: str = "boxplots",
    cls_col: str = "clsid",
    box_plots: bool = False,
):
    """
    Create plots for each of the variables comparing the

    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data.
    :param vec_file: the input vector file path
    :param vec_lyr: the input vector layer name
    :param out_dir: output directory where the plots will be saved
    :param cls_col: the column name of the class variable
    :param box_plots: boolean flag indicating whether to plot boxplots (True)
                      or violin plots (False; Default)

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    import seaborn
    import matplotlib.pyplot as plt

    seaborn.set_theme()

    if not os.path.exists(out_dir):
        os.mkdir(out_dir)

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)
    for var in tqdm.tqdm(env_vars):
        col_name = env_vars[var].name
        if box_plots:
            sns_plot = seaborn.boxplot(data=data_gdf, x=cls_col, y=col_name)
        else:
            sns_plot = seaborn.violinplot(data=data_gdf, x=cls_col, y=col_name)
        sns_plot.figure.savefig(os.path.join(out_dir, f"{col_name}_comp_plot.png"))
        plt.clf()


def correlation_matrix(
    env_vars: Dict[str, EnvVarInfo],
    vec_file: str,
    vec_lyr: str,
    out_corr_file: str = "correlation_matrix.csv",
    out_plt_file: str = "correlation_matrix.png",
    fig_width: int = 15,
    fig_height: int = 14,
):
    """
    Create plots for each of the variables comparing the

    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data.
    :param vec_file: the input vector file path
    :param vec_lyr: the input vector layer name
    :param out_corr_file: output correlation matrix CSV file
    :param out_plt_file: output plot file (if None then ignored)
    :param fig_width: The width of the plot figure
    :param fig_height: The height of the plot figure

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    import seaborn
    import numpy
    import matplotlib.pyplot as plt

    seaborn.set_theme()

    data_gdf = geopandas.read_file(vec_file, layer=vec_lyr)

    analysis_vars = list()
    for var in env_vars:
        analysis_vars.append(env_vars[var].name)

    # Drop any columns not needed from data
    data_cols_drop = list()
    for col in data_gdf.columns:
        if col not in analysis_vars:
            data_cols_drop.append(col)

    if len(data_cols_drop) > 0:
        data_gdf.drop(columns=data_cols_drop, inplace=True)

    corr_df = data_gdf.corr()
    corr_df.to_csv(out_corr_file, index=True)

    if out_plt_file is not None:
        mask = numpy.triu(numpy.ones_like(corr_df, dtype=bool))

        f, ax = plt.subplots(figsize=(fig_width, fig_height))
        # Generate a custom diverging colormap
        cmap = seaborn.diverging_palette(230, 20, as_cmap=True)
        # Draw the heatmap with the mask and correct aspect ratio
        sns_plot = seaborn.heatmap(
            corr_df,
            mask=mask,
            cmap=cmap,
            vmax=0.3,
            center=0,
            square=True,
            linewidths=0.5,
            cbar_kws={"shrink": 0.5},
        )
        sns_plot.figure.savefig(out_plt_file)
        plt.clf()


def search_mdl_params(
    search_obj: BaseSearchCV,
    train_vec_file: str,
    train_vec_lyr: str,
    analysis_vars: List[str],
    cls_col: str = "clsid",
) -> Tuple[BaseEstimator, Dict[str, Any]]:
    """
    A function which will run a scikit-learn search (e.g., GridSearchCV)
    to find optimal parameters for the model estimator.

    :param search_obj: A scikit-learn SearchCV object
    :param train_vec_file: file path to a vector file with the training data.
    :param train_vec_lyr: vector layer name for the training data.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis. The names must be the column names
                          within the vector layer.
    :param cls_col: the name of the column specifying the class within the input
                    vector layer.
    :return: returns the estimator initialised with the best parameters and a
             dictionary of the best parameters.

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    train_data_gdf = geopandas.read_file(train_vec_file, layer=train_vec_lyr)

    train_y = train_data_gdf[cls_col].values
    train_x = train_data_gdf[analysis_vars]

    search_obj.fit(train_x, train_y)
    if not search_obj.refit:
        raise rsgislib.RSGISPyException("Search did no find a fit therefore failed...")

    print(
        "Best score was {} and has parameters {}.".format(
            search_obj.best_score_, search_obj.best_params_
        )
    )

    return search_obj.best_estimator_, search_obj.best_params_


def fit_sgl_mdl(
    est_cls_obj: BaseEstimator,
    train_vec_file: str,
    train_vec_lyr: str,
    test_vec_file: str,
    test_vec_lyr: str,
    analysis_vars: List[str],
    cls_col: str = "clsid",
    roc_curve_plot: str = None,
) -> Tuple[float, float, float]:
    """
    A function which fits a single scikit-learn estimator model returning
    the accuracy statistics and optionally plotting a ROC curve.

    :param est_cls_obj: a scikit-learn estimator model
    :param train_vec_file: file path to a vector file with the training data.
    :param train_vec_lyr: vector layer name for the training data.
    :param test_vec_file: file path to a vector file with the testing data.
    :param test_vec_lyr: vector layer name for the testing data.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis. The names must be the column names
                          within the vector layer.
    :param cls_col: the name of the column specifying the class within the input
                    vector layers.
    :param roc_curve_plot: A file path for the ROC curve plot to be outputted.
    :return: returns the training accuracy, testing accuracy and ROC AUC score.

    """
    import sklearn.metrics
    import matplotlib.pyplot as plt

    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    train_data_gdf = geopandas.read_file(train_vec_file, layer=train_vec_lyr)
    test_data_gdf = geopandas.read_file(test_vec_file, layer=test_vec_lyr)

    train_y = train_data_gdf[cls_col].values
    train_x = train_data_gdf[analysis_vars]

    test_y = test_data_gdf[cls_col].values
    test_x = test_data_gdf[analysis_vars]

    est_cls_obj.fit(train_x, train_y)

    train_acc_val = est_cls_obj.score(train_x, train_y)
    print(f"Classifier Train Score = {round(train_acc_val * 100, 2)}%")

    test_acc_val = est_cls_obj.score(test_x, test_y)
    print(f"Classifier Test Score = {round(test_acc_val * 100, 2)}%")

    test_probs = est_cls_obj.predict_proba(test_x)
    fpr, tpr, thresholds = sklearn.metrics.roc_curve(test_y, test_probs[:, -1])
    roc_auc = sklearn.metrics.auc(fpr, tpr)
    if roc_curve_plot is not None:
        display = sklearn.metrics.RocCurveDisplay(
            fpr=fpr, tpr=tpr, roc_auc=roc_auc, estimator_name="ROC Curve"
        )
        display.plot()
        plt.savefig(roc_curve_plot)
        plt.clf()

    return train_acc_val, test_acc_val, roc_auc


def fit_kfold_mdls(
    est_cls_obj: BaseEstimator,
    train_vec_file: str,
    train_vec_lyr: str,
    test_vec_file: str,
    test_vec_lyr: str,
    analysis_vars: List[str],
    n_kfolds: int = 10,
    fold_prop: float = 0.6,
    cls_col: str = "clsid",
    rnd_seed: int = None,
    sel_replacement: bool = True,
) -> List[BaseEstimator]:
    """
    Fit an ensemble of classifiers using K-fold selection of training data subsets.

    :param est_cls_obj: a scikit-learn estimator model
    :param train_vec_file: file path to a vector file with the training data.
    :param train_vec_lyr: vector layer name for the training data.
    :param test_vec_file: file path to a vector file with the testing data.
    :param test_vec_lyr: vector layer name for the testing data.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis. The names must be the column names
                          within the vector layer.
    :param n_kfolds: The number of models to fit from training data subsets
    :param fold_prop: The proportion of the training data set to use for each
                      model. The proportion should be between 0 and 1. The subset
                      will be randomly selected.
    :param cls_col: the name of the column specifying the class within the input
                    vector layers.
    :param rnd_seed: Optionally provide a random seed for the random number generator.
                     Default: None.
    :param sel_replacement: Optionally replace the training data when creating the
                            subsets. Default: True.
    :return: returns a list of scikit-learn estimator models

    """
    import sklearn.metrics
    import sklearn.base

    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")

    if rnd_seed is not None:
        numpy.random.seed(rnd_seed)

    train_data_gdf = geopandas.read_file(train_vec_file, layer=train_vec_lyr)
    test_data_gdf = geopandas.read_file(test_vec_file, layer=test_vec_lyr)

    test_y = test_data_gdf[cls_col].values
    test_x = test_data_gdf[analysis_vars]

    train_y = train_data_gdf[cls_col].values
    train_x = train_data_gdf[analysis_vars]

    n_fold_size = int(train_y.shape[0] * fold_prop)
    train_idxs = range(train_y.shape[0])

    train_fkolds = list()
    test_fkolds = list()
    roc_fkolds = list()
    fpr_fkolds = list()
    tpr_fkolds = list()
    est_cls_objs = list()
    for i in tqdm.tqdm(range(n_kfolds)):
        train_idx = numpy.random.choice(
            train_idxs, size=n_fold_size, replace=sel_replacement
        )
        train_y_set = train_y[train_idx]
        train_x_set = train_x.iloc[train_idx]

        est_cls_fold = sklearn.base.clone(est_cls_obj, safe=True)

        est_cls_fold.fit(train_x_set, train_y_set)
        est_cls_objs.append(est_cls_fold)

        train_acc_val = est_cls_fold.score(train_x_set, train_y_set)
        train_fkolds.append(train_acc_val)

        test_acc_val = est_cls_fold.score(test_x, test_y)
        test_fkolds.append(test_acc_val)

        test_probs = est_cls_fold.predict_proba(test_x)
        fpr, tpr, thresholds = sklearn.metrics.roc_curve(test_y, test_probs[:, -1])
        fpr_fkolds.append(fpr)
        tpr_fkolds.append(tpr)
        roc_auc = sklearn.metrics.auc(fpr, tpr)
        roc_fkolds.append(roc_auc)

    print(
        f"ROC: {numpy.min(roc_fkolds)} - "
        f"{numpy.median(roc_fkolds)} - "
        f"{numpy.max(roc_fkolds)}"
    )
    print(
        f"Train: {numpy.min(train_fkolds)} - "
        f"{numpy.median(train_fkolds)} - "
        f"{numpy.max(train_fkolds)}"
    )
    print(
        f"Test: {numpy.min(test_fkolds)} - "
        f"{numpy.median(test_fkolds)} - "
        f"{numpy.max(test_fkolds)}"
    )

    return est_cls_objs


def shap_mdl_explainer(
    est_cls_obj: BaseEstimator,
    train_vec_file: str,
    train_vec_lyr: str,
    analysis_vars: List[str],
    shap_summary_plot: str = None,
    shap_heatmap_plot: str = None,
    shap_scatter_plots_dir: str = None,
    shap_depend_plots_dir: str = None,
    subsample_n_smpls: int = None,
    use_tree_explainer: bool = False,
    use_linear_explainer: bool = False,
):
    """
    This functions uses the SHAP model to output feature importance and
    dependence plots for the est_cls_obj model.

    :param est_cls_obj: a scikit-learn estimator model
    :param train_vec_file: file path to a vector file with the training data.
    :param train_vec_lyr: vector layer name for the training data.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis. The names must be the column names
                          within the vector layer.
    :param shap_summary_plot:
    :param shap_heatmap_plot:
    :param shap_scatter_plots_dir:
    :param shap_depend_plots_dir:
    :param subsample_n_smpls:
    :param use_tree_explainer:
    :param use_linear_explainer:

    """
    if not GEOPANDAS_AVAIL:
        raise ImportError("Geopandas is not available")
    import shap
    import matplotlib.pyplot as plt

    if use_tree_explainer and use_linear_explainer:
        raise rsgislib.RSGISPyException(
            "You can only use one explainer. "
            "use_tree_explainer and use_linear_explainer cannot both be True"
        )

    show_plots = False
    if rsgislib.is_notebook():
        shap.initjs()
        show_plots = True

    train_data_gdf = geopandas.read_file(train_vec_file, layer=train_vec_lyr)
    train_x = train_data_gdf[analysis_vars]

    if (subsample_n_smpls is not None) and (subsample_n_smpls > 10):
        train_x_smpl = shap.sample(train_x, subsample_n_smpls)
    else:
        train_x_smpl = train_x

    if use_tree_explainer:
        explainer = shap.TreeExplainer(est_cls_obj, feature_names=analysis_vars)
    elif use_linear_explainer:
        explainer = shap.LinearExplainer(
            est_cls_obj, train_x_smpl, feature_perturbation="correlation_dependent"
        )
    else:
        explainer = shap.KernelExplainer(est_cls_obj.predict_proba, train_x_smpl)

    shap_values = explainer(train_x_smpl)
    if use_tree_explainer:
        shap_values = shap_values[:, :, 1]

    if shap_summary_plot is not None:
        fig = shap.summary_plot(shap_values, train_x_smpl, show=show_plots)
        plt.savefig(shap_summary_plot)
        plt.clf()

    if shap_heatmap_plot is not None:
        shap.plots.heatmap(shap_values, show=show_plots)
        plt.savefig(shap_heatmap_plot)
        plt.clf()

    if shap_scatter_plots_dir is not None:
        if not os.path.exists(shap_scatter_plots_dir):
            os.mkdir(shap_scatter_plots_dir)

        for name in analysis_vars:
            out_plot_file = os.path.join(
                shap_scatter_plots_dir, f"shap_scatter_{name}.png"
            )
            fig = (shap.plots.scatter(shap_values[:, name], show=show_plots),)
            plt.savefig(out_plot_file)
            plt.clf()

    if shap_depend_plots_dir is not None:
        if not os.path.exists(shap_depend_plots_dir):
            os.mkdir(shap_depend_plots_dir)

        sample_ind = 100
        for i, name in enumerate(analysis_vars):
            out_plot_file = os.path.join(
                shap_depend_plots_dir, f"shap_depends_{name}.png"
            )

            fig, ax = shap.partial_dependence_plot(
                name,
                est_cls_obj.predict_proba,
                train_x_smpl,
                model_expected_value=True,
                feature_expected_value=True,
                show=show_plots,
                ice=False,
                shap_values=shap_values[sample_ind : sample_ind + 1, :],
            )

            plt.savefig(out_plot_file)
            plt.clf()


def pred_slg_mdl_prob(
    est_cls_obj: BaseEstimator,
    in_msk_img: str,
    img_msk_val: int,
    env_vars: Dict[str, EnvVarInfo],
    analysis_vars: List[str],
    output_img: str,
    gdalformat: str = "KEA",
    normalise_data: bool = False,
    calc_img_stats: bool = True,
):
    """
    A function which calculates the probability of the presence of the species of
    interest using the trained model. Note, the est_cls_obj must have the
    predict_proba function available to use by this function.

    :param est_cls_obj: a scikit-learn estimator model
    :param in_msk_img: File path to a valid mask image which defines the region
                       of interest.
    :param img_msk_val: The value within the mask image which defines the region
                        of interest
    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data. The EnvVarInfo will provide the image
                     file path, image band and normalisation (mean and
                     standard deviation) values.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis - specifies the order of the variables
                          presented to the classifier.
    :param output_img: the output image path
    :param gdalformat: the output image format
    :param normalise_data: boolean specifying whether to normalise the input data.
                           This should be used if the model was trained with
                           normalised data. The mean and standard deviation of
                           variables used for normalisation should be provided
                           through the env_vars dict of EnvVarInfo objects.
    :param calc_img_stats: boolean specifying whether to calculate the image
                           statistic and pyramids are built for the output image.
                           Default: True

    """
    import rsgislib.imageutils

    in_files = applier.FilenameAssociations()
    in_files.image_mask = in_msk_img

    var_imgs = list()
    var_lut = dict()
    file_lut = dict()
    for i, var_name in enumerate(analysis_vars):
        if env_vars[var_name].file not in var_imgs:
            in_files.__dict__[f"img_{i}"] = env_vars[var_name].file
            var_lut[var_name] = f"img_{i}"
            file_lut[env_vars[var_name].file] = f"img_{i}"
            var_imgs.append(env_vars[var_name].file)
        else:
            var_lut[var_name] = file_lut[env_vars[var_name].file]

    outfiles = applier.FilenameAssociations()
    outfiles.out_image = output_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = est_cls_obj
    otherargs.msk_val = img_msk_val
    otherargs.env_vars = env_vars
    otherargs.analysis_vars = analysis_vars
    otherargs.var_lut = var_lut

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    # RIOS function to apply classifier
    def _calc_sk_class_prob(info, inputs, outputs, otherargs):
        """
        Internal function for rios applier. Used within apply_sklearn_classifier.
        """
        out_img_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.float32)
        out_img_vals[...] = -1

        if numpy.any(inputs.image_mask == otherargs.msk_val):
            out_img_vals = out_img_vals.flatten()
            img_mask_vals = inputs.image_mask.flatten()
            n_vars = len(otherargs.analysis_vars)
            env_pxl_vars = numpy.zeros(
                (out_img_vals.shape[0], n_vars), dtype=numpy.float32
            )
            # Array index which can be used to populate the output
            # array following masking etc.
            pxl_id = numpy.arange(img_mask_vals.shape[0])
            class_vars_idx = 0
            for env_var in otherargs.analysis_vars:
                img_lut_id = otherargs.var_lut[env_var]
                img_arr = inputs.__dict__[img_lut_id]
                env_var_band = otherargs.env_vars[env_var].band
                env_var_idx = env_var_band - 1
                data_arr = img_arr[(env_var_idx)].flatten()
                if (
                    normalise_data
                    and otherargs.env_vars[env_var].data_type
                    == rsgislib.VAR_TYPE_CONTINUOUS
                ):
                    data_arr = (
                        data_arr - otherargs.env_vars[env_var].norm_mean
                    ) / otherargs.env_vars[env_var].norm_std
                env_pxl_vars[..., class_vars_idx] = data_arr
                class_vars_idx = class_vars_idx + 1

            env_pxl_vars = env_pxl_vars[img_mask_vals == otherargs.msk_val]
            pxl_id = pxl_id[img_mask_vals == otherargs.msk_val]

            env_pxl_df = pandas.DataFrame(data=env_pxl_vars, columns=analysis_vars)

            # Perform classification
            pred_class_score = otherargs.classifier.predict_proba(env_pxl_df)
            out_img_vals[pxl_id] = pred_class_score[..., 1]

        out_img_vals = numpy.expand_dims(
            out_img_vals.reshape(
                (inputs.image_mask.shape[1], inputs.image_mask.shape[2])
            ),
            axis=0,
        )
        outputs.out_image = out_img_vals

    # Applying the Classifier
    applier.apply(
        _calc_sk_class_prob, in_files, outfiles, otherargs, controls=aControls
    )

    if calc_img_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=-1, calc_pyramids=True
        )


def pred_slg_mdl_cls(
    est_cls_obj: BaseEstimator,
    in_msk_img: str,
    img_msk_val: int,
    env_vars: Dict[str, EnvVarInfo],
    analysis_vars: List[str],
    output_img: str,
    gdalformat: str = "KEA",
    normalise_data: bool = False,
    calc_img_stats: bool = True,
):
    """
    A function which calculates the binary classification for the presence
    of the species of interest using the trained model. Note, the est_cls_obj
    must have the predict function available to use by this function.

    :param est_cls_obj: a scikit-learn estimator model
    :param in_msk_img: File path to a valid mask image which defines the region
                       of interest.
    :param img_msk_val: The value within the mask image which defines the region
                        of interest
    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data. The EnvVarInfo will provide the image
                     file path, image band and normalisation (mean and
                     standard deviation) values.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis - specifies the order of the variables
                          presented to the classifier.
    :param output_img: the output image path
    :param gdalformat: the output image format
    :param normalise_data: boolean specifying whether to normalise the input data.
                           This should be used if the model was trained with
                           normalised data. The mean and standard deviation of
                           variables used for normalisation should be provided
                           through the env_vars dict of EnvVarInfo objects.
    :param calc_img_stats: boolean specifying whether to calculate the image
                           statistic and pyramids are built for the output image.
                           Default: True

    """
    import rsgislib.imageutils

    in_files = applier.FilenameAssociations()
    in_files.image_mask = in_msk_img

    var_imgs = list()
    var_lut = dict()
    file_lut = dict()
    for i, var_name in enumerate(analysis_vars):
        if env_vars[var_name].file not in var_imgs:
            in_files.__dict__[f"img_{i}"] = env_vars[var_name].file
            var_lut[var_name] = f"img_{i}"
            file_lut[env_vars[var_name].file] = f"img_{i}"
            var_imgs.append(env_vars[var_name].file)
        else:
            var_lut[var_name] = file_lut[env_vars[var_name].file]

    outfiles = applier.FilenameAssociations()
    outfiles.out_image = output_img
    otherargs = applier.OtherInputs()
    otherargs.classifier = est_cls_obj
    otherargs.msk_val = img_msk_val
    otherargs.env_vars = env_vars
    otherargs.analysis_vars = analysis_vars
    otherargs.var_lut = var_lut

    if TQDM_AVAIL:
        progress_bar = rsgislib.TQDMProgressBar()
    else:
        progress_bar = rios.cuiprogress.GDALProgressBar()

    aControls = applier.ApplierControls()
    aControls.progress = progress_bar
    aControls.creationoptions = rsgislib.imageutils.get_rios_img_creation_opts(
        gdalformat
    )
    aControls.drivername = gdalformat
    aControls.omitPyramids = True
    aControls.calcStats = False

    # RIOS function to apply classifier
    def _calc_sk_class(info, inputs, outputs, otherargs):
        """
        Internal function for rios applier. Used within apply_sklearn_classifier.
        """
        out_img_vals = numpy.zeros_like(inputs.image_mask, dtype=numpy.uint8)

        if numpy.any(inputs.image_mask == otherargs.msk_val):
            out_img_vals = out_img_vals.flatten()
            img_mask_vals = inputs.image_mask.flatten()
            n_vars = len(otherargs.analysis_vars)
            env_pxl_vars = numpy.zeros(
                (out_img_vals.shape[0], n_vars), dtype=numpy.float32
            )
            # Array index which can be used to populate the output
            # array following masking etc.
            pxl_id = numpy.arange(img_mask_vals.shape[0])
            class_vars_idx = 0
            for env_var in otherargs.analysis_vars:
                img_lut_id = otherargs.var_lut[env_var]
                img_arr = inputs.__dict__[img_lut_id]
                env_var_band = otherargs.env_vars[env_var].band
                env_var_idx = env_var_band - 1
                data_arr = img_arr[(env_var_idx)].flatten()
                if (
                    normalise_data
                    and otherargs.env_vars[env_var].data_type
                    == rsgislib.VAR_TYPE_CONTINUOUS
                ):
                    data_arr = (
                        data_arr - otherargs.env_vars[env_var].norm_mean
                    ) / otherargs.env_vars[env_var].norm_std
                env_pxl_vars[..., class_vars_idx] = data_arr
                class_vars_idx = class_vars_idx + 1

            env_pxl_vars = env_pxl_vars[img_mask_vals == otherargs.msk_val]
            pxl_id = pxl_id[img_mask_vals == otherargs.msk_val]

            env_pxl_df = pandas.DataFrame(data=env_pxl_vars, columns=analysis_vars)

            # Perform classification
            pred_class_val = otherargs.classifier.predict(env_pxl_df)
            out_img_vals[pxl_id] = pred_class_val

        out_img_vals = numpy.expand_dims(
            out_img_vals.reshape(
                (inputs.image_mask.shape[1], inputs.image_mask.shape[2])
            ),
            axis=0,
        )
        outputs.out_image = out_img_vals

    # Applying the Classifier
    applier.apply(_calc_sk_class, in_files, outfiles, otherargs, controls=aControls)

    if calc_img_stats:
        if gdalformat == "KEA":
            import rsgislib.rastergis

            rsgislib.rastergis.pop_rat_img_stats(
                clumps_img=output_img,
                add_clr_tab=True,
                calc_pyramids=True,
                ignore_zero=True,
            )
        else:
            rsgislib.imageutils.pop_thmt_img_stats(
                output_img, add_clr_tab=True, calc_pyramids=True, ignore_zero=True
            )


def pred_ensemble_mdl_prob(
    est_cls_objs: List[BaseEstimator],
    in_msk_img: str,
    img_msk_val: int,
    env_vars: Dict[str, EnvVarInfo],
    analysis_vars: List[str],
    output_img: str,
    gdalformat: str = "KEA",
    tmp_dir: str = "tmp_dir",
    normalise_data: bool = False,
    calc_img_stats: bool = True,
):
    """
    A function which runs an ensemble of trained classifier models estimating the
    probability of the species presence. The ensemble results are combined by
    calculating the median of the probability from each model.


    :param est_cls_objs: A list of scikit-learn estimator models which have been
                         trained.
    :param in_msk_img: File path to a valid mask image which defines the region
                       of interest.
    :param img_msk_val: The value within the mask image which defines the region
                        of interest
    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data. The EnvVarInfo will provide the image
                     file path, image band and normalisation (mean and
                     standard deviation) values.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis - specifies the order of the variables
                          presented to the classifier.
    :param output_img: the output image path
    :param gdalformat: the output image format
    :param tmp_dir: the temporary directory where the intermediate image files
                    will be outputted.
    :param normalise_data: boolean specifying whether to normalise the input data.
                           This should be used if the model was trained with
                           normalised data. The mean and standard deviation of
                           variables used for normalisation should be provided
                           through the env_vars dict of EnvVarInfo objects.
    :param calc_img_stats: boolean specifying whether to calculate the image
                           statistic and pyramids are built for the output image.
                           Default: True

    """
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.tools.filetools

    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_basename = rsgislib.tools.filetools.get_file_basename(output_img)

    tmp_out_imgs = list()
    for i, est_cls_obj in enumerate(est_cls_objs):
        out_tmp_img = os.path.join(tmp_dir, f"{out_basename}_{i}.tif")
        pred_slg_mdl_prob(
            est_cls_obj=est_cls_obj,
            in_msk_img=in_msk_img,
            img_msk_val=img_msk_val,
            env_vars=env_vars,
            analysis_vars=analysis_vars,
            output_img=out_tmp_img,
            gdalformat=gdalformat,
            normalise_data=normalise_data,
            calc_img_stats=False,
        )
        tmp_out_imgs.append(out_tmp_img)

    rsgislib.imagecalc.calc_multi_img_band_stats(
        input_imgs=tmp_out_imgs,
        output_img=output_img,
        summary_stat=rsgislib.SUMTYPE_MEDIAN,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        no_data_val=-1,
        use_no_data=True,
    )

    if calc_img_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=-1, calc_pyramids=True
        )


def pred_ensemble_mdl_cls(
    est_cls_objs: List[BaseEstimator],
    in_msk_img: str,
    img_msk_val: int,
    env_vars: Dict[str, EnvVarInfo],
    analysis_vars: List[str],
    output_img: str,
    gdalformat: str = "KEA",
    tmp_dir: str = "tmp_dir",
    normalise_data: bool = False,
    calc_img_stats: bool = True,
):
    """
    A function which runs an ensemble of trained classifier models estimating the
    binary classification of the species presence. The ensemble results are combined by
    calculating the number of times a pixel is included in the presences class
    from each model.

    :param est_cls_objs: A list of scikit-learn estimator models which have been
                         trained.
    :param in_msk_img: File path to a valid mask image which defines the region
                       of interest.
    :param img_msk_val: The value within the mask image which defines the region
                        of interest
    :param env_vars: A dictionary of environment variables populated onto both the
                     presence and absence data. The EnvVarInfo will provide the image
                     file path, image band and normalisation (mean and
                     standard deviation) values.
    :param analysis_vars: a list of environmental variables to be used
                          for the analysis - specifies the order of the variables
                          presented to the classifier.
    :param output_img: the output image path
    :param gdalformat: the output image format
    :param tmp_dir: the temporary directory where the intermediate image files
                    will be outputted.
    :param normalise_data: boolean specifying whether to normalise the input data.
                           This should be used if the model was trained with
                           normalised data. The mean and standard deviation of
                           variables used for normalisation should be provided
                           through the env_vars dict of EnvVarInfo objects.
    :param calc_img_stats: boolean specifying whether to calculate the image
                           statistic and pyramids are built for the output image.
                           Default: True

    """
    import rsgislib.imagecalc
    import rsgislib.imageutils
    import rsgislib.tools.filetools

    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)
    out_basename = rsgislib.tools.filetools.get_file_basename(output_img)

    tmp_out_imgs = list()
    for i, est_cls_obj in enumerate(est_cls_objs):
        out_tmp_img = os.path.join(tmp_dir, f"{out_basename}_{i}.tif")
        pred_slg_mdl_cls(
            est_cls_obj=est_cls_obj,
            in_msk_img=in_msk_img,
            img_msk_val=img_msk_val,
            env_vars=env_vars,
            analysis_vars=analysis_vars,
            output_img=out_tmp_img,
            gdalformat=gdalformat,
            normalise_data=normalise_data,
            calc_img_stats=False,
        )
        tmp_out_imgs.append(out_tmp_img)

    tmp_sum_img = os.path.join(tmp_dir, f"{out_basename}_sum.tif")
    rsgislib.imagecalc.calc_multi_img_band_stats(
        input_imgs=tmp_out_imgs,
        output_img=tmp_sum_img,
        summary_stat=rsgislib.SUMTYPE_SUM,
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_8UINT,
        no_data_val=0,
        use_no_data=False,
    )

    band_defns = list()
    band_defns.append(rsgislib.imagecalc.BandDefn("sum_img", tmp_sum_img, 1))
    band_defns.append(rsgislib.imagecalc.BandDefn("msk", in_msk_img, 1))
    rsgislib.imagecalc.band_math(
        output_img=output_img,
        exp=f"(msk=={img_msk_val})?sum_img/{len(tmp_out_imgs)}:0",
        gdalformat=gdalformat,
        datatype=rsgislib.TYPE_32FLOAT,
        band_defs=band_defns,
    )

    if calc_img_stats:
        rsgislib.imageutils.pop_img_stats(
            output_img, use_no_data=True, no_data_val=-1, calc_pyramids=True
        )
