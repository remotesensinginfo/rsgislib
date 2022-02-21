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
    build-essential \
    python3-dev \
    python3-setuptools \
    libgdal-dev \
    libhdf5-serial-dev \
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
    wget \
    locales \
    cmake

ARG KEALIB_DEB_URL=https://envsys-public.s3.amazonaws.com/ubuntu/kealib/focal_ubuntugis_stable_libkea_1.4.14-1.deb
ARG RSGISLIB_VER=5.0.5
ARG PACKAGE_DIR=/usr/local/packages
ARG RSGSILIB_PACKAGE_DIR=$PACKAGE_DIR/rsgislib_$RSGISLIB_VER-1
ARG make_threads=7

# Install kealib
RUN cd /usr/local/src && \
    wget $KEALIB_DEB_URL && \
    dpkg -i *libkea*.deb && \
    ldconfig

# Install rsgislib
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
COPY ./contrib/ubuntu/focal_ubuntugis_stable/DEBIAN $RSGSILIB_PACKAGE_DIR/DEBIAN
RUN dpkg-deb --build $RSGSILIB_PACKAGE_DIR


