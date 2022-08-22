//
// server.h
//
// (c) 2022, Joshua Moore
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

#ifndef __SERVER_H
#define __SERVER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>


// main function called from Python
// this binds the HTTP server to a user-specified address and port
// and handles incoming requests
PyObject* ludicrous_runserver(PyObject* self, PyObject* args);

#endif
