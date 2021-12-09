#!/usr/bin/env python
"""
The tools.utils module contains some useful tools
"""

import datetime
import os
import string

import numpy
import rsgislib


def uid_generator(size: int = 6):
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


def is_number(str_val: str):
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


def zero_pad_num_str(
    num_val: float,
    str_len: int = 3,
    round_num: bool = False,
    round_n_digts: int = 0,
    integerise: bool = False,
    absolute: bool = False,
    gain: float = 1,
):
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


def powerset_iter(in_set: list):
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


def powerset_lst(in_set: list, min_items: int = 0):
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


def get_environment_variable(var: str):
    """
    A function to get an environmental variable, if variable is not present
    returns None.

    :param var: the name of the environmental variable to get the value for.
    :return: value of env var.

    """
    outVar = None
    try:
        outVar = os.environ[var]
    except Exception:
        outVar = None
    return outVar


def num_process_cores():
    """
    A functions which returns the number of processing cores available on the machine

    :return: int

    """
    import multiprocessing

    return multiprocessing.cpu_count()


def read_text_file_no_new_lines(input_file: str):
    """
    Read a text file into a single string removing new lines.

    :param input_file: File path to the input file.
    :return: string

    """
    txtStr = ""
    try:
        dataFile = open(input_file, "r")
        for line in dataFile:
            txtStr += line.strip()
        dataFile.close()
    except Exception as e:
        raise e
    return txtStr


def read_text_file_to_list(input_file: str):
    """
    Read a text file into a list where each line is an element in the list.

    :param input_file: File path to the input file.
    :return: list

    """
    outList = []
    try:
        dataFile = open(input_file, "r")
        for line in dataFile:
            line = line.strip()
            if line != "":
                outList.append(line)
        dataFile.close()
    except Exception as e:
        raise e
    return outList


def write_list_to_file(data_lst: list, out_file: str):
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


def write_dict_to_json(data_dict: dict, out_file: str):
    """
    Write some data to a JSON file. The data would commonly be structured as a dict
    but could also be a list.

    :param data_dict: The dict (or list) to be written to the output JSON file.
    :param out_file: The file path to the output file.

    """
    import json

    with open(out_file, "w") as fp:
        json.dump(
            data_dict,
            fp,
            sort_keys=True,
            indent=4,
            separators=(",", ": "),
            ensure_ascii=False,
        )


def write_dict_to_json_gz(data_dict: dict, out_file: str, encoding: str = "utf-8"):
    """
    Write some data to a JSON file compressed with gzip. The data would commonly be
    structured as a dict but could also be a list.

    :param data_dict: The dict (or list) to be written to the output JSON file.
    :param out_file: The file path to the output file.
    :param encoding: the character set encoding (default: utf-8)

    """
    import json
    import gzip

    json_str = json.dump(
        data_dict, sort_keys=True, indent=4, separators=(",", ": "), ensure_ascii=False
    )
    json_bytes = json_str.encode(encoding)

    with gzip.GzipFile(out_file, "w") as fout:
        fout.write(json_bytes)


def read_json_to_dict(input_file: str):
    """
    Read a JSON file. Will return a list or dict.

    :param input_file: input JSON file path.

    """
    import json

    with open(input_file) as f:
        data = json.load(f)
    return data


def read_gz_json_to_dict(input_file: str, encoding: str = "utf-8"):
    """
    Read a JSON file. Will return a list or dict.

    :param input_file: input JSON file path.
    :param encoding: the character set encoding (default: utf-8)

    """
    import json
    import gzip

    with gzip.GzipFile(input_file, "r") as fin:
        json_bytes = fin.read()

    json_str = json_bytes.decode(encoding)
    data = json.loads(json_str)
    return data


def create_var_list(in_vals_lsts: dict, val_dict: dict = None):
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

    Example:

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


def in_bounds(x: numpy.array, lower: float, upper: float, upper_strict: bool = False):
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


def mixed_signs(x: numpy.array):
    """
    Check whether an array of numbers has a mix of positive and negative values.

    :param x: list of values.
    :return: boolean

    """
    return numpy.min(x) < 0 and numpy.max(x) >= 0


def negative(x: numpy.array):
    """
    Is the maximum number in the list negative.
    :param x: list of values

    :return: boolean

    """
    return numpy.max(x) < 0


def is_odd(number: float):
    """
    A function which tests whether a number is odd

    :param number: number value to test.
    :return: True = input number is odd; False = input number is even

    """
    if (number % 2) != 0:
        return True
    return False


def hex_to_rgb(hex_str: str):
    """
    A function which converts an hexadecimal colour representation to RGB values
    between 0 and 255.

    For example: #b432be is equal to: 180, 50, 190

    :param hex_str: Input hex string which can be either 7 or 6 characters long.
                    If 7 characters then the first character will be a #.
    :return: R, G, B tuple

    Example:

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


def remove_repeated_chars(str_val: str, repeat_char: str):
    """
    A function which removes repeated characters within a string for the
    specified character

    :param str_val: The input string.
    :param repeat_char: The character
    :return: string without repeat_char

    """
    if len(repeat_char) != 1:
        raise Exception("The repeat character has multiple characters.")
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
):
    """
    A function which can check a string removing spaces (replaced with underscores),
    remove punctuation and any non ascii characters.

    :param str_val: the input string to be processed.
    :param rm_non_ascii: If True (default False) remove any non-ascii characters
                         from the string
    :param rm_dashs: If True (default False) remove any dashs from the string
                     and replace with underscores.
    :param rm_spaces: If True (default False) remove any spaces from the string.
    :param rm_punc: If True (default False) remove any punctuation
                    (other than '_' or '-') from the string.
    :return: returns a string outputted from the processing.

    """
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


def get_days_since(year: int, day_of_year: int, base_date: datetime.date):
    """
    Calculate the number of days from a base data to a defined year/day.

    :param year: int with year XXXX (e.g., 2020)
    :param day_of_year: int with the day within the year (1-365)
    :param base_date: a datetime.date object
    :return: int (n days)

    """
    if year < base_date.year:
        raise Exception("The year specified is before the base date.")
    date_val = datetime.date(year=int(year), month=1, day=1)
    date_val = date_val + datetime.timedelta(days=int(day_of_year - 1))
    return (date_val - base_date).days


def get_days_since_date(year: int, month: int, day, base_date: datetime.date):
    """
    Calculate the number of days from a base data to a defined year/day.

    :param year: int with year XXXX (e.g., 2020)
    :param month: int month in year (1-12) (e.g., 6)
    :param day: int with the day within the month (1-31) (e.g., 20)
    :param base_date: a datetime.date object
    :return: int (n days)

    """
    if year < base_date.year:
        raise Exception("The year specified is before the base date.")
    date_val = datetime.date(year=int(year), month=int(month), day=int(day))
    return (date_val - base_date).days


def dict_struct_does_path_exist(dict_struct_obj: dict, tree_sequence: list):
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
    dict_struct_obj: dict, tree_sequence: list, valid_values: list = None
):
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


def dict_struct_get_boolean_value(dict_struct_obj: dict, tree_sequence: list):
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
    dict_struct_obj: dict, tree_sequence: list, date_format: str = "%Y-%m-%d"
):
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
    dict_struct_obj: dict,
    tree_sequence: list,
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
    dict_struct_obj: dict, tree_sequence: list, valid_values: list = None
):
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
    dict_struct_obj: dict,
    tree_sequence: list,
    valid_lower: float = None,
    valid_upper: float = None,
):
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
    if (type(curr_dict_struct_obj).__name__ == "int") or (
        type(curr_dict_struct_obj).__name__ == "float"
    ):
        out_value = curr_dict_struct_obj
    elif type(curr_dict_struct_obj).__name__ == "str":
        if curr_dict_struct_obj.isnumeric():
            out_value = float(curr_dict_struct_obj)
        else:
            try:
                out_value = float(curr_dict_struct_obj)
            except:
                raise rsgislib.RSGISPyException(
                    "The identified value is not numeric '{}'".format(steps_str)
                )
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


def dict_struct_get_list_value(dict_struct_obj: dict, tree_sequence: list):
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
