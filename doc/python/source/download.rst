Download
=========

Docker and Singularity
------------------------

The simplest way to download and use RSGISLib and associated packages is via Docker (https://www.docker.com). We provide stable and development Docker images:

* Stable: petebunting/au-eoed 
* Development: petebunting/au-eoed-dev

.. code-block:: bash

    docker pull petebunting/au-eoed 
    # Note. for commands below all data and script need to be available in your 
    # local directory and to be referenced from the /data path.
    # Run your rsgislib python script
    docker run -i -t -v ${PWD}:/data petebunting/au-eoed python /data/my_rsgislib_script.py
    # Run an RSGISLib command line tool
    docker run -i -t -v ${PWD}:/data petebunting/au-eoed rsgiscalcimgstats.py -i img.kea -n 0
    # Gain access to the docker image terminal
    docker run -i -t -v ${PWD}:/data petebunting/au-eoed /bin/bash

For HPC users Docker cannot be used as it requires to execute as root user. However, Singularity (https://sylabs.io) allows Docker images to be imported and executed as a local user. 

.. code-block:: bash

    singularity pull docker://petebunting/au-eoed
    # Note. Singularity has the advantage that is can use local paths and does not 
    # need to mount the file system to access file.
    # Run your rsgislib python script
    singularity exec au-eoed.simg python my_rsgislib_script.py
    # Run an RSGISLib command line tool
    singularity exec au-eoed.simg rsgiscalcimgstats.py -i img.kea -n 0

Binary Downloads
-----------------

We make binary downloads available for Windows, Linux and MacOD, built using Python 3, through conda. You can get conda through the Anaconda or Miniconda Python distribution (https://docs.conda.io/en/latest/miniconda.html). More details are available from http://conda.pydata.org

The recomended way to install RSGISlib locally is from conda-forge using the following commands on MacOS and Linux:

.. code-block:: bash

    conda create -n osgeo-env-v1 python=3.7
    source activate osgeo-env-v1
    conda install -c conda-forge rsgislib
    
The following video shows the steps to undertake this installation.

.. youtube:: 9HqKLioyAeM


Source
-------

The RSGISLib source is available to download from  https://bitbucket.org/petebunting/rsgislib . Releases are available from the downloads tab, for the latest version of RSGISLib you can check out the development version with mercurial using:

.. code-block:: bash

    hg clone https://bitbucket.org/petebunting/rsgislib rsgislib-code
    

We have also created spack (https://spack.readthedocs.io) build instructions, which are updated on an adhoc basis. See https://github.com/spack/spack for spack source.


For help with compiling or downloading RSGISLib you can email our mailing list:

rsgislib-support@googlegroups.com

The archives can be accessed at: 
https://groups.google.com/forum/#!forum/rsgislib-support