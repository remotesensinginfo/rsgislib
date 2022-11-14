# To build .deb packages

There is a single Dockerfile that will build:

* rsgislib - A debian package without the python files
* python3-rsgislib - A debian package containing the python files for rsgislib
* python pip packages - A wheel and source package that can be installed with pip

in /usr/local/packages from where you can copy it out.  It will build packages
against some combination of ubuntu releases and ubuntugis (or not) PPAs depending
on the build args used.  At the moment, you can run the following from the root of
the repo:

```
docker build . \
    --build-arg=CODENAME=focal \
    --build-arg=REPO=ubuntugis_stable \
    -f contrib/ubuntu/Dockerfile \
    -t rsgislib_focal_stable

docker build . \
    --build-arg=CODENAME=focal \
    --build-arg=REPO=ubuntugis_unstable \
    -f contrib/ubuntu/Dockerfile \
    -t rsgislib_focal_unstable

docker build . \
    --build-arg=CODENAME=jammy \
    --build-arg=REPO=none \
    -f contrib/ubuntu/Dockerfile \
    -t rsgislib_jammy
```

More may be added in the future, but you should be able to build against other
versions of ubuntu or ubuntugis by inspecting the contents of `contrib/ubuntu`
and adapting to your needs.

Copy out the packages so built:

```
docker run --rm --entrypoint cat rsgislib_focal_stable \
    /usr/local/packages/<the_package_name> > /tmp/<the_package_name>
```

## Notes:

The process relies on fetching and installing a companion .deb of kealib to build against. This is hosted by Environment Systems Ltd in a public S3 Bucket. This part of the build could be replaced by building from source locally if required.

The built .deb package requires RIOS to run properly. This can be installed into your python environment in the normal way, no extra packaging is supplied.

Copies of packages generated as above are hosted by Environment Systems Ltd (no warranty express or implied) at:

 - https://envsys-public.s3.amazonaws.com/ubuntu/rsgislib/focal_ubuntugis_stable_rsgislib_5.0.11-1.deb
 - https://envsys-public.s3.amazonaws.com/ubuntu/rsgislib/focal_ubuntugis_unstable_rsgislib_5.0.11-1.deb
 - https://envsys-public.s3.amazonaws.com/ubuntu/rsgislib/focal_ubuntugis_stable_python3-rsgislib_5.0.11-1.deb
 - https://envsys-public.s3.amazonaws.com/ubuntu/rsgislib/focal_ubuntugis_unstable_python3-rsgislib_5.0.11-1.deb

You can get a listing of envsys-public s3 bucket here:

https://envsys-public.s3.amazonaws.com/
