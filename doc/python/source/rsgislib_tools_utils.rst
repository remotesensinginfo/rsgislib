RSGISLib Utility Tools
=================================

Unique Value
---------------
.. autofunction:: rsgislib.tools.utils.uid_generator


Text File I/O
---------------
.. autofunction:: rsgislib.tools.utils.read_text_file
.. autofunction:: rsgislib.tools.utils.read_text_file_no_new_lines
.. autofunction:: rsgislib.tools.utils.read_text_file_to_list
.. autofunction:: rsgislib.tools.utils.write_list_to_file
.. autofunction:: rsgislib.tools.utils.write_data_to_file
.. autofunction:: rsgislib.tools.utils.create_ascii_text_file
.. autofunction:: rsgislib.tools.utils.prettify_xml_file


JSON File I/O
---------------
.. autofunction:: rsgislib.tools.utils.write_dict_to_json
.. autofunction:: rsgislib.tools.utils.write_dict_to_json_gz
.. autofunction:: rsgislib.tools.utils.read_json_to_dict
.. autofunction:: rsgislib.tools.utils.read_gz_json_to_dict


Parse Dict/JSON
-------------------
.. autofunction:: rsgislib.tools.utils.dict_struct_does_path_exist
.. autofunction:: rsgislib.tools.utils.dict_struct_get_str_value
.. autofunction:: rsgislib.tools.utils.dict_struct_get_boolean_value
.. autofunction:: rsgislib.tools.utils.dict_struct_get_date_value
.. autofunction:: rsgislib.tools.utils.dict_struct_get_datetime_value
.. autofunction:: rsgislib.tools.utils.dict_struct_get_str_list_value
.. autofunction:: rsgislib.tools.utils.dict_struct_get_numeric_value
.. autofunction:: rsgislib.tools.utils.dict_struct_get_list_value


Strings
---------------
.. autofunction:: rsgislib.tools.utils.zero_pad_num_str
.. autofunction:: rsgislib.tools.utils.remove_repeated_chars
.. autofunction:: rsgislib.tools.utils.check_str

Encode String
--------------
.. autofunction:: rsgislib.tools.utils.encode_base64_text
.. autofunction:: rsgislib.tools.utils.decode_base64_text
.. autofunction:: rsgislib.tools.utils.create_username_password_file
.. autofunction:: rsgislib.tools.utils.get_username_password



Numeric
---------
.. autofunction:: rsgislib.tools.utils.is_number
.. autofunction:: rsgislib.tools.utils.in_bounds
.. autofunction:: rsgislib.tools.utils.mixed_signs
.. autofunction:: rsgislib.tools.utils.negative
.. autofunction:: rsgislib.tools.utils.is_odd

Colours
---------
.. autofunction:: rsgislib.tools.utils.hex_to_rgb
.. autofunction:: rsgislib.tools.utils.rgb_to_hex

Dates
-------
.. autofunction:: rsgislib.tools.utils.get_days_since
.. autofunction:: rsgislib.tools.utils.get_days_since_date
.. autofunction:: rsgislib.tools.utils.find_month_end_date
.. autofunction:: rsgislib.tools.utils.is_summer_winter
.. autofunction:: rsgislib.tools.utils.create_year_month_start_end_lst
.. autofunction:: rsgislib.tools.utils.create_year_month_n_months_lst

Powerset
----------
.. autofunction:: rsgislib.tools.utils.powerset_iter
.. autofunction:: rsgislib.tools.utils.powerset_lst
.. autofunction:: rsgislib.tools.utils.create_var_list


Compute Environment
--------------------

.. autofunction:: rsgislib.tools.utils.get_environment_variable
.. autofunction:: rsgislib.tools.utils.num_process_cores


* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
