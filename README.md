# RSGISLib #

[![<remotesensinginfo>](https://circleci.com/gh/remotesensinginfo/rsgislib/tree/main.svg?style=shield)](https://app.circleci.com/pipelines/github/remotesensinginfo/rsgislib)
[![codecov](https://codecov.io/gh/remotesensinginfo/rsgislib/branch/main/graph/badge.svg?token=EHTF2G1KCL)](https://codecov.io/gh/remotesensinginfo/rsgislib)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=remotesensinginfo_rsgislib&metric=bugs)](https://sonarcloud.io/summary/new_code?id=remotesensinginfo_rsgislib)
[![Install](https://anaconda.org/conda-forge/rsgislib/badges/version.svg)](https://anaconda.org/conda-forge/rsgislib)
[![Downloads](https://anaconda.org/conda-forge/rsgislib/badges/downloads.svg)](https://anaconda.org/conda-forge/rsgislib)
[![Platforms](https://anaconda.org/conda-forge/rsgislib/badges/platforms.svg)](https://anaconda.org/conda-forge/rsgislib)
  
The Remote Sensing and GIS software library (RSGISLib) is a collection of tools for processing remote sensing and GIS datasets.
For more details see the project website: http://rsgislib.org/

## Support the Project ##

We provide updated and support for the project for free as a open source project but if you'd like to support us you can buy a coffee - thank you! :)
[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/petebunting)
  
## Installing ##

Instructions on installing RSGISLib:
  
```shell
conda install -c conda-forge rsgislib
```
  
If you want to install a system of packages for undertaking remote sensing and GIS data analysis in python then the following packages would be a recommended list:
  
```shell
conda install -c conda-forge rsgislib gdal h5py parallel scikit-learn scikit-image scikit-optimize imbalanced-learn matplotlib pandas geopandas statsmodels scipy rasterio shapely networkx sqlalchemy pycurl xgboost lightgbm tpot seaborn numba pip sphinx elevation rtree tqdm jinja2 keras keras-preprocessing pytables bokeh pygal jupyterlab psutil pysal libpysal esda pyyaml netcdf4 xarray rasterstats fiona plotly python-kaleido psycopg2 ipywidgets tuiview
```

## Documentation and Support ##

The documentation for RSGISLib is available from the project website: http://rsgislib.org/

Support for RSGISLib is provided through a mailing list: https://groups.google.com/forum/#!forum/rsgislib-support

You need to be a member of the group to post.
Please check through existing posts to see if there is already an answer to your question before emailing.
To help us answer your question provide as much information as possible (RSGISLib version and how you installed it, OS, things you have already tried to solve the problem etc.,).

We occasionally post tutorials on RSGISLib and other useful bits and pieces to our [website remotesensing.info](https://remotesensing.info).

## Citing ##

If you use RSGISLib as part of published work please cite the following publication:

Peter Bunting, Daniel Clewley, Richard M. Lucas and Sam Gillingham. 2014. The Remote Sensing and GIS Software Library (RSGISLib), Computers & Geosciences. Volume 62, Pages 216-226 http://dx.doi.org/10.1016/j.cageo.2013.08.007.
[pre-print](http://rsgislib.org/publications/pbunting_etal_RSGISLib.pdf)

```
    @article{Bunting_etal_2014,
    	Author = {Bunting, Peter and Clewley, Daniel and Lucas, Richard M and Gillingham, Sam},
    	Title = {{The Remote Sensing and GIS Software Library (RSGISLib)}},
    	Journal = {Computers and Geosciences},
    	Pages = {216--226},
    	Volume = {62},
    	Year = {2014}}
```

## Building from Source ##
  
RSGISLib uses cmake for the build system, so you will need that and the dependencies installed on your system.

```shell
# Create a build directory in the release directory
mkdir build_dir
cd build_dir

# if all the dependencies are within the same path (e.g., installed with conda)
# then use a variable to specify the path.
export DEPEND_ENV_PATH=/home/user/miniconda/envs/rsgislib_v5_dev

# Set the install location and path to dependencies
cmake -D CMAKE_INSTALL_PREFIX=/usr/local \
-D BOOST_INCLUDE_DIR=$DEPEND_ENV_PATH/include \
-D BOOST_LIB_PATH=$DEPEND_ENV_PATH/lib \
-D GDAL_INCLUDE_DIR=$DEPEND_ENV_PATH/include \
-D GDAL_LIB_PATH=$DEPEND_ENV_PATH/lib \
-D HDF5_INCLUDE_DIR=$DEPEND_ENV_PATH/include \
-D HDF5_LIB_PATH=$DEPEND_ENV_PATH/lib \
-D GSL_INCLUDE_DIR=$DEPEND_ENV_PATH/include \
-D GSL_LIB_PATH=$DEPEND_ENV_PATH/lib \
-D MUPARSER_INCLUDE_DIR=$DEPEND_ENV_PATH/include \
-D MUPARSER_LIB_PATH=$DEPEND_ENV_PATH/lib \
-D KEA_INCLUDE_DIR=$DEPEND_ENV_PATH/include \
-D KEA_LIB_PATH=$DEPEND_ENV_PATH/lib \
-D Python_EXECUTABLE=$DEPEND_ENV_PATH/bin/python \
-D CMAKE_VERBOSE_MAKEFILE=ON \
-D CMAKE_BUILD_TYPE=Debug \
-D CMAKE_SKIP_RPATH=ON \
..
  
# build rsgislib -j N is the number of cores to use for build 
make -j 2
  
# Install the build into the install prefix.
make install
```
  

  
[![SonarCloud](https://sonarcloud.io/images/project_badges/sonarcloud-white.svg)](https://sonarcloud.io/summary/new_code?id=remotesensinginfo_rsgislib)
  
