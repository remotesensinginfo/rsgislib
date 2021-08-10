import os

import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
TOOLS_DATA_DIR = os.path.join(DATA_DIR, "tools", "utils")


def test_uidGenerator_len4():
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uidGenerator(4)
    assert len(uid_str) == 4


def test_uidGenerator_len6():
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uidGenerator(6)
    assert len(uid_str) == 6


def test_uidGenerator_len8():
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uidGenerator(8)
    assert len(uid_str) == 8


def test_isNumber_int_1():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber(1)


def test_isNumber_int_100():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber(100)


def test_isNumber_int_1str():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber("1")


def test_isNumber_int_100str():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber("100")


def test_isNumber_float_125():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber(1.25)


def test_isNumber_float_10025():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber(100.25)


def test_isNumber_float_str125():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber("1.25")


def test_isNumber_float_str10025():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isNumber("100.25")


def test_isNumber_str_Hello():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isNumber("Hello")


def test_isNumber_str_World():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isNumber("World")


def test_isNumber_str_1a():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isNumber("1a")


def test_isNumber_str_100q():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isNumber("100?")


def test_isNumber_str_1e():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isNumber("1!")


def test_isNumber_str_10at():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isNumber("10@")


def test_zero_pad_num_str_1():
    import rsgislib.tools.utils

    num_str = rsgislib.tools.utils.zero_pad_num_str(
        1, str_len=3, round_num=False, round_n_digts=0, integerise=False
    )
    assert num_str == "001"


def test_zero_pad_num_str_11_round_int():
    import rsgislib.tools.utils

    num_str = rsgislib.tools.utils.zero_pad_num_str(
        1.1, str_len=3, round_num=True, round_n_digts=0, integerise=True
    )
    assert num_str == "001"


def test_zero_pad_num_str_float_round():
    import rsgislib.tools.utils

    num_str = rsgislib.tools.utils.zero_pad_num_str(
        112.1354, str_len=8, round_num=True, round_n_digts=2, integerise=False
    )
    assert num_str == "00112.14"


def test_powerset_lst():
    import rsgislib.tools.utils

    set_vals = [1, 2, 3]
    pwr_set = rsgislib.tools.utils.powerset_lst(set_vals)
    vld_pwr_set = [[1, 2, 3], [2, 3], [1, 3], [3], [1, 2], [2], [1], []]

    correct_answer = True
    if len(pwr_set) != len(vld_pwr_set):
        correct_answer = False
    else:
        for set_val in vld_pwr_set:
            if set_val not in pwr_set:
                correct_answer = False
                break

    assert correct_answer


def test_powerset_lst_min2():
    import rsgislib.tools.utils

    set_vals = [1, 2, 3]
    pwr_set = rsgislib.tools.utils.powerset_lst(set_vals, min_items=2)
    vld_pwr_set = [[1, 2, 3], [2, 3], [1, 3], [1, 2]]

    correct_answer = True
    if len(pwr_set) != len(vld_pwr_set):
        correct_answer = False
    else:
        for set_val in vld_pwr_set:
            if set_val not in pwr_set:
                correct_answer = False
                break

    assert correct_answer


def test_readTextFileNoNewLines_str():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_DATA_DIR, "basic_str.txt")
    in_data = rsgislib.tools.utils.readTextFileNoNewLines(input_file)
    assert in_data == "Hello"


def test_readTextFileNoNewLines_str_mline():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_DATA_DIR, "basic_str_mline.txt")
    in_data = rsgislib.tools.utils.readTextFileNoNewLines(input_file)
    assert in_data == "HelloWorld"


def test_readTextFileNoNewLines_int():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_DATA_DIR, "basic_int.txt")
    in_data = rsgislib.tools.utils.readTextFileNoNewLines(input_file)
    assert int(in_data) == 100


def test_readTextFile2List_strs():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_DATA_DIR, "basic_str_mline.txt")
    in_data = rsgislib.tools.utils.readTextFile2List(input_file)
    ref_data = ["Hello", "World"]
    if len(in_data) != len(ref_data):
        assert False
    for data_val, ref_val in zip(in_data, ref_data):
        if data_val != ref_val:
            assert False
    assert True


def test_writeData2File_fileExists(tmp_path):
    import rsgislib.tools.utils

    out_data = "CONTENT"
    out_file = os.path.join(tmp_path, "out_file.txt")
    rsgislib.tools.utils.writeData2File(out_data, out_file)
    assert os.path.exists(out_file)


def test_writeData2File_str_chkcontent(tmp_path):
    import rsgislib.tools.utils

    out_data = "CONTENT"
    out_file = os.path.join(tmp_path, "out_file.txt")
    rsgislib.tools.utils.writeData2File(out_data, out_file)
    in_data = rsgislib.tools.utils.readTextFileNoNewLines(out_file)
    assert in_data == out_data


def test_writeData2File_int_chkcontent(tmp_path):
    import rsgislib.tools.utils

    out_data = 100
    out_file = os.path.join(tmp_path, "out_file.txt")
    rsgislib.tools.utils.writeData2File(out_data, out_file)
    in_data = rsgislib.tools.utils.readTextFileNoNewLines(out_file)
    assert int(in_data) == out_data


def test_readJSON2Dict_basic():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_DATA_DIR, "basic_dict.json")
    out_data = dict()
    out_data["Hello"] = "World"
    out_data["ten"] = 10
    in_data = rsgislib.tools.utils.readJSON2Dict(input_file)
    assert in_data == out_data


def test_writeDict2JSON_basic(tmp_path):
    import rsgislib.tools.utils

    out_data = dict()
    out_data["Hello"] = "World"
    out_data["ten"] = 10
    out_file = os.path.join(tmp_path, "out_file.json")
    rsgislib.tools.utils.writeDict2JSON(out_data, out_file)
    in_data = rsgislib.tools.utils.readJSON2Dict(out_file)
    assert in_data == out_data


def test_inBounds_sgl_num():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.inBounds(5, 1, 10, upper_strict=False)


def test_inBounds_sgl_num_fail():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.inBounds(15, 1, 10, upper_strict=False)


def test_inBounds_sgl_num_strict_false():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.inBounds(10, 1, 10, upper_strict=False)


def test_inBounds_sgl_num_strict_true():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.inBounds(10, 1, 10, upper_strict=True)


def test_inBounds_arr():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([2, 3, 4, 7, 8])
    assert rsgislib.tools.utils.inBounds(arr, 1, 10, upper_strict=False)


def test_inBounds_arr_fail():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([12, 23, 34, 47, 68])
    assert not rsgislib.tools.utils.inBounds(arr, 1, 10, upper_strict=False)


def test_inBounds_arr_strict_false():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([2, 3, 4, 7, 8, 10])
    assert rsgislib.tools.utils.inBounds(arr, 1, 10, upper_strict=False)


def test_inBounds_arr_strict_true():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([2, 3, 4, 7, 8, 10])
    assert not rsgislib.tools.utils.inBounds(arr, 1, 10, upper_strict=True)


def test_mixedSigns_sgl_num_pos():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.mixedSigns(5)


def test_mixedSigns_sgl_num_neg():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.mixedSigns(-5)


def test_mixedSigns_arr():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([-1, 1, -2, 2])
    assert rsgislib.tools.utils.mixedSigns(arr)


def test_mixedSigns_arr_fail():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([1, 1, 2, 2])
    assert not rsgislib.tools.utils.mixedSigns(arr)


def test_negative_sgl_num_pos():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.negative(5)


def test_negative_sgl_num_neg():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.negative(-5)


def test_negative_arr_mix():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([-1, 1, -2, 2])
    assert not rsgislib.tools.utils.negative(arr)


def test_negative_arr_pos():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([1, 1, 2, 2])
    assert not rsgislib.tools.utils.negative(arr)


def test_negative_arr_neg():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([-1, -1, -2, -2])
    assert rsgislib.tools.utils.negative(arr)


def test_isOdd_num_even():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.isOdd(2)


def test_isOdd_num_odd():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.isOdd(5)


def test_isOdd_str():
    import rsgislib.tools.utils

    with pytest.raises(TypeError):
        assert rsgislib.tools.utils.isOdd("5")


def test_removeRepeatedChars_two():
    import rsgislib.tools.utils

    in_str = "Hello__World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.removeRepeatedChars(in_str, repeat_char="_")
    assert ref_str == out_str


def test_removeRepeatedChars_three():
    import rsgislib.tools.utils

    in_str = "Hello___World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.removeRepeatedChars(in_str, repeat_char="_")
    assert ref_str == out_str


def test_removeRepeatedChars_four():
    import rsgislib.tools.utils

    in_str = "Hello____World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.removeRepeatedChars(in_str, repeat_char="_")
    assert ref_str == out_str


def test_checkStr_nochng():
    import rsgislib.tools.utils

    in_str = "Hello_World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.checkStr(
        in_str, rm_non_ascii=False, rm_dashs=False, rm_spaces=False, rm_punc=False
    )
    assert ref_str == out_str


def test_checkStr_rmDashs():
    import rsgislib.tools.utils

    in_str = "Hello-World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.checkStr(
        in_str, rm_non_ascii=False, rm_dashs=True, rm_spaces=False, rm_punc=False
    )
    assert ref_str == out_str


def test_checkStr_rmSpaces():
    import rsgislib.tools.utils

    in_str = "Hello World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.checkStr(
        in_str, rm_non_ascii=False, rm_dashs=False, rm_spaces=True, rm_punc=False
    )
    assert ref_str == out_str


def test_checkStr_rmPunc():
    import rsgislib.tools.utils

    in_str = "Hello_World!"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.checkStr(
        in_str, rm_non_ascii=False, rm_dashs=False, rm_spaces=False, rm_punc=True
    )
    assert ref_str == out_str


def test_checkStr_rmNonASCII():
    import rsgislib.tools.utils

    in_str = "Hello_World£"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.checkStr(
        in_str, rm_non_ascii=True, rm_dashs=False, rm_spaces=False, rm_punc=False
    )
    assert ref_str == out_str


def test_getDaysSince():
    import datetime
    import rsgislib.tools.utils

    base_date = datetime.date(2000, 1, 1)
    year = 2000
    day_of_year = 195
    n_days = rsgislib.tools.utils.getDaysSince(year, day_of_year, base_date)
    assert n_days == 194


def test_getDaysSinceDate():
    import datetime
    import rsgislib.tools.utils

    base_date = datetime.date(2000, 1, 1)
    year = 2000
    month = 4
    day = 15
    n_days = rsgislib.tools.utils.getDaysSinceDate(year, month, day, base_date)
    assert n_days == 105
