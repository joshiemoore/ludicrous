//
// json.h
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

#ifndef __JSON_H
#define __JSON_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define INIT_BUF_SIZE 32


typedef struct _json_ser_buf {
    int index;
    int size;
    char* data;
} JSONSerializationBuffer;

// convert a Python dict object to a JSON C string
// caller is responsible for freeing dynamically-allocated string
char* json_encode(PyObject* json_dict);

// convert a JSON C string to a Python dict object
// caller is responsible for decref'ing the resulting object
PyObject* json_decode(char* json_string);

#endif
