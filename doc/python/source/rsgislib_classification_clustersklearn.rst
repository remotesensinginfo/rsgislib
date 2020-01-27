RSGISLib Scikit-Learn Unsupervised Pixel Classification Module
===============================================================

.. automodule:: rsgislib.classification.clustersklearn


To use the unsupervised classification functions you need to create an instance of a scikit-learn cluster (https://scikit-learn.org/stable/modules/clustering.html). For example K-Means::

    from sklearn.cluster import MiniBatchKMeans
    sklclusterer = MiniBatchKMeans(n_clusters=60, init='k-means++', max_iter=100, batch_size=100)


You can then run one of the module functions, first we'll import the module and define our images::

    input_img = "S2_UVD_27sept_27700_sub.kea"
    output_img = "S2_UVD_27sept_27700_sub_clusters.kea"

Using all the image pixels, this can be time and memory intensive so this would normally be used for smaller datasets::

    rsgislib.classification.clustersklearn.img_pixel_cluster(input_img, output_img, gdalformat='KEA', noDataVal=0, clusterer=sklclusterer)

If you have a larger image then you might want to use one of the other two functions, which either performs the clustering in tiles and therefore has tile boundaries, alternatively, you can sample the input image performing the clustering on the sample and then applying to the whole image.

Clustering using tiling::

    rsgislib.classification.clustersklearn.img_pixel_tiled_cluster(input_img, output_img, gdalformat='KEA', noDataVal=0, clusterer=sklclusterer)

Clustering using samples, the imgSamp parameter specifies the size of the sample taken, in the example below it will be every 100th pixel (i.e., a 1 percent sample)::

    rsgislib.classification.clustersklearn.img_pixel_sample_cluster(input_img, output_img, gdalformat='KEA', noDataVal=0, imgSamp=100, clusterer=sklclusterer)


Function Specifications
------------------------

.. autofunction:: rsgislib.classification.clustersklearn.img_pixel_sample_cluster
.. autofunction:: rsgislib.classification.clustersklearn.img_pixel_tiled_cluster
.. autofunction:: rsgislib.classification.clustersklearn.img_pixel_cluster




