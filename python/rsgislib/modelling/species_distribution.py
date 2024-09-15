#!/usr/bin/env python
"""
Functions implementing species distribution modelling.
"""
from typing import Dict
import os

import rsgislib
import numpy
from osgeo import gdal, ogr


class EnvVarInfo(object):
    """
    This is a class to store the defining the parameters for each environment variable.

    :param name: Name of the variable.
    :param file: Image file path for the variable.
    :param band: Band in the image representing the variable.
    :param type: Variable type (Either rsgislib.VAR_TYPE_CONTINUOUS or
                 rsgislib.VAR_TYPE_CATEGORICAL).
    :param min_vld_val: The minimum valid value for the variable.
    :param max_vld_val: The maximum valid value for the variable.

    """

    def __init__(
        self,
        name: str = None,
        file: str = None,
        band: int = 1,
        type: int = rsgislib.VAR_TYPE_UNDEFINED,
        min_vld_val: float = None,
        max_vld_val: float = None,
    ):
        """

        :param name: Name of the variable.
        :param file: Image file path for the variable.
        :param band: Band in the image representing the variable.
        :param type: Variable type (Either rsgislib.VAR_TYPE_CONTINUOUS or
                     rsgislib.VAR_TYPE_CATEGORICAL).
        :param min_vld_val: The minimum valid value for the variable.
        :param max_vld_val: The maximum valid value for the variable.

        """
        self.name = name
        self.file = file
        self.band = band
        self.type = type
        self.min_vld_val = min_vld_val
        self.max_vld_val = max_vld_val

    def __str__(self):
        type_str = "Unknown"
        if type == rsgislib.VAR_TYPE_CONTINUOUS:
            type_str = "Continuous"
        elif type == rsgislib.VAR_TYPE_CATEGORICAL:
            type_str = "Categorical"
        return f"{self.name}: {self.file} (Band: {self.band}) (Type: {type_str}) (Range: {self.min_vld_val}, {self.max_vld_val})"

    def __repr__(self):
        type_str = "Unknown"
        if type == rsgislib.VAR_TYPE_CONTINUOUS:
            type_str = "Continuous"
        elif type == rsgislib.VAR_TYPE_CATEGORICAL:
            type_str = "Categorical"
        return f"{self.name}: {self.file} (Band: {self.band}) (Type: {type_str}) (Range: {self.min_vld_val}, {self.max_vld_val})"


def gen_pseudo_absences_smpls(
    in_msk_img: str,
    img_msk_val: int,
    out_vec_file: str,
    out_vec_lyr: str,
    n_smpls: int = 10000,
    xtra_n_smpls: int = 30000,
    present_smpls_vec_file: str = None,
    present_smpls_vec_lyr: str = None,
    present_smpls_dist_thres: float = 1000,
    out_format: str = "GeoJSON",
    rnd_seed: int = None,
):
    import geopandas
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

    if present_smpls_vec_file is not None:
        # calc distance to present_smpls_vec_lyr
        rsgislib.vectorattrs.calc_near_dist_to_feats(
            vec_file=out_vec_file,
            vec_lyr=out_vec_lyr,
            vec_dist_file=present_smpls_vec_file,
            vec_dist_lyr=present_smpls_vec_lyr,
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
        dist_sel_arr = numpy.where(dist_col_arr > present_smpls_dist_thres, 1, 0)

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
