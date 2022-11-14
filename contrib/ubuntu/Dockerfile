ARG CODENAME=jammy

FROM ubuntu:$CODENAME

ARG CODENAME
ARG REPO=none
ARG KEALIB_DEB_URL=https://envsys-public.s3.amazonaws.com/ubuntu/kealib/${CODENAME}_${REPO}_libkea_1.4.14-1.deb
ARG RSGISLIB_VER=5.0.11
ARG PACKAGE_DIR=/usr/local/packages
ARG RSGISLIB_PACKAGE_DIR=$PACKAGE_DIR/rsgislib_$RSGISLIB_VER-1
ARG RSGISLIB_PYTHON_PACKAGE_DIR=$PACKAGE_DIR/python3-rsgislib_$RSGISLIB_VER-1
ARG RSGISLIB_PYTHON_PIP_PACKAGE_DIR=$PACKAGE_DIR/py-rsgislib
ARG make_threads=3

RUN echo $CODENAME

MAINTAINER EnvSys <developers@envsys.co.uk>

ENV ROOTDIR /usr/local/

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Load assets
WORKDIR $ROOTDIR/

COPY ./contrib/ubuntu/${CODENAME}_${REPO}/install_build_deps.sh /install_build_deps.sh

RUN /install_build_deps.sh


RUN pip3 install --upgrade pip && pip3 install build

# Install kealib
RUN cd /usr/local/src && \
    wget $KEALIB_DEB_URL && \
    dpkg -i *libkea*.deb && \
    ldconfig

# Install rsgislib
RUN mkdir /usr/local/src/rsgislib
COPY . /usr/local/src/rsgislib

RUN mkdir -p $RSGISLIB_PACKAGE_DIR

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
    make install DESTDIR=$RSGISLIB_PACKAGE_DIR && \
    make install && \
    ldconfig && \
    mkdir -p $RSGISLIB_PYTHON_PACKAGE_DIR/usr/lib/ && \
    mv $RSGISLIB_PACKAGE_DIR/usr/lib/python3 $RSGISLIB_PYTHON_PACKAGE_DIR/usr/lib/

RUN mkdir $RSGISLIB_PYTHON_PIP_PACKAGE_DIR
COPY ./contrib/pyproject.toml $RSGISLIB_PYTHON_PIP_PACKAGE_DIR/pyproject.toml

RUN cp -r $RSGISLIB_PYTHON_PACKAGE_DIR/usr/lib/python3/dist-packages/rsgislib $RSGISLIB_PYTHON_PIP_PACKAGE_DIR && \
    python3 -m build $RSGISLIB_PYTHON_PIP_PACKAGE_DIR && cp $RSGISLIB_PYTHON_PIP_PACKAGE_DIR/dist/* $PACKAGE_DIR


RUN mkdir -p $RSGISLIB_PYTHON_PACKAGE_DIR/usr/lib/python3.8/dist-packages/rsgislib-$RSGISLIB_VER.dist-info && \
    touch $RSGISLIB_PYTHON_PACKAGE_DIR/usr/lib/python3.8/dist-packages/rsgislib-$RSGISLIB_VER.dist-info/METADATA && \
    mkdir $RSGISLIB_PACKAGE_DIR/DEBIAN $RSGISLIB_PYTHON_PACKAGE_DIR/DEBIAN

COPY ./contrib/ubuntu/${CODENAME}_${REPO}/DEBIAN $RSGISLIB_PACKAGE_DIR/DEBIAN
COPY ./contrib/ubuntu/DEBIAN_py $RSGISLIB_PYTHON_PACKAGE_DIR/DEBIAN

RUN dpkg-deb --build $RSGISLIB_PACKAGE_DIR
RUN dpkg-deb --build $RSGISLIB_PYTHON_PACKAGE_DIR
RUN ls $PACKAGE_DIR
