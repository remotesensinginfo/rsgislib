# To build .deb packages

Run the following command **from the root of the repository**

`docker build . -f contrib/ubuntu/focal_ubuntugis_stable/Dockerfile -t rsgislib_focal_deb_builder`

This creates a docker image which should have:

* rsgislib - A debian package without the python files
* python3-rsgislib - A debian package containing the python files for rsgislib
* python pip packages - A wheel and source package that can be installed with pip

in /usr/local/packages from where you can copy it out, e.g.,

`docker run --rm --entrypoint cat rsgislib_focal_deb_builder /usr/local/packages/rsgislib_5.0.11-1.deb > /tmp/rsgislib_5.0.11-1.deb`

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
