#!/usr/bin/env python
"""
The tools.utils module contains some useful tools
"""

import collections.abc
import datetime
import json
import math
import os
import string
from typing import Dict, List, Union, Tuple

import numpy

import rsgislib


class RSGISNumpyArrayEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, numpy.integer):
            return int(obj)
        elif isinstance(obj, (numpy.floating, numpy.complexfloating)):
            return float(obj)
        elif isinstance(obj, numpy.ndarray):
            return obj.tolist()
        elif isinstance(obj, numpy.bool_):
            return bool(obj)
        elif isinstance(obj, numpy.bytes_):
            return str(obj)
        elif isinstance(obj, (numpy.void)):
            return None
        elif isinstance(obj, (datetime.datetime, datetime.date)):
            return obj.isoformat()
        if isinstance(obj, datetime.timedelta):
            return str(obj)
        if hasattr(obj, "to_json"):
            return obj.to_json()
        else:
            return super(RSGISNumpyArrayEncoder, self).default(obj)


def uid_generator(size: int = 6) -> str:
    """
    A function which will generate a 'random' string of the specified length
    based on the UUID

    :param size: the length of the returned string.
    :return: string of length size.

    """
    import uuid

    random_str = str(uuid.uuid4())
    random_str = random_str.replace("-", "")
    return random_str[0:size]


def is_number(str_val: str) -> bool:
    """
    A function which tests whether the input string contains a number of not.

    :return: boolean

    """
    try:
        float(str_val)  # for int, long and float
    except ValueError:
        try:
            complex(str_val)  # for complex
        except ValueError:
            return False
    return True


def similar_numeric_vals(
    val_a: Union[numpy.array, float, int],
    val_b: Union[numpy.array, float, int],
    eql_thres: Union[float, int] = 0.1,
    abs_diff: bool = True,
) -> bool:
    """
    A function which tests whether two number have a similar value within
    a threshold. To calculate the difference: val_a - val_b. If using arrays
    they must have the same length.

    :param val_a: the first value can also be a numpy array
    :param val_b: the second value can also be a numpy array
    :param eql_thres: the threshold below which the numbers are similar.
    :param abs_diff: use the absolute difference between the two values.
    :return: boolean; True values are similar. False values are not similar.

    """
    if isinstance(val_a, collections.abc.Sequence):
        val_a = numpy.array(val_a)
    if isinstance(val_b, collections.abc.Sequence):
        val_b = numpy.array(val_b)

    if abs_diff:
        diff = numpy.abs(val_a - val_b)
    else:
        diff = val_a - val_b

    similar = diff < eql_thres

    if type(similar).__name__ == "ndarray":
        similar = similar.all()

    return similar


def str_to_float(str_val: str, err_val: float = None) -> float:
    """
    A function which converts a string to a float. If the string cannot be
    converted to a float and an error value is provided that that will be
    returned otherwise an exception will be thrown.

    :param str_val: the string to be converted to a float.
    :param err_val: the error value to be returned if the string cannot be converted
                    to a float
    :return: floating point value

    """
    str_val = str(str_val).strip()
    try:
        out_float = float(str_val)
    except ValueError:
        if not err_val is None:
            out_float = float(err_val)
        else:
            raise rsgislib.RSGISPyException(
                f"Could not convert string to float: '{str_val}'."
            )
    return out_float


def str_to_int(str_val: str, err_val: int = None) -> int:
    """
    A function which converts a string to a int. If the string cannot be
    converted to a int then it will be attempted to be converted to a float and
    then rounded to an int. If that is not possble and an error value is provided
    that that will be returned otherwise an exception will be thrown.

    :param str_val: the string to be converted to a int.
    :param err_val: the error value to be returned if the string cannot be converted
                    to a int
    :return: integer value

    """
    str_val = str(str_val).strip()
    try:
        out_int = int(str_val)
    except ValueError:
        try:
            tmp_float = float(str_val)
            out_int = int(math.floor(tmp_float + 0.5))
        except ValueError:
            if not err_val is None:
                out_int = int(err_val)
            else:
                raise rsgislib.RSGISPyException(
                    f"Could not convert string to int: '{str_val}'."
                )
    return out_int


def zero_pad_num_str(
    num_val: float,
    str_len: int = 3,
    round_num: bool = False,
    round_n_digts: int = 0,
    integerise: bool = False,
    absolute: bool = False,
    gain: float = 1,
) -> str:
    """
    A function which zero pads a number to make a string

    :param num_val: number value to be processed.
    :param str_len: the number of characters in the output string.
    :param round_num: boolean whether to round the input number value.
    :param round_n_digts: If rounding, the number of digits following decimal
                          points to round to.
    :param integerise: boolean whether to integerise the input number
    :param absolute: make number positive.
    :param gain: apply a gain before integerising.
    :return: string with the padded numeric value.

    """
    if absolute:
        num_val = abs(num_val)
    if round_num:
        num_val = round(num_val, round_n_digts)
    if integerise:
        num_val = int(num_val * gain)

    num_str = "{}".format(num_val)
    num_str = num_str.zfill(str_len)
    return num_str


def powerset_iter(in_set: List):
    """
    A function which returns an iterator (generator) for all the subsets
    of the inputted set (i.e., the powerset)

    :params inset: the input set for which the powerset will be produced

    """
    if len(in_set) <= 1:
        yield in_set
        yield []
    else:
        for item in powerset_iter(in_set[1:]):
            yield [in_set[0]] + item
            yield item


def powerset_lst(in_set: List, min_items: int = 0) -> List:
    """
    A function which returns a list for all the subsets
    of the inputted set (i.e., the powerset)

    :params inset: the input set for which the powerset will be produced
    :params min_items: Optional parameter specifying the minimum number
                       of items in the output sets. If 0 or below then
                       ignored. Default is 0.

    """
    out_pset = []
    for subset in powerset_iter(in_set):
        if min_items > 0:
            if len(subset) >= min_items:
                out_pset.append(subset)
        else:
            out_pset.append(subset)
    return out_pset


def get_environment_variable(var: str) -> str:
    """
    A function to get an environmental variable, if variable is not present
    returns None.

    :param var: the name of the environmental variable to get the value for.
    :return: value of env var.

    """
    out_var = None
    try:
        out_var = os.environ[var]
    except Exception:
        out_var = None
    return out_var


def num_process_cores() -> int:
    """
    A functions which returns the number of processing cores available on the machine

    :return: int

    """
    import multiprocessing

    return multiprocessing.cpu_count()


def read_text_file(input_file: str) -> str:
    """
    Read a text file into a single string.

    :param input_file: File path to the input file.
    :return: string

    """
    txt_str = ""
    try:
        data_file = open(input_file, "r")
        for line in data_file:
            txt_str += line
        data_file.close()
    except Exception as e:
        raise e
    return txt_str


def read_text_file_no_new_lines(input_file: str) -> str:
    """
    Read a text file into a single string removing new lines.

    :param input_file: File path to the input file.
    :return: string

    """
    txt_str = ""
    try:
        data_file = open(input_file, "r")
        for line in data_file:
            txt_str += line.strip()
        data_file.close()
    except Exception as e:
        raise e
    return txt_str


def read_text_file_to_list(input_file: str, include_empty_lines: bool = False) -> List:
    """
    Read a text file into a list where each line is an element in the list.

    :param input_file: File path to the input file.
    :param include_empty_lines: Include empty lines within the line (Default: False)
    :return: list

    """
    out_list = []
    try:
        data_file = open(input_file, "r")
        for line in data_file:
            line = line.strip()
            if include_empty_lines or (line != ""):
                out_list.append(line)
        data_file.close()
    except Exception as e:
        raise e
    return out_list


def write_list_to_file(data_lst: List, out_file: str):
    """
    Write a list a text file, one line per item.

    :param data_lst: List of values to be written to the output file.
    :param out_file: File path to the output file.

    """
    try:
        f = open(out_file, "w")
        for item in data_lst:
            f.write(str(item) + "\n")
        f.flush()
        f.close()
    except Exception as e:
        raise e


def write_data_to_file(data_val, out_file: str):
    """
    Write some data (a string or can be converted to a string using str(data_val) to
    an output text file.

    :param data_val: Data to be written to the output file.
    :param out_file: File path to the output file.

    """
    try:
        f = open(out_file, "w")
        f.write(str(data_val) + "\n")
        f.flush()
        f.close()
    except Exception as e:
        raise e


def write_dict_to_json(data_dict: Dict, out_file: str):
    """
    Write some data to a JSON file. The data would commonly be structured as a dict
    but could also be a list.

    :param data_dict: The dict (or list) to be written to the output JSON file.
    :param out_file: The file path to the output file.

    """
    with open(out_file, "w") as fp:
        json.dump(
            data_dict,
            fp,
            cls=RSGISNumpyArrayEncoder,
            sort_keys=True,
            indent=4,
            separators=(",", ": "),
            ensure_ascii=False,
        )


def write_dict_to_json_gz(data_dict: Dict, out_file: str, encoding: str = "utf-8"):
    """
    Write some data to a JSON file compressed with gzip. The data would commonly be
    structured as a dict but could also be a list.

    :param data_dict: The dict (or list) to be written to the output JSON file.
    :param out_file: The file path to the output file.
    :param encoding: the character set encoding (default: utf-8)

    """
    import gzip

    json_str = json.dumps(
        data_dict,
        cls=RSGISNumpyArrayEncoder,
        sort_keys=True,
        indent=4,
        separators=(",", ": "),
        ensure_ascii=False,
    )
    json_bytes = json_str.encode(encoding)

    with gzip.GzipFile(out_file, "w") as fout:
        fout.write(json_bytes)


def read_json_to_dict(input_file: str) -> Dict:
    """
    Read a JSON file. Will return a list or dict.

    :param input_file: input JSON file path.

    """
    with open(input_file) as f:
        data = json.load(f)
    return data


def read_gz_json_to_dict(input_file: str, encoding: str = "utf-8") -> Dict:
    """
    Read a JSON file. Will return a list or dict.

    :param input_file: input JSON file path.
    :param encoding: the character set encoding (default: utf-8)

    """
    import gzip

    with gzip.GzipFile(input_file, "r") as f_in:
        json_bytes = f_in.read()

    json_str = json_bytes.decode(encoding)
    data = json.loads(json_str)
    return data


def create_var_list(in_vals_lsts: Dict, val_dict: Dict = None) -> List[Dict]:
    """
    A function which will produce a list of dictionaries with all the combinations
    of the input variables listed (i.e., the powerset).

    :param in_vals_lsts: dictionary with each value having a list of values.
    :param val_dict: variable used in iterative nature of function which lists
                     the variable for which are still to be looped through. Would
                     normally not be provided by the user as default is None. Be
                     careful if you set as otherwise you might not get a correct
                     or valid result.
    :returns: list of dictionaries with the same keys are the input but only a
              single value will be associate with key rather than a list.

    .. code:: python

        import rsgislib.tools.utils
        seg_vars_ranges = dict()
        seg_vars_ranges['k'] = [5, 10, 20, 30, 40, 50, 60, 80, 100, 120]
        seg_vars_ranges['d'] = [10, 20, 50, 100, 200, 1000, 10000]
        seg_vars_ranges['minsize'] = [5, 10, 20, 50, 100, 200]
        seg_vars = rsgislib.tools.utils.create_var_list(seg_vars_ranges)

    """
    out_vars = []
    if (in_vals_lsts is None) and (val_dict is not None):
        out_val_dict = dict()
        for key in val_dict.keys():
            out_val_dict[key] = val_dict[key]
        out_vars.append(out_val_dict)
    elif in_vals_lsts is not None:
        if len(in_vals_lsts.keys()) > 0:
            key = list(in_vals_lsts.keys())[0]
            vals_arr = in_vals_lsts[key]
            next_vals_lsts = dict()
            for ckey in in_vals_lsts.keys():
                if ckey != key:
                    next_vals_lsts[ckey] = in_vals_lsts[ckey]

            if len(next_vals_lsts.keys()) == 0:
                next_vals_lsts = None

            if val_dict is None:
                val_dict = dict()

            for val in vals_arr:
                c_val_dict = dict()
                for ckey in val_dict.keys():
                    c_val_dict[ckey] = val_dict[ckey]
                c_val_dict[key] = val
                c_out_vars = create_var_list(next_vals_lsts, c_val_dict)
                out_vars = out_vars + c_out_vars
    return out_vars


def in_bounds(
    x: numpy.array, lower: float, upper: float, upper_strict: bool = False
) -> bool:
    """
    Checks whether a value or array of values is within specified bounds.

    :param x: value or numpy array of values to check.
    :param lower: lower bound
    :param upper: upper bound
    :param upper_strict: True is less than upper; False is less than equal to upper
    :return: boolean

    """
    if upper_strict:
        return lower <= numpy.min(x) and numpy.max(x) < upper
    else:
        return lower <= numpy.min(x) and numpy.max(x) <= upper


def mixed_signs(x: numpy.array) -> bool:
    """
    Check whether an array of numbers has a mix of positive and negative values.

    :param x: list of values.
    :return: boolean

    """
    return numpy.min(x) < 0 and numpy.max(x) >= 0


def negative(x: numpy.array) -> bool:
    """
    Is the maximum number in the list negative.
    :param x: list of values

    :return: boolean

    """
    return numpy.max(x) < 0


def is_odd(number: float) -> bool:
    """
    A function which tests whether a number is odd

    :param number: number value to test.
    :return: True = input number is odd; False = input number is even

    """
    if (number % 2) != 0:
        return True
    return False


def hex_to_rgb(hex_str: str) -> (int, int, int):
    """
    A function which converts an hexadecimal colour representation to RGB values
    between 0 and 255.

    For example: #b432be is equal to: 180, 50, 190

    :param hex_str: Input hex string which can be either 7 or 6 characters long.
                    If 7 characters then the first character will be a #.
    :return: R, G, B tuple

    .. code:: python

        import rsgislib.tools.utils
        r, g, b = rsgislib.tools.utils.hex_to_rgb("#b432be")

    """
    if hex_str[0] == "#":
        hex_str = hex_str[1:]
    if len(hex_str) != 6:
        raise rsgislib.RSGISPyException(
            "String must be of length 6 or 7 if starting with #"
        )

    r_hex = hex_str[0:2]
    g_hex = hex_str[2:4]
    b_hex = hex_str[4:6]
    return int(r_hex, 16), int(g_hex, 16), int(b_hex, 16)


def rgb_to_hex(
    r: Union[int, float],
    g: Union[int, float],
    b: Union[int, float],
    normalised: bool = False,
) -> str:
    """
    A function which converts red, green, blue values to a hexadecimal colour
    representation.

    For example: 180, 50, 190 is equal to: #b432be

    :param r: number with range either 0-255 or 0-1 if normalised
    :param g: number with range either 0-255 or 0-1 if normalised
    :param b: number with range either 0-255 or 0-1 if normalised
    :param normalised: a boolean specifying the inputs are in range 0-1
    :return: string with hexadecimal colour representation
    """
    if normalised:
        r = int(r * 255)
        g = int(g * 255)
        b = int(b * 255)

    if (r < 0) or (r > 255):
        raise rsgislib.RSGISPyException(
            "Red value must be between 0-255 or 0-1 if normalised"
        )
    if (g < 0) or (g > 255):
        raise rsgislib.RSGISPyException(
            "Green value must be between 0-255 or 0-1 if normalised"
        )
    if (b < 0) or (b > 255):
        raise rsgislib.RSGISPyException(
            "Blue value must be between 0-255 or 0-1 if normalised"
        )

    return "#{:02x}{:02x}{:02x}".format(r, g, b)


def remove_repeated_chars(str_val: str, repeat_char: str) -> str:
    """
    A function which removes repeated characters within a string for the
    specified character

    :param str_val: The input string.
    :param repeat_char: The character
    :return: string without repeat_char

    """
    if len(repeat_char) != 1:
        raise rsgislib.RSGISPyException("The repeat character has multiple characters.")
    out_str = ""
    p = ""
    for c in str_val:
        if c == repeat_char:
            if c != p:
                out_str += c
        else:
            out_str += c
        p = c
    return out_str


def check_str(
    str_val: str,
    rm_non_ascii: bool = False,
    rm_dashs: bool = False,
    rm_spaces: bool = False,
    rm_punc: bool = False,
) -> str:
    """
    A function which can check a string removing spaces (replaced with underscores),
    remove punctuation and any non ascii characters.

    :param str_val: the input string to be processed.
    :param rm_non_ascii: If True (default False) remove any non-ascii characters
                         from the string
    :param rm_dashs: If True (default False) remove any dashes from the string
                     and replace with underscores.
    :param rm_spaces: If True (default False) remove any spaces from the string.
    :param rm_punc: If True (default False) remove any punctuation
                    (other than '_' or '-') from the string.
    :return: returns a string outputted from the processing.

    """
    if str_val is None:
        str_val = ""
    str_val_tmp = str_val.strip()

    if rm_non_ascii:
        str_val_tmp_ascii = ""
        for c in str_val_tmp:
            if (
                (c in string.ascii_letters)
                or (c in string.punctuation)
                or (c in string.digits)
                or (c == " ")
            ):
                str_val_tmp_ascii += c
        str_val_tmp = str_val_tmp_ascii

    if rm_dashs:
        str_val_tmp = str_val_tmp.replace("-", "_")
        str_val_tmp = remove_repeated_chars(str_val_tmp, "_")

    if rm_spaces:
        str_val_tmp = str_val_tmp.replace(" ", "_")
        str_val_tmp = remove_repeated_chars(str_val_tmp, "_")

    if rm_punc:
        for punct in string.punctuation:
            if (punct != "_") and (punct != "-"):
                str_val_tmp = str_val_tmp.replace(punct, "")
        str_val_tmp = remove_repeated_chars(str_val_tmp, "_")

    return str_val_tmp


def create_ascii_text_file(input_file: str, output_file: str):
    """
    A function which will ensure that an input text file will
    only have ascii characters. Non-ascii characters will be
    removed.

    :param input_file: input file path
    :param output_file: output file path

    """
    txt_lst = read_text_file_to_list(input_file, True)
    out_txt_lst = list()
    for line in txt_lst:
        out_line = check_str(line, rm_non_ascii=True)
        out_txt_lst.append(out_line)

    write_list_to_file(out_txt_lst, output_file)


def get_days_since(year: int, day_of_year: int, base_date: datetime.date) -> int:
    """
    Calculate the number of days from a base data to a defined year/day.

    :param year: int with year XXXX (e.g., 2020)
    :param day_of_year: int with the day within the year (1-365)
    :param base_date: a datetime.date object
    :return: int (n days)

    """
    if year < base_date.year:
        raise rsgislib.RSGISPyException("The year specified is before the base date.")
    date_val = datetime.date(year=int(year), month=1, day=1)
    date_val = date_val + datetime.timedelta(days=int(day_of_year - 1))
    return (date_val - base_date).days


def get_days_since_date(year: int, month: int, day, base_date: datetime.date) -> int:
    """
    Calculate the number of days from a base data to a defined year/day.

    :param year: int with year XXXX (e.g., 2020)
    :param month: int month in year (1-12) (e.g., 6)
    :param day: int with the day within the month (1-31) (e.g., 20)
    :param base_date: a datetime.date object
    :return: int (n days)

    """
    if year < base_date.year:
        raise rsgislib.RSGISPyException("The year specified is before the base date.")
    date_val = datetime.date(year=int(year), month=int(month), day=int(day))
    return (date_val - base_date).days


def find_month_end_date(year: int, month: int) -> int:
    """
    A function which returns the date of the last day of the month.

    :param year: int for the year (e.g., 2019)
    :param month: month (e.g., 9)
    :return: last day of the month date

    """
    import calendar

    cal = calendar.Calendar()
    month_days = cal.monthdayscalendar(year, month)
    max_day_month = numpy.array(month_days).flatten().max()
    return max_day_month


def create_year_month_start_end_lst(
    start_year: int, start_month: int, end_year: int, end_month: int
) -> List[Tuple[int, int]]:
    """
    A function which creates a list of year and month tuples from a start
    and end month and year.

    :param start_year: int with the start year
    :param start_month: int with the start month
    :param end_year: int with the end year
    :param end_month: int with the end month
    :return: List of tuples (year, month)

    """
    out_year_mnt_lst = list()
    years = numpy.arange(start_year, end_year + 1, 1)
    for year in years:
        if (year == start_year) and (year == end_year):
            months = numpy.arange(start_month, end_month + 1, 1)
        elif year == start_year:
            months = numpy.arange(start_month, 13, 1)
        elif year == end_year:
            months = numpy.arange(1, end_month + 1, 1)
        else:
            months = numpy.arange(1, 13, 1)
        for month in months:
            out_year_mnt_lst.append((year, month))

    return out_year_mnt_lst


def create_year_month_n_months_lst(
    start_year: int, start_month: int, n_months: int
) -> List[Tuple[int, int]]:
    """
    A function which creates a list of year and month tuples from a start
    and end month and year.

    :param start_year: int with the start year
    :param start_month: int with the start month
    :param n_months: int with the number of months ahead
    :return: List of tuples (year, month)

    """
    if start_year < 0:
        raise rsgislib.RSGISPyException("Year must be positive")
    if (start_month < 1) or (start_month > 12):
        raise rsgislib.RSGISPyException("Month must be between 1-12")

    out_year_mnt_lst = list()
    out_year_mnt_lst.append((start_year, start_month))
    months = numpy.arange(0, n_months, 1)
    months = months + start_month

    month_vals = months % 12
    year = start_year
    first = True
    for month in month_vals:
        if first:
            out_year_mnt_lst.append((year, month + 1))
            first = False
        else:
            if month == 0:
                year += 1
            out_year_mnt_lst.append((year, month + 1))

    return out_year_mnt_lst


def is_summer_winter(lat: float, date: datetime.date) -> int:
    """
    A function which returns an integer specifying whether the date provided
    is within the summer or winter for a given latitude.

    Southern Hemisphere Winter: May - November
    Northern Hemisphere Winter: March - October

    :param lat: float for the latitude
    :param date: a datetime.date object
    :return: int (1 == summer; 2 == winter)

    """
    summer_winter = 0
    if lat < 0:
        # Southern Hemisphere
        if (date.month > 4) & (date.month < 11):
            summer_winter = 2  # Winter
        else:
            summer_winter = 1  # Summer
    else:
        # Northern Hemisphere
        if (date.month > 3) & (date.month < 10):
            summer_winter = 1  # Summer
        else:
            summer_winter = 2  # Winter
    return summer_winter


def dict_struct_does_path_exist(dict_struct_obj: Dict, tree_sequence: List) -> bool:
    """
    A function which tests whether a path exists within JSON file.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :return: boolean

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    pathExists = True
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            pathExists = False
            break
    return pathExists


def dict_struct_get_str_value(
    dict_struct_obj: Dict, tree_sequence: List, valid_values: List = None
) -> str:
    """
    A function which retrieves a single string value from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :param valid_values: An optional list of valid values. An exception will be
                         thrown if a list is provided and the string value found
                         is not within the list.
    :return: the string value at the path provided.

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))
    if valid_values is not None:
        if curr_dict_struct_obj not in valid_values:
            raise rsgislib.RSGISPyException(
                "'{}' is not within the list of valid values.".format(
                    curr_dict_struct_obj
                )
            )
    return curr_dict_struct_obj


def dict_struct_get_boolean_value(dict_struct_obj: Dict, tree_sequence: List) -> bool:
    """
    A function which retrieves a single boolean value from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :return: boolean value.

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))
    if type(curr_dict_struct_obj).__name__ == "bool":
        rtn_bool = curr_dict_struct_obj
    else:
        raise rsgislib.RSGISPyException(
            "'{}' is not 'True' or 'False'.".format(curr_dict_struct_obj)
        )
    return rtn_bool


def dict_struct_get_date_value(
    dict_struct_obj: Dict, tree_sequence: List, date_format: str = "%Y-%m-%d"
) -> datetime.datetime:
    """
    A function which retrieves a single date value from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :param date_format: a string or list of strings for the date/time format
                             to be parsed by datetime.datetime.strptime.
    :return: a date object with the value at the path specified.

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))

    if type(date_format) is str:
        try:
            out_date_obj = datetime.datetime.strptime(
                curr_dict_struct_obj, date_format
            ).date()
        except Exception as e:
            raise rsgislib.RSGISPyException(e)
    elif type(date_format) is list:
        found = False
        except_obj = None
        for date_format_str in date_format:
            try:
                out_date_obj = datetime.datetime.strptime(
                    curr_dict_struct_obj, date_format_str
                ).date()
                found = True
                break
            except Exception as e:
                except_obj = e
        if not found:
            raise rsgislib.RSGISPyException(except_obj)
    else:
        raise rsgislib.RSGISPyException(
            "Do not know what the type is of date_format variable."
        )

    return out_date_obj


def dict_struct_get_datetime_value(
    dict_struct_obj: Dict,
    tree_sequence: List,
    date_time_format: str = "%Y-%m-%dT%H:%M:%S.%f",
):
    """
    A function which retrieves a single date value from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :param date_time_format: a string or list of strings for the date/time format
                             to be parsed by datetime.datetime.strptime.
    :return: datetime object with the value at the path provided.
    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))

    curr_dict_struct_obj = curr_dict_struct_obj.replace("Z", "")
    if type(date_time_format) is str:
        try:
            out_datetime_obj = datetime.datetime.strptime(
                curr_dict_struct_obj, date_time_format
            )
        except Exception as e:
            raise rsgislib.RSGISPyException(e)
    elif type(date_time_format) is list:
        found = False
        except_obj = None
        for date_time_format_str in date_time_format:
            try:
                out_datetime_obj = datetime.datetime.strptime(
                    curr_dict_struct_obj, date_time_format_str
                )
                found = True
                break
            except Exception as e:
                except_obj = e
        if not found:
            raise rsgislib.RSGISPyException(except_obj)
    else:
        raise rsgislib.RSGISPyException(
            "Do not know what the type is of date_time_format variable."
        )

    return out_datetime_obj


def dict_struct_get_str_list_value(
    dict_struct_obj: Dict, tree_sequence: List, valid_values: List = None
) -> List[str]:
    """
    A function which retrieves a list of string values from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :param valid_values: An optional list of valid values. An exception will be
                         thrown if a list is provided and a string value found
                         is not within the list.
    :return: list of strings

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))

    if type(curr_dict_struct_obj).__name__ != "list":
        raise rsgislib.RSGISPyException("Retrieved value is not a list.")
    if valid_values is not None:
        for val in curr_dict_struct_obj:
            if type(val).__name__ != "str":
                raise rsgislib.RSGISPyException(
                    "'{}' is not of type string.".format(val)
                )
            if val not in valid_values:
                raise rsgislib.RSGISPyException(
                    "'{}' is not within the list of valid values.".format(val)
                )
    return curr_dict_struct_obj


def dict_struct_get_numeric_value(
    dict_struct_obj: Dict,
    tree_sequence: List,
    valid_lower: float = None,
    valid_upper: float = None,
) -> Union[int, float]:
    """
    A function which retrieves a single numeric value from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings specifying the path within the structure
    :param valid_lower: optional lower valid bounds, error throw if outside the bounds.
    :param valid_upper: optional upper valid bounds, error throw if outside the bounds.
    :return: numeric value at path

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))

    out_value = 0.0
    if is_number(curr_dict_struct_obj):
        if "int" in type(curr_dict_struct_obj).__name__:
            out_value = int(curr_dict_struct_obj)
        elif "float" in type(curr_dict_struct_obj).__name__:
            out_value = float(curr_dict_struct_obj)
        else:
            out_value = float(curr_dict_struct_obj)
    else:
        raise rsgislib.RSGISPyException(
            "The identified value is not numeric '{}'".format(steps_str)
        )

    if valid_lower is not None:
        if out_value < valid_lower:
            raise rsgislib.RSGISPyException(
                "'{}' is less than the defined valid range.".format(out_value)
            )
    if valid_upper is not None:
        if out_value > valid_upper:
            raise rsgislib.RSGISPyException(
                "'{}' is higher than the defined valid range.".format(out_value)
            )
    return out_value


def dict_struct_get_list_value(dict_struct_obj: Dict, tree_sequence: List) -> List:
    """
    A function which retrieves a list of values from a JSON structure.

    :param dict_struct_obj: the structure of dicts (i.e., JSON) to be searched
    :param tree_sequence: list of strings  specifying the path within the structure
    :return: list of values

    """
    curr_dict_struct_obj = dict_struct_obj
    steps_str = ""
    for tree_step in tree_sequence:
        steps_str = steps_str + ":" + tree_step
        if tree_step in curr_dict_struct_obj:
            curr_dict_struct_obj = curr_dict_struct_obj[tree_step]
        else:
            raise rsgislib.RSGISPyException("Could not find '{}'".format(steps_str))

    if type(curr_dict_struct_obj).__name__ != "list":
        raise rsgislib.RSGISPyException("Retrieved value is not a list.")
    return curr_dict_struct_obj


def encode_base64_text(input_txt: str) -> str:
    """
    A function which encoded the input text using base64 into bytes.
    Useful for storing passwords in configure files, which while not
    secure means they cannot just be read as plain text.

    :param plain_txt: The input string to be encoded
    :return: Output encoded string.

    """
    import base64

    encoded_txt = base64.b64encode(input_txt.encode()).decode()
    return encoded_txt


def decode_base64_text(in_encoded_txt: str) -> str:
    """
    A function which decoded text encoded using the encode_base64_text function.

    :param in_encoded_txt: Input encoded byte string (i.e., encoded using
                           the encode_base64_text function).
    :return: Output plain text string

    """
    import base64

    output_txt = base64.b64decode(in_encoded_txt.encode()).decode()
    return output_txt


def create_username_password_file(username: str, password: str, out_file: str):
    """
    A function which will create a username/password file where the
    username and password are encoded using base64 so the information
    is not stored in plain text - note this is note 'secure' just means
    that someone cannot just read the plain text username and password.

    :param username: string with the username
    :param password: string with the password
    :param out_file: output file path.

    """
    out_data = []
    out_data.append(encode_base64_text(username))
    out_data.append(encode_base64_text(password))
    write_list_to_file(out_data, out_file)


def get_username_password(input_file: str) -> (str, str):
    """
    A function which retrieves a username and password from an input file
    where the data has been encoded with base64.

    :param input_file: input file path.
    :return: plain text username and password

    """
    in_data_lst = read_text_file_to_list(input_file)
    if len(in_data_lst) != 2:
        raise rsgislib.RSGISPyException(
            "There should just be two lines in username/password file."
        )
    username = decode_base64_text(in_data_lst[0])
    password = decode_base64_text(in_data_lst[1])
    return username, password


def prettify_xml_file(input_file: str, output_file: str):
    """
     A function which prettifies an XML file - i.e.,
     inserts new lines and tabs.

    :param input_file: The input file path.
    :param output_file: The output file path.

    """
    import xml.etree.ElementTree as ET

    xml_str = read_text_file_no_new_lines(input_file)

    xml_elements = ET.XML(xml_str)
    ET.indent(xml_elements)
    xml_str = ET.tostring(xml_elements, encoding="unicode")

    write_data_to_file(xml_str, output_file)
