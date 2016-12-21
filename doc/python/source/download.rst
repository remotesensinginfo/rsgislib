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

The recomended way to install is though the conda environment we provide. On Linux run:

.. code-block:: bash

    conda env create au-eoed/au-osgeo-lnx
    source activate au-osgeo-lnx

And for OS X:

.. code-block:: bash

    conda env create au-eoed/au-osgeo-osx
    source activate au-osgeo-osx


If you just want RSGISLib without the extras in the pre-packaged environments above then you can also install using the following commands:

.. code-block:: bash

    conda install -c conda-forge -c rios rsgislib
    
It is recommended that you also install the following packages:

.. code-block:: bash

    conda install -c conda-forge -c rios scikit-learn rios tuiview h5py



