#
# This creates an Ubuntu derived base image to build
# RSGISLib and package it as .deb
# 
# NOTE: The built package is designed to work on ubuntu focal with ubuntu-gis stable PPA
# This is for the updated gdal versions available within that PPA.
#
# Ubuntu 20.04 Bionic Beaver
FROM ubuntu:focal

MAINTAINER EnvSys <developers@envsys.co.uk>

ENV ROOTDIR /usr/local/

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Load assets
WORKDIR $ROOTDIR/

# Install basic dependencies
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends software-properties-common && \
    add-apt-repository universe && \
    add-apt-repository -y ppa:ubuntugis/ppa && \
    apt-get update -y && apt-get install --no-install-recommends -y \
    software-properties-common \
    build-essential \
    python3-dev \
    python3-pip \
    python3-wheel \
    libspatialite-dev \
    libpq-dev \
    libgdal-dev \
    libcurl4-gnutls-dev \
    libproj-dev \
    libxml2-dev \
    libnetcdf-dev \
    libpoppler-dev \
    libspatialite-dev \
    libhdf4-alt-dev \
    libhdf5-serial-dev \
    bash-completion \
    gdal-bin \
    libgsl-dev \
    libgsl23 \
    libgmp-dev \
    libboost-filesystem-dev \
    libboost-date-time-dev \
    libboost-thread-dev \
    libmpfr-dev \
    libmuparser-dev \
    libmuparser2v5 \
    libhdf5-dev \
    libhdf5-cpp-103 \
    wget \
    unzip \
    python3-setuptools \
    locales \
    cmake

ARG KEALIB_VER=1.4.14
ARG RIOS_VER=1.4.8
ARG RSGISLIB_VER=5.0.5
ARG PACKAGE_DIR=/usr/local/packages
ARG RSGSILIB_PACKAGE_DIR=$PACKAGE_DIR/rsgislib_$RSGISLIB_VER-1
ARG make_threads=7

# Install kealib
RUN cd /usr/local/src && \
    wget https://github.com/ubarsc/kealib/releases/download/kealib-$KEALIB_VER/kealib-$KEALIB_VER.tar.gz && \
    tar -zxvf kealib-$KEALIB_VER.tar.gz && \
    cd ./kealib-$KEALIB_VER && \
    cmake \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D HDF5_INCLUDE_DIR=/usr/include/hdf5/serial \
    -D HDF5_LIB_PATH=/usr/lib/x86_64-linux-gnu/hdf5/serial \
    -D GDAL_INCLUDE_DIR=/usr/include/gdal \
    -D GDAL_LIB_PATH=/usr/lib/ \
    -D LIBKEA_WITH_GDAL=ON \
    -D BUILD_SHARED_LIBS=ON \
    -D CMAKE_BUILD_TYPE=Release \
    . && \
    make -j "${make_threads}" && \
    make install && \
    echo export GDAL_DRIVER_PATH=/usr/local/lib/gdalplugins >> /etc/profile.d/kealib.sh && \
    echo export LD_LIBRARY_PATH=/usr/local/lib >> /etc/profile.d/kealib.sh

# Install rios
RUN cd /usr/local/src && \
    wget https://github.com/ubarsc/rios/releases/download/rios-$RIOS_VER/rios-$RIOS_VER.tar.gz && \
    tar -zxvf rios-$RIOS_VER.tar.gz && \
    cd ./rios-$RIOS_VER && \
    python3 setup.py install

# Copy in RSGISLib
RUN mkdir /usr/local/src/rsgislib
COPY . /usr/local/src/rsgislib

RUN mkdir -p $RSGSILIB_PACKAGE_DIR

RUN cd /usr/local/src/rsgislib && \
    cmake \
    -D GDAL_INCLUDE_DIR=/usr/include/gdal \
    -D GDAL_LIB_PATH=/usr/lib/gdal \
    -D HDF5_INCLUDE_DIR=/usr/include/hdf5/serial \
    -D HDF5_LIB_PATH=/usr/lib/x86_64-linux-gnu/hdf5/serial \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D CMAKE_INSTALL_PREFIX=/usr \
    . && \
    make -j "${make_threads}" && \
    make install DESTDIR=$RSGSILIB_PACKAGE_DIR && \
    make install && \
    ldconfig && \
    mkdir -p $RSGSILIB_PACKAGE_DIR/usr/lib/python3.8/dist-packages/rsgislib-$RSGISLIB_VER.dist-info && \
    touch $RSGSILIB_PACKAGE_DIR/usr/lib/python3.8/dist-packages/rsgislib-$RSGISLIB_VER.dist-info/METADATA
    
RUN mkdir $RSGSILIB_PACKAGE_DIR/DEBIAN
COPY ./docker/deb_builder/focal_ubuntugis_stable/DEBIAN $RSGSILIB_PACKAGE_DIR/DEBIAN

RUN dpkg-deb --build $RSGSILIB_PACKAGE_DIR


