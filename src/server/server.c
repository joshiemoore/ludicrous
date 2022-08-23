//
// server.c
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
#include "server.h"
#include "mongoose.h"

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

char  SERVER_HOST[256];
long  SERVER_PORT = 8000;
char  BIND_STRING[300];


// URL paths are stored in a binary tree structure
typedef struct _urlnode {
    char* endpoint;
    PyObject* callback;
    struct _urlnode* left;
    struct _urlnode* right;
} URLNode;

// the base of the URL tree, the root endpoint
// this gets returned when a request is made to
// "/", and is also the parent of all other URL nodes
static URLNode* ROOT_NODE;

// create a new URL node
URLNode* url_create_node(char* endpoint, PyObject* callback)
{
    URLNode* node = malloc(sizeof(URLNode));

    node->endpoint = endpoint;
    node->callback = callback;
    node->left = NULL;
    node->right = NULL;

    return node;
}

// insert a URL into the URL tree
// root and node should not be null
// return 0 if the URL was inserted in the tree
// return 1 otherwise
int url_insert(URLNode* root, const char* url, PyObject* callback)
{
    printf("Adding route %s\n", url);

    if (!strcmp(url, "/"))
    {
        // root node, simply set the callback
        ROOT_NODE->callback = callback;
        return 0;
    }

    const int url_len = strlen(url);
    char* url_copy = (char*)malloc(url_len + 1);
    snprintf(url_copy, url_len + 1, "%s", url);

    char* tok = strtok(url_copy, "/");
    while (tok)
    {
        printf("%s\n", tok);
        tok = strtok(0, "/");
    }

    free(url_copy);
    return 0;
}

// find an endpoint in the tree based on a URL
// returns the endpoint's URLNode if the endpoint exists
// returns NULL if the endpoint does not exist
URLNode* url_find(URLNode* root, struct mg_str url)
{
    return NULL;
}

// Handle interrupts, like CTRL+C
static int s_signo;
static void signal_handler(int signo)
{
    s_signo = signo;
}

static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message*) ev_data;

        // match URI to route
        struct mg_str url = hm->uri;
        PyObject* endpoint_callback = NULL;

        if (!strcmp(url.ptr, "/"))
        {
            // special case, root node
            endpoint_callback = ROOT_NODE->callback;
        }
        else
        {
            URLNode* endpoint = url_find(ROOT_NODE, url);
            if (endpoint)
            {
                endpoint_callback = endpoint->callback;
            }
        }

        if (endpoint_callback)
        {
            // call route handler, then send encoded JSON back to client
        }
        else
        {
            // endpoint not found
            mg_http_reply(c, 404, "", "");
        }
    }
}

PyObject* ludicrous_server_run(PyObject* self, PyObject* args, PyObject* kwargs)
{
    printf("Preparing ship for ludicrous speed. . .\n");

    // parse arguments
    static char* keywords[] = {"host", "port", NULL};
    char* arg_host = NULL;
    long arg_port = 0;

    PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "|sI",
        keywords,
        &arg_host,
        &arg_port
    );

    if (arg_host)
    {
        snprintf(SERVER_HOST, 256, "%s", arg_host);
    }
    else
    {
        snprintf(SERVER_HOST, 10, "localhost");
    }

    if (arg_port)
    {
        SERVER_PORT = arg_port;
    }

    // join host and port into one string to bind to
    snprintf(BIND_STRING, 300, "%s:%ld", SERVER_HOST, SERVER_PORT);

    // intialize URL root node
    ROOT_NODE = url_create_node(NULL, NULL);

    // set up routing based on functions in global namespace
    PyObject* globals = PyEval_GetGlobals();
    Py_INCREF(globals);
    PyObject* global_key;
    PyObject* global_value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(globals, &pos, &global_key, &global_value))
    {
        Py_INCREF(global_value);
        if (PyCallable_Check(global_value))
        {
            PyObject* url = PyObject_GetAttr(global_value, PyUnicode_FromString("_LUDICROUS"));
            Py_XINCREF(url);
            if (url && PyUnicode_Check(url))
            {
                // add the endpoint and its request handler to the tree
                const char* url_str = PyUnicode_AsUTF8(url);
                url_insert(ROOT_NODE, url_str, global_value);
            }
            Py_XDECREF(url);
        }
        Py_DECREF(global_value);
    }
    Py_DECREF(globals);

    // initialize mongoose
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    struct mg_connection *c = mg_http_listen(&mgr, BIND_STRING, fn, NULL);

    if (c == NULL)
    {
        //\TODO set error message
        printf("Failed to bind on %s\n", BIND_STRING);
        return NULL;
    }

    printf("Server started on %s (CTRL+C to stop)\n", BIND_STRING);
    fflush(stdout);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // loop until interrupted, handle requests
    while(s_signo == 0) mg_mgr_poll(&mgr, 1000);

    Py_RETURN_NONE;
}
