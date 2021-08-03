import pytest

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
    num_str = rsgislib.tools.utils.zero_pad_num_str(1, str_len=3, round_num=False, round_n_digts=0, integerise=False)
    assert num_str == '001'

def test_zero_pad_num_str_11_round_int():
    import rsgislib.tools.utils
    num_str = rsgislib.tools.utils.zero_pad_num_str(1.1, str_len=3, round_num=True, round_n_digts=0, integerise=True)
    assert num_str == '001'

def test_zero_pad_num_str_float_round():
    import rsgislib.tools.utils
    num_str = rsgislib.tools.utils.zero_pad_num_str(112.1354, str_len=8, round_num=True, round_n_digts=2, integerise=False)
    assert num_str == '00112.14'

def test_powerset_lst():
    import rsgislib.tools.utils
    set_vals = [1,2,3]
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
    set_vals = [1,2,3]
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
