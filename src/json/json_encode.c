//
// json_encode.c
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

#include <stdlib.h>
#include <string.h>

const char* JSON_TRUE = "true";
const char* JSON_FALSE = "false";
const char* JSON_NULL = "null";


void _init_json_buffer(JSONSerializationBuffer* buf)
{
    buf->index = 0;
    buf->size = INIT_BUF_SIZE;
    buf->data = (char*) malloc(INIT_BUF_SIZE);
}

// write byte array to the output buffer
//
// buf: the JSONSerializationBuffer to write to
// str: the byte array to copy characters from
// n:   the number of bytes to copy
void _buf_append(JSONSerializationBuffer* buf, const char* str, int n)
{
    if (buf->index + n >= buf->size)
    {
        // grow buffer as necessary
        int new_size = buf->size * 2;
        while (buf->index + n >= new_size - 1)
        {
            new_size *= 2;
        }

        buf->data = (char*) realloc(buf->data, new_size);
        buf->size = new_size;
    }

    // write data to buffer
    memcpy(buf->data + buf->index, str, n);
    buf->index += n;
}

// serialize a Python object to JSON
void _serialize_object(JSONSerializationBuffer* buf, PyObject* object)
{
    Py_XINCREF(object);

    if (PyDict_Check(object))
    {
        // serialize dict
        _buf_append(buf, "{", 1);

        PyObject *key, *value;
        Py_ssize_t pos = 0;
        Py_ssize_t size = PyDict_Size(object);

        while (PyDict_Next(object, &pos, &key, &value))
        {
            // serialize key
            if (!PyUnicode_Check(key))
            {
                // ignore elements with non-string keys
                continue;
            }
            _serialize_object(buf, key);

            // key:value colon separator
            _buf_append(buf, ":", 1);

            // serialize value
            _serialize_object(buf, value);

            // add comma separators
            if (pos < size)
            {
                _buf_append(buf, ",", 1);
            }
        }

        _buf_append(buf, "}", 1);
    }
    else if (PyList_Check(object))
    {
        // serialize list
        _buf_append(buf, "[", 1);

        const Py_ssize_t len = PyList_Size(object);
        for (Py_ssize_t i = 0; i < len; i++)
        {
            // serialize list item
            PyObject* list_item = PyList_GetItem(object, i);
            _serialize_object(buf, list_item);

            // add comma separators
            if (i < len - 1) {
                _buf_append(buf, ",", 1);
            }
        }

        _buf_append(buf, "]", 1);
    }
    else if (PyUnicode_Check(object))
    {
        // serialize string
        _buf_append(buf, "\"", 1);

        // convert PyUnicode object to UTF-8 encoded byte array,
        // and write it to the output JSON buffer
        Py_ssize_t str_size;
        const char* utf8_str = PyUnicode_AsUTF8AndSize(object, &str_size);
        if (utf8_str)
        {
            _buf_append(buf, utf8_str, str_size);
        }

        _buf_append(buf, "\"", 1);
    }
    else if (PyBool_Check(object))
    {
        // serialize boolean
        if (object == Py_True)
        {
            _buf_append(buf, JSON_TRUE, 4);
        }
        else
        {
            _buf_append(buf, JSON_FALSE, 5);
        }
    }
    else if (PyLong_Check(object))
    {
        // serialize integer
        //\TODO optimize this to one string write
        // currently uses two string writes, to create the string and
        // then to write it to the output buffer
        char integer_string[32];
        long long value = PyLong_AsLongLong(object);
        int n = snprintf(integer_string, 32, "%lld", value);
        _buf_append(buf, integer_string, n);
    }
    else if (PyFloat_Check(object))
    {
        // serialize float
        //\TODO optimize this to one string write
        // currently uses two string writes, to create the string and
        // then to write it to the output buffer
        char float_string[32];
        double value = PyFloat_AsDouble(object);
        int n = snprintf(float_string, 32, "%f", value);
        _buf_append(buf, float_string, n);
    }
    else if (object == Py_None)
    {
        // serialize None -> null
        _buf_append(buf, JSON_NULL, 4);
    }

    Py_XDECREF(object);
}

// recursively serialize a Python object to JSON
// dictionary keys must be strings, per JSON
// non-string dict keys will be ignored
//
// returns a dynamically-allocated byte array containing
// the encoded JSON
char* json_encode(PyObject* object)
{
    // initialize our JSON string buffer structure
    JSONSerializationBuffer buf;
    _init_json_buffer(&buf);

    // perform recursive serialization
    _serialize_object(&buf, object);

    // append null string terminator to encoded JSON string
    buf.data[buf.index] = 0;
    return buf.data;
}
