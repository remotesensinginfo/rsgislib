#!/bin/sh

docker build . \
    --build-arg=CODENAME=$1 \
    --build-arg=REPO=$2 \
    -f contrib/ubuntu/Dockerfile \
    -t rsgislib_$1_$2

for pkg in {"rsgislib_*.deb","python3-rsgislib_*.deb","rsgislib-*.whl","rsgislib-*.tar.gz"}; do
    fname=$(docker run --rm "rsgislib_$1_$2" find /usr/local/packages -maxdepth 1 -name "$pkg")
    outname=$(basename "$fname")
    docker run --rm --entrypoint cat rsgislib_$1_$2 \
        "$fname" > "$3/$1_$2_$outname"
done
