#! /usr/bin/env python
############################################################################
#  dropbox.py
#
#  Copyright 2020 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose: Provide a set of tools to interface with dropbox, for
#          example, upload a file to dropbox and create a shared
#          link.
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 02/04/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################

import dropbox
import os
import tqdm

class RSGISDropboxUtils():

    def __init__(self, dbx_key):
        """
        Initialise with an access token, which can be
        generated from https://www.dropbox.com/developers/apps

        :param dbx_key: string with the access token.

        """
        self.dbx_key = dbx_key

    def get_chunk_size(self, size_mb):
        """
        Converts a size in Mb to bytes.
        :param size_mb: size in megabytes.
        :return: size in bytes

        """
        return size_mb * 1024 * 1024

    def upload_file(self, input_file, dbx_dest_path, share_link=False, timeout=900, chunk_size=4 * 1024 * 1024):
        """
        A function which will upload a file to dropbox and optionally create a shared link to that file.

        :param input_file: The local input file path (e.g., /my/local/dir/hello_world.txt).
        :param dbx_dest_path: The file remote file path on dropbox (e.g., /my/dropbox/transfer/hello_world.txt)
        :param share_link: a boolean specifying whether a shared link should be created for the file. Default False.
                           If True then a shared link will be created and returned by the function.
        :param timeout: The timeout in seconds for the connection (Default: 900).
        :param chunk_size: The chunk size the file will be uploaded in. Default 4Mb. If larger then upload might
                           be faster but errors are more likely to occur so increase if you have a faster stable
                           connection. Feedback is provided in chunk_size intervals so if you increase this you
                           will get less feedback on the upload progress.
        :return: If share_link=True then the link URL will be returned otherwise None will be returned.

        """
        dbx = dropbox.Dropbox(self.dbx_key, timeout=timeout)

        if not os.path.exists(input_file):
            raise Exception("The specified input file does not exist: '{}'".format(input_file))

        file_size = os.path.getsize(input_file)
        with open(input_file, 'rb') as f:
            if file_size <= chunk_size:
                print("File size is less than chunk size so uploading as single file.")
                dbx.files_upload(f.read(), dbx_dest_path)
            else:
                pbar = tqdm.tqdm(total=file_size)
                counter = 0
                upload_session_start_result = dbx.files_upload_session_start(f.read(chunk_size))
                cursor = dropbox.files.UploadSessionCursor(session_id=upload_session_start_result.session_id,
                                                           offset=f.tell())
                commit = dropbox.files.CommitInfo(path=dbx_dest_path)
                pbar.update(chunk_size)
                while f.tell() < file_size:
                    if ((file_size - f.tell()) <= chunk_size):
                        pbar_up_val = (file_size - f.tell())
                        file_info = dbx.files_upload_session_finish(f.read(chunk_size), cursor, commit)
                        pbar.update(pbar_up_val)
                    else:
                        dbx.files_upload_session_append(f.read(chunk_size), cursor.session_id, cursor.offset)
                        cursor.offset = f.tell()
                        pbar.update(chunk_size)
                pbar.close()

        link_url = None
        if share_link:
            link_url = self.check_if_file_shared(dbx_dest_path)
            if link_url is None:
                link = dbx.sharing_create_shared_link_with_settings(dbx_dest_path)
                link_url = link.url
        return link_url

    def check_if_file_shared(self, dbx_dest_path):
        """
        A function which checks whether the path on dropbox has a shared link. If a link exists it is returned
        otherwise None is returned.

        :param dbx_dest_path: The file remote file path on dropbox (e.g., /my/dropbox/hello_world.txt)
        :return: If a link exists the URL is returned otherwise None is returned.

        """
        dbx = dropbox.Dropbox(self.dbx_key)
        share_metadata = dbx.sharing_list_shared_links(dbx_dest_path)
        link_url = None
        if len(share_metadata.links) > 0:
            link_url = share_metadata.links[0].url
        return link_url

    def upload_files(self, file_lst, dbx_dest_path, share_link=False, timeout=900, chunk_size=4 * 1024 * 1024):
        """
        A function which uses the upload_file function to upload and optionally generated shared links for
        each of the files.

        :param file_lst: a list of local file paths.
        :param dbx_dest_path: The directory remote file path on dropbox where the files will be uploaded
                              (e.g., /my/dropbox/Transfer)
        :param share_link: a boolean specifying whether a shared link should be created for the file. Default False.
                           If True then a shared link will be created and returned by the function.
        :param timeout: The timeout in seconds for the connection (Default: 900).
        :param chunk_size: The chunk size the file will be uploaded in. Default 4Mb. If larger then upload might
                           be faster but errors are more likely to occur so increase if you have a faster stable
                           connection. Feedback is provided in chunk_size intervals so if you increase this you
                           will get less feedback on the upload progress.
        :return: A dict is returned with the file names as keys and the dropbox path and, if generated, the
                 shared link URL.

        """
        file_info = dict()
        for cfile in file_lst:
            cfile_basename = os.path.basename(cfile)
            file_info[cfile_basename] = dict()
            file_info[cfile_basename]['dbx_path'] = os.path.join(dbx_dest_path, cfile_basename)
            link_url = self.upload_file(cfile, file_info[cfile_basename]['dbx_path'], share_link=share_link,
                                        timeout=timeout, chunk_size=chunk_size)
            if share_link:
                file_info[cfile_basename]['share_url'] = link_url
        return file_info

    def get_shared_url_lst(self, file_info):
        """
        Using the output from upload_files this function generates a list of the shared link URLs.

        :param file_info: dict object as outputted from upload_files.
        :return: returns a list of URLs.

        """
        url_lst = []
        for cfile in file_info:
            if 'share_url' in file_info[cfile]:
                url_lst.append(file_info[cfile]['share_url'])
        return url_lst


def upload_email_links(dbx_key, file_lst, dbx_dest_path, email_subject, receiver_email, timeout=900,
                       chunk_size=4 * 1024 * 1024, sender_email=None, smtp_server=None,
                       smtp_port=None, smtp_password=None):
    """

    :param dbx_key: string with the dropbox access token
    :param file_lst: a list of local file paths.
    :param dbx_dest_path: The directory remote file path on dropbox where the files will be uploaded
                          (e.g., /my/dropbox/Transfer)
    :param email_subject: The subject of the email to be sent.
    :param receiver_email: The receivers email address.
    :param timeout: dropbox timeout. Default: 900 seconds.
    :param chunk_size: dropbox upload chunk size in bytes (Default: 4Mb)
    :param sender_email: The email address of the sender. If None then will read variable from RSGIS_SENDER_EMAIL.
    :param smtp_server: The SMTP server to send the email. If None then will read variable from RSGIS_SMPT_SERVER.
    :param smtp_port: The SMTP server port. If None then will read variable from RSGIS_SMTP_PORT.
    :param smtp_password: The password for the SMTP server. If None then will read variable from RSGIS_PASSWORD.

    """
    from rsgislib.tools.notify_utils import send_email_notification
    dbx_utils = RSGISDropboxUtils(dbx_key)
    file_info = dbx_utils.upload_files(file_lst, dbx_dest_path, share_link=True, timeout=timeout, chunk_size=chunk_size)
    shared_urls = dbx_utils.get_shared_url_lst(file_info)
    msg_txt = 'URLs for the shared files:\n'
    for file_url in shared_urls:
        msg_txt = "{}\t{}\n".format(msg_txt, file_url)
    msg_txt = "{}\n\n This is an automated email, generated from the " \
              "RSGISLib (https://www.rsgislib.org) software\n\n".format(msg_txt)

    send_email_notification(msg_txt, email_subject, receiver_email, sender_email, smtp_server, smtp_port,
                            smtp_password)
