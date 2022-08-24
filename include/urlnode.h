//
// urlnode.h
//
// Copyright (c) 2022, Joshua Moore
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

#include "mongoose.h"


// URL paths are stored in a tree structure
typedef struct _urlnode {
    // name of the URL node
    // not actually an "endpoint" unless it's a leaf node,
    // but I couldn't think of a better name
    char* endpoint;

    // the python function that will be called when this
    // endpoint is accessed
    // if callback is NULL, this endpoint will return a
    // 404 when it is accessed directly
    PyObject* callback;

    // structure for storing this node's children
    struct _urlnode** children;
    int children_count;
    int children_size;
} URLNode;

// create a new URL node
// return a pointer to the node on success
// return NULL on failure
URLNode* url_create_node(char* endpoint, PyObject* callback);

// delete a URL node and its children
void url_delete_node(URLNode* node);

// insert a URL into the URL tree
// root and node should not be null
// "root" should be the actual root node of the tree
// return 1 on success, 0 on failure
int url_insert(URLNode* root, const char* url, PyObject* callback);

// find an endpoint in the tree based on a URL
// returns a pointer to the endpoint's URLNode if the endpoint exists
// returns NULL if the endpoint does not exist
URLNode* url_find(URLNode* root, struct mg_str url);
