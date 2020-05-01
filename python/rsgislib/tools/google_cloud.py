#!/usr/bin/env python
"""
The tools.google_cloud module contains functions for manipulating
"""
import os

def upload_to_google_bucket(file_to_upload, goog_cred, bucket_name, bucket_dir):
    """
    A function to upload a file to a google cloud bucket.

    :param file_to_upload: The local file to be uploaded.
    :param goog_cred: The JSON credentials file from Google.
    :param bucket_name: The google bucket name.
    :param bucket_dir: The directory path within the bucket for the file to be saved.

    """
    from google.cloud import storage
    # Explicitly use service account credentials by specifying the private key  file.
    storage_client = storage.Client.from_service_account_json(goog_cred)

    lcl_path, file_name = os.path.split(file_to_upload)
    bucket_file_path = os.path.join(bucket_dir, file_name)
    bucket_obj = storage_client.get_bucket(bucket_name)
    blob_obj = bucket_obj.blob(bucket_file_path)
    blob_obj.upload_from_filename(file_to_upload)


