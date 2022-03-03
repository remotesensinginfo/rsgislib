#!/usr/bin/env python
"""
The tools.httptools
"""
from typing import Dict
import json
import requests
import rsgislib


def send_http_json_request(url:str, data:Dict, api_key:str=None)->str:
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
    json_data = json.dumps(data)

    if api_key == None:
        response = requests.post(url, json_data)
    else:
        headers = {'X-Auth-Token': api_key}
        response = requests.post(url, json_data, headers=headers)

    try:
        http_status_code = response.status_code
        if response == None:
            raise rsgislib.RSGISPyException("No output from service")

        output = json.loads(response.text)
        if output['errorCode'] != None:
            raise rsgislib.RSGISPyException("{} - {}".format(output['errorCode'], output['errorMessage']))
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

    return output['data']