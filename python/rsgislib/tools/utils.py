#!/usr/bin/env python
"""
The tools.utils module contains some useful tools
"""


def uidGenerator(size=6):
    """
    A function which will generate a 'random' string of the specified length based on the UUID

    :param size: the length of the returned string.
    :return: string of length size.

    """
    import uuid
    randomStr = str(uuid.uuid4())
    randomStr = randomStr.replace("-", "")
    return randomStr[0:size]


def isNumber(str_val):
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


def zero_pad_num_str(num_val, str_len=3, round_num=False, round_n_digts=0, integerise=False):
    """
    A function which zero pads a number to make a string

    :param num_val: number value to be processed.
    :param str_len: the number of characters in the output string.
    :param round_num: boolean whether to round the input number value.
    :param round_n_digts: If rounding, the number of digits following decimal points to round to.
    :param integerise: boolean whether to integerise the input number
    :return: string with the padded numeric value.

    """
    if round_num:
        num_val = round(num_val, round_n_digts)
    if integerise:
        num_val = int(num_val)

    num_str = "{}".format(num_val)
    num_str = num_str.zfill(str_len)
    return num_str


def powerset_iter(in_set):
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


def powerset_lst(in_set, min_items=0):
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


def getEnvironmentVariable(var):
    """
    A function to get an environmental variable, if variable is not present returns None.

    :return: value of env var.

    """
    import os
    outVar = None
    try:
        outVar = os.environ[var]
    except Exception:
        outVar = None
    return outVar


def numProcessCores():
    """
    A functions which returns the number of processing cores available on the machine

    :return: int

    """
    import multiprocessing
    return multiprocessing.cpu_count()


def readTextFileNoNewLines(input_file):
    """
    Read a text file into a single string
    removing new lines.

    :param input_file: File path to the input file.
    :return: string

    """
    txtStr = ""
    try:
        dataFile = open(input_file, 'r')
        for line in dataFile:
            txtStr += line.strip()
        dataFile.close()
    except Exception as e:
        raise e
    return txtStr


def readTextFile2List(input_file):
    """
    Read a text file into a list where each line
    is an element in the list.

    :param input_file: File path to the input file.
    :return: list

    """
    outList = []
    try:
        dataFile = open(input_file, 'r')
        for line in dataFile:
            line = line.strip()
            if line != "":
                outList.append(line)
        dataFile.close()
    except Exception as e:
        raise e
    return outList


def writeList2File(data_lst, out_file):
    """
    Write a list a text file, one line per item.

    :param data_lst: List of values to be written to the output file.
    :param out_file: File path to the output file.

    """
    try:
        f = open(out_file, 'w')
        for item in data_lst:
            f.write(str(item) + '\n')
        f.flush()
        f.close()
    except Exception as e:
        raise e


def writeData2File(data_val, out_file):
    """
    Write some data (a string or can be converted to a string using str(data_val) to
    an output text file.

    :param data_val: Data to be written to the output file.
    :param out_file: File path to the output file.

    """
    try:
        f = open(out_file, 'w')
        f.write(str(data_val) + '\n')
        f.flush()
        f.close()
    except Exception as e:
        raise e


def writeDict2JSON(data_dict, out_file):
    """
    Write some data to a JSON file. The data would commonly be structured as a dict but could also be a list.

    :param data_dict: The dict (or list) to be written to the output JSON file.
    :param out_file: The file path to the output file.

    """
    import json
    with open(out_file, 'w') as fp:
        json.dump(data_dict, fp, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)


def readJSON2Dict(input_file):
    """
    Read a JSON file. Will return a list or dict.

    :param input_file: input JSON file path.

    """
    import json
    with open(input_file) as f:
        data = json.load(f)
    return data


def createVarList(in_vals_lsts, val_dict=None):
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

    Example::

        seg_vars_ranges = dict()
        seg_vars_ranges['k'] = [5, 10, 20, 30, 40, 50, 60, 80, 100, 120]
        seg_vars_ranges['d'] = [10, 20, 50, 100, 200, 1000, 10000]
        seg_vars_ranges['minsize'] = [5, 10, 20, 50, 100, 200]
        seg_vars = rsgis_utils.createVarList(seg_vars_ranges)

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
                c_out_vars = createVarList(next_vals_lsts, c_val_dict)
                out_vars = out_vars + c_out_vars
    return out_vars


def in_bounds(x, lower, upper, upper_strict=False):
    """
    Checks whether a value is within specified bounds.

    :param x: value or array of values to check.
    :param lower: lower bound
    :param upper: upper bound
    :param upper_strict: True is less than upper; False is less than equal to upper

    :return: boolean

    """
    import numpy
    if upper_strict:
        return lower <= numpy.min(x) and numpy.max(x) < upper
    else:
        return lower <= numpy.min(x) and numpy.max(x) <= upper


def mixed_signs(x):
    """
    Check whether a list of numbers has a mix of postive and negative values.

    :param x: list of values.

    :return: boolean

    """
    import numpy
    return numpy.min(x) < 0 and numpy.max(x) >= 0


def negative(x):
    """
    Is the maximum number in the list negative.
    :param x: list of values

    :return: boolean

    """
    import numpy
    return numpy.max(x) < 0


def isodd(number):
    """
    A function which tests whether a number is odd

    :param number: number value to test.
    :return: True = input number is odd; False = input number is even

    """
    if (number % 2) != 0:
        return True
    return False


def remove_repeated_chars(str_val, repeat_char):
    """
    A function which removes repeated characters within a string for the specified character

    :param str_val: The input string.
    :param repeat_char: The character
    :return: string without repeat_char

    """
    if len(repeat_char) != 1:
        raise Exception("The repeat character has multiple characters.")
    out_str = ''
    p = ''
    for c in str_val:
        if c == repeat_char:
            if c != p:
                out_str += c
        else:
            out_str += c
        p = c
    return out_str


def check_str(str_val, rm_non_ascii=False, rm_dashs=False, rm_spaces=False, rm_punc=False):
    """
    A function which can check a string removing spaces (replaced with underscores),
    remove punctuation and any non ascii characters.

    :param str_val: the input string to be processed.
    :param rm_non_ascii: If True (default False) remove any non-ascii characters from the string
    :param rm_dashs: If True (default False) remove any dashs from the string and replace with underscores.
    :param rm_spaces: If True (default False) remove any spaces from the string.
    :param rm_punc: If True (default False) remove any punctuation (other than '_' or '-') from the string.
    :return: returns a string outputted from the processing.

    """
    import string
    str_val_tmp = str_val.strip()

    if rm_non_ascii:
        str_val_tmp_ascii = ""
        for c in str_val_tmp:
            if (c in string.ascii_letters) or (c in string.punctuation) or (c in string.digits) or (c == ' '):
                str_val_tmp_ascii += c
        str_val_tmp = str_val_tmp_ascii

    if rm_dashs:
        str_val_tmp = str_val_tmp.replace('-', '_')
        str_val_tmp = remove_repeated_chars(str_val_tmp, '_')

    if rm_spaces:
        str_val_tmp = str_val_tmp.replace(' ', '_')
        str_val_tmp = remove_repeated_chars(str_val_tmp, '_')

    if rm_punc:
        for punct in string.punctuation:
            if (punct != '_') and (punct != '-'):
                str_val_tmp = str_val_tmp.replace(punct, '')
        str_val_tmp = remove_repeated_chars(str_val_tmp, '_')

    return str_val_tmp


def get_days_since(year, dayofyear, base_date):
    """
    Calculate the number of days from a base data to a defined year/day.

    :param year: int with year XXXX (e.g., 2020)
    :param dayofyear: int with the day within the year (1-365)
    :param base_date: a datetime
    :return: int (n days)

    """
    import datetime
    if year < base_date.year:
        raise Exception("The year specified is before the base date.")
    date_val = datetime.date(year=int(year), month=1, day=1)
    date_val = date_val + datetime.timedelta(days=int(dayofyear - 1))
    return (date_val - base_date).days


def get_days_since_date(year, month, day, base_date):
    """
    Calculate the number of days from a base data to a defined year/day.

    :param year: int with year XXXX (e.g., 2020)
    :param month: int month in year (1-12) (e.g., 6)
    :param day: int with the day within the month (1-31) (e.g., 20)
    :param base_date: a datetime
    :return: int (n days)

    """
    import datetime
    if year < base_date.year:
        raise Exception("The year specified is before the base date.")
    date_val = datetime.date(year=int(year), month=int(month), day=int(day))
    return (date_val - base_date).days


