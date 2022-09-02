//
// json_decode.c
//
// Copyright (c) 2022 Joshua Moore
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "json.h"
#include "jsmn.h"


static int _decode_tokens(const char* json_string, jsmntok_t* toks, PyObject** result)
{
    int total_decoded_toks = 0;

    if (toks->type == JSMN_PRIMITIVE)
    {
        // JSMN_PRIMITIVE: True, False, None, number
        double double_val;
        long long_val;
        switch(*(json_string + toks->start))
        {
            case 't':
                Py_INCREF(Py_True);
                *result = Py_True;
                break;
            case 'f':
                Py_INCREF(Py_False);
                *result = Py_False;
                break;
            case 'n':
                Py_INCREF(Py_None);
                *result = Py_None;
                break;
            default:
                // if it's not True, False, or None,
                // then it must be a number
                sscanf(json_string + toks->start, "%lf", &double_val);
                long_val = (long)double_val;
                if (long_val == double_val)
                {
                    // if the long cast is equal to the double, then it's an integer
                    *result = PyLong_FromLong(long_val);
                }
                else
                {
                    // otherwise, it's floating point
                    *result = PyFloat_FromDouble(double_val);
                }
                break;
        }

        return 1;
    }
    else if (toks->type == JSMN_STRING)
    {
        // JSMN_STRING: str
        *result = PyUnicode_FromStringAndSize(
            json_string + toks->start,
            toks->end - toks->start
        );
        return 1;
    }
    else if (toks->type == JSMN_ARRAY)
    {
        // JSMN_ARRAY: list
        const int num_elements = toks->size;
        PyObject* result_list = PyList_New(num_elements);
        if (!result_list)
        {
            return 0;
        }

        for (int i = 0; i < num_elements; i++)
        {
            PyObject* list_element = NULL;
            total_decoded_toks += _decode_tokens(
                json_string,
                toks + i + 1,
                &list_element
            );
            if (!list_element)
            {
                return 0;
            }

            // I will leave off the IndexError check because
            // we don't need it. We explicitly created a list
            // of size num_elements, so we will not write out
            // of bounds here in this loop.
            PyList_SetItem(
                result_list,
                i,
                list_element
            );
        }
        *result = result_list;
        return total_decoded_toks + 1;
    }
    else if (toks->type == JSMN_OBJECT)
    {
        // JSMN_OBJECT: dict
        const int num_elements = toks->size;
        PyObject* result_dict = PyDict_New();

        for (int i = 0; i < num_elements; i++)
        {
            PyObject* key = NULL;
            total_decoded_toks += _decode_tokens(
                json_string,
                toks + total_decoded_toks + 1,
                &key
            );
            if (!key)
            {
                return 0;
            }

            PyObject* value = NULL;
            total_decoded_toks += _decode_tokens(
                json_string,
                toks + total_decoded_toks + 1,
                &value
            );
            if (!value)
            {
                return 0;
            }

            PyDict_SetItem(result_dict, key, value);
        }
        *result = result_dict;
        return total_decoded_toks + 1;
    }
    return 0;
}

PyObject* json_decode(const char* json_string, int str_len)
{
    // initialize jsmn parser
    jsmn_parser parser;
    jsmn_init(&parser);

    // perform the actual parsing
    jsmntok_t toks[2048];
    int result = jsmn_parse(
        &parser,
        json_string,
        str_len,
        toks,
        sizeof(toks) / sizeof(toks[0])
    );
    if (result < 0)
    {
        // failed to parse JSON
        // TODO set error
        return NULL;
    }

    // decode the parsed tokens to Python objects
    PyObject* py_result = NULL;
    _decode_tokens(
        json_string,
        toks,
        &py_result
    );
    if (!py_result)
    {
        // failed to decode JSON
        // TODO set error
        return NULL;
    }
    return py_result;
}
