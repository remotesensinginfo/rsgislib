import os

import pytest

DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
TOOLS_UTILS_DATA_DIR = os.path.join(DATA_DIR, "tools", "utils")
IMGREG_DATA_DIR = os.path.join(DATA_DIR, "imageregistration")


def test_uid_generator_len4():
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator(4)
    assert len(uid_str) == 4


def test_uid_generator_len6():
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator(6)
    assert len(uid_str) == 6


def test_uid_generator_len8():
    import rsgislib.tools.utils

    uid_str = rsgislib.tools.utils.uid_generator(8)
    assert len(uid_str) == 8


def test_is_number_int_1():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number(1)


def test_is_number_int_100():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number(100)


def test_is_number_int_1str():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number("1")


def test_is_number_int_100str():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number("100")


def test_is_number_float_125():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number(1.25)


def test_is_number_float_10025():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number(100.25)


def test_is_number_float_str125():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number("1.25")


def test_is_number_float_str10025():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_number("100.25")


def test_is_number_str_Hello():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_number("Hello")


def test_is_number_str_World():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_number("World")


def test_is_number_str_1a():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_number("1a")


def test_is_number_str_100q():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_number("100?")


def test_is_number_str_1e():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_number("1!")


def test_is_number_str_10at():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_number("10@")


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


def test_read_text_file_no_new_lines_str():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str.txt")
    in_data = rsgislib.tools.utils.read_text_file_no_new_lines(input_file)
    assert in_data == "Hello"


def test_read_text_file_no_new_lines_str_mline():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str_mline.txt")
    in_data = rsgislib.tools.utils.read_text_file_no_new_lines(input_file)
    assert in_data == "HelloWorld"


def test_read_text_file_no_new_lines_int():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_int.txt")
    in_data = rsgislib.tools.utils.read_text_file_no_new_lines(input_file)
    assert int(in_data) == 100


def test_read_text_file_to_list_strs():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_str_mline.txt")
    in_data = rsgislib.tools.utils.read_text_file_to_list(input_file)
    ref_data = ["Hello", "World"]
    if len(in_data) != len(ref_data):
        assert False
    for data_val, ref_val in zip(in_data, ref_data):
        if data_val != ref_val:
            assert False
    assert True


def test_write_data_to_file_fileExists(tmp_path):
    import rsgislib.tools.utils

    out_data = "CONTENT"
    out_file = os.path.join(tmp_path, "out_file.txt")
    rsgislib.tools.utils.write_data_to_file(out_data, out_file)
    assert os.path.exists(out_file)


def test_write_data_to_file_str_chkcontent(tmp_path):
    import rsgislib.tools.utils

    out_data = "CONTENT"
    out_file = os.path.join(tmp_path, "out_file.txt")
    rsgislib.tools.utils.write_data_to_file(out_data, out_file)
    in_data = rsgislib.tools.utils.read_text_file_no_new_lines(out_file)
    assert in_data == out_data


def test_write_data_to_file_int_chkcontent(tmp_path):
    import rsgislib.tools.utils

    out_data = 100
    out_file = os.path.join(tmp_path, "out_file.txt")
    rsgislib.tools.utils.write_data_to_file(out_data, out_file)
    in_data = rsgislib.tools.utils.read_text_file_no_new_lines(out_file)
    assert int(in_data) == out_data


def test_read_json_to_dict_basic():
    import rsgislib.tools.utils

    input_file = os.path.join(TOOLS_UTILS_DATA_DIR, "basic_dict.json")
    out_data = dict()
    out_data["Hello"] = "World"
    out_data["ten"] = 10
    in_data = rsgislib.tools.utils.read_json_to_dict(input_file)
    assert in_data == out_data


def test_write_dict_to_json_basic(tmp_path):
    import rsgislib.tools.utils

    out_data = dict()
    out_data["Hello"] = "World"
    out_data["ten"] = 10
    out_file = os.path.join(tmp_path, "out_file.json")
    rsgislib.tools.utils.write_dict_to_json(out_data, out_file)
    in_data = rsgislib.tools.utils.read_json_to_dict(out_file)
    assert in_data == out_data


def test_in_bounds_sgl_num():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.in_bounds(5, 1, 10, upper_strict=False)


def test_in_bounds_sgl_num_fail():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.in_bounds(15, 1, 10, upper_strict=False)


def test_in_bounds_sgl_num_strict_false():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.in_bounds(10, 1, 10, upper_strict=False)


def test_in_bounds_sgl_num_strict_true():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.in_bounds(10, 1, 10, upper_strict=True)


def test_in_bounds_arr():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([2, 3, 4, 7, 8])
    assert rsgislib.tools.utils.in_bounds(arr, 1, 10, upper_strict=False)


def test_in_bounds_arr_fail():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([12, 23, 34, 47, 68])
    assert not rsgislib.tools.utils.in_bounds(arr, 1, 10, upper_strict=False)


def test_in_bounds_arr_strict_false():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([2, 3, 4, 7, 8, 10])
    assert rsgislib.tools.utils.in_bounds(arr, 1, 10, upper_strict=False)


def test_in_bounds_arr_strict_true():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([2, 3, 4, 7, 8, 10])
    assert not rsgislib.tools.utils.in_bounds(arr, 1, 10, upper_strict=True)


def test_mixed_signs_sgl_num_pos():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.mixed_signs(5)


def test_mixed_signs_sgl_num_neg():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.mixed_signs(-5)


def test_mixed_signs_arr():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([-1, 1, -2, 2])
    assert rsgislib.tools.utils.mixed_signs(arr)


def test_mixed_signs_arr_fail():
    import rsgislib.tools.utils
    import numpy

    arr = numpy.array([1, 1, 2, 2])
    assert not rsgislib.tools.utils.mixed_signs(arr)


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


def test_is_odd_num_even():
    import rsgislib.tools.utils

    assert not rsgislib.tools.utils.is_odd(2)


def test_is_odd_num_odd():
    import rsgislib.tools.utils

    assert rsgislib.tools.utils.is_odd(5)


def test_is_odd_str():
    import rsgislib.tools.utils

    with pytest.raises(TypeError):
        assert rsgislib.tools.utils.is_odd("5")


def test_remove_repeated_chars_two():
    import rsgislib.tools.utils

    in_str = "Hello__World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.remove_repeated_chars(in_str, repeat_char="_")
    assert ref_str == out_str


def test_remove_repeated_chars_three():
    import rsgislib.tools.utils

    in_str = "Hello___World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.remove_repeated_chars(in_str, repeat_char="_")
    assert ref_str == out_str


def test_remove_repeated_chars_four():
    import rsgislib.tools.utils

    in_str = "Hello____World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.remove_repeated_chars(in_str, repeat_char="_")
    assert ref_str == out_str


def test_check_str_nochng():
    import rsgislib.tools.utils

    in_str = "Hello_World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.check_str(
        in_str, rm_non_ascii=False, rm_dashs=False, rm_spaces=False, rm_punc=False
    )
    assert ref_str == out_str


def test_check_str_rmDashs():
    import rsgislib.tools.utils

    in_str = "Hello-World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.check_str(
        in_str, rm_non_ascii=False, rm_dashs=True, rm_spaces=False, rm_punc=False
    )
    assert ref_str == out_str


def test_check_str_rmSpaces():
    import rsgislib.tools.utils

    in_str = "Hello World"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.check_str(
        in_str, rm_non_ascii=False, rm_dashs=False, rm_spaces=True, rm_punc=False
    )
    assert ref_str == out_str


def test_check_str_rmPunc():
    import rsgislib.tools.utils

    in_str = "Hello_World!"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.check_str(
        in_str, rm_non_ascii=False, rm_dashs=False, rm_spaces=False, rm_punc=True
    )
    assert ref_str == out_str


def test_check_str_rmNonASCII():
    import rsgislib.tools.utils

    in_str = "Hello_World£"
    ref_str = "Hello_World"
    out_str = rsgislib.tools.utils.check_str(
        in_str, rm_non_ascii=True, rm_dashs=False, rm_spaces=False, rm_punc=False
    )
    assert ref_str == out_str


def test_get_days_since():
    import datetime
    import rsgislib.tools.utils

    base_date = datetime.date(2000, 1, 1)
    year = 2000
    day_of_year = 195
    n_days = rsgislib.tools.utils.get_days_since(year, day_of_year, base_date)
    assert n_days == 194


def test_get_days_since_date():
    import datetime
    import rsgislib.tools.utils

    base_date = datetime.date(2000, 1, 1)
    year = 2000
    month = 4
    day = 15
    n_days = rsgislib.tools.utils.get_days_since_date(year, month, day, base_date)
    assert n_days == 105
