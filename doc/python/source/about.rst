About
===============

The Remote Sensing and Geographical Information Systems software Library (RSGISLib) was started by `Pete Bunting <http://users.aber.ac.uk/pfb/>`_, a lecturer in the Department of Geography and Earth Sciences (DGES) at Aberystwyth University (Wales, UK) in April 2008 as a means of grouping together code developed as part of his research and to provide a better platform for development of new functions.

`Dan Clewley <http://mixil.usc.edu/people/staff/daniel-clewley.htm>`_ joined the project in December 2008, shortly after starting his PhD within DGES, at Aberystwyth University (funded by a NERC studentship).

Features were added to RSGISLib, as required by Pete and Dan or requested by other researches and students within Aberystwyth University and collaborators in other institutions. 

The project was released under the GPL license in November 2009 in the hope it would be useful to others working in remote sensing and GIS and has been actively developed since then. Currently there are over 300 commands to perform a number of tasks.

Frequently Asked Questions
--------------------------

**What does RSGISLib do?**

RSGISLib contains a number of algorithms for processing and analysing remote sensing data that are the product of research carried out by the authors and their collaborators. The primary aim of RSGISLib was to make it easier to develop code for our research. Over time, more functionality has been added to make it easier to carry out our research. The project was released with the hope it will be of use to others working with Remote Sensing and GIS data. Some of the functionality available within RSGISlib includes:

* Image segmentation
* Object-based classification
* Image-to-image registration
* Image filtering
* Zonal statistics
* General raster and vector processing

To date RSGISLib been used as part of a number of publications and theses, for more details see:

Peter Bunting, Daniel Clewley, Richard M. Lucas and Sam Gillingham. 2014. The Remote Sensing and GIS Software Library (RSGISLib), Computers & Geosciences. Volume 62, Pages 216-226 http://dx.doi.org/10.1016/j.cageo.2013.08.007.

For storing and processing laser scanning data the Sorted Pulse Data Library (SPDLib) is available to download from http://www.spdlib.org/

**How do I cite RSGISLib?**

If you have used RSGISLib for published work then you should cite the following publication, and / or the specific paper of the algorithm you used (if applicable). 

Peter Bunting, Daniel Clewley, Richard M. Lucas and Sam Gillingham. 2014. The Remote Sensing and GIS Software Library (RSGISLib), Computers & Geosciences. Volume 62, Pages 216-226 http://dx.doi.org/10.1016/j.cageo.2013.08.007.

Bibtex::

    @article{Bunting_etal_2014,
    	Author = {Bunting, Peter and Clewley, Daniel and Lucas, Richard M and Gillingham, Sam},
    	Title = {{The Remote Sensing and GIS Software Library (RSGISLib)}},
    	Journal = {Computers and Geosciences},
    	Pages = {216--226},
    	Volume = {62},
    	Year = {2014}}
    
**Is RSGISLib intended to be a substitute for commercial GIS / RS packages or other open source packages?**

No, RSGISLib is not being developed as a substitute for ArcMap / QGIS or ENVI. As stated above, RSGISLib is mainly developed to support our research, because of this there is a lot of advanced functionality in RSGISLib that is unavailable in other packages. Although RSGISLib works well on the desktop, it scales well to a High Performance Computing (HPC) environment, which is an area many of the previously mentioned software aren't focusing on.

For visualising data it is recommended RSGISLib is used in combination with TuiView, an open source cross-platform viewer available to download from `here <https://bitbucket.org/chchrsc/tuiview>`_.

**Is this free software?**

Yes, this software is provided freely under a GPL3 license. The software is free to use and the source code is available to view and modify.

**Where can I get help with RSGISLib?**

The RSGISLib documentation, which contains tutorials for selected functions is available to download from `here <https://bitbucket.org/petebunting/rsgislib-documentation/>`_ , documentation for the Python bindings is available on this website.

Help is available for RSGISLib through emailing our mailing list: rsgislib-support@googlegroups.com

The archive is available to view at `groups.google.com <https://groups.google.com/forum/#!forum/rsgislib-support>`_ 

**Do you offer training on RSGISLib?**

RSGISLib is taught as part of the Masters course at Aberystwyth University. The notes for the Python course, which include a chapter using RSGISLib for object based classification is available `Here <https://bitbucket.org/petebunting/python-tutorial-for-spatial-data-processing>`_. We have recently given a training course at the 20th JAXA Kyoto & Carbon initiative meeting (notes available shortly). If you are interested in training sessions, please contact us.

**How do I go about getting functionality added to RSGISLib?**

For general suggestions submit a ticket on our `issues <https://bitbucket.org/petebunting/rsgislib/issues?status=new&status=open>`_ page on Bitbucket or post a suggestion to the Google groups mailing list. If there's functionality you need adding for a specific project you could try to add it yourself or you can contact us to enquire about consultancy work. 

**Who funds RSGISLib?**

There has been no direct funding to develop RSGISLib or obligations to release it. However, there has been funding which has supported us and allowed us work on it and we feel strongly about the role of open source software in reproducible research.

Pete Bunting is employed by Aberystwyth University and has been since RSGISLib was started, with the exception of a year spent at Landcare Research (New Zealand), where a number of features including the segmentation and Raster GIS were added. Landcare Research continue to provide support for Pete. 

Dan Clewley's PhD (2008 - 2012) was funded by a NERC studentship. He is currently employed in `Mahta Moghaddam's <http://mixil.usc.edu/people/director/>`_ Microwave Systems Sensors and Imaging Lab (MiXIL) at the University of Southern California, primarily funded through a NASA-ESDR award.

