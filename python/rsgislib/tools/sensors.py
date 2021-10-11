#!/usr/bin/env python
"""
The tools and utilities for specific sensors.
"""

def read_landsat_mtl_to_dict(in_mtl_file):
    """
    A function which can read a Landsat MTL header file and return a
    structured dict with the information from the MTL file.

    :param in_mtl_file: the file path to MTL header file (*MTL.txt)
    :return: dict structure.

    """
    h_file = open(in_mtl_file, 'r')
    header_params = dict()
    for line in h_file:
        line = line.strip()
        if line:
            line_vals = line.split('=')
            if len(line_vals) == 2:
                if (line_vals[0].strip() == "GROUP"):
                    c_dict = dict()
                    header_params[line_vals[1].strip()] = c_dict
                elif (line_vals[0].strip() != "END_GROUP"):
                    c_dict[line_vals[0].strip()] = line_vals[1].strip().replace('"','')
    h_file.close()
    return header_params

