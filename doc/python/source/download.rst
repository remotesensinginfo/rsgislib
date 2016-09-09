Download
===============

Source
--------------------

The RSGISLib source is available to download from  https://bitbucket.org/petebunting/rsgislib . Releases are available from the downloads tab, for the latest version of RSGISLib you can check out the development version with mercurial using:


.. code-block:: bash

    hg clone https://bitbucket.org/petebunting/rsgislib rsgislib-code


Instructions on installing RSGISLib and the pre-requisites are in the documentation, available from: https://bitbucket.org/petebunting/rsgislib-documentation/

For help with installation you can email our mailing list:

rsgislib-support@googlegroups.com

The archives can be accessed at: 
https://groups.google.com/forum/#!forum/rsgislib-support


Binary Downloads
---------------------

We make binary downloads available for Linux and OS X, built using Python 3, through conda. You can get conda through the Anaconda or Miniconda Python distribution. More details are available from http://conda.pydata.org

To install RSGISLib use:

.. code-block:: bash

    conda install -c osgeo rsgislib

You then need to set the following environmental variables:

.. code-block:: bash

    export GDAL_DRIVER_PATH=~/anaconda3/lib/gdalplugins:$GDAL_DRIVER_PATH
    export GDAL_DATA=~/anaconda3/share/gdal

Changing if you have not installed anaconda to the standard location (or used miniconda which installs to ‘miniconda3′).


