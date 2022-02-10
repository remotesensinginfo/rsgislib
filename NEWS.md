# RSGISLib Release Notes



## v5.0 (2022-02-10):

Version 5.0 of RSGISLib has been under development since June 2021 and has significant changes compared to v4.x. These changes were required for the following reasons:

* RSGISLib had a dependency on the GEOS C++ API which had changed and therefore this meant that RSGISLib would no longer build using the lastest version of the libraries on conda-forge. C++ dependecies have caused problems for us in the past so reducing the number library dependencies was the starting point of the v5 changes.
* Pythonic naming and convensions. RSGISLib did not start as a set of Python modules but has turned into that over the years. However, some of the convensions which were used (e.g., CamelCase verses snake_case) has become inconsistent over time. Also, use of keyword arguments in functions wasn't always supports.
* There were a number of functions and code which were adding over the years which either weren't being maintained or were very specific to a project we did a long time ago and was now no longer relevant. 
* We had some adhoc tests for RSGISLib but the completeness and maintance of those tests was poor.
* The RSGISLib release cycle was slow with us using a development branch as our working version for long periods before releases, in part due to the problems with dependencies and testing.



Therefore, the following changes have been introduced within RSGISLib v5. Please note, these changes are highly likely to break any code which was dependent on RSGISLib v4 or earlier. 

* The number of dependencies has been reduced, with dependencies on GEOS, xerces-c and CGAL all being removed. The C++ dependencies are now: GDAL, Boost, GSL, MuParser, HDF5 and KEALib. 
* All function names and variables are now snake_case and all functions support keyword arguments. Note, this change will cause code to break. For example, the bandMath function is now band_math.
* Module structure has been updated and changed in some cases. For example, vectorutils has now been split into multiple modules.
* Code which was not in use or being maintained or dupicated has been removed or refactored to improve code quality. 
* Added functions and other improvements to aid use and integration with jupyter notebooks.
* Standardisation of variable names for functions has been implemented. A developer guide has also be written with these listed: https://github.com/remotesensinginfo/rsgislib/blob/main/doc/developer_guide.md
* A pytest test suite has been implemented with broad coverage. Coverage can be viewed at https://app.codecov.io/gh/remotesensinginfo/rsgislib
* Continuous integration using circleci has also been implemented so the test suite are run with each commit and pull request. 
* The black code formatter is now used to format the pure Python code. 
* Static code checking for the pure Python code has been setup using sonar cloud.  https://sonarcloud.io/summary/new_code?id=remotesensinginfo_rsgislib
* Documentation / website has been updated to reflect the updates to the v5 code base.
* New tutorials have been produced in the form of jupyter notebooks: https://github.com/remotesensinginfo/rsgislib-tutorials



New modules within this release include:

* rsgislib.changedetect 
* rsgislib.classification.classxgboost
* rsgislib.classification.classimblearn
* rsgislib.classification.classratutils
* rsgislib.classification.clustersklearn
* rsgislib.classification.classaccuracymetrics
* rsgislib.imagecalc.specunmixing
* rsgislib.imagecalc.leastcostpath
* rsgislib.regression
* rsgislib.segmentation.shepherdseg
* rsgislib.segmentation.skimgseg
* rsgislib.timeseries
* rsgislib.tools.filetools
* rsgislib.tools.geometrytools
* rsgislib.tools.plotting
* rsgislib.tools.projection
* rsgislib.tools.sensors
* rsgislib.tools.stats
* rsgislib.tools.tilecacheutils
* rsgislib.tools.utils
* rsgislib.tools.utm
* rsgislib.tools.checkdatasets
* rsgislib.vectorattrs
* rsgislib.vectorgeoms
* rsgislib.vectorutils.createvectors
* rsgislib.vectorutils.createrasters
* rsgislib.zonalstats - *not new but completely re-written and old functions removed*



There are still some areas of the library where updates haven't been fully completed and you may find some changes in upcoming releases:

* rsgislib.imagecalibration
* rsgislib.imageutils.imagecomp
* rsgislib.imageutils.tilingutils
* rsgislib.segmentation.tiledsegsingle 

At release it should also be noted that the ARCSI software we also produce has not yet been updated for RSGISLib v5 but is on-going work.



