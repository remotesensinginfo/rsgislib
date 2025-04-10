#!/usr/bin/env python
"""
The tools.maths module contains some useful tools for maths operations which aren't
easily available else where.

"""
from typing import Union, List

import numpy

import rsgislib


def round_to_nearest_val(
    input_val: Union[numpy.array, int, float, List[int], List[float]],
    round_val: int = 1,
    round_direction: int = rsgislib.ROUND_NEAREST,
):
    """
    Rounds elements of a NumPy array to the nearest multiple of the value specified,
    with options for rounding up, down or to the nearest value. For example, if a
    round_val of 5 is provided then the numbers will be rounded to intervals of 5.

    :param input_val: A NumPy array of numbers.
    :param round_val: The value by which the input is to be rounded to.
    :param round_direction: Specify the round direction (rsgislib.ROUND_NEAREST,
                            rsgislib.ROUND_UP, or rsgislib.ROUND_DOWN). The
                            default is rsgislib.ROUND_NEAREST.
    :return: a single value or a numpy array of the rounded values.

    """
    if isinstance(input_val, list):
        input_val = numpy.array(input_val, dtype=float)

    round_val_flt = float(round_val)
    round_val_int = int(round_val)
    if round_direction == rsgislib.ROUND_NEAREST:
        return numpy.round(input_val / round_val_flt) * round_val_int
    elif round_direction == rsgislib.ROUND_UP:
        return numpy.ceil(input_val / round_val_flt) * round_val_int
    elif round_direction == rsgislib.ROUND_DOWN:
        return numpy.floor(input_val / round_val_flt) * round_val_int
    else:
        raise rsgislib.RSGISPyException(
            f"Invalid round_direction: '{round_direction}'. Please use "
            f"rsgislib.ROUND_NEAREST, rsgislib.ROUND_UP, or rsgislib.ROUND_DOWN."
        )
