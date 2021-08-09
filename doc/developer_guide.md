# Developer Guide

## Purpose
This is a short set of docs which detail the coding standards for contributions to the RSGISLib library.

## History and Future
RSGISLib started as a C++ only library with command line tools and has evolved over time, 
first providing an XML based interface when the number of command line tools got too large 
to easily maintain. That XML interface has since evolved into a Python interface and more
recently has led to an increasing amount of pure python based functions being added to the 
library of modules. 

Looking into the future we expect the number of pure python functions to increase and in 
some case even replace the C++ implementations. This is for a number of reasons, firstly
that development is quicker and easier with python and the number of people who can contribute
to maintenance of the code base is higher. It is also easier in terms of dependency maintenance
and compilation.

## Overall Code Conventions

The following details code styles and things should be done before a pull request/edits 
are accepted into the library:

1. Classes and module names should start with the `RSGIS` prefix (upper case for Classes 
   and lower case for modules)
2. Function names within the python interface should be in CamelCase (first letter should be 
   lower case) while variables should be in snake_case (all lower case) - see below for common 
   name examples).
3. Although not always (most often not) present in the historical code unit and overall 
   function tests should be provided where ever possible. See info below on creation of 
   tests within the existing system in Python and C++ below.
4. Documentation should be provided for the Python functions and added to the sphinx configuration files.
5. Documentation should provide a brief example of the function being called and a more complete tutorial 
   style example provided within the tutorials section so someone can see the function(s) being used in a
   *real world* example


## Python Coding Style

The main coding style things to be aware of when providing a function for RSGISLib are:

1. Variables should be in snake_case and all characters should be in lower case
2. Class and function names should be in CamelCase
3. Module names should be lower case with no underscores between words.
4. Functions should have keyword specified through the C++ interface
5. Pure python functions should provide type hints.

### Python Code Style Tools
Before code is committed to the RSGISLib git repo the [black](https://black.readthedocs.io) 
code formatter should be run to ensure that the code formatting is standard with the rest of the 
modules. The [flake8](https://flake8.pycqa.org/) and [pylint](https://www.pylint.org) static code 
checkers should also be run and relevant issues addressed. 

To run those tools you can use the following commands:

```shell
black path/to/python/script.py
```

```shell
flake8 path/to/python/script.py
```

```shell
pylint --function-naming-style=camelCase --max-line-length=88 path/to/python/script.py
```

Note. for flake8 you will want to create a config file `~/.config/flake8` specifying the
line length of 88 characters to match the black formatting:

```
[flake8]
max-line-length = 88
```

If you don't have these tools installed then they can be installed with the following commands:

```shell
pip install black
pip install flake8
pip install pylint
```

### Python Module Imports
For imports, they should be at the top of the file for widely used modules (e.g., GDAL, 
Numpy, Scipy) but for imports which are only specifically used in just a one or a few  
functions then the important should be within the function. When importing modules should
be grouped in the following order:

* Standard library imports.
* Related third party imports.
* Local application/library specific imports.

Note, you need some care when importing RSGISLib modules within RSGISLib so module level
circular importing is not introduced. Therefore, we would recommend avoiding importing
other RSGISLib modules at the module level within RSGISLib.

### Python Function Variable Names 

For consistency, we try to keep the following commonly used input variable names the same across different
functions:

#### Images

| Variable Name | Description |
| ----------- | ----------- |
| input_img | An input image when there is only a single input image within the function. |
| output_img | An output image when there is only a single output image within the function. |
| input_imgs | A list of input images |
| in_XXX_img | An input image where there are multiple input images with XXX used to indicate the input image type (e.g., dem) |
| clumps_img | An input clumps image (i.e., with a raster attribute table) such as used in there rastergis module|
| gdalformat | The output image file format using the GDAL format shorthands (e.g., GTIFF, KEA, ENVI, HFA) - note for legacy reasons this is not snake_case|
| datatype | The output image data type (e.g., rsgis.TYPE_16INT) of the image - note for legacy reasons this is not snake_case|
| out_img_base | The base file path and name for a set of output images (e.g., tiles) |
| out_img_ext | The file extension for a output image (i.e., when using out_img_base) |
| use_no_data | boolean variable to specify whether a specified no data value should be used |
| no_data_val | numeric variable specifying a no data value |
| out_no_data | numeric variable specifying an output no data value if different from the input no data value (i.e., no_data_val) |
| img_band | the index (start at 1; GDAL convention) of the input image band
| img_band_idx | the index (start at 0; array convention) of the input image band
| n_bands | the number of bands (i.e., output bands)
| calc_stats | boolean to specify whether image metadata statistics and pyramids should be created. 

#### Vectors

| Variable Name | Description |
| ----------- | ----------- |
| vec_file | An input vector file when there is only a single input vector file within the function. |
| vec_lyr | An input vector layer when there is only a single input vector layer within the function. |
| vec_XXX_file | An input vector file when there are multiple input vector files within the function and XXX is used to indicate the purpose of the input file. |
| vec_XXX_lyr | An input vector layer when there are multiple input vector layers within the function and XXX is used to indicate the purpose of the input layer. |
| out_vec_file | An output vector file when there is only a single output vector file within the function. |
| out_vec_lyr | An output vector layer when there is only a single output vector layer within the function. |
| out_format | Provide the output OGR format (e.g., GPKG, GeoJSON) for the output vector file. |
| del_exist_vec | In some cases GDAL/OGR cannot simply overwrite an existing vector layer, this variable is provided by many RSGISLib function to allow the user to specify that is the output vector layer exists it should be deleted before the main processing starts| 
| out_epsg | If there is an option to specify the output projection then generally use an EPSG code |
| vec_lyr_obj | An OGR input vector layer object |

#### Others
| Variable Name | Description |
| ----------- | ----------- |
| in_file | Generic input file (e.g., text file) |
| out_file | Generic output file (e.g., text file) |
| in_h5_file | An input HDF5 file |
| out_h5_file | An output HDF5 file. |
| h5_XXX_file | Where there are multiple input and/or output HDF5 files then XXX is used to specify the purpose. |
| min_x, max_x | Min and Max x values |
| min_y, max_y | Min and Max y values |
| wkt_file | Path to a text file containing a WKT string for a projection |
| wkt_str | A WKT representation of a projection |
| rnd_seed | Seed for random number generator |
| win_size | specify a window size, normally for filtering |
| win_h_size | specify a half window size used in some places to ensure window size is odd |
| x_size | a size in the X axis |
| y_size | a size in the Y axis |


## Python Tests Implementation
Python testing is undertaken using the [pytest](https://docs.pytest.org) tool and are within the 
python_tests directory. You can install pytest using the following command:

```shell
pip install pytest
```

The tests can be run from within the rsgislib top level directory using the following command:

```shell
pytest -v  python_tests
```

## C++ Coding Style

1. When creating iterators use the `auto` keyword for cleaner code.
2. Curly brackets on new lines 
3. Use curly brackets even for single line if-statements.



## C++ Test Implementation


## Command Line Utilities Conventions





