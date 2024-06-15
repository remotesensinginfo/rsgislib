from typing import List
import os
import shutil
import tqdm


class ImageTimeCluster(object):
    """
    A class which supports the split_photos_by_time function.
    """

    def __init__(self, cluster_id=None, init_time=None):
        """

        :param cluster_id: unique id for the cluster
        :param init_time: is the initial time of the first image

        """
        self.cluster_id = cluster_id
        self.start_time = init_time
        self.end_time = init_time
        self.images = list()

    def within_cluster(self, date_time_obj, time_split_secs=60):
        date_time_in_cluster = False
        start_diff = self.start_time - date_time_obj
        if abs(start_diff.total_seconds()) < time_split_secs:
            date_time_in_cluster = True

        if not date_time_in_cluster:
            end_diff = self.end_time - date_time_obj
            if abs(end_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True
        return date_time_in_cluster

    def add_image_to_cluster(self, image_file, date_time_obj):
        self.images.append(image_file)
        if date_time_obj < self.start_time:
            self.start_time = date_time_obj
        elif date_time_obj > self.end_time:
            self.end_time = date_time_obj

    def does_cluster_overlap(self, cluster_obj, time_split_secs=60):
        date_time_in_cluster = False
        time_diff = self.start_time - cluster_obj.start_time
        if abs(time_diff.total_seconds()) < time_split_secs:
            date_time_in_cluster = True

        if not date_time_in_cluster:
            time_diff = self.end_time - cluster_obj.end_time
            if abs(time_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True

        if not date_time_in_cluster:
            time_diff = self.start_time - cluster_obj.end_time
            if abs(time_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True

        if not date_time_in_cluster:
            time_diff = self.end_time - cluster_obj.start_time
            if abs(time_diff.total_seconds()) < time_split_secs:
                date_time_in_cluster = True

        if not date_time_in_cluster:
            if (cluster_obj.start_time > self.start_time) and (
                cluster_obj.start_time < self.end_time
            ):
                date_time_in_cluster = True

        if not date_time_in_cluster:
            if (cluster_obj.end_time > self.start_time) and (
                cluster_obj.end_time < self.end_time
            ):
                date_time_in_cluster = True

        return date_time_in_cluster

    def merge_cluster(self, cluster_obj):
        self.images = self.images + cluster_obj.images
        if cluster_obj.start_time < self.start_time:
            self.start_time = cluster_obj.start_time
        elif cluster_obj.end_time > self.end_time:
            self.end_time = cluster_obj.end_time

    def __str__(self):
        return "{}: {} - {} = {}".format(
            self.cluster_id, self.start_time, self.end_time, len(self.images)
        )

    def __repr__(self):
        return "{}: {} - {} = {}".format(
            self.cluster_id, self.start_time, self.end_time, len(self.images)
        )


def split_photos_by_time(
    input_imgs,
    output_base_dir,
    time_split_secs=60,
    date_time_tag="DateTime",
    date_time_format="%Y:%m:%d %H:%M:%S",
    out_date_time_format="%Y%m%d_%H%M%S",
):
    """
    A function which splits a set of images (e.g., from drone surveys) by time.
    Where photos taken within the specified time window will be clustered together
    (default: 60 seconds). The photos are then copied into a new directory based on
    the start time of the cluster. Note, photos are copied and not moved from their
    current location.

    :param input_imgs: a list of input images (e.g., glob.glob("*.jpg"))
    :param output_base_dir: the output base directory where the output directories
                            will be created.
    :param time_split_secs: The time difference in seconds for a new cluster to be
                            created.
    :param date_time_tag: the EXIF tag for the date / time of the acquisition.
                          Default is 'DataTime' but this might be different for
                          different cameras and therefore you might need to use the
                          list_exif_info function to find the correct field.
    :param date_time_format: The python datetime.strptime format string for the
                             datetime provided in the EXIF metadata.
                             Default: %Y:%m:%d %H:%M:%S
    :param out_date_time_format: The python datetime.strftime format string for the
                                 used to create the output directory for the files.
                                 Note, this must be unique. Default: %Y%m%d_%H%M%S

    """
    import datetime

    import tqdm
    from PIL import Image
    from PIL.ExifTags import TAGS

    img_dt_lut = dict()
    for img_file in input_imgs:
        pil_image = Image.open(img_file)
        pil_exif_data = pil_image.getexif()

        for tag_id in pil_exif_data:
            # get the tag name, instead of human unreadable tag id
            tag = TAGS.get(tag_id, tag_id)
            try:
                data = pil_exif_data.get(tag_id)
                # decode bytes
                if isinstance(data, bytes):
                    data = data.decode()
                if tag == date_time_tag:
                    img_dt_lut[img_file] = data
            except:
                continue

    img_clusters = list()
    next_cluster_id = 0
    first = True
    for img_file in img_dt_lut:
        date_time_obj = datetime.datetime.strptime(
            img_dt_lut[img_file], date_time_format
        )

        if first:
            img_cluster_obj = ImageTimeCluster(next_cluster_id, date_time_obj)
            img_cluster_obj.images.append(img_file)
            img_clusters.append(img_cluster_obj)
            next_cluster_id += 1
            first = False
        else:
            for img_cluster in img_clusters:
                if img_cluster.within_cluster(date_time_obj, time_split_secs):
                    img_cluster.add_image_to_cluster(img_file, date_time_obj)
                    break
                else:
                    img_cluster_obj = ImageTimeCluster(next_cluster_id, date_time_obj)
                    img_cluster_obj.images.append(img_file)
                    img_clusters.append(img_cluster_obj)
                    next_cluster_id += 1
                    break

        merged_cluster = False
        for img_cluster_out in img_clusters:
            for img_cluster_in in img_clusters:
                if img_cluster_out.cluster_id != img_cluster_in.cluster_id:
                    cluster_overlap = img_cluster_out.does_cluster_overlap(
                        img_cluster_in
                    )
                    if cluster_overlap:
                        merged_cluster = True
                        img_cluster_out.merge_cluster(img_cluster_in)
                        img_clusters.remove(img_cluster_in)
                        break
            if merged_cluster:
                break

    for img_cluster in img_clusters:
        print("Copying Files for: {}".format(img_cluster))
        out_dir_name = img_cluster.start_time.strftime(out_date_time_format)
        out_dir_path = os.path.join(output_base_dir, out_dir_name)
        print("Output Directory: {}".format(out_dir_path))
        if not os.path.exists(out_dir_path):
            os.mkdir(out_dir_path)
        for img in tqdm.tqdm(img_cluster.images):
            shutil.copy(img, out_dir_path)


def create_flightline_vec(
    input_imgs: List[str],
    pts_vec_file: str,
    pts_vec_lyr: str,
    line_vec_file: str,
    line_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which takes a list input images, which have the GPS data within
    the EXIF data, and creates a vector layer of points with the key metadata
    and a line vector layer of the flightline. If the output vector file can
    take multiple layers (e.g., GPKG) then the same file can be provided for
    both pts_vec_file and line_vec_file.

    :param input_imgs: List of input images
    :param pts_vec_file: Output vector file for the photo points
    :param pts_vec_lyr: Output vector layer for the photo points
    :param line_vec_file: Output vector file for the flightline
    :param line_vec_lyr: Output vector layer for the flightline
    :param out_format: output vector format

    """
    # Based on code from https://github.com/spifftek70/Drone-Footprints
    import rsgislib.tools.filetools
    import datetime
    import pandas
    import geopandas
    import shapely.geometry
    import exiftool

    def _img_date_sort(e):
        img_path = list(e.keys())[0]
        img_date = e[img_path]["EXIF:DateTimeOriginal"]
        img_date_obj = datetime.datetime.strptime(img_date, "%Y:%m:%d %H:%M:%S")
        return img_date_obj

    # Sort the input images so they in the order they were acquired.
    input_imgs = rsgislib.tools.filetools.sort_file_by_datetime(input_imgs)
    print(f"There are {len(input_imgs)} input images")

    # Read the exif metadata from the input image files
    exif_dict = list()
    et = exiftool.ExifToolHelper()
    for img in input_imgs:
        exif_dict.append({img: et.get_metadata(img)[0]})
    exif_dict.sort(key=_img_date_sort)

    img_info = dict()
    img_info["lat"] = list()
    img_info["lon"] = list()
    img_info["relative_altitude"] = list()
    img_info["absolute_altitude"] = list()

    img_info["gimbal_roll_degree"] = list()
    img_info["gimbal_pitch_degree"] = list()
    img_info["gimbal_yaw_degree"] = list()
    img_info["flight_pitch_degree"] = list()
    img_info["flight_roll_degree"] = list()
    img_info["flight_yaw_degree"] = list()

    img_info["focal_length"] = list()
    img_info["focal_length35mm"] = list()
    img_info["image_width"] = list()
    img_info["image_height"] = list()
    img_info["max_aperture_value"] = list()
    img_info["datetime_original"] = list()
    img_info["sensor_model_data"] = list()
    img_info["sensor_index"] = list()
    img_info["image_filename"] = list()

    for img_meta_dict in exif_dict:
        img = list(img_meta_dict.keys())[0]
        img_exif = img_meta_dict[img]
        img_info["image_filename"].append(os.path.split(img)[1])

        # Get Location Info
        img_info["lat"].append(
            float(
                img_exif.get("Composite:GPSLatitude")
                or img_exif.get("EXIF:GPSLatitude")
            )
        )
        img_info["lon"].append(
            float(
                img_exif.get("Composite:GPSLongitude")
                or img_exif.get("EXIF:GPSLongitude")
            )
        )

        img_info["relative_altitude"].append(
            float(
                img_exif.get("XMP:RelativeAltitude")
                or img_exif.get("Composite:GPSAltitude")
            )
        )
        img_info["absolute_altitude"].append(
            float(
                img_exif.get("XMP:AbsoluteAltitude")
                or img_exif.get("Composite:GPSAltitude")
            )
        )

        # Get Gimbal and Flight Orientation Info
        img_info["gimbal_roll_degree"].append(
            float(
                img_exif.get("XMP:GimbalRollDegree")
                or img_exif.get("MakerNotes:CameraRoll")
                or img_exif.get("XMP:Roll")
            )
        )
        img_info["gimbal_pitch_degree"].append(
            float(
                img_exif.get("XMP:GimbalPitchDegree")
                or img_exif.get("MakerNotes:CameraPitch")
                or img_exif.get("XMP:Pitch")
            )
        )
        img_info["gimbal_yaw_degree"].append(
            float(
                img_exif.get("XMP:GimbalYawDegree")
                or img_exif.get("MakerNotes:CameraYaw")
                or img_exif.get("XMP:Yaw")
            )
        )

        flight_pitch_degree_tmp = float(
            img_exif.get("XMP:FlightPitchDegree")
            or img_exif.get("MakerNotes:Pitch")
            or 999
        )
        flight_roll_degree_tmp = float(
            img_exif.get("XMP:FlightRollDegree")
            or img_exif.get("MakerNotes:Roll")
            or 999
        )
        flight_yaw_degree_tmp = float(
            img_exif.get("XMP:FlightYawDegree") or img_exif.get("MakerNotes:Yaw") or 999
        )

        if flight_pitch_degree_tmp is None:
            flight_pitch_degree_tmp = img_info["gimbal_roll_degree"][-1]
        if flight_roll_degree_tmp is None:
            flight_roll_degree_tmp = img_info["gimbal_pitch_degree"][-1]
        if flight_yaw_degree_tmp is None:
            flight_yaw_degree_tmp = img_info["gimbal_yaw_degree"][-1]

        img_info["flight_pitch_degree"].append(flight_pitch_degree_tmp)
        img_info["flight_roll_degree"].append(flight_roll_degree_tmp)
        img_info["flight_yaw_degree"].append(flight_yaw_degree_tmp)

        # Get Lens, Image and Sensor Info
        img_info["focal_length"].append(float(img_exif.get("EXIF:FocalLength")))
        img_info["focal_length35mm"].append(
            float(img_exif.get("EXIF:FocalLengthIn35mmFormat"))
        )
        img_info["image_width"].append(
            int(img_exif.get("EXIF:ImageWidth") or img_exif.get("EXIF:ExifImageWidth"))
        )
        img_info["image_height"].append(
            int(
                img_exif.get("EXIF:ImageHeight") or img_exif.get("EXIF:ExifImageHeight")
            )
        )
        img_info["max_aperture_value"].append(img_exif.get("EXIF:MaxApertureValue"))
        img_info["datetime_original"].append(img_exif.get("EXIF:DateTimeOriginal"))
        img_info["sensor_model_data"].append(img_exif.get("EXIF:Model"))
        img_info["sensor_index"].append(
            str(img_exif.get("XMP:RigCameraIndex") or img_exif.get("XMP:SensorIndex"))
        )

    imgs_df = pandas.DataFrame(img_info)
    imgs_gdf = geopandas.GeoDataFrame(
        imgs_df,
        geometry=geopandas.points_from_xy(imgs_df.lon, imgs_df.lat),
        crs="EPSG:4326",
    )

    if len(imgs_gdf) > 0:
        if out_format == "GPKG":
            imgs_gdf.to_file(pts_vec_file, layer=pts_vec_lyr, driver=out_format)
        else:
            imgs_gdf.to_file(pts_vec_file, driver=out_format)

        flightline_ids = list()
        flightline_geoms = list()
        first = True
        id = 1
        for lon_lat in zip(imgs_df.lon, imgs_df.lat):
            if first:
                first = False
                start_lon_lon = lon_lat
            else:
                flightline_ids.append(id)
                flightline_geoms.append(
                    shapely.geometry.LineString(
                        [
                            shapely.geometry.Point(start_lon_lon),
                            shapely.geometry.Point(lon_lat),
                        ]
                    )
                )
                id += 1
                start_lon_lon = lon_lat

        flightline_gdf = geopandas.GeoDataFrame(
            data={"ID": flightline_ids, "geometry": flightline_geoms},
            geometry="geometry",
            crs="EPSG:4326",
        )
        if out_format == "GPKG":
            flightline_gdf.to_file(line_vec_file, layer=line_vec_lyr, driver=out_format)
        else:
            flightline_gdf.to_file(line_vec_file, driver=out_format)


def create_flightline_extern_gps_vec(
    gps_data_df,
    input_imgs_dir: str,
    pts_vec_file: str,
    pts_vec_lyr: str,
    line_vec_file: str,
    line_vec_lyr: str,
    out_format: str = "GPKG",
):
    """
    A function which takes an external GPS dataset and the photo image files
    and creates a vector layer of points with the key metadata and a line
    vector layer of the flightline. If the output vector file can take
    multiple layers (e.g., GPKG) then the same file can be provided for
    both pts_vec_file and line_vec_file.

    The input pandas DataFrame needs to have the following columns:
    image, lat, lon, alt, roll, pitch, yaw

    The columns should be:
    image - image file name
    lat - latitude
    lon - longitude
    alt - absolute altitude
    roll - flight roll
    pitch - flight pitch
    yaw - flight yaw

    :param gps_data_df: A pandas DataFrame with the external GPS data.
    :param input_imgs_dir: The directory with input images
    :param pts_vec_file: Output vector file for the photo points
    :param pts_vec_lyr: Output vector layer for the photo points
    :param line_vec_file: Output vector file for the flightline
    :param line_vec_lyr: Output vector layer for the flightline
    :param out_format: output vector format

    """
    # Based on code from https://github.com/spifftek70/Drone-Footprints
    import datetime
    import pandas
    import geopandas
    import shapely.geometry
    import exiftool

    def _img_date_sort(e):
        img_path = list(e.keys())[0]
        img_date = e[img_path]["exif"]["EXIF:DateTimeOriginal"]
        img_date_obj = datetime.datetime.strptime(img_date, "%Y:%m:%d %H:%M:%S")
        return img_date_obj

    exif_dict = list()
    et = exiftool.ExifToolHelper()
    for index, row in gps_data_df.iterrows():
        input_img = os.path.join(input_imgs_dir, row["image"])
        if os.path.exists(input_img):
            gps_data = {
                "lat": row["lat"],
                "lon": row["lon"],
                "alt": row["alt"],
                "roll": row["roll"],
                "pitch": row["pitch"],
                "yaw": row["yaw"],
            }
            exif_dict.append(
                {
                    input_img: {
                        "gps_data": gps_data,
                        "exif": et.get_metadata(input_img)[0],
                    }
                }
            )
    exif_dict.sort(key=_img_date_sort)
    print(f"There were {len(exif_dict)} images found matching the GPS data.")

    img_info = dict()
    img_info["lat"] = list()
    img_info["lon"] = list()
    img_info["absolute_altitude"] = list()

    img_info["gimbal_roll_degree"] = list()
    img_info["gimbal_pitch_degree"] = list()
    img_info["gimbal_yaw_degree"] = list()
    img_info["flight_pitch_degree"] = list()
    img_info["flight_roll_degree"] = list()
    img_info["flight_yaw_degree"] = list()

    img_info["focal_length"] = list()
    img_info["focal_length35mm"] = list()
    img_info["image_width"] = list()
    img_info["image_height"] = list()
    img_info["max_aperture_value"] = list()
    img_info["datetime_original"] = list()
    img_info["sensor_model_data"] = list()
    img_info["sensor_index"] = list()
    img_info["image_filename"] = list()

    for img_meta_dict in exif_dict:
        img = list(img_meta_dict.keys())[0]
        img_exif = img_meta_dict[img]["exif"]
        img_gps_data = img_meta_dict[img]["gps_data"]

        img_info["image_filename"].append(os.path.split(img)[1])

        # Get Location Info
        img_info["lat"].append(float(img_gps_data["lat"]))
        img_info["lon"].append(float(img_gps_data["lon"]))
        img_info["absolute_altitude"].append(float(img_gps_data["alt"]))

        # Get Gimbal and Flight Orientation Info
        img_info["gimbal_roll_degree"].append(
            float(
                img_exif.get("XMP:GimbalRollDegree")
                or img_exif.get("MakerNotes:CameraRoll")
                or img_exif.get("XMP:Roll")
                or 0.0
            )
        )

        img_info["gimbal_pitch_degree"].append(
            float(
                img_exif.get("XMP:GimbalPitchDegree")
                or img_exif.get("MakerNotes:CameraPitch")
                or img_exif.get("XMP:Pitch")
                or 0.0
            )
        )

        img_info["gimbal_yaw_degree"].append(
            float(
                img_exif.get("XMP:GimbalYawDegree")
                or img_exif.get("MakerNotes:CameraYaw")
                or img_exif.get("XMP:Yaw")
                or 0.0
            )
        )

        img_info["flight_pitch_degree"].append(
            float(
                img_exif.get("XMP:FlightPitchDegree")
                or img_exif.get("MakerNotes:Pitch")
                or img_gps_data["pitch"]
            )
        )
        img_info["flight_roll_degree"].append(
            float(
                img_exif.get("XMP:FlightRollDegree")
                or img_exif.get("MakerNotes:Roll")
                or img_gps_data["roll"]
            )
        )
        img_info["flight_yaw_degree"].append(
            float(
                img_exif.get("XMP:FlightYawDegree")
                or img_exif.get("MakerNotes:Yaw")
                or img_gps_data["yaw"]
            )
        )

        # Get Lens, Image and Sensor Info
        img_info["focal_length"].append(float(img_exif.get("EXIF:FocalLength")))
        img_info["focal_length35mm"].append(
            float(img_exif.get("EXIF:FocalLengthIn35mmFormat"))
        )
        img_info["image_width"].append(
            int(img_exif.get("EXIF:ImageWidth") or img_exif.get("EXIF:ExifImageWidth"))
        )
        img_info["image_height"].append(
            int(
                img_exif.get("EXIF:ImageHeight") or img_exif.get("EXIF:ExifImageHeight")
            )
        )
        img_info["max_aperture_value"].append(img_exif.get("EXIF:MaxApertureValue"))
        img_info["datetime_original"].append(img_exif.get("EXIF:DateTimeOriginal"))
        img_info["sensor_model_data"].append(img_exif.get("EXIF:Model"))
        img_info["sensor_index"].append(
            str(img_exif.get("XMP:RigCameraIndex") or img_exif.get("XMP:SensorIndex"))
        )

    imgs_df = pandas.DataFrame(img_info)
    imgs_gdf = geopandas.GeoDataFrame(
        imgs_df,
        geometry=geopandas.points_from_xy(imgs_df.lon, imgs_df.lat),
        crs="EPSG:4326",
    )

    if len(imgs_gdf) > 0:
        if out_format == "GPKG":
            imgs_gdf.to_file(pts_vec_file, layer=pts_vec_lyr, driver=out_format)
        else:
            imgs_gdf.to_file(pts_vec_file, driver=out_format)

        flightline_ids = list()
        flightline_geoms = list()
        first = True
        id = 1
        for lon_lat in zip(imgs_df.lon, imgs_df.lat):
            if first:
                first = False
                start_lon_lon = lon_lat
            else:
                flightline_ids.append(id)
                flightline_geoms.append(
                    shapely.geometry.LineString(
                        [
                            shapely.geometry.Point(start_lon_lon),
                            shapely.geometry.Point(lon_lat),
                        ]
                    )
                )
                id += 1
                start_lon_lon = lon_lat

        flightline_gdf = geopandas.GeoDataFrame(
            data={"ID": flightline_ids, "geometry": flightline_geoms},
            geometry="geometry",
            crs="EPSG:4326",
        )
        if out_format == "GPKG":
            flightline_gdf.to_file(line_vec_file, layer=line_vec_lyr, driver=out_format)
        else:
            flightline_gdf.to_file(line_vec_file, driver=out_format)


def define_extern_gps_in_imgs(
    gps_data_df,
    input_imgs_dir: str,
    flying_height: float,
    gimbal_roll: float,
    gimbal_pitch: float,
    gimbal_yaw: float,
):
    """

    The input pandas DataFrame needs to have the following columns:
    image, lat, lon, alt, roll, pitch, yaw

    The columns should be:
    image - image file name
    lat - latitude
    lon - longitude
    alt - absolute altitude
    roll - flight roll
    pitch - flight pitch
    yaw - flight yaw

    :param gps_data_df: A pandas DataFrame with the external GPS data.
    :param input_imgs_dir: The directory with input images
    :param flying_height: The flying height of the drone to be used as the
                          relative elevation
    :param gimbal_roll: The gimbal roll angle (degrees)
    :param gimbal_pitch: The gimbal pitch angle (degrees)
    :param gimbal_yaw: The gimbal yaw angle (degrees)

    """
    import exiftool

    with exiftool.ExifToolHelper() as et:
        for index, row in tqdm.tqdm(gps_data_df.iterrows()):
            input_img = os.path.join(input_imgs_dir, row["image"])
            if os.path.exists(input_img):
                out_tags = dict()
                # Location
                out_tags["Composite:GPSLatitude"] = float(row["lat"])
                out_tags["EXIF:GPSLatitude"] = float(row["lat"])
                out_tags["Composite:GPSLongitude"] = float(row["lon"])
                out_tags["EXIF:GPSLongitude"] = float(row["lon"])

                # Altitude
                out_tags["XMP:AbsoluteAltitude"] = float(row["alt"])
                out_tags["Composite:GPSAltitude"] = float(row["alt"])
                out_tags["XMP:RelativeAltitude"] = float(flying_height)

                # Pitch
                out_tags["XMP:FlightPitchDegree"] = float(row["pitch"])
                out_tags["MakerNotes:Pitch"] = float(row["pitch"])
                # Roll
                out_tags["XMP:FlightRollDegree"] = float(row["roll"])
                out_tags["MakerNotes:Roll"] = float(row["roll"])
                # Yaw
                out_tags["XMP:FlightYawDegree"] = float(row["yaw"])
                out_tags["MakerNotes:Yaw"] = float(row["yaw"])

                # Gimbal Roll
                out_tags["XMP:GimbalRollDegree"] = float(gimbal_roll)
                out_tags["MakerNotes:CameraRoll"] = float(gimbal_roll)
                out_tags["XMP:Roll"] = float(gimbal_roll)

                # Gimbal Pitch
                out_tags["XMP:GimbalPitchDegree"] = float(gimbal_pitch)
                out_tags["MakerNotes:CameraPitch"] = float(gimbal_pitch)
                out_tags["XMP:Pitch"] = float(gimbal_pitch)

                # Gimbal Yaw
                out_tags["XMP:GimbalYawDegree"] = float(gimbal_yaw)
                out_tags["MakerNotes:CameraYaw"] = float(gimbal_yaw)
                out_tags["XMP:Yaw"] = float(gimbal_yaw)

                et.set_tags(input_img, out_tags)
