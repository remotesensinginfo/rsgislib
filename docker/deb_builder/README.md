# To build .deb packages

Run the following command **from the root of the repository**

`docker build . -f docker/deb_builder/focal_ubuntugis_stable/Dockerfile -t rsgislib_focal_deb_builder`

This creates a docker image which should have the debian built in /usr/local/packages from where you can copy it out, e.g.,

`docker run --rm --entrypoint cat rsgislib_focal_deb_builder /usr/local/packages/rsgislib_5.0.5-1.deb > /tmp/rsgislib_5.0.5-1.deb`
