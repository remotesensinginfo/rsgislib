#!/usr/bin/env python
"""
The tools.httptools
"""
from typing import Dict
import json
import os
import requests
import rsgislib
import tqdm


class RSGISPyResponseException(rsgislib.RSGISPyException):
    def __init__(self, value, response=None):
        """
        Init for the RSGISPyResponseException class
        """
        self.value = value
        self.response = response

    def __str__(self):
        """
        Return a string representation of the exception
        """
        return "HTTP status {0} {1}: {2}".format(
            self.response.status_code, self.response.reason, repr(self.value)
        )


def check_http_response(response: requests.Response, url: str) -> bool:
    """
    Check the HTTP response and raise an exception with appropriate error message
    if request was not successful.

    :param response:
    :param url:
    :return:

    """
    try:
        response.raise_for_status()
        success = True
    except (requests.HTTPError, ValueError):
        success = False
        excpt_msg = "Invalid API response."
        try:
            excpt_msg = response.headers["cause-message"]
        except:
            try:
                excpt_msg = response.json()["error"]["message"]["value"]
            except:
                excpt_msg = (
                    "Unknown error ('{0}'), check url in a web browser: '{1}'".format(
                        response.reason, url
                    )
                )
        api_error = RSGISPyResponseException(excpt_msg, response)
        api_error.__cause__ = None
        raise api_error
    return success


def send_http_json_request(url: str, data: Dict = None, api_key: str = None) -> str:
    """
    A function which sends a http request with a json data packet.
    If an error occurs an exception will be raised.

    :param url: The URL for the request to be sent.
    :param data: dictionary of data which can be converted to str
                 using json.dumps.
    :param api_key: if provided then the api-key will be provided
                    via the http header.
    :return: the JSON string returned by the server.

    """
    if data is None:
        json_data = None
    else:
        json_data = json.dumps(data)

    if api_key == None:
        response = requests.post(url, json_data)
    else:
        headers = {"X-Auth-Token": api_key}
        response = requests.post(url, json_data, headers=headers)

    try:
        http_status_code = response.status_code
        if response == None:
            raise rsgislib.RSGISPyException("No output from service")

        output = json.loads(response.text)
        if output["errorCode"] != None:
            raise rsgislib.RSGISPyException(
                "{} - {}".format(output["errorCode"], output["errorMessage"])
            )
        if http_status_code == 404:
            raise rsgislib.RSGISPyException("404 Not Found")
        elif http_status_code == 401:
            raise rsgislib.RSGISPyException("401 Unauthorized")
        elif http_status_code == 400:
            raise rsgislib.RSGISPyException(f"Error Code: {http_status_code}")
    except Exception as e:
        response.close()
        raise rsgislib.RSGISPyException(f"{e}")
    response.close()

    return output["data"]


def download_file_http(
    input_url: str,
    out_file_path: str,
    username: str = None,
    password: str = None,
    no_except: bool = True,
):
    """

    :param input_url:
    :param out_file_path:
    :param username:
    :param password:
    :return:

    """
    session_http = requests.Session()
    if (username is not None) and (password is not None):
        session_http.auth = (username, password)
    user_agent = "rsgislib/{}".format(rsgislib.get_rsgislib_version())
    session_http.headers["User-Agent"] = user_agent

    tmp_dwnld_path = out_file_path + ".incomplete"

    headers = {}

    try:
        with session_http.get(
            input_url, stream=True, auth=session_http.auth, headers=headers
        ) as r:
            check_http_response(r, input_url)
            total = int(r.headers.get("content-length", 0))
            chunk_size = 2**20
            n_chunks = int(total / chunk_size) + 1

            with open(tmp_dwnld_path, "wb") as f:
                for chunk in tqdm.tqdm(
                    r.iter_content(chunk_size=chunk_size), total=n_chunks
                ):
                    if chunk:  # filter out keep-alive new chunks
                        f.write(chunk)
        os.rename(tmp_dwnld_path, out_file_path)
        print("Download Complete: {}".format(out_file_path))

    except Exception as e:
        if no_except:
            print(e)
        else:
            raise rsgislib.RSGISPyException("{}".format(e))
        return False
    return True
