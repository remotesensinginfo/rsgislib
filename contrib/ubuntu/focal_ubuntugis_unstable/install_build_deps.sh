#!/bin/sh

set -e

apt-get update -y
apt-get install -y --no-install-recommends software-properties-common
add-apt-repository universe
add-apt-repository -y ppa:ubuntugis/ubuntugis-unstable

apt-get update -y
apt-get install --no-install-recommends -y \
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
    cmake \
    python3-pip \
    python3-venv

pip3 install --upgrade pip
pip3 install build
