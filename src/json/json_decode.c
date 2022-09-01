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


static PyObject* _decode_tokens(const char* json_string, jsmntok_t* toks)
{
    if (toks->type == JSMN_PRIMITIVE)
    {
        // JSMN_PRIMITIVE: True, False, None, number
        switch(*(json_string + toks->start))
        {
            case 't':
                Py_RETURN_TRUE;
            case 'f':
                Py_RETURN_FALSE;
            case 'n':
                Py_RETURN_NONE;
        }

        // we haven't returned yet, so this primitive is a number
        // TODO number parsing
    }
    else if (toks->type == JSMN_STRING)
    {
        // JSMN_STRING: str
        return PyUnicode_FromStringAndSize(
            json_string + toks->start,
            toks->end - toks->start
        );
    }
    else if (toks->type == JSMN_ARRAY)
    {
        // JSMN_ARRAY: list
        const int num_elements = toks->size;
        PyObject* result_list = PyList_New(num_elements);
        if (!result_list)
        {
            return NULL;
        }

        for (int i = 0; i < num_elements; i++)
        {
            PyObject* list_element = _decode_tokens(
                json_string,
                toks + i + 1
            );
            if (!list_element)
            {
                return NULL;
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
        return result_list;
    }
    else if (toks->type == JSMN_OBJECT)
    {
        // JSMN_OBJECT: dict
        // TODO
    }

    // invalid token type
    return NULL;
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
        return NULL;
    }

    // convert the parsed tokens to Python objects
    return _decode_tokens(json_string, toks);
}
