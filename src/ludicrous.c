//
// ludicrous.c
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

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "server.h"


static PyMethodDef LudicrousServerMethods[] = {
    {"run", (PyCFunction)(void(*)(void))ludicrous_server_run,
        METH_VARARGS | METH_KEYWORDS, "run ludicrous speed JSON API server"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef ludicrous_server = {
    PyModuleDef_HEAD_INIT,
    "ludicrous",
    NULL,
    -1,
    LudicrousServerMethods
};

PyMODINIT_FUNC PyInit_server(void)
{
    return PyModule_Create(&ludicrous_server);
}
