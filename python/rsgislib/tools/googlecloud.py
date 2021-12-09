#!/usr/bin/env python
"""
The tools.google_cloud module contains functions for manipulating
"""
import os


def blob_exists(goog_cred, bucket_name, filename):
    """
    A function to test whether a file .

    :param file_to_upload: The local file to be uploaded.
    :param goog_cred: The JSON credentials file from Google.
    :param bucket_name: The google bucket name.
    :param filename: The file path within the bucket for the file. Note this should not start
                     with a slash. i.e., directory/path rather than /directory/path.
    :return: boolean (True if the file exists / False if the file does not exist)
    """
    from google.cloud import storage

    client = storage.Client.from_service_account_json(goog_cred)
    bucket = client.get_bucket(bucket_name)
    blob = bucket.blob(filename)
    return blob.exists()


def list_files(goog_cred, bucket_name, bucket_dir):
    """
    A function to list the contents of a directory within a bucket.

    :param goog_cred: The JSON credentials file from Google.
    :param bucket_name: The google bucket name.
    :param bucket_dir: The directory path within the bucket to be listed. Note this
                       should not start with a slash. i.e., directory/path rather than /directory/path.
    :return: returns a list of file paths.

    """
    from google.cloud import storage

    client = storage.Client.from_service_account_json(goog_cred)
    bucket = client.get_bucket(bucket_name)
    files = bucket.list_blobs(prefix=bucket_dir)
    fileList = [file.name for file in files if "." in file.name]
    return fileList


def upload_to_google_bucket(file_to_upload, goog_cred, bucket_name, bucket_dir):
    """
    A function to upload a file to a google cloud bucket. Once the file is uploaded the presence
    of the file is checked. If the file is not present then an exception is raised.

    :param file_to_upload: The local file to be uploaded.
    :param goog_cred: The JSON credentials file from Google.
    :param bucket_name: The google bucket name.
    :param bucket_dir: The directory path within the bucket for the file to be saved. Note this
                       should not start with a slash. i.e., directory/path rather than /directory/path.

    """
    from google.cloud import storage

    # Explicitly use service account credentials by specifying the private key file.
    storage_client = storage.Client.from_service_account_json(goog_cred)

    lcl_path, file_name = os.path.split(file_to_upload)
    bucket_file_path = os.path.join(bucket_dir, file_name)
    bucket_obj = storage_client.get_bucket(bucket_name)
    blob_obj = bucket_obj.blob(bucket_file_path)
    blob_obj.upload_from_filename(file_to_upload)
    if not blob_exists(goog_cred, bucket_name, bucket_file_path):
        raise Exception(
            "File was not successfully uploaded to Google Cloud: '{}'".format(
                file_to_upload
            )
        )
